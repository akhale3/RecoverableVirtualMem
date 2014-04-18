RecoverableVirtualMem
=====================

A recoverable virtual memory system for creating persistent segments of memory accessible by sequential transactions.

### Execution
Just run the script file run.sh <testfile_name.c> in the root directory of the project folder.

### Testing
This code was tested on a Linux x86-64 SMP on the Killerbee cluster at Georgia Tech.

### Log File
The structure of the log file, rvm.log, contains:
###### Segment name
###### Segment size
###### Segment offset
###### Segment data
Truncation of the log files is handled by the rvm_truncate() function. A call to this function with a global directory ID (rvm) as the parameter results in creation of a temporary log file, viz. rvm.log.tmp, containing all uncommitted log segment entries. At the end of the function, the actual log file (rvm.log) is overwritten by the temporary log file.
