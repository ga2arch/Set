//
//  main.cpp
//  Set
//
//  Created by Gabriele Carrettoni on 08/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cassert>
#include <cstddef>
#include <stdexcept>

template <typename T>
class Set {
    
    class const_iterator {
        //
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef const T*                        pointer;
        typedef const T&                        reference;
        
        const_iterator(const const_iterator &other) {
            base = other.base;
            data = other.data;
        }
        
        const_iterator& operator=(const const_iterator &other) {
            base = other.base;
            data = other.data;
            return *this;
        }
        
        // Ritorna il dato riferito dall'iteratore (dereferenziamento)
        reference operator*() const {
            return *data;
        }
        
        // Ritorna il puntatore al dato riferito dall'iteratore
        pointer operator->() const {
            return data;
        }
        
        // Operatore di accesso random
        reference operator[](int index) {
            return &*base[index];
        }
        
        // Operatore di iterazione post-incremento
        const_iterator operator++(int) {
            return const_iterator(base, data++);
        }
        
        // Operatore di iterazione pre-incremento
        const_iterator& operator++() {
            ++data;
            return *this;
        }
        
        // Operatore di iterazione post-decremento
        const_iterator operator--(int) {
            return const_iterator(base, data--);
        }
        
        // Operatore di iterazione pre-decremento
        const_iterator& operator--() {
            --data;
            return *this;
        }
        
        // Spostamentio in avanti della posizione
        const_iterator operator+(int offset) {
            return const_iterator(base, data+offset);
        }
        
        // Spostamentio all'indietro della posizione
        const_iterator operator-(int offset) {
            return const_iterator(base, data-offset);
        }
        
        // Spostamentio in avanti della posizione
        const_iterator& operator+=(int offset) {
            data += offset;
            return *this;
        }
        
        // Spostamentio all'indietro della posizione
        const_iterator& operator-=(int offset) {
            data -= offset;
            return *this;
        }
        
        // Numero di elementi tra due iteratori
        difference_type operator-(const const_iterator &other) {
            difference_type diff = data - other.data;
            return diff;
        }
        
        // Uguaglianza
        bool operator==(const const_iterator &other) const {
            return data == other.data;
        }
        
        // Diversita'
        bool operator!=(const const_iterator &other) const {
            return data != other.data;
        }
        
        // Confronto
        bool operator>(const const_iterator &other) const {
            return data > other.data;
        }
        
        
        bool operator>=(const const_iterator &other) const {
            return data >= other.data;
        }
        
        // Confronto
        bool operator<(const const_iterator &other) const {
            return data < other.data;
        }
        
        
        // Confronto
        bool operator<=(const const_iterator &other) const {
            return data <= other.data;
        }
        
    private:
        T* base;
        T* data;
        
        friend class Set;

        const_iterator(T* base_, T* data_): base(base_), data(data_) {}
        const_iterator(T* data_): base(data_), data(data_) {}
        
    };
    
public:
    Set() =default;
    Set(const Set& set_) {
        for (auto e: set_)
            insert(e);
    }
    
    /**
     Constructor from generic iterators
     
     @param begin first element of the iterator
     @param end last element of the iterator
     @returns class instance
     */
    template <typename Iterator>
    Set(Iterator begin, Iterator end) {
        for (;begin != end; begin++)
            insert(*begin);
    }
    
    const T& operator[](int p) const {
        assert(p >= 0 && p <= last);
        return data[p];
    }
    
    void insert(const T t) {
        for (int i=0; i <= last; i++)
            if (data[i] == t)
                throw std::runtime_error("Error, element already present");

        if (++last == size)
            grow();
        
        data[last] = t;
    }
    
    void remove(const T t) {
        for (int i=0; i <= last; i++) {
            if (data[i] == t) {
                
                for (; i < last; i++)
                    data[i] = std::move(data[i+1]);
                
                if (--last < size/2)
                    shrink();
                
                return;
            }
        }
        
        throw std::runtime_error("Element not found");
    }
    
    const_iterator begin() const {
        return const_iterator(data);
    }
    
    const_iterator end() const {
        return const_iterator(data, data+last+1);
    }
    
private:
    void grow() {
        if (size) size *= 1.5;
        else size = 1;
        
        alloc(size);
    }
    
    void shrink() {
        size /= 2;
        alloc(size);
    }
    
    void alloc(size_t s) {
        auto mem = std::realloc(data, s*sizeof(T));
        if (!mem)
            throw std::bad_alloc();
        
        data = static_cast<T*>(mem);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Set &set) {
        for (auto e: set) {
            os << e << " ";
        }
        
        return os;
    }
    
    T* data = nullptr;
    int last = -1;
    size_t size = 0;
    
};

template <typename T, typename F>
Set<T> filter_out(const Set<T>& s, F p) {
    Set<T> n;
    
    for (auto e: s) {
        if (!p(e))
            n.insert(e);
    }
    
    return n;
}

int main(int argc, const char * argv[]) {
    Set<int> s;
    std::vector<int> l{4,5};
    
    s.insert(4);
    
    assert(s[0] == 4);
    
    s.insert(3);
    s.insert(5);

    s.remove(3);
    
    assert(s[1] == 5);
    
    for (int i=0; i < 2; i++) {
        assert(l[i] == s[i]);
    }
    
    auto n = filter_out(s, [](int t){ return t == 4; });
    
    std::cout << s;
    
}
