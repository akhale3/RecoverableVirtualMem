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
#include "rvm_global.h"
#include "rvm_directory.h"

typedef struct rvm_seg
{
	char *				seg_name;
	void *				seg_base_addr;
	int					seg_size;
	trans_t				seg_trans_id;
	struct rvm_seg *	seg_next;
}	rvm_seg_t;

int rvm_seg_mapped(char * seg_name, rvm_t dir_id);				// Checks whether a segment has already been mapped.
int rvm_seg_exists(char * seg_name, rvm_t dir_id);				// Checks whether a segment already exists on disk.
int rvm_seg_delete(void * seg_base_addr, rvm_t dir_id);		// Deletes a segment.
int rvm_seg_size(char * seg_name, char * dir_name);				// Returns file size in bytes of seg_name for comparison with size_to_create.
int rvm_seg_write(char * seg_name, int seg_size, char * mode);	// Writes a segment file for mode 'w+' and appends for mode 'a' with NULL characters.
rvm_seg_t * rvm_seg_get(void * seg_base_addr, rvm_t dir_id);	// Returns a segment structure pointer for a particular segbase.

#endif /* RVM_SEGMENT_H_ */
