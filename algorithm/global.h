/**
	\author: Trasier
	\date: 2019.01.06
*/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <bits/stdc++.h>
using namespace std;
//#include <vector>
//#include <map>
//#include <iostream>
//#include <fstream>
//#include <algorithm>

//typedef pair<double,double> pdd;
typedef struct{
	double x;
	double y;
} pdd;
const double eps = 1e-6;
const double inf = 1e25;
const double speed = 0.1;

typedef struct task_t {
	int at;
	pdd loc;
	double ddl;
} task_t; 

typedef struct worker_t {
	int at;
	pdd loc;
	double ddl;
	double rad;
} worker_t;

//extern task_t* tasks;
//extern worker_t* workers;
//extern int obj;
//extern double totTime;
//extern double avgTime;
//extern double memUsage;
//extern int taskN, workerN;

task_t* tasks = NULL;
worker_t* workers = NULL;
int obj = 0;
double totTime = 0.0;
double avgTime = 0.0;
int usedMemory = -1;
int taskN, workerN;



int dcmp(double x);
//void readInput(const string& fileName, int& taskN, int& workerN, task_t*& tasks, worker_t*& workers);
void readInput(const string& fileName);
//void readInput(istream& fin, int& taskN, int& workerN, task_t*& tasks, worker_t*& workers);
void readInput(istream& fin);
double distance(const pdd& pa, const pdd& pb);
double distance2(const pdd& pa, const pdd& pb);
bool checkRange(const task_t& t, const worker_t& w);
bool checkDeadline(const task_t& t, const worker_t& w);
bool checkDeadline(int curTime, const task_t& t, const worker_t& w);
void dumpResult(const char* exec, int obj, double totTime, double avgTime, double memUsage, int taskn);
bool isExpired(const task_t& t, int curTime);
bool isExpired(const worker_t& t, int curTime);


void readInput(const string& fileName) {
	ifstream fin(fileName.c_str(), ios::in);
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s is invalid.", fileName.c_str());
		exit(1);
	}
	
	readInput(fin);
	fin.close();
}

/**
Line 1:      n m  // number of task, number of worker
Line 2~n+m+1:  tid 't'('w') loc.x loc.y ddl (rad) 
*/

void readInput(istream& fin) {
	fin >> taskN >> workerN;
	if (tasks == NULL)
		tasks = new task_t[taskN];
	if (workers == NULL)
		workers = new worker_t[workerN];

	char stmp[16];
	int tid = 0, wid = 0, at;
	for (int i=0; i<taskN+workerN; ++i) {
		fin >> at >> stmp;
		if (stmp[0] == 't') {
			tasks[tid].at = at;
			fin >> tasks[tid].loc.x >> tasks[tid].loc.y >> tasks[tid].ddl;
			++tid;
		} else {
			workers[wid].at = at;
			fin >> workers[wid].loc.x >> workers[wid].loc.y >> workers[wid].ddl >> workers[wid].rad;
			++wid;
		}
	}
	
	#ifdef DEBUG
	assert(tid>=0 && tid<taskN);
	assert(wid>=0 && wid<workerN);
	#endif
}

inline double distance(const pdd& pa, const pdd& pb) {
	return sqrt((pa.x-pb.x)*(pa.x-pb.x) + (pa.y-pb.y)*(pa.y-pb.y));
}

inline double distance2(const pdd& pa, const pdd& pb) {
	return(pa.x-pb.x)*(pa.x-pb.x) + (pa.y-pb.y)*(pa.y-pb.y);
}

inline bool checkRange(const task_t& t, const worker_t& w) {
	return distance2(t.loc, w.loc) <= w.rad * w.rad;
}

inline bool checkDeadline(const task_t& t, const worker_t& w) {
	return t.at <= w.at+w.ddl && w.at <= t.at+t.ddl;
}

inline bool checkDeadline(int curTime, const task_t& t, const worker_t& w) {
	return curTime>=t.at && curTime<=t.at+t.ddl && curTime>=w.at && curTime<=w.at+w.ddl && checkDeadline(t, w);
}

void dumpResult(const char *exec, int obj, double totTime, double avgTime, double memUsage=-1.0, int taskNum = 0) {
	if (memUsage >= 0) {
		printf("%s %d %.4lf %.4lf %.4lf %d\n", exec, obj, totTime, avgTime, memUsage, taskNum);
	} else {
		printf("%s %d %.4lf %.4lf\n", exec, obj, totTime, avgTime);
	}
}

inline bool isExpired(const task_t& t, int curTime) {
	return t.at+t.ddl < curTime;
}

inline bool isExpired(const worker_t& w, int curTime) {
	return w.at+w.ddl < curTime;
}

int dcmp(double x) {
	if (fabs(x) < eps)
		return 0;
	return x<0 ? -1:1;
}


int spatioId(const pdd& pa, const pdd& size, const pdd& per) {	// location, gridSize, gridPer
	int ret = 0;
	int xsize = size.x, ysize = size.y;
	int xper = per.x, yper = per.y;

	ret = int(pa.y / ysize) * xper + int(pa.x / xsize);

	return ret;
}

int temporalId(const int& tid, const int& size) {	//这里的tid的意思是一个worker或task初始的时间
	int ret = tid / size;
	return ret;
}

pdd spatioLoc(const int& id, const pdd& size, const pdd& per) {
	pdd ret;
	int xsize = size.x, ysize = size.y;
	int xper = per.x, yper = per.y;
	int xid = id % xper, yid = id / xper;

	ret.x = 1.0 * xid * xsize + 0.5 * xsize;
	ret.y = 1.0 * yid * ysize + 0.5 * ysize;

	return ret;
}

int temporalAt(const int& id, const int& size) {
	int ret = id * size + 0.5 * size;
	return ret;
}


#endif
