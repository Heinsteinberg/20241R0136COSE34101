#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> // isspace
#include <stdbool.h>

#define MAX_STRING_LEN 100
#define ALLOW_EMPTY_INPUT true
#define NO_EMPTY_INPUT false

#define MAX_NUM_OF_SIMULATIONS 99

#define NUM_OF_SCHEDULING_ALGS 6
#define FCFS 0
#define NON_PREEMPTIVE_SJF 1
#define PREEMPTIVE_SJF 2
#define NON_PREEMPTIVE_PRIORITY 3
#define PREEMPRIVE_PRIORITY 4
#define ROUND_ROBIN 5

const char *const schedulingModeVector[] = {
    "FCFS",
    "Non-Preemptive SJF",
    "Preemptive SJF",
    "Non-Preemptive Priority",
    "Preemptive Priority",
    "Round Robin"
};

#define MIN_TIME_QUANTUM 1
int min_time_quantum = MIN_TIME_QUANTUM;
#define MAX_TIME_QUANTUM 50
int max_time_quantum = 10;

#define ASCENDING 1
#define NO_PRIORITY 0
#define DESCENDING -1

#define MIN_NUM_OF_PROCESSES 1
int min_num_of_processes = MIN_NUM_OF_PROCESSES;
#define MAX_NUM_OF_PROCESSES 20
int max_num_of_processes = 5;

#define MIN_PID 0
int min_pid = MIN_PID;
#define MAX_PID 9999
int max_pid = MAX_PID;
#define MIN_ARRIVAL 0
int min_arrival = MIN_ARRIVAL;
#define MAX_ARRIVAL 1000
int max_arrival = 100;
#define MIN_PRIORITY 0
int min_priority = MIN_PRIORITY;
#define MAX_PRIORITY MAX_NUM_OF_PROCESSES
int max_priority = 5;
#define MIN_CPU_BURST 1
int min_cpu_burst = MIN_CPU_BURST;
#define MAX_CPU_BURST 100
int max_cpu_burst = 30;
#define MAX_IO_BURST_NUM (MAX_CPU_BURST - 1)
#define MIN_IO_BURST 1
int min_io_burst = MIN_IO_BURST;
#define MAX_IO_BURST 100
int max_io_burst = 30;

#define INCLUDE_HEADER true
#define NO_HEADER false
#define INCLUDE_LAST_NEWLINE true
#define NO_LAST_NEWLINE false

#define ALLOW_NEWLINE true
#define NO_NEWLINE false

#define MIN_GANTT_CHART_WIDTH 0
#define MAX_GANTT_CHART_WIDTH 1000
int gantt_chart_width = 80;
bool allow_redundancy = true;

typedef struct Node {
    struct Node *prev, *nxt;
    void *data;
} Node;

typedef struct List {
    Node *front, *rear;
    int size;
} List;

List list_init();
Node* list_index(const List, const int);
bool list_empty(const List);
void list_push(List *const, void *const);
void list_pop(List *const, Node *const);

typedef struct CPUBurst {
    int burst, remaining;
} CPUBurst;

typedef struct IO {
    int prevCPUburst, burst, remaining1, remaining2; // remaining1 for prevCPUburst and remaining2 for burst, used in simulate()
} IO;

typedef struct IOBurst {
    int num, idx; 
    IO io[MAX_IO_BURST_NUM];
} IOBurst;

typedef struct Process {
    int number, PID, arrival, priority, lastAddedToReadyQueue, lastRemovedFromCPU, turnaroundTime, waitingTime;
    CPUBurst CPUburst;
    IOBurst IOburst;
} Process;

typedef struct ProcessData {
    int idx;
    List processList;
} ProcessData;

typedef struct GanttData {
    int start, end, number;
} GanttData;

typedef struct SchedulingPolicy {
    int schedulingMode, timeQuantum, priorityDirection;
    bool isPreemptiveMode, isTimeoutMode;
} SchedulingPolicy;

typedef struct Simulation {
    ProcessData processData;
    List GanttList;
    SchedulingPolicy schedulingPolicy;
} Simulation;

typedef struct tm tm_t;

typedef struct SimulationBatch {
    ProcessData processData;
    List simulationList;
    tm_t tm;
    bool usedSchedulingMode[NUM_OF_SCHEDULING_ALGS];
} SimulationBatch;

typedef struct ReadyQueueNode {
    struct ReadyQueueNode *left, *right, *parent;
    Process *process;
} ReadyQueueNode;

// the ready queue is implemented by a binary search tree based priority queue
typedef struct ReadyQueue {
    ReadyQueueNode *root;
    SchedulingPolicy schedulingPolicy;
} ReadyQueue;

ReadyQueue readyQueue_init(const SchedulingPolicy schedulingPolicy);
bool readyQueue_empty(const ReadyQueue);
ReadyQueueNode* readyQueue_front(const ReadyQueue);
bool readyQueue_compare(const ReadyQueue, const Process *const, const Process *const);
void readyQueue_push(ReadyQueue *const, Process *const, const int);
Process* readyQueue_pop(ReadyQueue *const);

int inputString(const char *const, char *const);
char inputChar(const char *const, const bool);
bool inputYesNo(const char *const);
int inputNaturalNumber(const char *const, const bool);

int randRange(const int, const int);
bool randTrueFalse();

SchedulingPolicy inputSchedulingPolicy(bool[]);
int inputNumOfProcesses();

int randProcessPID(bool[]);
int randProcessArrival();
int randProcessPriority();
CPUBurst randProcessCPUburst();
IOBurst randProcessIOburst(const CPUBurst);
Process* inputProcess(int, bool[]);
ProcessData* inputProcessData();

ProcessData* selectProcessData(List *const);

void simulate(Simulation*);

SimulationBatch* selectSimulationBatch(const List);

void showProcessInfo(const Process, const bool, FILE *const);
void showProcessesInfo(const List, const bool, const bool, FILE *const);
int intDigits(const int);
void printCharSequence(const char, const int, FILE *const);
void showGanttChart(const List, const bool, const bool, FILE *const);
void showEvaluation(const List, FILE *const);
void showOverallEvaluation(const List simulationList, FILE *const);

void showSimulationBatchResult(const SimulationBatch, FILE *const);

void inputConfiguration(const char *const, const int, int*, const int, int*);
void configuration();

