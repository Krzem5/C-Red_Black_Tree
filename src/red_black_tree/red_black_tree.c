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



void _rotate(rb_tree_t* t,rb_node_t* n,uint8_t d){
	rb_node_t* a=n->c[1-d];
	n->c[1-d]=a->c[d];
	if (a->c[d]){
		RB_NODE_SET_PARENT(a->c[d],n);
	}
	rb_node_t* p=RB_NODE_GET_PARENT(n);
	RB_NODE_SET_PARENT(a,p);
	if (!p){
		t->r=a;
	}
	else if (n==p->c[0]){
		p->c[0]=a;
	}
	else{
		p->c[1]=a;
	}
	a->c[d]=n;
	RB_NODE_SET_PARENT(n,a);
}



void _print_node(rb_node_t* n,uint16_t i){
	if (n->c[0]){
		_print_node(n->c[0],i+2);
	}
	for (uint16_t j=0;j<i;j++){
		putchar(' ');
	}
	printf("%c: %"PRIu64"\n",(RB_NODE_GET_COLOR(n)==COLOR_RED?'R':'B'),n->v);
	if (n->c[1]){
		_print_node(n->c[1],i+2);
	}
}



void _free_node(rb_node_t* n){
	if (n->c[0]){
		free(n->c[0]);
	}
	if (n->c[1]){
		free(n->c[1]);
	}
	free(n);
}



void rb_init_tree(rb_tree_t* t){
	t->r=NULL;
}



rb_node_t* rb_insert_node(rb_tree_t* t,uint64_t v){
	if (!t->r){
		t->r=malloc(sizeof(rb_node_t));
		ASSERT((((uint64_t)t->r)&0xfffffffffffffffe)==((uint64_t)t->r));
		t->r->p=COLOR_BLACK;
		t->r->c[0]=NULL;
		t->r->c[1]=NULL;
		t->r->v=v;
		return t->r;
	}
	rb_node_t* n=t->r;
	rb_node_t* p=NULL;
	uint8_t d;
	do{
		p=n;
		if (v==n->v){
			return n;
		}
		d=(v<n->v?0:1);
		n=n->c[d];
	} while (n);
	n=malloc(sizeof(rb_node_t));
	ASSERT((((uint64_t)n)&0xfffffffffffffffe)==((uint64_t)n));
	n->p=((uint64_t)p&0xfffffffffffffffe)|COLOR_RED;
	n->c[0]=NULL;
	n->c[1]=NULL;
	n->v=v;
	p->c[d]=n;
	rb_node_t* o=n;
	do{
		if (RB_NODE_GET_COLOR(p)==COLOR_BLACK){
			break;
		}
		rb_node_t* g=RB_NODE_GET_PARENT(p);
		if (!g){
			RB_NODE_SET_COLOR(p,COLOR_BLACK);
			break;
		}
		d=(g->c[0]==p?0:1);
		rb_node_t* u=g->c[1-d];
		if (!u||RB_NODE_GET_COLOR(u)==COLOR_BLACK){
			if (n==p->c[1-d]){
				_rotate(t,p,d);
				n=p;
				p=g->c[d];
			}
			_rotate(t,g,1-d);
			RB_NODE_SET_COLOR(p,COLOR_BLACK);
			RB_NODE_SET_COLOR(g,COLOR_RED);
			break;
		}
		RB_NODE_SET_COLOR(p,COLOR_BLACK);
		RB_NODE_SET_COLOR(u,COLOR_BLACK);
		RB_NODE_SET_COLOR(g,COLOR_RED);
		n=g;
		p=RB_NODE_GET_PARENT(n);
	} while (p);
	return o;
}



