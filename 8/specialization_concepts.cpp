#include <iterator>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

void advance(forward_iterator auto & it, int n)
{
	while (n > 0)
	{
		it++;
		n--;
	}
	cout << "forward\n";
}

void advance(random_access_iterator auto & it, int n)
{
	it += n;
	cout << "random access\n";
}

void advance(auto & it, int n)
{
	it += n;
	cout << "generic\n";
}

int main()
{
	list l {1, 2, 3};
	vector v {1, 2, 3};

	auto lit = l.begin();
	auto vit = v.begin();
	advance(lit, 1);
	advance(vit, 1);
}
