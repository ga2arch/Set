//
//  main.cpp
//  Set
//
//  Created by Gabriele Carrettoni on 08/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

/**
 @file
 @brief main
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <cstddef>
#include <stdexcept>

/**
 Custom exception thrown in the case one tries to insert an element
 already present in the set.
 */
struct already_in: public std::runtime_error {
    already_in(): std::runtime_error("Element already in set") {}
};


/**
 Custom exception thrown in the case one tries to delete an element
 no found in the set.
 */
struct not_found: public std::runtime_error {
    not_found(): std::runtime_error("Element not found") {}
};

/**
 Combine two hashes
 @param t element to hash
 @returns hashed value
 */
template <typename T>
inline size_t hash_combine(const T& t, size_t seed) {
    std::hash<T> hashfn;
    seed ^= hashfn(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    
    return seed;
}

/**
 Enumerator class to rappresent the results of a Query
 */
enum class Query { FOUND, NOT_FOUND, MAYBE };

/**
 Class that implements a BasicFilter, it always returns a MAYBE when queried 
 for a value, forcing the Set class to linear search the Set for the element.
 */
class BaseFilter {
    
public:
    template <typename T>
    void add(const T t) { }

    template <typename T>
    Query query(const T t) {
        return Query::MAYBE;
    }

    template <typename T>
    void remove(const T t) { }
    
};

/**
 Class that implements a BloomFilter for O(1) check if an element is NOT present
 into the Set. Query could return false-positive, that's why in the event the element
 is found it returns MAYBE.

 @param SIZE the size of the bloomfilter array
 @param K the number of hashing functions
 */
template <size_t SIZE = 1000, size_t K = 5>
class BloomFilter: public BaseFilter {

public:
    BloomFilter(): bloom(std::unique_ptr<uint8_t[]>(new uint8_t[SIZE])) {};
    
    /**
     Add the value t to the bloomfilter
     @param t value to add
     */
    template <typename T>
    void add(const T t) {
        for (int i=0; i < K; i++)
            bloom[hash(t, i)]++;
    }
    
    /**
     Query the value t, if the query is NOT_FOUND, the element is NOT in the bloomfilter,
     if the query is MAYBE, the element COULD be in the bloomfilter.
     @param t value to query
     @returns Query query result
     */
    
    template <typename T>
    Query query(const T t) {
        for (int i=0; i < K; i++)
            if (!bloom[hash(t, i)]) return Query::NOT_FOUND;
        
        return Query::MAYBE;
    }
    
    /**
     Remove the value t from the bloomfilter
     @param t value to remove
     */
    
    template <typename T>
    void remove(const T t) {
        for (int i=0; i < K; i++)
            bloom[hash(t, i)]--;
    }
    
private:
    /**
     Hash the value passed by the i-th hash function and reduce it in the range
     0 <= hash < SIZE.
     Uses only two hash function to simulate any number of hash functions
     based on the paper Less Hashing, Same Performance 
     (http://www.eecs.harvard.edu/~kirsch/pubs/bbbf/esa06.pdf)
     @param t the element to hash
     @param i the i-th hash function
     @returns int the hash
     */
    
    template <typename T>
    int hash(const T t, int i) {
        auto h1 = hash_combine(t, 0);
        auto h2 = hash_combine(t, h1);
        
        return (h1 + i*h2) % SIZE;
    }
    
    std::unique_ptr<uint8_t[]> bloom;
};

/**
 Class that implement a Set-like structure, with random access in O(1), orderer insertion,
 and custom lookup filter.
 
 @param T the type of the values inside the Set
 @param F the filter to use, defaulted to BloomFilter
 */
template <typename T, class F = BloomFilter<>>
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
        
        /**
         Copy constructor
         @param other reference to the iterator to copy.
         */
        iterator(const iterator &other) {
            base = other.base;
            data = other.data;
        }
        
        /**
         Assignment
         @param other reference to the iterator to copy.
         */
        iterator& operator=(const iterator &other) {
            base = other.base;
            data = other.data;
            return *this;
        }
        
        /**
         Dereference pointer
         */
        reference operator*() const {
            return *data;
        }
        
        /**
         Return pointer
         @returns pointer to the current element.
         */
        pointer operator->() const {
            return data;
        }
        
        /**
         Return element at index
         @returns reference to the value if the element at index
         */
        reference operator[](int index) {
            return base[index];
        }
        
        /**
         Post-increment operator;
         @returns iterator not incremented;
         */
        iterator operator++(int) {
            return iterator(base, data++);
        }
        
        /**
         Pre-increment operator;
         @returns iterator incremented;
         */
        iterator& operator++() {
            ++data;
            return *this;
        }
        
        /**
         Post-decrement operator;
         @returns iterator not decremented;
         */
        iterator operator--(int) {
            return iterator(base, data--);
        }
        
        /**
         Pre-decrement operator;
         @returns iterator decremented;
         */
        iterator& operator--() {
            --data;
            return *this;
        }
        
        /**
         Advance operator;
         @param offset how much to advance
         @returns iterator not advanced by offset;
         */
        iterator operator+(int offset) {
            return iterator(base, data+offset);
        }
        
        /**
         Recede operator;
         @param offset how much to recede
         @returns iterator not receded by offset;
         */
        iterator operator-(int offset) {
            return iterator(base, data-offset);
        }
        
        /**
         Advance operator;
         @param offset how much to advance
         @returns iterator advanced by offset;
         */
        iterator& operator+=(int offset) {
            data += offset;
            return *this;
        }
        
        /**
         Recede operator;
         @param offset how much to recede
         @returns iterator receded by offset;
         */
        iterator& operator-=(int offset) {
            data -= offset;
            return *this;
        }
        
        /**
         Difference operator;
         @param iterator reference to the other iterator
         @returns how many elements between this and other iterator
         */
        difference_type operator-(const iterator &other) {
            difference_type diff = data - other.data;
            return diff;
        }
        
        /**
         Equality operator, check if two iterator are the same.
         @param other reference to the other iterator
         @returns if the two iterators are equal
         */
        bool operator==(const iterator &other) const {
            return data == other.data && base == other.base;
        }
        
        /**
         Inequality operator, check if two iterator are NOT the same.
         @param other reference to the other iterator
         @returns if the two iterators are NOT equal
         */
        bool operator!=(const iterator &other) const {
            return data != other.data && base != other.base;
        }
        
        
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
            try {
                insert(*begin);
            } catch (std::runtime_error) {
                
            }
    }
    
    /**
     Subscribe operator to access element by index in costant time
     @param p the index of the element to retrieve
     @returns const reference to the element
     @exception assert exception if p is outside the valide range
     */
    const T& operator[](int p) const {
        assert(p >= 0 && p <= last);
        return data[p];
    }
    
    /**
     Perform an insertion of an element into the Set, it checks first the Checker, 
     if the query is negative, procedes with the insertion, if positive
     it checks for the existence of the element (could be a false positive) and
     eventually performs the insertion.
     @param t the element
     @exception already_in() if the elements is already present in the Set.
     */
    void insert(const T t) {
        auto query = filter.query(t);
        if (query == Query::MAYBE) {
            
            for (int i=0; i <= last; i++)
                if (data[i] == t)
                    throw already_in();

        } else if (query == Query::FOUND)
            throw already_in();
        
        filter.add(t);
        if (++last == size)
            grow();
        
        data[last] = t;
    }
    
    /**
     Remove and element from the Set, it checks first the Checker, if the
     query is negative, throws an exception, if positive, search the element.
     If the element is found, use std::rotate to perform a left rotation, effectively 
     moving the element to remove at the end of the Set but outside the valid range.
     @param t the element
     @returns void
     @exception not_found() if the element is not found in the Set.
     */
    void remove(const T t) {
        auto query = filter.query(t);
        if (query == Query::NOT_FOUND)
            throw not_found();

        for (int i=0; i <= last; i++) {
            if (data[i] == t) {
                
                filter.remove(t);
                std::rotate(begin()+i, begin()+i+1, end());
                
                if (--last < size/2)
                    shrink();
                
                return;
            }
        }
        
        throw not_found();
    }
    
    /**
     Return the const_iterator, pointing at the first element
     @returns the const:iterator
     */
    const_iterator begin() const {
        return const_iterator(data);
    }
    
    /**
     Return the const_iterator, pointing at the element after the last
     @returns the const_iterator
     */
    const_iterator end() const {
        return const_iterator(data, data+last+1);
    }
    
    /**
     Return the iterator, pointing at the first element
     @returns the iterator
     */
    iterator begin() {
        return iterator(data);
    }
    
    /**
     Return the iterator, pointing at the last element
     @returns the iterator
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
        if (size) size *= 2;
        else size = 1;
        
        alloc(size);
    }
    
    /**
     Shrink the size of the buffer exponetially
     @exception bad_alloc if the allocation isn't successfull
     */
    void shrink() {
        size /= 2;
        alloc(size);
    }
    
    /**
     Realloc a chunk of memory based on the size passed.
     @param s
     @exception bad_alloc if the allocation isn't successfull
     */
    void alloc(size_t s) {
        auto mem = std::realloc(data, s*sizeof(T));
        if (!mem)
            throw std::bad_alloc();
        
        data = static_cast<T*>(mem);
    }
    
    /**
     Comodity function to easily display the content of a Set.
     @param os stream to write on
     @param set reference of the set to write
     @return output stream
     */
    friend std::ostream& operator<<(std::ostream &os, const Set &set) {
        for (auto e: set) {
            os << e << " ";
        }
        
        return os;
    }
    
    F filter;
    T* data = nullptr;
    int last = -1;
    size_t size = 0;
    
};


/**
 Create a new Set from a Set filtering out the elements that pass the 
 predicate function passed.
 @param s the set to filter out
 @param p the function or lambda to use to filter the elements
 @returns the new Set
 */
template <typename T, class F, typename P>
Set<T,F> filter_out(const Set<T,F>& s, P p) {
    Set<T,F> n;
    
    for (auto e: s) {
        if (!p(e)) {
            n.insert(e);
        }
    }
    
    return n;
}

int main(int argc, const char * argv[]) {
    Set<int, BloomFilter<1000, 10>> s;
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

    std::cout << n;
    
    std::cout << s;
}
