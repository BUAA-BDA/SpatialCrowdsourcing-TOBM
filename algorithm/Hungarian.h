/**
	\author: Trasier
	\date: 2019.01.06
	\reference: GIS'12
*/
#ifndef HUNGARIAN_H
#define HUNGARIAN_H

#include "global.h"


// Hungarin algorithm, Time complexity: O(|T|^2|W|).
struct Hungarian_t {
	int *taskIds, *workerIds, *fa;
	int taskN, workerN;
	bool *visit;

	void Init(int *_taskIds, int _taskN, int *_workerIds, int _workerN, int curTime) {
		workerIds = new int[_workerN];
		taskIds = new int[_taskN];
		fa = new int[_workerN];
		visit = new bool[_workerN];


		workerN = _workerN, taskN= _taskN;
		memcpy(workerIds, _workerIds, sizeof(int)*_workerN);
		memcpy(taskIds, _taskIds, sizeof(int)*_taskN);
		memset(fa, -1, sizeof(int)*_workerN);

		// taskN = 0;
		// for (int i=0; i<_taskN; ++i) {
			// int tid = _taskIds[i];
			// if (!isExpired(tasks[tid], curTime)) {
				// taskIds[taskN++] = tid;
			// }
		// }

		// workerN = 0;
		// for (int i=0; i<_workerN; ++i) {
			// int wid = _workerIds[i];
			// if (!isExpired(workers[wid], curTime)) {
				// workerIds[workerN++] = wid;
			// }
		// }
	}

	void Delete() {
		delete[] taskIds;
		delete[] workerIds;
		delete[] fa;
		delete[] visit;
	}

	bool Find(int j, int curTime) {
		int tid = taskIds[j];
		int wid;
		const task_t& t = tasks[tid];

		for (int i=0; i<workerN; ++i) {
			wid = workerIds[i];
			const worker_t& w = workers[wid];
			if (checkRange(t, w) && checkDeadline(curTime, t, w) && !visit[i]) {
				visit[i] = true;
				if (fa[i]==-1 || Find(fa[i], curTime)) {
					fa[i] = j;
					return true;
				}
			}
		}

		return false;
	}

	int Match(int curTime, int *res) {
		int ret = 0;

		for (int j=0; j<taskN; ++j) {
			memset(visit, false, sizeof(bool)*workerN);
			if (Find(j, curTime))
				++ret;
		}

		memcpy(res, fa, sizeof(int)*workerN);

		return ret;
	}
};

extern double priority(const task_t& t, const worker_t& w);


// Kuhn and Munkres Algorithm, Time complexity: O(|T|^2|W|).
struct wHungarian_t {
	int *taskIds, *workerIds;
	int taskN, workerN;
	bool *S, *T;
	int *xy, *yx;
	double *lx, *ly;
	double *slack;

	void Init(int *_taskIds, int _taskN, int *_workerIds, int _workerN, int curTime) {
		workerIds = new int[_workerN];
		taskIds = new int[_taskN];

		workerN = _workerN, taskN= _taskN;
		memcpy(workerIds, _workerIds, sizeof(int)*_workerN);
		memcpy(taskIds, _taskIds, sizeof(int)*_taskN);

		S = new bool[_taskN];
		T = new bool[_workerN];
		xy = new int[_taskN];
		yx = new int[_workerN];
		lx = new double[_taskN];
		ly = new double[_workerN];
		slack = new double[max(_taskN,_workerN)];

		memset(S, false, sizeof(bool)*_taskN);
		memset(T, false, sizeof(bool)*_workerN);
		memset(xy, -1, sizeof(int)*_taskN);
		memset(yx, -1, sizeof(int)*_workerN);
		memset(lx, 0, sizeof(double)*_taskN);
		memset(ly, 0, sizeof(double)*_workerN);
	}

	void Delete() {
		delete[] taskIds;
		delete[] workerIds;

		delete[] S;
		delete[] T;
		delete[] xy;
		delete[] yx;
		delete[] lx;
		delete[] ly;
		delete[] slack;
	}

