### Project Structure
* For C++ `include` and `src`. But to exclude possibility of name collisions we should also 
  use a top level directory for the source code. So have `include/{top_level_dir}` and `src/{top_level_dir}`.
  `top_level_dir` also becomes a namespace `{top_level_namespace}` for the whole project. 
  We should also use a separate directory for each concept. 
  Related functionality and different versions of the concept will go there. 
  But it's not necessary to have a namespace per directory, especially if we don't expect any name collisions.
  Having files directly in `top_level_dir` could be confusing, since hard to know what concept they relate to.
  Have a `common` directory for primitive functionality that is used across the whole project.
* For C++ `app` directory to store sources of executables.
* For python, we have top level `scripts` folder to store all executable scripts.
  We should store reusable modules in `py{top_level_dir}`.

#### Current:
* app
  * measure
  * solver
* include
  * {top_level_namespace}
* src
  * {top_level_namespace}
* benchmark
* test
* py{top_level_namespace}
* scripts

### Principals
* Fast development is the most important thing.
* Abbreviations are allowed, but for more subtle ones, they should be mentioned
  in a dedicated file

#### Concept implementations naming.

Optimization programming goes through a lot of `trial and error`.
The code will be changing a lot. We expect many implementations 
for the same concept with slight tweaks. 

We must keep all the implementations to be able to prove why one 
thing works over the other any time.

In file names we don't want to separate between abstract and concrete classes and 
templates. Since abstract class could be promoted to concrete and concrete - to template 
to satisfy functionality needs.

Implementation versioning is an important part of the process. 
The first concept implementation gets named after the concept. 
Then we start adding version numbers `_{number}` in order of natural numbers starting with `2`.
We don't try to represent inheritance hierarchy in the name. 

If a concept has one existing implementation, and we want to add a new totally different one, 
it is possible to add a letter (using multiple letters at once is discouraged) to the implementation class name.
For example:
```c++
class Board {};
class Board_1 : public Board {}
class Board_2 {};

class Board_n {};
class Board_n1 {};
class Board_n2 {};
```

#### Primitive types naming.

We could also want to optimize on primitive types.
For that matter we can define our own with `using {what_for}_t` and
branch at compile time.

#### Code reuse / dependencies.

Certain concepts are well known and already implemented somewhere 
outside of this project. 
We should try to make a copy for as many as we can, shrinking down 
functionality to required point. Take a dependency only on popular
sources and defend yourself with your own interface for an easy switching.

Dependencies tend to break. And they are impossible to change for 
possible specific needs.

