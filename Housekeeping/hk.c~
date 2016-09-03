#include <stdio.h>
#include <stdlib.h>
#include "hwfunctions.h"
#include "/home/smr/Anant/Memory_management/mempool.h"
#include <time.h>

int main()
{
    init_pool(50);                      //memory pool instead of malloc().
    char *finalstr = (char* ) alloc();
    
    FILE *f = fopen("/home/smr/Anant/Housekeeping/fullhk.txt", "a");
    if(f==NULL)
    {
        printf("Not opened\n");
        exit(0);
    }
    printf("opened\n");
    time_t curt;
    curt = time(NULL);
    //fseek(f, 0, SEEK_END);
    sprintf(finalstr, "Time: %s\n", ctime(&curt));fputs(finalstr, f);
    sprintf(finalstr, "Bx: %d\n", get_Bx());fputs(finalstr, f);
    sprintf(finalstr, "By: %d\n", get_By());fputs(finalstr, f);
    sprintf(finalstr, "Bz: %d\n", get_Bz());fputs(finalstr, f);
    sprintf(finalstr, "S1: %d\n", get_S1());fputs(finalstr, f);
    sprintf(finalstr, "S2: %d\n", get_S2());fputs(finalstr, f);
    sprintf(finalstr, "S3: %d\n", get_S3());fputs(finalstr, f);
    sprintf(finalstr, "S4: %d\n", get_S4());fputs(finalstr, f);
    sprintf(finalstr, "S5: %d\n", get_S5());fputs(finalstr, f);
    sprintf(finalstr, "Wx: %d\n", get_Wx());fputs(finalstr, f);
    sprintf(finalstr, "Wy: %d\n", get_Wy());fputs(finalstr, f);
    sprintf(finalstr, "Wz: %d\n", get_Wz());fputs(finalstr, f);
    sprintf(finalstr, "Ax: %d\n", get_Ax());fputs(finalstr, f);
    sprintf(finalstr, "Ay: %d\n", get_Ay());fputs(finalstr, f);
    sprintf(finalstr, "Az: %d *end of block*\n\n", get_Az());
    fputs(finalstr, f);
    fclose(f);
    ret((void* ) finalstr, 140);
}
