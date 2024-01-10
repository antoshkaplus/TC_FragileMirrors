#pragma once
#include <cstdlib>
#include "mirrors/common/types.hpp"


namespace mirrors {

struct BeamWidth {
    size_t init;

    size_t width(cell_count_t /*empty_lines*/) const {
        return init;
    }
};

struct BeamWidth_2 {
    size_t init;

    size_t width(cell_count_t empty_lines) const {
        return init * sqrt(1 + empty_lines);
    }
};

struct BeamWidth_3 {
    size_t init;
    double param;

    size_t width(cell_count_t empty_lines) const {
        return static_cast<size_t>(init+param*empty_lines);
    }
};

}