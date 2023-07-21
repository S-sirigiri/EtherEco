#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>

#define NUM_SPAM_DETECTORS 5

extern pid_t spam_detector_process_ids[NUM_SPAM_DETECTORS];

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////

int start_spam_detector(const char* argv);
void start_child_processes();
void stop_all_spam_detectors();
void wait_for_child_processes();

#endif /* PROCESS_HANDLER_H */