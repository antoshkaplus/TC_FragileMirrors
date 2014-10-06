import java.util.Arrays;

public class FragileMirrors {
	private int n, bestSolSize = 1000, stackIdx;
	private int[] castSize = new int[1024];
	private int[][] castStack = new int[1024][4096];
	private int[] board, bestSol, currSol;
	private long seed = 19720909;
	private final long multiplier = 0x5DEECE66DL, addend = 0xBL, mask = (1L << 48) - 1;
	static int TIMEOUT = 9500, cycles;
	private long timeout = System.currentTimeMillis() + TIMEOUT;
	private int[] minRemain = new int[1024], colCount = new int[1024], rowCount = new int[1024];

	public int[] destroy(String[] sboard) {
		Arrays.fill(minRemain, 10001);
		parse(sboard);
		cycles = 0;
		Arrays.fill(rowCount, n);
		Arrays.fill(colCount, n);
		buildSol(n * n, 0);
		while (System.currentTimeMillis() < timeout) {
			int off = (random(bestSolSize) * (bestSolSize - 1 - (int) (timeout - System.currentTimeMillis()) * bestSolSize / TIMEOUT) / bestSolSize) / 2;
			int remain = n * n;
			System.arraycopy(bestSol, 0, currSol, 0, off * 2);
			Arrays.fill(rowCount, n);
			Arrays.fill(colCount, n);
			for (int i = 0; i < off; i++) {
				remain -= doCast(bestSol[i * 2 + 1], bestSol[i * 2]);
				minRemain[i] = Math.min(minRemain[i], remain);
			}
			buildSol(remain, off);
			cycles++;
		}
		return bestSol;
	}

	private void check(int[] cand, int[] val, int a, int b) {
		if (b <= val[val.length - 1]) return;
		for (int i = 0; i < val.length; i++) {
			if (b >= val[i]) {
				for (int j = val.length - 1; j > i; j--) {
					val[j] = val[j - 1];
					cand[j] = cand[j - 1];
				}
				val[i] = b;
				cand[i] = a;
				break;
			}
		}
	}

	private void buildSol(int premain, int off) {
		int currSolSize = off * 2;
		int remain = premain;
		boolean[][] empty = new boolean[4][n];
		int[] cand = new int[5];
		int[] val = new int[cand.length];
		NEXT: while (remain > 0 && currSolSize < bestSolSize) {
			int px = -1;
			int py = -1;
			for (int i = 0; i < n; i++) {
				int idx = i * n;
				for (int j = 0; j < n; j++) {
					if ((board[idx++] & 2) == 0) {
						if (colCount[j] == 1 && rowCount[i] == 1) {
							remain -= doCast(-1, i);
							currSol[currSolSize++] = i;
							currSol[currSolSize++] = -1;
							continue NEXT;
						}
					}
				}
			}
			Arrays.fill(val, -1);
			int k = remain * 3 / n / 2 + 1;
			for (int side = 0; side < 4; side++) {
				boolean[] es = empty[side];
				boolean[] er = empty[side == 1 ? 0 : side == 0 ? 1 : side == 2 ? 3 : 2];
				for (int pos = 0; pos < n; pos++) {
					if (es[pos]) continue;
					int x = side == 0 ? -1 : side == 1 ? n : pos;
					int y = side == 2 ? -1 : side == 3 ? n : pos;
					int c = tryCast(x, y);
					if (c > 0) {
						if (c + k - 1 > val[val.length - 1]) check(cand, val, (pos << 2) | side, c + random(k));
					} else {
						es[pos] = er[pos] = true;
					}
					uncastLast();
				}
			}
			int max = -1;
			for (int i = 0; i < cand.length; i++) {
				int curr = val[i];
				if (curr < 0) continue;
				int ci = cand[i];
				int si = ci & 3;
				int pi = ci >> 2;
				int xi = si == 0 ? -1 : si == 1 ? n : pi;
				int yi = si == 2 ? -1 : si == 3 ? n : pi;
				tryCast(xi, yi);

				int next1 = 0;
				int next2 = 0;
				boolean[] es = empty[si];
				for (int pos = 0; pos < n; pos++) {
					if (es[pos]) continue;
					int c = tryCast(si == 0 ? -1 : si == 1 ? n : pos, si == 2 ? -1 : si == 3 ? n : pos);
					if (c > next1) {
						next2 = next1;
						next1 = c;
					} else if (c > next2) {
						next2 = c;
					}
					uncastLast();
				}
				uncastLast();
				curr = curr * 3 + next1 * 2 + next2;
				if (curr > max) {
					max = curr;
					px = xi;
					py = yi;
				}
			}
			remain -= doCast(px, py);
			if (remain << 2 > minRemain[currSolSize >> 1] * 5) break;
			if (remain < minRemain[currSolSize >> 1]) minRemain[currSolSize >> 1] = remain;
			currSol[currSolSize++] = py;
			currSol[currSolSize++] = px;
		}
		if (remain == 0 && currSolSize <= bestSolSize) {
			bestSolSize = currSolSize;
			bestSol = new int[bestSolSize];
			System.arraycopy(currSol, 0, bestSol, 0, bestSolSize);
		}
		clearBoard();
	}

