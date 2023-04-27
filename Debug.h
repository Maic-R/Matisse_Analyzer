#ifndef Debug_h
#define Debug_h

#include <iostream>

// set VERBOSE to true to show all the debug output
#define VERBOSE false

#define DBPRINT(x, y) { \
    if (VERBOSE) { \
        std::cout << "Function " << __func__ << " called in file : " << __FILE__ << " at line " << __LINE__ << "\n\t"; \
        std::cout << x << y << std::endl << std::endl; \
    } \
}

#endif