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
	while (!IS_ROOT(tree, k) && IS_RED(PARENT(k))) {
		parent = PARENT(k);
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
	while (x && LEFT(x)) x = LEFT(x);
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
	if (IS_RED(parent))
		SET_RED(child);
	else
		SET_BLACK(child);
}

static void rbtree_remove_fixup(RbTree *tree, RbTreeNode *p, RbTreeNode *w,
				RbTreeNode *x) {
	while (!IS_ROOT(tree, x) && IS_BLACK(x)) {
		if (w == RIGHT(p)) {
			if (IS_RED(w)) {
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
			if (IS_RED(w)) {
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
	i32 do_fixup = IS_BLACK(node_to_delete);
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
		do_fixup = IS_BLACK(successor);
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

#endif /* RBTREE_IMPL_GUARD */
#endif /* RBTREE_IMPL */
