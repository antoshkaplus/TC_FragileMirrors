#pragma once
#include <memory>
#include <vector>


namespace mirrors {

template <class CastType>
struct CastNode {
    CastNode(const CastType& cast, const std::shared_ptr<CastNode>& previous)
            : cast{cast}, previous{previous} {}

    static std::vector<CastType> ToHistory(std::shared_ptr<CastNode> node) {
        std::vector<CastType> casts;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        return casts;
    }

    static std::shared_ptr<CastNode> Push(std::shared_ptr<CastNode> node, CastType cast) {
        return std::make_shared<CastNode>(cast, node);
    }

    CastType cast;
    std::shared_ptr<CastNode> previous;
};

}

