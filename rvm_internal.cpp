/*
 * rvm_internal.cpp
 *
 *  Created on: Apr 17, 2014
 *      Authors: Anish K. and Nikita G.
 */

#include "rvm_internal.h"

/*** Global ***/
rvm_t rvm_global_dir_id;
trans_t rvm_global_trans_id;

void rvm_exit(char * err_msg)
{
	perror(err_msg);
	exit(0);
}

/*** Directory ***/
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

/*** Log ***/
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
 * @return			None
 */
void rvm_log_update(char * seg_name, char * dir)
{
	FILE * rvm_file_ptr, * rvm_backup_ptr;
	int temp_size = 100000;
	char temp[temp_size];
	int size, offset, status;

	chdir(dir);

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

	while(!feof(rvm_file_ptr))
	{
		if(fgets(temp, temp_size, rvm_file_ptr))
		{
			if(strncmp(temp, seg_name, sizeof(temp) - 1) != 10)
			{
				/* Got seg_name
								Get size, offset and data
								Effectively, skip segment */
				/*
								for(i = 0; i < 3; i++)
								{
									fgets(temp, 100000, rvm_file_ptr);
								}
				 */
				fwrite(temp, strlen(temp), 1, rvm_backup_ptr);	// Segment Name

				fgets(temp, temp_size, rvm_file_ptr);
				fwrite(temp, strlen(temp), 1, rvm_backup_ptr);	// Size
				temp[strlen(temp) - 1] = NULL;
				size = atoi(temp);

				fgets(temp, temp_size, rvm_file_ptr);
				fwrite(temp, strlen(temp), 1, rvm_backup_ptr);	// Offset

				char * buff = (char *) malloc(size);
				fread(buff, size, 1, rvm_file_ptr);
				fwrite(buff, size, 1, rvm_backup_ptr);			// Data
			}
			else
			{
				//fprintf(rvm_backup_ptr, "%s", temp);
				fgets(temp, temp_size, rvm_file_ptr);
				temp[strlen(temp) - 1] = NULL;
				size = atoi(temp);

				fgets(temp, temp_size, rvm_file_ptr);
				temp[strlen(temp) - 1] = NULL;
				offset = atoi(temp);

				char * buff = (char *) malloc(size);
				fread(buff, size, 1, rvm_file_ptr);

				status = rvm_seg_update(seg_name, size, offset, buff, dir);
			}
		}
	}

	fclose(rvm_backup_ptr);
	fclose(rvm_file_ptr);

	chdir(dir);

	system("mv rvm.log.tmp rvm.log");

	chdir("..");
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

	if(rvm_trans_curr != NULL)
	{
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

				rvm_redo_prev = rvm_redo_curr;
				rvm_redo_curr = rvm_redo_curr->rvm_redo_next;
			}

			rvm_trans_curr = rvm_trans_curr->trans_next;
		}
	}
}

/*** Segment ***/
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
	//chdir(dir_name);
	struct stat buff;
	int status = -1;
	status = stat(seg_name, &buff);
	/* Switch back to root directory */
	//chdir("..");

	return (status + 1);
}

/*
 * @function				rvm_seg_delete
 * @brief					Deletes a segment
 * @param[seg_base_addr]	Base address of the segment as passed in rvm_unmap()
 * @param[dir_id]			Directory ID containing seg_base
 * @return					1 if successful, 0 if erroneous
 */
int rvm_seg_delete(void * seg_base_addr, rvm_t dir_id)
{
	rvm_dir_t * rvm_dir;
	rvm_dir = rvm_dir_get(dir_id);
	int ret = 0;

	if(rvm_dir == NULL)
	{
		rvm_exit("Unknown directory");
	}

	rvm_seg_t * rvm_seg_curr;
	rvm_seg_curr = rvm_dir->seg_head;

	/*
	if(rvm_seg_curr == NULL)
	{
		rvm_exit("Segment does not exist");
	}
	 */

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

			ret = 1;
			break;
		}
		else
		{
			rvm_seg_prev = rvm_seg_curr;
			rvm_seg_curr = rvm_seg_curr->seg_next;
		}
	}

	return ret;
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
	buff[0] = NULL;

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
 * @function				rvm_seg_get
 * @brief					Returns a segment structure pointer for a particular seg_base
 * @param[seg_base_addr]	Base address pointer of a segment
 * @param[dir_id]			Directory ID
 * @return					(rvm_seg_t *) if successful, NULL if erroneous
 */
rvm_seg_t * rvm_seg_get(void * seg_base_addr, rvm_t dir_id)
{
	rvm_dir_t * rvm_dir;
	rvm_dir = rvm_dir_get(dir_id);
	rvm_seg_t * ret = NULL;

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

	while(rvm_seg_curr != NULL)
	{
		if(rvm_seg_curr->seg_base_addr == seg_base_addr)
		{
			ret = rvm_seg_curr;
			break;
		}
		else
		{
			rvm_seg_curr = rvm_seg_curr->seg_next;
		}
	}

	return ret;
}

