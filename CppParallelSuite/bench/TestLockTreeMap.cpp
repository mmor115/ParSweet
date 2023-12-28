#include <iostream>
#include "../lock_tree_map/LockTreeMap.hpp"

using namespace parallel_suite::lock_tree_map;

LockTreeMap<std::string, int> map;

int main() {
    int val;

    map.put("a", 1);
    map.put("b", 2);
    map.put("c", 3);


    if (map.get("b", val)) {
        std::cout << "b -> " << val << std::endl;
    } else {
        std::cout << "b -> (empty)" << std::endl;
    }

    std::cout << "item was " << (map.del("b", val) ? "" : "not ") << "deleted (" << val << ")" << std::endl;

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
