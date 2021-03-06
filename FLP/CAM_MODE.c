#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/wait.h>

#define freq 1000000
#define CAMSIG SIGRTMIN

typedef struct linked{
void (*func)(void);
struct linked *next;
int not_running;
double next_time;
int id;
} node;

node *hk_node, *cam_pointing_n, *take_pic_n, *advbkn_node, *switch_node, *head;

clock_t clstr;

void child_terminate()   //nonblocking!
{
    int stat;
    int temp;
    temp = (int) waitpid(-1, &stat, WNOHANG);
   while(temp>0)
    {
        if(WIFEXITED(stat))
        {
            printf("Dead baby %d\n", temp);
            if(temp==hk_node->id)
            {
                hk_node->not_running = 1;
                hk_node->id = 0;
                //printf("ID of @%d updated to %d\n", hk_node, hk_node->id);       //testing only
            }
            else if(temp==advbkn_node->id)
            {
                advbkn_node->not_running = 1;
                advbkn_node->id = 0;
            }
            else if(temp==cam_pointing_n->id)
            {
                cam_pointing_n->not_running = 1;
                cam_pointing_n->id = 0;
            }
            else if(temp==take_pic_n->id)
            {
                take_pic_n->not_running = 1;
                take_pic_n->id = 0;
            }
        }
        temp = waitpid(-1, &stat, WNOHANG);
        if(temp==0)
        {
            return;
        }
        if(temp==-1)
        {
            return;
        }
    }
}

void respond(void)
{
    //lite
}

void hk(void)
{
    pid_t nid;
    nid = fork();
    if(nid<0)
    {
        printf("Failed\n");
    }
    else if(nid == 0)
    {
        printf("HK forked and execed with id %d\n", getpid());
        //sleep(5); //only for testing! 
        if(execv("../Housekeeping/hk", NULL)==-1)
        {
            perror("HK error");
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        hk_node->not_running = 0;
        hk_node->id = nid;   //fork returns the id of the child process to the parent process.
        return;
    }
}

void cam_pointing(void)
{
    //printf("cam_pointing_n called\n");
}

void take_pic(void)     //child must block till the cam triggers a hardware interrupt that pic was taken successfully.
{
    pid_t nid;
    nid = fork();
    if(nid<0)
    {
        printf("Failed\n");
    }
    else if(nid == 0)
    {
        printf("TAKE_PIC forked and execed %d\n", getpid());
        //sleep(3);       //remove after tesing
        if(execv("../CAM/TAKE_PIC", NULL)==-1)
        {
            perror("TAKE_PIC error");
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        take_pic_n->not_running = 0;
        take_pic_n->id = nid;
        return;
    }
}

void advbkn(void)
{
    pid_t nid;
    nid = fork();
    if(nid<0)
    {
        printf("Failed\n");
    }
    else if(nid == 0)
    {
        printf("ADVBKN forked and execed %d\n", getpid());
        //sleep(3);       //remove after tesing
        if(execv("../Transmit/transmitadv", NULL)==-1)
        {
            perror("ADVBBKN error");
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        advbkn_node->not_running = 0;
        advbkn_node->id = nid;
        return;
    }
}

void switch_mode(void)
{
    srand(time(0));
    if(rand()%10==1)
    {
        while(!(hk_node->not_running)||!(cam_pointing_n->not_running)||!(take_pic_n->not_running)||!(advbkn_node->not_running))     //Even if any one of the children is alive, wait for it to die.
        {
            //do nothing
        }
        if(execv("FLP", NULL)==-1)
        {
            perror("Switch CAM to FLP");
        }
    }
    return;
}

void order_list(node *temp)
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
    }
    while(iter!=NULL)
    {
        if(temp->next_time<(*iter).next_time)
        {
            temp->next = iter;
            if(prv_iter!=temp)
                prv_iter->next = temp;
            if(iter!=second_head&&flag)
            {
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
            head = second_head;
        }
    }
}

void iterate(void)
{   
    struct timespec ts;
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        double time_microsec = (double) ts.tv_sec*1000000 +  (double) ts.tv_nsec/1000;
        if(head->not_running==0)
        {
            head->next_time = time_microsec+freq;
            order_list(head);
        }
        else if(time_microsec>head->next_time||time_microsec==head->next_time)
        {
            (*head).func();
            head->next_time = time_microsec+freq;
            order_list(head);
        }
    }
}

int main()
{    
    printf("Hello this is CAM_MODE\n");
    struct sigaction handlr_struct;
    handlr_struct.sa_handler = child_terminate;
    handlr_struct.sa_flags = SA_RESTART|SA_NODEFER;
    sigemptyset(&handlr_struct.sa_mask);
    sigaction(SIGCHLD, &handlr_struct, NULL);
    
    struct sigaction handlr_struct_rt;
    handlr_struct_rt.sa_handler = respond;
    sigaction(CAMSIG, &handlr_struct_rt, NULL);
    
    hk_node = (node*) malloc(sizeof(node));
    cam_pointing_n = (node*) malloc(sizeof(node));
    take_pic_n = (node*) malloc(sizeof(node));
    advbkn_node = (node*) malloc(sizeof(node));
    switch_node = (node*) malloc(sizeof(node));
    
    hk_node->next = cam_pointing_n;
    cam_pointing_n->next = take_pic_n;
    take_pic_n->next = advbkn_node;
    advbkn_node->next = switch_node;
    switch_node->next = NULL;
    
    hk_node->func = hk;
    cam_pointing_n->func = cam_pointing;
    take_pic_n->func = take_pic;
    advbkn_node->func = advbkn;  
    switch_node->func = switch_mode;
    
    hk_node->not_running = 1;
    cam_pointing_n->not_running = 1;
    advbkn_node->not_running = 1;
    take_pic_n->not_running = 1;
    switch_node->not_running = 1;
    
    head = hk_node;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double time_microsec = (double) ts.tv_sec*1000000 + (double) ts.tv_nsec/1000;
    //printf("time %f\n", time_microsec);
    //initial next time can be changed;
    hk_node->next_time = time_microsec;
    cam_pointing_n->next_time = time_microsec;
    take_pic_n->next_time = time_microsec;
    advbkn_node->next_time = time_microsec;
    switch_node->next_time = time_microsec;
    
    iterate();
    free(hk_node);
    free(cam_pointing_n);
    free(advbkn_node);
    free(take_pic_n);
    free(switch_node);
}
