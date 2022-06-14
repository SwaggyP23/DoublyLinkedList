#pragma once

/*
 * SO, the basic way this stucture works is that when an object is first constructed it calls an allocate functions that 
 * constructs and store a certain amount of hollow nodes in a Dequeue, and whenever a push or insert function is called, one 
 * node is popped from the queue and is connected to the node, when the queue is empty is reallocates another amount of nodes
 * and stores it.
 * 
 * TO DO: Make it that when a pop or erase function is called, the popped node is hollowed and readded to the queue. (FIX IT)
 * Create Copy and Move constructors and assignment operator for the LinkedList itself so that it is copyable and movable.
 * Make comments look better by using comment highlighting.
 * Fix insert and emplace functions to handle the case of index == 0.
 */

#include <iostream>
#include <deque>

#define USE_Q 1

#ifdef DEBUG

#define ASSERT(x) { if(!(x)) { std::cout << "Cannot " __FUNCTION__ << std::endl; __debugbreak(); } }

#else

#define ASSERT(x)

#endif


namespace reda{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// ITERATOR
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // The way this iterator works is just like in a vector iterator you do + int to the pointer to change iterator position,
    // instead since this is a linked list and all the memory is on the heap and not contiguous we need to traverse n times
    // the next pointers of the nodes until we reach the specified target, if n leads to out of bounds the program will crash
    // and no asserts have been set up for that

    template<typename LinkedList>
    class ListIterator
    {
    public:
        using valueType             = LinkedList::valueType;
        using pointerType           = valueType*;
        using referenceType         = valueType&;
        using it_Ptr                = LinkedList::NodePtr;

    public:
        ListIterator(it_Ptr ptr)
            : m_Ptr(ptr) {}

        ListIterator& operator++()
        {
            m_Ptr = m_Ptr->next;
            return *this;
        }

        ListIterator operator++(int)
        {
            ListIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        ListIterator& operator--()
        {
            m_Ptr = m_Ptr->prev;
            return *this;
        }

        ListIterator operator--(int)
        {
            ListIterator iterator = *this;
            --(*this);
            return iterator;
        }

        ListIterator operator+(int right)
        {
            // Here we should be asserting in case we go out of bounds but i cant find a general way to do that.

            for (int i = 0; i < right; i++)
            {
                ++(*this);
            }

            return m_Ptr;
        }

        ListIterator operator-(int right)
        {
            // Here we should be asserting in case we go out of bounds but i cant find a general way to do that.

            for (int i = 0; i < right; i++)
            {
                --(*this);
            }

            return m_Ptr;
        }

        ListIterator& operator+=(int right)
        {
            *this + right;

            return *this;
        }

        ListIterator& operator-=(int right)
        {
            *this - right;

            return *this;
        }

        bool operator==(const ListIterator& right) const
        {
            return m_Ptr == right.m_Ptr;
        }

        bool operator!=(const ListIterator& right) const
        {
            return !(*this == right);
        }

        bool operator<(const ListIterator& right) const
        {
            return m_Ptr < right.m_Ptr;
        }

        bool operator>(const ListIterator& right) const
        {
            m_Ptr > right.m_Ptr;
        }

        bool operator<=(const ListIterator& right) const
        {
            m_Ptr <= right.m_Ptr;
        }

        bool operator>=(const ListIterator& right) const
        {
            m_Ptr >= right.m_Ptr;
        }

        pointerType operator->()
        {
            return m_Ptr; // The pointer here is also the position we are at.
        }

        referenceType operator*()
        {
            return m_Ptr->val;
        }

    private:
        it_Ptr m_Ptr;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// LIST
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#if DEBUG
            std::cout << "Node being destroyed!" << std::endl;
#endif
        }

        bool operator==(const Node& right) const
        {
            return val == right.val;
        }

        bool operator!=(const Node& right) const
        {
            return !(*this == right);
        }

        bool operator<(const Node& right) const
        {
            return val < right.val;
        }

        bool operator>(const Node& right) const
        {
            val > right.val;
        }

        bool operator<=(const Node& right) const
        {
            val <= right.val;
        }

        bool operator>=(const Node& right) const
        {
            val >= right.val;
        }
    };

    template<typename T>
    class LinkedList
    {
    private:
        using nodePtr = Node<T>*;

        std::string m_Name;

        nodePtr m_Head;
        nodePtr m_Curr;
        nodePtr m_Tail;

        size_t m_Size;

#if USE_Q
        std::deque<nodePtr> m_AvailableNodes; // So this is used in a way that we can issue one call that allocates
                                              // a certain number of nodes and adds them to the queue, then we can pop from
                                              // the queue and use the popped new nodes in the list
#endif

        bool m_FirstElement;

        struct doubleNodes
        {
            nodePtr first;
            nodePtr second;
        };

    public:
        using valueType = T;
        using Iterator = ListIterator<LinkedList<T>>;
        using NodePtr = nodePtr;

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