int main() {
    List processDataList = list_init();
    List simulationBatchList = list_init();

    srand(time(NULL));
    
    printf("CPU Scheduling Simulator\n");
    printf("designed by Jihyeok Choi\n");

    while (true) {
        const bool isSimulationAmountExceeded = simulationBatchList.size >= MAX_NUM_OF_SIMULATIONS; // to avoid excessive amount of simulation results
        int selection;

        printf("\n");
        printf("#----- CPU Scheduling Simulator -----#\n");
        printf("|                                    |\n");
        printf("|      1. Simulate%s         |\n", isSimulationAmountExceeded ? "(disabled)" : "          ");
        printf("|      2. Save Simulation            |\n");
        printf("|      3. Load Simulation            |\n");
        printf("|      4. Configuration              |\n");
        printf("|      5. Exit                       |\n");
        printf("|                                    |\n");
        printf("#------------------------------------#\n");

        while (true) {
            const int input = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

            if (input >= 1 && input <= 5) {
                selection = input;
                break;
            }
        }
        printf("\n");

        // conducts a simulation
        if (selection == 1) {
            if (isSimulationAmountExceeded) printf("* the amount of simulation results has exceeded its limitation(fixed to %d)!\n", MAX_NUM_OF_SIMULATIONS);
            else {
                const ProcessData *const processData = selectProcessData(&processDataList);

                if (processData != NULL) {
                    SimulationBatch *const simulationBatchPtr = (SimulationBatch*)malloc(sizeof(SimulationBatch));
                    const time_t t = time(NULL);

                    *simulationBatchPtr = (SimulationBatch){
                        .processData = *processData,
                        .simulationList = list_init(),
                        .tm = *localtime(&t)
                    };
                    memset(simulationBatchPtr->usedSchedulingMode, false, sizeof(simulationBatchPtr->usedSchedulingMode));

                    while (true) {
                        Simulation *const simulationPtr = (Simulation*)malloc(sizeof(Simulation));
                        
                        *simulationPtr = (Simulation){
                            .processData = (ProcessData){
                                .processList = list_init()
                            },
                            .GanttList = list_init(),
                            .schedulingPolicy = inputSchedulingPolicy(simulationBatchPtr->usedSchedulingMode) // the user inputs a scheduling mode
                        };

                        // copies simulationBatchPtr->processData to simulationPtr->processData
                        for (Node *node = simulationBatchPtr->processData.processList.front; node != NULL; node = node->nxt) {
                            Process *const process = (Process*)malloc(sizeof(Process));
                            
                            *process = *((Process*)(node->data));
                            list_push(&(simulationPtr->processData.processList), process);
                        }

                        simulate(simulationPtr); // simulates with the selected process set and the scheduling mode
                        list_push(&(simulationBatchPtr->simulationList), simulationPtr);

                        bool flag = false;

                        for (int i = 0; i < NUM_OF_SCHEDULING_ALGS; i++) if (!simulationBatchPtr->usedSchedulingMode[i]) flag = true;
                        if (flag) {
                            char selection;

                            printf("* input y to simulate again for other scheduling modes(otherwise, input n)\n");
                            if (!inputYesNo("your input: ")) break;
                        }
                        else {
                            printf("* all scheduling modes were used!\n");
                            break;
                        }
                    }
                    list_push(&simulationBatchList, simulationBatchPtr);
                    printf("\n");
                    showSimulationBatchResult(*simulationBatchPtr, stdout); // shows the simulation result
                }
            }
        }
        // saves a simulation result
        else if (selection == 2) {
            if (list_empty(simulationBatchList)) {
                printf("* you have to conduct a simulation first!\n");
                printf("\n");
            }
            else {
                printf("< save simulation >\n");

                const SimulationBatch *const simulationBatch = selectSimulationBatch(simulationBatchList);

                if (simulationBatch != NULL) {
                    char filename[25];
                    const tm_t tm = simulationBatch->tm;

                    sprintf(filename, "save-%04d%02d%02d-%02d%02d%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                    FILE *tmp = fopen(filename, "r");

                    bool flag = false;

                    if (tmp != NULL) { // if there is an existing file with the same name, asks the user whether to overwrite or not
                        fclose(tmp);
                        printf("* input y to overwrite the existing file(otherwise, input n)\n");
                        if (!inputYesNo("your input: ")) flag = true;
                    }

                    if (!flag) {
                        FILE *fp = fopen(filename, "w");

                        showSimulationBatchResult(*simulationBatch, fp);
                        fclose(fp);
                    }
                    else printf("\n");
                }
            }
        }
        // loads a simulation result
        else if (selection == 3) {
            if (list_empty(simulationBatchList)) {
                printf("* you have to conduct a simulation first!\n");
                printf("\n");
            }
            else {
                printf("< load simulation >\n");

                const SimulationBatch *const simulationBatch = selectSimulationBatch(simulationBatchList);

                if (simulationBatch != NULL) showSimulationBatchResult(*simulationBatch, stdout);
            }
        }
        // the user configures parameters
        else if (selection == 4) configuration();
        // terminates the process
        else if (selection == 5) {
            printf("goodbye!\n");
            exit(0);
        }
    }
}

// returns an initialized list
List list_init() {
    return (List){
        .front = NULL,
        .rear = NULL,
        .size = 0
    };
}

Node* list_index(const List list, const int idx) {
    if (idx < 0 || idx >= list.size) {
        fprintf(stderr, "ERROR: INVALID INDEX\n");
        exit(1);
    }
    else {
        int i = 0;

        for (Node *node = list.front; node != NULL; node = node->nxt) if (i++ == idx) return node;
    }
}

bool list_empty(const List list) {
    return list.size == 0;
}

void list_push(List *const list, void *const data) {
    Node *const tmp = (Node*)malloc(sizeof(Node));

    tmp->data = data;
    tmp->prev = list->rear;
    tmp->nxt = NULL;
    if (list_empty(*list)) list->front = tmp;
    else list->rear->nxt = tmp;
    list->rear = tmp;
    ++(list->size);
}

void list_pop(List *const list, Node *const node) {
    if (list_empty(*list)) {
        fprintf(stderr, "ERROR: ATTEMPTED TO POP AN EMPTY LIST\n");
        exit(1);
    }
    else {
        Node *const tmp = node;

        if (tmp->prev == NULL) list->front = tmp->nxt;
        else tmp->prev->nxt = tmp->nxt;
        if (tmp->nxt == NULL) list->rear = tmp->prev;
        else tmp->nxt->prev = tmp->prev;
        free(tmp);
        --(list->size);
    }
}

// returns an initialized ready queue
ReadyQueue readyQueue_init(const SchedulingPolicy schedulingPolicy) {
    return (ReadyQueue){
        .root = NULL,
        .schedulingPolicy = schedulingPolicy
    };
}

bool readyQueue_empty(const ReadyQueue readyQueue) {
    return readyQueue.root == NULL;
}

// returns the leftmost node
ReadyQueueNode* readyQueue_front(const ReadyQueue readyQueue) {
    for (ReadyQueueNode *node = readyQueue.root; node != NULL; node = node->left) if (node->left == NULL) return node;
    return NULL;
}

// returns true if p1 has higher priority than p2, false otherwise
bool readyQueue_compare(const ReadyQueue readyQueue, const Process *const p1, const Process *const p2) {
    const SchedulingPolicy schedulingPolicy = readyQueue.schedulingPolicy;

    switch (schedulingPolicy.schedulingMode) {
        case FCFS:
            if (p1->arrival != p2->arrival) return p1->arrival < p2->arrival;
            break;
        case NON_PREEMPTIVE_SJF:
        case PREEMPTIVE_SJF:
            if (p1->CPUburst.remaining != p2->CPUburst.remaining) return p1->CPUburst.remaining < p2->CPUburst.remaining;
            break;
        case NON_PREEMPTIVE_PRIORITY:
        case PREEMPRIVE_PRIORITY:
            if (p1->priority != p2->priority) return schedulingPolicy.priorityDirection == ASCENDING ? p1->priority < p2->priority : p1->priority > p2->priority;
            break;
        case ROUND_ROBIN:
            if (p1->lastAddedToReadyQueue == p2->lastAddedToReadyQueue) {
                if (p1->lastAddedToReadyQueue == p1->lastRemovedFromCPU && p2->lastAddedToReadyQueue == p2->lastRemovedFromCPU) break;
                else if (p1->lastAddedToReadyQueue == p1->lastRemovedFromCPU) return false;
                else if (p2->lastAddedToReadyQueue == p2->lastRemovedFromCPU) return true;
                else break;
            }
            return p1->lastAddedToReadyQueue < p2->lastAddedToReadyQueue;
        default:
            fprintf(stderr, "ERROR: UNDEFINED SCHEDULING MODE DETECTED IN COMPARING\n");
            exit(1);
    }

    // tie breaker
    if (p1->arrival == p2->arrival) return p1->number < p2->number;
    return p1->arrival < p2->arrival;
}

void readyQueue_push(ReadyQueue *const readyQueue, Process *const process, const int t) {
    ReadyQueueNode *const tmp = (ReadyQueueNode*)malloc(sizeof(ReadyQueueNode));

    process->lastAddedToReadyQueue = t;
    tmp->process = process;
    tmp->left = tmp->right = tmp->parent = NULL;
    if (readyQueue_empty(*readyQueue)) readyQueue->root = tmp;
    else {
        for (ReadyQueueNode *node = readyQueue->root;;) {
            if (readyQueue_compare(*readyQueue, tmp->process, node->process)) {
                if (node->left == NULL) {
                    node->left = tmp;
                    tmp->parent = node;
                    break;
                }
                node = node->left;
            }
            else {
                if (node->right == NULL) {
                    node->right = tmp;
                    tmp->parent = node;
                    break;
                }
                node = node->right;
            }
        }
    }
}

// removes leftmost node in the binary search tree and returns a pointer of the corresponding process
Process* readyQueue_pop(ReadyQueue *const readyQueue) {
    if (readyQueue_empty(*readyQueue)) {
        fprintf(stderr, "ERROR: ATTEMPTED TO POP AN EMPTY READY QUEUE\n");
        exit(1);
    }
    else {
        for (ReadyQueueNode *node = readyQueue->root; node != NULL; node = node->left) {
            if (node->left == NULL) {
                Process *ret = node->process;

                if (node == readyQueue->root) {
                    if (node->right != NULL) node->right->parent = NULL;
                    readyQueue->root = node->right;
                }
                else {
                    if (node->right != NULL) node->right->parent = node->parent;
                    node->parent->left = node->right;
                }
                free(node);
                return ret;
            }
        }
    }
}

// returns -1 if the input size is too long, 0 for an empty string(newline), 1 otherwise
int inputString(const char *const prompt, char *const dest) {
    char input[MAX_STRING_LEN + 1], *tmp;

    printf("%s", prompt);
    fgets(input, MAX_STRING_LEN + 1, stdin);
    if (strlen(input) == MAX_STRING_LEN && input[MAX_STRING_LEN - 1] != '\n') {
        int cnt = 0;
        while (getchar() != '\n') ++cnt;
        if (cnt > 0) {
            fprintf(stderr, "WARNING: MAXIMUM INPUT STRING LENGTH REACHED\n");
            strcpy(dest, "\0");
            return -1;
        }
    }
    if (strcmp(input, "\n") == 0) {
        *dest = '\0';
        return 0;
    }

    // lstrip
    tmp = input;
    while (*tmp != '\0' && isspace(*tmp)) ++tmp;
    for (int i = 0;; i++, tmp++) {
        *(input + i) = *tmp;
        if (*tmp == '\0') break;
    }

    // rstrip
    tmp = input + strlen(input) - 1;
    while (tmp != input && isspace(*tmp)) --tmp;
    *(tmp + 1) = '\0';

    strcpy(dest, input);
    return 1;
}

// returns the input character or '\0' for an empty string(newline)
char inputChar(const char *const prompt, const bool allowEmptyInput) {
    char input[MAX_STRING_LEN + 1], ret = '\0';

    while (true) {
        if (inputString(prompt, input) == 0 && allowEmptyInput) break; // not used in this code
        if (strlen(input) == 1) {
            ret = *input;
            break;
        }
    }
    return ret;
}

// returns true if the input character is either 'y' or 'Y', false if either 'n' or 'N'
bool inputYesNo(const char *const prompt) {
    bool ret;

    while (true) {
        const char input = inputChar(prompt, NO_EMPTY_INPUT);
        
        if (input == 'Y' || input == 'y') {
            ret = true;
            break;
        }
        else if (input == 'N' || input == 'n') {
            ret = false;
            break;
        }
    }
    return ret;
}

// returns the input natural number(a nonnegative whole number), -1 for an empty string(newline)
int inputNaturalNumber(const char *const prompt, const bool allowEmptyInput) {
    char input[MAX_STRING_LEN + 1];
    int ret = -1;

    while (true) {
        if (inputString(prompt, input) == 0 && allowEmptyInput) break;
        if (strlen(input) == 0 || strlen(input) > 1 && input[0] == '0') continue;

        bool flag = false;
        int tmp = 0;

        for (int i = 0; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                flag = true;
                break;
            }
            tmp = tmp * 10 + input[i] - '0';
        }
        if (!flag) {
            ret = tmp;
            break;
        }
    }
    return ret;
}

