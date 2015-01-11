//
//  Exceptions.h
//  Set
//
//  Created by Gabriele Carrettoni on 11/01/15.
//  Copyright (c) 2015 Gabriele Carrettoni. All rights reserved.
//

#ifndef Set_Exceptions_h
#define Set_Exceptions_h

#include <cstddef>
#include <stdexcept>

namespace set { namespace exceptions {
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
} }

#endif
