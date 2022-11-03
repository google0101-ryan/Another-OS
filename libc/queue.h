#pragma once

#include <stddef.h>

namespace std
{

template<typename T>
struct queue_node
{
	queue_node(const T& e) : data(e), next(0), prev(0) {}
	T data;
	queue_node<T>* next;
	queue_node<T>* prev;
};

template <typename T>
    class queue
    {
    public:
        queue() : head_(0), tail_(0), size_(0)
        {   }
        ~queue()
        { this->Clear(); /*Remove all the items from the queue*/ }
        int size() { return size_; }
        void push_back(const T &e);
        void push_front(const T &e);
        void Clear();
        T GetAt(int index);
        T operator[](int index);
        void Remove(int index);
        void Remove(const T &e);
		int IndexOf(const T &e);

        void operator+=(const T &e);
        void operator-=(const T &e);
    private:
        queue_node<T>* head_;
        queue_node<T>* tail_;
        int size_;
        queue_node<T>* insertInternal(const T &e, queue_node<T>* pos);
        void removeInternal(queue_node<T> *pos);
    //Iterators
    public:
        class iterator
        {
        public:
            iterator(queue_node<T> *p=0) : pos_(p) { }
            
            T &operator*()
            { return pos_->data; }

            T *operator->()
            { return &(pos_->data); }

            bool operator!=(const iterator &rhs)
            { return this->pos_ != rhs.pos_; }

            iterator operator++()
            { pos_ = pos_->next; return *this; }

            iterator operator--()
            { pos_ = pos_->prev; return *this; }

        private:
            queue_node<T> *pos_;
        };
        iterator begin()
        {
            return iterator(head_);
        }
        iterator end()
        {
            return iterator(0);
        }
    };
}

using namespace std;

/////////////
// Implementations
////////////
template <typename T>
queue_node<T>* queue<T>::insertInternal(const T &e, queue_node<T>* pos)
{
    queue_node<T> *n = new queue_node<T>(e);
    size_++;
    // no operation below this should throw
    // as state of the queue has changed and memory allocated
    n->next = pos;
    if(pos)
    {
        n->prev = pos->prev;
        pos->prev = n;
    }
    else
    {
        // pos is null that is at the very end of the queue
        n->prev = tail_;
        tail_ = n;
    }
    if(n->prev)
    {
        n->prev->next = n;
    }
    else
    {
        // at the very begining of the queue
        head_ = n;
    }
    return n;
}
template <typename T>
void std::queue<T>::push_back(const T &e)
{
    // inserts before the position, 
    // 0 is the end() iterator
    // hence insert at the end
    insertInternal(e, 0);
}
template <typename T>
void queue<T>::push_front(const T &e)
{
    // insert before the head
    insertInternal(e, head_);
}

template <typename T>
void queue<T>::removeInternal(queue_node<T> *pos)
{
	if(pos)
	{
		if(pos->prev)
			pos->prev->next = pos->next;
		if(pos->next)
			pos->next->prev = pos->prev;
		if(pos == head_)
			head_ = pos->next;
		if(pos == tail_)
			tail_ = pos->prev;
		delete pos;
		size_--;
	}
}

template <typename T>
void queue<T>::Remove(int index)
{
    queue_node<T>* cur = head_;
    for(int i = 0; i < index; ++i)
        cur = cur->next;
    removeInternal(cur);
}

template <typename T>
void queue<T>::Remove(const T &e)
{
    for(int i = 0; i < size_; i++)
        if(GetAt(i) == e)
            Remove(i);
}

template<typename T>
int queue<T>::IndexOf(const T& e)
{
	for(int i = 0; i < size_; i++)
        if(GetAt(i) == e)
            return i;
    return -1;
}

template <typename T>
void queue<T>::Clear()
{
    queue_node<T>* current( head_ );

    while(current)
    {
        queue_node<T>* next( current->next );
        delete current;
        current = next;
    }
    size_ = 0; //Reset the size to 0
    head_ = 0;
    tail_ = 0;
}

template <typename T>
T queue<T>::GetAt(int index)
{
    queue_node<T>* cur = head_;
    for(int i = 0; i < index; ++i)
        cur = cur->next;
    return cur->data;
}

template <typename T>
T queue<T>::operator[](int index)
{
    return GetAt(index);
}

template <typename T>
void queue<T>::operator+=(const T &e)
{
    push_back(e);
}

template <typename T>
void queue<T>::operator-=(const T &e)
{
    Remove(e);
}