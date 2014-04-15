/*
 * rvm_file.cpp
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

#include "rvm_file.h"

#define DEBUG 1

/*
 * @function		rvm_file_size
 * @brief			Returns file size in bytes of seg_name for comparison with size_to_create
 * @param[seg_name]	Name of segment file
 * @param[dir_name]	Name of directory containing the segment
 * @return			Size of the segment file
 */
int rvm_file_size(char * seg_name, char * dir_name)
{
	int size = 0;

	if(dir_name != NULL)
	{
		if(strcmp(dir_name, ".") > 0)
		{
			chdir(dir_name);
		}

		FILE * rvm_file_ptr = fopen(seg_name, "r");
		if(rvm_file_ptr != NULL)
		{
			fseek(rvm_file_ptr, 0, SEEK_END);
			size = ftell(rvm_file_ptr);
			fclose(rvm_file_ptr);
		}

		if(strcmp(dir_name, ".") > 0)
		{
			chdir("..");
		}
	}

	return size;
}

/*
 * @function		rvm_file_write
 * @brief			Writes a segment file for mode 'w+' and appends for mode 'a' with NULL characters
 * @param[seg_name]	Name of segment file
 * @param[seg_size]	Required size of the segment file
 * @param[mode]		'w+' for write mode, 'a' for append mode
 * @return			1 if successful, 0 if erroneous
 */
int rvm_file_write(char * seg_name, int seg_size, char * mode)
{
	FILE * rvm_file_ptr = fopen(seg_name, mode);

	int curr_seg_size = rvm_file_size(seg_name, ".");
	int seg_size_offset = seg_size;
	int i;
	int ret = 0;

	char buff[1];
	buff[0] = '\0';

	if(DEBUG)
	{
		cout << "curr_seg_size = " << curr_seg_size << "\n";
	}

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
