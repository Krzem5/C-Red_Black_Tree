#include <red_black_tree.h>
#include <inttypes.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



#define _ASSERT_STR_(l) #l
#define _ASSERT_STR(l) _ASSERT_STR_(l)
#define ASSERT(x) \
	do{ \
		if (!(x)){ \
			printf("File \"%s\", Line %u (%s): %s: Assertion Failed\n",__FILE__,__LINE__,__func__,_ASSERT_STR(x)); \
			raise(SIGABRT); \
		} \
	} while (0)



static void _print_node(rb_node_t* n,rb_node_t* e,uint16_t i){
	if (RB_NODE_GET_LEFT(n)!=e){
		ASSERT(RB_NODE_GET_LEFT(n)->p==n);
		_print_node(RB_NODE_GET_LEFT(n),e,i+2);
	}
	for (uint16_t j=0;j<i;j++){
		putchar(' ');
	}
	printf("%c: %"PRIu64"\n",(RB_NODE_GET_COLOR(n)==COLOR_RED?'R':'B'),n->v);
	if (n->r!=e){
		ASSERT(n->r->p==n);
		_print_node(n->r,e,i+2);
	}
}



static void _free_node(rb_node_t* n,rb_node_t* e){
	if (RB_NODE_GET_LEFT(n)!=e){
		_free_node(RB_NODE_GET_LEFT(n),e);
	}
	if (n->r!=e){
		_free_node(n->r,e);
	}
	if (n!=e){
		free(n);
	}
}



void rb_init_tree(rb_tree_t* t){
	ASSERT(!(((uint64_t)(&(t->_n)))&1));
	t->_n.p=0;
	t->_n.l=(uint64_t)(&(t->_n));
	t->_n.r=&(t->_n);
	t->_n.v=0;
	t->r=&(t->_n);
}



rb_node_t* rb_insert_node(rb_tree_t* t,uint64_t v){
	rb_node_t* nil=&(t->_n);
	rb_node_t* x=t->r;
	rb_node_t* y=NULL;
	while (x!=nil){
		y=x;
		if (x->v>v){
			x=RB_NODE_GET_LEFT(x);
		}
		else if (x->v<v){
			x=x->r;
		}
		else{
			return x;
		}
	}
	rb_node_t* o=malloc(sizeof(rb_node_t));
	o->p=y;
	o->l=((uint64_t)nil)|COLOR_RED;
	o->r=nil;
	o->v=v;
	if (!y){
		t->r=o;
		RB_NODE_SET_COLOR(o,COLOR_BLACK);
		return o;
	}
	if (o->v<y->v){
		RB_NODE_SET_LEFT(y,o);
	}
	else{
		y->r=o;
	}
	if (o->p->p){
		rb_node_t* n=o;
		while (RB_NODE_GET_COLOR(n->p)==COLOR_RED){
			if (n->p==n->p->p->r){
				rb_node_t* u=RB_NODE_GET_LEFT(n->p->p);
				if (RB_NODE_GET_COLOR(u)==COLOR_BLACK){
					if (n==RB_NODE_GET_LEFT(n->p)){
						n=n->p;
						rb_node_t* x=RB_NODE_GET_LEFT(n);
						RB_NODE_SET_LEFT(n,x->r);
						x->p=n->p;
						if (x->r!=nil){
							x->r->p=n;
						}
						if (!n->p){
							t->r=x;
						}
						else if (n==n->p->r){
							n->p->r=x;
						}
						else{
							RB_NODE_SET_LEFT(n->p,x);
						}
						n->p=x;
						x->r=n;
					}
					RB_NODE_SET_COLOR(n->p,COLOR_BLACK);
					x=n->p->p;
					RB_NODE_SET_COLOR(x,COLOR_RED);
					rb_node_t* y=x->r;
					x->r=RB_NODE_GET_LEFT(y);
					y->p=x->p;
					if (RB_NODE_GET_LEFT(y)!=nil){
						RB_NODE_GET_LEFT(y)->p=x;
					}
					if (!x->p){
						t->r=y;
					}
					else if (x==RB_NODE_GET_LEFT(x->p)){
						RB_NODE_SET_LEFT(x->p,y);
					}
					else{
						x->p->r=y;
					}
					x->p=y;
					RB_NODE_SET_LEFT(y,x);
				}
				else{
					RB_NODE_SET_COLOR(u,COLOR_BLACK);
					RB_NODE_SET_COLOR(n->p,COLOR_BLACK);
					n=n->p->p;
					RB_NODE_SET_COLOR(n,COLOR_RED);
				}
			}
			else{
				rb_node_t* u=n->p->p->r;
				if (RB_NODE_GET_COLOR(u)==COLOR_BLACK){
					if (n==n->p->r){
						n=n->p;
						rb_node_t* x=n->r;
						n->r=RB_NODE_GET_LEFT(x);
						x->p=n->p;
						if (RB_NODE_GET_LEFT(x)!=nil){
							RB_NODE_GET_LEFT(x)->p=n;
						}
						if (!n->p){
							t->r=x;
						}
						else if (n==RB_NODE_GET_LEFT(n->p)){
							RB_NODE_SET_LEFT(n->p,x);
						}
						else{
							n->p->r=x;
						}
						n->p=x;
						RB_NODE_SET_LEFT(x,n);
					}
					RB_NODE_SET_COLOR(n->p,COLOR_BLACK);
					x=n->p->p;
					RB_NODE_SET_COLOR(x,COLOR_RED);
					rb_node_t* y=RB_NODE_GET_LEFT(x);
					RB_NODE_SET_LEFT(x,y->r);
					y->p=x->p;
					if (y->r!=nil){
						y->r->p=x;
					}
					if (!x->p){
						t->r=y;
					}
					else if (x==x->p->r){
						x->p->r=y;
					}
					else{
						RB_NODE_SET_LEFT(x->p,y);
					}
					x->p=y;
					y->r=x;
				}
				else{
					RB_NODE_SET_COLOR(u,COLOR_BLACK);
					RB_NODE_SET_COLOR(n->p,COLOR_BLACK);
					n=n->p->p;
					RB_NODE_SET_COLOR(n,COLOR_RED);
				}
			}
			if (n==t->r){
				break;
			}
		}
		RB_NODE_SET_COLOR(t->r,COLOR_BLACK);
	}
	return o;
}



