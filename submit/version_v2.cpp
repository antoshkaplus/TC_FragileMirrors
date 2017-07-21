#include <vector>
#include <tuple>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <cassert>
#include <chrono>
#include <memory>
#include <unordered_set>
#include <cmath>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <list>
#include <bitset>
#include <random>
#include <limits>


// ANT

namespace ant {

using Int = int;
// index and count should be of type int
// because
// 1) unsigned types increase probability of making a bug
// 2) lesser type will create problem of casting or being too little
// 3) bigger type impossible to iterate through
// the only thing is unsigned integers is good for bitwise operations
using Count = int;
using Index = int;

using Long = int64_t;
using Float = float;
using Double = double;

}

namespace ant {

namespace geometry {

namespace d2 {


template<class T>
struct Size {
    Size() : Size(0, 0) {}

    Size(T width, T height)
    : width(width), height(height) {}

    void set(T width, T height) {
        this->width = width;
        this->height = height;
    }

    T area() const {
        return width * height;
    }

    T perimeter() const {
        return 2 * (height + width);
    }

    bool isCovering(const Size<T> &s) const {
        return width >= s.width && height >= s.height;
    }

    void swap() {
        std::swap(height, width);
    }

    Size swapped() const {
        return Size(height, width);
    }

    T width, height;
};

template<class T>
bool operator==(const Size<T> &s_0, const Size<T> &s_1) {
    return s_0.width == s_1.width && s_0.height == s_1.height;
}

template<class T>
bool operator!=(const Size<T> &s_0, const Size<T> &s_1) {
    return s_0.width != s_1.width || s_0.height != s_1.height;
}


namespace i {

using Size = d2::Size<size_t>;

struct Point {
    Point() {}

    Point(Int x, Int y) : x(x), y(y) {}

    void set(Int x, Int y) {
        this->x = x;
        this->y = y;
    }

    void swap() {
        std::swap(x, y);
    }

    Int x, y;
};

struct Segment {
    Segment() {}

    Segment(const Point &fst, const Point &snd)
    : fst(fst), snd(snd) {}

    Point fst, snd;

    void swap() {
        std::swap(fst, snd);
    }

    Segment swapped() const {
        return Segment(snd, fst);
    }
};

bool operator==(const Point &p_0, const Point &p_1);

bool operator!=(const Point &p_0, const Point &p_1);

struct Rectangle {
    Rectangle() {}

    Rectangle(const Point &origin, const Size &size)
    : origin(origin), size(size) {}

    Rectangle(Int x, Int y, Int width, Int height)
    : origin(x, y), size(width, height) {}

    void set(Int x, Int y, size_t width, size_t height) {
        origin.set(x, y);
        size.set(width, height);
    }

    void set(const Point &origin, const Point &diag) {
        this->origin = origin;
        size.set(diag.x - origin.x, diag.y - origin.y);
    }

    void swap() {
        origin.swap();
        size.swap();
    }

    size_t area() const {
        return size.area();
    }

    size_t perimeter() const {
        return size.perimeter();
    }

    bool isIntersect(const Rectangle &r) const {
        return origin.x < r.origin.x + r.size.width && origin.x + size.width > r.origin.x &&
               origin.y < r.origin.y + r.size.height && origin.y + size.height > r.origin.y;
    }

    Point origin;
    Size size;
};

bool operator==(const Rectangle &r_0, const Rectangle &r_1);

bool operator!=(const Rectangle &r_0, const Rectangle &r_1);

std::vector<Index> convex_hull(const std::vector<Point> &ps, bool is_clockwise);

} // namespace i

} // namespace d2

} // namespace geometry

} // namespace ant


namespace ant {

namespace grid {


// make it possible to substruct
struct Indent {
    Indent() : Indent(0, 0) {}

    Indent(Int row, Int col) : row(row), col(col) {}

    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }

    Int area() const {
        return row * col;
    }

    Int row, col;
};

struct Size {
    Size() : row(0), col(0) {}

    Size(Int row, Int col)
    : row(row), col(col) {}

    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }

    Count cell_count() const {
        return row * col;
    }

    Size swapped() const {
        return Size(col, row);
    }

    void swap() {
        std::swap(row, col);
    }

    Int row, col;
};

bool operator==(const Size &s_0, const Size &s_1);

// don't really know what to do with it
//template<class T>
//struct Position {
//    using value_type = T;
//
//    T row, col;
//
//    Position() {}
//    Position(T row, T col) : row(row), col(col) {}
//
//    void set(Int row, Int col) {
//        this->row = row;
//        this->col = col;
//    }
//
//    void shift(Int row, Int col) {
//        this->row += row;
//        this->col += col;
//    }
//
//    void swap() {
//        std::swap(row, col);
//    }
//    Position swapped() const {
//        return Position(col, row);
//    }
//
//    Position shifted(Int row_shift, Int col_shift) const {
//        return {row + row_shift, col + col_shift};
//    }
//
//    struct TopLeftComparator {
//        bool operator()(const Position& p_0, const Position& p_1) {
//            return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
//        }
//    };
//    struct BottomRightComparator {
//        bool operator()(const Position& p_0, const Position& p_1) {
//            return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
//        }
//    };
//};



struct Position {
    // operators see below
    Position() : Position(0, 0) {}

