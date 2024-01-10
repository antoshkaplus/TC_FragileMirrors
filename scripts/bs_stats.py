import sys
import os
sys.path.insert(1, os.path.join(os.path.dirname(__file__), ".."))
import pandas as pd
import typing as ty
import peewee
import math
import argparse
import multiprocessing
from pymirrors.db import DB, BaseModel
from pymirrors import generate_board, run, add_parser


class BS_Stats(BaseModel):
    beam_width = peewee.IntegerField()
    size = peewee.IntegerField()
    # The table is populated with fixed qty of results
    # per beam_width and board size. Without index user
    # is able to easily overpopulate this table with data.
    index = peewee.IntegerField()
    board_hash = peewee.IntegerField()
    board = peewee.TextField()
    casts = peewee.IntegerField()

    class Meta:
        primary_key = peewee.CompositeKey('beam_width', 'size', 'index')
        indexes = (
            (('beam_width', 'size', 'board_hash'), True),
        )


def make_table(_):
    DB.create_tables([BS_Stats])
    print('Ok')


def remake_table(_):
    DB.drop_tables([BS_Stats])
    DB.create_tables([BS_Stats])
    print('Ok')


class Work:
    def __init__(self, version: str, beam_width: int, board: str, idx: int):
        self.version = version
        self.beam_width = beam_width
        self.board = board
        self.idx = idx


def do_work(w: Work):
    result = run([f'./bin/{w.version}', f'--beam-width={w.beam_width}'], w.board)
    return int(result.split('\n')[0]) / 2


def board_sz(b: str):
    return int(math.sqrt(len(b)))


def set_stats(args):
    sz_list = list(range(50, 101))
    if args.sz:
        sz_list = args.sz

    work_list = []
    for sz in sz_list:
        for i in range(args.samples):
            b = generate_board(sz)
            w = Work(args.version, args.beam_width, b, i+1)
            work_list.append(w)

    pool = multiprocessing.Pool(args.cpu)
    chunk_size = 100
    for i in range(0, len(work_list), chunk_size):
        chunk = work_list[i:i+chunk_size]
        casts = pool.map(do_work, chunk)
        with DB.atomic():
            for work, cast_count in zip(chunk, casts):
                stats = BS_Stats(beam_width=work.beam_width,
                                 size=board_sz(work.board),
                                 index=work.idx,
                                 board_hash=hash(work.board),
                                 board=work.board,
                                 casts=cast_count)
                stats.save(force_insert=True)
        print(f'Done {i+chunk_size}/{len(work_list)}')
    print('Ok')


def list_stats(args):
    stats = list(BS_Stats.select().where(BS_Stats.beam_width==args.beam_width))
    columns = ['Size', 'Index', 'Casts']
    rows = []
    for s in stats:
        rows.append([s.size, s.index, s.casts])
    df = pd.DataFrame(rows, columns=columns)
    gg = df.groupby('Size').agg(
        Samples=('Index', 'count'),
        MeanCasts=('Casts', 'mean'),
        MinCasts=('Casts', 'min'),
        MaxCasts=('Casts', 'max'))
    gg.MeanCasts = gg.MeanCasts.round(2)
    print(gg)


def parse_int_list(s: str) -> ty.List[int]:
    return list(map(int,s.split(',')))


main_p = argparse.ArgumentParser()
sps = main_p.add_subparsers()

add_parser(sps, 'make-table', func=make_table)
add_parser(sps, 'remake-table', func=remake_table)

p = add_parser(sps, 'set', func=set_stats)
p.add_argument('--version', type=str, default='solv_bs_score_i1')
p.add_argument('beam_width', type=int)
p.add_argument('samples', type=int)
p.add_argument('--sz', type=parse_int_list)
p.add_argument('--cpu', type=int)

p = add_parser(sps, 'list', func=list_stats)
p.add_argument('beam_width', type=int)

args = main_p.parse_args()
args.func(args)
