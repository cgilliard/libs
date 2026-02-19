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

#ifndef _RBTREE_H
#define _RBTREE_H

#include <libfam/types.h>

typedef struct RbTreeNode {
	struct RbTreeNode *parent_color;
	struct RbTreeNode *right;
	struct RbTreeNode *left;
} RbTreeNode;

typedef struct {
	RbTreeNode *parent;
	RbTreeNode *self;
	bool is_right;
} RbTreeNodePair;

typedef struct {
	RbTreeNode *root;
} RbTree;

static const RbTree RBTREE_INIT = {NULL};

typedef i32 (*RbTreeSearch)(RbTreeNode *base, const RbTreeNode *value,
			    RbTreeNodePair *retval);

i32 rbtree_put(RbTree *tree, RbTreeNode *value, const RbTreeSearch search);

RbTreeNode *rbtree_remove(RbTree *tree, RbTreeNode *value,
			  const RbTreeSearch search);

#endif /* _RBTREE_H */

#ifdef RBTREE_IMPL
#ifndef RBTREE_IMPL_GUARD
#define RBTREE_IMPL_GUARD

#include <libfam/errno.h>
#include <libfam/rbtree.h>
#include <libfam/utils.h>

#define RED_NODE 1
#define BLACK_NODE 0
#define SET_NODE(node, color, parent, rightv, leftv)                      \
	do {                                                              \
		node->parent_color = (RbTreeNode *)((u64)parent | color); \
		node->right = rightv;                                     \
		node->left = leftv;                                       \
	} while (0)
#define IS_RED(node) (node && ((u64)node->parent_color & 0x1))
#define IS_BLACK(node) !IS_RED(node)
#define SET_BLACK(node)       \
	(node->parent_color = \
	     (RbTreeNode *)((u64)node->parent_color & ~RED_NODE))
#define SET_RED(node)         \
	(node->parent_color = \
	     (RbTreeNode *)((u64)node->parent_color | RED_NODE))
#define SET_RIGHT(node, rightv) node->right = rightv
#define SET_LEFT(node, leftv) node->left = leftv
#define SET_PARENT(node, parentv) \
	(node->parent_color =     \
	     (RbTreeNode *)((u64)parentv + ((u64)node->parent_color & 0x1)))
#define PARENT(node) ((RbTreeNode *)((u64)node->parent_color & ~0x1))
#define RIGHT(node) node->right
#define LEFT(node) node->left
#define SET_ROOT(tree, node) tree->root = node
#define ROOT(tree) (tree->root)
#define IS_ROOT(tree, value) (tree->root == value)

static void rbtree_rotate_right(RbTree *tree, RbTreeNode *x) {
	RbTreeNode *y = LEFT(x);
	SET_LEFT(x, RIGHT(y));
	if (RIGHT(y)) SET_PARENT(RIGHT(y), x);
	SET_PARENT(y, PARENT(x));
	if (!PARENT(x))
		SET_ROOT(tree, y);
	else if (x == RIGHT(PARENT(x)))
		SET_RIGHT(PARENT(x), y);
	else
		SET_LEFT(PARENT(x), y);
	SET_RIGHT(y, x);
	SET_PARENT(x, y);
}

static void rbtree_rotate_left(RbTree *tree, RbTreeNode *x) {
	RbTreeNode *y = RIGHT(x);
	SET_RIGHT(x, LEFT(y));
	if (LEFT(y)) SET_PARENT(LEFT(y), x);
	SET_PARENT(y, PARENT(x));
	if (!PARENT(x))
		SET_ROOT(tree, y);
	else if (x == LEFT(PARENT(x)))
		SET_LEFT(PARENT(x), y);
	else
		SET_RIGHT(PARENT(x), y);
	SET_LEFT(y, x);
	SET_PARENT(x, y);
}

