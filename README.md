TC_FragileMirrors
=================

TopCoder marathon match problem.

* Latest version is Solver_2. Solver could be optimized more. Look at psyho approach description and solution for reference.
    In short: open address hash function, sort only once unique, sort derivatives by parent to have board sitting in cache for
    multiple repetitive casts. Of course need to profile.

* Was able to achieve competitive results with `empty_lines` parameter. Use constant 30 or 40. 
    Hard to find any convergence based solely on board size. Maybe trying to find optimal value for many samples,
    accumulate points from all samples into best avg value, linearly approximating for non computed points.

    Another idea is to add more dependencies to the parameter. For the most part it seems this parameter alone just 
    helps to observe solution space in a different direction.

* I was not able to capitalize on `even_lines`. When scoring, > 1 values of the param were not leading to convergence
    for some reason. Could be because empty casts were allowed. After that was afraid to use something higher than 0.5.
    Psyho scoring function showed that flow.

### Scores:

* chokudai_1000 : 1.1177
* psyho_1000 : 1.0942
* solv_3_1000_psyho2 : 1.082 // my algo psyho scoring with even_lines
* solv_2_1000 : 1.0685 // empty_lines 30 or 40
* solv_2_1000_psyho : 1.0677 // my algo psyho scoring with empty_lines
* solv_1_1000 : 1.042