void rb_delete_node(rb_tree_t* t,rb_node_t* n){
	rb_node_t* a;
	uint8_t cl=RB_NODE_GET_COLOR(n);
	if (!n->c[0]){
		a=n->c[1];
		rb_node_t* p=RB_NODE_GET_PARENT(n);
		if (!p){
			t->r=a;
		}
		else if (n==p->c[0]){
			p->c[0]=a;
		}
		else{
			p->c[1]=a;
		}
		if (a){
			RB_NODE_SET_PARENT(a,p);
		}
	}
	else if (!n->c[1]){
		a=n->c[0];
		rb_node_t* p=RB_NODE_GET_PARENT(n);
		if (!p){
			t->r=a;
		}
		else if (n==p->c[0]){
			p->c[0]=a;
		}
		else{
			p->c[1]=a;
		}
		RB_NODE_SET_PARENT(a,p);
	}
	else{
		rb_node_t* b=n->c[1];
		while (b->c[0]){
			b=b->c[0];
		}
		a=b->c[1];
		cl=RB_NODE_GET_COLOR(b);
		if (RB_NODE_GET_PARENT(b)==n){
			RB_NODE_SET_PARENT(a,n);
		}
		else{
			rb_node_t* p=RB_NODE_GET_PARENT(b);
			if (!p){
				t->r=a;
			}
			else if (b==p->c[0]){
				p->c[0]=a;
			}
			else{
				p->c[1]=a;
			}
			if (a){
				RB_NODE_SET_PARENT(a,p);
			}
			b->c[1]=n->c[1];
			RB_NODE_SET_PARENT(b->c[1],b);
		}
		rb_node_t* p=RB_NODE_GET_PARENT(n);
		if (!p){
			t->r=b;
		}
		else if (n==p->c[0]){
			p->c[0]=b;
		}
		else{
			p->c[1]=b;
		}
		RB_NODE_SET_PARENT(b,p);
		b->c[0]=n->c[0];
		RB_NODE_SET_PARENT(b->c[0],b);
		RB_NODE_SET_COLOR(b,RB_NODE_GET_COLOR(n));
	}
	if (cl==COLOR_RED||RB_NODE_GET_COLOR(a)==COLOR_RED){
		return;
	}
	do{
		rb_node_t* p=RB_NODE_GET_PARENT(a);
		if (a==p->c[0]){
			rb_node_t* s=p->c[1];
			if (RB_NODE_GET_COLOR(s)==COLOR_RED){
				RB_NODE_SET_COLOR(s,COLOR_BLACK);
				RB_NODE_SET_COLOR(p,COLOR_RED);
				_rotate(t,p,0);
				s=p->c[1];
			}
			if (RB_NODE_GET_COLOR(s->c[0])==COLOR_BLACK&&RB_NODE_GET_COLOR(s->c[1])==COLOR_BLACK){
				RB_NODE_SET_COLOR(s,COLOR_RED);
				a=p;
			}
			else{
				if (RB_NODE_GET_COLOR(s->c[1])==COLOR_BLACK){
					RB_NODE_SET_COLOR(s->c[0],COLOR_BLACK);
					RB_NODE_SET_COLOR(s,COLOR_RED);
					_rotate(t,s,1);
					s=p->c[1];
				}
				RB_NODE_SET_COLOR(s,RB_NODE_GET_COLOR(p));
				RB_NODE_SET_COLOR(p,COLOR_BLACK);
				RB_NODE_SET_COLOR(s->c[1],COLOR_BLACK);
				_rotate(t,p,0);
				break;
			}
		}
		else{
			rb_node_t* s=p->c[0];
			if (RB_NODE_GET_COLOR(s)==COLOR_RED){
				RB_NODE_SET_COLOR(s,COLOR_BLACK);
				RB_NODE_SET_COLOR(p,COLOR_RED);
				_rotate(t,p,1);
				s=p->c[0];
			}
			if (RB_NODE_GET_COLOR(s->c[0])==COLOR_BLACK&&RB_NODE_GET_COLOR(s->c[1])==COLOR_BLACK){
				RB_NODE_SET_COLOR(s,COLOR_RED);
				a=p;
			}
			else{
				if (RB_NODE_GET_COLOR(s->c[0])==COLOR_BLACK){
					RB_NODE_SET_COLOR(s->c[1],COLOR_BLACK);
					RB_NODE_SET_COLOR(s,COLOR_RED);
					_rotate(t,s,0);
					s=p->c[0];
				}
				RB_NODE_SET_COLOR(s,RB_NODE_GET_COLOR(p));
				RB_NODE_SET_COLOR(p,COLOR_BLACK);
				RB_NODE_SET_COLOR(s->c[0],COLOR_BLACK);
				_rotate(t,p,1);
				break;
			}
		}
	} while (a!=t->r&&RB_NODE_GET_COLOR(a)==COLOR_BLACK);
	RB_NODE_SET_COLOR(a,COLOR_BLACK);
}



void rb_print_tree(rb_tree_t* t){
	if (t->r){
		_print_node(t->r,0);
	}
}



void rb_free_tree(rb_tree_t* t){
	if (t->r){
		_free_node(t->r);
		t->r=NULL;
	}
}
