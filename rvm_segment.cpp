/*
 * rvm_segment.cpp
 *
 *  Created on: Apr 8, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. Complete interface definitions.
 * 2. Create rvm_exit(string perror) containing perror and exit(0).
 */

#include "rvm_segment.h"

#define DEBUG 1

/*
 * @function		rvm_seg_mapped
 * @brief			Checks whether a segment has already been mapped
 * @param[seg_name]	Name of segment
 * @param[dir_id]	Directory ID containing seg_name
 * @return			1 if mapped, 0 if not
 */
int rvm_seg_mapped(char * seg_name, rvm_t dir_id)
{
	rvm_dir_t * rvm_dir;
	rvm_dir = rvm_dir_get(dir_id);

	if(rvm_dir == NULL)
	{
		rvm_exit("Unknown directory");
	}

	rvm_seg_t * rvm_seg;
	rvm_seg = rvm_dir->seg_head;

	if(rvm_seg == NULL)
	{
		if(DEBUG)
		{
			cout << "Segment not found";
		}
		return 0;
	}

	while(rvm_seg != NULL)
	{
		if(rvm_seg->seg_name == seg_name)
		{
			return 1;
		}
		else
		{
			rvm_seg = rvm_seg->seg_next;
		}
	}

	if(DEBUG)
	{
		cout << "Segment not found";
	}

	return 0;
}

/*
 * @function		rvm_seg_exists
 * @brief			Checks whether a segment already exists on disk
 * @param[seg_name]	Name of segment
 * @param[dir_id]	Directory ID containing seg_name
 * @return			1 if it exists, 0 if it doesn't
 */
int rvm_seg_exists(char * seg_name, rvm_t dir_id)
{
	rvm_dir_t * rvm_dir;
	rvm_dir = rvm_dir_get(dir_id);

	char * dir_name;
	dir_name = rvm_dir->dir_name;

	/* Switch to directory that is expected to host the concerned segment */
	chdir(dir_name);
	struct stat * buff;
	int status = -1;
	status = stat((const char *)seg_name, buff);
	/* Switch back to root directory */
	chdir("..");

	return (status + 1);
}

/*
 * @function		rvm_seg_delete
 * @brief			Deletes a segment
 * @param[seg_base]	Base address of the segment as passed in rvm_unmap()
 * @param[dir_id]	Directory ID containing seg_base
 * @return			None
 */
void rvm_seg_delete(void * seg_base_addr, rvm_t dir_id)
{
	rvm_dir_t * rvm_dir;
	rvm_dir = rvm_dir_get(dir_id);

	if(rvm_dir == NULL)
	{
		rvm_exit("Unknown directory");
	}

	rvm_seg_t * rvm_seg_curr;
	rvm_seg_curr = rvm_dir->seg_head;

	if(rvm_seg_curr == NULL)
	{
		if(DEBUG)
		{
			cout << "Segment not found";
		}
		return;
	}

	rvm_seg_t * rvm_seg_prev;
	rvm_seg_prev = rvm_seg_curr;

	while(rvm_seg_curr != NULL)
	{
		if(rvm_seg_curr->seg_base_addr == seg_base_addr)
		{
			rvm_seg_prev->seg_next = rvm_seg_curr->seg_next;
			if(rvm_seg_prev == rvm_seg_curr)	// Segment head. Set seg_head to NULL.
			{
				rvm_dir->seg_head = NULL;
			}
			return;
		}
		else
		{
			rvm_seg_prev = rvm_seg_curr;
			rvm_seg_curr = rvm_seg_curr->seg_next;
		}
	}

	if(DEBUG)
	{
		cout << "Segment not found";
	}

	return;
}