static void rbtree_insert_fixup(RbTree *tree, RbTreeNode *k) {
	RbTreeNode *parent, *gparent, *uncle;
	while (!IS_ROOT(tree, k)) {
		parent = PARENT(k);
		if (!((u64)parent->parent_color & 0x1)) break;
		gparent = PARENT(parent);
		if (parent == LEFT(gparent)) {
			uncle = RIGHT(gparent);
			if (IS_RED(uncle)) {
				SET_BLACK(parent);
				SET_BLACK(uncle);
				SET_RED(gparent);
				k = gparent;
			} else {
				if (k == RIGHT(parent)) {
					k = PARENT(k);
					rbtree_rotate_left(tree, k);
				}
				parent = PARENT(k);
				gparent = PARENT(parent);
				SET_BLACK(parent);
				SET_RED(gparent);
				rbtree_rotate_right(tree, gparent);
			}
		} else {
			uncle = LEFT(gparent);
			if (IS_RED(uncle)) {
				SET_BLACK(parent);
				SET_BLACK(uncle);
				SET_RED(gparent);
				k = gparent;
			} else {
				if (k == LEFT(parent)) {
					k = PARENT(k);
					rbtree_rotate_right(tree, k);
				}
				parent = PARENT(k);
				gparent = PARENT(parent);
				SET_BLACK(parent);
				SET_RED(gparent);
				rbtree_rotate_left(tree, gparent);
			}
		}
	}
	SET_BLACK(ROOT(tree));
}

static i32 rbtree_put_impl(RbTree *tree, RbTreeNodePair *pair,
			   RbTreeNode *value) {
	if (pair->self) {
		return -EDUPLICATE;
	} else if (pair->parent == 0) {
		SET_ROOT(tree, value);
		tree->root->parent_color = 0;
		tree->root->right = tree->root->left = 0;
	} else {
		SET_NODE(value, RED_NODE, pair->parent, 0, 0);
		if (pair->is_right)
			SET_RIGHT(pair->parent, value);
		else
			SET_LEFT(pair->parent, value);
	}
	return 0;
}

static RbTreeNode *rbtree_find_successor(RbTreeNode *x) {
	x = RIGHT(x);
	while (LEFT(x)) x = LEFT(x);
	return x;
}

static void rbtree_remove_transplant(RbTree *tree, RbTreeNode *dst,
				     RbTreeNode *src) {
	if (!PARENT(dst))
		SET_ROOT(tree, src);
	else if (dst == LEFT(PARENT(dst)))
		SET_LEFT(PARENT(dst), src);
	else
		SET_RIGHT(PARENT(dst), src);
	if (src) SET_PARENT(src, PARENT(dst));
}

static void rbtree_set_color(RbTreeNode *child, RbTreeNode *parent) {
	if (((u64)parent->parent_color & 0x1))
		SET_RED(child);
	else
		SET_BLACK(child);
}

static void rbtree_remove_fixup(RbTree *tree, RbTreeNode *p, RbTreeNode *w,
				RbTreeNode *x) {
	while (!IS_ROOT(tree, x) && IS_BLACK(x)) {
		if (w == RIGHT(p)) {
			if (((u64)w->parent_color & 0x1)) {
				SET_BLACK(w);
				SET_RED(p);
				rbtree_rotate_left(tree, p);
				w = RIGHT(p);
			}
			if (IS_BLACK(LEFT(w)) && IS_BLACK(RIGHT(w))) {
				SET_RED(w);
				x = p;
				if ((p = PARENT(p))) {
					RbTreeNode *pl = LEFT(p);
					if (x == pl)
						w = RIGHT(p);
					else
						w = pl;
				} else
					w = 0;
			} else {
				if (IS_BLACK(RIGHT(w))) {
					SET_BLACK(LEFT(w));
					SET_RED(w);
					rbtree_rotate_right(tree, w);
					w = RIGHT(p);
				}
				rbtree_set_color(w, p);
				SET_BLACK(p);
				SET_BLACK(RIGHT(w));
				rbtree_rotate_left(tree, p);
				x = ROOT(tree);
			}
		} else {
			if (((u64)w->parent_color & 0x1)) {
				SET_BLACK(w);
				SET_RED(p);
				rbtree_rotate_right(tree, p);
				w = LEFT(p);
			}
			if (IS_BLACK(RIGHT(w)) && IS_BLACK(LEFT(w))) {
				SET_RED(w);
				x = p;
				if ((p = PARENT(p))) {
					RbTreeNode *pl = LEFT(p);
					if (x == pl)
						w = RIGHT(p);
					else
						w = pl;
				} else
					w = 0;
			} else {
				if (IS_BLACK(LEFT(w))) {
					SET_BLACK(RIGHT(w));
					SET_RED(w);
					rbtree_rotate_left(tree, w);
					w = LEFT(p);
				}
				rbtree_set_color(w, p);
				SET_BLACK(p);
				SET_BLACK(LEFT(w));
				rbtree_rotate_right(tree, p);
				x = ROOT(tree);
			}
		}
	}

	SET_BLACK(x);
}

