# Resolver vs Environment extension

The Crafting Interpreters book walks us into a bug where a function defined within a scope accesses a global variable. After the function has been declared a shadowed local variable is declared with the same name as the global variable. After this, the function uses the local variable instead of the global one it was using before.

## Bug in question

```
    var a = "global";
    {
        fun showA() {
            print a;
        }

        showA();
        var a = "block";
        showA();
    }
```

```
"global"
"block"
```

## Book proposal

The book proposes setting up a layer of static analysis between parsing and interpreting, which would count the scope distance to the defined variable and store this in a global map. For the above example, the scope distance of `a` would be 1, since it's in the scope above where `showA()` is defined. The logic proposed by the book extends `Environment` so whenever `get()` or `assign()` is called, the caller tells the environment how many scopes to traverse. The called could then ensure that whenever `showA()` is invoked it will always use the variable of `a` with a scope distance of 1, and `Environment` will ignore a shadowed version of `a` in the current scope.

## Alternate fix (chosen)

Instead of this static analysis layer, we instead change Environment so there can be multiple environments for a single scope. When a function is declared it's given a ptr to the current Environment which will contain only variables that have been declared before it. A new Environment is then created as an extension to the previous one, and any new declarations in the same scope will be written to the extension.

```
            parent              parent             parent
globalEnv <--------- scopeEnv <-------- scopeExt <-------- newScope
|                    |                                     |
scopeStart           scopeStart                            scopeStart
```

## Reason for decision

In terms of performance, it's not clear without testing which is faster, with it likely being situational. The book's solution would need to run a static analysis layer for every program, whereas the alternate solution would need to traverse more Environment pointers to resolve variables since there could now be multiple Environments per scope.

I decided to go for the alternate solution as this would keep responsibility for resolving variables within the Environment class rather than being dependent on static analysis to calculate scope distance and having caller to tell Environment how many scopes to traverse. Having the caller track which component is currently being executed (since different objects will have different distances to `a`) also does not tie nicely into the current design where `std::visit<>()` only works with a single argument.