// returns a random natural number in [low, high]
int randRange(const int low, const int high) {
    if (low > high || low < 0) {
        fprintf(stderr, "ERROR: INVALID ARGUMENT IN RANDRANGE FUNCTION\n");
        exit(1);
    }
    return rand() % (high - low + 1) + low;
}

// returns either true or false randomly
bool randTrueFalse() {
    return randRange(0, 1) == 0;
}

SchedulingPolicy inputSchedulingPolicy(bool usedSchedulingMode[]) {
    SchedulingPolicy ret = (SchedulingPolicy){
        .priorityDirection = NO_PRIORITY,
        .timeQuantum = -1
    };
    
    printf("< scheduling mode >\n");
    for (int i = 0; i < NUM_OF_SCHEDULING_ALGS; i++) printf("[%d] %s%s\n", i + 1, schedulingModeVector[i], usedSchedulingMode[i] ? "(unavailable)" : "");
    printf("* press enter for a random choice\n");

    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            while (usedSchedulingMode[ret.schedulingMode = randRange(0, NUM_OF_SCHEDULING_ALGS - 1)]);
            printf("selected scheduling mode: %s\n", schedulingModeVector[ret.schedulingMode]);
            break;
        }
        else if (input >= 1 && input <= NUM_OF_SCHEDULING_ALGS) {
            if (usedSchedulingMode[input - 1]) printf("* this scheduling mode has already been used!\n");
            else {
                ret.schedulingMode = input - 1;
                break;
            }
        }
    }
    usedSchedulingMode[ret.schedulingMode] = true;
    ret.isPreemptiveMode = ret.schedulingMode == PREEMPRIVE_PRIORITY || ret.schedulingMode == PREEMPTIVE_SJF;
    ret.isTimeoutMode = ret.schedulingMode == ROUND_ROBIN;
    printf("\n");

    if (ret.schedulingMode == NON_PREEMPTIVE_PRIORITY || ret.schedulingMode == PREEMPRIVE_PRIORITY) {
        printf("< priority direction >\n");
        printf("[1] ascending order(lower numbers with higher priorities)\n");
        printf("[2] descending order(higher numbers with hight priorities)\n");
        printf("* press enter for a random choice\n");
        while (true) {
            const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

            if (input == -1) {
                ret.priorityDirection = randTrueFalse() ? ASCENDING : DESCENDING;
                printf("selected direction: %s\n", ret.priorityDirection == ASCENDING ? "ascending order" : "descending order");
                break;
            }
            else if (input == 1) {
                ret.priorityDirection = ASCENDING;
                break;
            }
            else if (input == 2) {
                ret.priorityDirection = DESCENDING;
                break;
            }
        }
        printf("\n");
    }
    else if (ret.schedulingMode == ROUND_ROBIN) {
        printf("< time quantum >\n");
        printf("* valid input range: %d-%d\n", min_time_quantum, max_time_quantum);
        printf("* press enter for a random choice\n");
        while (true) {
            const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

            if (input == -1) {
                ret.timeQuantum = randRange(min_time_quantum, max_time_quantum);
                printf("selected time quantum: %d\n", ret.timeQuantum);
                break;
            }
            else if (input >= min_time_quantum && input <= max_time_quantum) {
                ret.timeQuantum = input;
                break;
            }
        }
        printf("\n");
    }

    return ret;
}

