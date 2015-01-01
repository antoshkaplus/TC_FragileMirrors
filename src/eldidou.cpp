//
//  eldidou.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/14/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "eldidou.hpp"


#ifdef __amd64
#define LOCAL
#endif


#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstring>
#include <limits>
#include <queue>
#include <set>

namespace eldidou {

using namespace std;

#ifdef LOCAL
#define MAX_TIME 4.1
#else
#define MAX_TIME 10.
#endif

#ifdef __amd64
#define TIME_SCALE 3.0e9
__inline__ uint64_t rdtsc() { uint64_t a, d; __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d)); return (d<<32) | a; }
#else
#define TIME_SCALE 3.6e9
__inline__ uint64_t rdtsc() { uint64_t x; __asm__ volatile ("rdtsc" : "=A" (x)); return x; }
#endif

class Timer
{
public:
    uint64_t startTime;
    uint64_t elapsedTime;
    Timer() : startTime(getAbsoluteTime()), elapsedTime(0) {}
    ~Timer() { update(); }
    double update()
    {
        uint64_t absoluteTime = getAbsoluteTime();
        elapsedTime += absoluteTime - startTime;
        startTime = absoluteTime;
        return elapsedTime / TIME_SCALE;
    }
    uint64_t getAbsoluteTime() { /* return clock() / (double) CLOCKS_PER_SEC; */ return rdtsc(); }
};

class TimerGuard
{
public:
    Timer & timer;
    TimerGuard(Timer & timer) : timer(timer) {}
    ~TimerGuard() { timer.update(); }
};

// #define DEBUG

#ifdef DEBUG
#define DEBUG_TIMER() TimerGuard localTimerGuard(timers[__func__]);
#else
#define DEBUG_TIMER()
#endif

#define CALLBACK() do { \
timer.update(); \
if (callback != NULL) \
{ \
if (callback->callback(this, __func__)) \
quit = true; \
}\
timer.startTime = timer.getAbsoluteTime(); \
} while(0)

class FragileMirrorsCallback;

class Visu;

#define TOP    0
#define BOTTOM 1
#define LEFT   2
#define RIGHT  3

const int DR[] = {-1, 1, 0, 0};
const int DC[] = {0, 0, -1, 1};
const int LL[] = {RIGHT, LEFT, BOTTOM, TOP};
const int RR[] = {LEFT, RIGHT, TOP, BOTTOM};
const int UL[] = {false, true, false, true};
const int UR[] = {false, true, true, false};

class Ray
{
public:
    short r; short c; char d;
    Ray(){};
    Ray(short r, short c, char d) : r(r), c(c), d(d) {}
};

ostream& operator<<(ostream& o, const Ray& r)
{
    o << '[' << (int)r.r << ", " << (int)r.c << ", " << (int)r.d << ']';
    return o;
}

class EvaluatedRay
{
public:
    Ray ray;
    int score;
    EvaluatedRay(){};
    EvaluatedRay(Ray ray, int score) : ray(ray), score(score) {}
    bool operator<(const EvaluatedRay & other) const
    {
        return score > other.score;
    }
};

class Coord
{
public:
    short r; short c;
    Coord() : r(-1), c(-1) {}
    Coord(short r, short c) : r(r), c(c) {}
};

ostream& operator<<(ostream& o, const Coord& c)
{
    o << '[' << (int)c.r << ", " << (int)c.c << ']';
    return o;
}

class Cell : public Coord
{
public:
    Cell * next[4];
    bool border;
    char value;
    int groupUp;
    int groupDown;
};

ostream& operator<<(ostream& o, const Cell& c)
{
    if (c.border)
        o << '{' << (int)c.r << ", " << (int)c.c << "}    ";
    else
        o << '[' << (int)c.r << ", " << (int)c.c << "]    ";
    o << c.value << "    ";
    Coord * topCell = c.next[TOP];
    Coord * bottomCell = c.next[BOTTOM];
    Coord * leftCell = c.next[LEFT];
    Coord * rightCell = c.next[RIGHT];
    if (topCell != NULL)
        o << *topCell << ' ';
    else
        o << "NULL";
    if (bottomCell != NULL)
        o << *bottomCell << ' ';
    else
        o << "NULL";
    if (leftCell != NULL)
        o << *leftCell << ' ';
    else
        o << "NULL";
    if (rightCell != NULL)
        o << *rightCell << ' ';
    else
        o << "NULL";
    return o;
}

