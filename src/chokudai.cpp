//
//  chokudai.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/13/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//


#include "chokudai.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include <set>
#include <map>

namespace chokudai {
    using namespace std;

#define TIME 9500
#define ll long long

#ifdef _MSC_VER
#include <Windows.h>
ll getTime() {
    return GetTickCount();
}
#else
#include <sys/time.h>
ll getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ll result =  tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
    //cerr << result << endl;
    return result;
}
#endif

unsigned ll xor128() {
    static unsigned ll rx=123456789, ry=362436069, rz=521288629, rw=88675123;
    unsigned ll rt = (rx ^ (rx<<11));
    rx=ry; ry=rz; rz=rw;
    return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

ll starttime;

void settime(){
    starttime = getTime();
}

ll nowtime(){
    return getTime() - starttime;
}

int prehash[hashmax];
int nowhashcount;

ll keys[COUNTMAX];
int nextpos[COUNTMAX];
int nownn[COUNTMAX];
int values[COUNTMAX];

void inithash(){
    nowhashcount = 0;
    for(int i=0;i<hashmax;i++) prehash[i]=-1;
}

int searchhash(ll a){
    int h = (int)(a & (hashmax-1));
    int nowpos = prehash[h];
    while(nowpos != -1){
        if(keys[nowpos] == a)
            return values[nowpos];
        nowpos = nextpos[nowpos];
    }
    return 0;
}

void pushhash(ll a, int b){
    int h = (int)(a & (hashmax-1));
    nextpos[nowhashcount] = prehash[h];
    prehash[h] = nowhashcount;
    keys[nowhashcount] = a;
    values[nowhashcount] = b;
    nowhashcount++;
}


int ly[COUNTMAX];
int lx[COUNTMAX];
int lpre[COUNTMAX];

int nokori;
int nokori2;

int nextnum[NMAX * NMAX][4];
int type[NMAX * NMAX];
ll hashnum[NMAX * NMAX];
ll nowhash;

int removenext[NMAX * NMAX];
int turn[NMAX * NMAX];

int preodd[NMAX*NMAX];

int nowturn;

int odd;

int affect[NMAX * 4];
vector<int> addaffect[NMAX * NMAX];



int N;

int best;

int nokoridic[COUNTMAX];
int retnum;


int backy[NMAX];
int backx[NMAX];


int prechanged[NMAX * NMAX];
int precount;

int ccount;
int getpoint[NMAX];

priority_queue<pair<int, ll> >hs[1000];

int prestart;
int removelighty[NMAX * NMAX];
int removelightx[NMAX * NMAX];

int tate[NMAX];
int yoko[NMAX];

vector<int> FragileMirrors::destroy(vector<string> board)
{
    settime();
    N = board.size();
    cerr << "N = " << N << endl;
    best = N * 4 / 3;
    if(N%2==0) best = N + 1;
    init(board);
    
    pushhash(nowhash, -1);
    nokori = N * N;
    //nokoridic[ -1] = 0;
    hs[0].push(make_pair(0, nowhash));
    ccount = 1;
    
    int nn = 0;
    prestart = -1;
    int cc = 0;
    int ttc = 0;
    cerr << nowtime() << endl;
    
    while (true)
    {
        if ((ttc & 31) == 1)
        {
            nn++;
            if((ttc & 511) == 1){
                if(nowtime() >= TIME) break;
            }
            if (nn >= best - 1) nn = 1;
        }
        ttc++;
        if (hs[nn].size() == 0) continue;
        cc++;
        pair<int, ll> now = hs[nn].top();
        hs[nn].pop();
        
        int start = searchhash(now.second);
        int nownum = start;
        if (start != -1 && nn + nokoridic[start] >= best)
        {
            continue;
        }
        //cerr << "test" << endl;
        
        vector<int> vi;
        int nt = nn;
        int tnn = nn;
        int backcount = 0;
        
        while (prestart != nownum)
        {
            if (nt > nowturn)
            {
                if(nownum== - 1) break;
                backy[backcount] = ly[nownum];
                backx[backcount] = lx[nownum];
                backcount++;
                nownum = lpre[nownum];
                nt--;
            }
            else
            {
                if(prestart== - 1) break;
                back();
                prestart = lpre[prestart];
            }
        }
        
        
        if(prestart!=nownum){
            //cerr << "myon1" << endl;
            while(nownum!=-1){
                tnn++;
                backy[backcount] = ly[nownum];
                backx[backcount] = lx[nownum];
                backcount++;
                nownum = lpre[nownum];
            }
            while(prestart!=-1){
                tnn--;
                back();
                prestart = lpre[prestart];
            }
        }
        prestart = start;
        
        
        for (int i = backcount - 1; i >= 0; i--)
        {
            erase(backy[i],  backx[i]);
        }
        
        if (nowturn + (odd >> 1) >= best)
        {
            continue;
        }
        
        
        //cerr << nn << " " << nokoridic[now.second] << " " << nokori << " " << endl;
        
        bool flag = false;
        bool flag2 = true;
        int prei = -1;
        int prej = -1;
        int prenokori = nokori;            
        
        //cerr << "myon" << endl;
        for (int i = 0; i < 4; i++)
        {
            if (flag) break;
            for (int j = 0; j < N; j++)
            {
                if (!(i & 1) && !yoko[j + 1]) continue;
                if ((i & 1) && !tate[j + 1]) continue;
                if (tnn && tnn <= best - 6 && !affect[i * N + j]) continue;
                lighterase(i, j);
                
                int dec = prenokori - nokori;
                if (dec > 1) flag2 = false;
                else
                {
                    if (prenokori - nokori == 1)
                    {
                        prei = i;
                        prej = j;
                    }
                    lightback();
                    continue;
                }
                if (nowturn + (odd >> 1) >= best)
                {
                    lightback();
                    continue;
                }
                if(tnn > 1){
                    if(N%2==0){
                        if(nokori > 50 && dec < 5){
                            lightback();
                            continue;
                        }
                        if(nokori > 200 && dec < 10){
                            lightback();
                            continue;
                        }
                        if(nokori > 500 && dec < 20){
                            lightback();
                            continue;
                        }
                    }
                    else{
                        if(nokori > 50 && dec < 5){
                            lightback();
                            continue;
                        }
                        if(nokori > 500 && dec < 10){
                            lightback();
                            continue;
                        }
                    }
                }
                
                int thash;
                if ((thash = searchhash(nowhash)) != 0)
                {
                    if(nownn[thash] <= tnn + 1){
                        lightback();
                        continue;
                    }
                    //cerr << "myon2" << endl;
                }
                else thash = ccount++;
                
                if (nokori == 0)
                {
                    retnum = thash;
                    ly[thash]= i;
                    lx[thash] = j;
                    lpre[thash] = start;
                    nownn[thash] = tnn + 1;
                    cerr << "new score: " << (tnn + 1) << " " << nowtime() << endl;
                    best = tnn + 1;
                    flag = true;
                }
                else
                {
                    pushhash(nowhash, thash);
                    ly[thash] = i;
                    lx[thash] = j;
                    lpre[thash] = start;
                    nokoridic[thash] = odd >> 1;
                    nownn[thash] = tnn + 1;
                    hs[tnn+1].push(make_pair(getscore(tnn+1),nowhash));
                }
                lightback();
                if (flag) break;
            }
        }
        if (flag2 && prei != -1)
        {
            if (nowturn + (odd >> 1) < best)
            {
                lighterase(prei, prej);
                
                if (nokori == 0)
                {
                    retnum = ccount;
                    ly[ccount] = prei;
                    lx[ccount] = prej;
                    lpre[ccount] = start;
                    ccount++;
                    cerr << "new score: " << (tnn + 1) << " " << nowtime() << endl;
                    best = nn + 1;
                }
                else
                {
                    pushhash(nowhash, ccount);
                    ly[ccount] = prei;
                    lx[ccount] = prej;
                    lpre[ccount] = start;
                    nokoridic[ccount] = odd >> 1;
                    nownn[ccount] = tnn + 1;
                    ccount++;
                    
                    hs[tnn + 1].push(make_pair(getscore(tnn + 1), nowhash));
                }
                lightback();
            }
        }
    }
    cerr << cc << " " << ccount << endl;
    return makeret();
}
    
vector<int> FragileMirrors::makeret()
{
    vector<int> ret;
    for(int i = 0;i < best;i++){ ret.push_back(-100); ret.push_back(-100);}
    for (int i = best - 1; i >= 0; i--)
    {
        ret[i * 2] = ly[retnum];
        ret[i * 2 + 1] = lx[retnum];
        retnum = lpre[retnum];
    }
    for (int i = 0; i < best; i++)
    {
        int a = ret[i * 2];
        int b = ret[i * 2 + 1];
        if (a == 2)
        {
            ret[i * 2] = -1;
            ret[i * 2 + 1] = b;
        }
        else if (a == 3)
        {
            ret[i * 2] = b;
            ret[i * 2 + 1] = -1;
        }
        else if (a == 0)
        {
            ret[i * 2] = N;
            ret[i * 2 + 1] = b;
        }
        else
        {
            ret[i * 2] = b;
            ret[i * 2 + 1] = N;
        }
    }
    return ret;
}
       
int FragileMirrors::getscore(int turn)
{
    return -(int)(nokori2 + (odd * turnscore[turn]));
}


void FragileMirrors::init(vector<string> b)
{
    inithash();
    nokori = N * N;
    nokori2 = 0;
    
    for(int i=0;i<200;i++){
        if(N%2==0) turnscore[i] = 11 << 12;
        else turnscore[i] = (int)pow(3 << 15, pow((double)i / N, 0.05));
    }
    
    odd = N * 2 * (N & 1);
    
    nowturn = 0;
    precount = 1;
    nowhash = 0;
    turn[0] = nokori;
    
    for (int now = 0; now < NMAX * NMAX; now++)
    {
        nextnum[now][0] = now - NMAX;
        nextnum[now][1] = now - 1;
        nextnum[now][2] = now + NMAX;
        nextnum[now][3] = now + 1;
        type[now] = 2;
        hashnum[now] = (ll)xor128();
        prechanged[now] = 0;
    }
    
    for (int i = 0; i < NMAX; i++)
    {
        tate[i] = yoko[i] = N;
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int now = ((i + 1) << NSHIFT) + (j + 1);
            if (b[i][j] == 'L') type[now] = 1;
            else type[now] = 3;
        }
    }
    
    for(int i=1;i<NMAX;i++){
        getpoint[i] = (((1 & i) | (1 & (N + 1))) << 16) / (3 * i - 2);
        //getpoint[i] = (1 << 16) / (3 * i - 2);
    }
    
    for(int i=1;i<=N;i++){
        starterase[i-1][0] = ((N + 1) << NSHIFT) + i;
        starterase[i-1][1] = (i << NSHIFT) + N + 1;
        starterase[i-1][2] = i;
        starterase[i-1][3] = (i << NSHIFT);
    }
    
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < N; j++)
        {
            inputerase(i, j, i * N + j);
            inputback();
        }
    }
    
}
    
