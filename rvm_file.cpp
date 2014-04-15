/*
 * rvm_file.cpp
 *
 *  Created on: Apr 12, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. rvm_file_update()
 * 2. rvm_log_delete()
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

/*
 * @function		rvm_log_write
 * @brief			Appends to a log file for a particular segment
 * @param[seg_name]	Name of segment file
 * @param[log_size]	Size of the data to be written in the log file
 * @param[offset]	Can be used to calculate the beginning of the next log segment
 * @param[data]		Data to be written to the log file
 * @return			1 if successful, 0 if erroneous
 */
int rvm_log_write(char * seg_name, int log_size, int offset, char * data)
{
	FILE * rvm_file_ptr;
	int ret = 1;

	rvm_file_ptr = fopen("rvm.log", "a+");
	if(rvm_file_ptr == NULL)
	{
		rvm_exit("Log does not exist");
	}

	fprintf(rvm_file_ptr, "%s\n", seg_name);
	fprintf(rvm_file_ptr, "%d\n", log_size);
	fprintf(rvm_file_ptr, "%d\n", offset);
	fwrite(data, log_size, 1, rvm_file_ptr);

	fclose(rvm_file_ptr);

	return ret;
}

/*
 * @function		rvm_log_write
 * @brief			Delete the log file from the segment
 * @param[seg_name]	Name of segment file
 * @param[log_size]	Size of the data to be written in the log file
 * @param[offset]	Can be used to calculate the beginning of the next log segment
 * @param[data]		Data to be written to the log file
 * @return			1 if successful, 0 if erroneous
 */
int rvm_log_delete(char * seg_name)
{

}