class Node
{
public:
    vector<Ray> rays;
    int mirrors;
    int groups;
    bool operator<(const Node & other) const
    {
        return mirrors > other.mirrors;
    }
};

ostream& operator<<(ostream& o, const Node & n)
{
    for (unsigned int r = 0; r < n.rays.size(); ++r)
        o << n.rays[r] << " ";
    o << "mirrors: " << n.mirrors << " groups: " << n.groups;
    return o;
}

class FragileMirrors
{
public:
    int size;
    FragileMirrorsCallback * callback;
    vector<int> destroy(vector<string> board);
    FragileMirrors() :
    callback(NULL),
    bestRay(-1, -1, -1)
    {
    }
private:
    Timer timer;
    vector<Coord> finalCoords;
    int lowerToUpper;
    Ray bestRay;
    vector<Ray> bestRays;
    vector<Ray> currentRays;
    int globalBestScore;
    Cell ** cells;
    int * rowSum;
    int * colSum;
    int mirrors;
    double estimatedScale;
    bool quit;
    double shootRay(const Ray & ray, vector<Coord> & coords);
    void shootRayGroup(const Ray & ray, int groupId);
    double tryRay(const Ray & ray, int level);
    double tryRay2(const Ray & ray, int level);
    double evalBoardGreedy();
    double evalBoardGroup();
    double evalBoardHeuristic();
    void cancel(vector<Coord> & coords);
    double findBestMove(int level);
    double findBestMove2(int level);
    int greedySearch(int level);
    int depthFirstSearch(int level);
    int aStarSearch();
    void parallelSearch();
    bool expandNode(const Node & parent, priority_queue<Node> * next);
    // void expandNode(Node & node);
    double eval(int level, int mirrors);
    double timeleft() { return MAX_TIME - timer.update(); }
#ifdef DEBUG
    map<string, Timer> timers;
    void printTimesElapsed()
    {
        for (map<string, Timer>::iterator it = timers.begin(); it != timers.end(); ++it)
            cerr << it->first << " " << it->second.update() << 's' << endl;
    }
#endif
    friend class Visu;
};

class FragileMirrorsCallback
{
public:
    virtual bool callback(FragileMirrors * fragileMirrors, string location = "") = 0;
    virtual ~FragileMirrorsCallback(){};
};


double FragileMirrors::shootRay(const Ray & ray, vector<Coord> & coords)
{
    int d = ray.d;
    Cell * cell = &cells[ray.r][ray.c];
    
    coords.push_back(Coord(cell->r, cell->c));
    cell = cell->next[d];
    
    // int isolated = 0;
    while (!cell->border)
    {
        int r = cell->r;
        int c = cell->c;
        
        Cell * topCell = cell->next[TOP];
        Cell * bottomCell = cell->next[BOTTOM];
        Cell * leftCell = cell->next[LEFT];
        Cell * rightCell = cell->next[RIGHT];
        
        topCell->next[BOTTOM] = bottomCell;
        bottomCell->next[TOP] = topCell;
        leftCell->next[RIGHT] = rightCell;
        rightCell->next[LEFT] = leftCell;
        
        coords.push_back(Coord(r, c));
        
        d = (cell->value == 'L' ? LL[d] : RR[d]);
        
        cell->value -= lowerToUpper;
        --rowSum[r];
        --colSum[c];
        
        // if ()
        //     ++isolated;
        // if (colSum[c] == 0)
        //     ++isolated;
        // if (rowSum[r] == 0 || colSum[c] == 0)
        //     --isolated;
        
        cell = cell->next[d];
    }
    
    coords.push_back(Coord(cell->r, cell->c));
    int broken = coords.size() - 2;
    mirrors -= broken;
    return broken;
}

void FragileMirrors::shootRayGroup(const Ray & ray, int groupId)
{
    Cell * cell = &cells[ray.r][ray.c];
    int d = ray.d;
    bool up = (cell->value == 'L' ? UL[d] : UR[d]);;
    do
    {
        // cout << *cell << " " << up << " " << d << endl;
        if (up)
            cell->groupUp = groupId;
        else
            cell->groupDown = groupId;
        
        d = (cell->value == 'L' ? LL[d] : RR[d]);
        cell = cell->next[d];
        
        up = (cell->value == 'L' ? UL[d] : UR[d]);
    }
    while (!cell->border && ((up && cell->groupUp == 0) || (!up && cell->groupDown == 0)));
}

