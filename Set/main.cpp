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

#include "Set.h"

#include <vector>

using namespace set;

int main(int argc, const char * argv[]) {
    Set<int> s;
    std::vector<int> l{4,5,8,9,10};
    
    std::cout << "Test insertion: ";
    s.insert(4);
    s.insert(5);
    s.insert(8);
    s.insert(9);
    s.insert(10);
    
    assert(std::equal(s.begin(), s.end(), l.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test deletion: ";
    s.remove(5);
    
    l = {4,8,9,10};
    assert(std::equal(s.begin(), s.end(), l.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test insertion after deletion: ";
    s.insert(5);
    
    l = {4,8,9,10,5};
    assert(std::equal(s.begin(), s.end(), l.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test random access: ";
    assert(s[1] == l[1]);
    std::cout << "PASSED\n";
    
    std::cout << "Test insertion of already inserted element: ";
    bool error = false;
    try {
        s.insert(4);
    } catch (exceptions::already_in) {
        error = true;
    }
    
    assert(error);
    std::cout << "PASSED\n";
    
    std::cout << "Test deletion of element not in the set: ";
    error = false;
    try {
        s.remove(30);
    } catch (exceptions::not_found) {
        error = true;
    }
    
    assert(error);
    std::cout << "PASSED\n";
    
    std::cout << "Test copy constructor: ";
    auto c = s;
    
    assert(std::equal(s.begin(), s.end(), c.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test constructor from iterators of vector: ";
    l = {4,4,8,9,10};
    Set<int> m(l.begin(), l.end());
    l = {4,8,9,10};

    assert(std::equal(l.begin(), l.end(), m.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test filter out: ";
    auto f = filter_out(s, [](int x) { return x == 4; });
    l = {8,9,10,5};
    
    assert(std::equal(l.begin(), l.end(), f.begin()));
    std::cout << "PASSED\n";
    
    std::cout << "Test constructor from iterators of set: ";
    Set<int> m1(s.begin(), s.end());
    
    assert(std::equal(s.begin(), s.end(), m1.begin()));
    std::cout << "PASSED\n";

}
