/*
 * rvm_segment.h
 *
 *  Created on: Apr 8, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. Complete rvm_seg.
 */

#ifndef RVM_SEGMENT_H_
#define RVM_SEGMENT_H_

#include "rvm_types.h"
#include "rvm_directory.h"

typedef struct rvm_seg
{
	char *				seg_name;
	void *				seg_base_addr;
	int					seg_size;
	trans_t				seg_trans_id;
	struct rvm_seg *	seg_next;
}	rvm_seg_t;

int rvm_seg_mapped(char * seg_name, rvm_t dir_id);	// Checks whether a segment has already been mapped.
int rvm_seg_exists(char * seg_name, rvm_t dir_id);	// Checks whether a segment already exists on disk.
void rvm_seg_delete(void * seg_base, rvm_t dir_id);	// Deletes a segment.

#endif /* RVM_SEGMENT_H_ */