void FragileMirrors::cancel(vector<Coord> & coords)
{
    for (unsigned int coord = coords.size()-2; coord > 0; --coord)
    {
        int r = coords[coord].r;
        int c = coords[coord].c;
        Cell * cell = &cells[r][c];
        
        Cell * topCell = cell->next[TOP];
        Cell * bottomCell = cell->next[BOTTOM];
        Cell * leftCell = cell->next[LEFT];
        Cell * rightCell = cell->next[RIGHT];
        
        topCell->next[BOTTOM] = cell;
        bottomCell->next[TOP] = cell;
        leftCell->next[RIGHT] = cell;
        rightCell->next[LEFT] = cell;
        
        cell->value += lowerToUpper;
        ++rowSum[r];
        ++colSum[c];
        // cout << __func__ << " " << *cell << endl;
    }
    mirrors += coords.size() -2;
    coords.clear();
}

double FragileMirrors::tryRay2(const Ray & ray, int level)
{
    vector<Coord> coords;
    double rayScore = shootRay(ray, coords);
    double boardScore = findBestMove2(level+1);
    cancel(coords);
    // cout << __func__ << " " << ray << "   " << rayScore << " " << boardScore << "   " << level << endl;
    return rayScore + boardScore;
}

double FragileMirrors::findBestMove2(int level)
{
    // if (level == 0)
    // cout << __func__ << endl;
    if (mirrors == 0)
        return 100000. / (level + 1);
    
    bool prune = timeleft() < .4 * MAX_TIME;
    
    if (level == 0 || (level == 1 && !prune))
        // if (level < 2)
    {
        double score;
        double bestScore = -numeric_limits<double>::max();
        vector<Coord> coords;
        
        for (int rc = 0; rc < size; ++rc)
        {
            if (rowSum[rc] > 0)
            {
                Ray rayLeft(rc, size, LEFT);
                score = tryRay2(rayLeft, level);
                if (score >= bestScore)
                {
                    bestScore = score;
                    if (level == 0)
                        bestRay = rayLeft;
                }
                
                Ray rayRight(rc, -1, RIGHT);
                score = tryRay2(rayRight, level);
                if (score >= bestScore)
                {
                    bestScore = score;
                    if (level == 0)
                        bestRay = rayRight;
                }
            }
            
            if (colSum[rc] > 0)
            {
                Ray rayTop(size, rc, TOP);
                score = tryRay2(rayTop, level);
                if (score >= bestScore)
                {
                    bestScore = score;
                    if (level == 0)
                        bestRay = rayTop;
                }
                
                Ray rayBottom(-1, rc, BOTTOM);
                score = tryRay2(rayBottom, level);
                if (score >= bestScore)
                {
                    bestScore = score;
                    if (level == 0)
                        bestRay = rayBottom;
                }
            }
        }
        
        return bestScore;
        // cout << bestRay << endl;
    }
    
    return 0.;
}

double FragileMirrors::tryRay(const Ray & ray, int level)
{
    vector<Coord> coords;
    double rayScore = shootRay(ray, coords);
    double boardScore = evalBoardGreedy();
    cancel(coords);
    // cout << __func__ << " " << ray << "   " << rayScore << " " << boardScore << "   " << level << endl;
    return 0.001 * rayScore - boardScore;
}

double FragileMirrors::findBestMove(int level)
{
    // cout << __func__ << endl;
    if (mirrors == 0)
        return 100000. / (level + 1);
    
    double score;
    double bestScore = -numeric_limits<double>::max();
    vector<Coord> coords;
    
    for (int rc = 0; rc < size; ++rc)
    {
        if (rowSum[rc] > 0)
        {
            Ray rayLeft(rc, size, LEFT);
            score = tryRay(rayLeft, level);
            if (score > bestScore)
            {
                bestScore = score;
                bestRay = rayLeft;
            }
            
            Ray rayRight(rc, -1, RIGHT);
            score = tryRay(rayRight, level);
            if (score > bestScore)
            {
                bestScore = score;
                bestRay = rayRight;
            }
        }
        
        if (colSum[rc] > 0)
        {
            Ray rayTop(size, rc, TOP);
            score = tryRay(rayTop, level);
            if (score > bestScore)
            {
                bestScore = score;
                bestRay = rayTop;
            }
            
            Ray rayBottom(-1, rc, BOTTOM);
            score = tryRay(rayBottom, level);
            if (score > bestScore)
            {
                bestScore = score;
                bestRay = rayBottom;
            }
        }
    }
    // cout << bestScore << " " << bestRay << endl;
    return bestScore;
}

