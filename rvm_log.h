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

/* Redo record in memory */
typedef struct rvm_redo
{
	char *				seg_name;
	int 				size;
	int 				offset;
	void *				seg_base_addr;
	struct rvm_redo *	rvm_redo_next;
} rvm_redo_t;

int rvm_log_write(char * seg_name, int size, int offset, char * data);	// Appends to a log file for a particular backing store
int rvm_log_delete(char * seg_name);									// Delete the segment entry from the log file in the backing store

#endif /* RVM_LOG_H_ */
