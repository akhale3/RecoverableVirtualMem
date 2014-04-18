#!/bin/sh
# Clean, make, link, run!

# Check for correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: sh run.sh testfile.extension"
exit 1
fi

# Check if testfile.extenstion exists in the current directory
if [ -f "$1" ]; then
    make clean
    make
    rm *.o
    IFS='.' read -ra ADDR <<< "$1"
    g++ "$1" librvm.a -o "${ADDR[0]}"
    clear
    ./"${ADDR[0]}"
else
    echo "$1 does not exist"
fi