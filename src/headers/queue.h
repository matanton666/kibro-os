#pragma once

#include "std.h"

// queue class implemented with linked list
// T must have a next pointer by the name of `next`
template <typename T>
class Queue
{
private:
    T* _start = nullptr;
    unsigned int _size = 0;


public:

    void push(T* item)
    {
        if (_start == nullptr) { // first item
            _start = item;
            _start->next = nullptr;
            _size = 1;
        }
        else { // append to end
            peekEnd()->next = item;
            item->next = nullptr;
            _size++;
        }
    }

    T* pop()
    {
        if (_start == nullptr) {
            return nullptr;
        }
        else {
            T* item = _start;
            _start = _start->next;
            _size--;
            return item;
        }
    }

    T* peek()
    {
        return _start;
    }

    T* peekEnd()
    {
        if (_start == nullptr) {
            return nullptr;
        }
        else {
            T* curr = _start;
            while (curr->next != nullptr) {
                curr = curr->next;
            }
            return curr;
        }
    }

    unsigned int size()
    {
        return _size;
    }

    bool isEmpty()
    {
        return _size == 0;
    }

    void clear()
    {
        // TODO: free memory
        _start = nullptr;
        _size = 0;
    }

    void remove(T* item)
    {
        if (_start == nullptr || item == nullptr) {
            return;
        }
        else if (_start == item) { // first item
            _start = _start->next;
            _size--;
            // TODO: free memory
        }
        else { // find item
            T* prev = _start;
            T* curr = _start->next;
            while (curr != nullptr) {
                if (curr == item) {
                    prev->next = curr->next;
                    _size--;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
        }
    }


    void rotate()
    {
        if (_start == nullptr || _start->next == nullptr) { // nothing to rotate
            return;
        }
        else {
            T* item = _start;
            _start = _start->next;
            item->next = nullptr;
            peekEnd()->next = item;
        }
    }

};