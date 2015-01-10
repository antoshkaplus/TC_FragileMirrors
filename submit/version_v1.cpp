//
//  version_v1.cpp
//  
//
//  Created by Anton Logunov on 1/10/15.
//
//


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
    
    
    enum struct Enabler {}; 
    constexpr Enabler enabler = {};
    
    // need to avoid usage with one or 0 parameters sometimes
    
    template<typename... Nothing> 
    struct All {
        static constexpr bool value = true;
    };   
    template<typename Condition, typename... OtherConditions>
    struct All<Condition, OtherConditions...> {
        static constexpr bool value = Condition::value && All<OtherConditions...>::value;
    };
    
    template<typename... Nothing> 
    struct Any {
        static constexpr bool value = true;
    };   
    template<typename Condition, typename... OtherConditions>
    struct Any<Condition, OtherConditions...> {
        static constexpr bool value = Condition::value || All<OtherConditions...>::value;
    };
    
    
    template<typename Condition>
    using EnableIf = typename std::enable_if<Condition::value, Enabler>::type;
    
    template<typename Condition, typename... OtherConditions>
    using EnableIfAll = EnableIf<All<Condition, OtherConditions...>>;
    
    template<typename Condition, typename... OtherConditions>
    using EnableIfAny = EnableIf<Any<Condition, OtherConditions...>>;
    
    
    template<typename... Nothing>
    struct IsAnySame {
        static constexpr bool value = false;
    };
    template<typename Type, typename Another, typename... Other>
    struct IsAnySame<Type, Another, Other...> {
        static constexpr bool value = std::is_same<Type, Another>::value || IsAnySame<Type, Other...>::value;
    };
    
    
    
    template<class Key, class Value>
    std::tuple<std::vector<Key>, std::vector<Value>> Zip(std::map<Key, Value>& m) {
        std::tuple<std::vector<Key>, std::vector<Value>> r;
        auto& keys = std::get<0>(r);
        auto& values = std::get<1>(r);  
        keys.reserve(m.size());
        values.reserve(m.size());
        for (auto& p : m) {
            keys.push_back(p.first);
            values.push_back(p.second);
        }
        return r;
    }
    
    // sometimes someone can use with Long, not just Int type
    template<class T>
    class Range {
    public:
        class Iterator : std::iterator<std::input_iterator_tag, T> {
        public:
            Iterator(const Range& range, T current) 
            : range_(range), current_(current) {
                // just copied that lol
                if (range_.step_*(current_-range_.last_) > 0) current_ = range_.last_;
            }
            const T operator*() const { return current_; }
            bool operator==(const Iterator& it) const {
                return current_ == *it;
            }
            bool operator!=(const Iterator& it) const {
                return current_ != *it;
            }
            Iterator& operator++() {
                current_ += range_._step;
                if (range_.step_*(current_-range_.last_) > 0) current_ = range_.last_;
                return *this;
            }
            Iterator operator++(int) { 
                Iterator it(*this); 
                operator++(); 
                return it;
            }
        private:
            const Range& range_;
            T current_;
        };
        
        friend class Iterator;
        
        Range(T last) : first_(0), last_(last), step_(1) {}
        Range(T first, T last, T step = 1)
        : first_(first), last_(last), step_(step) {}
        
        Iterator begin() const { return Iterator(*this, first_); }
        Iterator end()   const { return Iterator(*this, last_); }
        
    private:
        T first_, last_, step_;
    };
    
    
    
    class DisjointSet {
    public:
        DisjointSet() {}
        DisjointSet(Count element_count) {
            Init(element_count);
        }
        
        void Init(Count element_count) {
            element_count_ = element_count;
            set_count_ = element_count;
            data_.resize(element_count);
            size_.resize(element_count);
            Reset();
        }
        
        void Reset() {
            std::iota(data_.begin(), data_.end(), 0);
            fill(size_.begin(), size_.end(), 1);
        }
        
        void Unite(Index i_0, Index i_1) {
            --set_count_;
            Index
            r_0 = root(i_0),
            r_1 = root(i_1);
            // will join r_0 to r_1, so r_1 height should be bigger
            if (size_[r_0] > size_[r_1]) {
                std::swap(r_0, r_1);
            }
            data_[r_0] = r_1;
            size_[r_1] += size_[r_0];
            
        }
        
        bool is_separate(Index i_0, Index i_1) {
            return root(i_0) != root(i_1);
        }
        
        Index root(Index i) {
            while (i != data_[i]) {
                i = data_[i] = data_[data_[i]];
            }
            return i;
        }
        
        size_t size() {
            return element_count_;
        }
        
        Count set_count() {
            return set_count_;
        }
        
    private:
        Count element_count_;
        Count set_count_;
        std::vector<Index> data_;
        // how many elements in set with index, if index is root
        std::vector<size_t> size_;
    }; 
    
    
    
    
    //struct discrete_distribution {
    //
    //    template<class ForwardIterator>
    //    discrete_distribution(ForwardIterator first, ForwardIterator last) {
    //        weight_.assign(first, last);
    //        cumulative_.resize(weight_.size());
    //        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
    //        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
    //    }
    //    discrete_distribution(const std::initializer_list<double>& weights) 
    //    : discrete_distribution(weights.begin(), weights.end()) {}
    //
    //    void set_weight(Index i, double w) {
    //        assert(w >= 0);
    //        //            auto d = w-weight_[i];
    //        //            for (auto k = i; k < weight_.size(); ++k) {
    //        //                cumulative_[k] += d;
    //        //            }
    //        //            
    //        weight_[i] = w;
    //        std::fill(cumulative_.begin(), cumulative_.end(), 0);
    //        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
    //        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
    //    }
    //
    //    double get_weight(Index i) {
    //        return weight_[i];
    //    }
    //
    //    template<class RNG> 
    //    Index operator()(RNG& rng) {
    //        Index i = std::lower_bound(cumulative_.begin(), cumulative_.end(), uni_(rng))-cumulative_.begin();
    //        if (cumulative_.back() != 0.) while ( weight_[i] == 0.) --i;
    //        return i;
    //    }
    //
    //    std::uniform_real_distribution<double> uni_;
    //    std::vector<double> cumulative_;
    //    std::vector<double> weight_;
    //};
    
    
    
    // current stack supports iteration!
    template<class T>
    class Stack {
    public:
        // can't inherit from vector iterator
        // too open class
        class ConstIterator : std::iterator<std::input_iterator_tag, T> {
        public:
            ConstIterator(const Stack& stack, typename std::vector<T>::const_iterator current) 
            : stack_(stack), current_(current) {}
            const T& operator*() const { return *current_; }
            bool operator==(const ConstIterator& it) const {
                return current_ == it;
            }
            bool operator!=(const ConstIterator& it) const {
                return current_ != it.current_;
            }
            ConstIterator& operator++() {
                ++current_;
                return *this;
            }
            // post iterator
            ConstIterator operator++(int) { 
                ConstIterator it(*this); 
                operator++(); 
                return it;
            }
        private:
            const Stack& stack_;
            typename std::vector<T>::const_iterator current_;
        };
        
        friend class ConstIterator;
        
        ConstIterator begin() const { return ConstIterator(*this, data_.begin()); }
        ConstIterator end()   const { return ConstIterator(*this, data_.end()); }
        
        T& top() {
            return data_.back();
        }
        void push(const T& val) {
            data_.push_back(val);
        }
        void pop() {
            data_.pop_back();
        }
        bool empty() const {
            return data_.empty();
        }
        size_t size() const {
            return data_.size();
        }
        
    private:
        std::vector<T> data_;
    };
    
    
    
    // probably should be an inheritance
    template <class T>
    class CountMap : public std::map<T, Count> {
    public:
        void decrease(const T& t) { decrease(t, 1); }
        
        void decrease(const T& t, Count val) {
            auto it = this->find(t);
            if ((it->second-=val) == 0) {
                this->erase(it);
            }
        }
        
        void increase(const T& t) { increase(t, 1); }
        
        void increase(const T& t, Count val) {
            this->emplace(t, 0).first->second+=val;
        }
        
        std::set<T> keys() const {
            std::set<T> r;
            for (auto p : *this) {
                r.insert(p.first);
            }
            return r;
        }
        
        size_t get(const T& t) const {
            auto it = this->find(t);
            return it == this->end() ? 0 : it->second;
        }
    };
    
    template<class T>
    class CircularList {
    private:
        struct Node {
            T value;
            Node* prev;
            Node* next;
        };
        
        template<class V>
        struct BaseIterator : std::iterator<std::bidirectional_iterator_tag, V> {
            BaseIterator(Node* n) : node_(n) {} 
            
            V& operator*() const { return node_->value; }
            V* operator->() const { return node_->value; }
            
            bool operator==(const BaseIterator& it) const {
                return node_ == it.node_;
            }
            bool operator!=(const BaseIterator& it) const {
                return node_ != it.node_;
            }
            BaseIterator& operator++() {
                node_ = node_->next;
                return *this;
            }
            // post iterator
            BaseIterator operator++(int) { 
                BaseIterator it(node_); 
                node_ = node_->next; 
                return it;
            }
            
            BaseIterator operator--() {
                node_ = node_->prev;
                return *this;
            }
            BaseIterator operator--(int) {
                BaseIterator it(node_);
                node_ = node_->prev;
                return it;
            }
            
        private:
            Node* node_;
            friend struct circular_list;
        };
        
    public:
        using Iterator = BaseIterator<T>;
        using ConstIterator = BaseIterator<const T>;
        
        CircularList() : focus_(nullptr) {}
        
        void InitFocus(const T& value) {
            focus_ = new Node();
            focus_->value = value;
            focus_->prev = focus_->next = focus_;
        }
        
        template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
        It InsertAfter(It it_pos, const T& value) {
            ++count_;
            Node *pos = it_pos.node_;
            if (pos == nullptr) {
                init_focus(value);
                return Iterator(focus_);
            }
            Node *prev = pos;
            Node *next = pos->next;
            Node *cur = new Node();
            cur->next = next;
            cur->prev = prev;
            cur->value = value;
            next->prev = cur;
            prev->next = cur;
            return It(cur);
        }
        
        template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
        It InsertBefore(Iterator it_pos, const T& value) {
            ++count_;
            Node *pos = it_pos.node_;
            if (pos == nullptr) {
                init_focus(value);
                return Iterator(focus_);
            }
            Node *prev = pos->prev;
            Node *next = pos;
            Node *cur = new Node();
            cur->next = next;
            cur->prev = prev;
            cur->value = value;
            prev->next = cur;
            next->prev = cur;
            return It(cur);
        }
        
        template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
        void Erase(It it_pos) {
            --count_;
            Node *pos = it_pos.node_;
            if (pos == focus_) focus_ = pos->next;
            pos->prev->next = pos->next;
            pos->next->prev = pos->prev;
            delete pos;
            if (count_ == 0) focus_ = nullptr;
        }
        
        Iterator focus() {
            return Iterator(focus_);
        }
        
        ConstIterator focus() const {
            return ConstIterator(focus_);
        }
        
        Count size() const {
            return count_;
        }
        
        bool is_empty() const {
            return focus_ == nullptr;
        }
        
    private:
        Node* focus_;
        Count count_ = 0;
    };
    
    
    
    std::map<std::string, std::string> command_line_options(const char* argv[], int argc);
    
    int atoi(char* str, Int n);
    int atoi(char* first, char *last);
    
    
    struct command_line_parser {
        command_line_parser(const char* argv[], int argc) {
            options_ = command_line_options(argv, argc);
        }
        
        bool exists(const std::string& option) const {
            return options_.find(option) != options_.end();
        }
        
        bool hasValue(const std::string& option) const {
            return options_.at(option) != "";
        }
        
        std::string getValue(std::string option) const {
            std::string value = options_.at(option);
            if (value == "") {
                throw std::logic_error("command line option has no value");
            }
            return value;
        }
    private:
        std::map<std::string, std::string> options_;
    };
    
    
    // let it be unsigned char, int or long
    template<class T>
    struct CombinationGenerator {
        // should be like iterator
        
        static const Int kByteBitCount = 8;
        static const Int kMaxElementCount = sizeof(T)*kByteBitCount;
        
        struct Tails : std::array<T, kMaxElementCount> {
            using std::array<T, kMaxElementCount>::at;
            
            Tails() {
                at(0) = 1;
                init(1);
            }
            
            void init(Int i) {
                if (i == kMaxElementCount) return;
                at(i) = at(i-1) << 1;
                at(i) += 1;
                init(i+1);
            } 
        };
        
        static constexpr Tails tails = Tails();
        
        CombinationGenerator(Int selection_count, Int element_count, T starting_combination = 0)
        : selection_count_(selection_count), element_count_(element_count), data_(starting_combination) {
            if (data_ == 0) {
                data_ = tails[selection_count-1];
            }
        }
        
        const T& next() {
            Int i = 0;
            Int k = 0; // how many elements behind
            while (i < element_count_) {
                
                if ((data_ >> i) && 1) {
                    if (!((data_ >> (i+1)) & 1)) {
                        //data_ |= (1 << i+1);
                        data_ >>= i+1;
                        data_ += 1;
                        data_ <<= i+1;
                        data_ |= tails[k-1];
                        break;
                    }
                    ++k;
                }
                // can shift father
            }
            return data_;
        } 
        
        bool hasNext() {
            return data_ != (tails[selection_count_-1] << (element_count_ - selection_count_)); 
        }
        
        T data_;
        Int element_count_;
        Int selection_count_;
    };
    
    template<class T>
    struct BinomialHeap {
        const T& min() {
            auto& x = data_[0];
            auto* min = data_;
            while (x < data_.size()) {
                if (data_[x] < min) {}
            } 
        }
        std::vector<T> data_;
    };
    
    
    
    template<class T>
    struct bst_set {
    public:
        
        bst_set() : size_(0), root_(0) {}
        
        virtual ~bst_set() {
            clear();
        }
        
        void clear() {
            node::clear(root_);
            root_ = nullptr;
            size_ = 0;
        }
        
        size_t size() const {
            return size_;
        }
        
        
    private:
        
        struct node {
            
            node(const T& t) : value_(t) {}
            
            // well if you call on null node it's your problem
            static node* next(node* n) {
                if (exists(right(n))) {
                    n = right(n);
                    while (exists(left(n))) {
                        n = left(n);
                    }
                    return n;
                }
                node* n_2 = parent(n);
                if (!exists(n_2)) return nullptr;
                
                if (left(n_2) == n) {
                    return n_2;
                } else {
                    // n right child
                    // will return nullptr if can't find anything
                    while (exists(left(n_2)) && left(n_2) != n) {
                        n = n_2;
                        n_2 = parent(n_2);
                    }
                    return n_2;
                }
            }
            
            static node* prev(node* n) {
                // like next but change left and right functions
                if (exists(left(n))) {
                    n = left(n);
                    while (exists(right(n))) {
                        n = right(n);
                    }
                    return n;
                }
                node* n_2 = parent(n);
                if (!exists(n_2)) return nullptr;
                
                if (right(n_2) == n) {
                    return n_2;
                } else {
                    // n right child
                    // will return nullptr if can't find anything
                    while (exists(right(n_2)) && right(n_2) != n) {
                        n = n_2;
                        n_2 = parent(n_2);
                    }
                    return n_2;
                }
                
            }
            
            // min element in subtree
            // can't be called will nullptr argument
            static node* min(node* n) {
                while (exists(left(n))) {
                    n = left(n);
                }
                return n;
            }
            
            static node* max(node* n) {
                while (exists(right(n))) {
                    n = right(n);
                }
                return n;
            }
            
            static void substitute_child(node* parent, node* child, node* substitution) {
                if (substitution != nullptr) substitution->parent_ = parent;
                if (left(parent) == child) {
                    parent->left_ = substitution;
                } else {
                    parent->right_ = substitution;
                }
            }
            
            
            
            static node* right(node* n) {
                return n->right_;
            }
            static node* parent(node* n) {
                return n->parent_;
            }
            static node* left(node* n) {
                return n->left_;
            }
            static bool exists(node* n) {
                return n != nullptr;
            }
            
            // clears whole subtree
            static void clear(node* n) {
                if (!exists(n)) return;
                // need to find out is right or left child then null everything
                if (exists(parent(n))) {
                    if (n->parent_->left == n) n->parent_->left = nullptr;
                    else n->parent_->right = nullptr;
                    n->parent_ = nullptr;
                }
                // now just clear separate tree with n
                while (n != nullptr) {
                    if (exists(left(n))) {
                        n = left(n);
                    } else if (exists(right(n))) {
                        n = right(n);
                    } else {
                        node* n_old = n;
                        // if nullptr will finish
                        n = parent(n);
                        delete n_old;
                    }
                }
            }
            
            node* right_    = nullptr;
            node* left_     = nullptr;
            node* parent_   = nullptr; 
            T value_;
        }; 
        
    public:    
        
        struct iterator : std::iterator<std::bidirectional_iterator_tag, node> {
            
            iterator(node* node) : current_(node) {}
            iterator() {}
            
            const T& operator*() const { 
                return current_->value;  
            } 
            
            bool operator==(const iterator& it) {
                return current_ == it.current_; 
            }
            bool operator!=(const iterator& it) {
                return current_ != it.current_;
            }
            
            // pred
            iterator& operator++() {
                current_ = node::next(current_);
                return *this;
            }
            // post
            iterator operator++(int) { 
                iterator it(current_);
                current_ = node::next(current_); 
                return it;
            }
            
        private:
            node* current_;
        };
        
        
        iterator begin() const {
            if (root_ == nullptr) return end();
            node* b = root_;
            while (node::exists(node::left(b))) {
                b = node::left(b);
            }
            return iterator(b);
        } 
        
        // probably should just return nullptr as node
        iterator end() const {
            return iterator(nullptr);
        }
        
        iterator find(const T& t) const {
            node* n = root_; 
            while (n != nullptr) {
                if (n->value_ < t) {
                    n = n->right_;
                    continue;
                }
                if (t < n->value_) {
                    n = n->left_;
                    continue;
                }
                return iterator(n);
            }
            return end();
        }
        
        bool exists(const T& t) const {
            return find(t) != end();
        }
        
        // like find but should keep track of element parent
        std::pair<iterator, bool> insert(const T& t) {
            node* n_new = new node(t); 
            if (root_ == nullptr) {
                root_ = n_new;
                size_ = 1;
                return {begin(), true};
            }
            node* n = root_;
            while (true) {
                if (n->value_ < t) {
                    if (n->right_ == nullptr) {
                        break;
                    }
                    n = n->right_;
                    continue;
                }
                if (t < n->value_) {
                    if (n->left_ == nullptr) {
                        break;
                    }
                    n = n->left_;
                    continue;
                }
                return {iterator(n), false};
            }
            // now should have not null parent
            n_new->parent_ = n;
            if (t < n->value_) {
                n->left_ = n_new;
            } else {
                // equality could not be
                n->right_ = n_new;
            }
            ++size_; 
            return {iterator(n_new), true};
        }
        
        // should return iterator on next element or end
        void erase(const T& t) {
            erase(find(t));
        }
        
        void erase(iterator it) {
            if (it == end()) return;
            --size_;
            node* n = it.current_;
            // what happens if n is root???
            if (n->right_ == nullptr && n->left_ == nullptr) {
                // ??? root_ goes to nullptr 
                
                node::substitute_child(n->parent_, n, nullptr);
                delete n;
                return;
            }
            if (n->right_ == nullptr && n->left_ != nullptr) {
                // ??? root_ goes to child
                
                node::substitute_child(n->parent_, n, n->left_);
                delete n;
                return;
            } 
            if (n->left_ == nullptr && n->right_ != nullptr) {
                // ??? root_ goes to child
                
                node::substitute_child(n->parent_, n, n->right_);
                delete n;
                return;
            }
            // ok, both children presents
            // ??? root_ goes to child
            if (std::uniform_int_distribution<>(0, 1)(rng_) == 0) {
                // first left to right
                node::substitute_child(n->parent_, n, n->right_);
                node* p = node::min(n->right_);
                n->left_->parent_ = p;
                p->left_ = n->left_;
            } else { 
                // second right to left
                node::substitute_child(n->parent_, n, n->left_);
                node* p = node::max(n->left_);
                n->right_->parent_ = p;
                p->right_ = n->right_;
            }
            delete n;
        }
        
        private :
        
        // can use in find and insert
        iterator find_closest(const T& t) {
            return end();
        }
        
        std::default_random_engine rng_{(unsigned)std::chrono::system_clock::now().time_since_epoch().count()};
        size_t size_;
        node* root_;
    };
    
}
namespace ant { 
    