double FragileMirrors::evalBoardGroup()
{
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
        {
            cells[r][c].groupUp = 0;
            cells[r][c].groupDown = 0;
        }
    
    int groupId = 0;
    for (int r = 0; r < size; ++r)
    {
        Cell * cell = cells[r][-1].next[RIGHT];
        while (!cell->border)
        {
            if (cell->groupUp == 0)
            {
                ++groupId;
                Ray ray(r, cell->c, BOTTOM);
                shootRayGroup(ray, groupId);
            }
            if (cell->groupDown == 0)
            {
                ++groupId;
                Ray ray(r, cell->c, TOP);
                shootRayGroup(ray, groupId);
                
                if (cell->value == 'L')
                {
                    Ray ray2(r, cell->c, LEFT);
                    shootRayGroup(ray2, groupId);
                }
                else
                {
                    Ray ray2(r, cell->c, RIGHT);
                    shootRayGroup(ray2, groupId);
                }
            }
            cell = cell->next[RIGHT];
        }
    }
    return groupId;
}

double FragileMirrors::evalBoardHeuristic()
{
    int res = 0;
    for (int rc = 0; rc < size; ++rc)
    {
        if (rowSum[rc] == 1)
            res += 1;
        if (colSum[rc] == 1)
            res += 1;
        res += rowSum[rc];
    }
    return res;
}

double FragileMirrors::evalBoardGreedy()
{
    int nbRay = 0;
    vector<vector<Coord> > cancelList;
    while (mirrors > 0)
    {
        double score;
        double bestScore = -numeric_limits<double>::max();
        vector<Coord> coords;
        Ray bestGreedyRay;
        
        for (int rc = 0; rc < size; ++rc)
        {
            if (rowSum[rc] > 0)
            {
                Ray rayLeft(rc, size, LEFT);
                score = shootRay(rayLeft, coords);
                if (score >= bestScore)
                {
                    bestScore = score;
                    bestGreedyRay = rayLeft;
                }
                cancel(coords);
                
                Ray rayRight(rc, -1, RIGHT);
                score = shootRay(rayRight, coords);
                if (score >= bestScore)
                {
                    bestScore = score;
                    bestGreedyRay = rayRight;
                }
                cancel(coords);
            }
            
            if (colSum[rc] > 0)
            {
                Ray rayTop(size, rc, TOP);
                score = shootRay(rayTop, coords);
                if (score >= bestScore)
                {
                    bestScore = score;
                    bestGreedyRay = rayTop;
                }
                cancel(coords);
                
                Ray rayBottom(-1, rc, BOTTOM);
                score = shootRay(rayBottom, coords);
                if (score >= bestScore)
                {
                    bestScore = score;
                    bestGreedyRay = rayBottom;
                }
                cancel(coords);
            }
        }
        
        // CALLBACK();
        
        ++nbRay;
        shootRay(bestGreedyRay, coords);
        cancelList.push_back(coords);
    }
    for (int c = cancelList.size()-1; c >= 0; --c)
        cancel(cancelList[c]);
    
    return nbRay;
}

int FragileMirrors::greedySearch(int level)
{
    vector<Ray> rays;
    for (int rc = 0; rc < size; ++rc)
    {
        if (rowSum[rc] > 0)
        {
            rays.push_back(Ray(rc, size, LEFT));
            rays.push_back(Ray(rc, -1, RIGHT));
        }
        if (colSum[rc] > 0)
        {
            rays.push_back(Ray(size, rc, TOP));
            rays.push_back(Ray(-1, rc, BOTTOM));
        }
    }
    
    double bestScore = -numeric_limits<double>::max();
    vector<Coord> coords;
    Ray bestGreedyRay;
    for (unsigned int r = 0; r < rays.size(); ++r)
    {
        Ray & ray = rays[r];
        double score = shootRay(ray, coords);
        // if (level > 50)
        //     score -= 5 * evalBoardGroup();
        cancel(coords);
        
        if (score > bestScore)
        {
            bestScore = score;
            bestGreedyRay = ray;
        }
    }
    
    shootRay(bestGreedyRay, coords);
    
    // evalBoardGroup();
    // CALLBACK();
    
    int score;
    if (mirrors > 0)
    {
        if (level+1 == globalBestScore)
            score = 2;
        else
        {
            currentRays.push_back(bestGreedyRay);
            score = greedySearch(level + 1) + 1;
            currentRays.pop_back();
        }
    }
    else
    {
        score = 1;
        if (level+1 < globalBestScore)
        {
            globalBestScore = level+1;
            bestRays = currentRays;
            bestRays.push_back(bestGreedyRay);
        }
    }
    
    cancel(coords);
    
    return score;
}

