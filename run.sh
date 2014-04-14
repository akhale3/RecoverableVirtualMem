# Clean, make, link, run!

make clean
make
IFS='.' read -ra ADDR <<< "$1"
g++ "$1" librvm.a -o "${ADDR[0]}"