int inputNumOfProcesses() {
    int ret;

    printf("< number of processes to simulate >\n");
    printf("* valid input range: %d-%d\n", min_num_of_processes, max_num_of_processes);
    printf("* press enter for a random choice\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            ret = randRange(min_num_of_processes, max_num_of_processes);
            printf("selected number: %d\n", ret);
            break;
        }
        else if (input >= min_num_of_processes && input <= max_num_of_processes) {
            ret = input;
            break;
        }
    }
    printf("\n");
    return ret;
}

int randProcessPID(bool isPIDoccupied[]) {
    int ret;

    while (isPIDoccupied[ret = randRange(min_pid, max_pid)]);
    return ret;
}

int randProcessArrival() {
    return randRange(min_arrival, max_arrival);
}

int randProcessPriority() {
    return randRange(min_priority, max_priority);
}

CPUBurst randProcessCPUburst() {
    const int burst = randRange(min_cpu_burst, max_cpu_burst);

    return (CPUBurst){
        .burst = burst,
        .remaining = burst
    };
}

IOBurst randProcessIOburst(const CPUBurst CPUburst) {
    IOBurst ret = (IOBurst){
        .num = 0,
        .idx = 0
    };

    if (CPUburst.burst > 1) {
        for (int t = 0; t <= CPUburst.burst - 2;) {
            if (randTrueFalse()) break; // randomly chooses whether or not to add an IO
            else {
                const int prevCPUburst = randRange(1, CPUburst.burst - t - 1), burst = randRange(min_io_burst, max_io_burst);

                ret.io[ret.num++] = (IO){
                    .prevCPUburst = prevCPUburst,
                    .burst = burst,
                    .remaining1 = prevCPUburst,
                    .remaining2 = burst 
                };
                t += prevCPUburst;
            }
        }
    }
    return ret;
}

