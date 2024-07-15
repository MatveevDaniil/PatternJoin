#ifndef BOUNDED_EDIT_DISTANCE_H
#define BOUNDED_EDIT_DISTANCE_H

#include <string>
#include <algorithm>
#include <cstdint>
#include <vector>

bool edit_distance_k(
    std::string a, 
    std::string b, 
    int k
);

bool hamming_distance_k(
    std::string a, 
    std::string b, 
    int k
);

#endif // BOUNDED_EDIT_DISTANCE_H