# Lecture 10: Nov 22 2021

## Concurrency

- Useful for:
  - Improving throughput
  - Improving responsiveness
- We saw limited support for this in the form of parallel algorithms from last class.
- C++ also supports other types of concurrency facilities in its standard library.

- We will call a _task_ a computation that can be executed concurrently with other tasks. A task will be a function or function object.
- A thread is the representation of a task by the OS. Threads can be run concurrently, either in parallel (on different CPU cores) or by context switching (running parts of a thread before switching to another thread)

- To compile the code in this lecture you will need to link against the `pthread` library using `-lpthread` at the end of your `g++` command.
  - This should only work on *nix environments. If you are using Visual C++ on Windows, it should work without having to do anything extra.

### `thread`s

[`thread.cpp`](thread.cpp)

- A `thread` (in `<thread>`) is what the standard library provides to represent a thread.
  - Pass the function (or functor) and its arguments separately to `thread`.
- As soon as a `thread` is initialized with a task, it starts running.
- `.join()` on a `thread` blocks the current `thread` until that `thread` finishes its task.
- `.detach()` on a `thread` will allow the `thread` to run independently, without having to be joined.
  - This is useful because the destructor of `thread` will terminate the program if it is called without `.join()` or `.detach()` being called (so signalling your intent is required).
  - Using a `jthread` (for joinable thread) instead of a `thread` will automatically call `join` in its destructor, so it fixes this issue.
- The return value of a task in a `thread` is ignored.
- We can use a reference to return a value from a `thread`.
  - However, the way we pass arguments to the `thread` constructor uses a variadic template that can accept any number of arguments. This doesn't work well with references for technical reasons so we have to wrap the variables with `ref` (from `<functional>`) to force the compiler to see that these arguments are references.
  - `bind` from last lecture has the same problem with references.

## "Higher-level" concurrency

- For simple tasks that don't require synchronization.

### `async`

[`async.cpp`](async.cpp)

- `async` (in `<future>`) allows you to perform a task _asynchronously_, that is, without waiting for it to finish, and get its return value later.
  - `async` separates the call part of a function call from its return value.
- The return value is a `future`. Use `.get()` on the `future` to get the result of the function call.
  - If the result is not available yet, `.get()` will block until it is.
  - `.get()` can only be used once. Multiple uses is undefined (use `.valid()` to check if it is available to use).
- `async` _may_ run a task in a separate thread, or just run it synchronously when its result is queried using `.get()`. This is decided at runtime.
  - You can explicitly choose by passing in `std::launch::async` or `std::launch::deferred` as the first argument.
- Only use `async` for "simple" tasks (e.g. don't edit any shared variables, don't require explicit synchronization).
  - Some constructs, like `mutex` (more on this later), have behavior that depends on the current `thread`. Since `async` may or may not choose to run your task in a new thread, this can cause some nasty bugs.

[`async_exception.cpp`](async_exception.cpp)

- `async` will also propagate any exceptions thrown in the task.

[`async_sync.cpp`](async_sync.cpp)

- The destructor of the returned `future` will block until the asynchronous call is complete.
  - This means if you don't bind the returned `future` to a variable, it will be immediately destructed and the `async` call will be essentially synchronous.
  - Note that this behavior of `~future` is only for `future`s obtained from `async` (see the next section for where else you can get `future`s from).

### `future`s and `promise`s

[`promise.cpp`](promise.cpp)

- Above, `async` returned a `future`, which represents the result of an asynchronous operation.
  - This asynchronous operation was a function call, and the result was the return value.
- More generally, a `future` can represent any result, not just the results of functions.
- A `promise` allows a task to pass a result to its corresponding `future`.
  - Use `set_value` to return a normal result.
  - Use `set_exception` to throw an exception when the associated `future` has `.get()` called.
- Use `.get_future()` to get the `future` associated with a `promise`. This function can only be called once.

[`shared_future.cpp`](shared_future.cpp)

