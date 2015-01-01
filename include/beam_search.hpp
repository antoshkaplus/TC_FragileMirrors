//
//  beam_search.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/26/14.
//
//

#ifndef FRAGILE_MIRRORS_beam_search_hpp
#define FRAGILE_MIRRORS_beam_search_hpp

#include <unordered_set>
#include <queue>

#include "board_v1.hpp"



class BeamSearch {
private:
    using Board = Board_v1;
    using Hash = Board::HashType; 
    
    struct Derivative {
        Derivative() {}
        Derivative(Board* b, const Position& p, Hash h, Count d) 
        : origin(b), cast(p), hash(h), destroyed_count(d) {}
        
        Board* origin;
        Position cast;
        Hash hash;
        Count destroyed_count;
    };
    
    struct Derivative_2 : public Derivative {
        using Derivative::Derivative;
        
        Count extra_casts;
        Count extra_destroyed;
    };
    
public:
    using BoardType = Board;
    
    std::function<bool(Index, Index)> IndexComparator(const vector<Board>& bs) {
        return [&](Index i_0, Index i_1) {
            return bs[i_0].MirrorsDestroyed() > bs[i_1].MirrorsDestroyed(); 
        };
    }
    
    std::function<bool(Index, Index)> IndexComparator(const vector<Derivative>& ds) {
        return [&](Index i_0, Index i_1) {
            return ds[i_0].destroyed_count > ds[i_1].destroyed_count;
        };
    }
    
    std::function<bool(Index, Index)> IndexComparator(const vector<Derivative_2>& ds, const vector<double>& coeffs) {
        return [&](Index i_0, Index i_1) {
            return (ds[i_0].destroyed_count) > //+ ds[i_0].extra_destroyed) *coeffs[ds[i_0].origin->CastCount() + 1 + ds[i_0].extra_casts] > 
                    (ds[i_1].destroyed_count); ///+ ds[i_1].extra_destroyed) *coeffs[ds[i_1].origin->CastCount() + 1 + ds[i_1].extra_casts];
        };
    }
    
    
    // using some global coeffs
    void Destroy_2(const Board& board, const vector<double>& coeffs, Count beam_width) {
        //cout << Destroy(board, beam_width).CastCount() << endl;
        
        vector<Board>* current = new vector<Board>;
        vector<Board>* next = new vector<Board>;
        vector<Derivative_2> next_casts;
        current->push_back(board);
        while (true) {
            if (current->at(0).CastCount() == 90) {
                cout << "here\n";
            }
            for (auto i = 0; i < current->size(); ++i) {
                AddDerivatives_2((*current)[i], next_casts);
            }
            if (next_casts.empty()) break;
            vector<Index> inds = SelectBoardIndexes_2(next_casts, coeffs, beam_width);
            RemoveDublicates(next_casts, inds);
            FillBoards(next_casts, inds, *next);
            next_casts.clear();
            swap(next, current);
            for (auto& bb : *current) {
                if (bb.AllDestroyed()) {
                    cout << "haha " << bb.CastCount() << endl;
                }
            }
        }
    }
    
    
    Board Destroy(const vector<string> str_board, Count beam_width) {
        return Destroy(Board(str_board), beam_width);
    }
    