static void rbtree_remove_impl(RbTree *tree, RbTreeNodePair *pair) {
	RbTreeNode *node_to_delete = pair->self;
	i32 do_fixup = !((u64)node_to_delete->parent_color & 0x1);
	RbTreeNode *x = 0, *w = 0, *p = 0;

	if (LEFT(node_to_delete) == 0) {
		x = RIGHT(node_to_delete);
		rbtree_remove_transplant(tree, node_to_delete, x);
		p = PARENT(node_to_delete);
		if (p) {
			if (p->left == 0)
				w = RIGHT(p);
			else if (p)
				w = LEFT(p);
		} else {
			do_fixup = 0;
			if (ROOT(tree) != 0) SET_BLACK(ROOT(tree));
		}
	} else if (RIGHT(node_to_delete) == 0) {
		x = LEFT(node_to_delete);
		rbtree_remove_transplant(tree, node_to_delete,
					 LEFT(node_to_delete));
		p = PARENT(node_to_delete);
		if (p) w = LEFT(p);
	} else {
		RbTreeNode *successor = rbtree_find_successor(node_to_delete);
		do_fixup = !((u64)successor->parent_color & 0x1);
		x = RIGHT(successor);
		w = RIGHT(PARENT(successor));
		if (w) {
			if (PARENT(w) == node_to_delete) {
				w = LEFT(node_to_delete);
				p = successor;
			} else
				p = PARENT(w);
		}

		if (PARENT(successor) != node_to_delete) {
			rbtree_remove_transplant(tree, successor,
						 RIGHT(successor));
			SET_RIGHT(successor, RIGHT(node_to_delete));

			if (RIGHT(successor))
				SET_PARENT(RIGHT(successor), successor);
		}

		rbtree_remove_transplant(tree, node_to_delete, successor);
		SET_LEFT(successor, LEFT(node_to_delete));
		SET_PARENT(LEFT(successor), successor);
		rbtree_set_color(successor, node_to_delete);
	}

	if (do_fixup) rbtree_remove_fixup(tree, p, w, x);
}

PUBLIC i32 rbtree_put(RbTree *tree, RbTreeNode *value,
		      const RbTreeSearch search) {
	i32 result;
	RbTreeNodePair pair = {0};
	if ((result = search(ROOT(tree), value, &pair)) < 0) return result;
	if ((result = rbtree_put_impl(tree, &pair, value)) < 0) return result;
	rbtree_insert_fixup(tree, value);
	return 0;
}

PUBLIC RbTreeNode *rbtree_remove(RbTree *tree, RbTreeNode *value,
				 const RbTreeSearch search) {
	RbTreeNodePair pair = {0};
	if (search(ROOT(tree), value, &pair) < 0) return NULL;
	if (pair.self) rbtree_remove_impl(tree, &pair);
	return pair.self;
}

/* GCOVR_EXCL_START */
#ifdef TEST

#include <libfam/test.h>

typedef struct {
	RbTreeNode _reserved;
	u64 value;
} TestRbTreeNode;

i32 test_rbsearch(RbTreeNode *cur, const RbTreeNode *value,
		  RbTreeNodePair *retval);

