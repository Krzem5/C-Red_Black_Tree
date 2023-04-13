#ifndef __RED_BLACK_TREE_H__
#define __RED_BLACK_TREE_H__ 1
#include <stdint.h>



#define COLOR_BLACK 0
#define COLOR_RED 1

#define RB_NODE_GET_LEFT(n) ((rb_node_t*)((n)->l&0xfffffffffffffffe))
#define RB_NODE_GET_COLOR(n) ((n)->l&1)
#define RB_NODE_SET_LEFT(n,v) ((n)->l=((n)->l&1)|((uint64_t)(v)))
#define RB_NODE_SET_COLOR(n,c) ((n)->l=((n)->l&0xfffffffffffffffe)|(c))



typedef struct __RB_NODE{
	struct __RB_NODE* p;
	uint64_t l;
	struct __RB_NODE* r;
	uint64_t v;
} rb_node_t;



typedef struct __RB_TREE{
	rb_node_t* r;
	rb_node_t _n;
} rb_tree_t;



void rb_init_tree(rb_tree_t* t);



rb_node_t* rb_insert_node(rb_tree_t* t,uint64_t v);



void rb_delete_node(rb_tree_t* t,rb_node_t* n);



void rb_print_tree(rb_tree_t* t);



void rb_free_tree(rb_tree_t* t);



#endif
