/*
 * rvm_global.h
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

#ifndef RVM_GLOBAL_H_
#define RVM_GLOBAL_H_

#include "rvm_types.h"
#include "rvm_directory.h"

extern rvm_t rvm_global_dir_id;
extern rvm_dir_t * rvm_global_dir_head;

extern trans_t rvm_global_trans_id;
extern rvm_trans_t * rvm_global_trans_head;

void rvm_exit(char * err_msg);

#endif /* RVM_GLOBAL_H_ */
