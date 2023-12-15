Use --cpu=4 to measure time performance.

### Native Board: Grid of mirrors that turned Destroyed on Cast

The simplest way to store a board is as a square Grid. 
Each element is a one byte mirror.  

Using BS_Greedy_i1 using beam_width=300, score=destroyed_count 
score.py 1-100 seeds perf is: ~85 sec.

Adding col/row elements counts with Board_i3. BS_Score_i1, Score_i1
score.py 1-100 seeds perf is: ~220-240 sec.

If we skip empty lines from traversing using Board_i4:
score.py 1-100 seeds perf is: ~142-156 sec.

#### ~

`-Ofast` gives a small advantage in like maybe 4-5%. Maybe no advantage at all.
`-march=native` does not improve.

There could be slight improvement traversing boards backwards to do casts.
But it's negligible. 

We also tried to replace `std::multiset` with `boost::priority_queue` in solver BS_Score_i2.
The container stores next iteration boards for BS method.  
We got a negligible improvement. It's clear that the real problem is casting performance.

#### Failure

With Board_i5 we tried to introduce Restore mechanic, where it's possible:
* Do cast
* Copy board if needed
* Restore to the state before the cast.
So it supposed to save time: usually we always copy the board, 
regardless of it will be taken to the next level or not.

using solve/bs_restore_i1.hpp and solve/bs_restore_i2.hpp 
didn't provide any meaningful result. Even though we tried partial solutions:
* do copy always
* restorable
and vice versa too. Also with different emptiness parameter values.

Probably casting through many destroyed cells is the most
time-consuming operation. With that copy is not a big deal. 
After copy memory is still hot, easier to perform operations.

### Neighbour Board: Grid

Underlying structure is still a grid, but each cell contains four links, one per direction,
to the next non-destroyed mirror/border.

Using Board_n_i1 with BS_Greedy_i1 app/solver beam_width=300 we get: ~150 sec.

But using Restorable functionality with Derivatives gives: ~42.12 sec 
even with empty row,col computation.
Result is from running Board_n_i2 on BS_Restore_i3 solver. 
The approach is state of the art.