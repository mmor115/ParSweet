
#ifndef NODE_MARKERS_HPP
#define NODE_MARKERS_HPP

namespace parallel_suite {
    struct HeadNode_t {
        consteval explicit HeadNode_t(int) { }
    };

    struct TailNode_t {
        consteval explicit TailNode_t(int) { }
    };

    inline constexpr HeadNode_t HeadNode{0};
    inline constexpr TailNode_t TailNode{0};
}


#endif //NODE_MARKERS_HPP