    Board Destroy(const Board& board, Count beam_width) {
        Board res = board; 
        vector<Board>* current = new vector<Board>;
        vector<Board>* next = new vector<Board>;
        vector<Derivative> next_casts;
        current->push_back(board);
        while (true) {
            for (auto i = 0; i < current->size(); ++i) {
                AddDerivatives((*current)[i], next_casts);
            }
            // current iteration changed
            vector<Index> inds = SelectBoardIndexes(next_casts, beam_width);
            RemoveDublicates(next_casts, inds);
            FillBoards(next_casts, inds, *next);
            next_casts.clear();
            swap(next, current);
            res = *max_element(current->begin(), current->end(), [&](const Board b_0, const Board& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (res.AllDestroyed()) {
                break;
            } 
        }
        return res;
    }
    
    Board Destroy(const Board& board, Count beam_width, Count child_count) {
        Board res = board; 
        vector<Board>* current = new vector<Board>;
        vector<Board>* next = new vector<Board>;
        vector<Derivative> next_casts;
        current->push_back(board);
        while (true) {
            for (auto i = 0; i < current->size(); ++i) {
                AddDerivatives((*current)[i], next_casts, child_count);
            }
            // current iteration changed
            vector<Index> inds = SelectBoardIndexes(next_casts, beam_width);
            RemoveDublicates(next_casts, inds);
            FillBoards(next_casts, inds, *next);
            next_casts.clear();
            swap(next, current);
            res = *max_element(current->begin(), current->end(), [&](const Board b_0, const Board& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (res.AllDestroyed()) {
                break;
            } 
        }
        return res;
    }
    
    
private:
    
    template<class D>
    void FillBoards(const vector<D>& derivs, 
                    const vector<Index>& inds,
                    vector<Board>& bs) {
        bs.resize(inds.size());
        for (auto k = 0; k < bs.size(); ++k) {
            Board& b = bs[k];
            b = *(derivs[inds[k]].origin);
            b.Cast(derivs[inds[k]].cast);
        }
    }

    
    template<class D>
    void FillBoards(const vector<D>& derivs, vector<Board>& bs) {
        bs.resize(derivs.size());
        for (int i = 0; i < bs.size(); ++i) {
            Board& b = bs[i];
            b = *(derivs[i].origin);
            b.Cast(derivs[i].cast);
        }
    }
    vector<Derivative> ds;
    void AddDerivatives(Board& b, vector<Derivative>& derivs, Count child_count) {
        // want to get smallest element everytime
        auto comp = [](const Derivative& d_0, const Derivative& d_1) {
            return d_0.destroyed_count > d_1.destroyed_count;
        };
        ds.clear();
        Count sz = b.board_size();
        for (int i = 0; i < sz; ++i) {
            array<Position, 4> cs = { { {i, -1}, {-1, i}, {i, sz}, {sz, i} } };
            for (auto& c : cs) {
                if (b.Cast(c) > 0) { 
                    ds.emplace_back(&b, c, b.hash(), b.MirrorsDestroyed());
                }               
                b.Restore();
            }
        }
        nth_element(ds.begin(),  ds.begin() + min<int>(child_count, ds.size()) - 1, ds.end(), comp);
        ds.resize(min<int>(child_count, ds.size()));
        derivs.insert(derivs.end(), ds.begin(), ds.end());
    }
    
    void AddDerivatives(Board& b, vector<Derivative>& derivs) {
        Count sz = b.board_size();
        for (int i = 0; i < sz; ++i) {
            array<Position, 4> cs = { { {i, -1}, {-1, i}, {i, sz}, {sz, i} } };
            for (auto& c : cs) {
                if (b.Cast(c) > 0) derivs.push_back({&b, c, b.hash(), b.MirrorsDestroyed()});
                b.Restore();
            }
        }
    }
    
    // can also add something to derivs instances
    Count AddDerivatives_2(Board& b, vector<Derivative_2>& derivs) {
        Count sz = b.size();
//        Count extra_casts = 0;
//        Count extra_destroyed = 0;
        vector<Derivative_2> ds;
        Index start = derivs.size();
        for (int i = 0; i < sz; ++i) {
            array<Position, 4> cs = { { {i, -1}, {-1, i}, {i, sz}, {sz, i} } };
            for (auto& c : cs) {
                if (b.Cast(c) > 0) {
                    if (b.IsLastIsolated()) {
                        // for 1 elements should divide by 4
                        // for 2 elements should divide by 2
//                        if (b.LastCastCount() == 2) {
//                            extra_casts += 2;
//                            extra_destroyed += 2*b.LastCastCount(); 
//                        } else {
//                            extra_casts++;
//                            extra_destroyed++;
//                        }
                        //if (c.row == -1) cout << b.last_cast().at(0).row << " " <<  b.last_cast().at(0).col  << endl;
                        ds.emplace_back(&b, c, b.hash(), b.MirrorsDestroyed());
                    } else {
                        derivs.emplace_back(&b, c, b.hash(), b.MirrorsDestroyed());
                    }
                } 
                b.Restore();

            }
        }
//        extra_casts /= 4;
//        extra_destroyed /= 4;
//        for_each(derivs.begin()+start, derivs.end(), [&](Derivative_2& d) {
//            d.extra_casts = extra_casts;
//            d.extra_destroyed = extra_destroyed;
//        });
//        if (derivs.size() - start == 0) {
//            cout << "opa " << b.EmptyLinesCount() << endl;
//        }
        if (derivs.size() - start == 0) derivs.insert(derivs.end(), ds.begin(), ds.end());
        return derivs.size() - start;
    }
    
    vector<Index> SelectBoardIndexes_2(const vector<Derivative_2>& bs, const vector<double>& coeffs, Count max_count) {
        vector<Index> inds(bs.size());
        iota(inds.begin(), inds.end(), 0);
        int count = min<int>(max_count, bs.size());
        nth_element(inds.begin(), inds.begin() + count-1, inds.end(), IndexComparator(bs, coeffs));
        inds.resize(count);
        return inds;
    }
    
    // here we have an oportunity to choose max_count good derivatives
    vector<Index> SelectBoardIndexes(const vector<Derivative>& bs, Count max_count) {
        vector<Index> inds(bs.size());
        iota(inds.begin(), inds.end(), 0);
        int count = min<int>(max_count, bs.size());
        nth_element(inds.begin(), inds.begin() + count-1, inds.end(), IndexComparator(bs));
        inds.resize(count);
        return inds;
    }   
    
    
    void RemoveDublicates_2(const vector<Board>& bs, vector<Index>& inds) {
        unordered_set<typename Board::HashType> hashes;
        // first need to sort by number of mirrors destroyed
        int k = 0;
        for (int i = 0; i < inds.size(); ++i)  {
            const Board& b = bs[inds[i]];
            if (hashes.count(b.hash()) != 0) {
                continue;
            }
            hashes.insert(b.hash());
            inds[k++] = inds[i];
        }
        inds.resize(k);
    }
    
    // changing inds a little
    template<class D>
    void RemoveDublicates(const vector<D>& next_casts, vector<Index>& inds) {
        unordered_set<typename Board::HashType> hashes;
        // first need to sort by number of mirrors destroyed
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
    
    void RemoveDublicates(const vector<Board>& bs, vector<Index>& inds) {
        unordered_set<typename Board::HashType> hashes;
        // first need to sort by number of mirrors destroyed
        int k = 0;
        for (int i = 0; i < inds.size(); ++i)  {
            const Board& b = bs[inds[i]];
            if (hashes.count(b.hash()) != 0) {
                continue;
            }
            hashes.insert(b.hash());
            inds[k++] = inds[i];
        }
        inds.resize(k);
    }
    
    template<class D>
    void SelectDerivatives(vector<Index>& inds, vector<D>& cs) {
        sort(inds.begin(), inds.end());
        for (auto j = 0; j < inds.size(); ++j) {
            cs[j] = cs[inds[j]];
        }
        cs.resize(inds.size());
    }
    
    void SelectBoards(vector<Index>& inds, vector<Board>& bs) {
        sort(inds.begin(), inds.end());
        for (auto j = 0; j < inds.size(); ++j) {
            bs[j] = bs[inds[j]];
        }
        bs.resize(inds.size());
        
    }
    
};


#endif
