/**
	\author: Trasier
	\date: 2019.01.06
	\reference: GIS'12
*/
#include <bits/stdc++.h>
using namespace std;
//#pragma comment(linker,"/STACK:102400000,1024000")
#include "global.h"
#include "Hungarian.h"
#include "monitor.h"

const int batchSize = 30; // seconds
double* responses = NULL;
int atn = 0, awn = 0;
int* avaTasks = NULL;
int* avaWorkers = NULL;
int* tCand = NULL, *wCand = NULL;
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
	
	#ifdef DEBUG
	assert(cnt == obj);
	#endif
	
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
	avaTasks[atn++] = tid;
}

void workerArrive(int wid) {
	avaWorkers[awn++] = wid;
}

void update(int& preTime, int curTime) {
	int k;
	
	preTime = curTime / batchSize * batchSize;
	
	{// remove expired tasks
		k = 0;
		for (int i=0; i<atn; ++i) {
			int tid = avaTasks[i];
			if (!isExpired(tasks[tid], preTime+batchSize)) {
				avaTasks[k++] = tid;
			} 
		}
		atn = k;
	}
	
	{// remvoe expired workers
		k = 0;
		for (int i=0; i<awn; ++i) {
			int wid = avaWorkers[i];
			if (!isExpired(workers[wid], preTime+batchSize)) {
				avaWorkers[k++] = wid;
			}
		}
		awn = k;
	}
}

void batch(int preTime) {
	int curTime = preTime + batchSize;
	int *M;
	wHungarian_t hung;
	
	M = new int[awn];
	hung.Init(avaTasks, atn, avaWorkers, awn, curTime);
	int tmp = hung.Match(curTime, M);
	
	for (int i=0; i<awn; ++i) {
		if (M[i] == -1) continue;
		
		int wid = avaWorkers[i];
		int tid = avaTasks[M[i]];
		const worker_t& w = workers[wid];
		const task_t& t = tasks[tid];
		
		++obj;
		responses[tid] = curTime;
		avaWorkers[i] = avaTasks[M[i]] = -1;
	}
	
	{// update avaWorkers & avaTasks
		int k = 0;
		for (int i=0; i<awn; ++i) {
			if (avaWorkers[i] != -1)
				avaWorkers[k++] = avaWorkers[i];
		}
		awn = k;
		
		k = 0;
		for (int i=0; i<atn; ++i) {
			if (avaTasks[i] != -1)
				avaTasks[k++] = avaTasks[i];
		}
		atn = k;
	}
	
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif	
	hung.Delete();
	delete[] M;
}

double priority(const task_t& t, const worker_t& w) {
	return -1.0 * distance(t.loc, w.loc);
}

void solve(){
	int curTime = 0, preTime = 0;
	int tid = 0, wid = 0;
	
	init();
	while (tid<taskN && wid<workerN) {
		curTime = min(tasks[tid].at, workers[wid].at);
		if (curTime >= preTime+batchSize) {
			batch(preTime);
			update(preTime, curTime);
		}
		if (tasks[tid].at <= workers[wid].at) {
			taskArrive(tid);
			++tid;
		} else {
			workerArrive(wid);
			++wid;
		}
	}
	while (tid < taskN) {
		curTime = tasks[tid].at;
		if (curTime >= preTime+batchSize) {
			batch(preTime);
			update(preTime, curTime);
		}
		taskArrive(tid);
		++tid;
	}
	while (wid < workerN) {
		curTime = workers[wid].at;
		if (curTime >= preTime+batchSize) {
			batch(preTime);
			update(preTime, curTime);
		}
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
    dumpResult("Batch-NNP", obj, totTime, avgTime, usedMemory, taskN);
	fflush(stdout);
	Delete();
	
	return 0;
}