    std::map<std::string, std::string> command_line_options(const char* argv[], int argc) {
        std::map<std::string, std::string> res;
        for (Index i = 0; i < argc; ++i) {
            if (argv[i][0] == '-') {
                std::string key(argv[i]+1);
                res[key] = "";
                if (i+1 < argc && argv[i+1][0] != '-') {
                    res[key] = std::string(argv[i+1]);
                    ++i;
                } 
            }
        }
        return res;
    } 
    
    int atoi(char* str, Int n) {
        return atoi(str, str+n);
    }
    
    int atoi(char* first, char *last) {
        char ch = *last;
        *last = '\0';
        int r = std::atoi(first);
        *last = ch;
        return r;
    }
    
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
                    return width*height;
                }
                T perimeter() const {
                    return 2*(height + width); 
                }
                bool isCovering(const Size<T>& s) const {
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
            bool operator==(const Size<T>& s_0, const Size<T>& s_1) {
                return s_0.width == s_1.width && s_0.height == s_1.height;
            }
            template<class T>
            bool operator!=(const Size<T>& s_0, const Size<T>& s_1) {
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
                    Segment(const Point& fst, const Point& snd)
                    : fst(fst), snd(snd) {}
                    Point fst, snd;
                    
                    void swap() {
                        std::swap(fst, snd);
                    }
                    
                    Segment swapped() const {
                        return Segment(snd, fst);
                    }
                };
                
                bool operator==(const Point& p_0, const Point& p_1);
                bool operator!=(const Point& p_0, const Point& p_1);
                
                struct Rectangle {
                    Rectangle() {}
                    Rectangle(const Point& origin, const Size& size) 
                    : origin(origin), size(size) {}
                    Rectangle(Int x, Int y, Int width, Int height) 
                    : origin(x, y), size(width, height) {}
                    
                    void set(Int x, Int y, size_t width, size_t height) {
                        origin.set(x, y);
                        size.set(width, height);
                    }
                    
                    void set(const Point& origin, const Point& diag) {
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
                    
                    bool isIntersect(const Rectangle& r) const {
                        return origin.x < r.origin.x + r.size.width  && origin.x + size.width  > r.origin.x &&
                        origin.y < r.origin.y + r.size.height && origin.y + size.height > r.origin.y;
                    }
                    
                    Point origin;
                    Size size;
                };
                
                bool operator==(const Rectangle& r_0, const Rectangle& r_1);
                bool operator!=(const Rectangle& r_0, const Rectangle& r_1);    
                
                // oab
                // Returns a positive value, if p_0, p_1, p_2 makes a counter-clockwise turn,
                // negative for clockwise turn, and zero if the points are collinear.
                static int cross_product(const Point& p_0, const Point& p_1, const Point& p_2) {
                    return (p_1.x - p_0.x)*(p_2.y - p_0.y) - (p_1.y - p_0.y)*(p_2.x - p_0.x);
                }
                
                std::vector<Index> convex_hull(const std::vector<Point>& ps, bool is_clockwise);    
                
            } // namespace i
            
            
            namespace f {
                
                struct Point {
                    Point() : Point(0, 0) {}
                    Point(Float x, Float y) : x(x), y(y) {}
                    
                    Float distance(const Point& p) const {
                        Float 
                        dx = p.x - x,
                        dy = p.y - y;
                        return sqrt(dx*dx + dy*dy);
                    } 
                    
                    static Float distance(const Point& p_0, const Point& p_1) {
                        return p_0.distance(p_1);
                    }
                    
                    void set(Float x, Float y) {
                        this->x = x;
                        this->y = y;
                    }
                    
                    Float x, y;
                };
                
                struct Indent {
                    Indent() : Indent(0, 0) {}
                    Indent(Float dx, Float dy) : dx(dx), dy(dy) {}
                    Indent& operator+=(const Indent& d) {
                        dx += d.dx;
                        dy += d.dy;
                        return *this;
                    }
                    double distance() {
                        return sqrt(dx*dx + dy*dy);
                    }
                    
                    Float dx, dy;
                };
                
                using Size = d2::Size<double>;
                
                struct Line {
                    Line() : Line(0, 0, 0) {}
                    Line(double a, double b, double c) : a(a), b(b), c(c) {}
                    Line(const Point& p_0, const Point& p_1) {
                        a = p_1.y - p_0.y;
                        b = p_0.x - p_1.x;
                        c = p_0.x*(p_0.y - p_1.y) + p_0.y*(p_1.x - p_0.x);
                    }
                    
                    double a, b, c;
                };
                
                struct Circle {
                    Circle() : radius(0) {}
                    Circle(Point center, double radius) : center(center), radius(radius) {}
                    Point center;
                    double radius;
                };
                
                struct Rectangle {
                    Rectangle() : origin(0, 0), size(0, 0) {}
                    Rectangle(Float x, Float y, Float width, Float height) 
                    : origin(x, y), size(width, height) {}
                    Rectangle(Point origin, Size size) 
                    : origin(origin), size(size) {}
                    
                    bool isInside(const Point& p) const {
                        return p.x >= origin.x && p.y >= origin.y && 
                        p.x <= origin.x+size.width && p.y <= origin.y+size.height;
                    }
                    
                    Point origin;
                    Size size; 
                };
                
                bool operator==(const Point& p_0, const Point& p_1);
                Indent operator-(const Point& p_0, const Point& p_1);
                Point operator+(const Point& p_0, const Point& p_1);            
                Indent operator/(const Indent& ind, Float d);   
                Point operator/(const Point& p, Float d);         
                Indent operator*(Float d, const Indent& ind);
                std::ostream& operator<<(std::ostream& output, const Point& p);
                
                std::pair<Point, Point> circleLineIntersection(const Circle& circle, const Line& line);
                
            } // namespace f
            
            
            
        } // namespace d2
        
    } // namespace geometry
    
} // namespace ant

