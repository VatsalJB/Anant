#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

typedef struct linked{
void (*func)(void);
struct linked *next;
int notrunning;
int nexttime;
pid_t id;
} node;

node *hk_node, *bdot_node, *advbkn_node;

void childterminate()   //nonblocking!
{
    int stat;
    pid_t temp;
    temp = waitpid(-1, &stat, WNOHANG);
    //printf("%d\n", temp);
   while(temp>0)
    {
        if(temp==-1)
        {
            perror("Error");
        }
        else if(WIFEXITED(stat))
        {
            printf("Dead baby %d\n", temp);      //can switch flags here.
            if(temp==hk_node->id)
            {
                hk_node->notrunning = 1;
            }
            else if(temp==advbkn_node->id)
            {
                advbkn_node->notrunning = 1;
            }
            else if(temp==bdot_node->id)
            {
                bdot_node->notrunning = 1;
            }
        }
       temp = waitpid(-1, &stat, WNOHANG);
    }
    printf("loop exited\n");
}

void hk(void)
{
    printf("HK CALLED\n");
    pid_t id;
    id = fork();
    if(id<0)
    {
        printf("Failed\n");
    }
    else if(id == 0)
    {
        printf("HK forked and execed with id %d\n", getpid());      //can remove getpid after testing.
        hk_node->id = getpid();
        execv("/home/smr/Anant/Housekeeping/hk", NULL);
        //end of housekeeping!
    }
    else
    {
        return;
    }
}

void bdot(void)
{
    //to be added.
    printf("BDOT CALLED\n");
    return;
}

void advbkn(void)
{
    printf("ADVBKN CALLED\n");
    pid_t id;
    id = fork();
    if(id<0)
    {
        printf("Failed\n");
    }
    else if(id == 0)
    {
        printf("ADVBKN forked and execed %d\n", getpid());      //again, can remove getpid after testing.
        advbkn_node->id = getpid();
       // sleep(0.7);                 //remove this!
        execv("/home/smr/Anant/Transmit/transmitadv", NULL);
        //end of housekeeping!
    }
    else
    {
        return;
    }
}

void iterate(void)
{   
    node *it = hk_node;
    while(it!=NULL)
    {
        (*it).func();
        it = it->next;
    } 
}

int main()
{
    struct sigaction handlrstruct;
    handlrstruct.sa_handler = childterminate;
    handlrstruct.sa_flags = SA_RESTART;// | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &handlrstruct, NULL);
    
    hk_node = (node*) malloc(sizeof(node*));
    bdot_node = (node*) malloc(sizeof(node*));
    advbkn_node = (node*) malloc(sizeof(node*));
    hk_node->next = bdot_node;
    bdot_node->next = advbkn_node;
    advbkn_node->next = NULL;
    
    hk_node->func = hk;
    bdot_node->func = bdot;
    advbkn_node->func = advbkn;
    
    iterate();
    //pause();
    while(1)
    {
        //nothing
    }
    printf("bye\n");
}
