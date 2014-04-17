/*
 * rvm_global.h
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

#ifndef RVM_GLOBAL_H_
#define RVM_GLOBAL_H_

#include "rvm_types.h"

extern rvm_t rvm_global_dir_id;
extern trans_t rvm_global_trans_id;

void rvm_exit(char * err_msg);

#endif /* RVM_GLOBAL_H_ */
