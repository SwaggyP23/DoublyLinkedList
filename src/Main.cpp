#include <iostream>
#include <concepts>
#include "DoublyLinked/DoublyLinked.h"

struct Point {
	int x, y;

	Point() : x(0), y(0) {}

	Point(int x, int y) : x(x), y(y) {}

	Point(const Point& other)
		: x(other.x), y(other.y)
	{
		std::cout << "Coppied\n";
	}

	Point& operator=(const Point& other) // Copy assignment operator
	{
		x = other.x;
		y = other.y;
		//m_MemoryBlock = new int[5];
		//memcpy(m_MemoryBlock, other.m_MemoryBlock, 5 * sizeof(int));

		std::cout << "Coppied\n";
		return *this;
	}

	Point(Point&& other) noexcept
		: x(other.x), y(other.y)
	{
		std::cout << "Moved\n";
		other.x = 0;
		other.y = 0;
	}

	Point& operator=(Point&& other) noexcept { // Move assignment operator

		//m_MemoryBlock = other.m_MemoryBlock;
		//other.m_MemoryBlock = nullptr;
		x = other.x;
		y = other.y;

		other.x = 0;
		other.y = 0;

		std::cout << "Moved\n";
		return *this;
	}

	~Point() {
		std::cout << "Point being Destroyed at: " << this << std::endl;
	}

	friend std::ostream& operator<<(std::ostream& stream, const Point& pnt);

	bool operator==(const Point& right)
	{
		return x == right.x && y == right.y;
	}

};

std::ostream& operator<<(std::ostream& stream, const Point& pnt)
{
	stream << "X: " << pnt.x << ", Y: " << pnt.y;
	return stream;
}

//template<typename Type>
//void display(const reda::LinkedList<Type>& list)
//{
//	for (int i = 0; i < list.size(); i++)
//		std::cout << list[i] << std::endl;
//}

int main()
{
	{
		reda::LinkedList<Point> temp("Reda");
		Point pnt1(1, 2);
		Point pnt2(3, 4);
		Point pnt3(5, 6);
		Point pnt4(7, 8);
		//temp.push_back({ 5, 5 });
		//temp.push_back({ 6, 8 });
		//temp.push_back({ 4, 2 });
		temp.push_back(pnt1);
		temp.push_back(pnt2);
		temp.push_back(pnt3);
		//temp.emplace_back(1, 2);
		//temp.emplace_back(3, 4);
		//temp.emplace_back(5, 6);
		std::cout << temp.getAt(pnt2) << std::endl;
		std::cout << temp.getAt(pnt3) << std::endl;
		temp.push_front((Point&&)pnt4);
		//std::cout << temp.getAt(pnt4) << std::endl;
	}

	{
		reda::LinkedList<Point> temp("Reda");
		Point pnt1(1, 2);
		Point pnt2(3, 4);
		Point pnt3(5, 6);
		//temp.push_back({ 5, 5 });
		//temp.push_back({ 6, 8 });
		//temp.push_back({ 4, 2 });
		temp.push_back((Point&&)pnt1);
		temp.push_back((Point&&)pnt2);
		temp.push_back((Point&&)pnt3); // These 3 are equivalent to the 3 above them in that they are both moved values
	}

	{
		reda::LinkedList<int> temp("Reda");
		temp.push_back(5);
		temp.push_back(6);
		temp.push_back(7);
	}
}