// configures a single process
Process* inputProcess(const int number, bool isPIDoccupied[]) {
    Process *const ret = (Process*)malloc(sizeof(Process));

    ret->number = number;
    ret->lastAddedToReadyQueue = ret->lastRemovedFromCPU = ret->turnaroundTime = ret->waitingTime = -1;
    printf("< P%d's data options >\n", ret->number);
    printf("[1] input manually\n");
    printf("[2] input randomly\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

        if (input == 1) break;
        else if (input == 2) {
            ret->PID = randProcessPID(isPIDoccupied);
            ret->arrival = randProcessArrival();
            ret->priority = randProcessPriority();
            ret->CPUburst = randProcessCPUburst();
            ret->IOburst = randProcessIOburst(ret->CPUburst);
            printf("\n");
            return ret;
        }
    }
    printf("\n");

    // the user inputs a PID
    printf("< PID >\n");
    printf("* valid input range: %d-%d\n", min_pid, max_pid);
    printf("* press enter for a random choice\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            printf("selected PID: %d\n", ret->PID = randProcessPID(isPIDoccupied));
            break;
        }
        else if (input >= min_pid && input <= max_pid) {
            if (isPIDoccupied[input]) {
                printf("* the PID is occupied for another process!\n");
                continue;
            }
            else {
                ret->PID = input;
                break;
            }
        }
    }
    isPIDoccupied[ret->PID] = true;
    printf("\n");

    // the user input an arrival
    printf("< arrival >\n");
    printf("* valid input range: %d-%d\n", min_arrival, max_arrival);
    printf("* press enter for a random choice\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            printf("selected arrival: %d\n", ret->arrival = randProcessArrival());
            break;
        }
        else if (input >= min_arrival && input <= max_arrival) {
            ret->arrival = input;
            break;
        }
    }
    printf("\n");

    // the user iuputs a priority
    printf("< priority >\n");
    printf("* valid input range: %d-%d\n", min_priority, max_priority);
    printf("* press enter for a random choice\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            printf("selected priority: %d\n", ret->priority = randProcessPriority());
            break;
        }
        else if (input >= min_priority && input <= max_priority) {
            ret->priority = input;
            break;
        }
    }
    printf("\n");

    // the user inputs a CPU-burst
    printf("< CPU-burst >\n");
    printf("* valid input range: %d-%d\n", min_cpu_burst, max_cpu_burst);
    printf("* press enter for a random choice\n");
    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            printf("selected CPU-burst: %d\n", (ret->CPUburst = randProcessCPUburst()).burst);
            break;
        }
        if (input >= min_cpu_burst && input <= max_cpu_burst) {
            ret->CPUburst.remaining = ret->CPUburst.burst = input;
            break;
        }
    }
    printf("\n");

    // the user inputs an IO-burst set
    if (ret->CPUburst.burst > 1) {
        printf("< IO-burst options >\n");
        printf("[0] no IO-burst\n");
        printf("[1] input manually\n");
        printf("[2] input randomly\n");
        while (true) {
            const int input = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

            if (input == 0) {
                ret->IOburst.num = ret->IOburst.idx = 0;
                break;
            }
            else if (input == 1) {
                printf("\n");
                ret->IOburst.num = ret->IOburst.idx = 0;
                for (int t = 0;;) {
                    if (t != 0) {
                        char selection;

                        printf("* input y to add IO-burst(otherwise, input n)\n");
                        if (!inputYesNo("your input: ")) break;
                        printf("\n");
                    }

                    printf("< IO #%d's prior CPU-burst >\n", ret->IOburst.num + 1);
                    printf("* valid input range: %d-%d\n", 1, ret->CPUburst.burst - t - 1);
                    while (true) {
                        const int input2 = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

                        if (input2 >= 1 && input2 <= ret->CPUburst.burst - t - 1) {
                            ret->IOburst.io[ret->IOburst.num].prevCPUburst = ret->IOburst.io[ret->IOburst.num].remaining1 = input2;
                            t += input2;
                            break;
                        }
                        else if (input2 == 0) printf("* prior CPU-burst should not be zero!\n");
                        else printf("* prior CPU-burst should not exceed the total CPU-burst time!\n");
                    }
                    printf("\n");

                    printf("< IO #%d's IO-burst >\n", ret->IOburst.num + 1);
                    printf("* valid input range: %d-%d\n", min_io_burst, max_io_burst);
                    while (true) {
                        const int input2 = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

                        if (input2 >= min_io_burst && input2 <= max_io_burst) {
                            ret->IOburst.io[ret->IOburst.num].burst = ret->IOburst.io[ret->IOburst.num].remaining2 = input2;
                            break;
                        }
                    }
                    ++(ret->IOburst.num);
                    if (t >= ret->CPUburst.burst - 1) {
                        printf("* terminated IO-burst input since no more IOs are possible\n");
                        break;
                    }
                    printf("\n");
                }
                break;
            }
            else if (input == 2) {
                ret->IOburst = randProcessIOburst(ret->CPUburst);
                break;
            }
        }
        printf("\n");
    }

    return ret;
}

// configures a set of processes
ProcessData* inputProcessData() {
    ProcessData *const ret = (ProcessData*)malloc(sizeof(ProcessData));

    *ret = (ProcessData){
        .processList = list_init()
    };

    const int numOfProcesses = inputNumOfProcesses();
    bool isPIDoccupied[MAX_PID + 1] = { false };

    for (int i = 1; i <= numOfProcesses; i++) {
        Process *const process = inputProcess(i, isPIDoccupied);

        list_push(&(ret->processList), process);
        showProcessInfo(*process, INCLUDE_HEADER, stdout);
    }
    return ret;
}

// show a list of process sets and gets an input from the user
ProcessData* selectProcessData(List *const processDataList) {
    ProcessData *ret = NULL;

    printf("< process data >\n");
    printf("[0] home\n");

    int i = 0;

    for (Node* node = processDataList->front; node != NULL; node = node->nxt) {
        const List processList = ((ProcessData*)(node->data))->processList;

        printf("[%d] %d process%s\n", ++i, processList.size, processList.size == 1 ? "" : "es");
        showProcessesInfo(processList, NO_HEADER, NO_LAST_NEWLINE, stdout);
    }
    printf("* press enter to create new process data\n");

    while (true) {
        const int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        // creates a new process data
        if (input == -1) {
            printf("\n");
            list_push(processDataList, ret = inputProcessData());
            ((ProcessData*)(processDataList->rear->data))->idx = processDataList->size - 1;
            break;
        }
        // goes back to home
        else if (input == 0) break;
        // selects an existing process data
        else if (input >= 1 && input <= processDataList->size) {
            ret = (ProcessData*)(list_index(*processDataList, input - 1)->data);
            break;
        }
    }
    printf("\n");
    return ret;
}

