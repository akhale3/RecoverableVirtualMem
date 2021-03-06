#include "rvm.h"

rvm_dir_t * rvm_global_dir_head;
rvm_trans_t * rvm_global_trans_head;

rvm_t rvm_init(const char *directory)
{
	char * dir = strdup(directory);
	int dir_status = rvm_dir_check_exists(dir);
	rvm_t ret = 0;
	int mkdir_status;
	// dir_status can have three values
	// if directory does not exist then return 0
	// if directory exists but not in the directory structure return 1;
	// if directory exists in directory structure then returns 2
	if(dir_status != 0)
	{
		if(dir_status == 2)
		{
			rvm_exit("Directory already exists");
		}
		if(dir_status == 1)
			ret = rvm_dir_create(dir);
	}
	else
	{
		mkdir_status = rvm_dir_mkdir(dir);
		if(mkdir_status == 1)
		{
			ret = rvm_dir_create(dir);
			chdir(dir);
			system("touch rvm.log");
			chdir("..");
		}
		else
		{
			rvm_exit("Directory creation error");
		}
	}

	return ret;
}

void *rvm_map(rvm_t rvm, const char *segname, int size_to_create)
{
	rvm_dir_t * rvm_dir_temp;
	char * dir_name;
	void * ret_pointer = NULL;
	char * rvm_seg = strdup(segname);
	rvm_seg_t * seg_node = (rvm_seg_t *) malloc(sizeof(rvm_seg_t));
	char * mode = "w+";
	size_t size = size_to_create;
	off_t offset = 0;
	int rvm_seg_file;

	if(rvm_seg_mapped(rvm_seg, rvm))
	{
		rvm_exit("Segment already mapped");
	}

	rvm_dir_temp = rvm_dir_get(rvm);

	if (rvm_dir_temp == NULL)
	{
		rvm_exit("Directory not found");
	}

	dir_name = rvm_dir_temp->dir_name;
	if(dir_name == NULL)
	{
		rvm_exit("Directory name empty");
	}

	if(rvm_seg_exists(rvm_seg, rvm))
	{
		chdir(dir_name);

		size = rvm_seg_size(rvm_seg, dir_name);

		if(size < size_to_create)
		{
			mode = "a";
		}
	}

	chdir(dir_name);

	if(rvm_seg_write(rvm_seg, size_to_create, mode) == 0)
	{
		rvm_exit("Segment creation error");
	}

	/* Segment Mapping */
	/*
	rvm_seg_file = open(rvm_seg, O_RDONLY, 0);
	ret_pointer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, rvm_seg_file, offset);
	if(ret_pointer == MAP_FAILED)
	{
		rvm_exit("Mapping unsuccessful");
	}
	 */

	seg_node->seg_name = rvm_seg;
	seg_node->seg_size = size_to_create;
	seg_node->seg_base_addr = malloc(size_to_create);
	seg_node->seg_trans_id = 0;
	seg_node->seg_next = NULL;

	if(rvm_seg_exists(rvm_seg, rvm))
	{
		rvm_log_update(rvm_seg, dir_name);

		void * buff = (void *) malloc((size_t)size);

		chdir(dir_name);

		FILE * rvm_file_ptr = fopen( rvm_seg, "r" );
		fread(buff, 1 ,(size_t) size, rvm_file_ptr);
		fclose(rvm_file_ptr);

		seg_node->seg_base_addr = buff;
	}

	ret_pointer = seg_node->seg_base_addr;

	if(rvm_dir_temp->seg_head == NULL)
	{
		rvm_dir_temp->seg_head = seg_node;
	}
	else
	{
		seg_node->seg_next = rvm_dir_temp->seg_head;
		rvm_dir_temp->seg_head = seg_node;
	}

	chdir("..");

	return ret_pointer;
}

void rvm_unmap(rvm_t rvm, void *segbase)
{
	if(segbase == NULL)
	{
		rvm_exit("Segment not found");
	}

	rvm_redo_delete(segbase);

	rvm_seg_delete(segbase, rvm);
}

