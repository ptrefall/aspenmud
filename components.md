# What are components? #
Components are generic objects that you can use to extend the functionality of in-game objects. Rather than a is-a hierarchy, components allow you to use a has-a system. this is useful in many cases.

First, imagine that you have the Entity object in Aspen. This object gives you basic in-game data, as well as functions for managing it; extending it is up to modules and components. Now, you want to make a backpack You could do something like:
`class Backpack:public Container`
where container inherits Entity. Now in order to make this work like a real backpack, it has to be worn, so lets do that:
`class Backpack:public Container, public Wearable`
Great, so now we have a backpack that is both a container and can be worn. Now you want a different sort of backpack; one that is armored as well in order to provide armor to the wearer's back.
`class NewBackpack:public Container, public Worn, public Armor`
As you can see, this gets messy really quickly; you need multiple classes for multiple different objects, and multiple inheritance as most coders with experience will tell you leads to problems down the road.
## Solution ##
Enter components. Components allow you to keep the inheritance structure of an object exactly the same, by letting you add individual components to your object. In order to get an armored wearable container with components, you would do this:
```
World* world = World::GetPtr();
obj->AddComponent(world->CreateComponent("container"));
obj->AddComponent(world->CreateComponent("armor"));
obj->AddComponent(world->CreateComponent("wearable"));
```
While this does lead to more code, you end with much cleaner code. This also makes it possible for scripts to add components to objects, so that you can have a worn armored backpack without creating your own Backpack object for the occasion.

## component dependencies ##
Components rely on others to provide basic features. For example, you may have a weapon component as well as a wieldable component to allow other objects to be wielded. As a result, you want to set the Wieldable component as a dependency to Weapon, so that when Weapon is added, the Wieldable component will also be added in the same go.