import sys
import os
sys.path.insert(1, os.path.join(os.path.dirname(__file__), ".."))
import random
import peewee
import argparse
import time
import multiprocessing
from pymirrors.db import DB, BaseModel
from multiprocessing import cpu_count
from pymirrors import generate_board, run, add_parser, parse_int_list


CPU_COUNT = cpu_count()
if CPU_COUNT > 2: CPU_COUNT-= 2


class BeamWidth(BaseModel):
    version=peewee.CharField()
    score_version = peewee.IntegerField()
    beam_width = peewee.IntegerField()
    size = peewee.IntegerField()
    time = peewee.DoubleField()
    seed = peewee.IntegerField()
    score = peewee.DoubleField()
    samples = peewee.IntegerField()


def make_tables(*_):
    DB.create_tables([BeamWidth])
    print('Ok')


class Work:
    def __init__(self,
                 version: str, score_version: int,
                 beam_width: int,
                 board: str, other_args = None):
        self.version = version
        self.score_version = score_version
        self.beam_width = beam_width
        self.board = board
        self.other_args = other_args


def do_work(w: Work):
    args = [f'./bin/{w.version}',
            f'--beam-width={w.beam_width}',
            f'--score={w.score_version}']
    if w.other_args:
        args += w.other_args
    enter_time = time.perf_counter()
    result = run(args, w.board)
    try:
        return int(result.split('\n')[0]) / 2, time.perf_counter() - enter_time
    except:
        print(f'Something is wrong with result: {result}')
        print(f'While running width:{w.beam_width}.')
        raise


class Optimizer:
    def __init__(self, args, board_size, other_args=None):
        self.pool = multiprocessing.Pool(args.cpu)

        random.seed(args.seed)
        boards = [generate_board(board_size) for _ in range(args.samples)]
        self.work_list = [Work(args.version, args.score_version, None, b, other_args)
                          for b in boards]

        self.board_size = board_size
        self.precision = args.precision
        self.time_limit = args.time_limit
        self.args = args

    def work(self, width: int):
        for work in self.work_list:
            work.beam_width = width

        res_list = self.pool.map(do_work, self.work_list)
        avg_score = sum(self.board_size/res[0] for res in res_list)/len(res_list)
        avg_time = sum(res[1] for res in res_list)/len(res_list)

        obj = BeamWidth(
            version=self.args.version,
            score_version=self.args.score_version,
            beam_width=width,
            size=self.board_size,
            time=avg_time,
            seed=self.args.seed,
            score=avg_score,
            samples=args.samples)
        obj.save(force_insert=True)

        print(f'Try width: {width}, time: {avg_time}, score: {avg_score}')
        return avg_time

    def optimize_bounds(self, width_a, time_a, width_b, time_b):
        if ((width_a > width_b and time_b > time_a) or
                (width_b > width_a and time_a > time_b)):
            raise RuntimeError(f'Inconsistent result: wa({width_a}) ta({time_a}) and wb({width_b}) tb({time_b}).')

        if abs(time_a - time_b) <= self.precision or abs(width_a - width_b) <= 1:
            return int((width_b + width_a) / 2)

        width = int((width_a + width_b) / 2)
        time = self.work(width)
        if time > self.time_limit:
            return self.optimize_bounds(width_a, time_a, width, time)
        else:
            return self.optimize_bounds(width, time, width_b, time_b)

    def optimize_directional(self, init_width: int, init_time, step: int):
        width = init_width + step
        time = self.work(width)

        if ((init_width > width and time > init_time) or
            (width > init_width and init_time > time)):
            raise RuntimeError(f'Inconsistent result: wi({init_width}) ti({init_time}) and w({width}) t({time}).')

        if time > self.time_limit:
            if step > 0:
                return self.optimize_bounds(init_width, init_time, width, time)
            else:
                # step negative - init was too big. still exceeding - this is new init
                return self.optimize_directional(width, time, 2 * step)
        else:
            # time left, can increase width
            if step > 0:
                return self.optimize_directional(width, time, 2 * step)
            else:
                return self.optimize_bounds(width, time, init_width, init_time)

    # time limit in seconds
    def optimize(self, init_width: int, init_step: int):
        time = self.work(init_width)
        if time > self.time_limit:
            return self.optimize_directional(init_width, time, -init_step)
        else:
            return self.optimize_directional(init_width, time, init_step)


