#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T value;
        node *prev;
        node *next;

        node(const T &val) : value(val), prev(nullptr), next(nullptr) {}
        node(T &&val) : value(std::move(val)), prev(nullptr), next(nullptr) {}
        node() : prev(nullptr), next(nullptr) {} // For sentinel
    };

protected:
    node *head; // Sentinel node
    size_t _size;

public:
    class iterator {
    private:
        node *ptr;
        list *list_ptr;
    public:
        iterator(node *p = nullptr, list *l = nullptr) : ptr(p), list_ptr(l) {}
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator & operator++() {
            if (ptr == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        iterator & operator--() {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        T & operator *() const {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            return ptr->value;
        }
        T * operator ->() const {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            return &(ptr->value);
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        friend class list;
    };

    class const_iterator {
    private:
        node *ptr;
        list *list_ptr;
    public:
        const_iterator(node *p = nullptr, list *l = nullptr) : ptr(p), list_ptr(l) {}
        const_iterator(const iterator &it) : ptr(it.ptr), list_ptr(it.list_ptr) {}

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        const_iterator & operator++() {
            if (ptr == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        const_iterator & operator--() {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        const T & operator *() const {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            return ptr->value;
        }
        const T * operator ->() const {
            if (ptr == nullptr || ptr == list_ptr->head) throw invalid_iterator();
            return &(ptr->value);
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        friend class list;
    };

    list() : _size(0) {
        head = new node();
        head->next = head;
        head->prev = head;
    }

    list(const list &other) : _size(0) {
        head = new node();
        head->next = head;
        head->prev = head;
        for (auto it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    virtual ~list() {
        clear();
        delete head;
    }

    list &operator=(const list &other) {
        if (this != &other) {
            clear();
            for (auto it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }

    const T & front() const {
        if (empty()) throw container_is_empty();
        return head->next->value;
    }
    const T & back() const {
        if (empty()) throw container_is_empty();
        return head->prev->value;
    }

    iterator begin() {
        return iterator(head->next, this);
    }
    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }
    iterator end() {
        return iterator(head, this);
    }
    const_iterator cend() const {
        return const_iterator(head, this);
    }

    virtual bool empty() const {
        return _size == 0;
    }
    virtual size_t size() const {
        return _size;
    }

    virtual void clear() {
        while (!empty()) {
            pop_front();
        }
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.list_ptr != this) throw invalid_iterator();
        node *p = pos.ptr;
        if (p == nullptr) throw invalid_iterator();
        
        node *newNode = new node(value);
        node *prev_node = p->prev;
        
        newNode->next = p;
        newNode->prev = prev_node;
        prev_node->next = newNode;
        p->prev = newNode;
        
        _size++;
        return iterator(newNode, this);
    }

    virtual iterator erase(iterator pos) {
        if (pos.list_ptr != this) throw invalid_iterator();
        if (pos.ptr == nullptr || pos.ptr == head) throw invalid_iterator();

        node *p = pos.ptr;
        node *prev_node = p->prev;
        node *next_node = p->next;

        prev_node->next = next_node;
        next_node->prev = prev_node;

        delete p;
        _size--;
        return iterator(next_node, this);
    }

    void push_back(const T &value) {
        node *newNode = new node(value);
        node *last = head->prev;
        newNode->next = head;
        newNode->prev = last;
        last->next = newNode;
        head->prev = newNode;
        _size++;
    }

    void pop_back() {
        if (empty()) throw container_is_empty();
        node *last = head->prev;
        node *prev_node = last->prev;
        prev_node->next = head;
        head->prev = prev_node;
        delete last;
        _size--;
    }

    void push_front(const T &value) {
        node *newNode = new node(value);
        node *first = head->next;
        newNode->next = first;
        newNode->prev = head;
        head->next = newNode;
        first->prev = newNode;
        _size++;
    }

    void pop_front() {
        if (empty()) throw container_is_empty();
        node *first = head->next;
        node *next_node = first->next;
        head->next = next_node;
        next_node->prev = head;
        delete first;
        _size--;
    }

    void sort() {
        if (_size <= 1) return;
        
        T* arr = new T[_size];
        size_t idx = 0;
        for (auto it = begin(); it != end(); ++it) {
            arr[idx++] = *it;
        }

        sjtu::sort<T*>(arr, arr + _size, [](const T& a, const T& b) {
            return a < b;
        });

        clear();
        for (size_t i = 0; i < _size; ++i) {
            push_back(arr[i]);
        }
        delete[] arr;
    }

    void merge(list &other) {
        if (other.empty()) return;
        if (empty()) {
            node *o_first = other.head->next;
            node *o_last = other.head->prev;
            
            head->next = o_first;
            o_first->prev = head;
            head->prev = o_last;
            o_last->next = head;
            
            _size = other._size;
            other.head->next = other.head;
            other.head->prev = other.head;
            other._size = 0;
            return;
        }

        node *this_curr = head->next;
        node *other_curr = other.head->next;

        while (other_curr != other.head) {
            if (this_curr == head || !(other_curr->value < this_curr->value)) {
                if (this_curr != head) this_curr = this_curr->next;
                if (this_curr == head) break;
            } else {
                node *next_other = other_curr->next;
                node *prev_node = this_curr->prev;
                
                other_curr->next = this_curr;
                other_curr->prev = prev_node;
                prev_node->next = other_curr;
                this_curr->prev = other_curr;
                
                _size++;
                other_curr = next_other;
            }
        }
        
        if (other_curr != other.head) {
            node *this_last = head->prev;
            node *other_first = other_curr;
            node *other_last = other.head->prev;
            
            this_last->next = other_first;
            other_first->prev = this_last;
            other_last->next = head;
            head->prev = other_last;
            
            size_t count = 0;
            node *tmp = other_first;
            while (tmp != other.head) {
                count++;
                tmp = tmp->next;
            }
            _size += count;
        }
        
        other.head->next = other.head;
        other.head->prev = other.head;
        other._size = 0;
    }

    void reverse() {
        if (_size <= 1) return;
        node *curr = head->next;
        while (curr != head) {
            node *tmp = curr->next;
            curr->next = curr->prev;
            curr->prev = tmp;
            curr = tmp;
        }
        node *old_first = head->next;
        node *old_last = head->prev;
        head->next = old_last;
        head->prev = old_first;
        head->next->prev = head;
        head->prev->next = head;
    }

    void unique() {
        if (_size <= 1) return;
        node *curr = head->next;
        while (curr != head && curr->next != head) {
            if (curr->value == curr->next->value) {
                node *duplicate = curr->next;
                curr->next = duplicate->next;
                duplicate->next->prev = curr;
                delete duplicate;
                _size--;
            } else {
                curr = curr->next;
            }
        }
    }
};
}

#endif //SJTU_LIST_HPP
