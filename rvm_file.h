/*
 * rvm_file.h
 *
 *  Created on: Apr 10, 2014
 *      Authors: Anish K. and Nikita G.
 */

#ifndef RVM_FILE_H_
#define RVM_FILE_H_

#include "rvm_types.h"
#include "rvm_global.h"

int rvm_file_size(char * seg_name, char * dir_name);				// Returns file size in bytes of seg_name for comparison with size_to_create
int rvm_file_write(char * seg_name, int seg_size, char * mode);		// Writes a segment file for mode 'w+' and appends for mode 'a' with NULL characters

#endif /* RVM_FILE_H_ */
