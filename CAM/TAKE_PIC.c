#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define CAMSIG SIGRTMIN
//signal handler for a custom real time signal that is generated iff the camera is not taking a picture.
//this signal handler should suicide.

void jagomohanpyare(void)
{
    //printf("Signal received\n");
    exit(EXIT_SUCCESS);
}

int check_power()
{
    //to be written
    return 1;
}

void take_pic(void)
{
    //Hardware function: will be called from a header file
    //will block till interrupt comes from the camera
    printf("process group id is %d\n", getpgid(0));
    sleep(3600);    
}

int main()
{
    struct sigaction handlr_struct_rt;
    handlr_struct_rt.sa_handler = jagomohanpyare;
    sigaction(CAMSIG, &handlr_struct_rt, NULL);
    int flag = check_power();
    if(flag==0)
    {
        return;
    }
    take_pic();
}