def run_beam_width(args, other_args):
    random.seed(args.seed)
    boards = [generate_board(args.size) for _ in range(args.samples)]
    for beam_width in getattr(args, 'beam-width'):
        work_list = []
        for b in boards:
            w = Work(args.version,
                     args.score_version,
                     beam_width,
                     b, other_args)
            work_list.append(w)

        with multiprocessing.Pool() as pool:
            cast_counts = pool.map(do_work, work_list)

        avg_score = sum(args.size/cs[0] for cs in cast_counts)/len(cast_counts)
        avg_time = sum(cs[1] for cs in cast_counts)/len(cast_counts)

        obj = BeamWidth(
            version=args.version,
            score_version=args.score_version,
            beam_width=beam_width,
            size=args.size,
            time=avg_time,
            seed=args.seed,
            score=avg_score,
            samples=args.samples)
        obj.save(force_insert=True)
        print(f'Ok BeamWidth: {beam_width} '
              f'Time: {avg_time:0.2f} Score: {avg_score:0.2f}')


def run_optimizer(args, others):
    for sz in args.size:
        print(f'Start Size: {sz}')
        res = Optimizer(args, sz, others).optimize(getattr(args, 'init-width'),
                                                   getattr(args, 'init-step'))
        print(f'Ok Size: {sz}, Width: {res}')


def list_values(args, others):
    time_limit = 10
    qry = BeamWidth.select().where(BeamWidth.samples >= 10,
                                   BeamWidth.version == 'solv_1')
    best_width = {}
    for item in qry:
        if item.size not in best_width:
            best_width[item.size] = (item.beam_width, item.time)
        else:
            _, time = best_width[item.size]
            if abs(time - time_limit) > abs(item.time - time_limit):
                best_width[item.size] = (item.beam_width, item.time)
    best_width = [(size, val[0]) for size, val in best_width.items()]
    best_width.sort(key=lambda x: x[0])
    print('Size+Width')
    print(best_width)
    print('Only Width')
    print(list(map(lambda x: x[1], best_width)))


main_p = argparse.ArgumentParser()
sps = main_p.add_subparsers()

add_parser(sps, 'make-tables', func=make_tables)

p = add_parser(sps, 'run', func=run_beam_width )
p.add_argument('size', type=int)
p.add_argument('beam-width', type=parse_int_list)
p.add_argument('--version', type=str, default='solv_bs_restore_i3')
p.add_argument('--score-version', type=int, default=4)
p.add_argument('--seed', type=int, default=random.randint(1, 100000))
p.add_argument('--samples', type=int, default=10)
p.add_argument("--cpu", type=int, default=CPU_COUNT)

p = add_parser(sps, 'optimize', func=run_optimizer )
p.add_argument('size', type=parse_int_list)
p.add_argument('init-width', type=int)
p.add_argument('init-step', type=int)
p.add_argument('--precision', type=float, default=1, help='Time precision in seconds')
p.add_argument('--version', type=str, default='solv_bs_restore_i3')
p.add_argument('--score-version', type=int, default=4)
p.add_argument('--seed', type=int, default=random.randint(1, 100000))
p.add_argument('--samples', type=int, default=10)
p.add_argument("--cpu", type=int, default=CPU_COUNT)
p.add_argument('--time-limit', type=int, default=10)

add_parser(sps, 'list', func=list_values)

args, others = main_p.parse_known_args()
args.func(args, others)