#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <list>
#include <bitset>
#include <vector>
#include <random>
#include <limits>



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
                return row*col;
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
                return row*col;
            }
            
            Size swapped() const {
                return Size(col, row);
            }
            
            void swap() {
                std::swap(row, col);
            }
            
            Int row, col;
        };
        
        bool operator==(const Size& s_0, const Size& s_1);
        
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
                bool operator()(const Position& p_0, const Position& p_1) {
                    return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
                }
            };
            struct BottomRightComparator {
                bool operator()(const Position& p_0, const Position& p_1) {
                    return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
                }
            };
        };   
        
        
        bool operator==(const Size& s_0, const Size& s_1) {
            return s_0.row == s_1.row && s_0.col == s_1.col;
        }
        
        Position operator-(const Position& p, const Indent& n) {
            return {p.row - n.row, p.col - n.col};
        }
        
        Position operator+(const Position& p, const Indent& n) {
            return {p.row + n.row, p.col + n.col};
        }        
        
        Position operator+(const Position& p, const Position& n) {
            return {p.row + n.row, p.col + n.col};
        } 
        
        bool operator==(const Position& p_0, const Position& p_1) {
            return p_0.row == p_1.row && p_0.col == p_1.col;
        }        
        
        bool operator!=(const Position& p_0, const Position& p_1) {
            return !(p_0 == p_1);
        }
        
        
        
        Position operator-(const Position& p, const Indent& n);
        Position operator+(const Position& p, const Indent& n);        
        bool operator==(const Position& p_0, const Position& p_1);        
        bool operator!=(const Position& p_0, const Position& p_1);
        
        struct Region {
            Region() : Region(0, 0, 0, 0) {}
            Region(Int row, Int col, Int row_count, Int col_count)
            : position(row, col), size(row_count, col_count) {}
            Region(const Position& p, const Size& d) 
            : position(p), size(d) {}
            
            void set(Int row, Int col, Int row_indent, Int col_indent) {
                position.set(row, col);
                size.set(row_indent, col_indent);
            }
            void set(const Position& p, const Size& d) {
                position = p;
                size = d;
            }
            
            void shift(Int row, Int col) {
                position.row += row;
                position.col += col;
            } 
            
            // maybe someone will find those getters useful 
            Int row_begin() const { return position.row; }
            Int row_end()   const { return position.row + size.row; }
            Int col_begin() const { return position.col; }
            Int col_end()   const { return position.col + size.col; }
            
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
            
            bool hasInside(const Position& p) const {
                return p.row >= position.row && 
                p.row  < position.row+size.row &&
                p.col >= position.col &&
                p.col  < position.col+size.col;
            }
            bool hasInside(const Region& region) const {
                return row_begin() <= region.row_begin() 
                && col_begin() <= region.col_begin()
                && row_end() >= region.row_end()
                && col_end() >= region.col_end();
            }
            
            Region intersection(const Region& r) const {
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
            
            bool hasIntersection(const Region& r) const {
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
                return size.row*size.col;
            }
            
            Position position;
            Size size;
        };
        
        std::ostream& operator<<(std::ostream& output, const Region& r);
        
        
        
        bool operator==(const Region& r_0, const Region& r_1) {
            return r_0.position == r_1.position && r_0.size == r_1.size;
        }
        bool operator==(const Region& r_0, const Region& r_1);
        
        
        
        // G - grid
        template<class G>
        struct GridView {
            
            using value_type = typename G::value_type;
            using const_reference = const value_type&;
        private:
            using real_type = typename std::conditional<std::is_const<G>::value, const value_type, value_type>::type;
        public:
            
            
            GridView(std::shared_ptr<G> grid, const Position& origin) : grid_(grid), origin_(origin) {}
            GridView() : origin_(0, 0) {}
            
            const_reference operator()(const Position& p) const {
                return (*grid_)(p + origin_);
            }
            const_reference operator()(Index row, Index col) const {
                return (*grid_)(row + origin_.row, col + origin_.col);
            }
            
            
            real_type operator()(const Position& p) {
                return (*grid_)(p + origin_);
            }
            real_type operator()(Index row, Index col) {
                return (*grid_)(row + origin_.row, col + origin_.col);
            }
            
            
            void set_origin(const Position& p) {
                origin_ = p;
            }
            
            const Size& size() const {
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
            
            
            value_type& operator()(const Position& p) {
                return grid_(p.row - origin_.row, p.col - origin_.col);
            }
            const value_type& operator()(const Position& p) const {
                return grid_(p.row - origin_.row, p.col - origin_.col);
            }
            
            value_type& operator[](const Position& p) {
                return grid_(p.row - origin_.row, p.col - origin_.col);
            }
            const value_type& operator[](const Position& p) const {
                return grid_(p.row - origin_.row, p.col - origin_.col);
            }
            
            value_type& operator()(Int row, Int col) {
                return grid_(row - origin_.row, col - origin_.col);
            }
            const value_type& operator()(Int row, Int col) const {
                return grid_(row - origin_.row, col - origin_.col);
            }
            
            
            G& grid() {
                return grid_;
            }
            
            const G& grid() const {
                return grid_;
            }
            
            void set_grid(G&& grid) {
                grid_ = std::move(grid);
            } 
            
            void set_grid(const G& grid) {
                grid_ = grid;
            }
            
            void set_origin(const Position& p) {
                origin_ = p;
            }
            
            Position& origin() {
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
            :   row_count_(row_count), 
            col_count_(col_count), 
            grid_(row_count*col_count) {}
            
            Grid(Count row_count, Count col_count, const T& value_type) 
            :   row_count_(row_count),
            col_count_(col_count),
            grid_(row_count*col_count, value_type) {}
            
            Grid(const Size& size) : Grid(size.row, size.col) {}
            Grid(std::initializer_list<std::vector<T>> list)
            :   Grid(list.size(), list.size() == 0 ? 0 : list.begin()->size()){
                auto b = list.begin();
                for (auto r = 0; r < row_count(); ++r, ++b) {
                    std::copy(b->begin(), b->end(), grid_.begin() + r*col_count());
                }
            }
            
            bool isInside(const Position& p) {
                return isInside(p.row, p.col);
            }
            bool isInside(Int row, Int col) {
                return row >= 0 && row < row_count_ && 
                col >= 0 && col < col_count_;
            }
            
            void resize(Count row_count, Count col_count) {
                row_count_ = row_count;
                col_count_ = col_count;
                grid_.resize(row_count*col_count);
            }
            void resize(const Size& size) {
                resize(size.row, size.col);
            }
            
            void fill(const T& t) {
                std::fill(grid_.begin(), grid_.end(), t);
            }
            
            Count row_count() const { return row_count_; }
            Count col_count() const { return col_count_; }
            Size size() const { return Size(row_count(), col_count()); } 
            Count cell_count() const { return row_count()*col_count(); } 
            
            T& operator()(const Position& p) {
                return grid_[p.row*col_count_ + p.col];
            }
            const T& operator()(const Position& p) const {
                return grid_[p.row*col_count_ + p.col];
            }
            
            T& operator[](const Position& p) {
                return grid_[p.row*col_count_ + p.col];
            }
            const T& operator[](const Position& p) const {
                return grid_[p.row*col_count_ + p.col];
            }
            
            T& operator()(Int row, Int col) {
                return grid_[row*col_count_ + col];
            }
            const T& operator()(Int row, Int col) const {
                return grid_[row*col_count_ + col];
            }
        private:
            Count row_count_, col_count_;
            std::vector<T> grid_;
            
            friend struct const_iterator;
            template<class U>
            friend bool operator==(const Grid<U>& g_0, const Grid<U>& g_1);
        };
        
        
        template<class T>
        bool operator==(const Grid<T>& g_0, const Grid<T>& g_1) {
            return g_0.row_count_ == g_1.row_count_ && g_0.grid_ == g_1.grid_;
        }
        template<class T>
        std::ostream& operator<<(std::ostream& output, const Grid<T>& g) {
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
            grid_(row_count*col_count) {
            }
            
            bool isInside(const Position& p) {
                return isInside(p.row, p.col);
            }
            bool isInside(Int row, Int col) {
                return row >= 0 && row < row_count_ && 
                col >= 0 && col < col_count_;
            }
            
            void resize(Count row_count, Count col_count) {
                row_count_ = row_count;
                col_count_ = col_count;
                grid_.resize(row_count*col_count);
            }
            
            void fill(const T& t) {
                std::fill(grid_.begin(), grid_.end(), t);
            }
            
            Count row_count() const { return row_count_; }
            Count col_count() const { return col_count_; }
            
            T& operator()(const Position& p) {
                return grid_[p.row*col_count_ + p.col];
            }
            const T& operator()(const Position& p) const {
                return grid_[p.row*col_count_ + p.col];
            }
            
            T& operator()(Int row, Int col) {
                return grid_[row*col_count_ + col];
            }
            const T& operator()(Int row, Int col) const {
                return grid_[row*col_count_ + col];
            }
        private:
            Count row_count_, col_count_;
            std::unordered_map<Index ,T> grid_;
        };
        
        
        // we think that top left is start  
        struct ParticleGrid {
            using Point = ant::geometry::d2::f::Point;
            using indent = ant::geometry::d2::f::Indent;
            
            // particles will be compared by address
            struct Particle {
                virtual bool intersects(const Particle& p) const = 0;
                // maybe to use position (but it can be confused with Position and left top angle)
                virtual Point center() const = 0;
            };
            
            ParticleGrid() {}
            ParticleGrid(
                         Float x_min, Float y_min, 
                         Float x_max, Float y_max,
                         Float max_particle_width, 
                         Float max_particle_height) {
                
                reset(x_min, y_min, x_max, y_max, max_particle_width, max_particle_height);
            }
            
            void clear() {
                for (auto row = 0; row < grid.row_count(); ++row) {
                    for (auto col = 0; col < grid.col_count(); ++col) {
                        grid(row, col).clear();
                    }
                }
            }
            
            void reset(Float x_min, Float y_min, 
                       Float x_max, Float y_max,
                       Float max_particle_width, 
                       Float max_particle_height) {
                clear();
                
                this->x_min = x_min;
                this->x_max = x_max;
                this->y_min = y_min;
                this->y_max = y_max;
                
                side_width = max_particle_width; // maybe need to add + some offset
                side_height = max_particle_height;
                
                Count 
                row_count = static_cast<Count>((x_max-x_min)/max_particle_width) + 1, 
                col_count = static_cast<Count>((y_max-y_min)/max_particle_height) + 1;
                grid.resize(row_count, col_count);
            }
            
            void add(const std::shared_ptr<Particle>& p) {
                grid(position(*p)).push_back(p);
            }
            
            // before you are going to change location of particle remove it from grid
            bool remove(const std::shared_ptr<Particle>& p) {
                // maybe will need to ... make equality operator
                // use find to produce 2 factor
                auto& list = grid(position(*p));
                auto it = find(list.begin(), list.end(), p);
                if (it == list.end()) return false;
                else {
                    list.erase(it);
                    return true;
                }
            }
            
            void relocate(std::shared_ptr<Particle>& p, geometry::d2::f::Point center) {
                
            }
            
            void shift(std::shared_ptr<Particle>& p, Indent indent) {
                
            }
            
            // input can be particle that aren't inside grid 
            // can overload with just one particle without shared ptr
            std::vector<std::shared_ptr<Particle>> intersections(const std::shared_ptr<Particle>& p) {
                std::vector<std::shared_ptr<Particle>> result;
                auto pp = position(*p);
                auto 
                r_first = std::max(0, pp.row-1),
                c_first = std::max(0, pp.col-1),
                r_c = static_cast<Int>(grid.row_count())-1, r_last = std::min(r_c, pp.row+1),
                c_c = static_cast<Int>(grid.col_count())-1, c_last = std::min(c_c, pp.col+1);
                for (auto row = r_first; row <= r_last; ++row) {
                    for (auto col = c_first; col <= c_last; ++col) {
                        for (auto& gp : grid(row, col)) {
                            if (gp == p || !p->intersects(*gp)) continue;
                            result.push_back(gp); 
                        }
                    }
                }
                return result;
            }
            
            bool hasIntersection(const std::shared_ptr<Particle>& p) {
                bool result = false;
                auto pp = position(*p);
                auto 
                r_first = std::max(0, pp.row-1),
                c_first = std::max(0, pp.col-1),
                r_c = static_cast<Int>(grid.row_count())-1, r_last = std::min(r_c, pp.row+1),
                c_c = static_cast<Int>(grid.col_count())-1, c_last = std::min(c_c, pp.col+1);
                for (auto row = r_first; row <= r_last; ++row) {
                    for (auto col = c_first; col <= c_last; ++col) {
                        for (auto& gp : grid(row, col)) {
                            if (gp == p || !p->intersects(*gp)) continue;
                            result = true;
                            goto finish; 
                        }
                    }
                }
            finish:
                return result;
            }
            
            bool hasInside(const Point p) {
                return p.x >= x_min && p.x <= x_max && p.y >= y_min && p.y <= y_max;
            }
            
        private:
            Position position(const Particle& p) {
                Point pt = p.center();
                Position pp{
                    static_cast<Int>((pt.y - y_min)/side_height), 
                    static_cast<Int>((pt.x - x_min)/side_width)
                };
                assert(!(pp.row < 0 || pp.col < 0 || pp.row >= grid.row_count() || pp.col >= grid.col_count()));
                return pp;
            } 
            
            Grid<std::list<std::shared_ptr<Particle>>> grid;
            double
            side_width, 
            side_height,
            x_min,
            x_max,
            y_min,
            y_max;
        };
        
        struct MaxEmptyRegionsFinder {
            MaxEmptyRegionsFinder() {}
            MaxEmptyRegionsFinder(Int row_count, Int col_count)
            : MaxEmptyRegionsFinder(Size(row_count, col_count)) {}
            MaxEmptyRegionsFinder(Size size) 
            : dN(size), dS(size) {}
            
            void set(Int row_count, Int col_count) {
                dN.resize(row_count, col_count);
                dS.resize(row_count, col_count);
            }
            
            void set(Size size) {
                dN.resize(size);
                dS.resize(size);
            }
            
            // grid values : is FILLED ? 
            std::vector<Region> find(const Grid<bool>& grid) {
                int 
                row_count = static_cast<Int>(grid.row_count()),
                col_count = static_cast<Int>(grid.col_count()); 
                std::vector<Region> regs;
                
                for (Int c = 0; c < col_count; ++c) {
                    dN(0, c) = grid(0, c) ? -1 : 0;
                    dS(row_count-1, c) = grid(row_count-1, c) ? -1 : 0;
                }
                
                for (Int r = 1; r < row_count; ++r) {
                    for (Int c = 0; c < col_count; ++c) {
                        dN(r, c) = grid(r, c) ? -1 : dN(r-1, c)+1; 
                    }
                }
                for (Int r = row_count-2; r >= 0; --r) {
                    for (Int c = 0; c < col_count; ++c) {
                        dS(r, c) = grid(r, c) ? -1 : dS(r+1, c)+1;
                    }
                }
                for (Int c = col_count-1; c >= 0; --c) {
                    Int maxS = row_count;
                    for (Int r = row_count-1; r >= 0; --r) {
                        ++maxS;
                        if (!grid(r, c) && (c == 0 || grid(r, c-1))) {
                            Int N = dN(r, c);
                            Int S = dS(r, c);
                            Int local_col_count = 1;
                            while (c + local_col_count < col_count && !grid(r, c + local_col_count)) {
                                Int nextN = dN(r, c + local_col_count);
                                Int nextS = dS(r, c + local_col_count);
                                if (nextN < N || nextS < S) {
                                    if (S < maxS) regs.push_back(Region(r-N, c, N+S+1, local_col_count));
                                    if (nextN < N) N = nextN;
                                    if (nextS < S) S = nextS;
                                }
                                ++local_col_count;
                            }
                            if (S < maxS) regs.push_back(Region(r-N, c, N+S+1, local_col_count));
                            maxS = 0;
                        } 
                    }
                }
                return regs;
            }
        private:
            Grid<Int> dN, dS;
        };
        
        struct MaxEmptyRegions {
            MaxEmptyRegions() {}
            // starting region
            MaxEmptyRegions(const Position& p, const Size& s)
            : MaxEmptyRegions(p.row, p.col, s.row, s.col) {} 
            MaxEmptyRegions(Int row, Int col, Int row_count, Int col_count) {
                max_empty_regions_.push_back(Region(row, col, row_count, col_count));
            }
            
            void set(Int row, Int col, Int row_count, Int col_count) {
                max_empty_regions_.clear();
                max_empty_regions_.push_back(Region(row, col, row_count, col_count));
            }
            
            void insertRegion(const Region& reg) {
                removeSubMaxEmptyRegions(insertCuts(reg));
            }
            
            const std::vector<Region>& max_empty_regions() {
                return max_empty_regions_;
            }
            
            bool isEmpty() const {
                return max_empty_regions_.empty();
            }
            
        private:
            // returns first inserted
            std::vector<Region>::iterator insertCuts(const Region& filled_region) {
                std::vector<Region> empty_bits;
                // go through all rects with it
                auto reg_it = max_empty_regions_.end();
                // first intersected
                auto end_it = max_empty_regions_.end();
                Int r, c, r_sz, c_sz;
                auto push = [&](Int r, Int c, Int r_sz, Int c_sz) {
                    empty_bits.push_back(Region(r, c, r_sz, c_sz));
                };
                // we do it from the cuz of future tail removing
                while (reg_it-- != max_empty_regions_.begin()) {
                    Region& region = *reg_it;
                    if (!filled_region.hasIntersection(region)) continue;
                    
                    // top
                    if ((r_sz = filled_region.row_begin() - region.row_begin()) > 0) {
                        push(
                             region.row_begin(), 
                             region.col_begin(), 
                             r_sz, 
                             region.col_count());
                    }
                    // left
                    if ((c_sz = filled_region.col_begin() - region.col_begin()) > 0) {
                        push(
                             region.row_begin(),
                             region.col_begin(),
                             region.row_count(),
                             c_sz);
                    }
                    // right 
                    c = filled_region.col_end();
                    if ((c_sz = region.col_end() - c) > 0) {
                        push(
                             region.row_begin(),
                             c,
                             region.row_count(),
                             c_sz);
                    }
                    // bottom
                    r = filled_region.row_end();
                    if ((r_sz = region.row_end() - r) > 0) {
                        push(
                             r,
                             region.col_begin(),
                             r_sz,
                             region.col_count());
                    }
                    std::swap(*reg_it, *(--end_it));
                }
                max_empty_regions_.erase(end_it, max_empty_regions_.end());
                // need to know before inserting
                Int region_count = (Int)max_empty_regions_.size();
                max_empty_regions_.insert(max_empty_regions_.end(), empty_bits.begin(), empty_bits.end());
                return max_empty_regions_.begin() + region_count;
            }
            
            void removeSubMaxEmptyRegions(std::vector<Region>::iterator start_it) {
                auto 
                sub_count = max_empty_regions_.end() - start_it,
                full_count = start_it - max_empty_regions_.begin();
                // sub inside sub
                if (sub_count > 1) {
                    std::sort(start_it, max_empty_regions_.end(), 
                              [](const Region& r_0, const Region& r_1) {
                                  return r_0.cell_count() > r_1.cell_count();
                              });
                    auto end_it = max_empty_regions_.end();
                    auto reg_outer_it = start_it;
                    while (reg_outer_it < end_it) {
                        auto reg_inner_it = reg_outer_it;
                        while (++reg_inner_it < end_it) {
                            if (reg_outer_it->hasInside(*reg_inner_it)) {
                                std::swap(*reg_inner_it, *(--end_it));
                            }
                        }
                        ++reg_outer_it;
                    }
                    max_empty_regions_.erase(end_it, max_empty_regions_.end());
                }
                // sub inside full empty  
                if (sub_count > 0 && full_count > 0) {
                    auto end_it = max_empty_regions_.end();
                    auto reg_outer_it = max_empty_regions_.begin();
                    // start iterator don't exchage
                    while (reg_outer_it != start_it) {
                        auto reg_inner_it = start_it;
                        // careful here
                        while (reg_inner_it < end_it) {
                            if (reg_outer_it->hasInside(*reg_inner_it)) {
                                std::swap(*reg_inner_it, *(--end_it));
                            }
                            ++reg_inner_it;
                        }
                        ++reg_outer_it;
                    }
                    max_empty_regions_.erase(end_it, max_empty_regions_.end());
                }
            }
            
            std::vector<Region> max_empty_regions_;
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
                    for (auto i = 0; i < (N-1)/S + 1; ++i) {
                        s <<= S;
                        s |= distr(rng);
                    } 
                    return s;
                };
                for (auto& cell : hash_table_) {
                    for (auto& set : cell) {
                        set = randomBitset();
                    }
                }
            }
            
        public:
            ZobristHashing(const Size& board_size, Count state_count) :
            hash_table_(board_size) {
                
                States states(state_count);
                states.shrink_to_fit();
                hash_table_.fill(states);
                initHashTable();
            }
            
            void xorState(std::bitset<N>* set, const Position& pos, Index state_index) {
                (*set) ^= hash_table_(pos)[state_index];
            }
            
            void xorNothing(std::bitset<N>* set) {
                (*set) ^= NOTHING;
            } 
            
            using value = std::bitset<N>;
        };
        
        template<size_t N> constexpr std::bitset<N> ZobristHashing<N>::NOTHING;
        
        
        
    } // namespace grid
    
} // namespace ant





