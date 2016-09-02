#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#define freq 0.1

typedef struct linked{
void (*func)(void);
struct linked *next;
int not_running;
double next_time;
pid_t id;
} node;

node *hk_node, *bdot_node, *advbkn_node, *head;

clock_t clstr;

void child_terminate()   //nonblocking!
{
    int stat;
    pid_t temp;
    temp = waitpid(-1, &stat, WNOHANG);
    //printf("%d\n", temp);
   while(temp>0)
    {
        if(WIFEXITED(stat))
        {
            printf("Dead baby %d\n", temp);      //can switch flags here.
            if(temp==hk_node->id)
            {
                hk_node->not_running = 1;
            }
            else if(temp==advbkn_node->id)
            {
                advbkn_node->not_running = 1;
            }
            else if(temp==bdot_node->id)
            {
                bdot_node->not_running = 1;
            }
        }
        temp = waitpid(-1, &stat, WNOHANG);    
        if(temp==-1)
        {
            perror("Error");
        }
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

void order_list(node *temp)         //temp will always be head. See iterate function.
{
    node *iter, *prv_iter;
    node *second_head;
    int flag =0;
    iter = head;
    prv_iter = head;
    if(temp==head)
    {
        second_head = head->next;
        flag = 1;
        //printf("flag 1\n");
    }
    while(iter!=NULL)
    {
        if(temp->next_time<(*iter).next_time)
        {
            temp->next = iter;
            if(prv_iter!=temp)
                prv_iter->next = temp;
            if(iter!=head->next&&flag)
            {
                //printf("head changed\n");
                head = second_head;
            }
            break;
        }
        prv_iter = iter;
        iter = iter->next;
    }
    if(iter==NULL)
    {
        prv_iter->next = temp;
        temp->next = NULL;
        if(flag == 1)
        {
            //printf("head changed\n");
            head = second_head;
        }
    }
    //printf("ret\n");
}

void iterate(void)
{   
    struct timespec ts;
    while(1)
    {
        //printf("iterate loop\n");
        printf("head is %f\n", head->next_time);
        (*head).func();
        clock_gettime(CLOCK_MONOTONIC, &ts);
        double time_nanosec = (double) ts.tv_sec*1000000 +  (double) ts.tv_nsec/1000;
        head->next_time = time_nanosec+freq;
        order_list(head);
    }
}

int main()
{    
    struct sigaction handlr_struct;
    handlr_struct.sa_handler = child_terminate;
    handlr_struct.sa_flags = SA_RESTART;// | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &handlr_struct, NULL);
    
    hk_node = (node*) malloc(sizeof(node));
    bdot_node = (node*) malloc(sizeof(node));
    advbkn_node = (node*) malloc(sizeof(node));
    hk_node->next = bdot_node;
    bdot_node->next = advbkn_node;
    advbkn_node->next = NULL;
    
    hk_node->func = hk;
    bdot_node->func = bdot;
    advbkn_node->func = advbkn;    
    head = hk_node;    
     
    struct timespec ts;         //fix the time initialisation
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double time_nanosec = (double) ts.tv_sec*1000000 + (double) ts.tv_nsec/1000;
    printf("time %f\n", time_nanosec);
    hk_node->next_time = time_nanosec;
    
    bdot_node->next_time = time_nanosec+0.3;
    advbkn_node->next_time = time_nanosec+0.5;
    
    iterate();
    //pause();
    while(1)
    {
        //nothing
    }
    printf("bye\n");
    free(hk_node);
    free(bdot_node);
    free(advbkn_node);
}
