// HW2 - Operating Systems (COSE341-02)
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct process_control_block {
    int process_number;
    char program_name[10];
    int arrival_time;
    int burst_time;
};

enum algorithm_choice { NONE, FCFS, SJF, RR };

/*
 * Calculates and records the time of the CPU burst for each process
 * Forks each program to run and finds the difference between start and end time
 */
void get_CPU_burst(char p_path[][10], char p_command[][10], double p_time[]) {
    for (int i = 0; i < 5; ++i) {
        clock_t start = clock();
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Failed");
            return;
        } else if (pid == 0) {
            execlp(p_path[i], p_command[i], NULL);
            exit(0);
        } else {
            wait(NULL);
        }
        clock_t end = clock();

        p_time[i] = ((double)(end - start) / CLOCKS_PER_SEC) * 100000;
    }
}

/*
 * Initalizes the ready queue with process_control_block
 */
void init_processes(struct process_control_block process[]) {
    char program_path[5][10] = {
        "/bin/ls", "/bin/uname", "/bin/echo", "/bin/date", "/bin/stty",
    };

    char program_command[5][10] = {
        "ls", "uname", "echo", "date", "stty",
    };
    double program_time[5];

    get_CPU_burst(program_path, program_command, program_time);

    for (int i = 0; i < 5; ++i) {
        process[i].process_number = i + 1;
        process[i].arrival_time = 0;
        strcpy(process[i].program_name, program_command[i]);
        process[i].burst_time = program_time[i];
    }
}

void display_process_info(struct process_control_block process[],
                          enum algorithm_choice choice) {
    if (choice == NONE) {
        printf("Initial Process Information:\n");
        printf("-------------------------------------------------\n");
        printf("|Process Number\t|Program Name\t|Burst Time\t|\n");
        printf("|-----------------------------------------------|\n");
        for (int i = 0; i < 5; ++i) {
            printf("|%d\t\t|%s\t\t|%d\t\t|\n", process[i].process_number,
                   process[i].program_name, process[i].burst_time);
        }
        printf("-------------------------------------------------\n");
        printf("\n");
    } else {
        int total_wait_time = 0;
        for (int i = 0; i < 5; ++i) {
            total_wait_time += process[i].arrival_time;
        }
        if (choice == FCFS) {
            printf("First Come First Serve Scheduling Algorithm:\n");
        } else if (choice == SJF) {
            printf("Nonpreemptive Shortest Job First Scheduling Algorithm:\n");
        } else if (choice == RR) {
            printf("Round Robin Scheduling Algorithm:\n");
        }
        printf("---------------------------------------------------------------"
               "--\n");
        printf(
            "|Process Number\t|Program Name\t|Waiting Time\t|Burst Time\t|\n");
        printf("|--------------------------------------------------------------"
               "-|\n");
        for (int i = 0; i < 5; ++i) {
            printf("|%d\t\t|%s\t\t|%d\t\t|%d\t\t|\n", process[i].process_number,
                   process[i].program_name, process[i].arrival_time,
                   process[i].burst_time);
        }
        printf("---------------------------------------------------------------"
               "--\n");
        printf("Average Waiting Time: %.2f\n\n", total_wait_time / 5.0);
    }
}

/*
 * Runs the FCFS algorithm on the processes and treats the array as a FIFO queue
 */
void run_FCFS(struct process_control_block process[], bool print) {
    int current_time = 0;
    int current_process = 0;
    int burst_time_left = 0;
    bool process_running = false;
    while (current_process < 5) {
        if (!process_running) {
            process[current_process].arrival_time = current_time;
            burst_time_left = process[current_process].burst_time - 1;
            process_running = true;
        }
        if (burst_time_left == 0) {
            process_running = false;
            current_process += 1;
        }
        burst_time_left -= 1;
        current_time += 1;
    }
    if (print) {
        display_process_info(process, FCFS);
    }
}

/*
 * Custom compare function used for SJF algorithm
 * First compares and looks for the shorter burst_time
 * If burst_time are the same, then SJF becomes FCFS
 */
int compare(const void *a, const void *b) {
    struct process_control_block *tmp1 = (struct process_control_block *)a;
    struct process_control_block *tmp2 = (struct process_control_block *)b;
    if (tmp1->burst_time < tmp2->burst_time) {
        return -1;
    } else if (tmp1->burst_time > tmp2->burst_time) {
        return 1;
    } else {
        if (tmp1->process_number < tmp2->process_number) {
            return -1;
        } else {
            return 1;
        }
    }
}

/*
 * Runs the nonpreemptive SJF algorithm on the processes
 * Typically, the algorithm should be used with priority queue data structure
 * However, since the total amount of processes was specified to be 5 and never
 * change, it is unnecessary and more complex to implement a priority queue when
 * new processes will not be added during runtime. Therefore, the same results
 * were achieved using a custom compare function and sorting algorithm
 */
void run_SJF(struct process_control_block process[]) {
    struct process_control_block priority_queued_process[5];
    memcpy(priority_queued_process, process, sizeof(process[0]) * 5);
    qsort(priority_queued_process, 5, sizeof(struct process_control_block),
          compare);
    run_FCFS(priority_queued_process, false);
    for (int i = 0; i < 5; ++i) {
        int ind = priority_queued_process[i].process_number - 1;
        process[ind].arrival_time = priority_queued_process[i].arrival_time;
    }
    display_process_info(process, SJF);
}

/*
 * Runs the RR algorithm on the processes
 * A good guideline says 80% of CPU bursts should be shorter than the time
 * quantum Therefore, after repeated measurements the time quantum is set to 25
 */
void run_RR(struct process_control_block process[]) {
    struct process_control_block c_process[5];
    memcpy(c_process, process, sizeof(process[0]) * 5);
    for (int i = 0; i < 5; ++i) {
        c_process[i].arrival_time = 0;
    }
    bool finished_process[5] = {false, false, false, false, false};
    const int time_slice = 25;
    int current_time = time_slice;
    int finished_processes = 0;
    int current_process = 0;

    while (finished_processes < 5) {
        int ind = current_process % 5;
        for (int i = 0; i < 5; ++i) {
            if (i == ind || finished_process[i]) {
                continue;
            }
            c_process[i].arrival_time += 1;
        }
        c_process[ind].burst_time -= 1;
        current_time -= 1;
        if (c_process[ind].burst_time <= 0) {
            finished_process[ind] = true;
            finished_processes += 1;
            current_time = time_slice;
            if (finished_processes < 5) {
                while (true) {
                    current_process += 1;
                    if (finished_process[current_process % 5] == false) {
                        break;
                    }
                }
            }
        } else if (current_time == 0) {
            current_time = time_slice;
            if (finished_processes < 5) {
                while (true) {
                    current_process += 1;
                    if (finished_process[current_process % 5] == false) {
                        break;
                    }
                }
            }
        }
    }
    for (int i = 0; i < 5; ++i) {
        c_process[i].burst_time = process[i].burst_time;
    }
    display_process_info(c_process, RR);
}

int main() {
    printf("Running programs...\n\n");
    struct process_control_block process[5];
    init_processes(process);
    printf("\nRunning scheduling algorithms...\n\n");

    display_process_info(process, NONE);

    run_FCFS(process, true);
    run_SJF(process);
    run_RR(process);
}