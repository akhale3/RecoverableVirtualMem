#include "rvm.h"

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
	FILE * rvm_seg_file;

	if(rvm_seg_mapped(rvm_seg, rvm))
	{
		rvm_exit("Segment already mapped");
	}

	rvm_dir_temp = rvm_dir_get(rvm);

	if (rvm_dir_temp == NULL)
	{
		rvm_exit("Directory not found");
	}
	else
	{
		dir_name = rvm_dir_temp->dir_name;
		if(dir_name == NULL)
		{
			rvm_exit("Directory name empty");
		}
	}

	if(rvm_seg_exists(rvm_seg, rvm))
	{
		size = rvm_seg_size(rvm_seg, dir_name);

		if(size < size_to_create)
		{
			mode = "a";
		}
		//This function updates the segment file and the log file both
		//	    gt_fileLookupLog("LogFile", segname,curr_dir);
	}

	if(rvm_seg_write(rvm_seg, size_to_create, mode) == 0)
	{
		rvm_exit("Segment creation error");
	}

	chdir(dir_name);

	/* Segment Mapping */
	rvm_seg_file = open(rvm_seg, O_RDONLY, 0);
	ret_pointer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, rvm_seg_file, offset);
	if(ret_pointer == MAP_FAILED)
	{
		rvm_exit("Mapping unsuccessful");
	}

	seg_node->seg_name = rvm_seg;
	seg_node->seg_size = size_to_create;
	seg_node->seg_base_addr = ret_pointer;
	seg_node->seg_trans_id = 0;
	seg_node->seg_next = NULL;

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
	//before deleting segment make sure all logs and transactions are deleted from that seg
	//consider closing file pointer: rvm_seg_file
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
			else
			{
				cout << "Segment Destroyed successfully" << "\n";
			}
		}
		chdir("..");
	}
}

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{
	int i = 0,j = 0;
	rvm_trans_t *temp;
	if(temp != NULL)
		for(i = 0; i < numsegs; i++)
		{
			while(temp != NULL)
			{
				for(j = 0; j < temp->trans_seg_count; j++)
				{
					if(segbases[i] == temp->trans_seg_bases[j])
					{
						cout<<"segment is modified by other transaction\n";
						return (trans_t)-1;
					}
				}
				temp = temp->trans_next;
			}
			temp = rvm_global_trans_head;
		}

	return rvm_trans_create(rvm, numsegs, segbases);

}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{

	int i = 0, flag = 0;
	rvm_trans_t *temp;
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
	char *seg_name = temp_seg->seg_name;
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
	char * data = (char *) malloc(rvm_redo_temp->size);

	if(data == NULL)
	{
		rvm_exit("Insufficient memory");
	}

	chdir(rvm_dir_temp->dir_name);

	while(rvm_redo_temp != NULL)
	{
		memcpy(data, rvm_redo_temp->seg_base_addr + rvm_redo_temp->offset, rvm_redo_temp->size);
		rvm_log_write(rvm_redo_temp->seg_name, rvm_redo_temp->offset, rvm_redo_temp->size, data);
		rvm_redo_temp = rvm_redo_temp->rvm_redo_next;
	}

	rvm_trans_delete(tid);

	chdir("..");
}

void rvm_abort_trans(trans_t tid)
{

}

void rvm_truncate_log(rvm_t rvm)
{

}
