#pragma once

/*
 * SO, the basic way this stucture works is that when an object is first constructed it calls an allocate functions that 
 * constructs and store a certain amount of hollow nodes in a Dequeue, and whenever a push or insert function is called, one 
 * node is popped from the queue and is connected to the node, when the queue is empty is reallocates another amount of nodes
 * and stores it.
 * 
 * TO DO: Make it that when a pop or erase function is called, the popped node is hollowed and readded to the queue.
 * Create emplace front function, emplace (in middle) function. Create iterator.
 * Make overloads for the erase and insert function that use iterators instead of temporary indexing.
 * Make comments look better by using comment highlighting.
 * Make it so that it is allowed to popBack or front when m_Size == 1 and getting an empty list.
 */

#include <iostream>
#include <deque>

#ifdef DEBUG

#define ASSERT(x) { if(!(x)) { std::cout << "Cannot " __FUNCTION__ << std::endl; __debugbreak(); } }

#else

#define ASSERT(x)

#endif


namespace reda{

    template<typename T>
    struct Node {
        T val;
        Node* next;
        Node* prev;
        Node() : next(nullptr), prev(nullptr) {}

        Node(const T& val) : val(val), next(nullptr), prev(nullptr) {}

        Node(const T& val, Node* next, Node* prev) : val(val), next(next), prev(prev) {}

        Node(T&& val) noexcept : val((T&&)val), next(nullptr), prev(nullptr) {}

        ~Node() {
            std::cout << "Node being destroyed!" << std::endl;
        }
    };

    template<typename T>
    class LinkedList{
    private:
        using nodePtr = Node<T>*;

        std::string m_Name;

        nodePtr m_Head;
        nodePtr m_Curr;
        nodePtr m_Tail;

        size_t m_Size;

        std::deque<nodePtr> m_AvailableNodes; // So this is used in a way that we can issue one function call that allocates 
                                              // a certain number of nodes and adds to the queue, then we can pop from the 
                                              // queue and use the nodes to link to the list.

        bool m_FirstElement;

    public:
        LinkedList()
            : m_Name("List"), m_Head(nullptr), m_Curr(nullptr), m_Tail(nullptr)
        {
            m_Size = 0;
            m_FirstElement = true;
            AllocateNodes(5);
            
        }

        LinkedList(std::string name) 
            : m_Name(name), m_Head(nullptr), m_Curr(nullptr), m_Tail(nullptr)
        {
            m_Size = 0;
            m_FirstElement = true;
            AllocateNodes(5);
        }

    // This is temporary just to access and print things to the console to ease the debugging before implementing [] operator
#if DEBUG
        const T& getAt(int index)
        {
            ASSERT(index <= m_Size - 1);

            int i = 0;
            m_Curr = m_Head;
            while (i < index) {
                m_Curr = m_Curr->next;
                i++;
            }
            return m_Curr->val;
        }
#endif

    // Append lvalue new node to the end of the list. Works in O(1) since it is appending to the tail directly
        void push_back(const T& val)
        {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val) T(val); // Copies the value

            if (m_FirstElement) {
                m_Head = newNode;
                m_Tail = newNode;
                m_Size++;
                m_FirstElement = false;
                return;
            }

            m_Tail->next = newNode;
            newNode->prev = m_Tail;

            m_Tail = newNode;
            m_Size++;
            return;
        }

    // Append rvalue new node to the end of the list
        void push_back(T&& val) 
        {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val)  T(std::move(val)); // Moves the value

            if (m_FirstElement) {
                m_Head = newNode;
                m_Tail = newNode;
                m_Size++;
                m_FirstElement = false;
                return;
            }

            m_Tail->next = newNode;
            newNode->prev = m_Tail;

