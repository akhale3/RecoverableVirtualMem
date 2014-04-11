#include "rvm.h"

rvm_t rvm_init(const char *directory)
{
	char * dir = strdup(directory);
	int dir_status= rvm_dir_check_exists(dir);
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
//			rvm_exit("Directory exists");
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
//			rvm_exit("Directory creation error");
		}
	}

	return ret;
}

void *rvm_map(rvm_t rvm, const char *segname, int size_to_create)
{

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