void rb_delete_node(rb_tree_t* t,rb_node_t* n){
	rb_node_t* nil=&(t->_n);
	uint8_t cl=RB_NODE_GET_COLOR(n);
	rb_node_t* x=NULL;
	if (RB_NODE_GET_LEFT(n)==nil){
		if (!n->p){
			t->r=n->r;
		}
		else if (n==RB_NODE_GET_LEFT(n->p)){
			RB_NODE_SET_LEFT(n->p,n->r);
		}
		else{
			n->p->r=n->r;
		}
		n->r->p=n->p;
		x=n->r;
	}
	else if (n->r==nil){
		if (!n->p){
			t->r=RB_NODE_GET_LEFT(n);
		}
		else if (n==RB_NODE_GET_LEFT(n->p)){
			RB_NODE_SET_LEFT(n->p,RB_NODE_GET_LEFT(n));
		}
		else{
			n->p->r=RB_NODE_GET_LEFT(n);
		}
		RB_NODE_GET_LEFT(n)->p=n->p;
		x=RB_NODE_GET_LEFT(n);
	}
	else{
		rb_node_t* y=n->r;
		while (RB_NODE_GET_LEFT(y)!=nil){
			y=RB_NODE_GET_LEFT(y);
		}
		cl=RB_NODE_GET_COLOR(y);
		x=y->r;
		if (y->p==n){
			x->p=y;
		}
		else{
			if (!y->p){
				t->r=y->r;
			}
			else if (y==RB_NODE_GET_LEFT(y->p)){
				RB_NODE_SET_LEFT(y->p,y->r);
			}
			else{
				y->p->r=y->r;
			}
			y->r->p=y->p;
			y->r=n->r;
			y->r->p=y;
		}
		if (!n->p){
			t->r=y;
		}
		else if (n==RB_NODE_GET_LEFT(n->p)){
			RB_NODE_SET_LEFT(n->p,y);
		}
		else{
			n->p->r=y;
		}
		y->p=n->p;
		RB_NODE_SET_LEFT(y,RB_NODE_GET_LEFT(n));
		RB_NODE_GET_LEFT(y)->p=y;
		RB_NODE_SET_COLOR(y,RB_NODE_GET_COLOR(n));
	}
	if (cl==COLOR_RED){
		return;
	}
	while (x!=t->r&&RB_NODE_GET_COLOR(x)==COLOR_BLACK){
		if (x==RB_NODE_GET_LEFT(x->p)){
			rb_node_t*y=x->p->r;
			if (RB_NODE_GET_COLOR(y)==COLOR_RED){
				RB_NODE_SET_COLOR(y,COLOR_BLACK);
				rb_node_t* z=x->p;
				RB_NODE_SET_COLOR(z,COLOR_RED);
				y=z->r;
				z->r=RB_NODE_GET_LEFT(y);
				y->p=z->p;
				if (RB_NODE_GET_LEFT(y)!=nil){
					RB_NODE_GET_LEFT(y)->p=z;
				}
				if (!z->p){
					t->r=y;
				}
				else if (z==RB_NODE_GET_LEFT(z->p)){
					RB_NODE_SET_LEFT(z->p,y);
				}
				else{
					z->p->r=y;
				}
				z->p=y;
				RB_NODE_SET_LEFT(y,z);
			}
			if (RB_NODE_GET_COLOR(RB_NODE_GET_LEFT(y))==COLOR_BLACK&&RB_NODE_GET_COLOR(y->r)==COLOR_BLACK){
				RB_NODE_SET_COLOR(y,COLOR_RED);
				x=x->p;
			}
			else{
				if (RB_NODE_GET_COLOR(y->r)==COLOR_BLACK){
					RB_NODE_SET_COLOR(RB_NODE_GET_LEFT(y),COLOR_BLACK);
					RB_NODE_SET_COLOR(y,COLOR_RED);
					rb_node_t* z=RB_NODE_GET_LEFT(y);
					RB_NODE_SET_LEFT(y,z->r);
					z->p=y->p;
					if (z->r!=nil){
						z->r->p=y;
					}
					if (!y->p){
						t->r=z;
					}
					else if (y==y->p->r){
						y->p->r=z;
					}
					else{
						RB_NODE_SET_LEFT(y->p,z);
					}
					y->p=z;
					z->r=y;
					y=x->p->r;
				}
				RB_NODE_SET_COLOR(y,RB_NODE_GET_COLOR(x->p));
				RB_NODE_SET_COLOR(x->p,COLOR_BLACK);
				RB_NODE_SET_COLOR(y->r,COLOR_BLACK);
				rb_node_t* z=x->p;
				x=t->r;
				rb_node_t* w=z->r;
				z->r=RB_NODE_GET_LEFT(w);
				w->p=z->p;
				if (RB_NODE_GET_LEFT(w)!=nil){
					RB_NODE_GET_LEFT(w)->p=z;
				}
				if (!z->p){
					t->r=w;
				}
				else if (z==RB_NODE_GET_LEFT(z->p)){
					RB_NODE_SET_LEFT(z->p,w);
				}
				else{
					z->p->r=w;
				}
				z->p=w;
				RB_NODE_SET_LEFT(w,z);
			}
		}
		else{
			rb_node_t* y=RB_NODE_GET_LEFT(x->p);
			if (RB_NODE_GET_COLOR(y)==COLOR_RED){
				RB_NODE_SET_COLOR(y,COLOR_BLACK);
				rb_node_t* z=x->p;
				RB_NODE_SET_COLOR(z,COLOR_RED);
				y=RB_NODE_GET_LEFT(z);
				RB_NODE_SET_LEFT(z,y->r);
				y->p=z->p;
				if (y->r!=nil){
					y->r->p=z;
				}
				if (!z->p){
					t->r=y;
				}
				else if (z==z->p->r){
					z->p->r=y;
				}
				else{
					RB_NODE_SET_LEFT(z->p,y);
				}
				z->p=y;
				y->r=z;
			}
			if (RB_NODE_GET_COLOR(RB_NODE_GET_LEFT(y))==COLOR_BLACK&&RB_NODE_GET_COLOR(y->r)==COLOR_BLACK){
				RB_NODE_SET_COLOR(y,COLOR_RED);
				x=x->p;
			}
			else{
				if (RB_NODE_GET_COLOR(RB_NODE_GET_LEFT(y))==COLOR_BLACK){
					RB_NODE_SET_COLOR(y->r,COLOR_BLACK);
					RB_NODE_SET_COLOR(y,COLOR_RED);
					rb_node_t* z=y->r;
					y->r=RB_NODE_GET_LEFT(z);
					z->p=y->p;
					if (RB_NODE_GET_LEFT(z)!=nil){
						RB_NODE_GET_LEFT(z)->p=y;
					}
					if (!y->p){
						t->r=z;
					}
					else if (y==RB_NODE_GET_LEFT(y->p)){
						RB_NODE_SET_LEFT(y->p,z);
					}
					else{
						y->p->r=z;
					}
					y->p=z;
					RB_NODE_SET_LEFT(z,y);
					y=RB_NODE_GET_LEFT(x->p);
				}
				RB_NODE_SET_COLOR(y,RB_NODE_GET_COLOR(x->p));
				RB_NODE_SET_COLOR(x->p,COLOR_BLACK);
				RB_NODE_SET_COLOR(RB_NODE_GET_LEFT(y),COLOR_BLACK);
				rb_node_t* z=x->p;
				x=t->r;
				rb_node_t* w=RB_NODE_GET_LEFT(z);
				RB_NODE_SET_LEFT(z,w->r);
				w->p=z->p;
				if (w->r!=nil){
					w->r->p=z;
				}
				if (!z->p){
					t->r=w;
				}
				else if (z==z->p->r){
					z->p->r=w;
				}
				else{
					RB_NODE_SET_LEFT(z->p,w);
				}
				z->p=w;
				w->r=z;
			}
		}
	}
	RB_NODE_SET_COLOR(x,COLOR_BLACK);
}



void rb_print_tree(rb_tree_t* t){
	if (t->r){
		_print_node(t->r,&(t->_n),0);
	}
}



void rb_free_tree(rb_tree_t* t){
	if (t->r){
		_free_node(t->r,&(t->_n));
		t->r=&(t->_n);
	}
}