    Position(Int row, Int col) : row(row), col(col) {}

    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }

    void shift(Int row, Int col) {
        this->row += row;
        this->col += col;
    }

    void swap() {
        std::swap(row, col);
    }

    Position swapped() const {
        return Position(col, row);
    }

    Position shifted(Int row_shift, Int col_shift) const {
        return {row + row_shift, col + col_shift};
    }

    Int row, col;

    struct TopLeftComparator {
        bool operator()(const Position &p_0, const Position &p_1) {
            return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
        }
    };

    struct BottomRightComparator {
        bool operator()(const Position &p_0, const Position &p_1) {
            return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
        }
    };
};


bool operator==(const Size &s_0, const Size &s_1) {
    return s_0.row == s_1.row && s_0.col == s_1.col;
}

Position operator-(const Position &p, const Indent &n) {
    return {p.row - n.row, p.col - n.col};
}

Position operator+(const Position &p, const Indent &n) {
    return {p.row + n.row, p.col + n.col};
}

Position operator+(const Position &p, const Position &n) {
    return {p.row + n.row, p.col + n.col};
}

bool operator==(const Position &p_0, const Position &p_1) {
    return p_0.row == p_1.row && p_0.col == p_1.col;
}

bool operator!=(const Position &p_0, const Position &p_1) {
    return !(p_0 == p_1);
}


Position operator-(const Position &p, const Indent &n);

Position operator+(const Position &p, const Indent &n);

bool operator==(const Position &p_0, const Position &p_1);

bool operator!=(const Position &p_0, const Position &p_1);

struct Region {
    Region() : Region(0, 0, 0, 0) {}

    Region(Int row, Int col, Int row_count, Int col_count)
    : position(row, col), size(row_count, col_count) {}

    Region(const Position &p, const Size &d)
    : position(p), size(d) {}

    void set(Int row, Int col, Int row_indent, Int col_indent) {
        position.set(row, col);
        size.set(row_indent, col_indent);
    }

    void set(const Position &p, const Size &d) {
        position = p;
        size = d;
    }

    void shift(Int row, Int col) {
        position.row += row;
        position.col += col;
    }

    // maybe someone will find those getters useful
    Int row_begin() const { return position.row; }

    Int row_end() const { return position.row + size.row; }

    Int col_begin() const { return position.col; }

    Int col_end() const { return position.col + size.col; }

    Int col_count() const { return size.col; }

    Int row_count() const { return size.row; }

    void set_row_begin(Int r) {
        size.row += position.row - r;
        position.row = r;
    }

    void set_row_end(Int r) {
        size.row = r - position.row;
    }

    void set_col_begin(Int c) {
        size.col += position.col - c;
        position.col = c;
    }

    void set_col_end(Int c) {
        size.col = c - position.col;
    }

    bool hasInside(const Position &p) const {
        return p.row >= position.row &&
               p.row < position.row + size.row &&
               p.col >= position.col &&
               p.col < position.col + size.col;
    }

    bool hasInside(const Region &region) const {
        return row_begin() <= region.row_begin()
               && col_begin() <= region.col_begin()
               && row_end() >= region.row_end()
               && col_end() >= region.col_end();
    }

    Region intersection(const Region &r) const {
        Region n;
        n.position.row = std::max(position.row, r.position.row);
        n.position.col = std::max(position.col, r.position.col);
        n.size.row = std::min(row_end(), r.row_end()) - n.position.row;
        n.size.col = std::min(col_end(), r.col_end()) - n.position.col;

        if (n.size.row < 0 || n.size.col < 0) {
            n.size.row = 0;
            n.size.col = 0;
        }
        return n;
    }

    bool hasIntersection(const Region &r) const {
        return !intersection(r).isEmpty();
    }

    bool isEmpty() const { return size.row == 0 || size.col == 0; }

    template<class ForwardIterator>
    static Region unite(ForwardIterator first, ForwardIterator last) {
        Region rect = *first;
        while (++first != last) {
            // should update "end" first because it dependent on "begin"
            rect.set_row_end(std::max(first->row_end(), rect.row_end()));
            rect.set_col_end(std::max(first->col_end(), rect.col_end()));
            rect.set_row_begin(std::min(first->row_begin(), rect.row_begin()));
            rect.set_col_begin(std::min(first->col_begin(), rect.col_begin()));

        }
        return rect;
    }

    Int cell_count() const {
        return size.row * size.col;
    }

    Position position;
    Size size;
};

std::ostream &operator<<(std::ostream &output, const Region &r);


bool operator==(const Region &r_0, const Region &r_1) {
    return r_0.position == r_1.position && r_0.size == r_1.size;
}

bool operator==(const Region &r_0, const Region &r_1);


// G - grid
template<class G>
struct GridView {

    using value_type = typename G::value_type;
    using const_reference = const value_type &;
private:
    using real_type = typename std::conditional<std::is_const<G>::value, const value_type, value_type>::type;
public:


    GridView(std::shared_ptr<G> grid, const Position &origin) : grid_(grid), origin_(origin) {}

    GridView() : origin_(0, 0) {}

    const_reference operator()(const Position &p) const {
        return (*grid_)(p + origin_);
    }

    const_reference operator()(Index row, Index col) const {
        return (*grid_)(row + origin_.row, col + origin_.col);
    }


    real_type operator()(const Position &p) {
        return (*grid_)(p + origin_);
    }

    real_type operator()(Index row, Index col) {
        return (*grid_)(row + origin_.row, col + origin_.col);
    }


    void set_origin(const Position &p) {
        origin_ = p;
    }

