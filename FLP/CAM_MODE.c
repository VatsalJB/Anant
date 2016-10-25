#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/wait.h>

#define freq 1000000

typedef struct linked{
void (*func)(void);
struct linked *next;
int not_running;
double next_time;
int id;
} node;

node *hk_node, *cam_pointing_n, *take_pic_n, *advbkn_node, *head;


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
                printf("ID of @%d updated to %d\n", hk_node, hk_node->id);       //testing only
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
        execv("/home/smr/Anant/Housekeeping/hk", NULL);
    }
    else
    {
        hk_node->not_running = 0;
        hk_node->id = nid;   //fork returns the id of the child process to the parent process.
        printf("New HK id should be %d and is %d @%d\n", nid, hk_node->id, hk_node);    //testing only
        return;
    }
}

void cam_pointing(void)
{
    //printf("cam_pointing_n called\n");
}

void take_pic(void)     //child must block till the cam triggers a hardware interrupt that pic was taken successfully.
{
    printf("%d\n", take_pic_n->id);
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
        execv("/home/smr/Anant/CAM/take_pic_n", NULL);
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
        execv("/home/smr/Anant/Transmit/transmitadv", NULL);
    }
    else
    {
        advbkn_node->not_running = 0;
        advbkn_node->id = nid;
        return;
    }
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
        printf("second head id %d pointing @%d\n", hk_node->id, second_head);       //testing only
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
            printf("ID is: %d @%d & hk id is:%d\n", head->id, head, hk_node->id);    //testing only
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
    struct sigaction handlr_struct;
    handlr_struct.sa_handler = child_terminate;
    handlr_struct.sa_flags = SA_RESTART|SA_NODEFER;
    sigemptyset(&handlr_struct.sa_mask);
    sigaction(SIGCHLD, &handlr_struct, NULL);
    
    hk_node = (node*) malloc(sizeof(node));
    cam_pointing_n = (node*) malloc(sizeof(node));
    take_pic_n = (node*) malloc(sizeof(node));
    advbkn_node = (node*) malloc(sizeof(node));
    //testing data
    printf("hk is @%d\n", hk_node);
    printf("cam_pointing is @%d\n", cam_pointing_n);
    printf("take_pic is @%d\n", take_pic_n);
    printf("advbkn is @%d\n", advbkn_node);
    //end of testing data
    hk_node->next = cam_pointing_n;
    cam_pointing_n->next = take_pic_n;
    take_pic_n->next = advbkn_node;
    advbkn_node->next = NULL;
    
    hk_node->func = hk;
    cam_pointing_n->func = cam_pointing;
    take_pic_n->func = take_pic;
    advbkn_node->func = advbkn;  
    
    hk_node->not_running = 1;
    cam_pointing_n->not_running = 1;
    advbkn_node->not_running = 1;
    take_pic_n->not_running = 1;
    
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
    
    iterate();
    free(hk_node);
    free(cam_pointing_n);
    free(advbkn_node);
    free(take_pic_n);
}
