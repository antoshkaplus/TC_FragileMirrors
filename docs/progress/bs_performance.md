Used 4 cores on 6 core processor to reduce system processing interference. 
Processor: Intel® Core™ i5-9600K CPU @ 3.70GHz.

If not specified, test uses:
* --beam-width=300
* --score=Score_i1, which is based solely on destroyed count.
* --test-count=100 - run score.py 1-100 seeds. (Should not use zero seed)

### Native Board: Grid of Mirrors that set to Destroyed on Cast

The simplest way to store a board is as a square Grid. 
Each element is a one byte `Mirror`.  

Using BS_Greedy_i1 perf is: ~85 sec.

Adding col/row elements counts with Board_i3. 
BS_Score_i1 perf is: ~220-240 sec.

If we skip empty lines from traversing 
using Board_i4 perf is: ~142-156 sec.

#### ~

`-Ofast` gives a small advantage in like maybe 4-5%. Maybe no advantage at all.
`-march=native` does not improve.

There could be slight improvement traversing boards backwards to do casts.
But it's negligible. 

We also tried to replace `std::multiset` with `boost::priority_queue` in solver BS_Score_i2.
The container stores next iteration boards for BS method.  
We got a negligible improvement. It's clear that the real problem is casting performance.

#### Failure

With Board_i5 we tried to introduce `Restore` mechanic, where it's possible:
* Do cast on a board.
* If condition met, copy resulting board to the next level boards array.
* Restore the board to the original state before the cast.
So it supposed to save time: usually we always copy the board, 
regardless of it will be taken to the next level or not.

Using BS_Restore_i1 and BS_Restore_i2 didn't provide any meaningful result. 
Even though we tried partial solutions:
* First do copy always
* Then switch to using `Restore` mechanic
and vice versa too. Also at different emptiness parameter values.

Probably casting through many destroyed cells is the most
time-consuming operation. With that copy is not a big deal. 
After copy, memory is still hot, easier to perform operations.

### Neighbour Board: Grid

Underlying structure is still a grid, but each cell contains four links, one per direction,
to the next non-destroyed mirror/border.

Using Board_n_i1 with BS_Greedy_i1 perf: ~150 sec.
 
Instead of computing next level boards on the fly, we can compute `BoardParams` for each possible cast first,
pick best among them, and perform actual board copy/cast. `BoardParams` are done with `Restore` mechanic. 
Using Board_n_i2 with BS_Restore_i3 perf: ~42.12 sec. (tracks empty row/col counts).

If not specified, test uses: BS_Restore_i3.

### Neighbour Board: 1D arrays

If we use Grid representation, it becomes hard to reduce board space requirements as we remove mirrors.
That can be solved by using 1D array, shrinking it as mirrors get removed.

Without downsizing Board_r perf: 44.38 sec.
With removal of destroyed mirror cells Board_r2 perf: ~31.67 sec.

If we also remove casts on empty lines Board_r3 perf: ~32.43 sec. 
It does not seem to improve the score, but still helps to traverse only available casts. 

We get to the point where it's hard to see a difference, so we make test use: 
* --beam-width=500
* --test-count=1000

With that:
* Board_r2 perf: ~597.47 sec.
* Board_r3 perf: ~570.53 sec.

It's possible to improve `Restore` mechanic by not breaking neighbour links during `BoardParams` 
computation. Most of the time the ray does not come back to the destroyed location.
With that Board_r4 perf: ~520 sec.

Adding together Board_r3 and Board_r4 in Board_r5 perf: ~499.69 sec.

Using profiling we found the following improvements:
* Use 2D array access instead of switch statement to compute reflection path (NextFrom).
* Replace `void ForEachCastCandidate(const std::function<void(const cell_count_t)>& func) const` 
  with `void ForEachCastCandidate(auto func) const` to allow inlining. Otherwise, function object creation is forced.
Implementation of both improvements in Board_r6 with Solver_1 perf: ~ 455.01 sec.

  
[//]: # (#### Eliminate Hash Table, Use Order By Hash Instead.)
[//]: # ()
[//]: # (The final change is removing hash table to dismiss duplicate boards from next level candidates.)
[//]: # (Instead, we pick the best candidates and sort by score and hash, promote to next level skipping duplicate hashes.)
[//]: # (Board_r6 and Solver_2 perf: )