    // Append lvalue new node to the end of the list. Works in O(1) since it is appending to the tail directly
        void push_back(const T& val)
        {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);
#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
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

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
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

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif

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
            
#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
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

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
            new((void*)&newNode->val)  T(std::move(val)); // Moves the value

            m_Head->prev = newNode;
            newNode->next = m_Head;
            m_Head = m_Head->prev;

            m_Size++;
        }

        // Adds a new node to the beginning of the list, however it creates the value object in place of its memory directly 
        // and no moving or copying is needed.
        template<typename... Args>
        void emplace_front(Args&&... args)
        {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif

            new((void*)&newNode->val) T(std::forward<Args>(args)...);

            if (m_FirstElement) {
                m_Head = newNode;
                m_Tail = newNode;
                m_Size++;
                m_FirstElement = false;
                return;
            }

            m_Head->prev = newNode;
            newNode->next = m_Head;

            m_Head = m_Head->prev;
            m_Size++;
            return;
        }

    // Inserts lvalue element in the list at the specified position. Create Iterator overload
        void insert(unsigned int index, const T& val)
        {
            if (index == m_Size)
                push_back(val);

            m_Curr = m_Head;

            for(unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided
         
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
            new((void*)&newNode->val) T(val); // Copies the value

            // Here for the patching, m_Curr is at the node that will be pushed forward one position

            newNode->prev = m_Curr->prev;
            m_Curr->prev->next = newNode;
            m_Curr->prev = newNode;
            newNode->next = m_Curr;

            m_Size++;
        }

    // Inserts rvalue element in the list at the specified position. Create Iterator overload
        void insert(unsigned int index, T&& val)
        {
            if (index == m_Size)
                push_back((T&&)val);

            m_Curr = m_Head;

            for (unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif
            new((void*)&newNode->val) T(std::move(val)); // Moves the value

            // Here for the patching, m_Curr is at the node that will be pushed forward one position

            newNode->prev = m_Curr->prev;
            m_Curr->prev->next = newNode;
            m_Curr->prev = newNode;
            newNode->next = m_Curr;

            m_Size++;
        }

        // Adds a new node to the specified position in the list, however it creates the value object in place of its memory 
        // directly and no moving or copying is needed.
        template<typename... Args>
        void emplace(unsigned int index, Args&&... args)
        {
            if (index == m_Size)
                emplace_back(args...);

            m_Curr = m_Head;

            for (unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

#if USE_Q
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
#else
            nodePtr newNode = new Node<T>();
#endif

            new((void*)&newNode->val) T(std::forward<Args>(args)...); // Create the val in-place

            // Here for the patching, m_Curr is at the node that will be pushed forward one position

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
                if (m_Size == 1)
                {
                    m_FirstElement = true;

                    m_Head = nullptr;
                    m_Curr = nullptr;

                    delete m_Tail;
                    m_Tail = nullptr;

                    m_Size--;
                    return;
                }

                m_Tail = m_Tail->prev; // Rechanging the Tail pointer position so that it doesnt get deleted in the hollowing

                //m_Tail->next->val.~T();                 // Here we create a hollow object
                //m_Tail->next->next = nullptr;
                //m_Tail->next->prev = nullptr;
                m_Curr = m_Tail->next;
                delete m_Curr;
                m_Curr = nullptr;

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
                if (m_Size == 1)
                {
                    m_FirstElement = true;

                    m_Tail = nullptr;
                    m_Curr = nullptr;

                    delete m_Head;
                    m_Head = nullptr;

                    m_Size--;
                    return;
                }

                m_Head = m_Head->next; // Rechanging the Head pointer position so that it doesnt get deleted in the hollowing
                
                //m_Head->prev->val.~T();
                //m_Head->prev->next = nullptr;
                //m_Head->prev->prev = nullptr;
                m_Curr = m_Head->prev;
                delete m_Curr;
                m_Curr = nullptr;

                m_Head->prev = nullptr;

                m_Size--;
            }
        }

    // Remove a single element from the list identified by index. Create iterator overload
        void erase(int index)
        {
            m_Curr = m_Head;
            
            if (index == 0) {
                pop_front();
                return;
            }

            else if (index == m_Size - 1) {
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

            //m_Curr->val.~T();
            //m_Curr->next = nullptr;
            //m_Curr->prev = nullptr;
            delete m_Curr;
            m_Curr = nullptr;

            m_Size--;
        }

    // Deletes all the nodes or CLEARS them and pushes the hollowed nodes into the deque
        void clear()
        {
            m_Curr = m_Head;

            for (size_t i = 0; i < m_Size; i++)
            {
                nodePtr temp = m_Curr->next;
                //m_Curr->val.~T();
                //m_Curr->next = nullptr;
                //m_Curr->prev = nullptr;
                delete m_Curr;
                m_Curr = temp;
            }

            m_Head = nullptr;
            m_Curr = nullptr;
            m_Tail = nullptr;

            m_Size = 0;
        }

    // Removes all instances of passed value found in list. If T is a pointer to memory, its memory is not 
    // destroyed, that has to be done manually.
        void remove(const T& data){
            m_Curr = m_Head;
            
            while(m_Curr){
                if (m_Curr->val == data and m_Curr == m_Head) {
                    m_Curr = m_Curr->next;
                    pop_front();

                    continue;
                }

                else if(m_Curr->val == data and m_Curr == m_Tail)
                    pop_back();

                else if(m_Curr->val == data){
                    nodePtr delPtr = m_Curr;
                    m_Curr->prev->next = m_Curr->next;
                    m_Curr->next->prev = m_Curr->prev;

                    m_Curr = m_Curr->next;
                    delete delPtr;

                    m_Size--;
                    continue;
                }

                m_Curr = m_Curr->next;
            }
        }
 
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
        
        void swapNodes(int index1, int index2)
        {
            doubleNodes dn = findNodes(index1, index2);

            nodePtr Node1 = dn.first;
            nodePtr Node2 = dn.second;

            swappingTheNodes(Node1, Node2);
        }

    // Swaps the Kth largest element with the Kth smallest element in the list.
        void swapKthNodes(int k)
        {
            if(k == 0) return;

            doubleNodes dn = findKthNodes(k);

            nodePtr Node1 = dn.first;
            nodePtr Node2 = dn.second;

            swappingTheNodes(Node1, Node2);
        }

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
        inline Iterator begin() const 
        {
            if (m_Head) return Iterator(m_Head);
            return nullptr;
        }

    // Returns a pointer pointing to the end of the list.
        inline Iterator end() const
        {
            if(m_Tail) return Iterator(m_Tail->next);
            return nullptr;
        }

    // Destructor
        ~LinkedList()
        {
            shutDown();
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

    private:
    // Allocates a nodeCount number of empty nodes and stores them in a Deque ready for popping and use.
        void AllocateNodes(size_t nodeCount)
        {
            for (size_t i = 0; i < nodeCount; i++)
                m_AvailableNodes.push_back(new Node<T>());
        }

    // Helper function for the swapKthNodes function.
        doubleNodes findKthNodes(int k)
        {
            m_Curr = m_Head;
            nodePtr left = m_Head, right = m_Head;
            int count = 1;

            while (m_Curr)
            {
                if (count < k) left = left->next;
                else if (count > k) right = right->next;

                m_Curr = m_Curr->next;
                count++;
            }

            return { left, right };
        }

    // Helper function for the swapNodes function.
        doubleNodes findNodes(int left, int right)
        {
            m_Curr = m_Head;
            nodePtr leftNode = m_Head, rightNode = m_Head;
            int l = 0, r = 0;

            while (m_Curr)
            {
                if (l != left) {
                    leftNode = leftNode->next;
                    l++;
                }
                if (r != right) {
                    rightNode = rightNode->next;
                    r++;
                }

                m_Curr = m_Curr->next;
            }

            return { leftNode, rightNode };
        }

    // Helper function for both swapping functions
        void swappingTheNodes(nodePtr& Node1, nodePtr& Node2)
        {
            if (Node1 == m_Head)
                m_Head = Node2;
            else if (Node2 == m_Head)
                m_Head = Node1;
            if (Node1 == m_Tail)
                m_Tail = Node2;
            else if (Node2 == m_Tail)
                m_Tail = Node1;

            // Swapping Node1 and Node2
            nodePtr temp;
            temp = Node1->next;
            Node1->next = Node2->next;
            Node2->next = temp;

            if (Node1->next != NULL)
                Node1->next->prev = Node1;
            if (Node2->next != NULL)
                Node2->next->prev = Node2;

            temp = Node1->prev;
            Node1->prev = Node2->prev;
            Node2->prev = temp;

            if (Node1->prev != NULL)
                Node1->prev->next = Node1;
            if (Node2->prev != NULL)
                Node2->prev->next = Node2;
        }

    // This functions just like the public clear function however it also deletes the memory of each node after 
    // destructing the T val in it, this is for the destructor only.
        void shutDown()
        {
            m_Curr = m_Head;

            for (size_t i = 0; i < m_Size; i++) {
                nodePtr temp = m_Curr->next;

                //m_Curr->val.~T();
                //m_Curr->next = nullptr;
                //m_Curr->prev = nullptr;
                delete m_Curr;

                m_Curr = temp;
            }

            m_Size = 0;

            while (!m_AvailableNodes.empty())
            {
#if DEBUG
                std::cout << "Deleting nodes in Queue\n";
#endif

                m_Curr = m_AvailableNodes.front();
                delete m_Curr;
                m_AvailableNodes.pop_front();
            }
        }
    };
}