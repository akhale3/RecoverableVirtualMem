/*
 * rvm_transaction.cpp
 *
 *  Created on: Apr 15, 2014
 *      Authors: Anish K. and Nikita G.
 */

#include "rvm_transaction.h"

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
	int i = numsegs;
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

	return rvm_trans_temp->trans_id;
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
