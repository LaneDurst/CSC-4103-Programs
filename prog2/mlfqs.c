#include "prioque.h"
#include "prioque.c"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>

Queue* ArrivalQ;
Queue* finalReport;

typedef struct ProcessBehavior{
    int cpuburst;
    int ioburst;
    int repeat;
}ProcessBehavior;

typedef struct Process{
    int arrival_time;
    int pid;
    int good; // stores how many times the process has behaved at this level
    int bad; //stores how many times the process has misbehaved at this level
    Queue* behaviors;
}Process;

void init_process(Process* p)
{
    init_queue(p->behaviors, sizoef(ProcessBehavior), true, NULL, true);
    //set all the values in Process to their base values. Will get changed later
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
            add_to_queue(&ArrivalQ, &p, INT64_MAX - p.arrival_time);
            init_process(&p);
        }
        first = false;
        p.pid = pid;
        p.arrival_time = arrival_time;
        add_to_queue(&p.behaviors, &b, 1);
    }
    add_to_queue(&ArrivalQ, &p, INT64_MAX - p.arrival_time);
}

void dump_arrival_queue(Queue* q)
{
    element = 0;
    rewind_queue(&q);
    printf("Queue contains:\n");
    while (! end_of_queue(&q)) 
    {
        printf("%" PRId64 " --> %d / %s with priority %" PRId32 ".\n",           
	    ++element,
	    ((SomeType *)pointer_to_current(&q))->a,
	    ((SomeType *)pointer_to_current(&q))->buf,
	    current_priority(&q));
        next_element(&q);
    }   
}

bool processes_exist(Queue* q1, Queue* q2, Queue* q3, Queue* q4)
{
    if (empty_queue(ArrivalQ) && empty_queue(q1) && empty_queue(q2) && empty_queue(q3) && empty_queue(q4))
        return false;
    else
        return true;
}

void queue_new_arrivals(int currentTime, Queue* HighPrioQueue)
{
    //TODO: DEFINE BEHAVIOR

    // check ArrivalQ for processes whose TIME = currentTime?
    // add these processes to highest priority queue q1
    // do printf("CREATE: Process %d entered the ready queue at time %d", pid, currentTime);
}

void execute_highest_priority_process(int currentTime, struct ProcessCompletionInfo finalReport[])
{
    //TODO: DEFINE BEHAVIOR

    // iterate through each queue in order of priority?

    // when you get to the process you need to figure out what queue it is in for the printf
    // print should be printf("RUN: Process %d started execution from level %d at time %d; wants to execute for %d ticks", pid, queueLevel, currentTime, timeToI0);
}

void do_IO(int currentTime)
{
    //TODO: DEFINE BEHAVIOR

    //printf("I/O: Process %d blocked for I/O at time %d", pid, currentTime);
}

void final_report(int currentTime, struct ProcessCompletionInfo finalReport[])
{
    printf("Scheduler shutdown at time %d.\nTotal CPU usage for all processes scheduled:\n", currentTime);
    // need to loop through finalReport and print to screen
}

int main(int argc, char* argv[])
{
    // creating and initializing all of the queues
    Queue* q1;
    Queue* q2;
    Queue* q3;
    Queue* q4;
    init_queue(ArrivalQ, sizeof(void*), true, NULL, true); // This queue is not created here as it is a global variable
    init_queue(finalReport, sizeof(void*), true, NULL, true); // This queue is a global queue, used to store each process and it's end time
    init_queue(q1, sizeof(void*), true, NULL, true);
    init_queue(q2, sizeof(void*), true, NULL, true);
    init_queue(q3, sizeof(void*), true, NULL, true);
    init_queue(q4, sizeof(void*), true, NULL, true);

    // this process is what runs when no actual processes exists or all processes are doing I/0
    // this could also be an integer if need be
    Process nullProcess;
    ProcessBehavior nullProcessBehavior;
    init_process(&nullProcess);

    // read in all the process info from stdin
    read_process_descriptions();

    if (argc > 1) 
    {// for debugging
    dump_arrival_queue();
    }

    int clock = 0;
    while (processes_exist(q1, q2, q3, q4)) 
    {
        clock++; // might need to put this after do_IO, unsure
        queue_new_arrivals(clock, q1);
        execute_highest_priority_process(clock);
        do_IO(clock);
    }

    clock++;
    final_report(clock);

return 0;
}
