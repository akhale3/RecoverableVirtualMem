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
#include "rvm_segment.h"
#include "rvm_directory.h"
#include "rvm_transaction.h"

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
void rvm_log_update(char * seg_name, char * dir);						// Update the segment entry in the log file in the backing store
void rvm_redo_delete(void * seg_base_addr);								// Deletes all redo records of all transactions from memory

#endif /* RVM_LOG_H_ */
