import random
import math
import subprocess
import typing as ty
import numpy as np


def generate_board(sz) -> str:
    items = random.choices(['R', 'L'], k=sz*sz)
    return ''.join(items)


def print_board(b: str):
    sz = int(math.sqrt(len(b)))
    print(sz)
    for i in range(0, len(b), sz):
        print(b[i:i+sz])

def write_board_bytes(io, board: str):
    sz = int(math.sqrt(len(board)))
    io.write(str(sz).encode('UTF-8'))
    io.write(b'\n')
    for i in range(0, len(board), sz):
        io.write(board[i:i+sz].encode())
        io.write(b'\n')
    io.flush()

def write_board(io, board: str):
    sz = int(math.sqrt(len(board)))
    io.write(str(sz))
    io.write('\n')
    for i in range(0, len(board), sz):
        io.write(board[i:i+sz])
        io.write('\n')
    io.flush()


def run(args: ty.List[str], board: str):
    proc = subprocess.Popen(args, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.DEVNULL)
    write_board_bytes(proc.stdin, board)
    result = proc.stdout.read()
    proc.wait()
    return result.decode()


def add_parser(sps, name, help=None, func=None):
    p = sps.add_parser(name, help=help)
    p.set_defaults(func=func)
    return p


# we could try to use symbol ':' for ranges steps
def parse_int_list(s: str) -> ty.List[int]:
    res = []
    for item in s.split(','):
        if '-' in item:
            first, last = item.split('-')
            first = int(first)
            step = 1
            if ':' in last:
                last, step = last.split(':')
                step = int(step)
            last = int(last)
            res.extend(range(first, last+1, step))
        else:
            res.append(int(item))
    return res


def parse_float_list(s: str) -> ty.List[float]:
    sign = 1.
    if s.startswith('-'):
        sign = -1.
        s = s[1:]
    res = []
    for item in s.split(','):
        if '-' in item:
            first, last = item.split('-')
            first = float(first)
            step = 1.
            if ':' in last:
                last, step = last.split(':')
                step = float(step)
            last = float(last) + 1.e-13
            res.extend(np.arange(first, last, step))
        else:
            res.append(float(item))
    res = [sign*r for r in res]
    return res