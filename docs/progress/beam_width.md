
Let's consider beam search with fixed width. 
With
```
python scripts/beam_width.py optimize {size} {init_width} {step} 
    --precision=0.5 --version=solv_1 --score-param=30 --samples={> 10 for better convergence}
```
computed optimal width for each size to find solution in ~10 seconds.
Results are stored in param/beam_width.hpp stored _10_SEC_FIXED_WIDTH_2.

Note: _10_SEC_FIXED_WIDTH - result from using inferior performance Board implementation.

Score for 10_SEC_FIXED_WIDTH_2: 1.042
```
python scripts/score.py solv_1 --score=4  --beam-width=10sec2 --beam-width-version=1 
    --solver-version=1 --test-count=1000 --score-param=30
```