i32 test_rbsearch(RbTreeNode *cur, const RbTreeNode *value,
		  RbTreeNodePair *retval) {
	while (cur) {
		u64 v1 = ((TestRbTreeNode *)cur)->value;
		u64 v2 = ((const TestRbTreeNode *)value)->value;
		if (v1 == v2) {
			retval->self = cur;
			break;
		} else if (v1 < v2) {
			retval->parent = cur;
			retval->is_right = 1;
			cur = cur->right;
		} else {
			retval->parent = cur;
			retval->is_right = 0;
			cur = cur->left;
		}
		retval->self = cur;
	}
	return 0;
}

static bool check_root_black(RbTree *tree) {
	if (!ROOT(tree)) return true;
	return IS_BLACK(ROOT(tree));
}

static bool check_no_consecutive_red(RbTreeNode *node) {
	if (!node) return true;

	if (IS_RED(node)) {
		if (RIGHT(node) && IS_RED(RIGHT(node))) return false;
		if (LEFT(node) && IS_RED(LEFT(node))) return false;
	}

	return check_no_consecutive_red(LEFT(node)) &&
	       check_no_consecutive_red(RIGHT(node));
}

static i32 check_black_height(RbTreeNode *node) {
	i32 left_height, right_height;
	if (!node) return 1;
	left_height = check_black_height(LEFT(node));
	right_height = check_black_height(RIGHT(node));

	if (left_height == -1 || right_height == -1) return -1;

	if (left_height != right_height) return -1;

	return left_height + (IS_BLACK(node) ? 1 : 0);
}

static void validate_rbtree(RbTree *tree) {
	ASSERT(check_root_black(tree), "Root must be black");
	ASSERT(check_no_consecutive_red(ROOT(tree)),
	       "No consecutive red nodes");
	ASSERT(check_black_height(ROOT(tree)) != -1,
	       "Inconsistent black height");
}

Test(rbtree1) {
	RbTree tree = RBTREE_INIT;
	TestRbTreeNode v1 = {{0}, 1};
	TestRbTreeNode v2 = {{0}, 2};
	TestRbTreeNode v3 = {{0}, 3};
	TestRbTreeNode v4 = {{0}, 0};
	TestRbTreeNode vx = {{0}, 3};
	TestRbTreeNode vy = {{0}, 0};
	RbTreeNodePair retval = {0};

	rbtree_put(&tree, (RbTreeNode *)&v1, test_rbsearch);
	validate_rbtree(&tree);
	rbtree_put(&tree, (RbTreeNode *)&v2, test_rbsearch);
	validate_rbtree(&tree);

	test_rbsearch(tree.root, (RbTreeNode *)&v1, &retval);
	ASSERT_EQ(((TestRbTreeNode *)retval.self)->value, 1, "value=1");

	test_rbsearch(tree.root, (RbTreeNode *)&v2, &retval);
	ASSERT_EQ(((TestRbTreeNode *)retval.self)->value, 2, "value=2");
	test_rbsearch(tree.root, (RbTreeNode *)&v3, &retval);
	ASSERT_EQ(retval.self, NULL, "self=NULL");

	rbtree_remove(&tree, (RbTreeNode *)&v2, test_rbsearch);
	validate_rbtree(&tree);
	test_rbsearch(tree.root, (RbTreeNode *)&v2, &retval);
	ASSERT_EQ(retval.self, NULL, "retval=NULL2");
	rbtree_put(&tree, (RbTreeNode *)&v3, test_rbsearch);
	validate_rbtree(&tree);
	rbtree_put(&tree, (RbTreeNode *)&v4, test_rbsearch);
	validate_rbtree(&tree);

	ASSERT_EQ(rbtree_put(&tree, (RbTreeNode *)&vx, test_rbsearch),
		  -EDUPLICATE, "duplicate");
	validate_rbtree(&tree);

	ASSERT_EQ(rbtree_put(&tree, (RbTreeNode *)&vy, test_rbsearch),
		  -EDUPLICATE, "duplicate2");
	validate_rbtree(&tree);
}

#define SIZE 100