/*
 * @function		rvm_seg_update
 * @brief			Writes data to segment file
 * @param[seg_name]	Name of segment file
 * @param[size]		Size of data
 * @param[offset]	Offset to next data
 * @param[data]		Data to be written
 * @param[dir]		Directory name
 * @return			1 if successful, 0 if erroneous
 */
int rvm_seg_update(char * seg_name, int size, int offset, char * data, char * dir)
{
	int rvm_seg_file;
	int ret = 1;
	chdir(dir);
	rvm_seg_file = open(seg_name, O_RDWR);
	if(rvm_seg_file < 0)
	{
		rvm_exit("Error opening file");
	}

	void * rvm_seg_backup;
	struct stat buff;

	if (fstat(rvm_seg_file, &buff) < 0)
	{
		rvm_exit("Error fetching file size");
	}

	rvm_seg_backup = mmap(NULL, buff.st_size, PROT_WRITE, MAP_SHARED, rvm_seg_file, 0);
	memcpy(rvm_seg_backup + offset, data, size * sizeof (char));
	munmap(rvm_seg_backup, buff.st_size);

	chdir("..");
	return ret;
}

/*** Transaction ***/
/*
 * @function			rvm_trans_create
 * @brief				Inserts a transaction structure to the head of the transaction linked list
 * @param[rvm]			Directory ID
 * @param[numsegs]		Number of segments
 * @param[**segbases]	Pointer to pointers of segment base addresses
 * @return				Transaction ID
 */
trans_t rvm_trans_create(rvm_t rvm, int numsegs, void **segbases)
{
	int i = numsegs - 1;
	rvm_trans_t * rvm_trans_temp;
	rvm_trans_temp = (rvm_trans_t *) malloc(sizeof(rvm_trans_t));
	if(rvm_trans_temp == NULL)
	{
		rvm_exit("Insufficient memory");
	}

	rvm_trans_temp->trans_id = ++rvm_global_trans_id;
	rvm_trans_temp->trans_seg_bases = (void **) malloc(numsegs * sizeof(void *));
	if(rvm_trans_temp->trans_seg_bases == NULL)
	{
		rvm_exit("Insufficient memory");
	}
	while(i >= 0)
	{
		rvm_trans_temp->trans_seg_bases[i] = segbases[i];
		i--;
	}
	rvm_trans_temp->trans_seg_count = numsegs;
	rvm_trans_temp->trans_dir_id = rvm;
	rvm_trans_temp->rvm_redo_head = NULL;
	rvm_trans_temp->trans_next = NULL;

	if(rvm_global_trans_head != NULL)
	{
		// Add newly created transaction structure to the head of the linked list
		rvm_trans_temp->trans_next = rvm_global_trans_head;
	}

	rvm_global_trans_head = rvm_trans_temp;

	return rvm_global_trans_id;
}

/*
 * @function		rvm_trans_delete
 * @brief			Deletes a particular transaction from the transaction linked list
 * @param[trans_id]	Transaction identifier
 * @return			None
 */
void rvm_trans_delete(trans_t trans_id)
{
	rvm_trans_t * rvm_trans_curr;
	rvm_trans_curr = rvm_global_trans_head;

	if(rvm_trans_curr == NULL)
	{
		rvm_exit("Transaction list empty");
	}

	rvm_trans_t * rvm_trans_prev;
	rvm_trans_prev = rvm_trans_curr;

	while(rvm_trans_curr != NULL)
	{
		if(rvm_trans_curr->trans_id == trans_id)
		{
			rvm_trans_prev->trans_next = rvm_trans_curr->trans_next;
			if(rvm_trans_curr == rvm_global_trans_head)
			{
				rvm_global_trans_head = NULL;
			}
			return;
		}
		else
		{
			rvm_trans_prev = rvm_trans_curr;
			rvm_trans_curr = rvm_trans_curr->trans_next;
		}
	}

	return;
}

/*
 * @function		rvm_trans_get
 * @brief			Returns a transaction structure pointer for a particular transaction
 * @param[trans_id]	Transaction identifier
 * @return			(rvm_trans_t *) if successful, NULL if erroneous
 */
rvm_trans_t * rvm_trans_get(trans_t trans_id)
{
	rvm_trans_t * rvm_trans_curr;
	rvm_trans_curr = rvm_global_trans_head;

	rvm_trans_t * ret = NULL;

	if(rvm_trans_curr == NULL)
	{
		rvm_exit("Transaction list empty");
	}

	while(rvm_trans_curr != NULL)
	{
		if(rvm_trans_curr->trans_id == trans_id)
		{
			ret = rvm_trans_curr;
		}
		rvm_trans_curr = rvm_trans_curr->trans_next;
	}

	return ret;
}
