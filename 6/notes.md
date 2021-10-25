# Lecture 6: Oct 25 2021

## Throwing constructors

[`ctor1.cpp`](ctor1.cpp)

- If something goes wrong in a constructor, you should throw an exception, since there is no return value.
- Note that if a constructor exits by a throw, the destructor is _not_ called.
  - Destructors are only called for objects that were successfully constructed.
- To catch exceptions thrown by the initializer list, use a _function try block_ which catches exceptions from the function body and the initializer list.
  - The catch clause(s) of a function try block _must_ throw an exception. If you don't throw one, it will rethrow automatically.
  - You can also use function try blocks for regular functions, but they are really only useful for constructors. You can read more about it [here](https://en.cppreference.com/w/cpp/language/function-try-block)
- The code above doesn't really work that well. We know it's `q` that's throwing, and that allocating `p` succeeds, but what if we don't? We have to delete the one(s) that were allocated successfully. Which do we delete?

[`ctor2.cpp`](ctor2.cpp)

- We can use smart pointers to handle this. Only objects that were successfully constructed will be destructed.

## Containers

- A container is an object that holds a collection of other objects.
- First we will talk about _sequence_ containers, where the user specifies the order.

## `vector`

[`vector.cpp`](vector.cpp)

- `vector` provides an array-like contiguous sequence of elements.
- `push_back` adds an object to the end of the `vector`.
- All containers provide `.size()` and `.empty()`, among other members.
- Note that you should use a `vector` of `unique_ptr` or something similar if you want polymorphic behavior.

[`emplace.cpp`](emplace.cpp)

- `vector` is typically implemented by doubling its size once it is full.
- Lots of copies/moves are made!
  - `vector` prefers copies/moves in this order: `noexcept` move ctor, copy ctor, normal move ctor (since a move ctor that throws is more dangerous than a copy ctor that throws, see [this SO question](https://stackoverflow.com/questions/28627348/noexcept-and-copy-move-constructors) for details)
- `push_back` copies/moves an object into the `vector`.
- `emplace_back` takes the arguments of the object constructor and creates the object _in_ the `vector`, saving a copy/move.

[`range.cpp`](range.cpp)

- Accessing elements using `[]` just accesses the underlying memory, without range checking.
- Using `.at(...)` to access an element does range checking, and throws `out_of_range` if the argument is out of range.
  - There is some overhead to doing this check (~10%).

[`operations.cpp`](operations.cpp)

- All containers provide a few standard operations.
  - `operator==` just checks if all the members are equal and in the same order.
  - `operator<` checks the lexicographic order.
  - `swap`, copy/move constructors/assignments are all defined.

## `array`

[`array.cpp`](array.cpp)

- A container for a fixed-length array, an alternative to using C arrays.
  - Benefits are that it stores its size (using `.size()`), doesn't convert to a pointer implicitly, and has some convenience functions.
  - _No_ runtime overhead compared to C arrays.
- Defined in `<array>`.

## `list`

[`list.cpp`](list.cpp)

- `list` is a doubly-linked list.
- `forward_list` is a singly-linked list, if you really need the lower overhead.
  - It also does not provide `size()` to reduce overhead.

## Other containers:

- A double ended queue (deque, pronounced "deck") has 2 usual implementations: as a doubly linked list and as an array that can be extended in either direction. Since C++ provides a linked list container already, `deque` is an implementation of the latter.
  - Like `vector`, but also with fast insertion at front.
  - Not strictly better however, since this is implemented with multiple underlying arrays (we can't extend arrays in both directions indefinitely). Multiple pointer dereferences are needed to access elements, as opposed to one dereference for a `vector`. It also typically needs more memory than a `vector`.

- C++ also provides container _adapters_, which are containers that use another container as its underlying container.
  - `stack` and `queue` are fairly straightforward. Both use `deque` by default, but `list` also works and `vector` is also suitable for `stack`.
  - `priority_queue` provides constant time for accessing the largest element, and logarithmic time for insertion and deleting the largest element.
    - Uses `vector` by default, though `deque` is also available.
  	- Uses standard library functions to manage a heap in the underlying container.
	  - The comparison function can be customized to reverse the ordering, for example.

| Container | Insertion | Access | Deletion |
| --- | --- | --- | --- |
| `vector` | O(1) at back<br>O(n) otherwise | O(1) | O(1) at back<br>O(n) otherwise |
| `deque` | O(1) at front and back<br>O(n) otherwise | O(1) | O(1) at front and back<br>O(n) otherwise |
| `list`/`forward_list` | O(1) after reaching the element | O(1) at front and back (only for `list`)<br>O(n) otherwise | O(1) after reaching the element |
| `priority_queue` | O(log n) | O(1) for the max | O(log n) for the max |

## Iterators

- But wait, how do we actually use a `list` besides accessing the front and back?

[`iterator1.cpp`](iterator1.cpp)

- Iterators are objects that denote an element from a sequence of elements.
- Containers all have a type member `::iterator` (and `::const_iterator`) for the type of its iterator.
- All containers have the `begin()` and `end()` (and `cbegin()` and `cend()` for const iterators) member functions to expose iterators to the beginning of the sequence and _one past the end_ of its sequence, respectively.
  - This is what the range based for loop (`for (int i : l)`) uses.
- To use iterators, dereference them using `*` or `->`. This doesn't mean iterators are pointers (though sometimes they are!).

[`iterator2.cpp`](iterator2.cpp)

- There is a hierarchy of iterator types, though iterators don't use a class hierarchy.
  - They use template arguments instead (next lecture) for performance.
- Here is a simplified description of the hierarchy:
  - All iterators can increment (`++`) through its sequence, be compared for equality (`==`, `!=`), and access its element of the sequence (`*`, `->`). If they are not `const` iterators, they can also be assigned to (`*it = ...`). Iterators that can _only_ do this are _forward iterators_.
    - e.g. `forward_list`
  - Bidirectional iterators can also decrement (`--`)
    - e.g. `list`
  - Random access iterators can go forwards and backwards any number of steps in constant time (`+`, `-`, `+=`, `-=`), and can be compared for inequality (`<`, `<=`, `>`, `>=`).
    - e.g. `vector`, `deque`
- Note that pointers are valid random access iterators (which also means they are valid instances of all the possible types of iterators).
- Note that also some of these operations can be simulated.
  - e.g. `advance(it, n)` in `<iterator>` just performs `++` on `it` `n` times.
    - Note that random access iterators can do this in constant time, while forward or bidirectional iterators will need linear time.
  - `distance(first, last)` returns the number of `++` required to get from `first` to `last`. This is undefined behavior if `last` is not reachable from `first`.
  	- Similarly to `advance` random access iterators can do this in constant time, while other ones will need linear time (by repeatedly doing `++` and testing using `==`).

[`iterator3.cpp`](iterator3.cpp)

- All sequence containers provide `insert`, which inserts before an iterator.
- Iterators can be _invalidated_ when the container changes. This depends on the container.
- See [chart](https://en.cppreference.com/w/cpp/container) for a summary of the invalidation rules

[`iterator4.cpp`](iterator4.cpp)

- All sequence containers provide `erase` as well, which deletes the element at an iterator.
- `array` iterators never get invalidated, since the size cannot change.

### Stream Iterators

[`ostream.cpp`](ostream.cpp)

- Iterators are elements of _sequences_, not containers.
- Iterators can be made from streams as well.
- `ostream_iterator` can write to an `ostream` with the same interface as a container.
  - `ostream_iterator` cannot be read from. What I said earlier about all iterators having `*` and `->` was a simplication. There are _input_ and _output_ iterators, and `ostream_iterator`s are output iterators only.

[`istream.cpp`](istream.cpp)

- Each time an `istream_iterator` is incremented (and when it is initialized with the stream), it will read from its stream.
- When it is dereferenced, it will return the value last read.
- The default-constructed `istream_iterator` represents the end-of-stream iterator.
