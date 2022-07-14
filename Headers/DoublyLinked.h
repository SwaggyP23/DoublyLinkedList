#pragma once
#ifndef _REDA_LIST_
#define _REDA_LIST_

/*
 * SO, the basic way this stucture works is that when an object is first constructed it calls an allocate functions that 
 * constructs and store a certain amount of hollow nodes in a Dequeue, and whenever a push or insert function is called, one 
 * node is popped from the queue and is connected to the node, when the queue is empty it reallocates another amount of nodes
 * and stores it.
 * The main reason of even doing this thing is to reduce heap allocation, so that we dont have to allocate memory on the heap everytime
 * we want to insert a node. Hence, we just allocate a certain amount of nodes at one time and untill the allocated amount is depleted
 * that is when we allocate again.
 * The amount of nodes that are allocated is defaulted to 5, however it is possible to change to any other custom amount you want.
 */

#include <iostream>
#include <deque>
#include <initializer_list>

#ifdef _REDA_DEBUG

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
    class BaseIterator
    {
    public:
        using valueType = typename LinkedList::valueType;
        using pointerType = valueType*;
        using referenceType = valueType&;
        using it_Ptr = LinkedList::NodePtr;
    public:
        BaseIterator(it_Ptr ptr)
            : m_Ptr(ptr) {}

        pointerType operator->() const
        {
            return m_Ptr; // The pointer here is also the position we are at.
        }

        referenceType operator*() const
        {
            return this->m_Ptr->val;
        }

        BaseIterator& operator++()
        {
            m_Ptr = m_Ptr->next;
            return *this;
        }

        BaseIterator operator++(int)
        {
            BaseIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        BaseIterator& operator--()
        {
            m_Ptr = m_Ptr->prev;
            return *this;
        }

        BaseIterator operator--(int)
        {
            BaseIterator iterator = *this;
            --(*this);
            return iterator;
        }

        BaseIterator operator+(int right)
        {
            // Here we should be asserting in case we go out of bounds but i cant find a general way to do that.

            for (int i = 0; i < right; i++)
            {
                ++(*this);
            }

            return m_Ptr;
        }

        BaseIterator operator-(int right)
        {
            // Here we should be asserting in case we go out of bounds but i cant find a general way to do that.

            for (int i = 0; i < right; i++)
            {
                --(*this);
            }

            return m_Ptr;
        }

        BaseIterator& operator+=(int right)
        {
            *this + right;

            return *this;
        }

        BaseIterator& operator-=(int right)
        {
            *this - right;

            return *this;
        }

        bool operator==(const BaseIterator& right) const
        {
            return m_Ptr == right.m_Ptr;
        }

        bool operator!=(const BaseIterator& right) const
        {
            return !(*this == right);
        }

        bool operator<(const BaseIterator& right) const
        {
            return m_Ptr < right.m_Ptr;
        }

        bool operator>(const BaseIterator& right) const
        {
            m_Ptr > right.m_Ptr;
        }

        bool operator<=(const BaseIterator& right) const
        {
            m_Ptr <= right.m_Ptr;
        }

        bool operator>=(const BaseIterator& right) const
        {
            m_Ptr >= right.m_Ptr;
        }

    protected:
        it_Ptr m_Ptr;
    };

    template<typename LinkedList>
    class ConstIterator : public  BaseIterator<LinkedList>
    {
    public:
        using valueType = typename LinkedList::valueType;
        using reference = const valueType&;

        reference operator*() const
        {
            return this->m_Ptr->val;
        }

        ConstIterator& operator++()
        {
            this->m_Ptr = this->m_Ptr->next;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        ConstIterator& operator--()
        {
            this->m_Ptr = this->m_Ptr->prev;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator iterator = *this;
            --(*this);
            return iterator;
        }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// NODE
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct Node {
        T val;
        Node* next;
        Node* prev;
        Node() 
            : val{}, next(nullptr), prev(nullptr) {}

        Node(const T& val) 
            : val(val), next(nullptr), prev(nullptr) {}

        Node(const T& val, Node* next, Node* prev) 
            : val(val), next(next), prev(prev) {}

        Node(T&& val) 
            : val((T&&)val), next(nullptr), prev(nullptr) {}

        Node(const Node& other) // Copy constructor
            : val(other.val), next(other.next), prev(other.prev) {}

        Node(Node&& other)      // Move constructor
            : next(other.next), prev(other.prev)
        {
            val = std::move(other.val);

            other.next = nullptr;
            other.prev = nullptr;
        }

        ~Node()
        {
#ifdef _REDA_DEBUG
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// LIST
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    class LinkedList
    {
    private:
        using nodePtr = Node<T>*;

        nodePtr m_Head;
        nodePtr m_Curr;
        nodePtr m_Tail;

        size_t m_Size;

#ifdef _REDA_LIST_USE_Q
        std::deque<nodePtr> m_AvailableNodes;
        // So this is used in a way that we can issue one call that allocates a certain number of nodes and adds them to the
        // queue, then we can pop from the queue and use the popped new nodes in the list.
#endif

        bool m_FirstElement;

        struct doubleNodes
        {
            nodePtr first;
            nodePtr second;
        };

    public:
        using valueType = T;
        using Iterator = BaseIterator<LinkedList<T>>;
        using const_Iterator = ConstIterator<const LinkedList<T>>;
        using NodePtr = nodePtr;

    public:
        LinkedList()
            : m_Head(nullptr), m_Curr(nullptr), m_Tail(nullptr), m_Size(0), m_FirstElement(true)
        {
#ifdef _REDA_LIST_USE_Q
            AllocateNodes(5);
#endif
        }
        
    // This constructor works like a reserve function in a vector class, it takes a size and allocates at construction 
    // size number of nodes therefore we wont need to allocate on the heap everytime we push or emplace untill the 
    // m_AvailableNodes is empty.
#ifdef _REDA_LIST_USE_Q
        LinkedList(unsigned int reserveSize) 
            : m_Head(nullptr), m_Curr(nullptr), m_Tail(nullptr), m_Size(0), m_FirstElement(true)
        {
            AllocateNodes(reserveSize);
        }
#endif

    // This constructors supports braced initializer-list LinkedList initialization.
        LinkedList(std::initializer_list<T> initList)
            : m_Size(0), m_FirstElement(true)
        {
#ifdef _REDA_LIST_USE_Q
            AllocateNodes(m_Size);
#endif

            for (auto it = initList.begin(); it != initList.end(); ++it)
                emplace_back(*it);
            
        }

    // Here instead of initiazlizing the pointers, we need to copy and allocate new nodes
        LinkedList(const LinkedList& other) // Copy constructor
            : m_Size(0), m_FirstElement(true)
        {
#ifdef _REDA_LIST_USE_Q
            AllocateNodes(m_Size);
#endif

            Iterator it = other.begin();

            while (it != other.end())
            {
                push_back(*it);
                ++it;
            }
        }

        LinkedList& operator=(const LinkedList& other) // Copy assignment operator
        {
            m_FirstElement = true;
            m_Size = 0;
            
#ifdef _REDA_LIST_USE_Q
            AllocateNodes(m_Size);
#endif

            Iterator it = other.begin();

            while (it != other.end())
            {
                push_back(*it);
                ++it;
            }

            return *this;
        }

    // For moving all the object we dont really need to allocate new nodes and delete the old ones since they are just
    // all pointers, therefore we can only transfer the ownership to the new List being constructed, and nullifying 
    // all the pointers in the old list.
        LinkedList(LinkedList&& other) noexcept // Move constructor
            : m_Head(other.m_Head), m_Curr(other.m_Curr), m_Tail(other.m_Tail), m_Size(other.m_Size),
                m_FirstElement(other.m_FirstElement)
        {
            other.m_Head = nullptr;
            other.m_Curr = nullptr;
            other.m_Tail = nullptr;
            other.m_Size = 0;
            other.m_FirstElement = false;

#ifdef _REDA_LIST_USE_Q
            m_AvailableNodes = std::move(other.m_AvailableNodes);
            //std::move(other.m_AvailableNodes.begin(), other.m_AvailableNodes.end(), m_AvailableNodes.begin());
            // This actually does not work and leads to double freeing problems.
#endif
        }

        LinkedList& operator=(LinkedList&& other) noexcept // Move assignment operator
        {
            m_Head = other.m_Head;
            m_Curr = other.m_Curr;
            m_Tail = other.m_Tail;
            m_Size = other.m_Size;
            m_FirstElement = other.m_FirstElement;

            other.m_Head = nullptr;
            other.m_Curr = nullptr;
            other.m_Tail = nullptr;
            other.m_Size = 0;
            other.m_FirstElement = false;

#ifdef _REDA_LIST_USE_Q
            m_AvailableNodes = std::move(other.m_AvailableNodes);
            //std::move(other.m_AvailableNodes.begin(), other.m_AvailableNodes.end(), m_AvailableNodes.begin());
            // This actually does not work and leads to double freeing problems.
#endif

            return *this;
        }

    // Destructor
        ~LinkedList()
        {
            shutDown();
        }

    // Append lvalue new node to the end of the list. Works in O(1) since it is appending to the tail directly
        void push_back(const T& val)
        {
#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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

#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
            return;
        }

    // Append rvalue new node to the beginning of the list.
        void push_front(T&& val) 
        {
            ASSERT(m_Head);

#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
            return;
        }

        // Adds a new node to the beginning of the list, however it creates the value object in place of its memory directly 
        // and no moving or copying is needed.
        template<typename... Args>
        void emplace_front(Args&&... args)
        {
#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
            {
                push_back((T&&)val);
                return;
            }
            else if (index == 0)
            {
                push_front((T&&)val);
                return;
            }

            m_Curr = m_Head;

            for(unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided
         
#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
            {
                push_back((T&&)val);
                return;
            }
            else if (index == 0)
            {
                push_front((T&&)val);
                return;
            }

            m_Curr = m_Head;

            for (unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided

#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
            {
                emplace_back(args...);
                return;
            }
            else if (index == 0)
            {
                emplace_front(args...);
                return;
            }

            m_Curr = m_Head;

            for (unsigned int i = 0; i < index; i++)
                m_Curr = m_Curr->next;

            // After the above loop exits i will equal the index provided

#ifdef _REDA_LIST_USE_Q
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

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
        void erase(unsigned int index)
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
            
            unsigned int i = 0;
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

    // Returns the head of a newly sorted list in ascending order if the list is of basic types, if not basic the user will need to
    // overload the comparison operator
        nodePtr sortList()
        {
            return splitNodes(&m_Head);
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
        
        void swapNodes(unsigned int index1, unsigned int index2)
        {
            doubleNodes dn = findNodes(index1, index2);

            nodePtr Node1 = dn.first;
            nodePtr Node2 = dn.second;

            swappingTheNodes(Node1, Node2);
        }

    // Swaps the Kth largest element with the Kth smallest element in the list.
        void swapKthNodes(unsigned int k)
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

    // Returns the val at the specified index in the list. (const version)
        const T& operator[](unsigned int index) const
        {
            ASSERT(index <= m_Size - 1);

            unsigned int i = 0;
            nodePtr temporary = m_Head;
            while (i < index) {
                temporary = temporary->next;
                i++;
            }
            return temporary->val;
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


    // Returns a read-write iterator pointing to the beginning of the list.
        inline Iterator begin() const 
        {
            if (m_Head) return Iterator(m_Head);
            return Iterator(nullptr);
        }

    // Returns a read-only iterator pointing to the beginning of the list.
        inline const_Iterator cbegin() const
        {
            if (m_Head) return const_Iterator(m_Head);
            return const_Iterator(nullptr);
        }

    // Returns a read-write iterator pointing to one past the end of the list.
        inline Iterator end() const
        {
            if(m_Tail) return Iterator(m_Tail->next);
            return Iterator(nullptr);
        }

    // Returns a read-only iterator pointing to one past the end of the list.
        inline const_Iterator cend() const
        {
            if (m_Tail) return const_Iterator(m_Tail->next);
            return const_Iterator(nullptr);
        }

    // For debugging
#ifdef _REDA_DEBUG
        const T& getAt(unsigned int index)
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
#ifdef _REDA_LIST_USE_Q
        void AllocateNodes(size_t nodeCount)
        {
            for (size_t i = 0; i < nodeCount; i++)
                m_AvailableNodes.push_back(new Node<T>());
        }
#endif

    // Helper function for the swapKthNodes function.
        doubleNodes findKthNodes(unsigned int k)
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
        doubleNodes findNodes(unsigned int left, unsigned int right)
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
#ifdef _REDA_LIST_USE_Q
            while (!m_AvailableNodes.empty())
            {
#ifdef _REDA_DEBUG
                std::cout << "Deleting nodes in Queue\n";
#endif

                m_Curr = m_AvailableNodes.front();
                delete m_Curr;
                m_AvailableNodes.pop_front();
            }
#endif
        }

    // Helper function for merge sorting
        nodePtr splitNodes(nodePtr* head)
        {
            // base case: 0 or 1 node
            if (!*head || !(*head)->next)
                return *head;

            // split head into `a` and `b` sublists
            nodePtr a = *head, b = nullptr;
            split(*head, &a, &b);

            // recursively sort the sublists
            splitNodes(&a);
            splitNodes(&b);

            // merge the two sorted lists
            *head = merge(a, b);

            m_Curr = *head;
            while (m_Curr->next)
                m_Curr = m_Curr->next;

            m_Tail = m_Curr;
        }

    // Helper function for merge sorting
        void split(nodePtr head, nodePtr* a, nodePtr* b)
        {
            nodePtr slow = head;
            nodePtr fast = head->next;

            // advance `fast` by two nodes, and advance `slow` by a single node
            while (fast)
            {
                fast = fast->next;
                if (fast)
                {
                    slow = slow->next;
                    fast = fast->next;
                }
            }

            *b = slow->next;
            slow->next = nullptr;
        }

    // Helper function for merge sorting
        nodePtr merge(nodePtr a, nodePtr b)
        {
            // base cases
            if (!a)
                return b;

            if (!b)
                return a;

            // pick either `a` or `b`, and recur
            if (a->val <= b->val)
            {
                a->next = merge(a->next, b);
                a->next->prev = a;
                a->prev = nullptr;
                return a;
            }
            else
            {
                b->next = merge(a, b->next);
                b->next->prev = b;
                b->prev = nullptr;
                return b;
            }
        }
    };
}

#endif