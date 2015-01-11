//
//  Filters.h
//  Set
//
//  Created by Gabriele Carrettoni on 11/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#ifndef Set_Filters_h
#define Set_Filters_h

#include <memory>
#include <algorithm>

namespace set { namespace filters {

    using namespace utils;
    
    /**
     Class that implements a BasicFilter, it always returns a MAYBE when queried
     for a value, forcing the Set class to linear search the Set for the element.
     */
    template <typename T>
    class BaseFilter {
        
    public:
        void add(const T t) { }
        
        Query query(const T t) {
            return Query::MAYBE;
        }
        
        void remove(const T t) { }
    };
    
    /**
     Class that implements a BloomFilter for O(1) check if an element is NOT present
     into the Set. Query could return false-positive, that's why in the event the element
     is found it returns MAYBE.
     
     @param SIZE the size of the bloomfilter array
     @param K the number of hashing functions
     */
    template <typename T, size_t SIZE = 1000, size_t K = 5>
    class BloomFilter {
        
    public:
        BloomFilter(): bloom(std::unique_ptr<uint8_t[]>(new uint8_t[SIZE]())) {};
        
        /**
         Add the value t to the bloomfilter
         @param t value to add
         */
        void add(const T t) {
            for (int i=0; i < K; i++)
                bloom[hash(t, SIZE, i)]++;
        }
        
        /**
         Query the value t, if the query is NOT_FOUND, the element is NOT in the bloomfilter,
         if the query is MAYBE, the element COULD be in the bloomfilter.
         @param t value to query
         @returns Query query result
         */
        Query query(const T t) {
            for (int i=0; i < K; i++)
                if (!bloom[hash(t, SIZE, i)]) return Query::NOT_FOUND;
            
            return Query::MAYBE;
        }
        
        /**
         Remove the value t from the bloomfilter
         @param t value to remove
         */
        void remove(const T t) {
            for (int i=0; i < K; i++)
                bloom[hash(t, SIZE, i)]--;
        }
        
    private:
        std::unique_ptr<uint8_t[]> bloom;
    };
    
    template <typename T,
              size_t SIZE = 1000,
              size_t K = 2,
              size_t STASH_SIZE = 2,
              size_t MAX_DEPTH = 100>
    class CuckooTable {
        
    public:
        CuckooTable(): table(std::unique_ptr<Nest[]>(new Nest[SIZE]())),
                       stash(std::unique_ptr<T[]>(new T[STASH_SIZE]())) {}
        
        void add(const T t, int i=0, int depth=0) {
            if (query(t) == Query::FOUND) return;
            
            for (int k=i; k < K; k++) {
                auto h = hash(t, size, k, seed);
                
                if (!table[h].full) {
                    table[h].insert(t, k+1);
                    
                    return;
                }
            }
            
            auto index = hash(t, size, 0, seed);
            
            if (depth == MAX_DEPTH) {
                if (stash_use < STASH_SIZE-1) {
                    stash[++stash_use] = t;
                } else
                    reindex(index);
                return;
            }
            
            auto node = table[index];
            table[index].swap(t);
            
            add(node.t, node.i, ++depth);
        }
        
        void remove(const T t) {
            if (query(t) == Query::NOT_FOUND) return;
            
            for (int k=0; k < K; k++) {
                auto h = hash(t, size, k, seed);
                
                if (table[h].t == t) {
                    table[h] = Nest();
                    
                    return;
                }
            }
        }
        
        Query query(const T t) {
            for (int k=0; k < K; k++) {
                auto h = hash(t, size, k, seed);
                
                if (table[h].t == t) return Query::FOUND;
            }
            
            for (int i=0; i < STASH_SIZE; i++)
                if (stash[i] == t) return Query::FOUND;
            
            return Query::NOT_FOUND;
        }
        
    private:
        void reindex(size_t seed_) {
            auto old_table = std::move(table);
            auto old_size = size;
            
            size *= 2;
            stash_use = 0;
            seed = seed_;
            table = std::unique_ptr<Nest[]>(new Nest[size]());
            
            for (int i=0; i < old_size; i++) {
                auto t = old_table[i].t;
                
                add(t);
            }
        }
        
        struct Nest {
            T t;
            uint i;
            bool full;
            
            Nest(): full(false), i(0), t(T()) {};
            
            void insert(const T t_, uint i_) {
                assert(!full);
                full = true;
                
                t = t_;
                i = i_;
            }
            
            void swap(const T t_) {
                assert(full);
                t = t_;
            }
        };
        
        size_t seed = 0;
        size_t size = SIZE;
        size_t stash_use = 0;
        
        std::unique_ptr<T[]> stash;
        std::unique_ptr<Nest[]> table;
    };
    
} }

#endif