    const Size &size() const {
        return grid_->size();
    }

private:


    std::shared_ptr<G> grid_;
    Position origin_;
};

template<class G>
class OriginGrid {
public:
    using value_type = typename G::value_type;

    OriginGrid() {}

    OriginGrid(Position origin, Size size) : origin_(origin), grid_(size) {}


    value_type &operator()(const Position &p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }

    const value_type &operator()(const Position &p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }

    value_type &operator[](const Position &p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }

    const value_type &operator[](const Position &p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }

    value_type &operator()(Int row, Int col) {
        return grid_(row - origin_.row, col - origin_.col);
    }

    const value_type &operator()(Int row, Int col) const {
        return grid_(row - origin_.row, col - origin_.col);
    }


    G &grid() {
        return grid_;
    }

    const G &grid() const {
        return grid_;
    }

    void set_grid(G &&grid) {
        grid_ = std::move(grid);
    }

    void set_grid(const G &grid) {
        grid_ = grid;
    }

    void set_origin(const Position &p) {
        origin_ = p;
    }

    Position &origin() {
        return origin_;
    }

    const Position origin() const {
        return origin_;
    }

private:
    Position origin_;
    G grid_;
};

template<class T>
struct Grid {
    using value_type = T;

    typename std::vector<T>::const_iterator begin() const {
        return grid_.begin();
    }

    typename std::vector<T>::iterator begin() {
        return grid_.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return grid_.end();
    }

    typename std::vector<T>::iterator end() {
        return grid_.end();
    }


    Grid() : Grid(0, 0) {}

    Grid(Count row_count, Count col_count)
    : row_count_(row_count),
      col_count_(col_count),
      grid_(row_count * col_count) {}

    Grid(Count row_count, Count col_count, const T &value_type)
    : row_count_(row_count),
      col_count_(col_count),
      grid_(row_count * col_count, value_type) {}

    Grid(const Size &size) : Grid(size.row, size.col) {}

    Grid(std::initializer_list<std::vector<T>> list)
    : Grid(list.size(), list.size() == 0 ? 0 : list.begin()->size()) {
        auto b = list.begin();
        for (auto r = 0; r < row_count(); ++r, ++b) {
            std::copy(b->begin(), b->end(), grid_.begin() + r * col_count());
        }
    }

    bool isInside(const Position &p) {
        return isInside(p.row, p.col);
    }

    bool isInside(Int row, Int col) {
        return row >= 0 && row < row_count_ &&
               col >= 0 && col < col_count_;
    }

    void resize(Count row_count, Count col_count) {
        row_count_ = row_count;
        col_count_ = col_count;
        grid_.resize(row_count * col_count);
    }

    void resize(const Size &size) {
        resize(size.row, size.col);
    }

    void fill(const T &t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }

    Count row_count() const { return row_count_; }

    Count col_count() const { return col_count_; }

    Size size() const { return Size(row_count(), col_count()); }

    Count cell_count() const { return row_count() * col_count(); }

    T &operator()(const Position &p) {
        return grid_[p.row * col_count_ + p.col];
    }

    const T &operator()(const Position &p) const {
        return grid_[p.row * col_count_ + p.col];
    }

    T &operator[](const Position &p) {
        return grid_[p.row * col_count_ + p.col];
    }

    const T &operator[](const Position &p) const {
        return grid_[p.row * col_count_ + p.col];
    }

    T &operator()(Int row, Int col) {
        return grid_[row * col_count_ + col];
    }

    const T &operator()(Int row, Int col) const {
        return grid_[row * col_count_ + col];
    }

private:
    Count row_count_, col_count_;
    std::vector<T> grid_;

    friend struct const_iterator;

    template<class U>
    friend bool operator==(const Grid<U> &g_0, const Grid<U> &g_1);
};


template<class T>
bool operator==(const Grid<T> &g_0, const Grid<T> &g_1) {
    return g_0.row_count_ == g_1.row_count_ && g_0.grid_ == g_1.grid_;
}

template<class T>
std::ostream &operator<<(std::ostream &output, const Grid<T> &g) {
    for (auto r = 0; r < g.row_count(); ++r) {
        for (auto c = 0; c < g.col_count(); ++c) {
            output << g(r, c) << " ";
        }
        output << std::endl;
    }
    return output;
}


template<class T>
struct SparceGrid {
    SparceGrid() : SparceGrid(0, 0) {}

    SparceGrid(Count row_count, Count col_count)
    : row_count_(row_count),
      col_count_(col_count),
      grid_(row_count * col_count) {
    }

    bool isInside(const Position &p) {
        return isInside(p.row, p.col);
    }

    bool isInside(Int row, Int col) {
        return row >= 0 && row < row_count_ &&
               col >= 0 && col < col_count_;
    }

    void resize(Count row_count, Count col_count) {
        row_count_ = row_count;
        col_count_ = col_count;
        grid_.resize(row_count * col_count);
    }

    void fill(const T &t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }

    Count row_count() const { return row_count_; }

    Count col_count() const { return col_count_; }

    T &operator()(const Position &p) {
        return grid_[p.row * col_count_ + p.col];
    }

    const T &operator()(const Position &p) const {
        return grid_[p.row * col_count_ + p.col];
    }

    T &operator()(Int row, Int col) {
        return grid_[row * col_count_ + col];
    }

    const T &operator()(Int row, Int col) const {
        return grid_[row * col_count_ + col];
    }

private:
    Count row_count_, col_count_;
    std::unordered_map<Index, T> grid_;
};

