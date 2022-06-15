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
void display(const reda::LinkedList<Type>& list)
{
	for (auto& it : list)
		std::cout << it << std::endl;

	std::cout << "--------------------------------" << std::endl;
}

int main()
{
	{
		reda::LinkedList<Point> list;
		list.push_back({ "one", 5, 5 });
		list.push_back({ "two", 6, 6 });
		list.push_back({ "three", 7, 7 });
		list.push_back({ "four", 8, 8 });
		list.push_back({ "five", 9, 9 });

		reda::LinkedList<Point> move(std::move(list));
		printf("move main\n");
		move.pop_back();
		display(move);
		std::cin.get();
		list = std::move(move);
		printf("list main\n");
		list.pop_front();
		display(list);
		std::cin.get();
		move = std::move(list);
		printf("move main\n");
		move.pop_back();
		display(move);
		std::cin.get();
		list = std::move(move);
		printf("list main\n");
		list.pop_front();
		display(list);
		std::cin.get();
		printf("Clearing on both lists\n");
		list.clear();
		move.clear();
	}
}