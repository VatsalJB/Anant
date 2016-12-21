#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main()
{
    //read and write manually into this function
    kill(-6548, SIGRTMIN);
}