            m_Tail = newNode;
            m_Size++;
            return;
        }

    // Adds a new node to the end of the list, however it creates the value object in place of its memory directly and no 
    // moving or copying is needed
        template<typename... Args>
        void emplace_back(Args&&... args)
        {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();

            new((void*)&newNode->val) T(std::forward<Args>(args)...);

            if (m_FirstElement) {
                m_Head = newNode;
                m_Tail = newNode;
                m_Size++;
                m_FirstElement = false;
                return;
            }

            m_Tail->next = newNode;
            newNode->prev = m_Tail;

            m_Tail = newNode;
            m_Size++;
            return;
        }

    // Append lvalue new node to the beginning of the list.
        void push_front(const T& val)
        {
            ASSERT(m_Head);

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);
            
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)& newNode->val) T(val); // Copies the value

            m_Head->prev = newNode;
            newNode->next = m_Head;
            m_Head = m_Head->prev;
            m_Size++;
        }

    // Append rvalue new node to the beginning of the list.
        void push_front(T&& val) 
        {
            ASSERT(m_Head);

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val)  T(std::move(val)); // Moves the value

            m_Head->prev = newNode;
            newNode->next = m_Head;
            m_Head = m_Head->prev;
            m_Size++;
        }

    // Inserts lvalue element in the list at the specified position. Create Iterator overload
        void insert(const T& val, int index)
        {
            m_Curr = m_Head;
            int i = 0;

            while (i < index) {
                m_Curr = m_Curr->next;
                i++;
            } // After exiting the loop i will equal whatever the index is and m_Curr will be on the node of index (index)
         
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val) T(val); // Copies the value

            newNode->prev = m_Curr->prev;
            m_Curr->prev->next = newNode;
            m_Curr->prev = newNode;
            newNode->next = m_Curr;
            m_Size++;
        }

    // Inserts rvalue element in the list at the specified position. Create Iterator overload
        void insert(T&& val, int index) 
        {
            m_Curr = m_Head;
            int i = 0;

            while (i < index) {
                m_Curr = m_Curr->next;
                i++;
            } // After exiting the loop i will equal whatever the index is and m_Curr will be on the node of index (index)

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val) T(std::move(val)); // Moves the value

            newNode->prev = m_Curr->prev;
            m_Curr->prev->next = newNode;
            m_Curr->prev = newNode;
            newNode->next = m_Curr;
            m_Size++;
        }

    // Removes the last element in the list.
        void pop_back()
        {
            if (m_Size > 0)
            {
                m_Tail = m_Tail->prev; // Rechanging the Tail pointer position so that it doesnt get deleted in the hollowing

                m_Tail->next->val.~T();                 // Here we create a hollow object
                m_Tail->next->next = nullptr;
                m_Tail->next->prev = nullptr;

                m_AvailableNodes.push_back(m_Tail->next);

                m_Tail->next = nullptr;

                m_Size--;
            }
        }

    // Removes the first element in the list.
        void pop_front()
        {
            ASSERT(m_Head);
            
            if (m_Size > 0)
            {
                m_Head = m_Head->next; // Rechanging the Head pointer position so that it doesnt get deleted in the hollowing
                
                m_Head->prev->val.~T();
                m_Head->prev->next = nullptr;
                m_Head->prev->prev = nullptr;

                m_AvailableNodes.push_back(m_Head->prev);

                m_Head->prev = nullptr;

                m_Size--;
            }
        }

    // Remove a single element from the list identified by index. Create iterator overload
        void erase(int index)
        {
            m_Curr = m_Head;
            
            if (index == 0) {
                ASSERT(m_Size > 1);
                pop_front();
                return;
            }

            else if (index == m_Size - 1) {
                ASSERT(m_Size > 1);
                pop_back();
                return;
            }
            
            int i = 0;
            while(i < index){
                m_Curr = m_Curr->next;
                i++;
            }

            m_Curr->prev->next = m_Curr->next;
            m_Curr->next->prev = m_Curr->prev;

            m_Curr->val.~T();
            m_Curr->next = nullptr;
            m_Curr->prev = nullptr;

            m_AvailableNodes.push_back(m_Curr);

            m_Size--;
        }

    // Deletes all the nodes or CLEARS them and pushes the hollowed nodes into the deque
        void clear()
        {
            m_Curr = m_Head;

            while(m_Curr)
            {
                nodePtr temp = m_Curr->next;

                m_Curr->val.~T();
                m_Curr->next = nullptr;
                m_Curr->prev = nullptr;

                m_AvailableNodes.push_back(m_Curr);
                m_Curr = temp;
            }

            m_Size = 0;
        }

    // Removes all instances of passed value found in list. If T is a pointer to memory, its memory is not 
    // destroyed, that has to be done manually.
        //void remove(const T& data){
        //    curr = m_Head;
        //    
        //    while(curr){
        //        if(curr->val == data and curr == m_Head){
        //            m_Head = m_Head->next;
        //            curr->next->prev.reset();
        //            length--;
        //        }
        //        else if(curr->val == data and curr == m_Tail){
        //            m_Tail = m_Tail->prev;
        //            curr->prev->next.reset();
        //            curr->prev.reset();
        //            length--;
        //        }
        //        else if(curr->val == data){
        //            nodePtr temp = curr->prev;
        //            temp->next = curr->next;
        //            curr->next->prev = temp;
        //            length--;
        //        }
        //        curr = curr->next;
        //    }
        //}
 
    // Reverses the list.
        void reverseList()
        {
            m_Curr = m_Head;
            nodePtr temp = nullptr;

            //swapping next and prev pointers
            while(m_Curr)
            {
                temp = m_Curr->prev;
                m_Curr->prev = m_Curr->next;
                m_Curr->next = temp;
                m_Curr = m_Curr->prev;
            }

            //resetting m_Tail and m_Head
            temp = m_Head;
            m_Head = m_Tail;
            m_Tail = temp;
        }

        #if 0
    // Swaps the Kth largest element with the Kth smallest element in the list.
        void swapNodes(const int& k)
        {
            if(k == 0) return;

            curr = m_Head;
            nodePtr left = m_Head, right = m_Head;
            int count = 1;

            while(curr){
                if(count < k) left = left->next;
                else if (count > k) right = right->next;
                curr = curr->next;
                count++;
            }

            T temp = left->val;
            left->val = right->val;
            right->val = temp;
        }
        #endif
    // Returns the val at the specified index in the list.
        T& operator[](unsigned int index)
        {
            ASSERT(index <= m_Size - 1);

            unsigned int i = 0;
            m_Curr = m_Head;
            while (i < index) {
                m_Curr = m_Curr->next;
                i++;
            }
            return m_Curr->val;
        }

    // Returns true if the List is empty, otherwise false.
        inline bool isEmpty() const
        {
            if(!m_Head) return true;
            return false;
        }

    // Returns the size of the list.
        inline const int size() const
        {
            return m_Size;
        }

    // Returns the value of the first element in the list.
        inline const T& front() const
        {
            return m_Head->val;
        }

    // Returns the value of the last element in the list.
        inline const T& back() const
        {
            return m_Tail->val;
        }

    // Returns a pointer pointing to the beginning of the list.
        inline nodePtr begin() const 
        {
            if(m_Head) return m_Head;
            return nullptr;
        }

    // Returns a pointer pointing to the end of the list.
        inline nodePtr end() const 
        {
            if(m_Tail) return m_Tail;
            return nullptr;
        }

    // Destructor
        ~LinkedList()
         {
            shutDown();
        }

    private:
    // Allocates a nodeCount number of empty nodes and stores them in a Deque ready for popping and use.
        void AllocateNodes(size_t nodeCount)
        {
            for (size_t i = 0; i < nodeCount; i++)
                m_AvailableNodes.push_back(new Node<T>());
        }

    // This functions just like the public clear function however it also deletes the memory of each node after 
    // destructing the T val in it, this is for the destructor only.
        void shutDown()
        {
            m_Curr = m_Head;

            while (m_Curr)
            {
                nodePtr temp = m_Curr->next;

                m_Curr->val.~T();
                ::operator delete(m_Curr);

                m_Curr = temp;
            }

            while (!m_AvailableNodes.empty())
            {
                std::cout << "Deleting nodes in Queue\n";

                m_Curr = m_AvailableNodes.front();
                m_Curr->val.~T();
                m_AvailableNodes.pop_front();
            }

            m_Size = 0;
        }
    };
}