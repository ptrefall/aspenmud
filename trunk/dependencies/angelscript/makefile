# Angelscript makefile for linux (based on MingW makefile)
#Modified for AspenMud
	LIB = libangelscript.a
INC = angelscript.h

SRCDIR = ./
INCDIR = ../../includes
	OBJDIR = obj
	LIBDIR = ../../libs
	CXX = g++
	# On i686 architecture you may need to add -march=i686 if you get 
	# an undefined symbol for __sync_sub_and_fetch_4 in as_atomic.cpp.
	CXXFLAGS += -g -Wall -march=native

AR = ar
RANLIB = ranlib
DELETER = rm -f
COPIER = cp -a

SRCNAMES = \
  as_atomic.cpp \
  as_builder.cpp  \
  as_bytecode.cpp \
  as_callfunc.cpp \
  as_callfunc_arm.cpp \
  as_callfunc_mips.cpp \
  as_callfunc_ppc.cpp \
  as_callfunc_ppc_64.cpp \
  as_callfunc_sh4.cpp \
  as_callfunc_x86.cpp \
  as_callfunc_x64_gcc.cpp \
  as_compiler.cpp \
  as_context.cpp \
  as_configgroup.cpp \
  as_datatype.cpp \
  as_generic.cpp \
  as_gc.cpp \
  as_globalproperty.cpp \
  as_memory.cpp \
  as_module.cpp \
  as_objecttype.cpp \
  as_outputbuffer.cpp \
  as_parser.cpp \
  as_restore.cpp \
  as_scriptcode.cpp \
  as_scriptengine.cpp \
  as_scriptfunction.cpp \
  as_scriptnode.cpp \
  as_scriptobject.cpp \
  as_string.cpp \
  as_string_util.cpp \
  as_thread.cpp \
  as_tokenizer.cpp \
  as_typeinfo.cpp \
  as_variablescope.cpp \

OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRCNAMES:.cpp=.o)))
BIN = $(LIBDIR)/$(LIB)

all: $(BIN)

$(INCDIR)/angelscript.h:
	$(COPIER) angelscript.h $(INCDIR)

$(BIN): $(OBJDIR) $(LIBDIR) $(OBJ)
	@$(AR) cq $(BIN) $(OBJ)
	$(RANLIB) $(BIN)
$(OBJDIR):
	mkdir $(OBJDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(DELETER) $(OBJ) $(BIN) $(LIBDIR)/$(LIB)
.PHONY: all clean install uninstall
