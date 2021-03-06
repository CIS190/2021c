#include <iostream>
#include <cmath>

using namespace std;

class point
{
private:
	int x;
	int y;

public:
	point(int x, int y) : x {x}, y {y}
	{ }

	void move(int dx, int dy)
	{
		x += dx;
		y += dy;
	}

	double getDistance()
	{
		return sqrt(x * x + y * y);
	}

	// can be member function since the first argument is a point
	point operator+(const point & p)
	{
		return point(x + p.x, y + p.y);
	}

	// cannot be member functions since the first argument is not a point (would theoretically have to go into the ostream/istream classes)
	friend ostream & operator<<(ostream &, const point &);
	friend istream & operator>>(istream &, point &);
};

ostream & operator<<(ostream & os, const point & p)
{
	os << p.x << ", " << p.y;

	return os;
}

istream & operator>>(istream & is, point & p)
{
	string s;
	// note that this breaks if we input, e.g. 10,10. Consider is.get(char &) instead for the comma.
	is >> p.x >> s >> p.y;

	if (s != ",")
		is.setstate(ios_base::failbit);

	return is;
}

int main()
{
	point p {1, -1};
	cout << p << "\n";

	if (cin >> p)
		cout << p << "\n";
}