	bool DFS(int j, int curTime) {
		int tid = taskIds[j];
		int wid;
		const task_t& t = tasks[tid];
		//cout << "DFS begin" << endl;

		S[j] = true;
		for (int i=0; i<workerN; ++i) {
			wid = workerIds[i];
			const worker_t& w = workers[wid];

			if (T[i]) continue;

			double weight = priority(t, w);
			if (!checkRange(t, w)) weight = -1e8;
			if (!checkDeadline(curTime, t, w)) weight = -1e8;
			double tmp = lx[j] + ly[i] - weight;
		//	cout << "tmp is " << tmp << endl;

			if (dcmp(tmp) == 0) {
				T[i] = true;
				if (yx[i]==-1 || DFS(yx[i], curTime)) {
					yx[i] = j;
					xy[j] = i;
					return true;
				}
			} else {
				slack[i] = min(slack[i], tmp);
			}
		}

		return false;
	}

	void Update() {
		double mn = inf;

		for (int i=0; i<workerN; ++i) {
			if (!T[i]) {
				mn = min(mn, slack[i]);
			}
		}

		for (int j=0; j<taskN; ++j) {
			if (S[j]) {
				lx[j] -= mn;
			}
		}

		for (int i=0; i<workerN; ++i) {
			if (T[i]) {
				ly[i] += mn;
			}
		}
	}

	bool DFS2(int i, int curTime) {
		int tid;
		int wid = workerIds[i];
		const worker_t& w = workers[wid];

		T[i] = true;
		for (int j=0; j<taskN; ++j) {
			tid = taskIds[j];
			const task_t& t = tasks[tid];

			if (S[j]) continue;

			double weight = priority(t, w);
			if (!checkRange(t, w)) weight = -1e8;
			if (!checkDeadline(curTime, t, w)) weight = -1e8;
			double tmp = lx[j] + ly[i] - weight;

			if (dcmp(tmp) == 0) {
				S[j] = true;
				if (xy[j]==-1 || DFS2(xy[j], curTime)) {
					xy[j] = i;
					yx[i] = j;
					return true;
				}
			} else {
				slack[j] = min(slack[j], tmp);
			}
		}

		return false;
	}

	void Update2() {
		double mn = inf;

		for (int j=0; j<taskN; ++j) {
			if (!S[j]) {
				mn = min(mn, slack[j]);
			}
		}

		for (int i=0; i<workerN; ++i) {
			if (T[i]) {
				ly[i] -= mn;
			}
		}

		for (int j=0; j<taskN; ++j) {
			if (S[j]) {
				lx[j] += mn;
			}
		}
	}

	void KM(int curTime) {

		memset(xy, -1, sizeof(int)*taskN);
		memset(yx, -1, sizeof(int)*workerN);
		memset(lx, 0, sizeof(double)*taskN);
		memset(ly, 0, sizeof(double)*workerN);

		if (taskN <= workerN) {
		//	cout << "dfs begin" << endl;
			for (int j=0; j<taskN; ++j) {
				const task_t& t = tasks[taskIds[j]];
				for (int i=0; i<workerN; ++i) {
					const worker_t& w = workers[workerIds[i]];
					lx[j] = max(lx[j], priority(t, w));
				}
			}

			for (int j=0; j<taskN; ++j) {
				for (;;) {
					for (int i=0; i<workerN; ++i) slack[i] = inf;
					memset(S, false, sizeof(bool)*taskN);
					memset(T, false, sizeof(bool)*workerN);
					if (DFS(j, curTime))
						break;
					else
						Update();
				}
			}
		} else {
		//	cout << "dfs2 begin" << endl;
			for (int i=0; i<workerN; ++i) {
				const worker_t& w = workers[workerIds[i]];
				for (int j=0; j<taskN; ++j) {
					const task_t& t = tasks[taskIds[j]];
					ly[i] = max(ly[i], priority(t, w));
				}
			}

			for (int i=0; i<workerN; ++i) {
				for (;;) {
					for (int j=0; j<taskN; ++j) slack[j] = inf;
					memset(S, false, sizeof(bool)*taskN);
					memset(T, false, sizeof(bool)*workerN);
					if (DFS2(i, curTime))
						break;
					else
						Update2();
				}
			}
		}
	}


	int Match(int curTime, int *res) {
		KM(curTime);
		//cout << "KM finished" << endl;

		int ret = 0;

		for (int i=0; i<workerN; ++i) {
			if (yx[i] == -1) {
				res[i] = -1;
				continue;
			}

			int tid = taskIds[yx[i]];
			const task_t& t = tasks[tid];
			int wid = workerIds[i];
			const worker_t& w = workers[wid];

			if (checkRange(t, w) && checkDeadline(curTime, t, w)) {
				res[i] = yx[i];
				++ret;
			} else {
				res[i] = -1;
			}
		}

		return ret;
	}
};


#endif

