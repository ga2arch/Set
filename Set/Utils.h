//
//  Utils.h
//  Set
//
//  Created by Gabriele Carrettoni on 11/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#ifndef Set_Utils_h
#define Set_Utils_h

#include <functional>

namespace set { namespace utils {

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
     Hash the value passed by the i-th hash function and reduce it in the range
     0 <= hash < SIZE.
     Uses only two hash function to simulate any number of hash functions
     based on the paper Less Hashing, Same Performance
     (https://www.eecs.harvard.edu/~michaelm/postscripts/tr-02-05.pdf)
     @param t the element to hash
     @param i the i-th hash function
     @returns int the hash
     */
    template <typename T>
    size_t hash(const T t, size_t size, int i=0, size_t seed=0) {
        auto h1 = hash_combine(t, seed);
        auto h2 = hash_combine(t, h1);
        
        return (h1 + i*h2) % size;
    }
    
    
    /**
     Enumerator class to rappresent the results of a Query
     */
    enum class Query { FOUND, NOT_FOUND, MAYBE };
    
} }

#endif
