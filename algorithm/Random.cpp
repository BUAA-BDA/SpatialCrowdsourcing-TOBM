/**
	\author: Trasier
	\date: 2019.01.05
	\reference: STOC'90
*/
#include <bits/stdc++.h>
using namespace std;
//#pragma comment(linker,"/STACK:102400000,1024000")
#include "global.h"
#include "monitor.h"

double* responses = NULL;
int atn = 0, awn = 0;
int* avaTasks = NULL;
int* avaWorkers = NULL;
int* tCand = NULL, * wCand = NULL;
program_t begProg, endProg;

void init() {
	avaTasks = new int[taskN];
	avaWorkers = new int[workerN];
	tCand = new int[taskN];
	wCand = new int[workerN];
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
	delete[] wCand;
	delete[] tCand;
}

void taskArrive(int tid) {
	const task_t& t = tasks[tid];
	int k = 0;
	int wid;
	
	for (int i=0; i<awn; ++i) {
		int tmpwid = avaWorkers[i];
		if (checkRange(t, workers[tmpwid]) && checkDeadline(t, workers[tmpwid])) {
			wCand[k++] = i;
		}
	}
	
	if (k == 0) {
		avaTasks[atn++] = tid;
	} else {
		int r = rand() % k, wid = avaWorkers[wCand[r]];
		swap(avaWorkers[awn-1], avaWorkers[wCand[r]]);
		--awn;
		
		++obj;
		responses[tid] = t.at;
	}
}

void workerArrive(int wid) {
	const worker_t& w = workers[wid];
	int k = 0;
	int tid;
	
	for (int i=0; i<atn; ++i) {
		int tmptid = avaTasks[i];
		if (checkRange(tasks[tmptid], w) && checkDeadline(tasks[tmptid], w)) {
			tCand[k++] = i;
		}
	}
	
	if (k == 0) {
		avaWorkers[awn++] = wid;
	} else {
		int r = rand() % k, tid = avaTasks[tCand[r]];
		swap(avaTasks[atn-1], avaTasks[tCand[r]]);
		--atn;
		
		++obj;
		responses[tid] = w.at;
	}
}

void solve(){
	int tid = 0, wid = 0;
	
	init();
	while (tid<taskN && wid<workerN) {
		if (tasks[tid].at <= workers[wid].at) {
			taskArrive(tid);
			++tid;
		} else {
			workerArrive(wid);
			++wid;
		}
	}
	while (tid < taskN) {
		taskArrive(tid);
		++tid;
	}
	while (wid < workerN) {
		workerArrive(wid);
		++wid;
	}
	
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
    dumpResult("Random", obj, totTime, avgTime, usedMemory, taskN);
	fflush(stdout);
	Delete();
	
	return 0;
}
