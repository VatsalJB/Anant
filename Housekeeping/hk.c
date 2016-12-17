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
    time_t curt;
    curt = time(NULL);
    //fseek(f, 0, SEEK_END);
    sprintf(finalstr, "Time: %s\n", ctime(&curt));fputs(finalstr, f);
<<<<<<< HEAD
    sprintf(finalstr, "Bx: %f\n", get_Bx());fputs(finalstr, f);
    sprintf(finalstr, "By: %f\n", get_By());fputs(finalstr, f);
    sprintf(finalstr, "Bz: %f\n", get_Bz());fputs(finalstr, f);
    sprintf(finalstr, "S1: %f\n", get_S1());fputs(finalstr, f);
    sprintf(finalstr, "S2: %f\n", get_S2());fputs(finalstr, f);
    sprintf(finalstr, "S3: %f\n", get_S3());fputs(finalstr, f);
    sprintf(finalstr, "S4: %f\n", get_S4());fputs(finalstr, f);
    sprintf(finalstr, "S5: %f\n", get_S5());fputs(finalstr, f);
    sprintf(finalstr, "Wx: %f\n", get_Wx());fputs(finalstr, f);
    sprintf(finalstr, "Wy: %f\n", get_Wy());fputs(finalstr, f);
    sprintf(finalstr, "Wz: %f\n", get_Wz());fputs(finalstr, f);
    sprintf(finalstr, "Ax: %f\n", get_Ax());fputs(finalstr, f);
    sprintf(finalstr, "Ay: %f\n", get_Ay());fputs(finalstr, f);
    sprintf(finalstr, "Az: %f *end of block*\n\n", get_Az());
=======
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
>>>>>>> d0696b80a70e6481bde4c9be018d0d5132ede03d
    fputs(finalstr, f);
    fclose(f);
    ret((void* ) finalstr, 140);
}
