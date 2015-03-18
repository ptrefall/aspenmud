# What Is Aspen? #
Aspen is a light weight, barebones mud engine written to be as customizable as possible.
Where most mud engines such as Smawg and Circle come with lots of extra code and Bloat, the goal of Aspen is to provide you with the tools to creat what you want. While this requires more work to get going, it also means that you will have many bonuses, as a result. First, there will not be so much to get used to as in a huge system like Smawg. Last, you will be able to design everything you want like you want it, without having to work around limitations or preexisting systems.

## Why Another Mudbase? ##
Aspen was designed to solve a few problems that I've noticed in other bases:
  * Most of the more popular mudbases that currently exist (Rom, Smaug, Circle), still have out-of-date code which should have been either optimized or cleaned long ago.
  * As far as I know, there doesn't yet exist a mudbase in c++ that is barebones, but provides a point for someone starting out to get started.

### How Is Aspen Structured? ###
Aspen's structure is set up so that objects don't have to know about other objects as much as possible in order to perform their functions; this allows for a modular component-based setup. While the world
handles most of the game play information such as the pointers to other objects, connected players and etc, other objects only have to interact with the
world object to get information about the server as-needed. With the event system, editing the world object to do something other than add methods and
properties crucial to the handling of game-data isn't needed. Using the event system, objects can subscribe to events that will be fired when a certain
event occurs, such as a player logging on or off, creating, deleting, etc.

# Getting Started #
To get started with Aspen, please visit our [installation tutorial](http://code.google.com/p/aspenmud/wiki/installation).
If you wish to contribute to Aspen, keep up with updates or join in discussions related to Aspen, you can subscribe to our [mailing list](http://groups.google.com/group/aspen-mud), or visit us on IRC at irc.freenode.net, #aspen-mud.


# credits #
Aspen would not have been possible without the continued support of a lot of friends to keep this going. Thanks especially to Chris Coale for answering a lot of questions and helping me iron out design issues. Thanks also to those who have given support, ideas, and suggestions. Lastly, this wouldn't have been possible without the vast amounts of Mountain Dew and coffee I consumed through development.

# Donations #
Donating to Aspen will both help to support the development, as well as provide motivation for more time toward this project. As of now, I am the only developer on Aspen. This means that any new features and additions have to be written when I have the time. Donations will show your support for Aspen. If you wish to donate, you can do so by visiting:
[my homepage](http://tds-solutions.net)

&lt;wiki:gadget url="http://googlegadgetworld.com/GreatQuotes/LifeQuotes/lifesayings.xml" height="210" border="0" /&gt;