Test(rbtree2) {
	u64 size, i;
	u64 next = 101;

	for (size = 1; size < SIZE; size++) {
		RbTree tree = RBTREE_INIT;
		TestRbTreeNode values[SIZE];
		for (i = 0; i < size; i++) {
			values[i].value = (next++ * 37) % 1001;
			rbtree_put(&tree, (RbTreeNode *)&values[i],
				   test_rbsearch);
			validate_rbtree(&tree);
		}

		for (i = 0; i < size; i++) {
			RbTreeNodePair retval = {0};
			TestRbTreeNode v = {{0}, 0};
			v.value = values[i].value;

			test_rbsearch(tree.root, (RbTreeNode *)&v, &retval);
			ASSERT(retval.self != NULL, "retval=NULL");
			ASSERT_EQ(((TestRbTreeNode *)retval.self)->value,
				  values[i].value, "value=values[i].value");
		}

		for (i = 0; i < size; i++) {
			TestRbTreeNode v = {{0}, 0};
			v.value = values[i].value;
			rbtree_remove(&tree, (RbTreeNode *)&v, test_rbsearch);
			validate_rbtree(&tree);
		}
		ASSERT_EQ(tree.root, NULL, "root=NULL");
		validate_rbtree(&tree);
	}
}

i32 test_rbsearch_rej(RbTreeNode *cur, const RbTreeNode *value,
		      RbTreeNodePair *retval);

i32 test_rbsearch_rej(RbTreeNode *cur, const RbTreeNode *value,
		      RbTreeNodePair *retval) {
	(void)cur;
	(void)value;
	(void)retval;
	return -1;
}

Test(rbtree3) {
	RbTree tree = RBTREE_INIT;
	TestRbTreeNode n1 = {.value = 1};
	rbtree_put(&tree, (RbTreeNode *)&n1, test_rbsearch);
	ASSERT(rbtree_remove(&tree, (RbTreeNode *)&n1, test_rbsearch), "rem");
	ASSERT(!rbtree_remove(&tree, (RbTreeNode *)&n1, test_rbsearch),
	       "already rem");
	ASSERT(!rbtree_remove(&tree, (RbTreeNode *)&n1, test_rbsearch_rej),
	       "rem err");
}

Test(rbtree4) {
#define COUNT 100000
	RbTree tree = RBTREE_INIT;
	TestRbTreeNode *nodes;
	u64 i;
	u64 seed = cycle_counter();

	nodes = map(sizeof(TestRbTreeNode) * COUNT * 2);

	for (i = 0; i < COUNT * 2; i++) {
		u64 value = splitmix64(&seed);
		nodes[i].value = value;
	}

	for (i = 0; i < COUNT; i++) {
		ASSERT(
		    !rbtree_put(&tree, (RbTreeNode *)&nodes[i], test_rbsearch),
		    "rbtree put {}", i);
	}
	validate_rbtree(&tree);
	ASSERT(tree.root, "not empty");

	for (i = COUNT / 2; i < COUNT; i++) {
		ASSERT(rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				     test_rbsearch),
		       "found {}", i);
		ASSERT(!rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				      test_rbsearch),
		       "found {}", i);
	}
	validate_rbtree(&tree);
	ASSERT(tree.root, "not empty");

	for (i = COUNT; i < COUNT * 2; i++) {
		ASSERT(
		    !rbtree_put(&tree, (RbTreeNode *)&nodes[i], test_rbsearch),
		    "rbtree put {}", i);
	}
	validate_rbtree(&tree);
	ASSERT(tree.root, "not empty");

	for (i = 0; i < COUNT / 2; i++) {
		ASSERT(rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				     test_rbsearch),
		       "found {}", i);
		ASSERT(!rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				      test_rbsearch),
		       "found {}", i);
	}
	validate_rbtree(&tree);
	ASSERT(tree.root, "not empty");

	for (i = COUNT * 2 - 1; i >= COUNT; i--) {
		ASSERT(rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				     test_rbsearch),
		       "found {}", i);
		ASSERT(!rbtree_remove(&tree, (RbTreeNode *)&nodes[i],
				      test_rbsearch),
		       "found {}", i);
	}

	validate_rbtree(&tree);
	ASSERT(!tree.root, "empty");

	munmap(nodes, sizeof(TestRbTreeNode) * COUNT * 2);
#undef COUNT
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* RBTREE_IMPL_GUARD */
#endif /* RBTREE_IMPL */
