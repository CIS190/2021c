# Lecture 7: Nov 1 2021

## Templates

- Templates are used for _parametric polymorphism_, for writing code that does not depend on the type of a value.

### Function templates

[`function_template.cpp`](function_template.cpp)

- We can write functions that are parametric on an input type.
- The compiler generates a separate function for each use of the template with a different type.
- `typename` and `class` are interchangeable in the template prefix. I prefer `typename`, since not all types are classes in C++. You may find `class` in older code.
- You can omit the `template` prefix and just use `auto` for the type names.

### Class templates

[`class_template.cpp`](class_template.cpp)

- We can also write class templates.
- The container classes and smart pointers that we've seen are all examples of these.
- The shorter `auto` version isn't allowed, since we typically need to refer to the name of the type.

[`templates/wrapper.hpp`](templates/wrapper.hpp)

[`templates/wrapper.tpp`](templates/wrapper.tpp)

- The compiler generates different code each time the template is _instantiated_, so the implementation and interface must be bundled together in the header for this to occur, rather than compiled separately.
  - Sometimes people still break up the interface and implementation for organization or design reasons, but `#include` the implementation file (usually using a different file extension, like `.tpp`) in the interface file so it still compiles.

[`containers.cpp`](containers.cpp)

- The things we've talked about in the past 2 weeks are _not_ object-oriented.
  - If it were, there would be overhead in the form of virtual function calls when trying to perform operations abstractly.
  - To abstract over them we can use templates instead, since they share a consistent common interface.
  - Note that we can't use templates in place of virtual calls, since virtual calls can perform dynamic dispatch (look up and call different functions at runtime), while templates cannot, and resolve all calls staticly (at compile-time).

## Type constraints

- For most cases we want some restrictions on what type parameters are acceptable in templates.
- E.g. for `print` above we need to be able to loop over a `Container`, but this is not specified in the code, and we have to rely on comments.
  - We also only get an error message when we try to use it, and the error message is cryptic (try passing a non-container in to `print`).
- Type constraints or requirements are a way of writing down these requirements on types, and enforcing them at compile-time, with better error messages.
  - Similar to traits in Rust or typeclasses in Haskell.
- Generally `template <typename T> requires [constraint]`, where constraint is typically something involving `T`.
- for `print`, we could give the function the requirement `requires ranges::range<Container>` to specify that the `Container` type should be a range (which means it has `begin` and `end` functions which return iterators).
  - We can also replace `typename` with the requirement: `template <ranges::range Container>`.
  - _Or_, if we use the `auto` style, we can write it like this: `void print(ranges::range auto c) ...`

[`template_error.cpp`](template_error.cpp)

- Without type constraints, if we never call `set`, then this code will compile!
- Type constraints allow us to specify what operations the type argument should support, and tell us immediately "early" in compilation, rather than late, resulting in better error messages.
- If we do want this error only if `set` is used (e.g. we don't want to have to require equality checking on `T` if we never use `==` on `vector<T>`, you can add the constraint to the member function only: `void set(T t) requires copyable<T>` (putting the `requires` after the function arguments is ok too in general), and weaken the class constraint to `copy_constructible<T>`.

### Concepts

- The two constraints we just saw used _concepts_, named constraints, that were defined in the standard library.
- We can combine multiple using `&&` and `||` after a `requires`.

[`concepts.cpp`](concepts.cpp)

- Concepts can also combine other concepts, or use `requires` expressions (a different use of `requires` than before), which describe what operations should be defined (among other things, but we won't go into the other things `requires` can do).
  - `requires` expressions can be used as a constraint on its own, but will be written as `template<typename T> requires requires (T x) {x + x;}`.
- The concept can replace `typename` if it takes one type argument.
- Built-in concepts to the standard library can be found [here](https://en.cppreference.com/w/cpp/concepts).
  - These concepts often have some _semantic_ meaning on top of the _syntactic_ ones in the definition. For example this could be things like `addable` ensuring that `a + b == b + a`. Such constraints are only in the documentation, and not checked by the compiler.

### Type aliases for templates

[`using.cpp`](using.cpp)

- All containers provide a few common member _types_, like `value_type`, so they can be used generically.
- `using` in this context is the same as `typedef`.
- We need `typename` to tell the compiler the following expression is a type.
  - This was not necessary for things like `vector<int>::iterator` since you can check that expression is a type.
  - Since `Container` here is a template parameter, the compiler cannot know what `Container::value_type` is until after template instantiation.

#### Unrelated note: templates can have default arguments.

## Value template arguments

[`value.cpp`](value.cpp)

- We can also write class or function templates that are parameterized by values.
  - Why when we can pass values normally? Some things (e.g. arrays) require _compile-time_ constants (see below), and also, having the argument be a constant can be useful, for example for precomputing things at compile-time based on constants.
- e.g. `array` that takes the size of the array.

### `constexpr`

- What can we use as a value template argument?
  - Alternatively, what can we use as an array size?

[`constexpr1.cpp`](constexpr1.cpp)

- `constexpr` is a _stronger_ notion of immutability than `const`.
  - `const` is basically a _promise_ that you won't change this variable.
- A variable declared `constexpr` means it will be evaluated at compile time.
  - `constexpr` variables are also `const.
- Useful for declaring constants and placing values in read-only memory in the executable.
  - Similar to `#define`'s use cases in C.
- So far this seems kind of useless. Why do we need a whole new keyword for literals that the compiler knows at compile time anyways? Simple cases work just fine with `const`.
- What if we want to perform more complex operations at compile time?

[`constexpr2.cpp`](constexpr2.cpp)

- What if we want to perform more complex operations at compile time?
- `constexpr` can also be applied to functions.
- Functions can only be made `constexpr` if they are simple (pure in functional programming terminology): they can only use arguments passed to it and cannot have side effects.
- When a `constexpr` function is called with `constexpr` arguments, the output is also `constexpr`.
- A `constexpr` function can also be used as a regular function (which doesn't return a `constexpr`).

- `g++ -s -S constexpr2.cpp` to see the assembly output of the compiler.
  - `-s` removes debugging information, and `-S` generates assembly instead of an executable.
  - You can see that the answer is precomputed and stored in the assembly, not computed on demand.

## Template specialization

[`specialization.cpp`](specialization.cpp)

- Templates can have custom code for specific instantiations.
- `template <>` is required, to tell the compiler the class/function is a template.
- The specializations must come after the general template declaration.
- e.g. `vector<bool>`

[`specialization_concepts.cpp`](specialization_concepts.cpp)

- Using type constraints we can also write specializations generically.
- The compiler will pick the most specific version of the function to call based on the definitions of the constraints, or give an ambiguity error if there is no most specific version.
- By unfolding the definitions of the iterator concepts, the compiler can tell that vector iterators are both `forward_iterator`s and `random_access_iterator`s, but that `random_access_iterator`s are more specific. If you comment out the random access version, then the forward iterator version will be called for the vector line.

### Template metaprogramming

- Metaprogramming is where programs can themselves deal with programs as data.
- Templates are already an instance of metaprogramming -- they generate code for you.

[`fib.cpp`](fib.cpp)

- A more general style that is actually Turing-complete (Veldhuizen 2003) was discovered, without the intent of the C++ designers.
- A "functional language" using templates, with specializations as "base cases".
- Moves computation from runtime to compile time.
- Uses `struct`s to hold values that we are computing.

[`collatz.cpp`](collatz.cpp)

- Similar in use to `constexpr`, though templates are more expressive.
  - E.g. templates can deal with any type, not just simple types.
