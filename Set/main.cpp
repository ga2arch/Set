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
            data = other.data;
            pos = other.pos;
        }
        
        const_iterator& operator=(const const_iterator &other) {
            data = other.data;
            pos = other.pos;
            return *this;
        }
        
        // Ritorna il dato riferito dall'iteratore (dereferenziamento)
        reference operator*() const {
            return data[pos];
        }
        
        // Ritorna il puntatore al dato riferito dall'iteratore
        pointer operator->() const {
            return data+pos;
        }
        
        // Operatore di accesso random
        reference operator[](int index) {
            return &*data[index];
        }
        
        // Operatore di iterazione post-incremento
        const_iterator& operator++(int) {
            const_iterator save = *this;
            ++pos;
            return save;
        }
        
        // Operatore di iterazione pre-incremento
        const_iterator operator++() {
            ++pos;
            return const_iterator(data, pos);
        }
        
        // Operatore di iterazione post-decremento
        const_iterator& operator--(int) {
            const_iterator save = *this;
            --pos;
            return save;
        }
        
        // Operatore di iterazione pre-decremento
        const_iterator operator--() {
            --pos;
            return const_iterator(data, pos);
        }
        
        // Spostamentio in avanti della posizione
        const_iterator operator+(int offset) {
            return const_iterator(data, pos+offset);
        }
        
        // Spostamentio all'indietro della posizione
        const_iterator operator-(int offset) {
            return const_iterator(data, pos-offset);
        }
        
        // Spostamentio in avanti della posizione
        const_iterator& operator+=(int offset) {
            pos += offset;
            return *this;
        }
        
        // Spostamentio all'indietro della posizione
        const_iterator& operator-=(int offset) {
            pos -= offset;
            return *this;
        }
        
        // Numero di elementi tra due iteratori
        difference_type operator-(const const_iterator &other) {
            difference_type diff = data - other.data;
            return diff;
        }
        
        // Uguaglianza
        bool operator==(const const_iterator &other) const {
            return data == other.data && pos == other.pos;
        }
        
        // Diversita'
        bool operator!=(const const_iterator &other) const {
            return data != other.data || pos != other.pos;
        }
        
        // Confronto
        bool operator>(const const_iterator &other) const {
            return data > other.data && pos > other.pos;
        }
        
        
        bool operator>=(const const_iterator &other) const {
            return data >= other.data && pos >= other.pos;
        }
        
        // Confronto
        bool operator<(const const_iterator &other) const {
            return data < other.data && pos < other.pos;
        }
        
        
        // Confronto
        bool operator<=(const const_iterator &other) const {
            return data <= other.data && pos <= other.pos;
        }
        
    private:
        T* data;
        int pos = 0;
        
        friend class Set;

        const_iterator(T* data_, int pos_=0): data(data_), pos(pos_) {}
        
    };
    
public:
    Set() =default;
    
    template <typename Iterator>
    Set(Iterator begin, Iterator end) {
        for (;begin != end; begin++)
            insert(*begin);
    }
    
    ~Set() {
        std::free(data);
        data = nullptr;
    }
    
    T& operator[](int p) {
        assert(p >= 0 && p <= count);
        return data[p];
    }
    
    void insert(const T t) {
        for (int i=0; i <= count; i++)
            if (data[i] == t)
                throw std::runtime_error("Error, element already present");

        resize(++count);
        
        data[count] = t;
    }
    
    void remove(const T t) {
        for (int i=0; i <= count; i++) {
            if (data[i] == t) {
                
                for (; i < count; i++)
                    data[i] = std::move(data[i+1]);
                
                resize(--count);

                return;
            }
        }
        
        throw std::runtime_error("Element not found");
    }
    
    const_iterator begin() const {
        return const_iterator(data);
    }
    
    const_iterator end() const {
        return const_iterator(data, count+1);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Set &set) {
        for (auto e: set) {
            os << e << " ";
        }
        
        return os;
    }
    
private:
    void resize(int size) {
        assert(size >= 0);
        auto mem = std::realloc(data, (size+1)*sizeof(T));
        if (!mem)
            throw std::bad_alloc();
        
        data = static_cast<T*>(mem);
    }
    
    T* data = nullptr;
    int count = -1;
    
};

int main(int argc, const char * argv[]) {
    Set<int> s;
    
    std::vector<int> l{4,3,5};
    
    s.insert(4);
    s.insert(3);
    s.insert(5);
    
//    Set<int> s(l.begin(), l.end());
    
    s.remove(3);
    s[10];
    for (auto e: s)
        std::cout << e << std::endl;
}
