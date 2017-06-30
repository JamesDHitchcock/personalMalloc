/*
Malloc Lab
James Hitchcock
*/
#include "mymalloc.h" F_START = NULL;

typedef flist{
	int s;
	struct flist *next;
} *Flist;

Flist F_START;
F_START->next = NULL;
F_START->s = 0;

void* my_malloc(size_t size){
	int remainder;
	Flist check, s_flist, s_end, f_end, tmp, prev;

	size = size + 8; //need 8 bytes for bookkeeping
	remainder = size % 8;
	if(remainder > 0){//aligns bytes to 8
		size = size + (8 - remainder);
	}
	check = (Flist) check_free_list(size);
	if(check != NULL){ //space in free list found, take off free
		if(size + 16 <= check->s){//can split node
			s_flist = check;

			//creates new node by splitting check
			s_end = check + (check->s)/4;//finds end of mem
			s_end = s_end - size/4;//finds start of new user mem
			s_end->s = size;
			s_end->next = NULL;
			s-end = s_end + 2;

			//value still on free list, but must update size
			s_flist->s = s_flist->s - size;

			return((void *) s_end);
		}
		else{//cannot split node, must take off free list
			prev = (Flist) free_list_prev((void *) check);
			tmp = check->next;
			prev->next = tmp;

			check->next = NULL;
			check = check + 2;
			return((void *)check);
		}
	}
	else{//no space in free list
		if(size < 8192){
			s_flist = (Flist) sbark(8192);
			s_end = s_flist + (8192/4); //might be inefficient. Finds end of mem
			s_end = s_end - (size / 4);//pointer arithmetic for / 4
			s_end->s = size;
			s_end->next = NULL;
			s_end = s_end + 2;//will return s_end
							  //this is user's mem block
			
			s_flist->s = 8192 - size;//this is rest of data for free list
			s_flist->next = NULL;
			f_end = (Flist) free_list_end();
			f_end->next = s_flist;

			return((void*) s_end);
		}
		else{//size >= 8192
			s_flist = (Flist) sbark(size);
			s_flist->s = size;
			s_flist->next = NULL;
			s_flist = s_flist + 2;
			return((void *)s_return);
		}
}

void my_free(void *ptr){

}

void* free_list_begin(){
	return ( (void *)F_START );
}

void* free_list_next(void * node){
	if(node == NULL){
		fprintf(stderr, "Error in calling free list next. No valid node passed.\n");
		exit(1);
	}
	Flist tmp = (Flist) node;
	return( (void *) tmp->next);
}

void coalesce_free_list{

}

void* check_free_list(size_t size){//returns first block of memory with > size
	Flist search = (Flist) free_list_begin();
	while(search != NULL){
		if(search.s >= size){
			return((void *)search);
		}
		search = search->next;
	}
	return NULL;//nothing large enough in free list
}

void* free_list_end(){//returns last block of list. (the pointer that points to
						//null)
	Flist search = (Flist) free_list_begin();
	
	while(search !=NULL){//could be a while(1) loop
		if(search->next == NULL){
			return( (void*)search);
		}
		search = search->next;
	}
	return((void *) search);
}

void* free_list_prev(void *node){
	Flist search = (Flist) free_list_begin();
	Flist target = (Flist) node;

	while(search->next != target){
		if(search->next == NULL){
			fprintf(stderr, "Huge error in calling free list prev. Node not in in list.\n");
			exit(1);
		}
		search = search->next;
	}
	return((void *)search);
}
