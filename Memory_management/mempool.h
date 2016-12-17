#ifndef mempool.h
#define mempool.h

enum int_errorcodes{ALLOK = 0, NOTFOUND = 1, MALLOCFAIL = 2, POOLDEAD = 3, MEMSETFAIL = 4};
//enum ptrerrorcodes{BUFFERFULL = NULL, DEADPOOL = NULL};

int init_pool(int blocksize);

void* alloc();

int ret(void* pt, int blocksize);

#endif
