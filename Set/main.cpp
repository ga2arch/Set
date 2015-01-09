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
class BloomFilter {
    
public:
    
    void add(const T t) {
        bloom[hash(t)]++;
    }
    
    bool query(const T t) {
        return bloom[hash(t)];
    }
    
    void remove(const T t) {
        bloom[hash(t)]--;
    }
    
private:
    int hash(const T t) {
        return hashfn(t) % 64;
    }
    
    int bloom[64] = { 0 };
    std::hash<T> hashfn;
};


template <typename T>
class Set {
    
    class const_iterator;
    
    class iterator {
        //
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef T*                        pointer;
        typedef T&                        reference;
        
        iterator(const iterator &other) {
            base = other.base;
            data = other.data;
        }
        
        iterator& operator=(const iterator &other) {
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
        iterator operator++(int) {
            return iterator(base, data++);
        }
        
        // Operatore di iterazione pre-incremento
        iterator& operator++() {
            ++data;
            return *this;
        }
        
        // Operatore di iterazione post-decremento
        iterator operator--(int) {
            return iterator(base, data--);
        }
        
        // Operatore di iterazione pre-decremento
        iterator& operator--() {
            --data;
            return *this;
        }
        
        // Spostamentio in avanti della posizione
        iterator operator+(int offset) {
            return iterator(base, data+offset);
        }
        
        // Spostamentio all'indietro della posizione
        iterator operator-(int offset) {
            return iterator(base, data-offset);
        }
        
        // Spostamentio in avanti della posizione
        iterator& operator+=(int offset) {
            data += offset;
            return *this;
        }
        
        // Spostamentio all'indietro della posizione
        iterator& operator-=(int offset) {
            data -= offset;
            return *this;
        }
        
        // Numero di elementi tra due iteratori
        difference_type operator-(const iterator &other) {
            difference_type diff = data - other.data;
            return diff;
        }
        
        // Uguaglianza
        bool operator==(const iterator &other) const {
            return data == other.data;
        }
        
        // Diversita'
        bool operator!=(const iterator &other) const {
            return data != other.data;
        }
        
        // Confronto
        bool operator>(const iterator &other) const {
            return data > other.data;
        }
        
        
        bool operator>=(const iterator &other) const {
            return data >= other.data;
        }
        
        // Confronto
        bool operator<(const iterator &other) const {
            return data < other.data;
        }
        
        
        // Confronto
        bool operator<=(const iterator &other) const {
            return data <= other.data;
        }
        
        friend class const_iterator;
        
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
        pointer base;
        pointer data;
        
        friend class Set;
        
        explicit iterator(T* base_, T* data_): base(base_), data(data_) {}
        explicit iterator(T* data_): base(data_), data(data_) {}
        
    };
    
    class const_iterator {
        //
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef const T*                        pointer;
        typedef const T&                        reference;
        
        const_iterator(const iterator &other) {
            base = other.base;
            data = other.data;
        }
        
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
        
        friend class iterator;
        
        // Uguaglianza
        bool operator==(const iterator &other) const {
            return data == other.data;
        }
        
        // Diversita'
        bool operator!=(const iterator &other) const {
            return data != other.data;
        }
        
        // Confronto
        bool operator>(const iterator &other) const {
            return data > other.data;
        }
        
        
        bool operator>=(const iterator &other) const {
            return data >= other.data;
        }
        
        // Confronto
        bool operator<(const iterator &other) const {
            return data < other.data;
        }
        
        // Confronto
        bool operator<=(const iterator &other) const {
            return data <= other.data;
        }
        
    private:
        pointer base;
        pointer data;
     
        friend class Set;

        const_iterator(T* base_, T* data_): base(base_), data(data_) {}
        const_iterator(T* data_): base(data_), data(data_) {}
        
    };
    
public:
    Set() =default;
    
    /**
     Copy constructor, performs a deep copy of all the elements in the other Set
     @param Set& reference to the set to copy
     @returns class instance
     */
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
    
    /**
     Subscribe operator to access element by index in costant time
     @param p the index of the element to retrieve
     @returns T& const reference of the element
     @exception assert exception if p is outside the valide range
     */
    const T& operator[](int p) const {
        assert(p >= 0 && p <= last);
        return data[p];
    }
    
    /**
     Perform an insertion of an element into the Set, it checks first the bloom
     filter, if the query is negative, procedes with the insertion, if positive 
     it checks for the existence of the element (could be a false positive) and
     eventually performs the insertion.
     @param t the element
     @returns void
     @exception runtime_error if the elements is already present in the Set.
     */
    void insert(const T t) {
        if (bloom.query(t)) {
            for (int i=0; i <= last; i++)
                if (data[i] == t)
                    throw std::runtime_error("Error, element already present");
        }
        
        bloom.add(t);
        if (++last == size)
            grow();
        
        data[last] = t;
    }
    
    /**
     Remove and element from the Set, it checks first the bloom filter, if the 
     query is negative, throws an exception, if positive, search the element.
     If the element is found, use std::rotate to perform a left rotation, effectively 
     moving the element to remove at the end of the Set but outside the valid range.
     @param t the element
     @returns void
     @exception runtime_error if the element is not found in the Set.
     */
    void remove(const T t) {
        if (!bloom.query(t))
            throw std::runtime_error("Element not found");

        for (int i=0; i <= last; i++) {
            if (data[i] == t) {
                
                bloom.remove(t);
                std::rotate(begin()+i, begin()+i+1, end());
                
                if (--last < size/2)
                    shrink();
                
                return;
            }
        }
        
        throw std::runtime_error("Element not found");
    }
    
    /**
     Return the const_iterator, pointing at the first element
     @returns const_iterator the iterator
     */
    const_iterator begin() const {
        return const_iterator(data);
    }
    
    /**
     Return the const_iterator, pointing at the last element
     @returns const_iterator the iterator
     */
    const_iterator end() const {
        return const_iterator(data, data+last+1);
    }
    
    /**
     Return the iterator, pointing at the first element
     @returns iterator the iterator
     */
    iterator begin() {
        return iterator(data);
    }
    
    /**
     Return the iterator, pointing at the last element
     @returns iterator the iterator
     */
    iterator end() {
        return iterator(data, data+last+1);
    }

private:
    /**
     Grows the size of the buffer exponetially
     @exception bad_alloc if the allocation isn't successfull
     */
    void grow() {
        if (size) size *= 1.5;
        else size = 1;
        
        alloc(size);
    }
    
    /**
     Shrink the size of the buffer exponetially
     @exception bad_alloc if the allocation isn't successfull
     */
    void shrink() {
        size /= 1.5;
        alloc(size);
    }
    
    /**
     Realloc a chunk of memory based on the size passed.
     @param s
     @returns void
     @exception bad_alloc if the allocation isn't successfull
     */
    void alloc(size_t s) {
        auto mem = std::realloc(data, s*sizeof(T));
        if (!mem)
            throw std::bad_alloc();
        
        data = static_cast<T*>(mem);
    }
    
    /**
     Comodity function to easily display the contento of a Set.
     */
    friend std::ostream& operator<<(std::ostream &os, const Set &set) {
        for (auto e: set) {
            os << e << " ";
        }
        
        return os;
    }
    
    BloomFilter<T> bloom;
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
