#include <stdio.h>
#include <stdlib.h>
#include "hwfunctions.h"
#include <time.h>

int main()
{
    char *finalstr = (char* ) malloc(50);
    
    FILE *f = fopen("../Housekeeping/fullhk.txt", "a");
    if(f==NULL)
    {
        perror("Couldn't open file\n");
        exit(0);
    }

	//initialization of the three sensors
	init_magnetometer();
	init_gy521();
	init_gy80();

    time_t curt;
    curt = time(NULL);
    //fseek(f, 0, SEEK_END);
    sprintf(finalstr, "Time: %s\n", ctime(&curt));fputs(finalstr, f);
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

    fputs(finalstr, f);
    fclose(f);
	
	//closing the three sensors
	clear_magnetometer();
	clear_gy521();
	clear_gy80();

    free(finalstr);
}
