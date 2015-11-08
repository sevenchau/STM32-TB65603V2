#ifndef __QUEUE_H
#define __QUEUE_H

typedef struct 
{
  unsigned short head;
  unsigned short tail;
  unsigned short queue_size;
  unsigned short element_num;
}Queue;

/* 初始化队列 */
void Queue_Init(Queue *queue,unsigned char queue_size,unsigned char element_size);

/* 判断队列是否为空 */
unsigned short isEmpty(Queue *queue);
/* 判断队列是否为满 */
unsigned short isFull(Queue *queue);
/* 数据入队列 */
unsigned short enQueue(Queue *queue, unsigned short* pointer);
/* 数据出队列 */
unsigned char deQueue(Queue *queue, unsigned short* pointer);

#endif