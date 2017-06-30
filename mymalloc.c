/*
Malloc Lab
James Hitchcock
*/
#include "mymalloc.h" 

typedef struct flist{
	int s;
	struct flist *next;
} *Flist;

Flist F_START = NULL;

void* free_list_begin(){
	return ( (void *)F_START );
}

void* free_list_prev(void *node){
	Flist search = (Flist) free_list_begin();
	Flist target = (Flist) node;
	if(search == target){//only node in list is target
		return NULL;
	}

	while(search->next != target){
		if(search->next == NULL){
			fprintf(stderr, "Huge error in calling free list prev. Node not in in list.\n");
			exit(1);
		}
		search = search->next;
	}
	return((void *)search);
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

void* check_free_list(size_t size){//returns first block of memory with > size
	Flist search = (Flist) free_list_begin();
	while(search != NULL){
		if(search->s >= (int)size){
			return((void *)search);
		}
		search = search->next;
	}
	return NULL;//nothing large enough in free list
}

void* my_malloc(size_t size){
	int remainder;
	Flist check, s_flist, s_end, f_end, tmp, prev;

	size = size + 8; //need 8 bytes for bookkeeping
	remainder = size % 8;
	if(remainder > 0){//aligns bytes to 8
		size = size + (8 - remainder);
	}
	check = (Flist) check_free_list((size_t)size);
	if(check != NULL){ //space in free list found, take off free
		if(size + 8 <= check->s){//can split node
			s_flist = check;

			//creates new node by splitting check
			s_end = check + (check->s)/8;//finds end of mem
			s_end = s_end - size/8;//finds start of new user mem
			s_end->s = size;
			s_end->next = NULL;
			s_end = s_end + 1;//pointer arithmetic s_end is 8 bytes (it's Flist)

			//value still on free list, but must update size
			s_flist->s = s_flist->s - size;

			return((void *) s_end);
		}
		else{//cannot split node, must take off free list
			prev = (Flist) free_list_prev((void *) check);
			if(prev == NULL){//nothing else on free list but check
				F_START = NULL;
			}
			else{
				tmp = check->next;
				prev->next = tmp;
			}
			check->next = NULL;
			check = check + 1;
			return((void *)check);
		}
	}
	else{//no space in free list
		if(size < 8192){
			s_flist = (Flist) sbrk(8192);
			s_end = s_flist + (8192/8); //might be inefficient. Finds end of mem
			s_end = s_end - (size / 8);//pointer arithmetic for / 8
			s_end->s = size;
			s_end->next = NULL;
			s_end = s_end + 1;//will return s_end
							  //this is user's mem block
			s_flist->s = 8192 - size;//this is rest of data for free list
			s_flist->next = NULL;
			f_end = (Flist) free_list_end();
			if(f_end == NULL){//no free list
				F_START = s_flist;
			}
			else{//free list exists
				f_end->next = s_flist;
			}
			return((void*) s_end);
		}
		else{//size >= 8192
			s_flist = (Flist) sbrk(size);
			s_flist->s = size;
			s_flist->next = NULL;
			s_flist = s_flist + 1;
			return((void *)s_flist);
		}
	}
}

void my_free(void *ptr){
	ptr = ptr - 8;
	Flist free = (Flist) ptr;
	Flist end = (Flist) free_list_end();
	if(end == NULL){//free list is empty
		F_START = free;
	}
	else{
		end->next = free;
	}
}


void* free_list_next(void * node){
	if(node == NULL){
		fprintf(stderr, "Error in calling free list next. No valid node passed.\n");
		exit(1);
	}
	Flist tmp = (Flist) node;
	return( (void *) tmp->next);
}

int cmpfunc(const void *a, const void *b){
	Flist x = *( (Flist*) a);
	Flist y = *( (Flist*) b);
	return ( x - y);
}

void coalesce_free_list(){
	Flist search = (Flist) free_list_begin();
	if(search == NULL || search->next == NULL){
		fprintf(stderr, "Coalescing empty free list.\n");
		exit(1);
	}
	int count = 0;
	while(search != NULL){
		count++;
		search = search->next;
	}
	Flist* p = (Flist *) malloc(sizeof(Flist)*count);
	int i;
	search = (Flist) free_list_begin();
	for(i = 0; i < count; i++){
		p[i] = search;
		search = search->next;
	}
	
	qsort(p, count, sizeof(Flist *), cmpfunc);
	for(i = 0; i < count - 1; i++){
		p[i]->next = p[i+1];
	}
	p[count-1]->next = NULL;
	F_START = p[0];

	free(p);
	//flist should now be sorted.

	//compare adjacent memory with flist as it's now sorted so next in flist
	//will be adjacent if both blocks are in flist
	Flist nextl;
	Flist adj;
	Flist tmp;
	search = (Flist) free_list_begin();
	while(search != NULL){
		nextl = search->next;
		adj = search + (search->s/8);
		if(nextl == adj){//next in free list is an adjacent node
			tmp = adj->next;
			search->s = search->s + adj->s;
			search->next = tmp;
			//we do not update search because we need to check to see if node
			//after adj is ALSO adj
		}
		else{//next in free list is not an adjacent node
			 //move to next node
			search = search->next;
		}
	}
}