// Syke'n'Sugarstarr - Ticket 2 Ride (Andrey Exx & Hot Hotels Remix )
template<size_t N>
struct ZobristHashing {
private:
    using Size = grid::Size;
    using States = std::vector<std::bitset<N>>;
    using Grid = grid::Grid<States>;
    using Position = grid::Position;

    Grid hash_table_;
    // will be just all zeros
    static constexpr std::bitset<N> NOTHING = 0;

    void initHashTable() {
        static const size_t S = 64;
        // will use 0 for nothing
        uint64_t min = 1;
        uint64_t max = std::numeric_limits<uint64_t>::max();
        std::uniform_int_distribution<uint64_t> distr(min, max);
        std::default_random_engine rng;
        auto randomBitset = [&]() {
            std::bitset<N> s;
            for (auto i = 0; i < (N - 1) / S + 1; ++i) {
                s <<= S;
                s |= distr(rng);
            }
            return s;
        };
        for (auto &cell : hash_table_) {
            for (auto &set : cell) {
                set = randomBitset();
            }
        }
    }

public:
    ZobristHashing(const Size &board_size, Count state_count) :
    hash_table_(board_size) {

        States states(state_count);
        states.shrink_to_fit();
        hash_table_.fill(states);
        initHashTable();
    }

    void xorState(std::bitset<N> *set, const Position &pos, Index state_index) {
        (*set) ^= hash_table_(pos)[state_index];
    }

    void xorNothing(std::bitset<N> *set) {
        (*set) ^= NOTHING;
    }

    using value = std::bitset<N>;
};

template<size_t N> constexpr std::bitset<N> ZobristHashing<N>::NOTHING;


// value should be immutable probably
template<class V>
struct TrailNode {
    const V value;
    const std::shared_ptr<TrailNode> previous;

    TrailNode(V value) : value(value) {}

    TrailNode(V value, const std::shared_ptr<TrailNode>& previous)
    : value(value), previous(previous) {}

    // current element considered last
    std::vector<V> Path() const {
        std::vector<V> r;
        TrailNode* node = this;
        while (node != nullptr) {
            r.push_back(node->value);
            node = node->previous.get();
        }
        std::reverse(r.begin(), r.end());
        return r;
    }
};

template<class V>
using TrailNodePtr = std::shared_ptr<TrailNode<V>>;


} // namespace grid

} // namespace ant

// END ANT

// UTIL


using namespace std;
using namespace ant;
using namespace ant::grid;


constexpr bool IsRightMirror(char mirror) {
    return mirror == 'R'; // '\'
}

constexpr bool IsLeftMirror(char mirror) {
    return mirror == 'L'; // '/'
}

struct CastIterator {
    CastIterator(Count sz) : cur(0), sz(sz) {}

    bool HasNext() {
        return cur != 4 * sz;
    }


    Position Next() {
        Index i = cur % 4;
        Index j = cur / 4;
        ++cur;
        switch (i) {
            case 0:
                return {j, -1};
            case 1:
                return {-1, j};
            case 2:
                return {sz, j};
            case 3:
                return {j, sz};
        }
        assert(false);
        return {};
    }

    Index cur;
    Count sz;
};


struct CastNode {
    CastNode(const Position &cast, const shared_ptr <CastNode> &previous)
    : cast(cast), previous(previous) {}

    static vector <Position> ToHistory(shared_ptr <CastNode> node) {
        vector <Position> casts;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        return casts;
    }


    Position cast;
    shared_ptr <CastNode> previous;
};

using Direction = char;


// should be somewhere in cpp probably
// from 50 to 100
constexpr array<double, 51> EMPTY_LINES_PARAM = {{
                                                 6.396, // 50
                                                 9.9972,
                                                 8.33269,
                                                 10.4997,
                                                 7.4959,
                                                 7.9984,
                                                 7.9984,
                                                 11.333,
                                                 7.9984,
                                                 11.9981,
                                                 7.9984, // 60
                                                 9.9972,
                                                 11.9981,
                                                 13.3318,
                                                 10.6623,
                                                 7.33329,
                                                 10.9966,
                                                 14.9963,
                                                 9.5003,
                                                 15.9957,
                                                 6.33389, // 70
                                                 10.6623,
                                                 9.9972,
                                                 12.4985,
                                                 7.4959,
                                                 10.9966,
                                                 8.66351,
                                                 10.4997,
                                                 8.5009,
                                                 9.5003,
                                                 7.66411, // 80
                                                 15.4988,
                                                 10.6623,
                                                 11.9981,
                                                 9.9972,
                                                 14.4994,
                                                 11.4956,
                                                 10.3315,
                                                 10.6623,
                                                 12.4985,
                                                 13.5, // 90
                                                 10.6623,
                                                 12.4985,
                                                 13.3318,
                                                 8.24685,
                                                 9.9972,
                                                 12.3324,
                                                 10.6623,
                                                 6.4965,
                                                 11.4956,
                                                 11.2506 // 100
                                                 }};


// END UTIL

// BOARD

class Board {
public:
    using HashType = bitset<64>;

    virtual HashType hash() const = 0;
    virtual Count MirrorsLeft() const {
        return (Count)(size()* size() - MirrorsDestroyed());
    }
    virtual Count MirrorsDestroyed() const = 0;
    virtual Count size() const = 0;

    virtual Count EmptyLinesCount() const = 0;

