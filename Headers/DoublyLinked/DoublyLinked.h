#pragma once

#include <iostream>
#include <memory>
#include <concepts>


namespace reda{

    namespace _R_Concepts{   
        
        template<typename T>
        concept reStraint = std::integral<T> or std::floating_point<T>;
    }

    template<typename T>
    class LinkedList{
    private:
        struct Node{
            T val;
            size_t index;
            std::shared_ptr<Node> next;
            std::shared_ptr<Node> prev;
            Node() : val(0), index(0), next(nullptr), prev(nullptr) {}

            Node(T val) : val(val), index(0), next(nullptr), prev(nullptr) {}

            Node(T val, size_t index, std::shared_ptr<Node> next, std::shared_ptr<Node> prev) : val(val), index(index), next(next), prev(prev) {}

            ~Node(){
                std::cout << "Node being destroyed at index: " << this->index << std::endl;
            }
        };

        typedef std::shared_ptr<Node> nodeSharPtr;

        std::string name;
        nodeSharPtr curr;
        nodeSharPtr head;
        nodeSharPtr tail;
        size_t length = 0;

    //Helper function for updating the indices each time insertion or deletion happens;
        void updateIndices(nodeSharPtr current){
            if(current == head){
                size_t i = 0;
                curr = head;
                while(curr){
                    curr->index = i;
                    curr = curr->next;
                    i++;
                }     
            }
            else{
                curr = current;
                size_t i = (curr->prev->index) + 1;
                while(curr){
                    curr->index = i;
                    curr = curr->next;
                    i++;
                }
            }
        }

    public:
        LinkedList() = default;

        LinkedList(std::string name) : name(name), head(nullptr), curr(nullptr), tail(nullptr) {}

    //Append new node to the end of the list.
        void push_back(const T& val, nodeSharPtr next = nullptr, nodeSharPtr prev = nullptr){
            if(head){
                curr = head;

                while(curr->next){
                    curr = curr->next;
                }
                nodeSharPtr new_node = std::make_shared<Node>(val);
                curr->next = new_node;
                new_node->prev = curr;
                tail = new_node;
                tail->index = (tail->prev->index) + 1;
                //updateIndices();
                //Lambda function to update indices.
                /*[&](){
                    int i = 0;
                    curr = head;
                    while(curr){
                        curr->index = i;
                        curr = curr->next;
                        i++;
                    }
                }();
                */
            }
            else{
                nodeSharPtr new_node = std::make_shared<Node>(val);
                head = new_node;
            }
            length++;
        }

    //Append new node to the beginning of the list.
        void push_front(const T& val){
            nodeSharPtr new_node = std::make_shared<Node>(val, 0, head, nullptr);
            curr = new_node->next;
            curr->prev = new_node;

            head = new_node; 

            updateIndices(head);
            length++;       
        }

    //Inserts element in the list at the specified position.
        void insert(const T& val, const size_t& idx){
            curr = head;

            while(curr->index != idx){
                curr = curr->next;
            }

            nodeSharPtr new_node = std::make_shared<Node>(val, idx, curr, nullptr);
            new_node->prev = curr->prev;
            curr->prev->next = new_node;
            curr->prev = new_node;

            updateIndices(new_node);
            length++;
        }

    //Removes the last element in the list.
        void pop_back(){
            tail = tail->prev;
            tail->next.reset();
            length--;
            //updateIndices();
        }

    //Removes the first element in the list.
        void pop_front(){
            head = head->next;
            head->prev.reset();
            length--;
            updateIndices(head);
        }

    //Remove a single element from the list identified by index.
        void erase(const size_t& idx){
            curr = head;
            
            if(idx == 0){
                head = head->next;
                curr->next->prev.reset();
                length--;
                updateIndices(head);
                return;
            }
            if(idx == tail->index){
                tail = tail->prev;
                tail->next->prev.reset();
                tail->next.reset();
                length--;
                //updateIndices();
                return;
            }
            
            while(curr->index != idx){
                curr = curr->next;
            }

            nodeSharPtr temp = curr->prev;
            temp->next = curr->next;
            curr->next->prev = temp;
            updateIndices(head);
            length--;
        }

    //Removes all instances of passed value found in list. If passed value is a pointer it is not 
    //destroyed, that has to be done manually.
        void remove(const T& data){
            curr = head;
            
            while(curr){
                if(curr->val == data and curr == head){
                    head = head->next;
                    curr->next->prev.reset();
                    length--;
                }
                else if(curr->val == data and curr == tail){
                    tail = tail->prev;
                    curr->prev->next.reset();
                    curr->prev.reset();
                    length--;
                }
                else if(curr->val == data){
                    nodeSharPtr temp = curr->prev;
                    temp->next = curr->next;
                    curr->next->prev = temp;
                    length--;
                }
                curr = curr->next;
            }
            updateIndices(head);
        }

    //Reverses the list.
        void reverseList(){
            curr = head;
            nodeSharPtr temp = nullptr;
            //swapping.
            while(curr){
                temp = curr->prev;
                curr->prev = curr->next;
                curr->next = temp;
                curr = curr->prev;
            }
            //resetting tail.
            curr = head;
            while(curr->next){
                curr = curr->next;
            }
            tail = curr;

            if(temp != nullptr){
                head = temp->prev;
            }
            updateIndices(head);
        }

    //Swaps the Kth largest element with the Kth smallest element in the list.
        void swapNodes(const int& k){
            if(k == 0) return;

            curr = head;
            nodeSharPtr left = head, right = head;
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

    //Returns the val at the specified index in the list.
        T& operator [] (size_t idx){
            curr = head;
            while(curr->index != idx){
                curr = curr->next;
            }
            return curr->val;
        }

    //List output function ONLY FOR BASIC DATATYPES. can not be used with structs and classes, build external output function for that.
        void output() requires _R_Concepts::reStraint<T>{
            curr = head;

            while(curr){
                std::cout << curr->val << " ";
                curr = curr->next;
            }
            std::cout << std::endl;
        }

    //Returns true if the List is empty, otherwise false.
        bool isEmpty() const{
            if(not head) return true;
            return false;
        }

    //Returns the size of the list.
        const size_t size() const{
            return length;
        }

    //Returns the value of the first element in the list.
        const T& front() const{
            return head->val;
        }

    //Returns the value of the last element in the list.
        const T& back() const{
            return tail->val;
        }

    //Returns a pointer pointing to the beginning of the list.
        nodeSharPtr begin() const {
            if(head) return head;
            return nullptr;
        }

    //Returns a pointer pointing to the end of the list.
        nodeSharPtr end() const {
            return tail;
        }

    //Destructor. After so many failing tries trying to reset the nodes themselves which did not call the destructor of the Node struct
    //I figured out that the pointers to reset are the links: next AND prev, since they are what forms the list, once they are reset the 
    //user count for each new_node is going to be 1 and then it will call reset one more time by itself thus destroying itself since there 
    //is no more pointers sharing the ownership. 
        ~LinkedList(){
            curr = head;
            int i = 0;
            while(curr){
                nodeSharPtr temp = curr;
                curr = curr->next;
                temp->prev.reset();
                temp->next.reset();
            }
            std::cout << "Object Destroyed at: " << this->name << std::endl;
        }
    };
}