- Sometimes many threads will want the result of a `future`, but `.get()` can only be called once. We could take the return value of `.get()` and share it, but then we have to wait until it's ready in a single thread, then distribute afterwards.
- `shared_future` allows multiple `thread`s to wait on its result.
- A `future` can be implictly converted to a `shared_future` (by one of `shared_future`'s constructors), or created by `future::share()`, which transfers the state of the `future` to the returned `shared_future` (so the original `future` is then invalid).

### `packaged_task`

[`packaged_task.cpp`](packaged_task.cpp)

- `packaged_task` basically wraps a task with the capability to return a `future`, allowing us to easily use the return value of, or any exceptions throw from a task.
- Since a `package_task` owns resources (the task and the internal state required for the `future`), it cannot be copied and must be `move`d.
- Using `packaged_task`s and `thread`s is similar to, but more flexible than `async`, since we can control how the task is run (whereas `async` decides for us if it should run in a separate thread, and starts the task immediately if it does).
  - Basically `async` wraps and uses `packaged_task` with a simpler interface.

## "Lower-level" concurrency

- For more complex tasks that require synchronization between threads.

### Issues with concurrency

- Data races
- Deadlock

### `mutex`

[`mutex.cpp`](mutex.cpp)

- A `mutex` (also commonly called a lock) (in `<mutex>`) is a synchronization mechanism.
  - A `thread` can `.lock()` a `mutex` to _own_ the `mutex`.
  - If another `thread` owns the mutex, the call to `.lock()` blocks until the `mutex` is no longer owned.
  - To release ownership, the owning `thread` must call `.unlock()`.
- The code between a `.lock()` and `.unlock()` is called a critical section. Multiple `threads` cannot access it concurrently.

### Locking `mutex`es

[`mutex_exception.cpp`](mutex_exception.cpp)

- Using `.lock()` and `.unlock()` is error-prone and breaks exception safety, just like using `new`/`delete`.

[`scoped_lock.cpp`](scoped_lock.cpp)

- A `mutex` is a resource as well, so RAII is a useful tool here.
- `scoped_lock` is a wrapper around a `mutex` that uses RAII to unlock its `mutex` when it is destroyed.

[`deadlock.cpp`](deadlock.cpp)

- Deadlocks occur when `thread`s acquire and hold a `mutex` while waiting for another `mutex`. If multiple threads do this and are waiting for each other to release a `mutex`, this results in a deadlock.

[`scoped_lock_deadlock.cpp`](scoped_lock_deadlock.cpp)

- `scoped_lock` can also wrap multiple `mutex`es, and uses a deadlock-avoiding algorithm to acquire the locks.
  - `scoped_lock` will never block trying to acquire a `mutex` while holding the lock for another of its `mutex`es.

### Sharing with reader and writer threads

[`shared_mutex.cpp`](shared_mutex.cpp)

- One common pattern is that there are some `thread`s that create data, and other `thread`s that just read the data. While there would a data race if one `thread` accesses the data while some other `thread` is writing to it, it is fine for multiple `thread`s to read the data at once.
- `shared_mutex` is a mutex for this situation. It has two levels of locking, a _shared_ level and an _exclusive_ level.
  - Multiple `thread`s can lock the `shared_mutex` at the shared level, and only if the exclusive lock is not acquired.
  - Only one `thread` can lock the `shared_mutex` at the exclusive level at a time, and only if the shared lock is not acquired by anyone.
- There are also RAII-style wrappers for shared and exclusive locking, called `shared_lock` and `unique_lock` respectively.
- In this example, multiple reader threads are in the critical section at the same time, which you can see by the interleaved prints.

### Waiting using condition variables

[`condition_variable.cpp`](condition_variable.cpp)

- `mutex` allows us to block until an exclusive resource (the `mutex`) is available. But what if we want to wait for something more complex?
  - We could just use a loop to continually check the condition: `while (condition = false) {} // afterwards condition must be true!`
  - This is both inefficient (wasting CPU time) and doesn't work (the condition may change back to false if we don't guard the condition with a `mutex`, and we never release the `mutex` if we do guard the condition).
- `condition_variable`s are a synchronization primitive used for threads to sleep until another thread wakes it up, based on some _condition_.
- `condition_variable` must be used with a `mutex`.
  - The waiting `thread` acquires the `mutex` using a `unique_lock`, then calls `.wait()` on the `condition_variable`. `.wait()` will release the lock and suspend the `thread` until it is woken up.
  - To wake up threads waiting on `condition_variable`s, a `thread` that changes the condition can call `.notify_one()` or `.notify_all()` to wake up one or all waiting `thread`s on that `condition_variable`.
  - When a thread wakes up, it must acquire the lock again to leave the `.wait()`.
- `condition_variable` uses `unique_lock` instead of `scoped_lock` because we need finer control of the lock. `scoped_lock` just locks on construction and unlocks on destruction. `unique_lock` provides methods to lock and unlock at any time.
- `.wait()` can take a second argument, a predicate that represents the condition to check.
  - This is to combat _spurious wakeup_, where suspended threads can just wake up randomly, for no reason.
  - Typically the `wait` is wrapped in a while loop: `while (!condition) cv.wait(...);`.
  - This overload of `.wait()` does this for you, the predicate is checked and if it is not true, we wait again.
    - Note that since `.wait()` needs to acquire the lock, the predicate is guarded by the lock as well.

### Semaphores

[`semaphore.cpp`](semaphore.cpp)

- An alternative to using `mutex` and `condition_variable` is using a `counting_semaphore`.
- Keeps track of a number, provides operations to increment (`release`)/decrement (`acquire`) it, and blocks if we try to decrement past 0.
  - `binary_semaphore` is a `counting_semaphore` with its max set to 1, so it is basically the same as `mutex`.
- Useful for allowing a number of threads into a critical section, not just one.
- Semaphores are not yet implemented in GCC 10.

### Barriers

[`barrier.cpp`](barrier.cpp)

- Another alternative synchronization method is a `barrier`, which blocks until a set number of threads arrive at the waiting point, at which point they can all proceed.
- `latch` is a similar mechanism, which can be decremented by a thread more than once, but can't be reused after letting a bunch of threads through once.

### Back to execution policies

[`execution.cpp`](execution.cpp)

- We are responsible for avoiding data races and deadlocks when using parallel algorithms (`execution::par`)
- When using vectorized algorithms (`execution::par_unseq`), statements can be _interleaved at compile time_, so that operations from different iterations can be optimized to run in the same thread. This means that any operation that depends on timing or ordering cannot be used.
  - This includes `new`/`delete` and `mutex` operations.
  - I couldn't get this to fail however, which may be because the compiler is too smart (recognizes the mutex and falls back to `execution::par`) or because it's too dumb (can't vectorize the operations properly).
- Note that to sum up a container in parallel without synchronization we can use `reduce`, which is `accumulate` (a fold in functional programming) that can be performed out of order.