	private int doCast(int px, int py) {
		int x = px;
		int y = py;
		int dir = x < 0 ? 0 : x >= n ? 1 : y < 0 ? 2 : 3;
		int size = 0;
		int idx = y * n + x;
		while (true) {
			if (dir == 0) {
				if (++x >= n) break;
				idx++;
			} else if (dir == 1) {
				if (--x < 0) break;
				idx--;
			} else if (dir == 2) {
				if (++y >= n) break;
				idx += n;
			} else {
				if (--y < 0) break;
				idx -= n;
			}
			int v = board[idx];
			if ((v & 2) == 0) {
				rowCount[y]--;
				colCount[x]--;
				size++;
				board[idx] = v | 2;
				if (dir == 0) dir = 2 + v;
				else if (dir == 1) dir = 3 - v;
				else if (dir == 2) dir = v;
				else dir = 1 - v;
			}
		}
		return size;
	}

	private int tryCast(int px, int py) {
		int x = px;
		int y = py;
		int dir = x < 0 ? 0 : x >= n ? 1 : y < 0 ? 2 : 3;
		int size = 0;
		int idx = y * n + x;
		int[] stack = castStack[stackIdx];
		while (true) {
			if (dir == 0) {
				if (++x >= n) break;
				idx++;
			} else if (dir == 1) {
				if (--x < 0) break;
				idx--;
			} else if (dir == 2) {
				if (++y >= n) break;
				idx += n;
			} else {
				if (--y < 0) break;
				idx -= n;
			}
			int v = board[idx];
			if ((v & 2) == 0) {
				board[stack[size++] = idx] = v | 2;
				if (dir == 0) dir = 2 + v;
				else if (dir == 1) dir = 3 - v;
				else if (dir == 2) dir = v;
				else dir = 1 - v;
			}
		}
		castSize[stackIdx++] = size;
		return size;
	}

	private void uncastLast() {
		int size = castSize[--stackIdx];
		if (size == 0) return;
		int[] stack = castStack[stackIdx];
		for (int i = 0; i < size; i++) {
			board[stack[i]] &= 1;
		}
	}

	private void clearBoard() {
		for (int i = 0; i < board.length; i++) {
			board[i] &= 1;
		}
	}

	private void parse(String[] sboard) {
		n = sboard.length;
		board = new int[n * n];
		currSol = new int[2 * n * n];
		int pos = 0;
		for (int i = 0; i < n; i++) {
			String si = sboard[i];
			for (int j = 0; j < n; j++) {
				if (si.charAt(j) == 'L') board[pos] = 1;
				pos++;
			}
		}
	}

	private final int random(int n) {
		if ((n & -n) == n) {
			return (int) ((n * (long) ((int) ((seed = (seed * multiplier + addend) & mask) >>> 17))) >> 31);
		}
		int bits, val;
		do {
			bits = (int) ((seed = (seed * multiplier + addend) & mask) >>> 17);
			val = bits % n;
		} while (bits - val + (n - 1) < 0);
		return val;
	}
}