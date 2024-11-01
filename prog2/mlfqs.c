#include "prioque.h"
#include "prioque.c"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>

Queue waitQ;
Queue q1;
Queue q2;
Queue q3;
Queue q4;
Queue IO;
Queue finalReport;

int quantums[] = {10, 30, 100, 200};
int64_t proCriteria[] = {INT64_MAX, 1, 2, 2}; // stores a processes promotion criteria for a given level
int64_t demoCriteria[] = {1, 2, 2, INT64_MAX}; // stores a processes demotion criteria for a given level

int waitTime = 0;

typedef struct finishInfo{
    int pid;
    int finishTime;
}finishInfo;

typedef struct ProcessBehavior{
    long unsigned int cpuburst;
    long unsigned int ioburst;
    long unsigned int repeat;
}ProcessBehavior;

typedef struct Process{
    int arrival_time;
    int pid;
    int remQuant; // how many ticks the process has remaining in its quantum
    int remBurst; // how many ticks the process has remaining in its current burst
    int timesRepeated; // the amount of times the current cpu I/0 process can repeat
    int remIO;
    int g; // stores how many times the process has behaved at its current level, consecutively
    int b; // stores how many times the process has misbehaved at its current level, consecutively
    int level; // stores what queue level the process is in, for easier access
    bool doingIO;
    bool usedQuant;
    bool justAdded; // tells if the value was just added to the queue, for cl output
    Queue behaviors;
}Process;

typedef struct _SomeType {
  int32_t a;
  char buf[10];
} SomeType;

void init_process(Process* p)
{
    init_queue(&p->behaviors, sizeof(ProcessBehavior), true, NULL, true);
    p->g = 0;
    p->b = 0;
    p->remQuant;
    p->timesRepeated = 0;
    p->doingIO = false;
    p->usedQuant = false;
}

static void read_process_descriptions(void)
{
    Process p;
    ProcessBehavior b;
    int32_t pid = 0;
    bool first = true;
    uint64_t arrival_time;

    init_process(&p);
    arrival_time = 0;
    while (scanf("%" PRIu64, &arrival_time) != EOF)
    {
        scanf("%" PRId32 "%" PRIu64 "%" PRIu64 "%" PRIu64, &pid, &b.cpuburst, &b.ioburst, &b.repeat);
        if (! first && p.pid != pid)
        {   
            add_to_queue(&waitQ, &p, INT64_MAX - p.arrival_time);
            init_process(&p);
        }
        first = false;
        p.pid = pid;
        p.arrival_time = arrival_time;
        add_to_queue(&p.behaviors, &b, 1);
    }
    add_to_queue(&waitQ, &p, INT64_MAX - p.arrival_time);
}

void dump_queue(Queue* q)
{
    long int element = 0;
    rewind_queue(q);
    printf("Queue contains:\n");
    while (! end_of_queue(q)) 
    {
        Process p;
        int32_t prior;
        peek_at_current(q, &p, &prior);
        printf("%ld --> %d / with priority %u.\n", ++element, p.pid, prior);
        next_element(q);
    }   
}

bool processes_exist(void)
{
    if (empty_queue(&waitQ) && empty_queue(&q1) && empty_queue(&q2) && empty_queue(&q3) && empty_queue(&q4) && empty_queue(&IO))
        return false;
    else
        return true;
}

void queue_new_arrivals(int currentTime, Queue* HighPrioQueue)
{
    rewind_queue(&waitQ); //moves to the start of waitQ
    int element = 0;
    while (! end_of_queue(&waitQ)) //read elements 1 by one
    {
        Process process;
        int32_t priority;
        peek_at_current(&waitQ, &process, &priority); //grabs values and store them in process and priority
        ProcessBehavior b;
        int32_t bPriority;
        rewind_queue(&process.behaviors);
        peek_at_current(&process.behaviors, &b, &bPriority);
        if (process.arrival_time == currentTime)
        {
            // add element to q1 and remove from waitQ
            process.level = 1;
            process.remQuant = quantums[process.level-1];
            process.remBurst = b.cpuburst;
            process.justAdded = true;
            add_to_queue(HighPrioQueue, &process, priority);
            delete_current(&waitQ);
            printf("CREATE: Process %d entered the ready queue at time %d\n", process.pid, currentTime);
        }
        else next_element(&waitQ);
    }
}

