#include "tree.h"

#define		RB_RED       0
#define		RB_BLACK     1

#define rb_parent(r)   (( rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

static  void rb_set_parent(rb_node *rb, rb_node *p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}

static  void rb_set_color(rb_node *rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)        ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)        (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)        (rb_set_parent(node, node))

void rb_insert_color(struct rb_node *, struct rb_root *);
void rb_erase(struct rb_node *, struct rb_root *);

typedef void(*rb_augment_f)(struct rb_node *node, void *data);

void rb_augment_insert(struct rb_node *node, rb_augment_f func, void *data);
struct rb_node *rb_augment_erase_begin(struct rb_node *node);
void rb_augment_erase_end(struct rb_node *node, rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
struct rb_node *rb_next(const struct rb_node *);
struct rb_node *rb_prev(const struct rb_node *);
struct rb_node *rb_first(const struct rb_root *);
struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void rb_replace_node(struct rb_node *victim, struct rb_node *new, struct rb_root *root);

static  void rb_link_node(rb_node * node, rb_node * parent, rb_node ** rb_link)
{
	node->rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static void __rb_rotate_left(rb_node *node, rb_root *root)
{
	rb_node *right = node->rb_right;
	rb_node *parent = rb_parent(node);

	if ((node->rb_right = right->rb_left))
		rb_set_parent(right->rb_left, node);
	right->rb_left = node;

	rb_set_parent(right, parent);

	if (parent)
	{
		if (node == parent->rb_left)
			parent->rb_left = right;
		else
			parent->rb_right = right;
	}
	else
		root->rb_node = right;
	rb_set_parent(node, right);
}

static void __rb_rotate_right(rb_node *node, rb_root *root)
{
	rb_node *left = node->rb_left;
	rb_node *parent = rb_parent(node);

	if ((node->rb_left = left->rb_right))
		rb_set_parent(left->rb_right, node);
	left->rb_right = node;

	rb_set_parent(left, parent);

	if (parent)
	{
		if (node == parent->rb_right)
			parent->rb_right = left;
		else
			parent->rb_left = left;
	}
	else
		root->rb_node = left;
	rb_set_parent(node, left);
}

void rb_insert_color(rb_node *node, rb_root *root)
{
	rb_node *parent, *gparent;

	while ((parent = rb_parent(node)) && rb_is_red(parent))
	{
		gparent = rb_parent(parent);

		if (parent == gparent->rb_left)
		{
			{
				register rb_node *uncle = gparent->rb_right;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->rb_right == node)
			{
				register rb_node *tmp;
				__rb_rotate_left(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_right(gparent, root);
		}
		else {
			{
				register rb_node *uncle = gparent->rb_left;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->rb_left == node)
			{
				register rb_node *tmp;
				__rb_rotate_right(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_left(gparent, root);
		}
	}

	rb_set_black(root->rb_node);
}

static void __rb_erase_color(rb_node *node, rb_node *parent, rb_root *root)
{
	rb_node *other;

	while ((!node || rb_is_black(node)) && node != root->rb_node)
	{
		if (parent->rb_left == node)
		{
			other = parent->rb_right;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				__rb_rotate_left(parent, root);
				other = parent->rb_right;
			}
			if ((!other->rb_left || rb_is_black(other->rb_left)) &&
				(!other->rb_right || rb_is_black(other->rb_right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->rb_right || rb_is_black(other->rb_right))
				{
					rb_set_black(other->rb_left);
					rb_set_red(other);
					__rb_rotate_right(other, root);
					other = parent->rb_right;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->rb_right);
				__rb_rotate_left(parent, root);
				node = root->rb_node;
				break;
			}
		}
		else
		{
			other = parent->rb_left;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				__rb_rotate_right(parent, root);
				other = parent->rb_left;
			}
			if ((!other->rb_left || rb_is_black(other->rb_left)) &&
				(!other->rb_right || rb_is_black(other->rb_right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->rb_left || rb_is_black(other->rb_left))
				{
					rb_set_black(other->rb_right);
					rb_set_red(other);
					__rb_rotate_left(other, root);
					other = parent->rb_left;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->rb_left);
				__rb_rotate_right(parent, root);
				node = root->rb_node;
				break;
			}
		}
	}
	if (node)
		rb_set_black(node);
}

void rb_erase(rb_node *node, rb_root *root)
{
	rb_node *child, *parent;
	int color;

	if (!node->rb_left)
		child = node->rb_right;
	else if (!node->rb_right)
		child = node->rb_left;
	else
	{
		rb_node *old = node, *left;

		node = node->rb_right;
		while ((left = node->rb_left) != NULL)
			node = left;

		if (rb_parent(old)) {
			if (rb_parent(old)->rb_left == old)
				rb_parent(old)->rb_left = node;
			else
				rb_parent(old)->rb_right = node;
		}
		else
			root->rb_node = node;

		child = node->rb_right;
		parent = rb_parent(node);
		color = rb_color(node);

		if (parent == old) {
			parent = node;
		}
		else {
			if (child)
				rb_set_parent(child, parent);
			parent->rb_left = child;

			node->rb_right = old->rb_right;
			rb_set_parent(old->rb_right, node);
		}

		node->rb_parent_color = old->rb_parent_color;
		node->rb_left = old->rb_left;
		rb_set_parent(old->rb_left, node);
		goto color;
	}

	parent = rb_parent(node);
	color = rb_color(node);

	if (child)
		rb_set_parent(child, parent);
	if (parent)
	{
		if (parent->rb_left == node)
			parent->rb_left = child;
		else
			parent->rb_right = child;
	}
	else
		root->rb_node = child;

color:
	if (color == RB_BLACK)
		__rb_erase_color(child, parent, root);
}

static void rb_augment_path(rb_node *node, rb_augment_f func, void *data)
{
	rb_node *parent;

up:
	func(node, data);
	parent = rb_parent(node);
	if (!parent)
		return;

	if (node == parent->rb_left && parent->rb_right)
		func(parent->rb_right, data);
	else if (parent->rb_left)
		func(parent->rb_left, data);

	node = parent;
	goto up;
}

void rb_augment_insert(rb_node *node, rb_augment_f func, void *data)
{
	if (node->rb_left)
		node = node->rb_left;
	else if (node->rb_right)
		node = node->rb_right;

	rb_augment_path(node, func, data);
}

struct rb_node *rb_augment_erase_begin(rb_node *node)
{
	rb_node *deepest;

	if (!node->rb_right && !node->rb_left)
		deepest = rb_parent(node);
	else if (!node->rb_right)
		deepest = node->rb_left;
	else if (!node->rb_left)
		deepest = node->rb_right;
	else {
		deepest = rb_next(node);
		if (deepest->rb_right)
			deepest = deepest->rb_right;
		else if (rb_parent(deepest) != node)
			deepest = rb_parent(deepest);
	}

	return deepest;
}

void rb_augment_erase_end(struct rb_node *node, rb_augment_f func, void *data)
{
	if (node)
		rb_augment_path(node, func, data);
}

struct rb_node *rb_first(rb_root *root)
{
	rb_node  *n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_left)
		n = n->rb_left;
	return n;
}

struct rb_node *rb_last(rb_root *root)
{
	rb_node  *n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_right)
		n = n->rb_right;
	return n;
}

struct rb_node *rb_next(rb_node *node)
{
	rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	if (node->rb_right) {
		node = node->rb_right;
		while (node->rb_left)
			node = node->rb_left;
		return (struct rb_node *)node;
	}

	while ((parent = rb_parent(node)) && node == parent->rb_right)
		node = parent;

	return parent;
}

struct rb_node *rb_prev(rb_node *node)
{
	rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	if (node->rb_left) {
		node = node->rb_left;
		while (node->rb_right)
			node = node->rb_right;
		return (struct rb_node *)node;
	}

	while ((parent = rb_parent(node)) && node == parent->rb_left)
		node = parent;

	return parent;
}

void rb_replace_node(rb_node *victim, rb_node *new, rb_root *root)
{
	rb_node *parent = rb_parent(victim);

	if (parent) {
		if (victim == parent->rb_left)
			parent->rb_left = new;
		else
			parent->rb_right = new;
	}
	else {
		root->rb_node = new;
	}
	if (victim->rb_left)
		rb_set_parent(victim->rb_left, new);
	if (victim->rb_right)
		rb_set_parent(victim->rb_right, new);

	*new = *victim;
}

rb_root *rb_new(){
	rb_root* new = (rb_root *)malloc(sizeof(rb_root));
	if (new){
		memset(new, 0, sizeof(rb_root));
		return new;
	}
	return NULL;
}

BOOL rb_insert(key_t key, data_t data, rb_root *root){
	rb_node **new = &(root->rb_node);
	rb_node  *parent = 0;
	rb_node  *rbt = (rb_node *)malloc(sizeof(rb_node));
	memset(rbt, 0, sizeof(rb_node));
	rbt->data = data;
	rbt->key = key;
	/* Figure out where to put new node */
	while (*new)
	{
		rb_node *this = *new;

		int result = rbt->key - this->key;

		parent = *new;

		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else
			return FALSE; // the key is already exists
	}

	/* Add new node and rebalance tree. */
	rb_link_node(rbt, parent, new);
	rb_insert_color(rbt, root);
	return TRUE;
}

rb_node *rb_search(key_t key, rb_root *root){
	rb_node *node = root->rb_node;

	while (node)
	{
		rb_node *this = node;

		int result = key - this->key;

		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return this;
	}
	return NULL;
}

void rb_delete(key_t key, rb_root*root){
	struct rb_node *find = rb_search(key, root);
	if (!find)
		return;
	rb_erase(find, root);
	free(find);
	return;
}

void rb_free(rb_root*root){
	rb_delete(root, root->rb_node);
}

void rb_foreach_c(rb_node *node, TreeVisitFunc visitfunc) {
	if (node != NULL) {
		rb_foreach_c(node->rb_left, visitfunc);
		visitfunc(node->data);
		rb_foreach_c(node->rb_right, visitfunc);
	}
}
void rb_foreach(rb_root *root, TreeVisitFunc visitfunc) {
	rb_foreach_c(root->rb_node, visitfunc);
}

#if (defined DEBUG_BORDER) && (DEBUG_BORDER == 1)
void rb_print_c(rb_node *node){
	if (node != NULL){
		rb_print_c(node->rb_left);
		printf("	%p(%s)\n", node->key, (node->rb_parent_color == RB_RED) ? "红" : "黑");
		rb_print_c(node->rb_right);
	}
}
/*打印结点*/
void rb_print(rb_root *root){
	printf("Tree结构:[\r\n");
	rb_print_c(root->rb_node);
	printf("]\r\n");
}
#endif
/*
//查找最小最小结点
Node *rbMinKey(RBTree T){
Node *x = T;
Node *p = nil;
while (x != nil){
p = x;
x = x->left;
}
return p == nil ? NULL : p;
}

//查找最大结点
Node *rbMaxKey(RBTree T){
Node *x = T;
Node *p = nil;
while (x != nil){
p = x;
x = x->right;
}
return p == nil ? NULL : p;
}
*/