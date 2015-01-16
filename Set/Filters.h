//
//  Filters.h
//  Set
//
//  Created by Gabriele Carrettoni on 11/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#ifndef Set_Filters_h
#define Set_Filters_h

#include <cstdlib>
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
    
    
    /**
     Class provvide implementation for a HashTable based on Cuckoo Hashing, allows for 
     ammortized O(1) fast set element query.
     @param SIZE the size of the table
     @param K the number of hash functions
     @param STASH_SIZE the size of the stash
     @param MAX_DEPTH the depth cutoff
     @param FIXED if the table is fixed
     */
    template <typename T,
              size_t SIZE = 1000,
              size_t K = 2,
              size_t STASH_SIZE = 2,
              size_t MAX_DEPTH = 100,
              bool   FIXED = false>
    class CuckooTable {
        
    public:
        CuckooTable(): table(std::unique_ptr<Nest[]>(new Nest[SIZE]())),
                       stash(std::unique_ptr<T[]>(new T[STASH_SIZE]())) {
        
            srand (static_cast<int>(time(NULL)));
        }
        
        /**
         Add an element to the hashtable, it hashes the element K times, each time 
         check if the nest at position given by the hash is free, if it is, puts the 
         element here and returns, if not, check the next nest, if there isn't an empty 
         nest, takes a random value between 0 <= K < 0, hashes the element with that function,
         kicks the element at related nest, insert the element, and recurse add with the kicked element.
         @param t the element to add
         @param i the last nest checked
         @param depth the depth cutoff for the recursion
         @exception runtime_error if the table is fixed and full.
         */
        void add(const T t, int i=0, int depth=0) {
            if (query(t) == Query::FOUND) return;
            
            for (int k=i; k < K; k++) {
                auto h = hash(t, size, k, seed);
                
                if (!table[h].full) {
                    table[h].insert(t, k+1);
                    
                    return;
                }
            }
            
            auto index = hash(t, size, rand() % K, seed);
            
            if (depth == MAX_DEPTH) {
                if (stash_use < STASH_SIZE-1)
                    stash[++stash_use] = t;
                
                else if (!FIXED)
                    rebuild(index);
                
                else
                    throw std::runtime_error("Full");
                
                return;
            }
            
            auto node = table[index];
            table[index].swap(t);
            
            add(node.t, node.i, ++depth);
        }
        
        /**
         Search K-nests for the element to remove.
         @param t the element to remove
         */
        void remove(const T t) {
            if (query(t) == Query::NOT_FOUND) return;
            
            for (int k=0; k < K; k++) {
                auto h = hash(t, size, k, seed);
                
                if (table[h].t == t) {
                    table[h].full = 0;
                    
                    return;
                }
            }
        }
        
        /**
         Search K-nests for the element to remove.
         @param t the element
         @returns the Query result
         */
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
        /**
         Rebuilds the table, increasing the size and using the last element 
         hash as the new seed of the hash functions.
         @param seed_ the new seed
         */
        void rebuild(size_t seed_) {
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
        
        /**
         Simple struct to rappresent each slot in the hashtable
         */
        struct Nest {
            T t;
            uint i;
            uint full : 1;
            
            Nest(): full(0), i(0), t(T()) {};
            
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
 
    
    /**
     Class to implement a CuckooFilter, a lot similar to the Cuckoo Hash Table, but 
     instead of storing the element, it stores the fingerprint (just another hash) and 
     makes use of partial hashing for easily retrieve the second hash from the first.
     @param SIZE the size of the filter
     @param BUCKETS the number of buckets to use.
     @param MAX_DEPTH depth cutoff in case of eviction.
     */
    template <typename T,
    size_t SIZE = 100,
    size_t BUCKETS = 4,
    size_t MAX_DEPTH = 100>
    
    class CuckooFilter {
        
        /**
         Nest structure, stores the fingerprint and 1 byte as flag to indicate
         if the nest if full or empty.
         */
        struct Nest {
            uint full : 1;
            size_t fingerprint;
            
            Nest(): full(0) {}
            
            void insert(size_t fp) {
                assert(!full);
                full = 1;
                
                fingerprint = fp;
            }
            
            void swap(size_t fp) {
                assert(full);
                
                fingerprint = fp;
            }
        };
        
        struct Result {
            bool found = false;
            size_t fingerprint;
            size_t h1;
            size_t h2;
        };

    public:
        CuckooFilter() {
            srand (static_cast<int>(time(NULL)));
            
            for (int i=0; i < SIZE; i++) {
                table[i] = Row(new Nest[BUCKETS]());
            }
        }
 
        void add(const T t) {
            auto res = lookup(t);
            if (res.found) return;
            
            move(res.fingerprint, res.h1);
        }
        
        void remove(const T t) {
            auto res = lookup(t);
            
            if (!res.found) return;
            
            if (remove_fp(res.fingerprint, res.h1)) return;
            if (remove_fp(res.fingerprint, res.h2)) return;
        }
        
        Query query(const T t) {
            auto res = lookup(t);
            if (res.found) return Query::MAYBE;
            
            return Query::NOT_FOUND;
        }
    
    private:
        void move(size_t fingerprint, size_t h1, int depth=0) {
            auto h2 = (h1 ^ hash(fingerprint, size, 900, seed)) % SIZE;
            
            if (add_fp(fingerprint, h1)) return;
            if (add_fp(fingerprint, h2)) return;

            if (depth == MAX_DEPTH)
                throw std::runtime_error("Full");
            
            auto row = rand() % 2 == 0 ? h1 : h2;
            auto col = rand() % 4;
            
            auto elem = table[row][col].fingerprint;
            table[row][col].swap(fingerprint);
            
            move(elem, row);
        }
        
        Result lookup(const T t) {
            auto fingerprint = hash(t, size, 1000, seed);
            auto h1 = hash(t, size, 0, seed);
            auto h2 = (h1 ^ hash(fingerprint, size, 900, seed)) % SIZE;
            
            Result res;
            for (int i=0; i < BUCKETS; i++) {
                auto& n1 = table[h1][i];
                
                if (n1.full && n1.fingerprint == fingerprint) {
                    res.found = true;
                    break;
                }

                auto& n2 = table[h2][i];
                
                if (n2.full && n2.fingerprint == fingerprint) {
                    res.found = true;
                    break;
                }

            }
            
            res.fingerprint = fingerprint;
            res.h1 = h1;
            res.h2 = h2;
            
            return res;
        }
        
        bool add_fp(size_t fp, size_t h) {
            for (int i=0; i < BUCKETS; i++) {
                if (!table[h][i].full) {
                    table[h][i].insert(fp);
                    
                    return true;
                }
            }
            
            return false;
        }
        
        bool remove_fp(size_t fp, size_t h) {
            for (int i=0; i < BUCKETS; i++)
                if (table[h][i].full && table[h][i].fingerprint == fp) {
                    table[h][i].full = 0;
                    
                    return true;
                }
            
            return false;
        }
        
        size_t seed = 0;
        size_t size = SIZE;
        
        using Row   = std::unique_ptr<Nest[]>;
        using Table = std::unique_ptr<Row[]>;
        
        Table table = Table(new Row[SIZE]);
    };
    
}}

#endif
