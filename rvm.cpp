#include "rvm.h"

rvm_t rvm_init(const char *directory)
{
	char * dir = strdup(directory);
	int dir_status = rvm_dir_check_exists(dir);
	rvm_t ret = 0;
	int mkdir_status;
	// dir status can have three values
	// if does not exist in file then return 0
	// if exists but not in the directory structure return 1;
	// if exists in directory structure then returns 2
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

	seg_node->seg_name = segname;
	seg_node->seg_size = size_to_create;
	seg_node->seg_base_addr =  malloc(size_to_create);
	seg_node->seg_trans_id = 0;
	seg_node->seg_next = NULL;


	if(rvm_seg_mapped(seg, rvm))
	  {
	    cout<<"segment mapped already, can not map again";
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

	    int size = rvm_file_size(seg, curr_dir);
	    if(size == 0)
	    {
	    	if(!(rvm_file_write(seg,size_to_create, "w+")))
	    	{
	    		cout<<"segment failed to write";
	    	}
	    }
	    if(size < size_to_create)
	      {
	    	if(!(rvm_file_write(seg, size_to_create, "a")))
	        {
	    		cout<<"segment failed to append size";
	        }
	      }
	    //This function updates the segment file and the log file both
//	    gt_fileLookupLog("LogFile", segname,curr_dir);

	    rvm_dir_t *temp = rvm_dir_get(rvm);
	    //Check if rvm directory exists

	    void *buf = (void *) malloc((size_t)size);
	    chdir(curr_dir);
	    FILE *f = fopen( seg, "r" );
	    fread(buf, 1 ,(size_t) size, f);
	    fclose(f);

	    //Does this even work?
	    seg_node->seg_base_addr = buf;

	    if(temp->seg_head == NULL)
	      temp->seg_head = seg_node;
	    else
	    {
	      seg_node->seg_next = temp->seg_head;
	      temp->seg_head = seg_node;
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
	    if(chdir(temp->dir_name)==-1) printf("ERROR!");

	    //temp_ret = gt_fileWriteSeg(segname, size_to_create);
	    if(temp_ret == 0)
	      cout<<"segment file could not be updated \n";

	    chdir("..");
	    ret_pointer = seg_node->seg_base_addr;
	    return seg_node->seg_base_addr;
	  }
	}




}

void rvm_unmap(rvm_t rvm, void *segbase)
{

}

void rvm_destroy(rvm_t rvm, const char *segname)
{

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
