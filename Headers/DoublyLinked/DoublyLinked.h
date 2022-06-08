#pragma once

/*
 * SO, the basic way this stucture works is that when an object is first constructed it calls an allocate functions that 
 * constructs and store a certain amount of hollow nodes in a Dequeue, and whenever a push or insert function is called, one 
 * node is popped from the queue and is connected to the node, when the queue is empty is reallocates another amount of nodes
 * and stores it.
 * TO DO: Make it that when a pop or erase function is called, the popped node is hollowed and readded to the queue.
 */

#include <iostream>
#include <memory>
#include <concepts>
#include <deque>

#ifdef DEBUG_ASSERTS

#define ASSERT(x) { if(!(x)) { std::cout << "Cannot push_front if no Head exists" << std::endl; __debugbreak(); } }

#else

#define ASSERT(x)

#endif


namespace reda{

    namespace _R_Concepts{   
        
        template<typename T>
        concept reStraint = std::integral<T> or std::floating_point<T>;
    }

    template<typename T>
    struct Node {
        T val;
        Node* next;
        Node* prev;
        Node() {}

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
        //size_t m_Capacity; This is not needed since we are keeping the deque

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

    // This is temporary just to access and print things to the console to ease the debugging
#if DEBUG
        const T& getAt(const T& val)
        {
            m_Curr = m_Head;
            while (m_Curr) {
                if (m_Curr->val == val)
                    return m_Curr->val;
                m_Curr = m_Curr->next;
            }
            std::cout << "Not found\n";
            return T();
        }
#endif

    //Append new node to the end of the list. Works in O(1) since it is appending to the tail directly
        void push_back(const T& val){
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val) T(val);

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

        void push_back(T&& val) {
            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val)  T(std::move(val));

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

    //Append new node to the beginning of the list.
        void push_front(const T& val){
            ASSERT(m_Head);

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);
            
            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)& newNode->val) T(val);

            m_Head->prev = newNode;
            newNode->next = m_Head;
            m_Head = m_Head->prev;
            m_Size++;
        }

        void push_front(T&& val) {
            ASSERT(m_Head);

            if (m_AvailableNodes.size() == 0)
                AllocateNodes(5);

            nodePtr newNode = m_AvailableNodes.front();
            m_AvailableNodes.pop_front();
            new((void*)&newNode->val)  T(std::move(val));

            m_Head->prev = newNode;
            newNode->next = m_Head;
            m_Head = m_Head->prev;
            m_Size++;
        }

    //Inserts element in the list at the specified position. needs refactoring
        //void insert(const T& val){
        //    curr = m_Head;

        //    while(curr != ){
        //        curr = curr->next;
        //    }

        //    nodePtr new_node = std::make_shared<Node>(val, curr, nullptr);
        //    new_node->prev = curr->prev;
        //    curr->prev->next = new_node;
        //    curr->prev = new_node;
        //    length++;
        //}

    //Removes the last element in the list.
        //void pop_back(){
        //    if (m_Size > 0) {
        //        m_Tail = m_Tail->prev;
        //        m_Tail->next->val.~T();
        //        //::operator delete(m_Tail->next);
        //    }
        //}

    //Removes the first element in the list.
        //void pop_front(){
        //    m_Head = m_Head->next;
        //    m_Head->prev.reset();
        //    length--;
        //}

    //Remove a single element from the list identified by index. needs refactoring
        //void erase(){
        //    curr = m_Head;
        //    
        //    if(){
        //        m_Head = m_Head->next;
        //        curr->next->prev.reset();
        //        length--;
        //        return;
        //    }
        //    if(){
        //        m_Tail = m_Tail->prev;
        //        m_Tail->next->prev.reset();
        //        m_Tail->next.reset();
        //        length--;
        //        return;
        //    }
        //    
        //    while(){
        //        curr = curr->next;
        //    }

        //    nodePtr temp = curr->prev;
        //    temp->next = curr->next;
        //    curr->next->prev = temp;
        //    length--;
        //}

    //Removes all instances of passed value found in list. If passed value is a pointer it is not 
    //destroyed, that has to be done manually.
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

        #if 0
    //Reverses the list.
        void reverseList(){
            curr = m_Head;
            nodePtr temp = nullptr;
            //swapping.
            while(curr){
                temp = curr->prev;
                curr->prev = curr->next;
                curr->next = temp;
                curr = curr->prev;
            }
            //resetting m_Tail.
            curr = m_Head;
            while(curr->next){
                curr = curr->next;
            }
            m_Tail = curr;

            if(temp != nullptr){
                m_Head = temp->prev;
            }
        }

    //Swaps the Kth largest element with the Kth smallest element in the list.
        void swapNodes(const int& k){
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
    //Returns the val at the specified index in the list. needs refactoring once i make iterators
        //T& operator [] (){
        //    curr = m_Head;
        //    while(){
        //        curr = curr->next;
        //    }
        //    return curr->val;
        //}

    //Returns true if the List is empty, otherwise false.
        bool isEmpty() const{
            if(!m_Head) return true;
            return false;
        }

    //Returns the size of the list.
        const size_t size() const{
            return m_Size;
        }

    //Returns the value of the first element in the list.
        const T& front() const{
            return m_Head->val;
        }

    //Returns the value of the last element in the list.
        const T& back() const{
            return m_Tail->val;
        }

    //Returns a pointer pointing to the beginning of the list.
        nodePtr begin() const {
            if(m_Head) return m_Head;
            return nullptr;
        }

    //Returns a pointer pointing to the end of the list.
        nodePtr end() const {
            return m_Tail;
        }

    // Needs refactoring
    //Destructor. After so many failing tries trying to reset the nodes themselves which did not call the destructor of the Node struct
    //I figured out that the pointers to reset are the links: next AND prev, since they are what forms the list, once they are reset the 
    //user count for each new_node is going to be 1 and then it will call reset one more time by itself thus destroying itself since there 
    //is no more pointers sharing the ownership. 
        //~LinkedList(){
        //    curr = m_Head;
        //    int i = 0;
        //    while(curr){
        //        nodePtr temp = curr;
        //        curr = curr->next;
        //        temp->prev.reset();
        //        temp->next.reset();
        //    }
        //    std::cout << "Object Destroyed at: " << this->name << std::endl;
        //}

    private:
        void AllocateNodes(size_t nodeCount)
        {
            for (size_t i = 0; i < nodeCount; i++)
                m_AvailableNodes.push_back(new Node<T>());
        }
    };
}