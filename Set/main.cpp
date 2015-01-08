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
        return data[p];
    }
    
    void insert(const T t) {
        for (int i=0; i <= count; i++)
            if (data[i] == t)
                throw std::runtime_error("Error, element already present");

        resize(++count*sizeof(T));
        
        data[count] = t;
    }
    
    void remove(const T t) {
        for (int i=0; i <= count; i++) {
            if (data[i] == t) {
                
                if (i < count)
                    for (int j=i; j < count; j++)
                        data[j] = data[j+1];
                
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
    void resize(size_t size) {
        auto mem = std::realloc(data, size);
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
    
    for (auto e: s)
        std::cout << e << std::endl;
}
