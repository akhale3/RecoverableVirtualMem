/*
 * rvm_transaction.h
 *
 *  Created on: Apr 15, 2014
 *      Authors: Anish K. and Nikita G.
 */

#ifndef RVM_TRANSACTION_H_
#define RVM_TRANSACTION_H_

#include "rvm_types.h"
#include "rvm_log.h"
#include "rvm_global.h"

typedef struct rvm_trans
{
	trans_t				trans_id;
	void **				trans_seg_bases;
	int 				trans_seg_count;
	rvm_t				trans_dir_id;
	struct rvm_redo *	rvm_redo_head;
	struct rvm_trans *	trans_next;
} rvm_trans_t;

extern rvm_trans_t * rvm_global_trans_head;

trans_t rvm_trans_create(rvm_t rvm, int numsegs, void **segbases);	// Inserts a transaction structure to the head of the transaction linked list.
void rvm_trans_delete(trans_t trans_id);							// Deletes a particular transaction from the transaction linked list.
rvm_trans_t * rvm_trans_get(trans_t trans_id);						// Returns a transaction structure pointer for a particular transaction.

#endif /* RVM_TRANSACTION_H_ */
