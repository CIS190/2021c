#include <iostream>

using namespace std;

int main()
{
	const int a {0}; // int const is the same
	const int b {1};

	// a++; // not allowed by compiler

	const int * p {&a}; // int const * is the same
	p = &b;
	// (*p)++; // not allowed by compiler

	const int * const q {&a};
	// q = &b; // not allowed by compiler
	// (*p)++; // not allowed by compiler

	cout << *p << " " << *q;
}
