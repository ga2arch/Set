//
//  main.cpp
//  Set
//
//  Created by Gabriele Carrettoni on 08/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#include <iostream>

template <typename T>
class Set {
    
public:
    Set(): data(nullptr), size(-1) {}
    
    ~Set() {
        delete data;
    }
    
    T& operator[](int p) {
        return *(data+p);
    }
    
    void insert(const T t) {
        for (int i=0; i <= size; i++)
            if (*(data+i) == t)
                throw std::runtime_error("Error");
        
        data = (T*) realloc(data, ++size);
        *(data+size) = t;
    }
    
    void remove(const T t) {
        for (int i=0; i <= size; i++) {
            if (*(data+i) == t) {
                
                if (i < size)
                    for (int j=i; j < size; j++)
                        *(data+j) = *(data+j+1);
                
                data = (T*) realloc(data, --size);
                
                return;
            }
        }
        
        throw std::runtime_error("Element not found");
    }
    
    
private:
    T* data;
    int size;
    
};

int main(int argc, const char * argv[]) {
    Set<int> s;
    
    s.insert(4);
    s.insert(3);
    s.insert(5);
    
    s.remove(3);
    
    std::cout << s[1];
    
}
