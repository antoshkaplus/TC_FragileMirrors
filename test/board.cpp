//
// Created by Anton Logunov on 3/31/17.
//

#include "gtest/gtest.h"

#include "board_v1_impl_1.hpp"
#include "board_v2_impl_1.hpp"
#include "board_v5.hpp"
#include "board_v6.hpp"
#include "cast_history.hpp"
#include "naive_search.hpp"
#include "beam_search.hpp"
#include "beam_search_new.hpp"
#include "score.hpp"


using StrBoard = vector<string>;

vector<StrBoard> ReadStrBoards(const string& path) {
    ifstream cin(path);
    int count;
    cin >> count;
    vector<StrBoard> res(count);
    for_each(res.begin(), res.end(), [&](auto& b) { b = ReadBoard(cin); });
    return res;
}

class BoardTest : public ::testing::Test {
protected:

    Board_v1_Impl_1<CastHistory_Nodes> b_1;
    Board_v1_Impl_1<CastHistory_Nodes> b_2;
    Board_v1_Impl_1<CastHistory_Nodes> b_3;

    virtual void SetUp() {
        auto bs = ReadStrBoards("./../data/test/board_sample.txt");
        transform(bs.begin(), bs.end(), bs.begin(), ConvertToLetterMirrors);
        b_1 = bs[0];
        b_2 = bs[1];
        b_3 = bs[2];
    }

};


TEST_F(BoardTest, Cast) {
    auto c = b_1.Cast({0, -1});
    ASSERT_EQ(1, c);

    c = b_1.Cast({-1, 0});
    ASSERT_EQ(6, c);
    c = b_1.Cast({b_1.size(), b_1.size()-1});
    ASSERT_EQ(8, c);

    ASSERT_EQ(3, b_1.CastCount());
    ASSERT_EQ(6, b_1.EmptyLinesCount());
    ASSERT_EQ(15, b_1.MirrorsDestroyed());
    ASSERT_EQ(8, b_1.LastCastCount());

    c = b_1.Cast({0, -1});
    ASSERT_TRUE(b_1.AllDestroyed());
    ASSERT_TRUE(b_1.hash().none());
}

TEST_F(BoardTest, Restore) {
    b_1.Cast({0, -1});
    auto h = b_1.hash();
    auto c = b_1.EmptyLinesCount();
    b_1.Cast({b_1.size(), b_1.size()-1});
    b_1.Restore();

    ASSERT_EQ(h, b_1.hash());
    ASSERT_EQ(c, b_1.EmptyLinesCount());
}

TEST_F(BoardTest, CastEmptyLines_Vertical) {
    b_1.Cast({4, 1});
    b_1.Cast({4, 1});

    b_1.Cast({4, 0});
    b_1.Restore();
    b_1.Cast({-1, 1});
    ASSERT_EQ(8, b_1.MirrorsDestroyed());

    b_1.Cast({4, 3});
    ASSERT_EQ(16, b_1.MirrorsDestroyed());

    for (auto p : b_1.CastCandidates()) {
        b_1.Cast(p);
        b_1.Restore();
    }
    ASSERT_EQ(16, b_1.MirrorsDestroyed());
}

TEST_F(BoardTest, CastEmptyLines_Horizontal) {
    for (auto i = 0; i < 4; ++i) {
        b_1.Cast({0, -1});
    }
    b_1.Cast({0, -1});
    b_1.Restore();
    b_1.Cast({0, 4});

    ASSERT_EQ(4, b_1.MirrorsDestroyed());
}


class SearchTest : public ::testing::Test {
protected:

    using B_1 = Board_v2_Impl_1<CastHistory_Nodes>;
    using B_2 = Board_v2_Impl_1<CastHistory_Vector>;
    using B_3 = Board_v5;
    using B_4 = Board_v6;

    B_1 b_1;
    B_2 b_2;
    B_3 b_3;
    B_4 b_4;

    array<Board*, 4> bs;

    virtual void SetUp() {
        auto b = GenerateStringBoard(50);

        b_1 = b;
        b_2 = b;
        b_3 = b;
        b_4 = b;

        bs[0] = &b_1;
        bs[1] = &b_2;
        bs[2] = &b_3;
        bs[3] = &b_4;
    }

    template <class B>
    B naiveSolve(const B& b) {
        Score s;
        return NaiveSearch<B, Score>().Destroy(b, s);
    }

    template <class B>
    B beamSolve(const B& b) {
        Score_v1 s;
        BeamSearch<B, Score_v1> bs;
        bs.set_beam_width(100);
        return bs.Destroy(b);
    }

};

TEST_F(SearchTest, NaiveSameResultAllBoards) {
    b_1 = naiveSolve(b_1);
    b_2 = naiveSolve(b_2);
    b_3 = naiveSolve(b_3);
    b_4 = naiveSolve(b_4);

    auto casts = b_1.CastHistory();
    for (auto b_ptr : bs) {
        ASSERT_EQ(casts, b_ptr->CastHistory());
    }
}

TEST_F(SearchTest, BeamSameResultAllBoards) {
    b_1 = beamSolve(b_1);
    b_2 = beamSolve(b_2);
    b_3 = beamSolve(b_3);
    b_4 = beamSolve(b_4);

    auto casts = b_1.CastHistory();
    for (auto b_ptr : bs) {
        ASSERT_EQ(casts, b_ptr->CastHistory());
    }
}

TEST_F(SearchTest, ReduceSameResultAllBoards) {
    b_1 = beamSolve(b_1);
    b_1.set_reduce_empty_ratio(0.25);
    b_2 = beamSolve(b_2);
    b_2.set_reduce_empty_ratio(0.5);
    b_3 = beamSolve(b_3);
    b_3.set_reduce_empty_ratio(0.75);
    b_4 = beamSolve(b_4);
    b_4.set_reduce_empty_ratio(1.);

    auto casts = b_1.CastHistory();
    for (auto b_ptr : bs) {
        ASSERT_EQ(casts, b_ptr->CastHistory());
    }
}

TEST(BeamSearchNew, Functional) {
    auto str_board = GenerateStringBoard(50);
    Board_v6 b = str_board;
    BeamSearchNew<Board_v6> s;
    s.set_beam_width(100);
    s.set_millis(5000);
    b = s.Destroy(b);
    auto history = b.CastHistory();
    Board_v1_Impl_1<CastHistory_Nodes> s_check = str_board;
    for_each(history.begin(), history.end(), [&](const Position& p) {
        s_check.Cast(p);
    });
    ASSERT_TRUE(s_check.AllDestroyed());
}