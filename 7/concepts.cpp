#include <concepts>
#include <iostream>

using namespace std;

class foo{};

template<typename T>
concept addable = requires (const T & x) { x + x; }; // requires x + x to be valid

template<typename T> // can also replace typename with addable, or use the auto syntax
requires addable<T>
T add(const T & a, const T & b)
{
	return a + b;
}

int main()
{
	cout << add(1, 2);
	// cout << add(foo(), foo());
}
