Use --cpu=4 to measure time performance.

The simplest way to store a board is as a square Grid. 
Each element is a one byte mirror.  

Using BS_Greedy_i1 using beam_width=300, score=destroyed_count 
score.py 1-100 seeds perf is: ~85 sec.

Adding col/row elements counts with Board_i3. BS_Score_i1, Score_i1
score.py 1-100 seeds perf is: ~220-240 sec.

If we skip empty lines from traversing using Board_i4:
score.py 1-100 seeds perf is: ~142-156 sec.

`-Ofast` gives a small advantage in like maybe 4-5%. Maybe no advantage at all.
`-march=native` does not improve.