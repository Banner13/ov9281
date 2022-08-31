

#include <stdlio.h> 


#define OVERFLOW 1
#define QUEUE_INIT_SIZE 61


typedef bool Status;
typedef int QElemType;

typedef struct QNode {
    QElemType data;
    struct QNode *next;
}QNode_t, *QueuePtr;

typedef struct LinkQueue {
    QueuePtr front;
    QueuePtr rear;
} LinkQueue_t ;


void InitQueue(LinkQueue_t *Q)
{ 
    if(!(Q->front = Q->rear = (QueuePtr)malloc(sizeof(QNode_t))))
        exit(OVERFLOW);

    Q->front->next = NULL;
}

void DestroyQueue(LinkQueue_t *Q)
{
    while (Q->front)
    {
        Q->rear = Q->front->next;
        free(Q->front);
        Q->front = Q->rear;
    }
}

void ClearQueue(LinkQueue_t *Q)
{
    QueuePtr p, q;
    p = Q->front->next;

    while (p)
    {
        q = p;
        p = p->next;
        free(q);
    }

    Q->front->next = NULL;
    Q->rear = Q->front;
}

Status QueueEmpty(LinkQueue_t Q)
{
    if (NULL == Q.front)
        return true;
    else
        return false;
}

int QueueLength(LinkQueue_t Q)
{
    QueuePtr p;
    int i = 0;

    p = Q.front;

    while(Q.rear != p)
    {
        i++;
        p = p->next;
    }

    return i;
}

Status GetHead(LinkQueue_t Q, QElemType *e)
{
    QueuePtr p;

    if(Q.front == Q.rear)
        return false;
    
    p = Q.front->next;
    e = p->data;
    return true;
}

void EnQueue(LinkQueue_t *Q, QElemType e)
{
    QueuePtr p;

    if(!(p = (QueuePtr)malloc(sizeof QNode_t)))
        exit(OVERFLOW);
    
    p->data = e;
    p->next = NULL;

    Q->rear->next = p;
    Q->rear = p;
}

Status DeQueue(LinkQueue_t *Q, QElemType *e)
{
    QueuePtr p;
    if(Q->front == Q->rear)
        return false;
    
    p = Q->front->next;
    e = p->data;

    Q->front->next = p->next;

    if(Q->rear == p)
        Q->rear = Q->front;

    free(p);
    return true;
}