using namespace std;
using namespace ant;
using namespace ant::grid;


default_random_engine RNG;

vector<string> GenerateStringBoard(int sz);


constexpr bool IsRightMirror(char mirror) {
    return mirror == 'R'; // '\'
}

constexpr bool IsLeftMirror(char mirror) {
    return mirror == 'L'; // '/'
}

struct CastIterator {
    CastIterator(Count sz) : cur(0), sz(sz) {}
    
    bool HasNext() {
        return cur != 4*sz;
    }
    
    
    Position Next() {
        Index i = cur % 4;
        Index j = cur / 4;
        ++cur;
        switch (i) {
            case 0: return {j, -1};
            case 1: return {-1, j};
            case 2: return {sz, j};
            case 3: return {j, sz};
        }
        assert(false);
        return {};
    }
    
    Index cur;
    Count sz;
};


struct CastNode {
    CastNode(const Position& cast, const shared_ptr<CastNode>& previous)
    : cast(cast), previous(previous) {}
    
    static vector<Position> ToHistory(shared_ptr<CastNode> node) {
        vector<Position> casts;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        return casts;
    }
    
    
    
    
    Position cast;
    shared_ptr<CastNode> previous;
};

using Direction = char;


// should be somewhere in cpp probably
// from 50 to 100
constexpr array<double, 51> EMPTY_LINES_PARAM = { {
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
} };

