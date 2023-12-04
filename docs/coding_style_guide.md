* Fast development is the most important thing.
* Abbreviations are allowed, but for more subtle ones, they should be mentioned
  in a dedicated file

Optimization programming goes through a lot of `trial and error`.
The code will be changing a lot. We expect many implementations 
for the same concept with slight tweaks. 

We must keep all the implementations to be able to prove why one 
thing works over the other.

So that concept interface (abstract class) will have suffixes `_v{number}`. 
And then implementations have suffix `_i{number}`.
If implementation is a template `_t{number}`
In case implementation inherits interface `_i{number}_v{number}`.

For example:
```c++
class Board_i1_v1 : public Board_v1 {}
```

We could also want to optimize on primitive types.
For that matter we can define our own with `using {what_for}_t` and
branch in compile time.

Certain concepts are well known and already implemented somewhere 
outside of this project. 
We should try to make a copy for as many as we can, shrinking down 
functionality to required point. Take a dependency only on popular
sources and defend yourself with your own interface for an easy switching.

Dependencies tend to break. And they are impossible to change for 
possible specific needs.

