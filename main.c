#include <stdio.h>
#include <stdlib.h>
#include "domaci1.h"

#define true 1
#define false 0

typedef struct node {
    int waiting;
    struct node* next;

}node;

node *tail = NULL;

int lock_n_threads_with_timeout(int id, int* local, double timeout){
    double start_time = lrk_get_time();
    node *currentNode = malloc(sizeof(node));
    currentNode->next = NULL;
    node *previous = lrk_get_and_set(&tail, currentNode);
    if (previous != NULL ) { //queue isn't empty
        currentNode->waiting = true;
        previous->next = currentNode;
        local[0] = currentNode;
        while (currentNode->waiting) {
            if(lrk_get_time() - start_time >= timeout){
                previous->next = currentNode->next;
                return 0;
            }
            lrk_sleep(1);
        }
        return 1;
    } else {
        currentNode->waiting = false; // lock was free, acquire.
        local[0] = currentNode;
        return 1;
    }
};

void unlock_n_threads_with_timeout(int id, int* local) {
    node *currentNode = local[0];
    if (currentNode->next == NULL) {
        if (lrk_compare_and_set(&tail, currentNode, NULL)) {
            return;
        }
        while (currentNode->next == NULL) {
            lrk_sleep(1);
        }
    }
    currentNode->next->waiting = false; //free lock, tell next node to acquire
};

int main() {
    printf("Start MCSLock!\n");
    start_timeout_mutex_n_threads_test(0.1);
    return 0;
}