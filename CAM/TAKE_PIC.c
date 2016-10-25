#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define CAMSIG SIGRTMIN
//signal handler for a custom real time signal that is generated iff the camera is not taking a picture.
//this signal handler should kill suicide.

void jagomohanpyare(void)
{
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
    sleep(3600);    
}

int main()
{
    struct sigaction handlr_struct;
    handlr_struct.sa_handler = jagomohanpyare;
    sigaction(CAMSIG, &handlr_struct, NULL);
    int flag = check_power();
    if(flag==0)
    {
        return;
    }
    take_pic();
}
