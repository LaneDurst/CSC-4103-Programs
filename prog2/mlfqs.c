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
Queue finalReport;

typedef struct ProcessBehavior{
    long unsigned int cpuburst;
    long unsigned int ioburst;
    long unsigned int repeat;
}ProcessBehavior;

typedef struct Process{
    int arrival_time;
    int pid;
    int good; // stores how many times the process has behaved at this level
    int bad; // stores how many times the process has misbehaved at this level
    Queue behaviors;
}Process;

typedef struct _SomeType {
  int32_t a;
  char buf[10];
} SomeType;

void init_process(Process* p)
{
    init_queue(&p->behaviors, sizeof(ProcessBehavior), true, NULL, true);
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
    if (empty_queue(&waitQ) && empty_queue(&q1) && empty_queue(&q2) && empty_queue(&q3) && empty_queue(&q4))
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
        if (process.arrival_time == currentTime)
        {
            // add element to q1 and remove from waitQ
            add_to_queue(HighPrioQueue, &process, priority);
            delete_current(&waitQ);
            printf("CREATE: Process %d entered the ready queue at time %d\n", process.pid, currentTime);
        }
        else next_element(&waitQ);
    }
}

void execute_highest_priority_process(int currentTime)
{
    //TODO: DEFINE BEHAVIOR
    printf("[EXEC] CURRENT TIME: %d\n", currentTime);

    // iterate through each queue in order of priority?

    // when you get to the process you need to figure out what queue it is in for the printf
    // print should be printf("RUN: Process %d started execution from level %d at time %d; wants to execute for %d ticks", pid, queueLevel, currentTime, timeToI0);
}

void do_IO(int currentTime)
{
    //TODO: DEFINE BEHAVIOR
    printf("[I/0] CURRENT TIME: %d\n", currentTime);

    //printf("I/O: Process %d blocked for I/O at time %d", pid, currentTime);
}

void final_report(int currentTime)
{
    printf("Scheduler shutdown at time %d.\nTotal CPU usage for all processes scheduled:\n", currentTime);
    // need to loop through finalReport and print to screen
}

int main(int argc, char* argv[])
{
    printf("=====================================\n");
    printf("INITIALIZING QUEUES\n");
    //initializing all of the queues
    init_queue(&waitQ, sizeof(Process), true, NULL, true); 
    printf("ARRIVAL QUEUE INITIALIZED\n");
    init_queue(&finalReport, sizeof(Process), true, NULL, true);
    printf("FINAL REPORT QUEUE INITIALIZED\n");
    init_queue(&q1, sizeof(Process), true, NULL, true);
    printf("Q1 INITIALIZED\n");
    init_queue(&q2, sizeof(Process), true, NULL, true);
    printf("Q2 INITIALIZED\n");
    init_queue(&q3, sizeof(Process), true, NULL, true);
    printf("Q3 INITIALIZED\n");
    init_queue(&q4, sizeof(Process), true, NULL, true);
    printf("Q4 INITIALIZED\n");
    printf("ALL QUEUES INITIALIZED SUCCESFULLY\n");
    printf("=====================================\n");

    // this process is what runs when no actual processes exists or all processes are doing I/0
    // this could also be an integer if need be
    printf("INITIALIZING THE NULL PROCESS\n");
    Process nullProcess;
    printf("Created: nullProcess\n");
    ProcessBehavior nullProcessBehavior;
    printf("Created: nullProcessBehavior\n");
    printf("initializing: nullProcess\n");
    init_process(&nullProcess);
    printf("initialized: nullProcess\n");
    printf("NULL PROCESS INITIALIZED\n");
    printf("=====================================\n");

    // read in all the process info from stdin
    printf("READING: STDIN\n");
    read_process_descriptions();
    printf("ALL PROCESSES ADDED TO WAITQ\n");
    printf("=====================================\n");

    if (argc > 1) 
    {// for debugging
    dump_queue(&waitQ);
    }

    int clock = 0;
    printf("CLOCK SET: 0\n");
    printf("ENETERING MAIN LOOP\n");
    while (processes_exist()) 
    {
        clock++; // might need to put this after do_IO, unsure
        printf("clock time: %d\n", clock);
        queue_new_arrivals(clock, &q1);
        printf("queued new arrivals\n");
        execute_highest_priority_process(clock);
        printf("executed highest priority process\n");
        do_IO(clock);
        printf("did I/0\n");
    }

    clock++;
    final_report(clock);

return 0;
}
