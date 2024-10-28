#include "prioque.h"
#include "prioque.c"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>

Queue* ArrivalQ;

typedef struct Process{
    int arrival_time;
    int pid;
    Queue* behaviors;
}Process;

typedef struct ProcessBehavior{
    int cpuburst;
    int ioburst;
    int repeat;
}ProcessBehavior;

void init_process(Process* p)
{
    //TODO: DEFINE BEHAVIOR
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

void dump_arrival_queue(void)
{
    //TODO: DEFINE BEHAVIOR
}

bool processes_exist(Queue* q1, Queue* q2, Queue* q3, Queue* q4)
{
    if (empty_queue(ArrivalQ) && empty_queue(q1) && empty_queue(q2) && empty_queue(q3) && empty_queue(q4))
        return false;
    else
        return true;
}

void queue_new_arrivals(void)
{
    //TODO: DEFINE BEHAVIOR
}

void execute_highest_priority_process(void)
{
    //TODO: DEFINE BEHAVIOR
}

void do_IO(void)
{
    //TODO: DEFINE BEHAVIOR
}

int main(int argc, char* argv[])
{
    Queue* q1;
    Queue* q2;
    Queue* q3;
    Queue* q4;
    init_queue(ArrivalQ, sizeof(void*), true, NULL, true);
    init_queue(q1, sizeof(void*), true, NULL, true);
    init_queue(q2, sizeof(void*), true, NULL, true);
    init_queue(q3, sizeof(void*), true, NULL, true);
    init_queue(q4, sizeof(void*), true, NULL, true);

    init_process(&IdleProcess);
    read_process_descriptions();

    if (argc > 1) 
    {// for debugging
    dump_arrival_queue();
    }

    int clock = 0;
    while (processes_exist(q1, q2, q3, q4)) 
    {
        clock++;
        queue_new_arrivals();
        execute_highest_priority_process();
        do_IO();
    }

    clock++;
    final_report();

return 0;
}
