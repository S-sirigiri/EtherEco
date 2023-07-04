#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"

int main(int argc, char **argv)
{
    mqd_t mq;
    char buffer[MAX_SIZE];

    /* initialize the queue attributes */
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if ((mqd_t)-1 == mq)
    {
        perror("mq_open");
        exit(1);
    }

    do {
        printf("Send to server (enter \"exit\" to stop): ");

        /* send the message */
        fgets(buffer, MAX_SIZE, stdin);
        if (0 > mq_send(mq, buffer, strlen(buffer), 0))
        {
            perror("mq_send");
            exit(1);
        }
    } while (strncmp(buffer, MSG_STOP, strlen(MSG_STOP)));

    /* cleanup */
    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}
