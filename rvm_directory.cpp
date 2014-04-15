/*
 * rvm_directory.cpp
 *
 *  Created on: Apr 7, 2014
 *      Authors: Anish K. and Nikita G.
 */

#include "rvm_directory.h"
/*
 * TODO:
 * 1. Complete all interface definitions.
 */

/*
 * @function		rvm_dir_check_exists
 * @brief			Checks if directory exists
 * @param[dir_name]	Name of directory
 * @return			2 if exists, 1 if it doesn't, 0 if erroneous
 */
int rvm_dir_check_exists(char * dir_name)
{
	DIR * directory = opendir(dir_name);
	int ret = 0;

	if(directory != NULL)
	{
		rvm_dir_t * temp;
		temp = rvm_global_dir_head;

		ret = 1;

		while(temp != NULL)
		{
			if(strcmp(dir_name, temp->dir_name) == 0)
			{
				cout << "File present in directory list";
			}
			temp = temp->dir_next;
			ret = 2;
		}
	}

	return ret;
}

/*
 * @function		rvm_dir_mkdir
 * @brief			Makes a directory at current location
 * @param[dir_name]	Name of directory
 * @return			1 if successful, 0 if erroneous
 */
int rvm_dir_mkdir(char * dir_name)
{
	char * mkdir = (char *) malloc(512);
	strcpy(mkdir, "mkdir -p ");
	strcat(mkdir,dir_name);
	int ret = 0;
	int status = system(mkdir);

	if(status != -1)
	{
		ret = 1;
	}

	return ret;
}

/*
 * @function		rvm_dir_create
 * @brief			Populates the directory structure and appends to the end of the directory linked list.
 * @param[dir_name]	Name of directory
 * @return			Directory ID
 */
rvm_t rvm_dir_create(char * dir_name)
{
	char *temp = strdup(dir_name);

	// Initialize the directory structure
	rvm_dir_t * dir_node;
	dir_node = (rvm_dir_t *) malloc(sizeof(rvm_dir_t));
	dir_node->dir_name = temp;
	dir_node->dir_next = NULL;
	dir_node->dir_id = ++rvm_global_dir_id;
	dir_node->seg_head = NULL;

	if(rvm_global_dir_head != NULL)
	{
		// Add the newly created directory node to the head of the directory list
		dir_node->dir_next = rvm_global_dir_head;
	}

	rvm_global_dir_head = dir_node;

	return dir_node->dir_id;
}

/*
 * @function		rvm_dir_get
 * @brief			Returns the directory structure by looking up with dir_id
 * @param[dir_id]	Directory ID
 * @return			Directory structure
 */
rvm_dir_t * rvm_dir_get(rvm_t dir_id)
{
	rvm_dir_t * temp;
	temp = rvm_global_dir_head;

	rvm_dir_t * ret = NULL;

	if(temp == NULL)
	{
		rvm_exit("Directory list empty");
	}

	while(temp != NULL)
	{
		if(dir_id == temp->dir_id)
		{
			ret = temp;
		}
		temp = temp->dir_next;
	}

	return ret;
}
