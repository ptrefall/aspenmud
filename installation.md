# Getting started #

In order to install Aspen, you will need to check it out through svn.
From the command line:
`svn checkout http://aspenmud.googlecode.com/svn/trunk/ aspenmud`
**note:** You can replace the aspenmud with any directory you choose. For example, if you would like the repo to be checked out under a directory called 'mud', you could replace aspenmud with 'mud', like so:
`svn checkout http://aspenmud.googlecode.com/svn/trunk/ mud`

## dependencies ##
Aspen used to ship with Lua as it's dependency. We have removed this, however, which means that you will need to have the following libraries installed in order to compile Aspen:
  * liblua5.1 or higher
  * boost 1.41 or higher
  * libssl

### potential problems ###
Currently there are a few potential problems. If you get compile errors due to lua.h not being found and you have it installed or link errors against liblua5.1, you will need to replace liblua5.1 in your Makefile, or edit scripts/scripts.h and include the proper path to lua.h as well as the other headers.

## Compiling ##
After you have successfully checked out the repository, you need to cd to the src directory:
`cd aspenmud/src`
Then type make to begin the compilation process.

First you will see it compiling the dependencies; these are the dependencies distributed with Aspen to ease compilation. After the dependencies successfully compile, you will see the rest of the engine compiling. If everything goes well, you will see a message that the mud is being linked, and your prompt again. The binary can be found in ../bin, or aspenmud/bin.

## Running Aspen ##
In order to run aspen, cd to the bin directory:
`cd ../bin`
from src, or:
`cd aspenmud/bin`
from the root directory. From here, you can run Aspen in two different ways.

### Running Aspen in the forground ###
When you are debugging or just trying out Aspen for the first time, you may want to run aspen in the console, which will allow you to watch any log messages that are printed. In order to do this, you need only:
`./aspen`

### Running aspen in the background ###
In order to run Aspen in the background, you can:
`nohup ./aspen &`
This will make the binary start, write it's output to nohup.out in the same directory, and then background it. Another bonus to this is if you close your terminal, Aspen will not close.