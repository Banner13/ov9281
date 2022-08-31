

/*******************************************************************************
 *  include
 * ****************************************************************************/
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "queue.h"


/*******************************************************************************
 *  definition
 * ****************************************************************************/
#define OVERFLOW 1
#define QUEUE_INIT_SIZE 61


typedef bool Status;


/*******************************************************************************
 *  code
 * ****************************************************************************/
void InitQueue(LinkQueue_t *Q)
{
    Q->base = (QueuePtr)malloc(sizeof(QNode_t) * Q->size);
    if(NULL == Q->base)
        exit(OVERFLOW);

    Q->front = Q->rear = 0;
}

void DestroyQueue(LinkQueue_t *Q)
{
    if (Q->base)
        free(Q->base);

    Q->size = Q->front = Q->rear = 0;
}

void ClearQueue(LinkQueue_t *Q)
{
    Q->front = Q->rear = 0;
}

static inline Status QueueEmpty(LinkQueue_t Q)
{
    if (Q.front == Q.rear)
    {
        printf("Q is Empty\n");
        return true;
    }
    else
        return false;
}

static inline Status QueueFull(LinkQueue_t Q)
{
    if ((Q.rear + 1) % Q.size == Q.front)
    {
        printf("Q is Full\n");
        return true;
    }
    else
        return false;
}

int QueueLength(LinkQueue_t Q)
{
    int length = (Q.rear - Q.front + Q.size)
                % Q.size ;

    return length;
}

Status GetHead(LinkQueue_t Q, QElemType *e)
{
    if(QueueEmpty(Q))
        return false;
    
    memcpy((void*)e, (void*)&Q.base[Q.front], sizeof(QElemType));
    return true;
}

bool EnQueue(LinkQueue_t *Q, QElemType e)
{
    if (QueueFull(*Q))
        return false;

    memcpy((void*)&Q->base[Q->front], (void*)&e, sizeof(QElemType));
    Q->rear++;

    if (Q->rear >= Q->size)
        Q->rear = 0;

    return true;
}

bool DeQueue(LinkQueue_t *Q, QElemType *e)
{
    if(QueueEmpty(*Q))
        return false;
    
    *e = Q->base[Q->front].data;
    memcpy((void*)e, (void*)&Q->base[Q->front], sizeof(QElemType));
    Q->front++;

    if (Q->front >= Q->size)
        Q->front = 0;

    return true;
}

void ShowQueue(LinkQueue_t Q)
{
    int head = Q.front;
    int tail = Q.rear;

    printf("front = %d\n", head);
    printf("rear = %d\n", tail);
    while (head != tail)
    {
        head++;
        if (head >= Q.size)
            head = 0;
    }
    printf("\n");
}

int main(void)
{

    return 0;
}