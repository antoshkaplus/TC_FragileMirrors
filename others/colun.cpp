#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <utility>
#include <cstring>
#include <algorithm>
#include <bitset>
#include <cmath>

#include <cassert>

#include "fragile_mirrors.hpp"

using namespace std;

typedef long long int64;
typedef unsigned long long uint64;

unsigned int myRandInt32() {
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

inline int myRandInt(int r) {
    unsigned int a = myRandInt32();
    int rr;
    asm(
        "mul %2;"
        :"=d"(rr)
        :"a"(a),"r"(r)
        );
    return rr;
}

#ifdef WIN32
#include <Windows.h>
static inline double getTime2(){
    return timeGetTime() * 1e-3;
}
#endif
#ifndef WIN32
#include <sys/time.h>
static inline double getTime2(){
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif
static double g_startTime;
static inline void initTime(){
    g_startTime = getTime2();
}
static inline double getTime(){
    return getTime2() - g_startTime;
}
const double g_timeupSecBase = 9.8;
double g_timeupSec = g_timeupSecBase;

//static inline bool isTimeup() {
//    return g_timeupSec <= getTime();
//}

template<class T>
class MemoryPool {
public:
    ~MemoryPool() {
        clear();
    }
    T * create(const T * t) {
        if(stack.empty()) {
            T * ret = new T(*t);
            assert(ret!=NULL);
            return ret;
        }
        T * ret = stack.back();
        stack.pop_back();
        *ret = *t;
        return ret;
    }
    T * create() {
        if(stack.empty()) {
            T * ret = new T();
            assert(ret!=NULL);
            return ret;
        }
        T * ret = stack.back();
        stack.pop_back();
        return ret;
    }
    void release(T * t) {
        stack.push_back(t);
    }
    void clear() {
        for(int i=0; i<(int)stack.size(); ++i) {
            delete stack[i];
        }
        stack.clear();
    }
protected:
    vector<T*> stack;
};

class Game;
struct GameChild;

MemoryPool<Game> g_pool;
MemoryPool<GameChild> g_childPool;

bool g_devFlag = false;

int g_N;
int g_NN;
int g_2NN;
char g_board[102][128];
int g_hashSeed[102][128];

typedef bitset<400> BitSet400;

BitSet400 g_range1[102][128];

template<int size>
struct FastSet {
    short data[size];
    short id;
    FastSet() {
        memset(data, 0, sizeof(data));
        id = 1;
    }
    void clear() {
        ++id;
        if(id==0) {
            ++id;
            memset(data, 0, sizeof(data));
        }
    }
    bool test(int i) const {
        return data[i]==id;
    }
    void set(int i) {
        data[i] = id;
    }
    void reset(int i) {
        data[i] = 0;
    }
};
template<int w, int h>
struct FastSet2D {
    char data[h][w];
    char id;
    FastSet2D() {
        memset(data, 0, sizeof(data));
        id = 1;
    }
    void clear() {
        ++id;
        if(id==0) {
            ++id;
            memset(data, 0, sizeof(data));
        }
    }
    bool test(int x, int y) const {
        return data[y][x]==id;
    }
    void set(int x, int y) {
        data[y][x] = id;
    }
    void reset(int x, int y) {
        data[y][x] = 0;
    }
};
template<int size>
struct FastCounter {
    short data[size];
    short id;
    FastCounter() {
        memset(data, 0, sizeof(data));
        id = 256;
    }
    void clear() {
        id+=256;
        if(id==0) {
            id+=256;
            memset(data, 0, sizeof(data));
        }
    }
    char get(int i) const {
        short d = data[i];
        return (d&~255)==id ? d : 0;
    }
    void set(int i, char n) {
        data[i] = id | (short)(unsigned char)n;
    }
};
template<int w, int h>
struct FastCounter2D {
    int data[w][h];
    int id;
    FastCounter2D() {
        memset(data, 0, sizeof(data));
        id = 65536;
    }
    void clear() {
        id+=65536;
        if(id==0) {
            id+=65536;
            memset(data, 0, sizeof(data));
        }
    }
    short get(int x, int y) const {
        int d = data[y][x];
        return (d&~65535)==id ? d : 0;
    }
    void set(int x, int y, short n) {
        data[y][x] = id | (int)(unsigned short)n;
    }
};

FastSet<400> g_thinkSkip;
short g_thinkScore[400];
FastSet2D<128, 102> g_thinkSet2;
FastCounter<102> g_thinkXCount;
FastCounter<102> g_thinkYCount;

void initBoardSub(int x, int y, int dir, int id) {
    while(true) {
        if(!(dir&1)) {
            x += ((dir&2) ? -1 : 1);
            if(x==0 || x==g_N+1) {
                break;
            }
        }
        else {
            y += ((dir&2) ? -1 : 1);
            if(y==0 || y==g_N+1) {
                break;
            }
        }
        if(g_range1[y][x].test(id)) {
            continue;
        }
        g_range1[y][x].set(id);
        char board = g_board[y][x];
        if(board=='L') {
            dir ^= 3;
        }
        else if(board=='R') {
            dir ^= 1;
        }
        else assert(false);
    }
}
void initBoard(const vector<string> & board) {
    g_N = board.size();
    g_NN = g_N*g_N;
    g_2NN = g_NN+g_NN;
    memset(g_board, 0, sizeof(g_board));
    memset(g_hashSeed, 0, sizeof(g_hashSeed));
    for(int y=1; y<=g_N; ++y) {
        for(int x=1; x<=g_N; ++x) {
            g_board[y][x] = board[y-1][x-1];
            int seed;
            while(true) {
                seed = myRandInt32() & ~2;
                if(seed) {
                    break;
                }
            }
            g_hashSeed[y][x] = seed | (board[y-1][x-1]=='L' ? 2 : 0);
        }
    }
    memset(g_range1, 0, sizeof(g_range1));
    for(int z=0; z<g_N; ++z) {
        initBoardSub(0, z+1, 0, z);
    }
    for(int z=0; z<g_N; ++z) {
        initBoardSub(z+1, 0, 1, 100+z);
    }
    for(int z=0; z<g_N; ++z) {
        initBoardSub(g_N+1, z+1, 2, 200+z);
    }
    for(int z=0; z<g_N; ++z) {
        initBoardSub(z+1, g_N+1, 3, 300+z);
    }
}

class BeamPool;
class Game {
private:
    char m_link[102][102][4];
    char m_result[200][2];
    int m_xCount[102];
    int m_yCount[102];
    int m_beforeScore[400];
    BitSet400 m_breakSeal;
    int m_remainW[7];
    int m_remainH[7];
    int m_turn;
    int m_hash;
    int m_score;
    int m_odd;
    int m_refCount;
    void copy(const Game & o) {
        memcpy(m_link, o.m_link, sizeof(m_link[0])*(g_N+2));
        //		int n2 = g_N+2;
        //		int n2sz = sizeof(m_link[0][0]) * n2;
        //		for(int i=0; i<n2; ++i) {
        //			memcpy(m_link[i], o.m_link[i], n2sz);
        //		}
        //memcpy(m_link, o.m_link, sizeof(m_link));
        memcpy(m_result, o.m_result, sizeof(m_result));
        memcpy(m_xCount, o.m_xCount, sizeof(m_xCount));
        memcpy(m_yCount, o.m_yCount, sizeof(m_yCount));
        memcpy(m_beforeScore, o.m_beforeScore, sizeof(m_beforeScore));
        m_breakSeal = o.m_breakSeal;
        memcpy(m_remainW, o.m_remainW, sizeof(m_remainW));
        memcpy(m_remainH, o.m_remainH, sizeof(m_remainH));
        m_turn = o.m_turn;
        m_hash = o.m_hash;
        m_score = o.m_score;
        m_odd = o.m_odd;
    }
public:
    Game() {
        for(int y=0; y<g_N+2; ++y) {
            for(int x=0; x<g_N+2; ++x) {
                m_link[y][x][0] = 1;
                m_link[y][x][1] = 1;
                m_link[y][x][2] = -1;
                m_link[y][x][3] = -1;
            }
        }
        for(int i=1; i<=g_N; ++i) {
            m_xCount[i] = g_N;
            m_yCount[i] = g_N;
        }
        memset(m_beforeScore, 0, sizeof(m_beforeScore));
        for(int i=0; i<(int)(sizeof(m_remainW)/sizeof(m_remainW[0])); ++i) {
            m_remainW[i] = g_N;
        }
        for(int i=0; i<(int)(sizeof(m_remainH)/sizeof(m_remainH[0])); ++i) {
            m_remainH[i] = g_N;
        }
        m_turn = 0;
        m_hash = 0;
        m_score = 0;
        m_odd = (g_N&1) ? g_N+g_N : 0;
        m_refCount = 1;
    }
    Game(const Game & o) { m_refCount = 1; copy(o); }
    void operator = (const Game & o) { copy(o); }
    void exec(int dir, int z) {
        int x, y;
        switch(dir&3) {
            case 0:
                x=0;
                y=z+1;
                m_odd += (m_yCount[y] & 1) ? -1 : 1;
                break;
            case 1:
                x=z+1;
                y=0;
                m_odd += (m_xCount[x] & 1) ? -1 : 1;
                break;
            case 2:
                x=g_N+1;
                y=z+1;
                m_odd += (m_yCount[y] & 1) ? -1 : 1;
                break;
            case 3:
                x=z+1;
                y=g_N+1;
                m_odd += (m_xCount[x] & 1) ? -1 : 1;
                break;
        }
        m_result[m_turn][0] = y-1;
        m_result[m_turn][1] = x-1;
        int sc = 0;
        int hash = 0;
        while(true) {
            int link = m_link[y][x][dir];
            if(!(dir&1)) {
                x += link;
                if(x==0 || x==g_N+1) {
                    m_odd += (m_yCount[y] & 1) ? 1 : -1;
                    break;
                }
            }
            else {
                y += link;
                if(y==0 || y==g_N+1) {
                    m_odd += (m_xCount[x] & 1) ? 1 : -1;
                    break;
                }
            }
            char board = g_board[y][x];
            assert(board!=0);
            sc += 1;
            hash ^= g_hashSeed[y][x];
            for(int d=0; d<4; ++d) {
                int link2 = m_link[y][x][d];
                int x2 = x;
                int y2 = y;
                if(!(d&1)) {
                    x2 += link2;
                }
                else {
                    y2 += link2;
                }
                m_link[y2][x2][d^2] = m_link[y][x][d^2] - link2;
            }
            m_breakSeal |= g_range1[y][x];
            {
            int r = --m_xCount[x];
            if(r<(int)(sizeof(m_remainW)/sizeof(m_remainW[0]))) {
                --m_remainW[r];
            }
            }
            {
            int r = --m_yCount[y];
            if(r<(int)(sizeof(m_remainH)/sizeof(m_remainH[0]))) {
                --m_remainH[r];
            }
            }
            if(board=='L') {
                dir ^= 3;
            }
            else if(board=='R') {
                dir ^= 1;
            }
            else assert(false);
        }
        m_score += sc;
        m_hash ^= hash;
        ++m_turn;
    }
    inline bool thinkSub(int x, int y, int dir, int id, int & hash, int & sc, int & odd, int * remainW, int * remainH, bool & beforeFlag) {
        int hash2 = 0;
        int sc2 = 0;
        int odd2;
        if(!(dir&1)) {
            odd2 = (m_yCount[y] & 1) ? -1 : 1;
        }
        else {
            odd2 = (m_xCount[x] & 1) ? -1 : 1;
        }
        bool symmetry = true;
        g_thinkSet2.clear();
        g_thinkXCount.clear();
        g_thinkYCount.clear();
        while(true) {
            int link = m_link[y][x][dir];
            if(!(dir&1)) {
                x += link;
                if(x==0) {
                    if(symmetry) {
                        g_thinkSkip.set(0 + y-1);
                        if(sc2==0) {
                            return false;
                        }
                    }
                    odd2 += ((m_yCount[y]-g_thinkYCount.get(y)) & 1) ? 1 : -1;
                    break;
                }
                else if(x==g_N+1) {
                    if(symmetry) {
                        g_thinkSkip.set(200 + y-1);
                        if(sc2==0) {
                            return false;
                        }
                    }
                    odd2 += ((m_yCount[y]-g_thinkYCount.get(y)) & 1) ? 1 : -1;
                    break;
                }
            }
            else {
                y += link;
                if(y==0) {
                    if(symmetry) {
                        g_thinkSkip.set(100 + x-1);
                        if(sc2==0) {
                            return false;
                        }
                    }
                    odd2 += ((m_xCount[x]-g_thinkXCount.get(x)) & 1) ? 1 : -1;
                    break;
                }
                else if(y==g_N+1) {
                    if(symmetry) {
                        g_thinkSkip.set(300 + x-1);
                        if(sc2==0) {
                            return false;
                        }
                    }
                    odd2 += ((m_xCount[x]-g_thinkXCount.get(x)) & 1) ? 1 : -1;
                    break;
                }
            }
            if(g_thinkSet2.test(x, y)) {
                symmetry = false;
                continue;
            }
            g_thinkSet2.set(x, y);
            int h = g_hashSeed[y][x];
            hash2 ^= h;
            sc2 += 1;
            {
            int xc = g_thinkXCount.get(x) + 1;
            g_thinkXCount.set(x, xc);
            int r = m_xCount[x]-xc;
            if(r<(int)(sizeof(m_remainW)/sizeof(m_remainW[0]))) {
                --remainW[r];
            }
            }
            {
            int yc = g_thinkYCount.get(y) + 1;
            g_thinkYCount.set(y, yc);
            int r = m_yCount[y]-yc;
            if(r<(int)(sizeof(m_remainH)/sizeof(m_remainH[0]))) {
                --remainH[r];
            }
            }
            dir ^= 1 ^ (h&2);
        }
        hash = hash2;
        sc = sc2;
        odd = odd2;
        beforeFlag = (sc2<=m_beforeScore[id]);
        m_beforeScore[id] = sc2;
        return true;
    }
    void copyResult(vector<int> & result) const {
        result.resize(m_turn * 2);
        for(int i=0; i<m_turn; ++i) {
            result[i*2] = m_result[i][0];
            result[i*2+1] = m_result[i][1];
        }
    }
    int getHash() const {
        return m_hash;
    }
    int getScore() const {
        return m_score;
    }
    int getTurn() const {
        return m_turn;
    }
    /*
     int getXYFin() const {
     return m_xFinCount + m_yFinCount;
     }
     */
    int getWH0() const {
        return m_remainW[0] + m_remainH[0];
    }
    /*
     int getXFin() const {
     return m_xFinCount;
     }
     int getYFin() const {
     return m_yFinCount;
     }
     */
    bool enabledSeal() const {
        int cnt = m_breakSeal.count();
        return cnt!=0 && cnt!=g_N*4;
    }
    void think(BeamPool & nextPool);
    void thinkSeal(BeamPool & nextPool);
    GameChild * think3Sub();
    void think3(BeamPool & nextPool);
    Game * addRef() {
        ++m_refCount;
        return this;
    }
    void initRef() {
        m_refCount = 1;
    }
    Game * duplicateRelease() {
        if(m_refCount==1) {
            return this;
        }
        release();
        return duplicate();
    }
    Game * duplicate() {
        Game * ret = g_pool.create(this);
        ret->initRef();
        return ret;
    }
    inline void release() {
        --m_refCount;
        if(m_refCount==0) {
            g_pool.release(this);
        }
    }
};

struct GameChild {
    Game * game;
    int dir;
    int z;
    int hash;
    int score;
    int score2;
    bool beforeFlag;
    double mix;
    inline void init(Game * game, int dir, int z, int hash, int score, int odd, int * remainW, int * remainH, bool beforeFlag);
    inline void release() {
        if(game!=NULL) {
            game->release();
        }
        g_childPool.release(this);
    }
    inline Game * makeGame() {
        if(game==NULL) {
            return new Game();
        }
        Game * result = game->duplicateRelease();
        game = NULL;
        result->exec(dir, z);
        return result;
    }
    inline bool operator < (const GameChild & o) const {
        if(mix!=o.mix) {
            return mix < o.mix;
        }
        /*
         if(finXY_add!=o.finXY_add) {
         return finXY_add < o.finXY_add;
         }
         if(finXY_mul!=o.finXY_mul) {
         return finXY_mul > o.finXY_mul;
         }
         */
        if(score!=o.score) {
            return score < o.score;
        }
        return hash < o.hash;
    }
};

const int g_beamCandidateMaxCountBase = 4096;
int g_beamCandidateMaxCount = g_beamCandidateMaxCountBase;

template <class T>
struct PtrGreater {
    bool operator()(const T * a, const T * b) const {
        return *b < *a;
    }
};

class BeamPool {
public:
    BeamPool() {
        count = 0;
        beforeHash = 0;
    }
    inline void put(GameChild * child) {
        if(count==g_beamCandidateMaxCount) {
            if(*(data[0]) < *child) {
                pop_heap(data, data+count, PtrGreater<GameChild>());
                data[count-1]->release();
                data[count-1] = child;
                push_heap(data, data+count, PtrGreater<GameChild>());
            }
            else {
                child->release();
            }
        }
        else {
            data[count++] = child;
            push_heap(data, data+count, PtrGreater<GameChild>());
        }
    }
    void clear() {
        for(int i=0; i<count; ++i) {
            data[i]->release();
        }
        count = 0;
    }
    void reduce() {
        sort_heap(data, data+count, PtrGreater<GameChild>());
        reverse(data, data+count);
        beforeHash = 0;
    }
    bool empty() const {
        return count==0;
    }
    Game * pop() {
        GameChild * ch;
        while(true) {
            if(count==0) {
                return NULL;
            }
            ch = data[--count];
            if(beforeHash==0 || ch->hash!=beforeHash) {
                break;
            }
            ch->release();
        }
        beforeHash = ch->hash;
        Game * result = ch->makeGame();
        ch->release();
        return result;
    }
protected:
    typedef GameChild * TyGameChildPtr;
    typedef map<int, TyGameChildPtr> TyPool;
    //TyPool pool;
    //vector<TyGameChildPtr> candidateList;
    TyGameChildPtr data[g_beamCandidateMaxCountBase];
    int count;
    int beforeHash;
};

//const int g_linePena = 0;

void Game::think(BeamPool & nextPool) {
    g_thinkSkip.clear();
    int hash_base = m_hash;
    int sc_base = m_score;
    int odd_base = m_odd;
    int remainW[sizeof(m_remainW)/sizeof(m_remainW[0])];
    int remainH[sizeof(m_remainH)/sizeof(m_remainH[0])];
    memcpy(remainW, m_remainW, sizeof(m_remainW));
    memcpy(remainH, m_remainH, sizeof(m_remainH));
    for(int z=0; z<g_N; ++z) if(m_yCount[z+1]) {
        const int id = 0+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(!g_thinkSkip.test(id) && thinkSub(0, z+1, 0, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 0, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_xCount[z+1]) {
        const int id = 100+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(!g_thinkSkip.test(id) && thinkSub(z+1, 0, 1, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 1, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_yCount[z+1]) {
        const int id = 200+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(!g_thinkSkip.test(id) && thinkSub(g_N+1, z+1, 2, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 2, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_xCount[z+1]) {
        const int id = 300+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(!g_thinkSkip.test(id) && thinkSub(z+1, g_N+1, 3, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 3, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
}
void Game::thinkSeal(BeamPool & nextPool) {
    g_thinkSkip.clear();
    int hash_base = m_hash;
    int sc_base = m_score;
    int odd_base = m_odd;
    int remainW[sizeof(m_remainW)/sizeof(m_remainW[0])];
    int remainH[sizeof(m_remainH)/sizeof(m_remainH[0])];
    memcpy(remainW, m_remainW, sizeof(m_remainW));
    memcpy(remainH, m_remainH, sizeof(m_remainH));
    for(int z=0; z<g_N; ++z) if(m_yCount[z+1]) {
        const int id = 0+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(m_breakSeal.test(id) && !g_thinkSkip.test(id) && thinkSub(0, z+1, 0, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 0, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_xCount[z+1]) {
        const int id = 100+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(m_breakSeal.test(id) && !g_thinkSkip.test(id) && thinkSub(z+1, 0, 1, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 1, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_yCount[z+1]) {
        const int id = 200+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(m_breakSeal.test(id) && !g_thinkSkip.test(id) && thinkSub(g_N+1, z+1, 2, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 2, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
    for(int z=0; z<g_N; ++z) if(m_xCount[z+1]) {
        const int id = 300+z;
        int hash, sc, odd;
        bool beforeFlag;
        if(m_breakSeal.test(id) && !g_thinkSkip.test(id) && thinkSub(z+1, g_N+1, 3, id, hash, sc, odd, remainW, remainH, beforeFlag)) {
            GameChild * ch = g_childPool.create();
            ch->init(this, 3, z, hash_base ^ hash, sc_base + sc, odd_base + odd, remainW, remainH, beforeFlag);
            nextPool.put(ch);
            memcpy(remainW, m_remainW, sizeof(m_remainW));
            memcpy(remainH, m_remainH, sizeof(m_remainH));
        }
        else {
            m_beforeScore[id] = 0;
        }
    }
}


vector<int> FragileMirrors::destroy(const vector<string> & board) {
    //g_beamCandidateMaxCount = min(g_beamCandidateMaxCountBase, (int)(8000000 / (board.size()*board.size())));
    initTime();
    initBoard(board);
    double prepareTime = getTime();
    fprintf(stderr, "PrepareTime: %fsec\n", prepareTime);
    double timeupSec = g_timeupSec - prepareTime;
    Game * bestScoreGame = new Game();
    fprintf(stderr, "N = %d\n", g_N);
    BeamPool beamPoolMem1, beamPoolMem2;
    BeamPool * beamPool = &beamPoolMem1;
    BeamPool * nextPool = &beamPoolMem2;
    {
    GameChild * ch = g_childPool.create();
    ch->init(NULL, 0, 0, 0, 0, 0, 0, 0, false);
    beamPool->put(ch);
    }
    const int guessTurn = (g_N&1) ? (int)(g_N*1.4) : (int)(g_N*1.1);
    int costSum = 0;
    for(int i=guessTurn; 1<=i; --i) {
        costSum += i;
    }
    int nowCost = 0;
    int turn = 0;
    for(int i=guessTurn; ; --i) {
        ++turn;
        if(1<=i) {
            nowCost += i;
        }
        double timeLimit = timeupSec * nowCost / costSum;
        beamPool->reduce();
        assert(nextPool->empty());
        int count = 0;
        while(!beamPool->empty()) {
            Game * game = beamPool->pop();
            if(game==NULL) {
                break;
            }
            ++count;
            if(bestScoreGame->getScore() < game->getScore()) {
                bestScoreGame->release();
                bestScoreGame = game;
                //fprintf(stderr, "BestScore: %d, Remain: %d(%dx%d), Turn: %d(%d)\n", bestScoreGame->getScore(), g_N*g_N - bestScoreGame->getScore(), g_N - bestScoreGame->getXFin(), g_N - bestScoreGame->getYFin(), bestScoreGame->getTurn(), g_N - bestScoreGame->getTurn());
                bestScoreGame->addRef();
            }
            if(20<game->getWH0() && game->enabledSeal()) {
                game->thinkSeal(*nextPool);
            }
            else {
                game->think(*nextPool);
            }
            game->release();
            if(timeLimit<=getTime()) {
                break;
            }
        }
        //fprintf(stderr, "ThinkCount(%d): %d\n", turn, count);
        beamPool->clear();
        swap(beamPool, nextPool);
        if(g_N*g_N <= bestScoreGame->getScore()) {
            break;
        }
    }
    beamPool->reduce();
    while(!beamPool->empty()) {
        Game * game = beamPool->pop();
        if(game==NULL) {
            break;
        }
        if(bestScoreGame->getScore() < game->getScore()) {
            bestScoreGame->release();
            bestScoreGame = game;
            //fprintf(stderr, "BestScore: %d, Remain: %d(%dx%d), Turn: %d(%d)\n", bestScoreGame->getScore(), g_N*g_N - bestScoreGame->getScore(), g_N - bestScoreGame->getXFin(), g_N - bestScoreGame->getYFin(), bestScoreGame->getTurn(), g_N - bestScoreGame->getTurn());
            bestScoreGame->addRef();
        }
        game->release();
    }
    vector<int> result;
    bestScoreGame->copyResult(result);
    fprintf(stderr, "Remain: %d, Turn: %d\n", g_N*g_N - bestScoreGame->getScore(), bestScoreGame->getTurn());
    bestScoreGame->release();
    fprintf(stderr, "Time: %fsec\n", getTime());
    return result;
}



inline void GameChild::init(Game * game, int dir, int z, int hash, int score, int odd, int * remainW, int * remainH, bool beforeFlag) {
    this->game = game;
    this->dir = dir;
    this->z = z;
    this->hash = hash;
    this->score = score;
    this->beforeFlag = beforeFlag;
    if(game!=NULL) {
        int r0 = g_2NN-remainW[0]*remainW[0]-remainH[0]*remainH[0];
        int r1 = g_2NN-remainW[1]*remainW[1]-remainH[1]*remainH[1];
        int r2 = g_2NN-remainW[2]*remainW[2]-remainH[2]*remainH[2];
        int r3 = g_2NN-remainW[3]*remainW[3]-remainH[3]*remainH[3];
        int r4 = g_2NN-remainW[4]*remainW[4]-remainH[4]*remainH[4];
        int r5 = g_2NN-remainW[5]*remainW[5]-remainH[5]*remainH[5];
        int r6 = g_2NN-remainW[6]*remainW[6]-remainH[6]*remainH[6];
        r6 -= r5;
        r5 -= r4;
        r4 -= r3;
        r3 -= r2;
        r2 -= r1;
        r1 -= r0;
        if(g_N&1) {
            this->mix = this->score * 0.8 + r0 * 0.79 + r2 * 0.3 + r4 * 0.2 + r6 * 0.11 - odd*odd*0.3 - (beforeFlag ? (remainW[0]+remainH[0])*0.7 : 0.0);// + scSubAvg * 1.0;
        }
        else {
            this->mix = this->score * 0.8 + r0 * 0.79 + r2 * 0.3 + r4 * 0.2 + r6 * 0.11 - odd*odd*0.45 - (beforeFlag ? (remainW[0]+remainH[0])*0.7 : 0.0);// + scSubAvg * 1.0;
        }
        game->addRef();
    }
    else {
        this->mix = 0;
    }
}
