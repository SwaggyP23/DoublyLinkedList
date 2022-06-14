#include <iostream>
#include <concepts>
#include "DoublyLinked/DoublyLinked.h"

struct Point {
	int x, y;
	std::string m_Name;
	int* m_memoryBlock = nullptr;

	Point() : x(0), y(0), m_Name("NO_NAME")
	{
		m_memoryBlock = new int[5];
		for (int i = 0; i < 5; i++)
			m_memoryBlock[i] = 0;
	}

	Point(const std::string& name, int x, int y) : x(x), y(y), m_Name(name)
	{
		m_memoryBlock = new int[5];
		for (int i = 0; i < 5; i++)
			m_memoryBlock[i] = 0;
	}

	Point(const Point& other)
		: x(other.x), y(other.y), m_Name(other.m_Name)
	{
		m_memoryBlock = new int[5];
		memcpy(m_memoryBlock, other.m_memoryBlock, 5 * sizeof(int));

		std::cout << "Coppied\n";
	}

	Point& operator=(const Point& other) // Copy assignment operator
	{
		x = other.x;
		y = other.y;
		m_Name = other.m_Name;
		m_memoryBlock = new int[5];
		memcpy(m_memoryBlock, other.m_memoryBlock, 5 * sizeof(int));

		std::cout << "Coppied\n";
		return *this;
	}

	Point(Point&& other) noexcept
		: x(other.x), y(other.y), m_Name(other.m_Name)
	{
		std::cout << "Moved\n";
		other.x = 0;
		other.y = 0;
		m_memoryBlock = other.m_memoryBlock;
		other.m_memoryBlock = nullptr;
	}

	Point& operator=(Point&& other) noexcept // Move assignment operator
	{
		x = other.x;
		y = other.y;
		m_Name = other.m_Name;
		m_memoryBlock = other.m_memoryBlock;

		other.x = 0;
		other.y = 0;
		other.m_memoryBlock = nullptr;

		std::cout << "Moved\n";
		return *this;
	}

	~Point() {
		std::cout << "Point being Destroyed at: " << m_Name << std::endl;
		delete[] m_memoryBlock;
	}

	friend std::ostream& operator<<(std::ostream& stream, const Point& pnt);

	bool operator==(const Point& right)
	{
		return x == right.x && y == right.y;
	}

	bool operator>(const Point& right) const
	{
		return x > right.x && y > right.y;
	}

	bool operator<(const Point& right) const
	{
		return x < right.x && y < right.y;
	}

};

std::ostream& operator<<(std::ostream& stream, const Point& pnt)
{
	stream << "X: " << pnt.x << ", Y: " << pnt.y;
	return stream;
}

template<typename Type>
void display(reda::LinkedList<Type>& list)
{
	for (int i = 0; i < list.size(); i++)
		std::cout << list[i] << std::endl;

	std::cout << "--------------------------------" << std::endl;
}

int main()
{
	{
		reda::LinkedList<Point> temp("Reda");
		Point pnt1("one", 1, 2);
		Point pnt2("two", 3, 4);
		Point pnt3("three", 5, 6);
		Point pnt4("four", 7, 8);
		temp.push_back(pnt1);
		temp.push_back(pnt2);
		temp.push_back(pnt3);
		temp.push_back(pnt4);
		display(temp);
		temp.reverseList();
		display(temp);
		std::cout << "Testing swap" << std::endl;
		temp.swapNodes(0, 3);
		display(temp);
		//std::cout << "IT\n";
		//reda::LinkedList<Point>::Iterator it(temp.begin());
		//display(temp);
		//std::cout << *(it += 2) << std::endl;
		temp.remove(pnt2);
		temp.pop_front();
		display(temp);

		std::cout << "Testing the iterator" << std::endl;
		std::cout << "Iterator loop" << std::endl;
		for (reda::LinkedList<Point>::Iterator it = temp.begin(); it != temp.end(); it++)
			std::cout << *it << std::endl;

		std::cout << "For each loop" << std::endl;
		for (auto& it : temp)
			std::cout << it << std::endl;

		std::cout << "\n\nPopping and clearing" << std::endl;
		temp.pop_back();
		temp.pop_front();
		temp.clear();
		display(temp);
	}

	//{
	//	reda::LinkedList<int> temp("Reda");
	//	temp.push_back(5);
	//	temp.push_back(6);
	//	temp.push_back(7);
	//}
}