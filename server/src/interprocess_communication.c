#include <interprocess_communication.h>

int interprocess_shared_memory_send(const char *name, const char *message) {

    int shm_fd;
    void *ptr;

    /* create the shared memory segment */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory segment */
    ftruncate(shm_fd, 4096);

    /* map the shared memory segment in the address space of the process */
    ptr = mmap(0, 4096, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (ptr == MAP_FAILED)
    {
        printf("Map failed\n");
        return -1;
    }

    /* write to the shared memory segment */
    sprintf(ptr, "%s", message);

    return 0;
}

int interprocess_shared_memory_receive(const char *name, char *message)
{
    int shm_fd;
    void *ptr;

    /* open the shared memory segment */
    shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        printf("Shared memory failed\n");
        return -1;
    }

    /* map the shared memory segment to the address space of the process */
    ptr = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed\n");
        return -1;
    }

    /* read from the shared memory segment */
    sscanf((char *)ptr, "%s", message);

    /* remove the shared memory segment */
    if (shm_unlink(name) == -1)
    {
        printf("Error removing %s\n", name);
        return -1;
    }

    return 0;
}


mqd_t interprocess_open_message_queue(char * queue_name)
{
    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(queue_name, O_CREAT | O_WRONLY, 0644, &attr);
    if ((mqd_t)-1 == mq)
    {
        perror("mq open failed");
        return -1;
    }
    return mq;
}

int interprocess_message_queue_send(mqd_t mq, char *buffer)
{
    /* send the message */
    if (0 > mq_send(mq, buffer, strlen(buffer), 0))
    {
        perror("mq send failed");
        return -1;
    }
    else
    {
        return 0;
    }
}

void interprocess_message_queue_cleanup(mqd_t mq, char *queue_name)
{
    mq_close(mq);
    mq_unlink(queue_name);
}

