#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void *free_list_begin();
void *free_list_next(void *node);