void FragileMirrors::erase(int a, int b)
{
    int t = a;
    int now = nextnum[starterase[b][a]][a]; 
    
    preodd[nowturn] = odd;
    
    if ((t & 1) == 1)
    {
        if (tate[now >> NSHIFT] % 2 == 1) odd--; 
        else odd++; 
    }
    else
    {
        if (yoko[now & (NMAX - 1)] % 2 == 1) odd--; 
        else odd++; 
    }
    
    prenokori2[nowturn] = nokori2;
    prehash[nowturn] = nowhash;
    
    while (type[now] != 2)
    {
        int* nar = nextnum[now];
        t ^= 2;
        
        removenext[nokori] = now;
        nokori--;
        
        int y = now >> NSHIFT;
        int x = now &(NMAX-1);
        nokori2 -= getpoint[tate[y]];
        nokori2 -= getpoint[yoko[x]];
        tate[y]--; yoko[x]--;
        nowhash ^= hashnum[now];
        
        for(int j = 0; j < (int)addaffect[now].size(); j++)
        {
            affect[addaffect[now][j]]++;
        }
        
        nextnum[nar[0]][2] = nar[2];
        nextnum[nar[2]][0] = nar[0];
        nextnum[nar[1]][3] = nar[3];
        nextnum[nar[3]][1] = nar[1];
        
        now = nar[t ^= type[now]];
    }
    
    if ((t & 1) == 1)
    {
        if (tate[now >> NSHIFT] % 2 == 0) odd--; 
        else odd++;
    }
    else
    {
        if (yoko[now & (NMAX-1)] % 2 == 0) odd--;
        else odd++; 
    }
    
    turn[++nowturn] = nokori;
}

