/**
	\author: Trasier
	\date: 2019.01.04
	\reference: STOC'18
*/
#include <bits/stdc++.h>
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
using namespace std;
//#pragma comment(linker,"/STACK:102400000,1024000")
#include "global.h"
#include "monitor.h"

typedef struct pdi {
public:	
	double first;
	int second;
	pdi(double first, double second) {
		this->first = first;
		this->second = second;
	}

}pdi;

struct cmp {
	bool operator() (const pdi& a, const pdi& b) {
		return a.first > b.first;
	}
};

const int BASE = 10000;
double* responses = NULL;
int atn = 0, awn = 0;
int* avaTasks = NULL;
int* avaWorkers = NULL;
double *yTasks = NULL;
double *yWorkers = NULL;
program_t begProg, endProg;
priority_queue<pdi, vector<pdi>, cmp> Q;

void init() {
	avaTasks = new int[taskN];
	avaWorkers = new int[workerN];
	yTasks = new double[taskN];
	yWorkers = new double[workerN];
	responses = new double[taskN];
	for (int i=0; i<taskN; ++i)
		responses[i] = -1.0;
	
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif
}

void calcResult() {
	int cnt = 0;
	
	totTime = calc_time(begProg, endProg);
	for (int i=0; i<taskN; ++i) {
		if (responses[i] >= 0) {
			avgTime += responses[i] - tasks[i].at;
			++cnt;
		}
		else{
			avgTime += tasks[i].ddl;
			++cnt;
		}
	}
	
	if (cnt > 0)
		avgTime /= cnt;
}

void Delete(){
	delete[] responses;
	delete[] tasks;
	delete[] workers;
	delete[] avaTasks;
	delete[] avaWorkers;
}

void taskArrive(int tid, int wid) {
	yTasks[tid] = rand() % BASE / (double)BASE;
	avaTasks[atn++] = tid;
	Q.push(pdi((double)tasks[tid].at+tasks[tid].ddl, tid));
}

void workerArrive(int tid, int wid) {
	yWorkers[wid] = rand() % BASE / (double)BASE;
	avaWorkers[awn++] = wid;
	Q.push(pdi((double)workers[wid].at+workers[wid].ddl, taskN+wid));
}

void deadlineArrive(int curTime) {
	
	while (Q.size() != 0 && Q.top().first <= curTime) {
		pdi node = Q.top();
		Q.pop();

		int r = -1;
		int tid = -1, wid = -1;
		
		if (node.second < taskN) {		// if task's deadline coming
			tid = node.second;

			int flag = -1;
			for (int i = 0; i < atn; i++) {
				if (avaTasks[i] == tid) {
					flag = i;
					break;
				}
			}
			if (flag == -1) continue;


			const task_t& t = tasks[tid];		//get the task
			
			for (int i=0; i<awn; ++i) {
				int wid_ = avaWorkers[i];		//scan all available worker
				if (checkRange(t, workers[wid_]) && checkDeadline(curTime, t, workers[wid_])) {
					if (wid<0 || yWorkers[wid_]<yWorkers[wid]) {
						wid = wid_;
						r = i;
					}
				}
			}
			
			if (wid >= 0) {
				swap(avaWorkers[awn-1], avaWorkers[r]);
				--awn;
				swap(avaTasks[atn - 1], avaTasks[flag]);
				--atn;
				
				++obj;
				responses[tid] = curTime;
			}
		} else {
			wid = node.second - taskN;
			const worker_t& w = workers[wid];
			
			int flag = -1;
			for (int i = 0; i < awn; i++) {
				if (avaWorkers[i] == wid) {
					flag = i;
					break;
				}
			}
			if (flag == -1) continue;


			for (int i=0; i<atn; ++i) {
				int tid_ = avaTasks[i];
				if (checkRange(tasks[tid_], w) && checkDeadline(curTime, tasks[tid_], w)) {
					if (tid<0 || yTasks[tid_]<yTasks[tid]) {
						tid = tid_;
						r = i;
					}
				}
			}
			
			if (tid >= 0) {
				swap(avaTasks[atn-1], avaTasks[r]);
				--atn;
				swap(avaWorkers[awn - 1], avaWorkers[flag]);
				--awn;
				
				++obj;
				responses[tid] = curTime;
			}
		}
	}
}

void solve(){
	int tid = 0, wid = 0;
	
	init();
	while (tid<taskN && wid<workerN) {
		if (tasks[tid].at <= workers[wid].at) {
			deadlineArrive(tasks[tid].at);
			taskArrive(tid, wid);
			++tid;
		} else {
			deadlineArrive(workers[wid].at);
			workerArrive(tid, wid);
			++wid;
		}
	}
	while (tid < taskN) {
		deadlineArrive(tasks[tid].at);
		taskArrive(tid, wid);
		++tid;
	}
	while (wid < workerN) {
		deadlineArrive(workers[wid].at);
		workerArrive(tid, wid);
		++wid;
	}
	deadlineArrive((int)1e9);
	
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif
}

int main(int argc, char **argv) {
	string fileName;
	
	if (argc > 1) {
		fileName = string(argv[1]);
	} else {
		fileName = "./data.txt";
	}
	
	readInput(fileName);
	
	save_time(begProg);
	solve();
	save_time(endProg);

	
	calcResult();
    dumpResult("ext-Ranking", obj, totTime, avgTime, usedMemory, taskN);
	fflush(stdout);
	Delete();
	
	return 0;
}