constexpr array<double, 51> OVERHEAD_PARAM = { {
    1.700666, // 50
    1.971176, 
    2.153436, 
    2.074502, 
    1.445232, 
    1.199114, 
    1.882926, 
    1.445232, 
    1.445232, 
    1.946784, 
    1.612416, // 60
    2.153436, 
    1.533482, 
    1.882926, 
    1.612416, 
    1.469623, 
    1.572949, 
    1.445232, 
    2.153436, 
    1.612416, 
    1.445232, // 70
    1.445232, 
    1.803992, 
    1.700666, 
    1.740133, 
    1.533482, 
    1.341906, 
    1.238580, 
    1.469623, 
    2.153436, 
    1.700666, // 80
    1.445232, 
    1.779600, 
    1.700666, 
    1.174722, 
    1.445232, 
    1.971176, 
    1.445232, 
    1.740133, 
    2.846564, 
    1.366298, // 90
    1.700666, 
    1.700666, 
    1.612416, 
    1.700666, 
    1.676275, 
    1.262972, 
    1.882926, 
    1.700666, 
    2.153436, 
    1.700666 // 100
} };

constexpr array<double, 71 - 50 + 1> REDUCE_PARAM = { { 
    0.31952, // 50
    4.91602,
    0.23984,
    0.23984,
    0.31952,
    0.23984,
    0.31952,
    0.23984,
    0.23984,
    0.289085,
    0.289085, // 60
    0.289085,
    0.23984,
    0.23984,
    0.23984,
    0.31952,
    0.23984,
    0.23984,
    0.23984,
    0.289085,
    7.13099, // 70
    0.368765
} };  