void FragileMirrors::back()
{
    nowturn--;
    odd = preodd[nowturn];
    
    nokori2 = prenokori2[nowturn];
    nowhash = prehash[nowturn];
    
    while (nokori < turn[nowturn])
    {
        nokori++;
        int now = removenext[nokori];
        int* nar = nextnum[now];
        nextnum[nar[0]][2] = now;
        nextnum[nar[2]][0] = now;
        nextnum[nar[1]][3] = now;
        nextnum[nar[3]][1] = now;
        
        
        int y = now >> NSHIFT;
        int x = now & (NMAX-1);
        tate[y]++; yoko[x]++;
        
        for(int j = 0; j < (int)addaffect[now].size(); j++)
        {
            affect[addaffect[now][j]]--;
        }
    }
}

void FragileMirrors::inputerase(int a, int b, int num)
{
    int t = a;
    int now = nextnum[starterase[b][a]][a]; 
    
    while (type[now] != 2)
    {
        int* nar = nextnum[now];
        t ^= 2;
        removenext[nokori] = now;
        nokori--;
        addaffect[now].push_back(num);
        nextnum[nar[0]][2] = nar[2];
        nextnum[nar[2]][0] = nar[0];
        nextnum[nar[1]][3] = nar[3];
        nextnum[nar[3]][1] = nar[1];
        
        now = nar[t ^= type[now]];
    }
    turn[++nowturn] = nokori;
}

