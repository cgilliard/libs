#ifndef _ENV_H
#define _ENV_H

#include <libfam/arena.h>
#include <libfam/types.h>

char *getenv(const char *name);
i32 init_environ(char **envp, Arena *a);

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

#ifdef ENV_IMPL
#ifndef ENV_IMPL_GUARD
#define ENV_IMPL_GUARD

#include <libfam/errno.h>
#include <libfam/rbtree.h>
#include <libfam/string.h>
#include <libfam/syscall.h>
#include <libfam/utils.h>

RbTree __env_tree = RBTREE_INIT;
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
	for (i = 0; envp[i]; i++) {
		i32 res;
		char *key, *value;
		char *itt = key = envp[i];
		u64 key_len = 0, value_len = 0;
		EnvNode *node;

		while (*itt && (*(itt++) != '=')) key_len++;
		value = itt ? itt + 1 : NULL;
		node = arena_alloc(a, sizeof(EnvNode));
		if (!node) return -ENOMEM;
		node->key = key;
		node->key_len = key_len;
		node->value = value;
		res = rbtree_put(&__env_tree, (RbTreeNode *)node,
				 env_rbtree_search);
		if (res < 0) return res;
	}
	return 0;
}

#ifdef TEST

Test(getenv) {
	char *shell = getenv("SHELL");
	ASSERT(shell);
	ASSERT(!strncmp(shell, "bin", 3));
}

#endif /* TEST */

#endif /* ENV_IMPL */
#endif /* ENV_IMPL_GUARD */
