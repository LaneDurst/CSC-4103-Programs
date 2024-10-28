#include "prioque.h"
#include "prioque.c"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>

Queue* ArrivalQ;
Queue* q1;
Queue* q2;
Queue* q3;
Queue* q4;
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
    int bad; // stores how many times the process has misbehaved at this level
    Queue* behaviors;
}Process;

typedef struct _SomeType {
  int32_t a;
  char buf[10];
} SomeType;

void init_process(Process* p)
{
    init_queue(p->behaviors, sizeof(ProcessBehavior), true, NULL, true);
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

void dump_queue(Queue* q)
{
    int element = 0;
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

bool processes_exist(void)
{
    if (empty_queue(ArrivalQ) && empty_queue(q1) && empty_queue(q2) && empty_queue(q3) && empty_queue(q4))
        return false;
    else
        return true;
}

void queue_new_arrivals(int currentTime, Queue* HighPrioQueue)
{
    rewind_queue(ArrivalQ); // moves to the start of ArrivalQ
    while(true)
    {
        Process* process;
        int32_t* priority;
        peek_at_current(ArrivalQ, process, priority); // copies our current element into process and priority variables

        // if it arrives at the currentTime, add it to Q1 and delete from ArrivalQ
        if(process->arrival_time == currentTime) /* [May need to be changed to '<=' unsure] */
        {
            add_to_queue(HighPrioQueue, process, priority);
            delete_current(ArrivalQ);
            printf("CREATE: Process %d entered the ready queue at time %d", process->pid, currentTime);
        }
        else break; // since elements should be in order, when the above check fails, we stop
    }
}

void execute_highest_priority_process(int currentTime)
{
    //TODO: DEFINE BEHAVIOR

    // iterate through each queue in order of priority?
    if(!empty_queue(q1))
    {
        // find and execute first process
    }
    else if(!emptyqueue(q2))
    {

    }
    else if(!emptyqueue(q3))
    {

    }
    else if(!emptyqueue(q4))
    {

    }
    else
    {
        //execute the null process
    }

    // when you get to the process you need to figure out what queue it is in for the printf
    // print should be printf("RUN: Process %d started execution from level %d at time %d; wants to execute for %d ticks", pid, queueLevel, currentTime, timeToI0);
}

void do_IO(int currentTime)
{
    //TODO: DEFINE BEHAVIOR

    //printf("I/O: Process %d blocked for I/O at time %d", pid, currentTime);
}

void final_report(int currentTime)
{
    printf("Scheduler shutdown at time %d.\nTotal CPU usage for all processes scheduled:\n", currentTime);
    // need to loop through finalReport and print to screen
}

int main(int argc, char* argv[])
{
    //initializing all of the queues
    init_queue(ArrivalQ, sizeof(void*), true, NULL, true); 
    init_queue(finalReport, sizeof(void*), true, NULL, true); 
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
    dump_queue(ArrivalQ);
    }

    int clock = 0;
    while (processes_exist()) 
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
