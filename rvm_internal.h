#ifndef RVM_INTERNAL_H_
#define RVM_INTERNAL_H_

/*** Types ***/
#include<iostream>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<cstring>

using namespace std;

/*** Global ***/
// rvm_t stores the entry point for the directory structure, i.e. dir_id
typedef int rvm_t;
typedef int trans_t;

extern rvm_t rvm_global_dir_id;
extern trans_t rvm_global_trans_id;

void rvm_exit(char * err_msg);

/*** Directory ***/
typedef struct rvm_dir
{
	rvm_t				dir_id;
	char *				dir_name;
	struct rvm_seg *	seg_head;
	struct rvm_dir *	dir_next;
}	rvm_dir_t;

extern rvm_dir_t * rvm_global_dir_head;

int rvm_dir_check_exists(char * dir_name);	// Checks if directory exists.
int rvm_dir_mkdir(char * dir_name);			// Makes a directory at current location.
rvm_t rvm_dir_create(char * dir_name);		// Populates the directory structure and appends to the end of the directory linked list.
rvm_dir_t * rvm_dir_get(rvm_t dir_id);		// Returns the directory structure by looking up with dir_id.

/*** Log ***/
// Redo record in memory
typedef struct rvm_redo
{
	char *				seg_name;
	int 				size;
	int 				offset;
	void *				seg_base_addr;
	void *				offset_data;
	struct rvm_redo *	rvm_redo_next;
} rvm_redo_t;

int rvm_log_write(char * seg_name, int size, int offset, char * data);	// Appends to a log file for a particular backing store
void rvm_log_update(char * seg_name, char * dir);						// Update the segment entry in the log file in the backing store
void rvm_redo_delete(void * seg_base_addr);								// Deletes all redo records of all transactions from memory

/*** Segment ***/
typedef struct rvm_seg
{
	char *				seg_name;
	void *				seg_base_addr;
	int					seg_size;
	trans_t				seg_trans_id;
	struct rvm_seg *	seg_next;
}	rvm_seg_t;

int rvm_seg_mapped(char * seg_name, rvm_t dir_id);						// Checks whether a segment has already been mapped.
int rvm_seg_exists(char * seg_name, rvm_t dir_id);						// Checks whether a segment already exists on disk.
int rvm_seg_delete(void * seg_base_addr, rvm_t dir_id);					// Deletes a segment.
int rvm_seg_size(char * seg_name, char * dir_name);						// Returns file size in bytes of seg_name for comparison with size_to_create.
int rvm_seg_write(char * seg_name, int seg_size, char * mode);			// Writes a segment file for mode 'w+' and appends for mode 'a' with NULL characters.
rvm_seg_t * rvm_seg_get(void * seg_base_addr, rvm_t dir_id);			// Returns a segment structure pointer for a particular segbase.
int rvm_seg_update(char * seg_name, int size,
		int offset, char * data, char * dir);							// Writes data to segment file.

/*** Transaction ***/
typedef struct rvm_trans
{
	trans_t				trans_id;
	void **				trans_seg_bases;
	int 				trans_seg_count;
	rvm_t				trans_dir_id;
	struct rvm_redo *	rvm_redo_head;
	struct rvm_trans *	trans_next;
} rvm_trans_t;

extern rvm_trans_t * rvm_global_trans_head;

trans_t rvm_trans_create(rvm_t rvm, int numsegs, void **segbases);	// Inserts a transaction structure to the head of the transaction linked list.
void rvm_trans_delete(trans_t trans_id);							// Deletes a particular transaction from the transaction linked list.
rvm_trans_t * rvm_trans_get(trans_t trans_id);						// Returns a transaction structure pointer for a particular transaction.

#endif /* RVM_INTERNAL_H_ */
