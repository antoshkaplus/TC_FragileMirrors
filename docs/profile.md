
### Generate boards with: 
`python scripts/gen_board.py 100`

### Run executable Profile, compiled with `-pg` option
Reduce optimization level to reduce inlining for finer grained results.

### Run with `gprof`:
`gprof program-name [ data-file ] [ > output-file ]`
* `data-file` - `gmon.out` we got from running executable. 
Will show up in directory where you ran the executable.
* `output-file` - The output contains text with long strings.
It's easier to read it from file.