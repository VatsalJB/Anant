/*
    Author- Saurabh Raje
    --Code for the flightplan master process.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/wait.h>

#define freq 1000000    //This is the housekeeping frequency in microseconds

/*
    ----Struct for the subtasks of the idle mode of the satellite----
    
    void (*func)(void) = function pointer to the corresponding wrapper function for the subtask.
    *next = pointer to the next node (subtask) of the linked list.
    int not_running = flag with value 1 if child process has died and zero if it is alive and executing.
    double next_time = next time of execution for the subtask.
    int id = zero if no child is running for that subtask. Otherwise equal to the process ID of the child in currently carrying out this task.
*/

typedef struct linked{
void (*func)(void);
struct linked *next;
int not_running;
double next_time;
int id;
} node;

node *hk_node, *bdot_node, *advbkn_node, *head;

clock_t clstr;

void child_terminate()   //nonblocking termination handler for any of the subtasks(child processes)
{
    int stat;           //Stores the meta-data of terminated child. See man page for details
    int temp;
    temp = (int) waitpid(-1, &stat, WNOHANG);   //WNOHANG ensures immediate return in case of no dead babies
    /*
        While loop is necessary in case of multiple child processes dying at the same time leading to generation of a single SIGCHILD signal.
        Waitpid() must be called in a loop to ensure no zombies. See the man page of waitpid for more details.
    */
    
    while(temp>0)
    {
        if(WIFEXITED(stat))
        {
            printf("Dead baby %d\n", temp);
            /*
                Checking the running status for each subtask
            */
            if(temp==hk_node->id)
            {
                hk_node->not_running = 1;
                hk_node->id = 0;
            }
            else if(temp==advbkn_node->id)
            {
                advbkn_node->not_running = 1;
                advbkn_node->id = 0;
            }
            else if(temp==bdot_node->id)
            {
                bdot_node->not_running = 1;
                bdot_node->id = 0;
            }
        }
        temp = waitpid(-1, &stat, WNOHANG);
        if(temp==0||temp==-1)
        {
            return;
        }
    }
}

/*
    Each of the following wrapper functions fork and exec the corresponding child process. 
    Process ID of the child processes is logged in the task node of that subtask residing in the parent process memory.
*/

void hk(void)
{
    pid_t id;
    id = fork();
    if(id<0)
    {
        printf("Failed\n");
    }
    else if(id == 0)
    {
        printf("HK forked and execed with id %d\n", getpid());
        //sleep(2); //only for testing asynchronous operation! 
        execv("/home/smr/Anant/Housekeeping/hk", NULL);
    }
    else
    {
        hk_node->not_running = 0;
        hk_node->id = id;   //fork returns the id of the child process to the parent process which will be logged for the subtask.
        return;
    }
}

void bdot(void)
{
    printf("BDOT CALLED\n");
    //to be added.
    return;
}

void advbkn(void)
{
    pid_t id;
    id = fork();
    if(id<0)
    {
        printf("Failed\n");
    }
    else if(id == 0)
    {
        printf("ADVBKN forked and execed %d\n", getpid());
        //sleep(3);       //remove after tesing
        execv("/home/smr/Anant/Transmit/transmitadv", NULL);
    }
    else
    {
        advbkn_node->not_running = 0;
        advbkn_node->id = id;
        return;
    }
}

/*
    --Consider this as a blackbox that takes a node and puts it back in the list corresponding to its time of next execution.--
    Tedious code follows...
*/

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

/*
    This pops the head node and conditionally executes it but necessarily reorders the linked list.
*/

void iterate(void)
{   
    struct timespec ts;
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        double time_microsec = (double) ts.tv_sec*1000000 +  (double) ts.tv_nsec/1000;
        if(head->not_running==0)        //check if the subtask corresponding to the head is currently running
        {
            /*if(head==hk_node)
                head->next_time = time_microsec+freq+1000000;
            else*/
            //printf("ID is: %d @%d & hk id is:%d\n", head->id, head, hk_node->id);    //testing only
            head->next_time = time_microsec+freq;
            order_list(head);
        }
        else if(time_microsec>head->next_time||time_microsec==head->next_time)      //check if the scheduled time has elapsed or not
        {
            (*head).func();         
            /*if(head==hk_node)
                head->next_time = time_microsec+freq+1000000;
            else*/
            head->next_time = time_microsec+freq;
            order_list(head);
        }
    }
}

int main()
{    
    
    /*
        Map the SIGCHLD signal to our custom made signal handler that will be called in case the signal is raised.
        SIGCHLD is a signal that is generated and sent to the parent process when the child process terminates.
    */
    struct sigaction handlr_struct;
    handlr_struct.sa_handler = child_terminate;
    handlr_struct.sa_flags = SA_RESTART|SA_NODEFER;
    sigemptyset(&handlr_struct.sa_mask);
    sigaction(SIGCHLD, &handlr_struct, NULL);
    
    /*
        Make and initialise the linked list.
    */
    
    hk_node = (node*) malloc(sizeof(node));
    bdot_node = (node*) malloc(sizeof(node));
    advbkn_node = (node*) malloc(sizeof(node));
    hk_node->next = bdot_node;
    bdot_node->next = advbkn_node;
    advbkn_node->next = NULL;
    
    hk_node->func = hk;
    bdot_node->func = bdot;
    advbkn_node->func = advbkn;  
    
    hk_node->not_running = 1;
    bdot_node->not_running = 1;
    advbkn_node->not_running = 1; 
    
    head = hk_node;    
     
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double time_microsec = (double) ts.tv_sec*1000000 + (double) ts.tv_nsec/1000;
    printf("time %f\n", time_microsec);
    hk_node->next_time = time_microsec;
    bdot_node->next_time = time_microsec;
    advbkn_node->next_time = time_microsec;
    
    iterate();      //Initialisation over. Hello World!
    free(hk_node);
    free(bdot_node);
    free(advbkn_node);
    
    //Drops mic
}
