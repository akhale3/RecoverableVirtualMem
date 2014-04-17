/*
 * rvm_global.cpp
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

#include "rvm_global.h"

rvm_t rvm_global_dir_id;
trans_t rvm_global_trans_id;

void rvm_exit(char * err_msg)
{
	perror(err_msg);
	exit(0);
}
