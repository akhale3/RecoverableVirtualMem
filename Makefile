#### RVM Library Makefile

CFLAGS  = -Wall -g -I.
LFLAGS  =
CC      = g++
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib

LIBRARY = librvm.a

LIB_SRC = rvm.cpp rvm_global.cpp rvm_directory.cpp rvm_segment.cpp rvm_log.cpp rvm_transaction.cpp

LIB_OBJ = $(patsubst %.cpp,%.o,$(LIB_SRC))

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(LIBRARY): $(LIB_OBJ)
	$(AR) $(LIBRARY) $(LIB_OBJ)
	$(RANLIB) $(LIBRARY)

clean:
	$(RM) $(LIBRARY) $(LIB_OBJ)
