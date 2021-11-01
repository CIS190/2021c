#include <iostream>
#include <concepts>

using namespace std;

class foo
{
public:
	foo() {};
	foo & operator=(const foo &) = delete; // disable the copy constructor
};

template <typename T>
requires copyable<T> // Could also replace typename above with copyable instead of this
class wrapper
{
    T t;
public:
    wrapper(T t) : t {t} {}

    T get() const
    {
        return t;
    }

    void set(T t)
    {
        this->t = t;
    }
};

int main()
{
    wrapper<foo> w {foo()};
	// w.set(foo()); // if we never use set, then this will compile if there is no type constraint
}