void simulate(Simulation *ret) {
    const SchedulingPolicy schedulingPolicy = ret->schedulingPolicy;

    List uncompletedProcessList = list_init();

    for (Node *node = ret->processData.processList.front; node != NULL; node = node->nxt) list_push(&uncompletedProcessList, (Process*)(node->data));

    ReadyQueue readyQueue = readyQueue_init(schedulingPolicy);
    List waitingQueue = list_init();
    Process *prev = NULL, *running = NULL; // running indicates the process running on CPU, and prev for the previous running

    // simulates by incrementing t by one until every process is terminated
    for (int t = 0, timer = schedulingPolicy.timeQuantum; !list_empty(uncompletedProcessList); t++) {
        // checks waiting queue
        for (Node *node = waitingQueue.front; node != NULL;) {
            Process *process = (Process*)(node->data);

            if ((process->IOburst.io[process->IOburst.idx].remaining2)-- == 0) {
                Node *tmp = node->nxt;

                ++(process->IOburst.idx);
                readyQueue_push(&readyQueue, process, t);
                list_pop(&waitingQueue, node);
                // printf("t=%d: P%d is added to ready queue from waiting queue\n", t, process->number);
                node = tmp;
            }
            else node = node->nxt;
        }
        
        // checks if there is a process where its arrival equals to t
        for (Node *node = uncompletedProcessList.front; node != NULL; node = node->nxt) { 
            Process *process = (Process*)(node->data);

            if (t == process->arrival) {
                readyQueue_push(&readyQueue, process, t);
                // printf("t=%d: P%d is added to ready queue\n", t, process->number);
            }
        }

        // checks ready queue
        if (readyQueue.root != NULL) {
            // if there is no process running on CPU
            if (running == NULL) {
                running = readyQueue_pop(&readyQueue);
                // printf("t=%d: P%d is running on CPU\n", t, running->number);
            }
            // if there is a process running on CPU and preemptive mode is activated
            else if (schedulingPolicy.isPreemptiveMode) {
                // checks preemption
                Process *const tmp = readyQueue_front(readyQueue)->process;

                if (readyQueue_compare(readyQueue, tmp, running)) {
                    readyQueue_push(&readyQueue, running, t);
                    running->lastRemovedFromCPU = t;
                    // printf("t=%d: P%d is preempted by %d\n", t, running->number, tmp->number);
                    running = readyQueue_pop(&readyQueue);
                }
            }
        }

        // generates a segment of the Gantt chart
        if (running != prev || t == 0 && running == NULL || schedulingPolicy.isTimeoutMode && running != NULL && running == prev && timer == schedulingPolicy.timeQuantum) {
            if (!list_empty(ret->GanttList) && ((GanttData*)(ret->GanttList.rear->data))->end == -1) ((GanttData*)(ret->GanttList.rear->data))->end = t; // involutary context switching
            
            GanttData *const tmp = (GanttData*)malloc(sizeof(GanttData));

            tmp->number = running == NULL ? -1 : running->number;
            tmp->start = t;
            tmp->end = -1; // the ending time is unknown yet
            list_push(&(ret->GanttList), tmp);
        }
        prev = running;

        // checks a process running on CPU
        if (running != NULL) {
            // decrements the remaining value of CPU-burst and at the same time checks if completed
            if (--(running->CPUburst.remaining) == 0) {
                for (Node *node = uncompletedProcessList.front; node != NULL; node = node->nxt) {
                    if (running == (Process*)(node->data)) {
                        // printf("t=%d: P%d is terminated\n", t + 1, running->number); // since it is completed at the end of the time unit, 1 is added
                        list_pop(&uncompletedProcessList, node);
                        break;
                    }
                }
                running->lastRemovedFromCPU = t + 1;
                running->turnaroundTime = t + 1 - running->arrival;
                running->waitingTime = t + 1 - running->arrival - running->CPUburst.burst;
                for (int i = 0; i < running->IOburst.num; i++) running->waitingTime -= running->IOburst.io[i].burst;
                ((GanttData*)(ret->GanttList.rear->data))->end = t + 1; // assign the ending time
                running = NULL;
            }
            // if the process is still running on CPU, checks if the process needs to be relocated to the waiting queue
            else if (running->IOburst.num > 0 && running->IOburst.idx < running->IOburst.num) {
                if (--(running->IOburst.io[running->IOburst.idx].remaining1) == 0) {
                    list_push(&waitingQueue, running);
                    running->lastRemovedFromCPU = t + 1;
                    // printf("t=%d: P%d is added to waiting queue from CPU\n", t + 1, running->number);
                    ((GanttData*)(ret->GanttList.rear->data))->end = t + 1; // assign the ending time
                    running = NULL;
                }
            }
            // checks timeout
            if (running == NULL) timer = schedulingPolicy.timeQuantum;
            else {
                if (schedulingPolicy.isTimeoutMode && --timer == 0) {
                    readyQueue_push(&readyQueue, running, t + 1);
                    running->lastRemovedFromCPU = t + 1;
                    // printf("t=%d: P%d is timed out\n", t + 1, running->number);
                    running = NULL;
                    timer = schedulingPolicy.timeQuantum;
                }
            }
        }
    }
    printf("simulation completed\n");
    printf("\n");
}

// shows a list of simulation batches and gets an input from the user
SimulationBatch* selectSimulationBatch(const List simulationBatchList) {
    SimulationBatch *ret = NULL;

    printf("[0] Home\n");

    int idx = 0;

    for (Node *node = simulationBatchList.front; node != NULL; node = node->nxt) {
        const SimulationBatch simulationBatch = *((SimulationBatch*)(node->data));
        const tm_t tm = simulationBatch.tm;
        
        printf("[%d] %04d/%02d/%02d %02d:%02d:%02d - ", ++idx, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("process set %d + ", simulationBatch.processData.idx + 1);
        for (Node *node2 = simulationBatch.simulationList.front; node2 != NULL; node2 = node2->nxt) {
            const SchedulingPolicy schedulingPolicy = ((Simulation*)(node2->data))->schedulingPolicy;
            const bool isPriorityMode = schedulingPolicy.schedulingMode == NON_PREEMPTIVE_PRIORITY || schedulingPolicy.schedulingMode == PREEMPRIVE_PRIORITY;
            const bool isRearNode = node2 == simulationBatch.simulationList.rear;

            printf("%s%s%s", schedulingModeVector[schedulingPolicy.schedulingMode], isPriorityMode ? (schedulingPolicy.priorityDirection == ASCENDING ? "(ascending order)" : "(descending order)") : "", isRearNode ? "" : "/");
        }
        printf("\n");
    }

    while (true) {
        const int input = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);
        
        if (input == 0) break; // goes back to home
        else if (input > 0 && input <= simulationBatchList.size) { // selects a simulation batch
            ret = (SimulationBatch*)(list_index(simulationBatchList, input - 1)->data);
            break;
        }
    }
    printf("\n");

    return ret;
}

// returns the digits of a natural number
int intDigits(const int n) {
    int tmp = n, ret = 0;

    if (!n) return 1;
    while (tmp) {
        tmp /= 10;
        ++ret;
    }
    return ret;
}

// shows a single process
void showProcessInfo(const Process process, const bool includeHeader, FILE *const _Stream) {
    if (includeHeader) fprintf(_Stream, "process   PID   arrival   priority   CPU-burst   IO-burst\n");
    fprintf(_Stream, "%*c%d  %4d   %7d   %8d   %9d   ", 7 - intDigits(process.number), 'P', process.number, process.PID, process.arrival, process.priority, process.CPUburst.burst);
    if (process.IOburst.num) {
        for (int i = 0; i < process.IOburst.num; i++) {
            const IO io = process.IOburst.io[i];

            fprintf(_Stream, "(%d, %d)%s", io.prevCPUburst, io.burst, i == process.IOburst.num - 1 ? "" : ", ");
        }
        fprintf(_Stream, "\n");
    }
    else fprintf(_Stream, "none\n");
    if (includeHeader) fprintf(_Stream, "\n");
}