class Board_v6 {
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
    
    constexpr const static array<int, 5> kDirOpposite = { {
        kDirBottom, 
        kDirTop, 
        kDirRight, 
        kDirLeft, 
        kDirNothing
    } };
    
    // first index mirror type
    // second index where ray going
    // result direction where will go  
    constexpr const static array<array<char, 4>, 2> kDirReflection = { {
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
    
    
    Count board_size_;
    Count mirrors_destroyed_;
    Count empty_lines_count_;
    
    Count filled_space_;
    Count empty_space_;
    
    HashType hash_;
    
    vector<Item> items_;
    // they are first in items
    // where is ray directed
    vector<Direction> ray_direction_;
    array<vector<char>, 2> mirrors_left_;
    
    shared_ptr<Mirrors> mirrors_;
    shared_ptr<HashFunction> hash_function_;
    shared_ptr<CastNode> history_casts_;
    // use for reduce and restore
    shared_ptr<vector<short>> buffer_;
    
public:
    
    Board_v6() {}
    
    Board_v6(const vector<string>& str_board) {
        board_size_ = str_board.size();
        mirrors_destroyed_ = 0;
        empty_lines_count_ = 0;
        
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
        hash_function_.reset(new HashFunction({board_size_, board_size_}, 1));
        for (auto r = 0; r < board_size_; ++r) {
            for (auto c = 0; c < board_size_; ++c) {
                HashIn(r, c);
            }
        }
    }
    
public:
    
    Count CastRestorable(short ray_index) {
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
    
    void Cast(short ray_index) {
        auto& mirs = *mirrors_;
        shared_ptr<CastNode> new_node(new CastNode({items_[ray_index].row, items_[ray_index].col}, history_casts_));
        history_casts_ = new_node;
        
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
        
    }
    
    void Restore() {
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
            ++empty_lines_count_;
            // empty is not counted as even?????
        } 
        
        if (--mirrors_left_[kOrientVer][row] == 0) {
            ++empty_lines_count_;
        } 
        HashOut({row, col});
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
            --empty_lines_count_;
        }         
        if (++mirrors_left_[kOrientVer][row] == 1) {
            --empty_lines_count_;
        } 
        HashIn({row, col});
    }
    
    
    void Reduce(vector<short>& shift) {
        Reduce();
        auto& offset = *buffer_;
        for (auto& s : shift) {
            s -= offset[s];
        }
    }
    
    // 4 * Number of items
    void Reduce() {
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
    
    bool AllDestroyed() const {
        return empty_lines_count_ == 2 * board_size_;
    }
    
    Count size() const {
        return board_size_;
    }
    
    Count RayCount() const {
        return ray_direction_.size();
    }
    
    Count MirrorsDestroyed() const {
        return mirrors_destroyed_;
    }
    
    Count EmptyLinesCount() const {
        return empty_lines_count_;
    }
    
    HashType hash() const {
        return hash_;
    }
    
    Count EmptySpace() const {
        return empty_space_;
    }
    
    Count FilledSpace() const {
        return filled_space_;
    }
    
    shared_ptr<CastNode> CastHistory() const {
        return history_casts_;
    }
    
private: 
    
    void HashIn(char row, char col) {
        HashIn({row, col});
    }
    
    void HashOut(char row, char col) {
        HashOut({row, col});
    }
    
    void HashIn(const Position& p) {
        hash_function_->xorNothing(&hash_); // xor out 
        hash_function_->xorState(&hash_, p, 0);
    }
    
    void HashOut(const Position& p) {
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }
    
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
    
};


constexpr const array<int, 5> Board_v6::kDirOpposite;
constexpr const array<array<char, 4>, 2> Board_v6::kDirReflection;



template<class Board>
struct BeamSearch_v2 {
private:
    using Hash = typename Board::HashType; 
    
    struct Derivative {
        Derivative() {}
        Derivative(Board* b, short r, Hash h, double s) 
        : origin(b), hash(h), score(s), ray_index(r) {}
        
        Board* origin;
        Hash hash;
        double score;
        short ray_index;
    };
    
    double empty_lines_param_{0};
    double overhead_param_{1};
    
    // our goal is to minimize
    double Score(const Board& b) const {
        return - (b.MirrorsDestroyed() + empty_lines_param_ * b.EmptyLinesCount());
    }
    
public:
    using BoardType = Board;
    using Hashes = unordered_set<typename Board::HashType>;
    
    Board Destroy(const Board& board, Count beam_width, double empty_lines_param, double overhead_param) {
        empty_lines_param_ = empty_lines_param;
        overhead_param_  = overhead_param;
        assert(overhead_param_ >= 1.);
        typename vector<Board>::iterator res;
        // has move semantics now 
        vector<Board> current;
        vector<Board> next;
        vector<Derivative> next_casts;  
        vector<Index> inds;
        Hashes hashes;
        current.push_back(board);
        while (true) {
            for (auto& c : current) {
                AddDerivatives(c, next_casts);
            }
            // current iteration changed
            // dublicates overhead
            SelectBoardIndexes(next_casts, inds, overhead_param_*beam_width);
            RemoveDublicates(next_casts, inds, hashes);
            inds.resize(min<Count>(inds.size(), beam_width));
            FillBoards(next_casts, inds, next);
            next_casts.clear();
            // should use move
            swap(next, current);
            if (current.begin()->AllDestroyed()) {
                break;
            } 
        }
        return *(current.begin());
    }    
    
private:
    
    // bs - where to write results
    void FillBoards(const vector<Derivative>& derivs, 
                    vector<Index>& inds,
                    vector<Board>& bs) {
        
        sort(inds.begin(), inds.end(), [&](Index i_0, Index i_1) {
            return derivs[i_0].origin < derivs[i_1].origin;
        });
        
        bs.resize(inds.size());
        vector<short> rays;
        Index i = 0;
        while (i < bs.size()) {
            Index start = i;
            rays.clear();
            rays.push_back(derivs[inds[start]].ray_index);
            while (++i < bs.size() && derivs[inds[start]].origin == derivs[inds[i]].origin) {
                rays.push_back(derivs[inds[i]].ray_index);
            }
            auto& b_or = *(derivs[inds[start]].origin); 
            for (Index k = 0; k < rays.size(); ++k) {
                bs[start + k] = b_or;
                bs[start + k].Cast(rays[k]);
            }
        }
    }
    
    void AddDerivatives(Board& b, vector<Derivative>& derivs) { 
        for (int i = 0; i < b.RayCount(); ++i) {
            if (b.CastRestorable(i) > 0) derivs.emplace_back(&b, i, b.hash(), Score(b));
            b.Restore();
        }
    }
    
    // here we have an oportunity to choose max_count good derivatives
    void SelectBoardIndexes(const vector<Derivative>& bs, vector<Index>& inds, Count max_count) {
        inds.resize(bs.size());
        iota(inds.begin(), inds.end(), 0);
        int count = min<int>(max_count, bs.size());
        nth_element(inds.begin(), inds.begin() + count-1, inds.end(), [&](Index i_0, Index i_1) {
            return bs[i_0].score < bs[i_1].score;
        });
        inds.resize(count);
    }   
    
    // changing inds a little
    void RemoveDublicates(const vector<Derivative>& next_casts, vector<Index>& inds, Hashes& hashes) {
        hashes.clear();
        for (int i = 0; i < inds.size();)  {
            Hash h = next_casts[inds[i]].hash;
            if (hashes.count(h) != 0) {
                swap(inds[i], inds.back());
                inds.pop_back();
                continue;
            }
            hashes.insert(h);
            ++i;
        }
    }
};


class FragileMirrors {
public:
    vector<int> destroy(vector<string>& board)  {
        Board_v6 b(board);
        BeamSearch_v2<Board_v6> bs;
        b = bs.Destroy(b, 
                       500 * pow(100. / b.size(), 2), 
                       EMPTY_LINES_PARAM[b.size()-50], 
                       OVERHEAD_PARAM[b.size()-50]);
        auto ps = CastNode::ToHistory(b.CastHistory());
        vector<int> res;
        for (auto& p : ps) {
            res.push_back(p.row);
            res.push_back(p.col);
        }
        return res;
    }
};


