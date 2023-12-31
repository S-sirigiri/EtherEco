#ifndef INTERPROCESS_COMMUNICATION_H
#define INTERPROCESS_COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <mqueue.h>
#include <semaphore.h>
#include <string.h>

#define MAX_SIZE 1024

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


//shared memory
int interprocess_shared_memory_send(const char *name, const char *message);
int interprocess_shared_memory_receive(const char *name, char *message, const char* sem_name);

//message queues
mqd_t interprocess_open_message_queue(char * queue_name);
int interprocess_message_queue_send(char *message_queue_name, char *buffer);
void interprocess_message_queue_cleanup(char *message_queue_name);



#endif /* INTERPROCESS_COMMUNICATION_H */