    virtual Count CastCount() const = 0;

    virtual vector<Position> CastHistory() const = 0;

    virtual bool AllDestroyed() const {
        return MirrorsLeft() == 0;
    }

    virtual ~Board() {}
};


// Restore works only once after cast
class Board_v1 : public Board {
public:
    using CastType = Position;

    virtual bool IsLineEmpty(Position p) const = 0;
    virtual Count Cast(const Position& p) = 0;
    virtual void Restore() = 0;
    virtual const vector<Position>& CastCandidates() const = 0;

    template<class Functor>
    void ForEachAppliedCast(Functor func) {
        for (auto& p : CastCandidates()) {
            Cast(p);
            func(p);
            Restore();
        }
    }

    virtual ~Board_v1() {}
};


class Board_v2 : public Board {
public:
    using CastType = short ;

    virtual Count Cast(short ray_index) = 0;
    virtual void Restore() = 0;
    virtual Count CastRestorable(short ray_index) = 0;
    virtual Count RayCount() const = 0;

    template<class Functor>
    void ForEachAppliedCast(Functor func) {
        for (auto i = 0; i < RayCount(); ++i) {
            CastRestorable(i);
            func(i);
            Restore();
        }
    }

    virtual ~Board_v2() {}
};

class Board_v2_Reduce : public Board_v2 {
public:

    Count Cast(short ray_index) final {
        auto destroyed = CastImpl(ray_index);
        if (EmptySpace()/TotalSpace() > reduce_empty_ratio_) {
            Reduce();
        }
        return destroyed;
    }

    void set_reduce_empty_ratio(double ratio) {
        reduce_empty_ratio_ = ratio;
    }

protected:
    virtual Count CastImpl(short ray_index) = 0;
    virtual void Reduce() = 0;

    virtual Count EmptySpace() const = 0;
    virtual Count TotalSpace() const = 0;

private:
    // best parameter found out from benchmarking
    double reduce_empty_ratio_{0.63};
};

// END BOARD

// CAST_HISTORY


class CastHistory {
public:
    virtual void Push(const Position& p) = 0;
    virtual void Pop() = 0;
    virtual ant::Count Count() const = 0;
};


class CastHistory_Vector : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {
        throw runtime_error("not supported");
    }

    void Push(const Position& p) override {
        history_.push_back(p);
    }

    void Pop() override {
        history_.pop_back();
    }

    ant::Count Count() const override {
        return history_.size();
    }

private:
    vector<Position> history_;

    friend vector<Position> ToVector(const CastHistory_Vector& history);
};

// I want to add more data in there
class CastHistory_Nodes : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {
        throw runtime_error("not supported");
    }

    void Pop() override {
        history_ = history_->previous;
        --count_;
    }

    void Push(const Position& p) override {
        CastNode* new_cast = new CastNode(p, history_);
        history_.reset(new_cast);
        ++count_;
    }

    ::Count Count() const override {
        return count_;
    }

private:
    shared_ptr<CastNode> history_;
    ::Count count_ = 0;

    friend vector<Position> ToVector(const CastHistory_Nodes& history);
};

class CastHistory_Nodes_v2 {

    struct Data {
        Position pos;
        short ray_index;
    };

    using Node = TrailNode<Data>;
    using NodePtr = TrailNodePtr<Data>;

public:
    void Pop() {
        history_ = history_->previous;
        --count_;
    }

    void Push(const Position&p, short ray_index) {
        Node *new_node = new Node(Data{p, ray_index}, history_);
        history_.reset(new_node);
        ++count_;
    }

    ::Count Count() const {
        return count_;
    }

    NodePtr history_;
    ::Count count_{0};

    friend vector<Position> ToVector(const CastHistory_Nodes_v2& history);
};

vector<Position> ToVector(const CastHistory_Vector& history);

vector<Position> ToVector(const CastHistory_Nodes_v2& history);


vector<Position> ToVector(const CastHistory_Vector& history) {
    return history.history_;
}

vector<Position> ToVector(const CastHistory_Nodes& history) {
    vector<Position> casts;
    CastNode* node = history.history_.get();
    while (node != nullptr) {
        casts.push_back(node->cast);
        node = node->previous.get();
    }
    reverse(casts.begin(), casts.end());
    return casts;
}



template<class Func>
static void ForEach(const CastHistory_Nodes_v2& history, Func func) {
    auto node = history.history_.get();
    while (node != nullptr) {
        func(node->value);
        node = node->previous.get();
    }
}


vector<Position> ToVector(const CastHistory_Nodes_v2& history) {
    vector<Position> casts;
    ForEach(history, [&](const CastHistory_Nodes_v2::Data& val){
        casts.push_back(val.pos);
    });
    reverse(casts.begin(), casts.end());
    return casts;
}


// END CAST_HISTORY

// SCORE

class Score {
public:
    virtual double operator()(const Board& b) const {
        return b.MirrorsDestroyed();
    }

    virtual ~Score() {}
};


class Score_v1 : public Score {
public:
    double operator()(const Board& b) const override {
        return b.MirrorsDestroyed() + EMPTY_LINES_PARAM[b.size()-50] * b.EmptyLinesCount();
    }
};

template<class Board>
class Score_Psyho {
public:
    double operator()(const Board& b) const {
        return b.MirrorsDestroyed() + (b.EmptyRowCount()+1)/(b.EmptyColCount()+1) * 8;
    }
};


