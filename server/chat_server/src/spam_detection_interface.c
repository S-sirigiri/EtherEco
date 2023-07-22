#include "spam_detection_interface.h"


/**
 * Load balancer function that distributes work across multiple workers.
 * It maintains a static variable to keep track of the next worker to be assigned work.
 *
 * @return The result is an integer representing the worker number to which the job is assigned.
 */
int loadbalancer() {
    static int currentNumber = 0;
    int result = currentNumber;

    currentNumber = (currentNumber + 1) % NUM_SPAM_DETECTORS;

    return result;
}


/**
 * Detects spam in a given message using interprocess communication techniques.
 *
 * @param message The message to be checked for spam.
 * @param username The username of the sender of the message.
 * @param rcpt_username The username of the recipient of the message.
 *
 * @return This function has no return value. The original message might be modified
 *         with a prefix "LIKELY SPAM" if it is detected as spam.
 */
void detect_spam(char *message, char *username, char *rcpt_username) {
    char message_queue_name[12] = "/mq";
    sprintf(message_queue_name, "%s%d", message_queue_name, loadbalancer());

    char sm_name[200];
    char sem_name[200];
    sprintf(sm_name, "%s%s%ld", username, rcpt_username, time(NULL));
    sprintf(sem_name, "%s%s%ld", username, rcpt_username, time(NULL));

    // Send the message to the spam detection process using shared memory
    interprocess_shared_memory_send(sm_name, message);

    char buffer[400];
    sprintf(buffer, "%s|%s", sm_name, sem_name);
    // Send the metadata (shared memory name and semaphore name) to the spam detection process using a message queue
    interprocess_message_queue_send(message_queue_name, buffer);

    char spam[2];
    memset(spam, '\0', sizeof(spam));
    // Receive the spam detection result from the spam detection process using shared memory
    interprocess_shared_memory_receive(sm_name, spam, sem_name);

    // If the message is detected as spam, modify the original message with a prefix "LIKELY SPAM"
    if (atoi(spam) == 1) {
        sprintf(message, "LIKELY SPAM %s", message);
    }
}