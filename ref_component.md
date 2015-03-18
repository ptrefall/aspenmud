

  * file: component.h
  * Synopsis: The component class provides a way to add functionality and extend in-game objects.

---


# Public fields #

## variables ##
a PropertyTree holding variables associated with the component.

## events ##
an EventManager object that allows for events to be bound to the object.


---


# Members #

## `Component(const std::string &name)` ##
### Parameters ###
  * in: the name of the component.

### Description ###

Initializes the component to all default values, and sets the name to the one provided.

## `Component();` ##
### Description ###

Initializes the component to all default values.

## `  void SetName(const std::string &name)` ##

### Parameters ###
  * In: The name of the component.
### Description ###
Sets the name of the component to the name specified.