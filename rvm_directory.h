/*
 * rvm_directory.h
 *
 *  Created on: Apr 7, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. Link with rvm_segment.h.
 */

#include "rvm_types.h"
#include "rvm_segment.h"
#include "rvm_global.h"

#ifndef RVM_DIRECTORY_H_
#define RVM_DIRECTORY_H_

typedef struct rvm_dir
{
	rvm_t				dir_id;
	char *				dir_name;
	struct rvm_seg *	seg_head;
	struct rvm_dir *	dir_next;
}	rvm_dir_t;

extern rvm_dir_t * rvm_global_dir_head;

int rvm_dir_check_exists(char * dir_name);	// Checks if directory exists.
int rvm_dir_mkdir(char * dir_name);			// Makes a directory at current location.
rvm_t rvm_dir_create(char * dir_name);		// Populates the directory structure and appends to the end of the directory linked list.
rvm_dir_t * rvm_dir_get(rvm_t dir_id);		// Returns the directory structure by looking up with dir_id.

#endif /* RVM_DIRECTORY_H_ */
