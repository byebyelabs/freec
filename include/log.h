#ifndef FREEC_LOG_H
#define FREEC_LOG_H 1

/**
 * Print a message directly to standard error without invoking malloc or free.
 * \param message   A null-terminated string that contains the message to be printed
 */
void log_message(char *message);

#endif