// this returns whatever queue the process was put into so we can track it
bool promote_demote(Process *p, int32_t priority, Queue *q, int currentTime)
{
    Queue *proQ;
    Queue *demoQ;
    switch(p->level)
    {
        case 1:
            proQ = &q1; // this should never occur, but fallback behavior is good
            demoQ = &q2;
        case 2:
            proQ = &q1;
            demoQ = &q3;
        case 3:
            proQ = &q2;
            demoQ = &q4;
        case 4:
            proQ = &q3;
            demoQ = &q4; // this should never occur, but fallback behavior is good
    }

    /*
    Process redundProc;
    int32_t redundPrio;
    peek_at_current(q, &redundProc, &redundPrio);

    if(!(redundProc.pid == p->pid && redundPrio == priority))
    {
        printf("[line 166] ERROR: current element for queue does not match input element\n");
        //printf("[%d =/= %d || %d =/= %d]\n", redundProc.pid, p->pid, redundPrio, priority);
        return NULL;
    }
    */
    
    if (p->g >= proCriteria[p->level-1])
    {
        printf("promoting %d\n", p->pid);
        p->level = p->level-1;
        p->g = 0;
        p->b = 0;
        p->remQuant = quantums[p->level-1];
        p->usedQuant = false;
        p->justAdded = true;
        delete_current(q);
        add_to_queue(proQ, p, priority);
        printf("QUEUED: Process %d queued at level %d at time %d.\n", p->pid, p->level, currentTime);
        return true;
    }
    else if (p->b >= demoCriteria[p->level-1])
    {
        //printf("demoting %d\n", p->pid);
        p->level = p->level+1;
        p->g = 0;
        p->b = 0;
        p->remQuant = quantums[p->level-1];
        p->justAdded = true;
        p->usedQuant = false;
        delete_current(q);
        add_to_queue(demoQ, p, priority);
        printf("QUEUED: Process %d queued at level %d at time %d.\n", p->pid, p->level, currentTime);
        return true;
    }
    else
        return false;
}

void execute_highest_priority_process(int currentTime)
{
    Process p;
    int32_t priority;
    Queue *q;
    //printf("grabbing process info\n");
    if (!empty_queue(&q1)) // if it is NOT empty
    {
        rewind_queue(&q1); // make sure we are at the start
        peek_at_current(&q1, &p, &priority); // grab the first value in the queue
        q = &q1;
    }
    else if (!empty_queue(&q2))
    {
        rewind_queue(&q2);
        peek_at_current(&q2, &p, &priority);
        q = &q2;
    }
    else if (!empty_queue(&q3))
    {
        rewind_queue(&q3);
        peek_at_current(&q3, &p, &priority);
        q = &q3;
    }
    else if (!empty_queue(&q4))
    {
        rewind_queue(&q4);
        peek_at_current(&q4, &p, &priority);
        q = &q4;
    }
    else
    {
        waitTime++;
        return;
    }


    //grabing behavior info
    ProcessBehavior b;
    int32_t bPriority;
    rewind_queue(&p.behaviors);
    peek_at_current(&p.behaviors, &b, &bPriority);

    int Quantum = quantums[p.level-1];
    if(p.justAdded) // it has just entered the cpu stage
    {
        printf("RUN: Process %d started execution from level %d at time %d; wants to execute for %d ticks.\n", p.pid, p.level, currentTime, p.remBurst);
        p.justAdded = false;
    }

    if(p.remQuant == 0) // the process used its full quantum
    {
        p.b++;
        p.g = 0;
        p.usedQuant = true;
        //printf("[%d] bad: %d\tgood:%d\n",currentTime, p.b, p.g);
        if (p.remBurst == 0) // finished the cpu burst
        {
            p.doingIO = true;
            p.remIO = b.ioburst;
        }
        p.remQuant = quantums[p.level-1]; // for if it uses the full quantum but doesn't get demoted
    }
    else if(p.remBurst == 0) // finished burst, but didn't use the whole quantum
    {
        p.g++;
        p.b = 0;
        //printf("[%d] bad: %d\tgood:%d\n",currentTime, p.b, p.g);
        p.doingIO = true;
        p.remIO = b.ioburst;
    }
    else // has not used the full quantum or finished cpu burst
    {
        p.remQuant--;
        p.remBurst--;
    }

    if (p.timesRepeated == b.repeat) // if the current behavior has finished its repeat cycles
        delete_current(&p.behaviors);
    if (empty_queue(&p.behaviors)) // the process has completely finished
    {
        // adding data to finalReport queue
        finishInfo f = {.pid = p.pid, .finishTime = currentTime};
        add_to_queue(&finalReport, &f, priority);
        delete_current(q);

        printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
        return;
    }

    if (p.doingIO) //because of how IO is set up, this will handle promotions for values put into IO once they are finished
    {
        delete_current(q);
        add_to_queue(&IO, &p, priority);
    }
    else
    {
        bool moved = promote_demote(&p, priority, q, currentTime);
        if(p.usedQuant && !moved)
        {
            printf("QUEUED: Process %d queued at level %d at time %d.\n", p.pid, p.level, currentTime);
            p.usedQuant = false;
            p.justAdded = true;
        }
        if (!moved) update_current(q, &p);
    }
}

