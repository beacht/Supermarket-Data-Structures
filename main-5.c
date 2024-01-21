#include <stdio.h>
#include <stdlib.h>

void *malloc(size_t size);

int storeTime = 0;
int minTime = 1000000000;
#define ARRAY_SIZE 12

typedef struct customer
{
  int entryTime;
  int line;
  char name[9]; //Not malloc'ed because the assignment PDF says the max length of a name is 9
  int items;
} customer;

typedef struct node
{
  customer *customer;
  struct node *next;
} node;

typedef struct queue
{
  node *head;
  node *rear;
} queue;

node peek(struct queue *line)
{
  return *line->head;
}

int empty(struct queue *line)
{
  if(line->head == NULL)
    return 1;
  else
    return 0;
}

node* dequeue(struct queue *line)
{
  node* temp = line->head;
  temp = line->head->next;
  free(line->head);
  return temp;
}

node* enqueue(struct queue *line, customer *person)
{
  node *t;
  node *temp;
  temp = (node*)malloc(sizeof(node));
  temp->customer = person;
  temp->next = NULL;

  if(line->head == NULL)
    line->head = temp;
  else
  {
    t = line->head;
    while(t->next != NULL)
      t = t->next;
    t->next = temp;
  }
  return line->head;
}

void checkoutCustomer(queue *queueArray, node* person)
{
  storeTime += (30 + (5 * person->customer->items));
  printf("%s from line %d checks out at time %d.\n", person->customer->name, person->customer->line, storeTime);
  queueArray[person->customer->line-1].head = dequeue(&queueArray[person->customer->line-1]);
  free(person->customer);
}

queue fillArray(queue *queueArray, FILE *input, int customerCount)
{
  for(int x = 0; x < ARRAY_SIZE; x++)
  {
    queueArray[x].head = NULL;
    queueArray[x].rear = NULL;
  }

  for(int customerIndex = 0; customerIndex < customerCount; customerIndex++)
  {
    node *temp;
    temp = (node*)malloc(sizeof(node));
    temp->customer = (customer*)malloc(sizeof(customer));
    temp->next = NULL;

    fscanf(input, "%d %d %s %d", &temp->customer->entryTime, &temp->customer->line, temp->customer->name, &temp->customer->items);
    queueArray[temp->customer->line-1].head = enqueue(&queueArray[temp->customer->line-1], temp->customer);
    if(customerIndex == 0)
    {
      storeTime = temp->customer->entryTime;
    }
    free(temp);
  }
  return *queueArray;
}

void doBusiness(queue *queueArray, int customerCount)
{
  for(int customerIndex = 0; customerIndex < customerCount; customerIndex++)
  {
    node checkout;
    int maximumItems = 101; //Per the assignment PDF, the maximum amount of items a customer can have is 100
    int lowestLine = 0;
    int timeFlag = 0;
    for(int lineIndex = 0; lineIndex < ARRAY_SIZE; lineIndex++)
    {
      if(!empty(&queueArray[lineIndex]))
      {
        node frontOfLine = peek(&queueArray[lineIndex]);
        if(frontOfLine.customer->entryTime <= storeTime)
        {
          timeFlag = 1; //This means that a customer is actually in line at the given time (that there isn't a time period where nobody is in line before someone else comes, like Aisa)
          if(frontOfLine.customer->items < maximumItems)
          {
            checkout = frontOfLine;
            maximumItems = frontOfLine.customer->items;
            lowestLine = frontOfLine.customer->line;
          }
          else if(frontOfLine.customer->items == maximumItems)
          {
            if(frontOfLine.customer->line < lowestLine)
            {
              checkout = frontOfLine;
              maximumItems = frontOfLine.customer->items;
              lowestLine = frontOfLine.customer->line;
            }
          }
        }
      }
    }
    if(timeFlag == 0) //This means there's a period where nobody is in line. This occurs in Test Case 1, after Jenny is checked out but before Aisa enters the line.
    {
      node personToSetTimeTo;
      for(int lineIndex = 0; lineIndex < ARRAY_SIZE; lineIndex++)
      {
        if(!empty(&queueArray[lineIndex]))
        {
          personToSetTimeTo = peek(&queueArray[lineIndex]);
          if(personToSetTimeTo.customer->entryTime < minTime)
          {
            minTime = personToSetTimeTo.customer->entryTime; //This loop finds who the next person to actually enter the line is, by finding who has the soonest entry time.
          }
        }
      }
      storeTime = personToSetTimeTo.customer->entryTime; //Sets the store time to the next person (Aisa)'s entry time, and then repeats the process of finding who's next (with an advanced store time).

      for(int lineIndex = 0; lineIndex < ARRAY_SIZE; lineIndex++)
      {
        if(!empty(&queueArray[lineIndex]))
        {
          node frontOfLine = peek(&queueArray[lineIndex]);
          if(frontOfLine.customer->entryTime <= storeTime)
          {
            if(frontOfLine.customer->items < maximumItems)
            {
              checkout = frontOfLine;
              maximumItems = frontOfLine.customer->items;
              lowestLine = frontOfLine.customer->line;
            }
            else if(frontOfLine.customer->items == maximumItems)
            {
              if(frontOfLine.customer->line < lowestLine)
              {
                checkout = frontOfLine;
                maximumItems = frontOfLine.customer->items;
                lowestLine = frontOfLine.customer->line;
              }
            }
          }
        }
      }
    }
    checkoutCustomer(queueArray, &checkout);
  }
}

int main(void)
{
  FILE *inputfile;
  int t, customerCount;
  inputfile = fopen("assignment2input.txt", "r");

  fscanf(inputfile, "%d", &t);
  for(int x = 0; x < t; x++)
  {
    queue queueArray[ARRAY_SIZE] = {0};
    fscanf(inputfile, "%d", &customerCount);
    *queueArray = fillArray(queueArray, inputfile, customerCount);
    doBusiness(queueArray, customerCount);
  }

  fclose(inputfile);
  return 0;
}