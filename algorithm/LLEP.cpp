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
double *entropy;
pdd gridSize, gridPer;
program_t begProg, endProg;

void readEntropy(string fileName) {
	ifstream fin(fileName.c_str(), ios::in);
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s is invalid.", fileName.c_str());
		exit(1);
	}
	
	fin >> gridSize.x >> gridSize.y >> gridPer.x >> gridPer.y;
	int gridNum = gridPer.x * gridPer.y;
	entropy = new double[gridNum];

	for (int i=0; i< gridPer.x * gridPer.y; ++i)
		fin >> entropy[i];		// entropy for each task, here we need to change that
	fin.close();
}

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
	delete[] entropy;
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
	int curTime = preTime + batchSize;		// calculate current time
	int *M;
	wHungarian_t hung;			// using hungarian algorithm
	
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
	int gridid;
	gridid = spatioId(t.loc, gridSize, gridPer);
	return entropy[gridid];
}

void solve(){
	int curTime = 0, preTime = 0;
	int tid = 0, wid = 0;
	
	init();
	while (tid<taskN && wid<workerN) {
		curTime = min(tasks[tid].at, workers[wid].at);
		if (curTime >= preTime+batchSize) {		// setting the batch and make assignment once per batch gap
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
		taskArrive(tid);	// no use, just add into taskpool
		++tid;
	}
	while (wid < workerN) {
		curTime = workers[wid].at;
		if (curTime >= preTime+batchSize) {
			batch(preTime);
			update(preTime, curTime);
		}
		workerArrive(wid);	// add into worker pool
		++wid;
	}
	
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif
}

int main(int argc, char **argv) {
	string fileName;
	string entropyFileName;
	
	if (argc > 1) {
		fileName = string(argv[1]);
	} else {
		fileName = "./data.txt";
	}
	
	if (argc > 2) {
		entropyFileName = string(argv[2]);
	} else {
		entropyFileName = "./entropy.txt";
	}
	
	readInput(fileName);
	readEntropy(entropyFileName);
	
	save_time(begProg);
	solve();
	save_time(endProg);

	
	calcResult();
    dumpResult("Batch-LLEP", obj, totTime, avgTime, usedMemory, taskN);
	fflush(stdout);
	Delete();
	

	return 0;
}
