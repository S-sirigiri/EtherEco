#ifndef SPAM_DETECTION_INTERFACE_H
#define SPAM_DETECTION_INTERFACE_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "interprocess_communication.h"
#include "process_handler.h"

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


int loadbalancer();
void detect_spam(char *message, char *username, char *rcpt_username);

#endif /* SPAM_DETECTION_INTERFACE_H */