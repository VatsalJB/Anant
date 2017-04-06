#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define CAMSIG SIGRTMIN
//signal handler for a custom real time signal that is generated iff the camera is not taking a picture.
//this signal handler should suicide.

void respond(int i)                 //change by VJB (function argument changed to int from void)
{
    //printf("Signal received\n");
    exit(EXIT_SUCCESS);     // i is just used to match the prototype of function pointer sa_handler
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
    handlr_struct_rt.sa_handler = respond; 			 //change by VJB (function pointer data type mismatch)
    sigaction(CAMSIG, &handlr_struct_rt, NULL);
    int flag = check_power();
    if(flag==0)
    {
        return;
    }
    take_pic();
}