void rvm_destroy(rvm_t rvm, const char *segname)
{
	char * seg = strdup(segname);
	if(rvm_seg_mapped(seg, rvm))
	{
		rvm_exit("Segment already mapped. Cannot be destroyed");
	}

	rvm_dir_t * dir;
	dir = rvm_dir_get(rvm);

	if(dir)
	{
		chdir(dir->dir_name);
		if(rvm_seg_exists(seg,rvm))
		{
			if(rvm_seg_delete(seg, rvm) != 0)
			{
				rvm_exit("Segment not destroyed");
			}
			/*
			else
			{
				cout << "Segment Destroyed successfully" << "\n";
			}
			 */
		}
		chdir("..");
	}
}

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{
	int i = 0, j = 0;
	rvm_trans_t * temp = rvm_global_trans_head;
	if(temp != NULL)
	{
		for(i = 0; i < numsegs; i++)
		{
			while(temp != NULL)
			{
				for(j = 0; j < temp->trans_seg_count; j++)
				{
					if(segbases[i] == temp->trans_seg_bases[j])
					{
						cout << "Segment is being modified by other transaction\n";
						return (trans_t) -1;
					}
				}
				temp = temp->trans_next;
			}
			temp = rvm_global_trans_head;
		}
	}

	return rvm_trans_create(rvm, numsegs, segbases);
}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{
	int i = 0, flag = 0;
	rvm_trans_t * temp;
	temp = rvm_trans_get(tid);
	if(temp == NULL)
	{
		rvm_exit("Transaction not found");
	}

	for(i = 0; i < temp->trans_seg_count; i++)
	{
		if(temp->trans_seg_bases[i] == segbase)
		{
			flag = 1;
			break;
		}
	}

	if(flag == 0)
	{
		rvm_exit("Segment is not present in this transaction");
	}

	rvm_seg_t * temp_seg = rvm_seg_get(segbase, temp->trans_dir_id);
	char * seg_name = temp_seg->seg_name;
	rvm_redo_t * redo_log = (rvm_redo_t *) malloc(sizeof(rvm_redo_t));
	redo_log->offset = offset;
	redo_log->size = size;
	redo_log->rvm_redo_next = NULL;
	redo_log->seg_name = strdup(seg_name);
	redo_log->seg_base_addr = segbase;
	redo_log->offset_data = malloc(size);
	memcpy(redo_log->offset_data, segbase + offset, size);

	if(temp->rvm_redo_head == NULL)
	{
		temp->rvm_redo_head = redo_log;
	}
	else
	{
		rvm_redo_t *redo = temp->rvm_redo_head;
		while(redo->rvm_redo_next != NULL)
		{
			redo = redo->rvm_redo_next;
		}
		redo->rvm_redo_next = redo_log;
	}
}

void rvm_commit_trans(trans_t tid)
{
	rvm_trans_t * rvm_trans_temp = rvm_trans_get(tid);

	if(rvm_trans_temp == NULL)
	{
		rvm_exit("Transaction not found");
	}

	rvm_dir_t * rvm_dir_temp = rvm_dir_get(rvm_trans_temp->trans_dir_id);

	if(rvm_dir_temp == NULL)
	{
		rvm_exit("Directory does not exist");
	}

	rvm_redo_t * rvm_redo_temp = rvm_trans_temp->rvm_redo_head;

	chdir(rvm_dir_temp->dir_name);

	while(rvm_redo_temp != NULL)
	{
		char * data = (char *) malloc(rvm_redo_temp->size);
		if(data == NULL)
		{
			rvm_exit("Insufficient memory");
		}
		memcpy(data, rvm_redo_temp->seg_base_addr + rvm_redo_temp->offset, rvm_redo_temp->size);
		rvm_log_write(rvm_redo_temp->seg_name, rvm_redo_temp->offset, rvm_redo_temp->size, data);
		rvm_redo_temp->offset_data = NULL;
		rvm_redo_temp = rvm_redo_temp->rvm_redo_next;
	}

	rvm_trans_delete(tid);

	chdir("..");
}

void rvm_abort_trans(trans_t tid)
{
	rvm_trans_t * temp;
	temp = rvm_trans_get(tid);

	if(temp == NULL)
	{
		rvm_exit("Transaction for this transaction ID not found");
	}

	rvm_redo_t * redo_rec = temp->rvm_redo_head;

	if(redo_rec != NULL)
	{
		while(redo_rec != NULL)
		{
			memcpy(redo_rec->seg_base_addr + redo_rec->offset, redo_rec->offset_data, redo_rec->size);
			redo_rec = redo_rec->rvm_redo_next;
		}

		temp->rvm_redo_head = NULL;
		rvm_trans_delete(tid);
	}
}

void rvm_truncate_log(rvm_t rvm)
{
	rvm_dir_t * temp = rvm_dir_get(rvm);

	if(temp == NULL)
	{
		rvm_exit("Can not truncate, directory does not exists");
	}

	chdir(temp->dir_name);

	rvm_seg_t * temp_seg;
	temp_seg = temp->seg_head;
	while(temp_seg != NULL)
	{
		rvm_log_update(temp_seg->seg_name, temp->dir_name);
		temp_seg = temp_seg->seg_next;
	}

	chdir("..");
}
