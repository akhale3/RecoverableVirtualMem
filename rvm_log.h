/*
 * rvm_log.h
 *
 *  Created on: Apr 10, 2014
 *      Authors: Anish K. and Nikita G.
 */

#ifndef RVM_LOG_H_
#define RVM_LOG_H_

#include "rvm_types.h"
#include "rvm_global.h"

int rvm_log_write(char * seg_name, int log_size, int offset, char * data);	// Appends to a log file for a particular backing store
int rvm_log_delete(char * seg_name);										// Delete the log file from the backing store associated with a particular segment

#endif /* RVM_LOG_H_ */
