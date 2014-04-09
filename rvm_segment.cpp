/*
 * rvm_segment.cpp
 *
 *  Created on: Apr 8, 2014
 *      Authors: Anish K. and Nikita G.
 */

/*
 * TODO:
 * 1. Complete interface definitions.
 */

/*
 * @function		rvm_seg_mapped
 * @brief			Checks whether a segment has already been mapped
 * @param[seg_name]	Name of segment
 * @param[dir_id]	Directory ID containing seg_name
 * @return			1 if mapped, 0 if not
 */
int rvm_seg_mapped(char * seg_name, rvm_t dir_id)
{

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

}

/*
 * @function		rvm_seg_delete
 * @brief			Deletes a segment
 * @param[seg_base]	Base address of the segment as passed in rvm_unmap()
 * @param[dir_id]	Directory ID containing seg_base
 * @return			None
 */
void rvm_seg_delete(void * seg_base, rvm_t dir_id)
{

}
