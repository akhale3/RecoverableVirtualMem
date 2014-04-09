#include "rvm.h"
#include<iostream>
#include<stdlib.h>

using namespace std;
rvm_t rvm_init(const char *directory)
{
	char* dir= directory;
	int dir_status= rvm_dir_check_exists(dir);
	// dir status can have three values
	// if doesnot exist in file then return 0
	// if exists but not in the directory structure return 1;
	// if exists in directory structure then returns 2
	if(dir_status!=0)
	{
		if(dir_status==2) exit(0);
		if(dir_status==1)
		return rvm_dir_create(dir);
	}
	else
	{
		int ret = rvm_dir_mkdir(dir);
		return rvm_dir_create(dir);
	}


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
