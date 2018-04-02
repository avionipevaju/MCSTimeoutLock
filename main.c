#include <stdio.h>
#include <stdlib.h>
#include "domaci1.h"

#define true 1
#define false 0

typedef struct node {
    int waiting;
    struct node* next;
    int predecessor_timeout;

}node;

node *tail = NULL;

int lock_n_threads_with_timeout(int id, int* local, double timeout){
    double start_time = lrk_get_time();
    node *currentNode = malloc(sizeof(node));
    currentNode->next = NULL;
    currentNode->predecessor_timeout = false;
    node *previous = lrk_get_and_set(&tail, currentNode);
    if (previous == NULL) { // lock was free, acquire.
        currentNode->waiting = false;
        local[0] = currentNode;
        return 1;
    } else { //queue isn't empty
        currentNode->waiting = true;
        previous->next = currentNode;
        while (currentNode->waiting) {
            if(lrk_get_time() - start_time > timeout){
                if(currentNode->next != NULL)
                    lrk_get_and_set(&currentNode->next->predecessor_timeout, true);
                while (lrk_compare_and_set(&currentNode->predecessor_timeout, true, true)){
                    lrk_sleep(0);
                }
                lrk_get_and_set(&previous->next, currentNode->next);
                if(currentNode->next != NULL)
                    lrk_get_and_set(&currentNode->next->predecessor_timeout, false);
                local[0]= currentNode;
                return 0;
            }
            lrk_sleep(0);
        }
        currentNode->waiting = false;
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
            lrk_sleep(0);
        }
    }
    currentNode->next->waiting = false;
};

int main() {
    printf("Start MCSLock!\n");
    //0.0348
    start_timeout_mutex_n_threads_test(0.05);
    return 0;
}
