#include <iostream>
#include <concepts>
#include "DoublyLinked/DoublyLinked.h"

struct Point {
	int x, y;
	Point(int x, int y) : x(x), y(y) {}
	~Point() {
		std::cout << "Point being Destroyed at: " << this << std::endl;
	}
};

void display(reda::LinkedList<Point*>& val) {
	for (int i = 0; i < val.size(); i++) {
		std::cout << "index: " << i << " X val : " << val[i]->x << " ";
		std::cout << "Y val: " << val[i]->y << "\n";
	}
}

int main()
{
	Point* A = new Point(5, 5);
	Point* B = new Point(6, 6);
	Point* C = new Point(7, 7);
	reda::LinkedList<Point*> temp("Reda");
	std::cout << std::boolalpha;
	std::cout << "Is list empty: " << temp.isEmpty() << std::endl;
	temp.push_back(A);
	temp.push_back(B);
	temp.push_back(C);

	std::cout << "X at the back: " << temp.back()->x << std::endl;
	std::cout << "Y at the back: " << temp.back()->y << std::endl;
	std::cout << "X at the front: " << temp.front()->x << std::endl;
	std::cout << "Y at the front: " << temp.front()->y << std::endl;

	temp.reverseList();
	std::cout << "List after reversing: \n";
	display(temp);

	std::cout << "X at the back: " << temp.back()->x << std::endl;
	std::cout << "Y at the back: " << temp.back()->y << std::endl;
	std::cout << "X at the front: " << temp.front()->x << std::endl;
	std::cout << "Y at the front: " << temp.front()->y << std::endl;

	temp.remove(B);
	delete B;

	display(temp);

	std::cout << std::boolalpha;
	std::cout << "Is list empty: " << temp.isEmpty() << std::endl;

	Point* D = new Point(156, 12);
	temp.insert(D, 1);

	display(temp);

	std::cout << "Wassup Mate\n";

	std::cin.get();
	delete A;
	delete C;
}