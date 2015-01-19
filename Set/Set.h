//
//  Set.h
//  Set
//
//  Created by Gabriele Carrettoni on 11/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#ifndef Set_Set_h
#define Set_Set_h

#include <iostream>
#include <cassert>

#include "Exceptions.h"
#include "Utils.h"
#include "Filters.h"

namespace set {
    
    using namespace filters;
    
    /**
     Class that implement a Set-like structure, with random access in O(1), orderer insertion,
     and custom lookup filter.
     
     @param T the type of the values inside the Set
     @param F the filter to use, defaulted to BloomFilter
     */
    template <typename T, typename F = BaseFilter<T>>
    class Set {
        
        template <bool is_const = true>
        class _iterator {
            //
        public:
            using iterator_category = std::random_access_iterator_tag ;
            using value_type        = typename std::conditional<is_const, const T, T>::type;
            using difference_type   = ptrdiff_t;
            using pointer           = value_type*;
            using reference         = value_type&;
            /**
             Copy constructor
             @param other reference to the _iterator to copy.
             */
            _iterator(const _iterator &other) {
                base = other.base;
                data = other.data;
            }
            
            /**
             Assignment
             @param other reference to the _iterator to copy.
             */
            _iterator& operator=(const _iterator &other) {
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
                return *base[index];
            }
            
            /**
             Post-increment operator;
             @returns _iterator not incremented;
             */
            _iterator operator++(int) {
                return _iterator(base, data++);
            }
            
            /**
             Pre-increment operator;
             @returns _iterator incremented;
             */
            _iterator& operator++() {
                ++data;
                return *this;
            }
            
            /**
             Post-decrement operator;
             @returns _iterator not decremented;
             */
            _iterator operator--(int) {
                return _iterator(base, data--);
            }
            
            /**
             Pre-decrement operator;
             @returns _iterator decremented;
             */
            _iterator& operator--() {
                --data;
                return *this;
            }
            
            /**
             Advance operator;
             @param offset how much to advance
             @returns _iterator not advanced by offset;
             */
            _iterator operator+(int offset) {
                return _iterator(base, data+offset);
            }
            
            /**
             Recede operator;
             @param offset how much to recede
             @returns _iterator not receded by offset;
             */
            _iterator operator-(int offset) {
                return _iterator(base, data-offset);
            }
            
            /**
             Advance operator;
             @param offset how much to advance
             @returns _iterator advanced by offset;
             */
            _iterator& operator+=(int offset) {
                data += offset;
                return *this;
            }
            
            /**
             Recede operator;
             @param offset how much to recede
             @returns _iterator receded by offset;
             */
            _iterator& operator-=(int offset) {
                data -= offset;
                return *this;
            }
            
            /**
             Difference operator;
             @param _iterator reference to the other _iterator
             @returns how many elements between this and other _iterator
             */
            difference_type operator-(const _iterator &other) {
                difference_type diff = data - other.data;
                return diff;
            }
            
            /**
             Equality operator, check if the two iterators point to the same element
             @param other reference to the other _iterator
             */
            bool operator==(const _iterator<> &other) const {
                return data == other.data && base == other.base;
            }
            
            /**
             Inequality operator, check iif the two iterators
             don't point to the same element
             @param other reference to the other _iterator
             */
            bool operator!=(const _iterator<> &other) const {
                return data != other.data || base != other.base;
            }
            
            /**
             > operator, check an _iterator point to an element with an index in the collection
             that is > the index of the element pointed by the other _iterator
             @param other reference to the other _iterator
             */
            bool operator>(const _iterator<> &other) const {
                return data > other.data && base == other.base;
            }
            
            /**
             >= operator, check an _iterator point to an element with an index in the collection
             that is >= the index of the element pointed by the other _iterator
             @param other reference to the other _iterator
             */
            bool operator>=(const _iterator<> &other) const {
                return data >= other.data && base == other.base;
            }
            
            /**
             < operator, check an _iterator point to an element with an index in the collection
             that is < the index of the element pointed by the other _iterator
             @param other reference to the other _iterator
             */
            bool operator<(const _iterator<> &other) const {
                return data < other.data && base == other.base;
            }
            
            /**
             <= operator, check an _iterator point to an element with an index in the collection
             that is <= the index of the element pointed by the other _iterator
             @param other reference to the other _iterator
             */
            bool operator<=(const _iterator<> &other) const {
                return data <= other.data && base == other.base;
            }
            
            /**
             Equality operator, check if the two iterators point to the same element.
             @param other reference to the other iterator
             */
            bool operator==(const _iterator<false> &other) const {
                return data == other.data && base == other.base;
            }
            