// shows a set of processes
void showProcessesInfo(const List processList, const bool includeHeader, const bool includeLastNewline, FILE *const _Stream) {
    if (includeHeader) fprintf(_Stream, "----- process%s information -----\n", processList.size == 1 ? "" : "es");
    fprintf(_Stream, "process   PID   arrival   priority   CPU-burst   IO-burst\n");
    for (Node *node = processList.front; node != NULL; node = node->nxt) showProcessInfo(*(Process*)(node->data), NO_HEADER, _Stream);
    if (includeLastNewline) fprintf(_Stream, "\n");
}

// prints a sequence of a character
void printCharSequence(const char c, const int n, FILE *const _Stream) {
    for (int i = 1; i <= n; i++) fprintf(_Stream, "%c", c);
}

void showGanttChart(const List GanttList, const bool allowRedundancy, const bool allowNewline, FILE *const _Stream) { // allowNewline for outputs overflowing the width of the window
    Node *begin = GanttList.front, *nextLine = NULL;

    fprintf(_Stream, "* Gantt chart\n");
    while (begin != NULL) {
        int cnt = 0;

        // first row
        for (Node *node = begin; node != NULL && (!allowNewline || allowNewline && cnt + 1 <= gantt_chart_width); nextLine = node = node->nxt) {
            const GanttData *const curGanttData = (GanttData*)(node->data);
            
            fprintf(_Stream, " ");
            ++cnt;

            int space;

            if ((curGanttData->number == -1 ? 3 : intDigits(curGanttData->number) + 3) < intDigits(curGanttData->start)) {
                space = intDigits(curGanttData->start) - (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1);
                space += space % 2; // to align center
            }
            else space = 2;
            printCharSequence(' ', space / 2, _Stream);
            if (curGanttData->number == -1) fprintf(_Stream, " ");
            else fprintf(_Stream, "P%d", curGanttData->number);
            printCharSequence(' ', space / 2, _Stream);
            
            cnt += (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1) + space;

            if (!allowRedundancy) while (node->nxt != NULL && ((GanttData*)(node->data))->number == ((GanttData*)(node->nxt->data))->number) node = node->nxt;
        }
        fprintf(_Stream, "\n");

        // second row
        for (Node *node = begin; node != nextLine; node = node->nxt) {
            const GanttData *const curGanttData = (GanttData*)(node->data);

            if (node == begin) fprintf(_Stream, "|");

            int space;

            if ((curGanttData->number == -1 ? 3 : intDigits(curGanttData->number) + 3) < intDigits(curGanttData->start)) space = intDigits(curGanttData->start) - (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1);
            else space = 2;
            space += space % 2;

            printCharSequence('=', (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1) + space, _Stream);

            if (!allowRedundancy) while (node->nxt != nextLine && ((GanttData*)(node->data))->number == ((GanttData*)(node->nxt->data))->number) node = node->nxt;
            fprintf(_Stream, "|");
        }
        fprintf(_Stream, "\n");

        // third row
        for (Node *node = begin; node != nextLine; node = node->nxt) {
            const GanttData *const curGanttData = (GanttData*)(node->data);

            fprintf(_Stream, "%d", curGanttData->start);

            int space;

            if ((curGanttData->number == -1 ? 3 : intDigits(curGanttData->number) + 3) < intDigits(curGanttData->start)) space = intDigits(curGanttData->start) - (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1);
            else space = 2;
            space += space % 2;

            printCharSequence(' ', (curGanttData->number == -1 ? 1 : intDigits(curGanttData->number) + 1) + space - (intDigits(curGanttData->start) - 1), _Stream);

            if (!allowRedundancy) while (node->nxt != nextLine && ((GanttData*)(node->data))->number == ((GanttData*)(node->nxt->data))->number) node = node->nxt;
            if (node->nxt == nextLine) fprintf(_Stream, "%d", ((GanttData*)(node->data))->end);
        }
        fprintf(_Stream, "\n");
        begin = nextLine;
    }
    fprintf(_Stream, "\n");
}

void showEvaluation(const List processList, FILE *const _Stream) {
    double sumTurnaroundTime = 0, sumWaitingTime = 0;

    fprintf(_Stream, "* evaluation\n");
    fprintf(_Stream, "process   turnaround-time   waiting-time\n");
    for (Node *node = processList.front; node != NULL; node = node->nxt) {
        const Process process = *(Process*)(node->data);

        fprintf(_Stream, "%*c%d   %15d   %12d\n", 7 - intDigits(process.number), 'P', process.number, process.turnaroundTime, process.waitingTime);
        sumTurnaroundTime += process.turnaroundTime;
        sumWaitingTime += process.waitingTime;
    }
    fprintf(_Stream, "%7s%18.3lf%15.3lf\n", "avg", sumTurnaroundTime / processList.size, sumWaitingTime / processList.size);
    fprintf(_Stream, "\n");
}

void showOverallEvaluation(const List simulationList, FILE *const _Stream) {
    fprintf(_Stream, "----- overall evaluation(average) -----\n");

    int maxSchedulingModeNameLen = 0; // for indentation

    for (Node *node = simulationList.front; node != NULL; node = node->nxt) {
        const SchedulingPolicy schedulingPolicy = ((Simulation*)(node->data))->schedulingPolicy;
        const int curSchedulingModeNameLen = strlen(schedulingModeVector[schedulingPolicy.schedulingMode]);

        if (curSchedulingModeNameLen > maxSchedulingModeNameLen) maxSchedulingModeNameLen = curSchedulingModeNameLen;
    }
    fprintf(_Stream, "%*sscheduling mode   turnaround-time   waiting-time\n", maxSchedulingModeNameLen < 15 ? 0 : maxSchedulingModeNameLen - 15, "");

    for (Node *node = simulationList.front; node != NULL; node = node->nxt) {
        const List processList = ((Simulation*)(node->data))->processData.processList;
        double sumTurnaroundTime = 0, sumWaitingTime = 0;

        for (Node *node2 = processList.front; node2 != NULL; node2 = node2->nxt) {
            const Process process = *(Process*)(node2->data);

            sumTurnaroundTime += process.turnaroundTime;
            sumWaitingTime += process.waitingTime;
        }

        const int schedulingMode = ((Simulation*)(node->data))->schedulingPolicy.schedulingMode;

        fprintf(_Stream, "%*s%18.3lf%15.3lf\n", maxSchedulingModeNameLen < 15 ? 15 : maxSchedulingModeNameLen, schedulingModeVector[schedulingMode], sumTurnaroundTime / processList.size, sumWaitingTime / processList.size);
    }
    fprintf(_Stream, "\n");
}

