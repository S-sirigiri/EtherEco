#include "process_handler.h"


/**
 * Starts the spam detector by executing the Python script using execlp.
 *
 * @param argv The argument to be passed to the Python script.
 * @return Returns 0 if the execution is successful, -1 if there's an error.
 */
int start_spam_detector_process(const char* argv) {
    // Use execlp to start the spam detector
    if(execlp("python", "python", "../spam_detector/src/main.py", argv, (char *) NULL) == -1){
        // Check for error
        fprintf(stderr, "execlp : python ../spam_detector/src/main.py %s", argv);
        // Return -1 to indicate an error
        return -1;
    }
    return 0; // this line will never be reached if execlp is successful
}


/**
 * Starts child processes using fork and executes the spam detector in each child process.
 * The child processes are created to run the spam detector in parallel.
 * Opens the message queue for that child process i.e., spam detector
 *
 * @return This function has no return value.
 */
void fork_and_start_multiple_spam_detectors() {
    // Loop to create child processes
    for (int iter = 0; iter < NUM_SPAM_DETECTORS; iter++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            char message_queue_name[10] = "/mq";
            sprintf(message_queue_name, "%s%d", message_queue_name, iter);
            interprocess_open_message_queue(message_queue_name);
            start_spam_detector_process(message_queue_name);
        } else if (pid < 0) {
            // Fork failed
            perror("fork");
        } else {
            // Parent process
            spam_detector_process_ids[iter] = pid;
            printf("Child process %d with PID %d started\n", iter + 1, pid);
        }
    }
}

/**
 * Stops all spam detector processes by sending them a SIGINT signal.
 * Closes the message queue for that spam detector
 *
 * @return This function has no return value.
 */
void stop_all_spam_detectors() {
    // Loop through all spam detector processes
    for (int iter=0; iter < NUM_SPAM_DETECTORS; iter++) {
        if (kill(spam_detector_process_ids[iter], SIGINT) == 0) {
            printf("SIGINT signal sent to the process\n");
        } else {
            perror("kill");
        }
        char message_queue_name[10] = "/mq";
        sprintf(message_queue_name, "%s%d", message_queue_name, iter);
        interprocess_message_queue_cleanup(message_queue_name);
    }
}


/**
 * Waits for all child processes to finish and prints their exit status.
 *
 * @return This function has no return value.
 */
void wait_for_child_processes() {
    int status;
    pid_t pid;

    // Loop until no more child processes are present
    while ((pid = wait(&status)) != -1) {
        // Print the exit status of the finished child process
        printf("Child process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
    }
}