class InterLevelScoreFunctor : public Score {

public:
    double operator()(const Board& b) const override {
        // may need to take into account something else
        return Score_v1()(b)/b.CastCount();
    }
};

// END SCORE

// BOARD_HASH


class BoardHash {

    constexpr static size_t HashBitsCount = 64;
    using HashFunction = ZobristHashing<HashBitsCount>;
public:
    using HashType = typename HashFunction::value;


    BoardHash() {}

    BoardHash(Count sz) {
        hash_function_.reset(new HashFunction({sz, sz}, 1));
    }

    void HashIn(char row, char col) {
        HashIn({row, col});
    }

    void HashIn(const Position& p) {
        hash_function_->xorNothing(&hash_); // xor out
        hash_function_->xorState(&hash_, p, 0);
    }

    void HashOut(const Position& p) {
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }

    const HashType& hash() const {
        return hash_;
    }

    void clear() {
        hash_ = 0;
    }

    bool empty() const {
        return hash_ == 0;
    }

    bool operator==(const BoardHash& bh) const {
        return bh.hash_ == hash_ && bh.hash_function_ == hash_function_;
    }

private:
    shared_ptr<HashFunction> hash_function_;
    HashType hash_;
};

// END BOARD_HASH

// BOARD_V6


class Board_v6 : public Board_v2_Reduce {
private:

    using int8_t = short;

    constexpr static size_t HashBitsCount = 64;

    const constexpr static int kDirTop      = 0;
    const constexpr static int kDirBottom   = 1;
    const constexpr static int kDirLeft     = 2;
    const constexpr static int kDirRight    = 3;
    const constexpr static int kDirNothing  = 4;

    const constexpr static char kMirRight     = 0;
    const constexpr static char kMirLeft      = 1;
    const constexpr static char kMirBorder    = 2;
    const constexpr static char kMirOffset    = 3;

    const constexpr static char kOrientHor = 0;
    const constexpr static char kOrientVer = 1;

    using Direction = char;
    using Mirror = char;
    using Neighbors = array<short, 4>;

    using HashFunction = ZobristHashing<HashBitsCount>;
    using Mirrors = Grid<int8_t>;
public:
    using HashType = typename HashFunction::value;

private:

    struct Ray {
        Ray(short pos, Direction dir)
        : pos(pos), dir(dir) {}

        short pos;
        Direction dir;
    };

    struct Item {
        array<short, 4> ns;
        char row;
        char col;
    };

    // first index mirror type
    // second index where ray going
    // result direction where will go
    const static array<array<char, 4>, 2> kDirReflection;


public:

    Board_v6() {}

    Board_v6(const vector<string>& str_board) : board_size_(str_board.size()),
                                                hash_(board_size_) {
        mirrors_destroyed_ = 0;
        empty_row_count_ = 0;
        empty_col_count_ = 0;

        filled_space_ = str_board.size()*str_board.size();
        empty_space_ = 0;

        InitItems();
        mirrors_left_[kOrientHor].resize(board_size_, board_size_);
        mirrors_left_[kOrientVer].resize(board_size_, board_size_);
        InitHash();

        InitMirrors(str_board);
        buffer_.reset(new vector<short>());
    }

private:

    void InitItems() {
        items_.resize(4*board_size_ + board_size_*board_size_);
        ray_direction_.resize(4*board_size_);

        // initializing inner links
        auto offset = 4*board_size_;
        auto ToIndex = [&](int r, int c) {
            return r * board_size_ + c + offset;
        };
        for (int r = 0; r < board_size_; ++r) {
            for (int c = 0; c < board_size_; ++c) {
                Index i = ToIndex(r, c);
                auto& t = items_[i];
                t.ns[kDirTop] = ToIndex(r-1, c);
                t.ns[kDirRight] = ToIndex(r, c+1);
                t.ns[kDirBottom] = ToIndex(r+1, c);
                t.ns[kDirLeft] = ToIndex(r, c-1);
                t.row = r;
                t.col = c;
            }
        }
        // initializing border links
        for (int i = 0; i < board_size_; ++i) {
            int s = 4*i;
            int m_i; // mirror index
            int b_i; // border index

            // TOP
            m_i = ToIndex(0, i);
            b_i = s + kDirTop;

            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirBottom] = m_i;
            items_[b_i].row = -1;
            items_[b_i].col = i;
            ray_direction_[b_i] = kDirBottom;
            items_[m_i].ns[kDirTop] = b_i;


            // RIGHT
            m_i = ToIndex(i, board_size_-1);
            b_i = s + kDirRight;
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirLeft] = m_i;
            items_[b_i].row = i;
            items_[b_i].col = board_size_;
            ray_direction_[b_i] = kDirLeft;
            items_[m_i].ns[kDirRight] = b_i;

            // BOTTOM
            m_i = ToIndex(board_size_-1, i);
            b_i = s + kDirBottom;
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirTop] = m_i;
            items_[b_i].row = board_size_;
            items_[b_i].col = i;
            ray_direction_[b_i] = kDirTop;
            items_[m_i].ns[kDirBottom] = b_i;

            // LEFT
            m_i = ToIndex(i, 0);
            b_i = s + kDirLeft;
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirRight] = m_i;
            items_[b_i].row = i;
            items_[b_i].col = -1;
            ray_direction_[b_i] = kDirRight;
            items_[m_i].ns[kDirLeft] = b_i;
        }
    }

    void InitMirrors(const vector<string>& str_board) {
        mirrors_.reset(new Mirrors(board_size_, board_size_));
        auto& mirs = *mirrors_;
        for (auto r = 0; r < board_size_; ++r) {
            for (auto c = 0; c < board_size_; ++c) {
                mirs(r, c) = IsRightMirror(str_board[r][c]) ? kMirRight : kMirLeft;
            }
        }
    }

    void InitHash() {
        for (auto r = 0; r < board_size_; ++r) {
            for (auto c = 0; c < board_size_; ++c) {
                hash_.HashIn(r, c);
            }
        }
    }

