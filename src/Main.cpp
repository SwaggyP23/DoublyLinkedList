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
		//temp.push_back({ 5, 5 });
		//temp.push_back({ 6, 8 });
		//temp.push_back({ 4, 2 });
		temp.push_back(pnt1);
		temp.push_back(pnt2);
		temp.push_back(pnt3);
		//temp.emplace_back("emplaced 1", 9, 9);
		//temp.emplace_back("emplaced 2", 8, 8);
		//temp.emplace_back(5, 6);
		display(temp);
		temp.insert({ "Temporary node", 15, 15}, 2);
		display(temp);
		std::cout << "Emplacing front" << std::endl;
		temp.emplace_front("Emplaced Front", 20, 20);
		display(temp);
		std::cout << "Reversing List" << std::endl;
		temp.reverseList();
		display(temp);
		std::cout << "Swapping kth nodes" << std::endl;
		temp.swapKthNodes(2);
		display(temp);
		std::cout << "Clearing and popping" << std::endl;
		temp.pop_back();
		temp.clear();
		display(temp);
	}

	//{
	//	reda::LinkedList<Point> temp("Reda");
	//	Point pnt1(1, 2);
	//	Point pnt2(3, 4);
	//	Point pnt3(5, 6);
	//	//temp.push_back({ 5, 5 });
	//	//temp.push_back({ 6, 8 });
	//	//temp.push_back({ 4, 2 });
	//	temp.push_back((Point&&)pnt1);
	//	temp.push_back((Point&&)pnt2);
	//	temp.push_back((Point&&)pnt3); // These 3 are equivalent to the 3 above them in that they are both moved values
	//}

	//{
	//	reda::LinkedList<int> temp("Reda");
	//	temp.push_back(5);
	//	temp.push_back(6);
	//	temp.push_back(7);
	//}
}