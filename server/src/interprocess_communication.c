#include <interprocess_communication.h>

/**
 * Sends a message to a shared memory segment.
 *
 * @param name - The name of the shared memory segment.
 * @param message - The message to be sent.
 * @return 0 if successful, -1 if an error occurs.
 */
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

/**
 * Receives a message from a shared memory segment.
 *
 * @param name - The name of the shared memory segment.
 * @param message - The buffer to store the received message.
 * @param sem_name - The name of the semaphore associated with the shared memory segment.
 * @return 0 if successful, -1 if an error occurs.
 */
int interprocess_shared_memory_receive(const char *name, char *message, const char* sem_name)
{
    int shm_fd;
    void *ptr;
    sem_t *semaphore;

    /* open the semaphore */
    semaphore = sem_open(sem_name, 0);  // Open a preexisting semaphore
    if (semaphore == SEM_FAILED)
    {
        perror("sem_open failed");
        return -1;
    }

    /* wait for the semaphore */
    if (sem_wait(semaphore) == -1)
    {
        perror("sem_wait failed");
        return -1;
    }

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

    /* Close the shared memory segment */
    if (close(shm_fd) == -1)
    {
        printf("Error closing %s\n", name);
        return -1;
    }

    /* Close the semaphore */
    if (sem_close(semaphore) == -1)
    {
        perror("sem_close failed");
        return -1;
    }

    /* Remove the semaphore */
    if (sem_unlink(name) == -1) {
        perror("sem_unlink failed");
        return -1;
    }

    return 0;
}

/**
 * Opens or creates a message queue with the given name.
 *
 * @param queue_name - The name of the message queue.
 * @return The message queue descriptor (mqd_t) if successful, -1 if an error occurs.
 */
mqd_t interprocess_open_message_queue(char * queue_name)
{
    mqd_t mq;

    // Set the attributes for the message queue
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

/**
 * Sends a message to the specified message queue.
 *
 * @param mq - The message queue descriptor (mqd_t).
 * @param buffer - The buffer containing the message to send.
 * @return 0 if successful, -1 if an error occurs.
 */
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

/**
 * Cleans up and closes the specified message queue.
 *
 * @param mq - The message queue descriptor (mqd_t).
 * @param queue_name - The name of the message queue.
 */
void interprocess_message_queue_cleanup(mqd_t mq, char *queue_name)
{
    // Close the message queue using mq_close()
    mq_close(mq);
    // Unlink (delete) the message queue using mq_unlink()
    mq_unlink(queue_name);
}
