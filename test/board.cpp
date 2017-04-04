//
// Created by Anton Logunov on 3/31/17.
//

#include "gtest/gtest.h"

#include "board_v1.hpp"

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

    Board_v1 b_1;
    Board_v1 b_2;
    Board_v1 b_3;

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