void do_IO(int currentTime)
{
    if (!empty_queue(&IO)) // if the IO queue isn't empty
    {
        rewind_queue(&IO);
        while (!end_of_queue(&IO))
        {
            Process p;
            int32_t priority;
            ProcessBehavior b;
            int32_t bPriority;
            peek_at_current(&IO, &p, &priority);
            rewind_queue(&p.behaviors);
            peek_at_current(&p.behaviors, &b, &bPriority);

            if (p.remIO == b.ioburst) // first tick it is in here for
                printf("I/O: Process %d blocked for I/O at time %d.\n", p.pid, currentTime);
            
            p.remIO--;
            update_current(&IO, &p);
            if (p.remIO == 0)
            {
                p.doingIO = false;
                p.remQuant = quantums[p.level-1];
                p.remBurst = b.cpuburst;
                p.timesRepeated++; //has finished one CPU / IO cycle
                p.justAdded = true;
                printf("QUEUED: Process %d queued at level %d at time %d.\n", p.pid, p.level, currentTime);
                switch(p.level) // add it back to whatever queue it was in
                {
                case 1:
                    add_to_queue(&q1, &p, priority);
                    break;
                case 2:
                    add_to_queue(&q2, &p, priority);
                    break;
                case 3:
                    add_to_queue(&q2, &p, priority);
                    break;
                case 4:
                    add_to_queue(&q2, &p, priority);
                    break;
                }
                delete_current(&IO); // remove it from the IO queue
            }
            else next_element(&IO); // if you deleted the element you were on, you don't want to move also
        }
    }
    else
        return;
}

void final_report(int currentTime)
{
    printf("Scheduler shutdown at time %d.\nTotal CPU usage for all processes scheduled:\n", currentTime);
    rewind_queue(&finalReport);
    while(!end_of_queue(&finalReport))
    {
        finishInfo f;
        int32_t p;
        peek_at_current(&finalReport, &f, &p);
        printf("Process %d:\t%d time units.\n", f.pid, f.finishTime);
        next_element(&finalReport);
    }
}

int main(int argc, char* argv[])
{
    //printf("=====================================\n");
    //printf("INITIALIZING QUEUES\n");
    //initializing all of the queues
    init_queue(&waitQ, sizeof(Process), true, NULL, true); 
    //printf("ARRIVAL QUEUE INITIALIZED\n");
    init_queue(&finalReport, sizeof(finishInfo), true, NULL, true);
    //printf("FINAL REPORT QUEUE INITIALIZED\n");
    init_queue(&q1, sizeof(Process), true, NULL, true);
    //printf("Q1 INITIALIZED\n");
    init_queue(&q2, sizeof(Process), true, NULL, true);
    //printf("Q2 INITIALIZED\n");
    init_queue(&q3, sizeof(Process), true, NULL, true);
    //printf("Q3 INITIALIZED\n");
    init_queue(&q4, sizeof(Process), true, NULL, true);
    //printf("Q4 INITIALIZED\n");
    init_queue(&IO, sizeof(Process), true, NULL, true);
    //printf("ALL QUEUES INITIALIZED SUCCESFULLY\n");
    //printf("=====================================\n");

    // this process is what runs when no actual processes exists or all processes are doing I/0
    // this could also be an integer if need be
    //printf("INITIALIZING THE NULL PROCESS\n");
    //Process nullProcess;
    //printf("Created: nullProcess\n");
    //ProcessBehavior nullProcessBehavior;
    //printf("Created: nullProcessBehavior\n");
    //printf("initializing: nullProcess\n");
    //init_process(&nullProcess);
    //add_to_queue(&q4, &nullProcess, 0);
    //printf("initialized: nullProcess\n");
    //printf("NULL PROCESS INITIALIZED\n");
    //printf("=====================================\n");

    // read in all the process info from stdin
    //printf("READING: STDIN\n");
    read_process_descriptions();
    //printf("ALL PROCESSES ADDED TO WAITQ\n");
    //printf("=====================================\n");

    if (argc > 1) 
    {// for debugging
    dump_queue(&waitQ);
    }

    int clock = 0;
    //printf("CLOCK SET: 0\n");
    //printf("ENETERING MAIN LOOP\n");
    while (processes_exist()) //TODO: CHANGE THIS BACK TO 'while(processes_exist())' ONCE FINISHED
    {
        clock++; // might need to put this after do_IO, unsure
        //if (clock%10 == 0) printf("[DEBUG] Clock time: %d\n", clock);
        queue_new_arrivals(clock, &q1);
        //printf("queued new arrivals\n");
        execute_highest_priority_process(clock);
        //printf("executed highest priority process\n");
        do_IO(clock);
        //printf("did I/0\n");
    }
    //printf("=====================================\n");
    clock++;
    final_report(clock);

return 0;
}
