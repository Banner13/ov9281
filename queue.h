#ifndef QUEUE_H
#define QUEUE_H

/*******************************************************************************
 *  include
 * ****************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 *  definition
 * ****************************************************************************/
typedef int QElemType;

typedef struct QNode {
    QElemType data;
}QNode_t, *QueuePtr;

typedef struct LinkQueue {
    QueuePtr base;
    char size;
    int front;
    int rear;
} LinkQueue_t ;

/*******************************************************************************
 *  prototype
 * ****************************************************************************/
void InitQueue(LinkQueue_t *Q);
void DestroyQueue(LinkQueue_t *Q);
void ClearQueue(LinkQueue_t *Q);
int QueueLength(LinkQueue_t Q);
bool EnQueue(LinkQueue_t *Q, QElemType e);
bool DeQueue(LinkQueue_t *Q, QElemType *e);

#endif /* endif QUEUE */