import random
import sys
import os
sys.path.insert(1, os.path.join(os.path.dirname(__file__), ".."))
import peewee
import argparse
from collections import defaultdict
import typing as ty
import multiprocessing
import scipy.optimize as opt
from pymirrors.db import DB, BaseModel
from pymirrors import generate_board, run, add_parser, parse_float_list, parse_int_list


class EvenLinesParam_Optimal(BaseModel):
    version=peewee.CharField()
    score_version = peewee.IntegerField()
    beam_width = peewee.IntegerField()
    size = peewee.IntegerField()
    value = peewee.DoubleField()
    # score error
    precision = peewee.DoubleField()
    samples = peewee.IntegerField()


class EvenLinesParam_AvgScore(BaseModel):
    version=peewee.CharField()
    score_version = peewee.IntegerField()
    beam_width = peewee.IntegerField()
    size = peewee.IntegerField()
    value = peewee.DoubleField()
    # more is better
    score = peewee.DoubleField()
    samples = peewee.IntegerField()


def make_tables(*_):
    DB.create_tables([EvenLinesParam_Optimal, EvenLinesParam_AvgScore])
    print('Ok')


class Work:
    def __init__(self,
                 version: str, score_version: int, score_even_param: float,
                 beam_width: int, board: str, other_args = None):
        self.version = version
        self.score_version = score_version
        self.score_even_param = score_even_param
        self.beam_width = beam_width
        self.board = board
        self.other_args = other_args


def do_work(w: Work):
    args = [f'./bin/{w.version}',
            f'--beam-width={w.beam_width}',
            f'--score={w.score_version}',
            f'--score-empty-param={w.other_args.score_empty_param}',
            f'--score-even-param={w.score_even_param}']
    if w.other_args:
        args += w.other_args
    result = run(args, w.board)
    try:
        return int(result.split('\n')[0]) / 2
    except:
        return result



class Optimizer:
    def __init__(self, args, board_size, other_args=None):
        self.pool = multiprocessing.Pool()
        boards = [generate_board(board_size) for _ in range(args.samples)] # generate
        self.work_list = [Work(args.version, args.score_version, None,
                               args.beam_width, b, other_args)
                          for b in boards]
        self.board_size = board_size
        self.tol = args.precision


    def work(self, value: float):
        for work in self.work_list:
            work.score_even_param = value

        cast_counts = self.pool.map(do_work, self.work_list)
        for cs in cast_counts:
            if not isinstance(cs, (int, float)):
                print(cs)
                return None
        score = sum(self.board_size/cs for cs in cast_counts)
        print(f'Try value: {value}, sum score: {score}')
        return -score

    def optimize(self):
        max_value = 1 / (2*self.board_size)
        return opt.minimize_scalar( self.work,
                                    bracket=(0, max_value/2, max_value),
                                    method='brent',
                                    tol=self.tol,
                                    options={'disp': True})


def run_score_param(args, _):
    if args.seed:
        random.seed(args.seed)
    boards = [generate_board(args.size) for _ in range(args.samples)]
    for v in args.value:
        work_list = []
        for b in boards:
            w = Work(args.version,
                     args.score_version,
                     v,
                     args.beam_width,
                     b)
            work_list.append(w)

        with multiprocessing.Pool() as pool:
            cast_counts = pool.map(do_work, work_list)
        score = sum(args.size/cs for cs in cast_counts)/len(cast_counts)
        obj = EvenLinesParam_AvgScore(
            version=args.version,
            score_version=args.score_version,
            beam_width=args.beam_width,
            size=args.size,
            value=v,
            score=score,
            samples=args.samples
        )
        obj.save(force_insert=True)
        print(f'Ok Value: {v}, AvgScore: {score}')


def run_optimizer(args, others):
    for sz in args.size:
        print(f'Start Size: {sz}')
        res = Optimizer(args, sz, others).optimize()
        print(res)
        # Now can write to the db.
        obj = EvenLinesParam_Optimal(
            version=args.version,
            score_version=args.score_version,
            beam_width=args.beam_width,
            size=sz,
            value=res.x,
            precision=args.precision,
            samples=args.samples
        )
        obj.save(force_insert=True)
        print(f'Ok Size: {sz}, Value: {res.x}')


def list_values(_, __):
    samples = 10000
    params = (EvenLinesParam_Optimal
              .select()
              .where(EvenLinesParam_Optimal.samples >= samples)
              .order_by(EvenLinesParam_Optimal.size))
    res = defaultdict(list)
    for param in params:
        res[param.size].append(param.value)
    for size, params in res.items():
        print(size, sum(params)/len(params))


main_p = argparse.ArgumentParser()
sps = main_p.add_subparsers()

add_parser(sps, 'make-tables', func=make_tables)


# TODO: allow score-empty-param

p = add_parser(sps, 'run', func=run_score_param)
p.add_argument('size', type=int)
p.add_argument('value', type=parse_float_list)
p.add_argument('--version', type=str, default='solv_bs_restore_i8')
p.add_argument('--score-version', type=int, default=8)
p.add_argument('--score-empty-param', type=float, default=0)
p.add_argument('--beam-width', type=int, default=300)
p.add_argument('--samples', type=int, default=100)
p.add_argument('--seed', type=int)

p = add_parser(sps, 'optimize', func=run_optimizer)
# should allow multiple sizes
p.add_argument('size', type=parse_int_list)
p.add_argument('--version', type=str, default='solv_bs_restore_i8')
p.add_argument('--score-version', type=int, default=8)
p.add_argument('--beam-width', type=int, default=300)
p.add_argument('--samples', type=int, default=100)
p.add_argument('--seed', type=int)
# score error
p.add_argument('--precision', type=float, default=0.01)

# p.add_argument('--tol', type=float, default=0.01)
#
# p = add_parser(sps, 'list', func=list_stats)
# p.add_argument('beam_width', type=int)

add_parser(sps, 'list', func=list_values)


args, others = main_p.parse_known_args()
args.func(args, others)