int FragileMirrors::depthFirstSearch(int level)
{
    if (mirrors <= size)
        return greedySearch(level);
    
    vector<Ray> rays;
    for (int rc = 0; rc < size; ++rc)
    {
        if (rowSum[rc] > 0)
        {
            rays.push_back(Ray(rc, size, LEFT));
            rays.push_back(Ray(rc, -1, RIGHT));
        }
        if (colSum[rc] > 0)
        {
            rays.push_back(Ray(size, rc, TOP));
            rays.push_back(Ray(-1, rc, BOTTOM));
        }
    }
    
    vector<Coord> coords;
    vector<EvaluatedRay> evaluatedRays;
    
    for (unsigned int r = 0; r < rays.size(); ++r)
    {
        Ray & ray = rays[r];
        double score = shootRay(ray, coords);
        cancel(coords);
        
        evaluatedRays.push_back(EvaluatedRay(ray, score));
    }
    sort(evaluatedRays.begin(), evaluatedRays.end());
    
    if (evaluatedRays.size() > 2)
        evaluatedRays.resize(2);
    
    int score = numeric_limits<int>::max();
    for (unsigned int r = 0; r < evaluatedRays.size(); ++r)
    {
        Ray & ray = evaluatedRays[r].ray;
        
        shootRay(ray, coords);
        
        // for (int i = 0; i < level; ++i)
        //     cout << "    ";
        // cout << ray << endl;
        CALLBACK();
        
        if (mirrors > 0)
        {
            if (level+1 == globalBestScore)
                score = 2;
            else
            {
                currentRays.push_back(ray);
                score = depthFirstSearch(level+1) + 1;
                currentRays.pop_back();
            }
        }
        else
        {
            score = 1;
            if (level+1 < globalBestScore)
            {
                if (globalBestScore != numeric_limits<int>::max())
                    cout << endl << globalBestScore - level-1 << " " << globalBestScore << " " << level+1 << endl;
                globalBestScore = level+1;
                bestRays = currentRays;
                bestRays.push_back(ray);
            }
        }
        
        cancel(coords);
        
        if (quit || timeleft() < .1 * MAX_TIME)
            break;
        // return numeric_limits<int>::max();
    }
    
    return score;
}

void FragileMirrors::parallelSearch()
{
    priority_queue<Node> flip;
    priority_queue<Node> flop;
    
    priority_queue<Node> * current = &flip;
    priority_queue<Node> * next = &flop;
    
    bool finish = false;
    
    Node root;
    root.mirrors = 0;
    root.groups = 0;
    current->push(root);
    
    int iter = 0;
    
    do
    {
        ++iter;
        const Node & firstNode = current->top();
        vector<vector<Coord> > cancelList;
        for (unsigned int r = 0; r < firstNode.rays.size(); ++r)
        {
            vector<Coord> coords;
            const Ray & ray = firstNode.rays[r];
            shootRay(ray, coords);
            cancelList.push_back(coords);
        }
        
        int estimatedNbRay = ceil(.5 * evalBoardGreedy());
        double timeMargin = .05 * MAX_TIME;
        double timeLimit = timeMargin + (timeleft() - timeMargin) * (estimatedNbRay - 1) / estimatedNbRay;
        // cout << "============= " << iter << " " << timeLimit << " " << current->size() << " =============" << endl;
        
        // evalBoardGroup();
        // CALLBACK();
        
        for (int c = cancelList.size()-1; c >= 0; --c)
            cancel(cancelList[c]);
        
        while (!current->empty())
        {
            Node node = current->top();
            current->pop();
            finish = expandNode(node, next);
            if (finish || timeleft() < timeLimit)
                break;
        }
        
        if (!finish)
        {
            *current = priority_queue<Node>();
            swap(current, next);
        }
    } while (!finish);
    
}