public:

    Count CastRestorable(short ray_index) override {
        auto& last = *buffer_;
        last.clear();
        auto& mirs = *mirrors_;

        Ray ray{ray_index, ray_direction_[ray_index]};
        ray = NextFromEmpty(ray);
        while (ray.pos >= ray_direction_.size()) {
            char r = items_[ray.pos].row;
            char c = items_[ray.pos].col;
            if (mirs(r, c) >= kMirOffset) {
                ray = NextFromEmpty(ray);
                continue;
            }
            last.push_back(ray.pos);
            Destroy(r, c);
            ray = NextFromMirror(ray, mirs(r, c));
            mirs(r, c) += kMirOffset;
        }
        mirrors_destroyed_ += last.size();
        return last.size();
    }

    Count CastImpl(short ray_index) override {
        auto& mirs = *mirrors_;
        history_casts_.Push({items_[ray_index].row, items_[ray_index].col}, ray_index);

        Ray ray = NextFromBorder(ray_index);
        Count count = 0;
        while (ray.pos >= ray_direction_.size()) {
            Destroy(items_[ray.pos].row, items_[ray.pos].col);
            DestroyLinks(ray.pos);
            ray = NextFromMirror(ray, mirs(items_[ray.pos].row, items_[ray.pos].col));
            ++count;
        }
        empty_space_ += count;
        filled_space_ -= count;
        mirrors_destroyed_ += count;
        return count;
    }

    void Restore() override {
        auto& last = *buffer_;
        auto& mirs = *mirrors_;

        mirrors_destroyed_ -= last.size();
        while (!last.empty()) {
            char r = items_[last.back()].row;
            char c = items_[last.back()].col;
            mirs(r, c) -= kMirOffset;
            Restore(r, c);
            last.pop_back();
        }
    }

    void Destroy(char row, char col) {
        if (--mirrors_left_[kOrientHor][col] == 0) {
            ++empty_row_count_;
            // empty is not counted as even?????
        }

        if (--mirrors_left_[kOrientVer][row] == 0) {
            ++empty_col_count_;
        }
        hash_.HashOut({row, col});
    }

    void DestroyLinks(short index) {
        auto& ns = items_[index].ns;
        items_[ns[kDirTop]].ns[kDirBottom] = ns[kDirBottom];
        items_[ns[kDirBottom]].ns[kDirTop] = ns[kDirTop];
        items_[ns[kDirLeft]].ns[kDirRight] = ns[kDirRight];
        items_[ns[kDirRight]].ns[kDirLeft] = ns[kDirLeft];
    }

    void Restore(char row, char col) {
        if (++mirrors_left_[kOrientHor][col] == 1) {
            --empty_row_count_;
        }
        if (++mirrors_left_[kOrientVer][row] == 1) {
            --empty_col_count_;
        }
        hash_.HashIn({row, col});
    }


    void Reduce(vector<short>& shift) {
        Reduce();
        auto& offset = *buffer_;
        for (auto& s : shift) {
            s -= offset[s];
        }
    }

    // 4 * Number of items
    void Reduce() override {
        auto& offset = *buffer_;
        offset.resize(items_.size());
        auto cur = 0;
        for (auto i = 0; i < ray_direction_.size(); ++i) {
            if (IsEmptyLine(i)) {
                ++cur;
            }
            offset[i] = cur;
        }
        for (auto i = ray_direction_.size(); i < items_.size(); ++i) {
            if (items_[items_[i].ns[kDirTop]].ns[kDirBottom] != i) {
                ++cur;
            }
            offset[i] = cur;
        }
        if (offset[0] == 0) {
            short& p = items_[0].ns[ray_direction_[0]];
            p -= offset[p];
        }
        for (auto i = 1; i < ray_direction_.size(); ++i) {
            if (offset[i-1] != offset[i]) {
                // increased cur on i pos: deleted element
                continue;
            }
            short& p = items_[i].ns[ray_direction_[i]];
            p -= offset[p];
            items_[i - offset[i]] = items_[i];
            ray_direction_[i - offset[i]] = ray_direction_[i];
        }
        for (auto i = ray_direction_.size(); i < items_.size(); ++i) {
            if (offset[i-1] != offset[i]) {
                continue;
            }
            auto& ns = items_[i].ns;
            for (int q = 0; q < 4; ++q) {
                ns[q] -= offset[ns[q]];
            }
            items_[i - offset[i]] = items_[i];
        }
        // now resize both vectors
        auto last = ray_direction_.size()-1;
        ray_direction_.resize(last + 1 - offset[last]);
        last = items_.size()-1;
        items_.resize(last + 1 - offset[last]);

        empty_space_ = 0;
        filled_space_ = items_.size() - ray_direction_.size();
    }

    bool IsEmptyLine(short ray_index) {
        return NextFromBorder(ray_index).pos < RayCount();

    }

    bool AllDestroyed() const override {
        return EmptyLinesCount() == 2 * board_size_;
    }

    Count size() const override {
        return board_size_;
    }

    Count RayCount() const override {
        return ray_direction_.size();
    }

    Count MirrorsDestroyed() const override {
        return mirrors_destroyed_;
    }

    Count EmptyLinesCount() const override {
        return EmptyColCount() + EmptyRowCount();
    }

    Count EmptyRowCount() const {
        return empty_row_count_;
    }

    Count EmptyColCount() const {
        return empty_col_count_;
    }

    HashType hash() const override {
        return hash_.hash();
    }

    Count EmptySpace() const override {
        return empty_space_;
    }

    Count TotalSpace() const override {
        return items_.size();
    }

    Count CastCount() const override {
        return history_casts_.Count();
    }

    vector<Position> CastHistory() const override {
        return ToVector(history_casts_);
    }

