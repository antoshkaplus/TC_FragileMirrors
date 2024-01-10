import sys
import os
sys.path.insert(1, os.path.join(os.path.dirname(__file__), ".."))
import argparse
from pymirrors import generate_board, write_board


parser = argparse.ArgumentParser()
parser.add_argument('-n', '--name', type=str, default='board')
parser.add_argument('sz', type=int)

args = parser.parse_args()

b = generate_board(args.sz)
with open(f'./data/board/{args.name}_{args.sz}.txt', 'w') as file:
    write_board(file, b)