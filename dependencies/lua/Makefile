# makefile for installing Lua
#heavily modified to compile for Aspen's dependencies.
# see INSTALL for installation instructions
# see src/Makefile and src/luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= ansi

# Convenience platforms targets.
PLATS= aix ansi bsd freebsd generic linux macosx mingw posix solaris
# Lua version and release.
V= 5.1
R= 5.1.4

all:	$(PLAT)
	cp src/lauxlib.h src/lualib.h src/luaconf.h src/lua.h ../../includes
	cp src/liblua.a ../../libs

$(PLATS) clean:
	cd src && $(MAKE) $@
