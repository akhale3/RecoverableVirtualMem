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
 * 3. rvm_seg_update()
 */

#include "rvm_segment.h"

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
		rvm_exit("Segment does not exist");
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

			/* Unmap segment from virtual memory */
			if(munmap(rvm_seg_curr->seg_base_addr, rvm_seg_curr->seg_size) != 0)
			{
				rvm_exit("Segment unmapping error");
			}

			return;
		}
		else
		{
			rvm_seg_prev = rvm_seg_curr;
			rvm_seg_curr = rvm_seg_curr->seg_next;
		}
	}

	return;
}

/*
 * @function		rvm_seg_size
 * @brief			Returns file size in bytes of seg_name for comparison with size_to_create
 * @param[seg_name]	Name of segment file
 * @param[dir_name]	Name of directory containing the segment
 * @return			Size of the segment file
 */
int rvm_seg_size(char * seg_name, char * dir_name)
{
	int size = 0;

	if(dir_name != NULL)
	{
		if(strcmp(dir_name, ".") != 0)
		{
			chdir(dir_name);
		}
		/**/
		FILE * rvm_file_ptr = fopen(seg_name, "r");
		if(rvm_file_ptr != NULL)
		{
			fseek(rvm_file_ptr, 0, SEEK_END);
			size = ftell(rvm_file_ptr);
			fclose(rvm_file_ptr);
		}

		if(strcmp(dir_name, ".") != 0)
		{
			chdir("..");
		}
	}

	return size;
}

/*
 * @function		rvm_seg_write
 * @brief			Writes a segment file for mode 'w+' and appends for mode 'a' with NULL characters
 * @param[seg_name]	Name of segment file
 * @param[seg_size]	Required size of the segment file
 * @param[mode]		'w+' for write mode, 'a' for append mode
 * @return			1 if successful, 0 if erroneous
 */
int rvm_seg_write(char * seg_name, int seg_size, char * mode)
{
	FILE * rvm_file_ptr = fopen(seg_name, mode);

	int curr_seg_size = rvm_seg_size(seg_name, ".");
	int seg_size_offset = seg_size;
	int i;
	int ret = 0;

	char buff[1];
	buff[0] = '\0';

	if(seg_size > curr_seg_size)
	{
		seg_size_offset = seg_size - curr_seg_size;
	}

	if(rvm_file_ptr != NULL)
	{
		for(i = 0; i < seg_size_offset; i++)
		{
			fwrite(buff, sizeof(char), 1, rvm_file_ptr);
		}
		fclose(rvm_file_ptr);
		ret = 1;
	}

	return ret;
}