bool FragileMirrors::expandNode(const Node & parent, priority_queue<Node> * next)
{
    // cout << parent << endl;
    vector<vector<Coord> > cancelList;
    for (unsigned int r = 0; r < parent.rays.size(); ++r)
    {
        vector<Coord> coords;
        const Ray & ray = parent.rays[r];
        shootRay(ray, coords);
        cancelList.push_back(coords);
    }
    
    // cout << parent << " " << evalBoardGroup() << endl;
    // CALLBACK();
    
    vector<Ray> rays;
    for (int rc = 0; rc < size; ++rc)
    {
        if (rowSum[rc] > 0)
        {
            rays.push_back(Ray(rc, size, LEFT));
            rays.push_back(Ray(rc, -1, RIGHT));
        }
        if (colSum[rc] > 0)
        {
            rays.push_back(Ray(size, rc, TOP));
            rays.push_back(Ray(-1, rc, BOTTOM));
        }
    }
    
    for (unsigned int r = 0; r < rays.size(); ++r)
    {
        Ray & ray = rays[r];
        vector<Coord> coords;
        shootRay(ray, coords);
        if (mirrors == 0)
        {
            bestRays = parent.rays;
            bestRays.push_back(ray);
            
            cancel(coords);
            for (int c = cancelList.size()-1; c >= 0; --c)
                cancel(cancelList[c]);
            
            return true;
        }
        else
        {
            Node node;
            node.rays = parent.rays;
            node.rays.push_back(ray);
            // node.mirrors = mirrors;
            node.mirrors = evalBoardHeuristic();
            
            next->push(node);
            
            // finalCoords = coords;
            // cout << "   expand " << node << endl;
            // CALLBACK();
        }
        
        cancel(coords);
    }
    
    for (int c = cancelList.size()-1; c >= 0; --c)
        cancel(cancelList[c]);
    
    return false;
}

vector<int> FragileMirrors::destroy(vector<string> board)
{
    TimerGuard timerGuard(timer);
    quit = false;
    
    lowerToUpper = 'A' - 'a';
    size = board.size();
    
    cells = (new Cell*[size+2])+1;
    for (int r = -1; r < size+1; ++r)
        cells[r] = (new Cell[size+2])+1;
    
    for (int r = -1; r < size+1; ++r)
        for (int c = -1; c < size+1; ++c)
        {
            Cell * cell = &(cells[r][c]);
            
            cell->c = c;
            cell->r = r;
            cell->groupUp = 0;
            cell->groupDown = 0;
            
            if (r >= 0)
                cell->next[TOP] = &(cells[r-1][c]);
            else
                cell->next[TOP] = NULL;
            
            if (r < size)
                cell->next[BOTTOM] = &(cells[r+1][c]);
            else
                cell->next[BOTTOM] = NULL;
            
            if (c >= 0)
                cell->next[LEFT] = &(cells[r][c-1]);
            else
                cell->next[LEFT] = NULL;
            
            if (c < size)
                cell->next[RIGHT] = &(cells[r][c+1]);
            else
                cell->next[RIGHT] = NULL;
            
            cell->border = (r < 0 || r >= size || c < 0 || c >= size);
            if (cell->border)
                cell->value = 'R';
        }
    
    rowSum = new int[size];
    colSum = new int[size];
    for (int r = 0; r < size; ++r)
        rowSum[r] = size;
    for (int c = 0; c < size; ++c)
        colSum[c] = size;
    
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            cells[r][c].value = board[r][c];
    
    globalBestScore = numeric_limits<int>::max();
    
    mirrors = size*size;
    estimatedScale = 100.;
    
    vector<int> ofTheJedi;
    parallelSearch();
    
    if (!quit)
        for (unsigned int r = 0; r < bestRays.size(); ++r)
        {
#ifdef LOCAL
            finalCoords.clear();
            shootRay(bestRays[r], finalCoords);
            CALLBACK();
#endif
            
            ofTheJedi.push_back(bestRays[r].r);
            ofTheJedi.push_back(bestRays[r].c);
        }
    return ofTheJedi;
}

}
