#Angelscript Makefile-Heavily modified
#generated for Aspen <http://tds-solutions.net/aspen>
AR = ar
RANLIB = ranlib
CC	= g++
CFLAGS	= -O3 -s
CFLAGS += -I../../includes
OUTPUT = ../../libs/libangelscript.a

S_FILES = as_atomic.cpp as_builder.cpp as_bytecode.cpp as_callfunc.cpp as_callfunc_arm.cpp \
  as_callfunc_mips.cpp as_callfunc_ppc.cpp as_callfunc_ppc_64.cpp as_callfunc_sh4.cpp as_callfunc_x86.cpp \
  as_callfunc_x64_gcc.cpp as_compiler.cpp as_context.cpp as_configgroup.cpp as_datatype.cpp \
  as_generic.cpp as_gc.cpp as_globalproperty.cpp as_memory.cpp as_module.cpp \
  as_objecttype.cpp as_outputbuffer.cpp as_parser.cpp as_restore.cpp as_scriptcode.cpp\
  as_scriptengine.cpp as_scriptfunction.cpp as_scriptnode.cpp as_scriptobject.cpp as_string.cpp \
  as_string_util.cpp as_thread.cpp as_tokenizer.cpp as_typeinfo.cpp as_variablescope.cpp scriptstdstring/scriptstdstring.cpp scriptbuilder/scriptbuilder.cpp

O_FILES := $(patsubst %.cpp,%.o, $(filter %.cpp, $(S_FILES)))
all: $(O_FILES)
	@rm -f $(OUTPUT)
	@$(AR) cq $(OUTPUT) $(O_FILES)
	@$(RANLIB) $(OUTPUT)
.cpp.o: all
		@echo `date '+%H:%M:%S:'` Compiling $<.
	@$(CC) -c $(CFLAGS) -o $(patsubst %.cpp,%.o, $<) $<
clean:
	@rm -f *.o
	@rm -f $(OUTPUT)