void FragileMirrors::inputback()
{
    nowturn--;
    while (nokori < turn[nowturn])
    {
        nokori++;
        int now = removenext[nokori];
        int* nar = nextnum[now];
        nextnum[nar[0]][2] = now;
        nextnum[nar[2]][0] = now;
        nextnum[nar[1]][3] = now;
        nextnum[nar[3]][1] = now;
    }
}

void FragileMirrors::lighterase(int a, int b)
{
    precount++;
    int t = a;
    int now = nextnum[starterase[b][a]][a]; 
    
    preodd[nowturn] = odd;
    
    if ((t & 1) == 1)
    {
        if (tate[now >> NSHIFT] % 2 == 1) odd--; 
        else odd++; 
    }
    else
    {
        if (yoko[now & (NMAX - 1)] % 2 == 1) odd--; 
        else odd++; 
    }
    
    prenokori2[nowturn] = nokori2;
    prehash[nowturn] = nowhash;
    
    while (type[now] != 2)
    {
        int* nar = nextnum[now];
        if (prechanged[now] == precount)
        {
            now = nar[t];
            continue;
        }
        prechanged[now] = precount;
        t ^= 2;
        
        
        int y = removelighty[nokori] = now >> NSHIFT;
        int x = removelightx[nokori] = now & (NMAX-1);
        nokori--;
        nokori2 -= getpoint[tate[y]];
        nokori2 -= getpoint[yoko[x]];
        tate[y]--; yoko[x]--;
        nowhash ^= hashnum[now];
        
        now = nar[t ^= type[now]];
    }
    
    if ((t & 1) == 1)
    {
        if (tate[now >> NSHIFT] % 2 == 0) odd--; 
        else odd++;
    }
    else
    {
        if (yoko[now & (NMAX-1)] % 2 == 0) odd--;
        else odd++; 
    }
    
    
    turn[++nowturn] = nokori;
}
    
void FragileMirrors::lightback()
{
    nowturn--;
    odd = preodd[nowturn];
    
    nokori2 = prenokori2[nowturn];
    nowhash = prehash[nowturn];
    
    while (nokori < turn[nowturn])
    {
        nokori++;
        tate[removelighty[nokori]]++; yoko[removelightx[nokori]]++;
        
    }
}

}