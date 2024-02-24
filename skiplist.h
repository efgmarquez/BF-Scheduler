#include "param.h"
#include "bfs.h"

// skiplist.c
void            initNodeTable(void);
void            initSkipList(void);

// skip list node structure
struct node {
  struct node* left;  // left node 
  struct node* right; // right node
  struct node* down;  // down node
  struct proc* data;  // pointer to the process
};

// skip list
struct node* skipList[NUM_HEADS];

// memory for skiplist
struct node nodeTable[256];
struct node nodeHeads[NUM_HEADS];