void showSimulationBatchResult(const SimulationBatch simulationBatch, FILE *const _Stream) {
    const tm_t tm = simulationBatch.tm;

    fprintf(_Stream, "%04d/%02d/%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    showProcessesInfo(simulationBatch.processData.processList, INCLUDE_HEADER, INCLUDE_LAST_NEWLINE, _Stream);
    
    int i = 0;

    fprintf(_Stream, "----- simulation result%s -----\n", simulationBatch.simulationList.size == 1 ? "" : "s");
    for (Node *node = simulationBatch.simulationList.front; node != NULL; node = node->nxt) {
        const Simulation simulation = *((Simulation*)(node->data));
        const SchedulingPolicy schedulingPolicy = simulation.schedulingPolicy;
        
        fprintf(_Stream, "%d. %s", ++i, schedulingModeVector[schedulingPolicy.schedulingMode]);
        if (schedulingPolicy.schedulingMode == NON_PREEMPTIVE_PRIORITY || schedulingPolicy.schedulingMode == PREEMPRIVE_PRIORITY) fprintf(_Stream, schedulingPolicy.priorityDirection == ASCENDING ? "(ascending order)" : "(descending order)");
        else if (schedulingPolicy.schedulingMode == ROUND_ROBIN) fprintf(_Stream, "(time quantum = %d)", schedulingPolicy.timeQuantum);
        fprintf(_Stream, "\n");
        showGanttChart(simulation.GanttList, allow_redundancy, _Stream == stdout ? ALLOW_NEWLINE : NO_NEWLINE, _Stream);
        showEvaluation(simulation.processData.processList, _Stream);
    }
    if (simulationBatch.simulationList.size > 1) showOverallEvaluation(simulationBatch.simulationList, _Stream);
}

// configures a value that contains maximum and minimum values
void inputConfiguration(const char *const s, const int MIN, int *minPtr, const int MAX, int *maxPtr) {
    printf("< configuration of %s >\n", s);

    printf("<< min value >>\n");
    printf("* valid input range: %d-%d\n", MIN, MAX);
    printf("* press enter to skip\n");
    while (true) {
        int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) break;
        else if (input >= MIN && input <= MAX) {
            *minPtr = input;
            break;
        }
    }
    printf("\n");
    
    printf("<< max value >>\n");
    printf("* valid input range: %d-%d\n", *minPtr, MAX);
    printf("* press enter to skip\n");
    while (true) {
        int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

        if (input == -1) {
            if (*maxPtr < *minPtr) printf("* max value should not be less than min value!\n");
            else break;
        }
        else if (input >= *minPtr && input <= MAX) {
            *maxPtr = input;
            break;
        }
    }
    printf("\n");
}

void configuration() {
    while (true) {
        int selection;

        printf("< configuration >\n");
        printf("* select a property to configure\n");
        printf("[0] home\n");
        printf("[1] time quantum: %d-%d\n", min_time_quantum, max_time_quantum);
        printf("[2] number of processes: %d-%d\n", min_num_of_processes, max_num_of_processes);
        printf("[3] PID: %d-%d\n", min_pid, max_pid);
        printf("[4] arrival: %d-%d\n", min_arrival, max_arrival);
        printf("[5] priority: %d-%d\n", min_priority, max_priority);
        printf("[6] CPU-burst: %d-%d\n", min_cpu_burst, max_cpu_burst);
        printf("[7] IO-burst: %d-%d\n", min_io_burst, max_io_burst);
        printf("[8] Gantt chart width: %d\n", gantt_chart_width);
        printf("[9] redundancy in gantt chart: %s\n", allow_redundancy ? "true" : "false");

        const bool flag = max_num_of_processes > max_pid - min_pid + 1;

        if (flag) printf("* the number of processes should not exceed the number of possible PIDs!\n");

        while (true) {
            int input = inputNaturalNumber("your input: ", NO_EMPTY_INPUT);

            if (input >= 0 && input <= 9) {
                if (flag && !(input == 2 || input == 3)) {
                    printf("* select [2] or [3] to handle this issue!\n");
                    continue;
                }
                selection = input;
                break;
            }
        }
        if (selection == 0) break;
        else {
            printf("\n");
            if (selection == 1) inputConfiguration("time quantum", MIN_TIME_QUANTUM, &min_time_quantum, MAX_TIME_QUANTUM, &max_time_quantum);
            else if (selection == 2) inputConfiguration("number of processes", MIN_NUM_OF_PROCESSES, &min_num_of_processes, MAX_NUM_OF_PROCESSES, &max_num_of_processes);
            else if (selection == 3) inputConfiguration("PID", MIN_PID, &min_pid, MAX_PID, &max_pid);
            else if (selection == 4) inputConfiguration("arrival", MIN_ARRIVAL, &min_arrival, MAX_ARRIVAL, &max_arrival);
            else if (selection == 5) inputConfiguration("priority", MIN_PRIORITY, &min_priority, MAX_PRIORITY, &max_priority);
            else if (selection == 6) inputConfiguration("CPU-burst", MIN_CPU_BURST, &min_cpu_burst, MAX_CPU_BURST, &max_cpu_burst);
            else if (selection == 7) inputConfiguration("IO-burst", MIN_IO_BURST, &min_io_burst, MAX_IO_BURST, &max_io_burst);
            else if (selection == 8) {
                printf("< configuration of Gantt chart width >\n");
                printf("* if a width of a row exceeds this value after adding a segment from the Gantt chart, remaining segments will be printed in the newline\n");
                printf("* valid input range: %d-%d\n", MIN_GANTT_CHART_WIDTH, MAX_GANTT_CHART_WIDTH);
                printf("* press enter to skip\n");
                while (true) {
                    int input = inputNaturalNumber("your input: ", ALLOW_EMPTY_INPUT);

                    if (input == -1) break;
                    else if (input >= MIN_GANTT_CHART_WIDTH && input <= MAX_GANTT_CHART_WIDTH) {
                        gantt_chart_width = input;
                        break;
                    }
                }
                printf("\n");
            }
            else {
                printf("< configuration of redundancy in Gantt chart >\n");
                printf("* input y to change the current configuration(otherwise, input n)\n");

                if (inputYesNo("your input: ")) allow_redundancy = !allow_redundancy;
            }
        }
    }
    printf("\n");
}