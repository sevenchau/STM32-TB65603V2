#include <string.h>
#include "queue.h"

void Queue_Init(Queue *queue,unsigned char queue_size,unsigned char element_num)
{
  queue->tail = queue->head = 0;
  queue->element_num = element_num;
  queue->queue_size = queue_size;
}

unsigned short isEmpty(Queue *queue)
{
  if(queue->head == queue->tail)
  {
    return 1;
  }
  return 0;
}

unsigned short isFull(Queue *queue)
{
  if((queue->head == ((queue->tail + 1)%queue->queue_size)))// || (queue->tail - queue->head == queue->queue_size - 1))
  {
    return 1;
  }
  return 0;
}

unsigned short enQueue(Queue *queue, unsigned short* pointer)
{
  if(isFull(queue))
  {
    return 1;
  }
  else
  {
    *pointer = queue->tail;

    queue->tail++;
    if(queue->tail == queue->queue_size)
    {
      queue->tail = 0;
    }
  }
  return 0;
}

unsigned char deQueue(Queue *queue, unsigned short* pointer)
{
  if(isEmpty(queue))
  {
    return 1;
  }
  else
  {
    *pointer = queue->head;
    queue->head++;
    if(queue->head == queue->queue_size)
    {
      queue->head = 0;
    }
  }
  return 0;
}
