#ifndef INTERPROCESS_COMMUNICATION_H
#define INTERPROCESS_COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>

#define MAX_SIZE 1024

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


//shared memory
int interprocess_shared_memory_send(const char *name, const char *message);
int interprocess_shared_memory_receive(const char *name, char *message);

//message queues
mqd_t interprocess_open_message_queue(char * queue_name);
int interprocess_message_queue_send(mqd_t mq, char *buffer);
void interprocess_message_queue_cleanup(mqd_t mq, char *queue_name);



#endif /* INTERPROCESS_COMMUNICATION_H */