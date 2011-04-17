#aspen Makefile
#compiler and project specific variables

OUTPUT=../bin/aspen
#program defines
MAKE=make -s
RM = rm -f
ECHO = echo
#compiler flags:
#enable if you want to profile.
#PROF = -pg
CC	= g++
#debugging code (should always be used except for special cases).
CFLAGS	= -ggdb -pedantic -Wall -Werror -O1
#makes the compile use our libraries.
CFLAGS += -L../libs
#makes the compiler use our includes.
#tinyxml is a third-party library, so we just compile it as a module.
CFLAGS += -I../includes

#required libraries
L_FLAGS	= -lcrypto -llua5.1 -ltinyxml

#formatting specific flags
FORMATTER = astyle
FORMAT_FLAGS = --style=gnu

#modules
MODULES = scripts syslog help components intermud

#dependencies
DEPS = tinyxml

#source files to compile:
S_FILES= world.cpp variant.cpp entity.cpp player.cpp socket.cpp\
utils.cpp log.cpp server.cpp main.cpp exception.cpp\
command.cpp com_gen.cpp com_wiz.cpp event.cpp menu.cpp\
living.cpp channel.cpp modules.cpp component.cpp npc.cpp\
editor.cpp exit.cpp room.cpp zone.cpp com_builder.cpp\
com_movement.cpp ComponentFactory.cpp components.cpp olc.cpp socials.cpp\
option.cpp baseSocket.cpp

###
###YOU SHOULD NOT MODIFY ANYTHING PAST THIS POINT.
###IF YOU HAVE CHANGES, MAKE THEM ABOVE TO THE FLAGS.
###

include $(patsubst %,%/include.mod, $(MODULES))
O_FILES := $(patsubst %.cpp,%.o, $(filter %.cpp, $(S_FILES)))

all:	dependencies $(O_FILES)
	@$(RM) $(OUTPUT)
	@$(ECHO) `date '+%H:%M:%S:'` Linking
	@$(CC) $(PROF) $(CFLAGS) -o $(OUTPUT) $(O_FILES) $(L_FLAGS)
.cpp.o: all
	@$(ECHO) `date '+%H:%M:%S:'` Compiling $<.
	@$(CC) $(PROF) -c $(CFLAGS) -o $(patsubst %.cpp,%.o, $<) $<
dependencies:
	@for d in $(DEPS);do $(ECHO) Compiling $$d;cd "../dependencies/$$d";$(MAKE);cd $(CURDIR);done
clean:
	@$(ECHO) `date '+%H:%M:%S:'` Cleaning
	@$(RM) *.o $(patsubst %,%/*.o, $(MODULES))
	@$(RM) $(OUTPUT)
clean-all:
	@$(MAKE) clean
	@$(ECHO) `date '+%H:%M:%S:'`  cleaning dependencies
	@for d in $(DEPS);do cd ../dependencies/$$d;$(MAKE) clean;cd $(CURDIR);done
	@$(RM) ../libs/* ../includes/*

format:
	@$(ECHO) `date '+%H:%M:%S:'` Formatting
	@$(FORMATTER) $(FORMAT_FLAGS) $(S_FILES)
	@$(FORMATTER) $(FORMAT_FLAGS) *.h
	@$(FORMATTER) $(FORMAT_FLAGS) *.hpp
	@$(RM) -f *.orig
	@for d in $(MODULES);do cd $$d;$(FORMATTER) $(FORMAT_FLAGS) *.h;$(FORMATTER) $(FORMAT_FLAGS) *.cpp;$(FORMATTER) $(FORMAT_FLAGS) *.hpp;$(RM) *.orig;cd $(CURDIR);done
	@$(ECHO) `date '+%H:%M:%S:'` Done
