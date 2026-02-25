#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

void outprint(int time_x, int time_y, int pid, int arrival_time, int execution_time);

void scheduler(Process* proc, LinkedQueue** ProcessQueue, int proc_num, int queue_num, int period){
    printf("Process number: %d\n", proc_num);
    for (int i = 0;i < proc_num; i++)
        printf("%d %d %d\n", proc[i].process_id, proc[i].arrival_time, proc[i].execution_time);

    printf("\nQueue number: %d\n", queue_num);
    printf("Period: %d\n", period);

    for (int i = 0;i < queue_num; i++){
        printf("%d %d %d\n", i, ProcessQueue[i]->time_slice, ProcessQueue[i]->allotment_time);
    }

    // MLFQ Implementation
    int currentTime = 0;
    int completed_processes = 0;
    int proc_idx = 0; 

    while (completed_processes < proc_num) {
        // Rule 5: Priority Boost
        if (currentTime > 0 && currentTime % period == 0) {
            for (int i = 1; i < queue_num; i++) {
                while (!IsEmptyQueue(ProcessQueue[i])) {
                    Process p = DeQueue(ProcessQueue[i]);
                    EnQueue(ProcessQueue[0], p);
                }
            }
        }

        // Rule 3: Arrival Check
        while (proc_idx < proc_num && proc[proc_idx].arrival_time <= currentTime) {
            EnQueue(ProcessQueue[0], proc[proc_idx]);
            proc_idx++;
        }

        // Rule 1: Find highest priority non-empty queue
        int q_idx = -1;
        for (int i = 0; i < queue_num; i++) {
            if (!IsEmptyQueue(ProcessQueue[i])) {
                q_idx = i;
                break;
            }
        }

        if (q_idx != -1) {
            // Rule 2: Round-Robin execution
            Process p = DeQueue(ProcessQueue[q_idx]);
            int quantum = ProcessQueue[q_idx]->time_slice;
            int timeToRun = (p.execution_time < quantum) ? p.execution_time : quantum;

            outprint(currentTime, currentTime + timeToRun, p.process_id, p.arrival_time, p.execution_time);

            p.execution_time -= timeToRun;
            currentTime += timeToRun;

            // Re-check arrivals
            while (proc_idx < proc_num && proc[proc_idx].arrival_time <= currentTime) {
                EnQueue(ProcessQueue[0], proc[proc_idx]);
                proc_idx++;
            }

            // Rule 4: Demotion
            if (p.execution_time > 0) {
                if (timeToRun == quantum && q_idx < queue_num - 1) {
                    EnQueue(ProcessQueue[q_idx + 1], p);
                } else {
                    EnQueue(ProcessQueue[q_idx], p);
                }
            } else {
                completed_processes++;
            }
        } else {
            currentTime++;
        }
    }
}
