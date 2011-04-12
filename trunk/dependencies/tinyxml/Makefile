#tinyxml makefile
#generated for Aspen <http://tds-solutions.net/aspen>
AR = ar
RANLIB = ranlib
CC	= g++
CFLAGS	= -O3 -s -pedantic -Wall -Werror
OUTPUT = ../../libs/libtinyxml.a
S_FILES = tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
#DEFINES = -D TIXML_USE_STL
O_FILES := $(patsubst %.cpp,%.o, $(filter %.cpp, $(S_FILES)))
all: $(O_FILES)
	@rm -f $(OUTPUT)
	@$(AR) cq $(OUTPUT) $(O_FILES)
	@$(RANLIB) $(OUTPUT)
	@cp tinyxml.h ../../includes
	@cp tinystr.h ../../includes
.cpp.o: all
		@echo `date '+%H:%M:%S:'` Compiling $<.
	@$(CC) -c $(CFLAGS) -o $(patsubst %.cpp,%.o, $<) $<
clean:
	@rm -f *.o
	@rm -f $(OUTPUT)
	