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
 * @function		rvm_log_update
 * @brief			Update the segment entry in the log file in the backing store
 * @param[seg_name]	Name of segment file
 * @param[dir]		Name of directory
 * @return			1 if successful, 0 if erroneous
 */
int rvm_log_update(char * seg_name, char * dir)
{
	FILE * rvm_file_ptr, * rvm_backup_ptr;
	int ret = 1;
	int i;

	rvm_file_ptr = fopen("rvm.log", "r");
	if(rvm_file_ptr == NULL)
	{
		rvm_exit("Log does not exist");
	}

	rvm_backup_ptr = fopen("rvm.log.tmp", "w+");
	if(rvm_backup_ptr == NULL)
	{
		rvm_exit("Temporary log backup creation error");
	}

	char temp[100000];

	while(!feof(rvm_file_ptr))
	{
		if(fgets(temp, 100000, rvm_file_ptr))
		{
			if(strncmp(temp, seg_name, sizeof(temp) - 1) != 0)
			{
				fprintf(rvm_backup_ptr, "%s", temp);
			}
			else
			{
				/* Got seg_name
				Get size, offset and data
				Effectively, skip segment */
				for(i = 0; i < 3; i++)
				{
					fgets(temp, 100000, rvm_file_ptr);
				}
			}
		}
	}

	fclose(rvm_backup_ptr);
	fclose(rvm_file_ptr);

	system("mv rvm.log.tmp rvm.log");

	return ret;
}

/*
 * @function				rvm_redo_delete
 * @brief					Deletes all redo records of all transactions from memory
 * @param[seg_base_addr]	Base address of segment whose redo records are to be deleted
 * @return					None
 */
void rvm_redo_delete(void * seg_base_addr)
{
	rvm_trans_t * rvm_trans_curr;
	rvm_trans_curr = rvm_global_trans_head;

	if(rvm_trans_curr == NULL)
	{
		rvm_exit("Transaction list empty");
	}

	while(rvm_trans_curr != NULL)
	{
		rvm_redo_t * rvm_redo_curr;
		rvm_redo_curr = rvm_trans_curr->rvm_redo_head;

		if(rvm_redo_curr == NULL)
		{
			rvm_exit("No redo records exist");
		}

		rvm_redo_t * rvm_redo_prev;
		rvm_redo_prev = rvm_redo_curr;

		while(rvm_redo_curr != NULL)
		{
			if(rvm_redo_curr->seg_base_addr == seg_base_addr)
			{
				rvm_redo_prev->rvm_redo_next = rvm_redo_curr->rvm_redo_next;
				return;
			}
			else
			{
				rvm_redo_prev = rvm_redo_curr;
				rvm_redo_curr = rvm_redo_curr->rvm_redo_next;
			}
		}
	}
}