            /**
             Inequality operator, check if the two iterators
             don't point to the same element.
             @param other reference to the other iterator
             */
            bool operator!=(const _iterator<false> &other) const {
                return data != other.data || base != other.base;
            }
            
            /**
             > operator, check a _iterator point to an element with an index in the collection
             that is > the index of the element pointed by the other _iterator
             @param other reference to the other iterator
             */
            bool operator>(const _iterator<false> &other) const {
                return data > other.data && base == other.base;
            }
            
            /**
             >= operator, check a _iterator point to an element with an index in the collection
             that is >= the index of the element pointed by the other iterator
             @param other reference to the other iterator
             */
            bool operator>=(const _iterator<false> &other) const {
                return data >= other.data && base == other.base;
            }
            
            /**
             < operator, check a _iterator point to an element with an index in the collection
             that is < the index of the element pointed by the other iterator
             @param other reference to the other iterator
             */
            bool operator<(const _iterator<false> &other) const {
                return data < other.data && base == other.base;
            }
            
            /**
             <= operator, check a _iterator point to an element with an index in the collection
             that is <= the index of the element pointed by the other iterator
             @param other reference to the other iterator
             */
            bool operator<=(const _iterator<false> &other) const {
                return data <= other.data && base == other.base;
            }
            
        private:
            T* base;
            T* data;
            
            friend class Set;
            
            _iterator(T* base_, T* data_): base(base_), data(data_) {}
            _iterator(T* data_): base(data_), data(data_) {}
            
        };
        
        using const_iterator = _iterator<true>;
        using iterator       = _iterator<false>;
        
    public:
        /**
         Constructor
         */
        Set() {
            alloc(size);
        };
        
        /**
         Copy constructor, performs a deep copy of all the elements in the other Set
         @param Set& reference to the set to copy
         @returns class instance
         */
        Set(const Set& set_) {
            size = set_.size;
            alloc(size);
            
            for (const auto e: set_)
                insert(e);
        }
        
        /**
         Move constructor
         */
        Set(Set&& set_) {
            size = set_.size;
            last = set_.last;
            
            data = std::move(set_.data);
        }
        
        /**
         Destructor, it frees up the memory used.
         */
        ~Set() {
            if (data) std::free(data);
            data = nullptr;
        }
        
        /**
         Constructor from generic iterators
         
         @param begin first element of the iterator
         @param end last element of the iterator
         @returns class instance
         */
        template <typename Iterator>
        Set(Iterator begin, Iterator end) {
            size = end - begin;
            alloc(size);
            
            for (; begin != end; begin++)
                try {
                    insert(*begin);
                } catch (exceptions::already_in) {
                    
                }
        }
        
        /**
         Subscribe operator to access element by index in costant time
         @param p the index of the element to retrieve
         @returns const reference to the element
         @exception assert exception if p is outside the valide range
         */
        const T& operator[](int p) const {
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
                        throw exceptions::already_in();
                
            } else if (query == Query::FOUND)
                throw exceptions::already_in();
            
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
         @exception not_found() if the element is not found in the Set.
         */
        void remove(const T t) {
            auto query = filter.query(t);
            if (query == Query::NOT_FOUND)
                throw exceptions::not_found();
            
            for (int i=0; i <= last; i++) {
                if (data[i] == t) {
                    
                    filter.remove(t);
                    std::rotate(begin()+i, begin()+i+1, end());
                    
                    data[last].~T();
                    
                    if (--last < size/2) shrink();
                    return;
                }
            }
            
            throw exceptions::not_found();
        }
        
        /**
         Return the const_iterator, pointing at the first element
         @returns the const_iterator
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
         Grows the size of the buffer exponentially
         @exception bad_alloc if the allocation isn't successfull
         */
        void grow() {
            if (size) size *= 2;
            else size = 1;
            
            alloc(size);
        }
        
        /**
         Shrink the size of the buffer exponentially
         @exception bad_alloc if the allocation isn't successfull
         */
        void shrink() {
            size /= 1.5;
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
            for (const auto e: set) {
                os << e << " ";
            }
            
            return os;
        }
        
        F filter;
        T* data = nullptr;
        int last = -1;
        size_t size = 1;
    };
    
    /**
     Create a new Set from a Set filtering out the elements that pass the
     predicate function passed.
     @param s the set to filter out
     @param p the function or lambda to use to filter the elements
     @returns the new Set
     */
    template <typename T, typename F, typename P>
    Set<T,F> filter_out(const Set<T,F>& c, P p) {
        Set<T,F> n_c;
        
        for (auto e: c)
            if (!p(e))
                n_c.insert(e);
        
        return n_c;
    }
}

#endif
