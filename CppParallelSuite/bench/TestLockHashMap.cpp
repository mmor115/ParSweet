#include <iostream>
#include "../lock_hash_map/LockHashMap.hpp"

using namespace parallel_suite::lock_hash_map;

LockHashMap<std::string, int, 16> map;

int main() {
    int val;

    map.put("a", 1, val);
    map.put("b", 2, val);
    map.put("c", 3, val);


    if (map.get("b", val)) {
        std::cout << "b -> " << val << std::endl;
    } else {
        std::cout << "b -> (empty)" << std::endl;
    }

    std::cout << map.del("b") << std::endl;

    if (map.get("b", val)) {
        std::cout << "b -> " << val << std::endl;
    } else {
        std::cout << "b -> (empty)" << std::endl;
    }

    if (map.get("c", val)) {
        std::cout << "c -> " << val << std::endl;
    } else {
        std::cout << "c -> (empty)" << std::endl;
    }

    return 0;
}
