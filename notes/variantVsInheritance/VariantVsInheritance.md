# Variant vs Inheritance

I was trying to implement a "visitor" pattern with the motivation to have dumb generated POD classes and the logic is held in external classes.

This could be done by:
- the visitor pattern using inheritance
- using std::variant. 

## Visitor with fixed return type

Writing the visitor pattern with inheritance is fairly easy in CPP as long as you don't want to have a generic return type. The code for a fixed return type visitor pattern can be seen in `fixedRetTypeVisitor.cpp`. The subclasses override the base class `accept()` method and runtime polymorphism will ensure the subclass method will be called. Simple!

## Visitor with generic return type

I wanted to have the return type be generic so one visitor could return a string and another could return an int. However, you're not allowed to have a templated virtual method in CPP, so the example in `fixedRetTypeVisitor.cpp` can't just be edited to be a template.

You also can't just define a non-virtual template in the base class and allow the subclasses to call that, since the `this` pointer will be to an object of the base class type rather than the subclass type.

I ended up getting around this by using the Curiously Recurring Template Pattern (CRTP), which templates the Base class with the Derived class. That allows the base class to know what type the derived class is and it can then call the correct method on the visitor. The code for this is in `crtpVisitor.cpp`. Unfortunately, that results in highly templated (i.e. unreadable) code

## std::variant to the rescue

The best solution for me was to use `std::variant`, which under the hood does all this for you and can be used with `std::visit` to call the correct method in the visitor. See `variant.cpp`.