private:

    Ray NextFromMirror(const Ray& ray, char mir) const {
        Direction dir = kDirReflection[mir][ray.dir];
        return {items_[ray.pos].ns[dir], dir};
    }

    Ray NextFromBorder(short ray_index) const {
        Direction dir = ray_direction_[ray_index];
        return {items_[ray_index].ns[dir], dir};
    }

    // same as for FromBorder
    Ray NextFromEmpty(const Ray& ray) const {
        return {items_[ray.pos].ns[ray.dir], ray.dir};
    }


    Count board_size_;
    Count mirrors_destroyed_;
    Count empty_row_count_ = 0;
    Count empty_col_count_ = 0;

    Count filled_space_;
    Count empty_space_;

    BoardHash hash_;

    vector<Item> items_;
    // they are first in items
    // where is ray directed
    vector<Direction> ray_direction_;
    array<vector<char>, 2> mirrors_left_;

    shared_ptr<Mirrors> mirrors_;
    CastHistory_Nodes_v2 history_casts_;
    // use for reduce and restore
    shared_ptr<vector<short>> buffer_;

};

const array<array<char, 4>, 2> Board_v6::kDirReflection = { {
                                                            // kMirRight
                                                            { {
                                                              kDirLeft,  // to top
                                                              kDirRight,   // to bottom
                                                              kDirTop, // to left
                                                              kDirBottom     // to right
                                                              } },
                                                            // kMirLeft
                                                            { {
                                                              kDirRight,   // to top
                                                              kDirLeft,  // to bottom
                                                              kDirBottom,    // to left
                                                              kDirTop  // to right
                                                              } }
                                                            } };

// END BOARD_V6

// BEAM_SEARCH

// has to keep ScoreType as template parameter to support
// score functions that require specific board class as argument
template<class BoardType, class ScoreType>
class BeamSearch {

    using HashType = typename BoardType::HashType;
    using CastType = typename BoardType::CastType;

    /// can make use of more parametered possibly
    struct Derivative {
        Derivative() {}
        Derivative(BoardType* b, const CastType& p, HashType h, double s)
        : origin(b), cast(p), hash(h), score(s) {}

        BoardType* origin;
        CastType cast;
        HashType hash;
        double score;

        /// want to sort in reverse order
        bool operator<(const Derivative& d) const {
            return score > d.score;
        }
    };

public:

    BoardType Destroy(const BoardType& b_in) {
        unordered_set<HashType> visited;
        vector<Derivative> derivs;
        vector<BoardType> b_0;
        vector<BoardType> b_1;
        b_0.reserve(beam_width_);
        b_1.reserve(beam_width_);
        Count side_count = 4;
        derivs.reserve(beam_width_*side_count*b_in.size());
        auto cur = &b_0;
        auto next = &b_1;
        cur->push_back(b_in);
        while (true) {
            for (auto& b : *cur) {
                Count d_was = b.MirrorsDestroyed();
                auto func = [&](CastType c) {
                    Count d_now = b.MirrorsDestroyed();
                    if (d_now > d_was && visited.count(b.hash()) == 0) {
                        visited.insert(b.hash());
                        derivs.emplace_back(&b, c, b.hash(), score_(b));
                    }
                };
                b.ForEachAppliedCast(func);
            }
            Count sz = min<Count>(beam_width_, derivs.size());
            nth_element(derivs.begin(), derivs.begin()+sz-1, derivs.end());
            derivs.resize(sz);
            next->resize(sz);
            for (Index i = 0; i < sz; ++i) {
                (*next)[i] = *(derivs[i].origin);
                (*next)[i].Cast(derivs[i].cast);
            }
            swap(cur, next);
            auto rr = max_element(cur->begin(), cur->end(), [] (const BoardType& b_0, const BoardType& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (rr->AllDestroyed()) {
                return *rr;
            }
            /// cleanup before next step
            next->clear();
            derivs.clear();
            visited.clear();
        }
    }

    void set_beam_width(Count beam_width) {
        beam_width_ = beam_width;
    }

    Count beam_width_;
    ScoreType score_;
};

// END BEAM_SEARCH

inline std::vector<int> ToSolution(const std::vector<Position>& ps) {
    vector<int> res;
    res.reserve(2*ps.size());
    for (auto& p : ps) {
        res.push_back(p.row);
        res.push_back(p.col);
    }
    return res;
}

class FragileMirrors {
public:
    std::vector<int> destroy(const std::vector<std::string>& board);
};

std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    BeamSearch<Board_v6, Score_v1> solver;
    solver.set_beam_width(500.*pow(100./board.size(), 2));
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}