# personalMalloc

Part of an assignment in Operating Systems Programming was to implement a "myMalloc" program, i.e. we were supposed to create our own version of the malloc C call in C. The user would prompt the program for memory of some amount of bytes, and we were to return those bytes on part of a memory stack (we were only allowed to call sbrk with values greater than 8192 just like with malloc), and as the user called for more and more bytes we would progress through this stack of memory.
