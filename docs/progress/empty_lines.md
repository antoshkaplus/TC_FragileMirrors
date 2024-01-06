
# 1

By the way casting works, it is a common sense that in two cases with
the same number of mirrors, the case with the fewer exits should on average
destroy more mirrors.

Simply imagine casting to be a black box where
a ray can leave any cell side with equal probability.
The fewer the exits we have - less chance for ray to leave
the maze.

# 2

To prove the point introduce Score_i1: `(destroyed_count, empty_cols + empty_rows)`,
where in care of equal `destroyed_count` we resolve in favour of more empty lines.
And compare with BS_Greedy_i1. Use `beam_width=300`. Run `score.py` seeds 1-100. 
* only destroyed count: 0.8714 score
* with the empty lines: 0.8782 score

# 3 

Let's now find param for each board size to optimize results using score function:
`destroyed_count + param * (empty_cols + empty_rows)`.
Using beam-width=300 and number of samples=100 the clear convergence to a certain param based on size 
was not observed. We saw different end results each single run, double peaks. 
Using beam-width=1 samples=10k provides clear stable convergence over reasonable time. 

Storing computed parameters in EMPTY_LINES_PARAM, running score.py 
with --beam-width=500, --test-count=1000 avg score: 0.97

# 4 

After improving algorithm performance it is possible to run optimizer #3 with:
--beam-width=10 --samples=10k. Storing results in EMPTY_LINES_PARAM_10, 
with --beam-width=500, --test-count=1000 avg score: 0.9858

But then with constant --score-param=20 avg score: 0.99.
And with constant --score-param=30 avg score: 0.9945.
And with constant --score-param=40 avg score: 0.9936.

Running optimizer #3 with --beam-width=100 --samples=1k allows convergence. 
Resulting parameters can be found in EMPTY_LINES_PARAM_100.
Most of the time optimal value ~30. Rerunning initial outliers sz=55,61,67,73,85
proves this. 

Still the tail of the distribution is thick. There could be factors that 
determine empty_lines param dynamically.