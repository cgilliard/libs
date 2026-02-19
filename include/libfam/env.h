/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 Christopher Gilliard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#ifndef _ENV_H
#define _ENV_H

#include <libfam/arena.h>
#include <libfam/types.h>

char *getenv(const char *name);
i32 init_environ(char **envp, Arena *a);

#ifdef TEST
#define IS_VALGRIND()                             \
	({                                        \
		bool _ret__;                      \
		char *_vg__ = getenv("VALGRIND"); \
		if (_vg__ && _vg__[0] == '1')     \
			_ret__ = true;            \
		else                              \
			_ret__ = false;           \
		_ret__;                           \
	})

#endif /* _ENV_H */

void _debug_insert_env(const char *key, char *value, Arena *a);
void _debug_remove_env(const char *key, char *value);
#endif /* TEST */

#ifdef ENV_IMPL
#ifndef ENV_IMPL_GUARD
#define ENV_IMPL_GUARD

#include <libfam/debug.h>
#include <libfam/errno.h>
#include <libfam/fmt.h>
#include <libfam/rbtree.h>
#include <libfam/string.h>
#include <libfam/syscall.h>
#include <libfam/utils.h>

RbTree __env_tree;
typedef struct {
	RbTreeNode reserved;
	const char *key;
	char *value;
	u64 key_len;
} EnvNode;

static i32 compare_env_keys(const EnvNode *v1, const EnvNode *v2) {
	i32 cmp;
	u64 min_len = v1->key_len < v2->key_len ? v1->key_len : v2->key_len;
	if ((cmp = strncmp(v1->key, v2->key, min_len))) return cmp;
	return v1->key_len < v2->key_len   ? -1
	       : v1->key_len > v2->key_len ? 1
					   : 0;
}

static i32 env_rbtree_search(RbTreeNode *cur, const RbTreeNode *value,
			     RbTreeNodePair *retval) {
	while (cur) {
		const EnvNode *v1 = ((const EnvNode *)cur);
		const EnvNode *v2 = ((const EnvNode *)value);
		i32 v = compare_env_keys(v1, v2);
		if (!v) {
			retval->self = cur;
			break;
		} else if (v < 0) {
			retval->parent = cur;
			retval->is_right = true;
			cur = cur->right;
		} else {
			retval->parent = cur;
			retval->is_right = false;
			cur = cur->left;
		}
		retval->self = cur;
	}
#ifdef TEST
	if (_debug_env_insert_fail) return -1;
#endif /* TEST */
	return 0;
}

PUBLIC char *getenv(const char *name) {
	RbTreeNodePair pair = {0};
	EnvNode node = {.key = name, .key_len = strlen(name)};
	env_rbtree_search(__env_tree.root, (const RbTreeNode *)&node, &pair);
	return pair.self ? (char *)((EnvNode *)pair.self)->value : NULL;
}

PUBLIC i32 init_environ(char **envp, Arena *a) {
	u64 i;
	RbTree *ptr;

	if (a->align < 8)
		panic("init_environ: alignment of arena must be at least 8!");
#ifdef TEST
	ptr = _debug_env_rbtree ? _debug_env_rbtree : &__env_tree;
#else
	ptr = &__env_tree;
#endif
	ptr->root = NULL;

	for (i = 0; envp[i]; i++) {
		i32 res;
		char *key, *value;
		char *itt = key = envp[i];
		u64 key_len = 0, value_len = 0;
		EnvNode *node;

		while (*itt && (*(itt++) != '=')) key_len++;
		value = *itt ? itt : NULL;
		node = arena_alloc(a, sizeof(EnvNode));
		if (!node) return -ENOMEM;
		node->key = key;
		node->key_len = key_len;
		node->value = value;
		res = rbtree_put(ptr, (RbTreeNode *)node, env_rbtree_search);
		if (res < 0) return res;
	}
	return 0;
}

/* GCOVR_EXCL_START */
#ifdef TEST
PUBLIC void _debug_insert_env(const char *key, char *value, Arena *a) {
	EnvNode *node = arena_alloc(a, sizeof(EnvNode));
	node->key = key;
	node->key_len = strlen(key);
	node->value = value;
	rbtree_put(&__env_tree, (RbTreeNode *)node, env_rbtree_search);
}
PUBLIC void _debug_remove_env(const char *key, char *value) {
	EnvNode node;
	node.key = key;
	node.key_len = strlen(key);
	node.value = value;
	rbtree_remove(&__env_tree, (RbTreeNode *)&node, env_rbtree_search);
}

#include <libfam/test.h>

Test(getenv) {
	char *shell = getenv("SHELL");
	ASSERT(shell);
	ASSERT(!strncmp(shell, "/bin", 4));
}

Test(test_special_cases) {
	Arena *a = NULL;
	char *envp[3];
	RbTree tree;

	arena_init(&a, 1024, 8);
	_debug_env_rbtree = &tree;
	envp[0] = "abc";
	envp[1] = NULL;
	ASSERT(!init_environ(envp, a), "empty value");
	_debug_env_rbtree = NULL;
	arena_destroy(a);

	arena_init(&a, 1024, 8);
	_debug_env_rbtree = &tree;
	_debug_env_insert_fail = true;
	envp[0] = "abc=123";
	envp[1] = NULL;
	ASSERT_EQ(init_environ(envp, a), -1, "insert_fail");
	_debug_env_insert_fail = false;
	_debug_env_rbtree = NULL;
	arena_destroy(a);

	arena_init(&a, 1024, 8);
	_debug_env_rbtree = &tree;
	envp[0] = "abc=";
	envp[1] = NULL;
	ASSERT(!init_environ(envp, a), "empty value 2");
	_debug_env_rbtree = NULL;
	arena_destroy(a);

	arena_init(&a, 8, 8);
	_debug_env_rbtree = &tree;
	envp[0] = "abc=def";
	envp[1] = NULL;
	ASSERT_EQ(init_environ(envp, a), -ENOMEM, "enomem");
	_debug_env_rbtree = NULL;
	arena_destroy(a);

	_debug_no_exit = true;
	_debug_no_write = true;

	arena_init(&a, 1024, 4);
	_debug_env_rbtree = &tree;
	envp[0] = "abc=";
	envp[1] = NULL;
	ASSERT(!init_environ(envp, a), "low align");
	_debug_env_rbtree = NULL;
	arena_destroy(a);

	_debug_no_write = false;
	_debug_no_exit = false;
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* ENV_IMPL */
#endif /* ENV_IMPL_GUARD */
