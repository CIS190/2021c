# Lecture 8: Nov 8 2021

## Libraries, applications, and tools for C++

- Ideas for project
- General purpose language, so basically everything is doable and well-supported (but may be difficult)
- Very large list [here](https://en.cppreference.com/w/cpp/links/libs).

### Build systems and package managers
- C++ lacks a central tool to install libraries or compile projects, so you may want to choose one of the tools in the list above. If you aren't using too many weird libraries, it should be straightforward to compile with a simple Makefile.
- [CMake](https://cmake.org/) is a popular tool that _generates_ build files for different environments, like Makefiles, Visual Studio projects, or other targets, using one file. While this is very useful for cross-platform development, it's probably overkill for the final project.

### Frameworks
- The following are less libraries and more frameworks, that change the way you need to code.
  - Many of these are very old, and therefore predate some more recent features of C++, and instead build their own standard libraries (like their own string classes!).
  - I recommend against these due to the high learning curve unless there's something specific you want to do with one that you can't do otherwise.
- Game engines, like [Unreal engine](https://www.unrealengine.com/en-US/).
  - Most handle every aspect of creating a game, like game assets and animations.
- [Qt](https://www.qt.io/) for cross-platform GUIs. It includes most things you would need for writing applications, like networking, databases, and I/O.

### Libraries

- When compiling, you likely need to add the `-L[path to compiled library files]` to tell the linker where to find the library, the `-l[name of library]` flag to tell the linker to link the library files, and the `-I[path to library headers]` flag to tell the compiler where to find the library's header files.

- [Boost](https://www.boost.org/) contains many libraries, all of which are peer-reviewed and designed to work well with the standard library.
  - [Asio](https://www.boost.org/doc/libs/1_77_0/doc/html/boost_asio.html) is a go-to library for doing low-level networking.
  - Many standard library features started off as libraries in Boost.

- For 3D graphics, the lowest-level options are [Direct3D (for Windows)](https://docs.microsoft.com/en-us/windows/win32/direct3d) and [OpenGL (cross-platform)](https://www.opengl.org//). Higher-level libraries include

- The above options can also be used for 2D graphics, but for a higher-level option [SFML](https://www.sfml-dev.org/index.php) is a good choice. [SDL](https://www.libsdl.org/) is a similar option but is a C library.

- If you want a simpler user interface, there is also the option of [ncurses](https://invisible-island.net/ncurses/) library for writing text-based terminal UIs.

- [OpenCV](https://opencv.org/) is a popular computer vision library written in C++.

- For testing, we use [Catch2](https://github.com/catchorg/Catch2) for unit testing in the autograder, but there are other options such as the testing library in Boost.

### Other uses of C++

- Other areas where C++ is popular, aside from the ones mentioned above (but are not really suitable for a final project):
- Low-level applications, like embedded programming and operating systems. OSX and Windows are at last partly written in C++.
- Applications that require high performance, like browsers, databases, and distributed systems.

## Template specialization

[`specialization.cpp`](specialization.cpp)

- Templates can have custom code for specific instantiations.
- `template <>` is required, to tell the compiler the class/function is a template.
- The specializations must come after the general template declaration.
- e.g. `vector<bool>`

### Aside: metaprogramming

- Metaprogramming is where programs can themselves deal with programs as data.
- Templates are already an instance of metaprogramming -- they generate code for you.

[`fib.cpp`](fib.cpp)

- A more general style that is actually Turing-complete (Veldhuizen 2003) was discovered, without the intent of the C++ designers.
- A "functional language" using templates, with specializations as "base cases".
- Moves computation from runtime to compile time.
- Uses `struct`s to hold values that we are computing.
- Simple cases are subsumed by `constexpr` functions.

### Back to template specialization

[`specialization_concepts.cpp`](specialization_concepts.cpp)

- Using type constraints we can also write specializations generically.
- The compiler will pick the most specific version of the function to call based on the definitions of the constraints, or give an ambiguity error if there is no most specific version.
- By unfolding the definitions of the iterator concepts, the compiler can tell that vector iterators are both `forward_iterator`s and `random_access_iterator`s, but that `random_access_iterator`s are more specific. If you comment out the random access version, then the forward iterator version will be called for the vector line.

## Associative containers

- Containers that store a collection of key-value pairs (or something similar), which are not ordered by the user.

### First: `pair`

[`pair.cpp`](pair.cpp)

- Combines two objects of any type.
- In `<utility>`.
- Use `make_pair` to avoid typing out the types.
- Access the elements using `.first` and `.second` (note these are member _variables_, not member _functions_).
- Generalized by `tuple`.
- One possible implementation for an associative container is just a sequence container of `pair`s.

### `map`

[`map1.cpp`](map1.cpp)

- Typically implemented using a balanced BST like a red-black tree.
- `[]` returns a reference to the value corresponding to a key, if the key exists in the map. If the key doesn't exist in the map, it will be created.
- Other familiar member functions are also available:
  - `at` accesses a key's value. If the key doesn't exist, an exception is thrown.
  - `empty`, `size`, `clear`, `erase`.
  - `insert` and `emplace` are just like the functions on sequential containers, except they take a `pair` (no iterator required, since we don't need to manage the order ourselves).
- Iterating through the container will always go in sorted order.

### `unordered_map`

[`unordered_map.cpp`](unordered_map.cpp)

- `map` uses a BST, with logarithmic complexity for its operations.
- Hash tables can do better, and that's what `unordered_map` is.
  - Amortized constant time complexity of its operations.
- Otherwise, `unordered_map` has basically the same interface as `map`.
- Custom key types will be discussed later

### `set` and `unordered_set`

- These containers are basically the `map` and `unordered_map` containers, but without a value attached to the keys.

### `multi` variants

- There are also the `multimap`, `unordered_multimap`, `multiset`, and `unordered_multiset` classes for associative containers that can have multiple copies of a key.
- These containers can return a sequence of results using a start and end iterator.

## Abstracting over functions

- Higher order functions, as you've seen in CIS 120, are really useful.
- We can use function pointers from C, but these are fairly basic, cannot be parameterized (i.e. there are no function pointers to function templates), and are not dynamic (they must be known at compile-time).

### Function objects

[`functor.cpp`](functor.cpp)

- A function object (sometimes called a functor) is an object that can be called like a function.
- This is done by overloading `operator()`.
- The object can store other state inside, which can be useful.
- A functor is just a regular object, so its class can also be made a class template.
- Note that `struct`s and `class`es can be written within other structures, like functions and other `class`es.
- The `predicate` concept describes types that can be called with arguments of type `T` and returns something that can be converted to a `bool`.
  - also works with function pointers
- `invocable` or `regular_invocable` (the difference is purely semantic, `regular_invocable` requires the function to return the equal outputs for equal inputs) can be used to write down the argument types, and `invoke_result` along with `convertible_to` can be used to write down the return type.

### `hash`

- Other languages have hash codes built in to the language.
  - This requires a common base class.
  - But not everything in C++ is an object!
  - Also a virtual call may be required if we use a hash code method with dynamic dispatch. This forced overhead is really not in the spirit of C++.
- `unordered_map` also needs `operator==`, to deal with hash collisions.

[`hash.cpp`](hash.cpp)

- `unordered_map` uses `std::hash` by default, or you can provide your own functor.
- `std::hash` is a template class defining a functor.
- The standard library provides specializations of `hash` for the basic C++ types.
- We can write our own specialization as well, and add it to the global namespace.

### `map` ordering

[`map2.cpp`](map2.cpp)

- If your key is a custom class, you'll need to specify an ordering to use.
  - By default `map` uses `<`.
  - You can customize this by passing in a comparison functor type as a template argument.
    - The standard library has a few built in: `less<Key>`, which uses `operator<` for your class (if your class is named `Key`), is the default.
    - Others include `greater<Key>`, `less_equal<Key>`, and `greater_equal<Key>`.
- `map` doesn't use `operator==` to check for equality; we _only_ require an ordering.
  - If `!(a < b) && !(b < a)` then the `map` (if it was using `less`) considers `a` and `b` equivalent.

### Lambda expressions

[`lambda.cpp`](lambda.cpp)

- A lambda expression is shorthand for declaring a function object.
- Unspecified type (you have to use `auto`!).
  - If you need to write down the type of the lambda (e.g. if you want to use a lambda as the comparison object for `map` or a hash object), you can use `decltype`.

[`capture.cpp`](capture.cpp)

- `[]` is the _capture list_ of the lambda expression.
  - It allows the body of the lambda to access variables outside it.
  - `[limit]` captures `limit` by value.
  - `[&limit]` captures `limit` by reference.
  - `[=]` and `[&]` capture _everything_ by value and reference respectively.
  - A comma delimited list can customize which variables are captures by value or by reference.

[`generic_lambda.cpp`](generic_lambda.cpp)

- You can write generic lambdas using `auto` for its arguments.
- If necessary, you can use the syntax `-> T` after the `)` to specify that the lambda has return type T. When dealing with generic lambdas, often `decltype` is useful here.
