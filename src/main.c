#include <red_black_tree.h>
#include <stdio.h>



int main(int argc,const char** argv){
	rb_tree_t t;
	rb_init_tree(&t);
	rb_insert_node(&t,1);
	rb_insert_node(&t,6);
	rb_insert_node(&t,8);
	rb_insert_node(&t,11);
	rb_node_t* rm0=rb_insert_node(&t,19);
	rb_insert_node(&t,13);
	rb_insert_node(&t,15);
	rb_insert_node(&t,17);
	rb_insert_node(&t,22);
	rb_insert_node(&t,25);
	rb_insert_node(&t,27);
	rb_node_t* rm1=rb_insert_node(&t,126);
	rb_print_tree(&t);
	putchar('\n');
	rb_delete_node(&t,rm0);
	rb_delete_node(&t,rm1);
	rb_print_tree(&t);
	rb_free_tree(&t);
	return 0;
}
