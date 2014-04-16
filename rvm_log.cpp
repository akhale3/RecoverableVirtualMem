/*
 * rvm_log.cpp
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. rvm_log_delete()
 */

#include "rvm_log.h"

#define DEBUG 1

/*
 * @function		rvm_log_write
 * @brief			Appends to a log file for a particular backing store
 * @param[seg_name]	Name of segment file
 * @param[size]		Size of the segment data to be written in the log file
 * @param[offset]	Can be used to calculate the beginning of the next log segment
 * @param[data]		Data to be written to the log file
 * @return			1 if successful, 0 if erroneous
 */
int rvm_log_write(char * seg_name, int size, int offset, char * data)
{
	FILE * rvm_file_ptr;
	int ret = 1;

	rvm_file_ptr = fopen("rvm.log", "a+");
	if(rvm_file_ptr == NULL)
	{
		rvm_exit("Log does not exist");
	}

	fprintf(rvm_file_ptr, "%s\n", seg_name);
	fprintf(rvm_file_ptr, "%d\n", size);
	fprintf(rvm_file_ptr, "%d\n", offset);
	fwrite(data, size, 1, rvm_file_ptr);

	fclose(rvm_file_ptr);

	return ret;
}

/*
 * @function		rvm_log_delete
 * @brief			Delete the log file from the backing store associated with a particular segment
 * @param[seg_name]	Name of segment file
 * @return			1 if successful, 0 if erroneous
 */
int rvm_log_delete(char * seg_name)
{

}
