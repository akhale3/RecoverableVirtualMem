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
	rvm_dir_t *temp;
	char *curr_dir;
	void *ret_pointer = NULL;
	char *seg = strdup(segname);
	rvm_seg_t *seg_node = (rvm_seg_t *) malloc(sizeof(rvm_seg_t));

	seg_node->seg_name = seg;
	seg_node->seg_size = size_to_create;
	seg_node->seg_base_addr =  malloc(size_to_create);
	seg_node->seg_trans_id = 0;
	seg_node->seg_next = NULL;


	if(rvm_seg_mapped(seg, rvm))
	{
		rvm_exit("segment mapped already, can not map again");
	}
	if(rvm_seg_exists(seg, rvm))
	{
		temp = rvm_dir_get(rvm);

		if (temp == NULL)
			cout<<"directory not found";
		else
		{

			curr_dir = temp->dir_name;
			if(curr_dir==NULL)
				cout<<"NULL DIRECTORY";
		}

		if(chdir(curr_dir)==-1) exit(0);

		int size = rvm_seg_size(seg, curr_dir);
		if(size == 0)
		{
			if(!(rvm_seg_write(seg,size_to_create, "w+")))
			{
				cout<<"segment failed to write";
			}
		}
		if(size < size_to_create)
		{
			if(!(rvm_seg_write(seg, size_to_create, "a")))
			{
				cout<<"segment failed to append size";
			}
		}
		//This function updates the segment file and the log file both
		//	    gt_fileLookupLog("LogFile", segname,curr_dir);


		rvm_dir_t *temp1 = rvm_dir_get(rvm);

		//Check if rvm directory exists

		void *buf = (void *) malloc((size_t)size);
		chdir(curr_dir);
		FILE *f = fopen( seg, "r" );
		fread(buf, 1 ,(size_t) size, f);
		fclose(f);

		//This is where it maps
		seg_node->seg_base_addr = buf;

		if(temp1->seg_head == NULL)
			temp1->seg_head = seg_node;
		else
		{
			seg_node->seg_next = temp1->seg_head;
			temp1->seg_head = seg_node;
		}

		chdir("..");
		ret_pointer = seg_node->seg_base_addr;
		return ret_pointer;
	}
	else
	{
		int temp_ret;


		rvm_dir_t *temp = rvm_dir_get(rvm);
		if(temp==NULL)
			cout<<"Segment not initialized\n";

		//Insert this segment into the directory's segment list here
		if(temp->seg_head == NULL)
			temp->seg_head = seg_node;
		else
		{
			seg_node->seg_next = temp->seg_head;
			temp->seg_head = seg_node;
		}

		//Go into this directory now.
		if(chdir(temp->dir_name)==-1) exit(0);

		temp_ret = rvm_seg_write(seg, size_to_create, "w+");
		if(temp_ret == 0)
			cout<<"segment file could not be updated \n";

		chdir("..");
		ret_pointer = seg_node->seg_base_addr;
		return ret_pointer;
	}
}






void rvm_unmap(rvm_t rvm, void *segbase)
{
	//before deleting segment make sure all logs and transactions are deleted from that seg
	rvm_seg_delete(segbase, rvm);

}

void rvm_destroy(rvm_t rvm, const char *segname)
{
	  char * seg = strdup(segname);
	  if(rvm_seg_mapped(seg, rvm))
	  {
	    rvm_exit("Segment already mapped. Cannot be destroyed");
	  }

	  rvm_dir_t *dir;
	  dir= rvm_dir_get(rvm);

	  if(dir)
	  {

	    chdir(dir->dir_name);
	    if(rvm_seg_exists(seg,rvm))
	      {
	    	if(rvm_seg_delete(seg, rvm)!=0)
	      	    {
	    			rvm_exit("Segment not destroyed");
	      	    }
	    	else
	    		{
	    			cout<<"Segment Destroyed successfully";
	    		}
	      }
	    chdir("..");
	  }
}



trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{

}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{

}

void rvm_commit_trans(trans_t tid)
{

}

void rvm_abort_trans(trans_t tid)
{

}

void rvm_truncate_log(rvm_t rvm)
{

}
