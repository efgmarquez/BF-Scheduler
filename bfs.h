#define BFS_DEFAULT_QUANTUM 100
#define BFS_NICE_FIRST_LEVEL -20
#define BFS_NICE_LAST_LEVEL 19
#define NUM_HEADS 4
#define SEED 918729
#define NULL 0




// priority ratio = nice value + 21 (assuming [-20, 19])
// virtual deadline = current time + (priority ratio x BFS_DEFAULT_QUANTUM)
