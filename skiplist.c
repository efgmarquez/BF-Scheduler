#include "skiplist.h"
#include "proc.h"

// for randomizer
unsigned int seed = SEED;
unsigned int random(int max){
  seed ^= seed << 17;
  seed ^= seed >> 7;
  seed ^= seed << 5;
  return seed % max;
}

void skipListStatus(){
  cprintf("===============================================================================================\n");
  cprintf("[name, addy, left, data, right, down | PID, VD]\n");

  for (int i = NUM_HEADS - 1; i >= 0; i--){
    struct node *current = skipList[i];
    cprintf("Level %d ::\n", i);

    while (current->right != NULL){
      current = current->right;
      cprintf("[%s, %p, %p, %p, %p, %p | %d, %d]\n | \n v\n", 
      current->data->name, current, current->left, current->data, current->right, current->down, current->data->pid, current->data->deadline);
    } 
    
    cprintf("NULL");
    cprintf("\n");
  }
  cprintf("===============================================================================================\n");
  return;
}

void initNodeTable(void){
  // so that fields dont contain garbage values
  for (int nodeID = 0; nodeID < 256; nodeID++){
    nodeTable[nodeID].data = NULL;
    nodeTable[nodeID].down = NULL;
    nodeTable[nodeID].left = NULL;
    nodeTable[nodeID].right = NULL;
  }
  return;
}

// place head nodes inside skiplist's heads array
void initSkipList(void){
  for (int nodeID = 0; nodeID < NUM_HEADS; nodeID++){
    nodeHeads[nodeID].data = NULL;
    nodeHeads[nodeID].down = (nodeID == 0) ? NULL : &nodeHeads[nodeID - 1];
    nodeHeads[nodeID].left = NULL;
    nodeHeads[nodeID].right = NULL;
    skipList[nodeID] = &nodeHeads[nodeID];
  }
  return;
}

struct node *allocateSkipListNode(struct proc *data){
  struct node *newNode = NULL;

  for (int nodeID = 0; nodeID < 256; nodeID++){
    if (nodeTable[nodeID].data == NULL){
      newNode = &nodeTable[nodeID];
      break;
    }
  }

  if (newNode != NULL){
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->down = NULL;
    newNode->data = data;
  }
  return newNode;
}

// handles deletion of a process' node from the skip list
void deleteSkipListNode(struct proc *procToDelete){                      
  int topLevel = NUM_HEADS - 1;
  struct node *currentNode = skipList[topLevel];
  while (1) {            // finds the node of the process to delete
    while (currentNode->right != NULL){
      
      if (currentNode->right->data->deadline < procToDelete->deadline){
        // go right if deadline(right) < deadline(toDelete)
        currentNode = currentNode->right;
      }

      else if (currentNode->right->data->deadline == procToDelete->deadline){ // deadline(toDelete has been found)

        if (currentNode->right->data->pid == procToDelete->pid){ // handles duplicate deadlines
          currentNode = currentNode->right;
          goto found;
        }
        else{
          currentNode = (procToDelete->maxlevel == currentNode->right->data->maxlevel) ? currentNode->right : currentNode->down;
        }
      }
      else{ // handles if deadline(right) > deadline(current)
        currentNode = currentNode->down;
      }
    }

    if (currentNode->down != NULL){ // handles if end of level has been reached
      currentNode = currentNode->down;
    }
    else { // end of the skiplist has been reached, node is not in skip list
      return;
    }
  }

found:
  struct node *temp;
  kprintf("removed|[%d]%d\n", procToDelete->pid, procToDelete->maxlevel);
  
  //change the maxlevel of process to -1 since it is deleted from skiplist
  currentNode->data->maxlevel = -1;
  // fix the pointers of the nodes to the left and right
  do{
    if (currentNode->right != NULL){
      currentNode->right->left = currentNode->left;
    }

    currentNode->left->right = currentNode->right; 
    // left node will never be NULL, so no NULL check

    // clear the fields of the node
    currentNode->left = NULL;
    currentNode->right = NULL;
    currentNode->data = NULL;
    temp = currentNode->down;
    currentNode->down = NULL;
    currentNode = temp; // go to the next level
  } while (currentNode != NULL);
  return;
}

// feed in pointer of global skip list, level to be inserted, and data (i.e. pointer to process)
void insertSkipListNode(struct proc *procToInsert){
  //cprintf("Attempting to insert %s\n", procToInsert->name);
  struct node *currentNode = skipList[NUM_HEADS - 1];
  struct node *stack[NUM_HEADS]; // stack to store nodes that went down
  int top = -1;
  procToInsert->deadline = computeVD(procToInsert);

  while (1){
    while (currentNode->right != NULL){
      if (currentNode->right->data->deadline < procToInsert->deadline){
        // go right if deadline(right) < deadline(toInsert)
        currentNode = currentNode->right;
      }
      else{
        top += 1;
        stack[top] = currentNode;
        if (currentNode->down == NULL){
          goto startInsert;
        }
        currentNode = currentNode->down;
      }
    }

    top += 1;
    stack[top] = currentNode;
    if (currentNode->down == NULL){
      goto startInsert;
    }
    currentNode = currentNode->down;
  }

startInsert:
  struct node *pointDownTemp = NULL;
  for (int currLevel = 0; currLevel < NUM_HEADS; currLevel++){
    struct node *newNode;
    struct node *popped = stack[top--];

    if (currLevel == 0 ||(random(100)) < 25){
      newNode = allocateSkipListNode(procToInsert);

      newNode->right = popped->right;
      popped->right = newNode;

      if (newNode->right != NULL){
        newNode->right->left = newNode;
      }

      newNode->left = popped;
      newNode->down = pointDownTemp;
      pointDownTemp = newNode;

    }
    else
    {
      break;
    }

    procToInsert->maxlevel = currLevel;
  }

  kprintf("inserted|[%d]%d\n", procToInsert->pid, procToInsert->maxlevel);
  return;
}

