#include <bits/stdc++.h>
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
using namespace std;

#include "monitor.h"

typedef struct pdd {
	int x;
	int y;
} pdd;

typedef struct task_t {
	int at;
	pdd loc;
	double ddl;
	int gridid;
	int slotid;
} task_t; 

typedef struct worker_t {
	int at;
	pdd loc;
	double ddl;
	double rad;
	int gridid;
	int slotid;
} worker_t;



task_t * Tasks;
worker_t * Workers;
int workerN, taskN;
int slotSize;
int gridSizeX, gridSizeY;
int gridPerX, gridPerY;

int begTime, endTime;
int curTime = 0;
double totTime = 0.0;
double avgTime = 0.0;
double speed = 0.1; // equals to 1m/s
int usedMemory = -1;
program_t begProg, endProg;
double * responses;

map <pair< int, int >, vector<pair<int, int>> > predictMap;
map <pair<int, int>, vector<int>> dispatchedMap;
map <pair<int, int>, vector<int>> waitMap;

pdd spatioLoc(int gridid) {
	int xid, yid;
	xid = gridid % gridPerX;
	yid = gridid / gridPerX;

	pdd ret;

	ret.x = xid * gridSizeX + gridSizeX / 2;
	ret.y = yid * gridSizeY + gridSizeY / 2;
	return ret;
}

int temporalAt(int slotid) {
	return slotid * slotSize + slotSize / 2;
}

double distance(pdd wLoc, pdd tLoc) {
	return sqrt((wLoc.x - tLoc.x) * (wLoc.x - tLoc.x) + (wLoc.y - tLoc.y) * (wLoc.y - tLoc.y));
}

const double eps = 1e-6;


int dcmp(double x) {
	if (fabs(x) < eps)
		return 0;
	return x<0 ? -1 : 1;
}

double min(double a, double b) {
	return (a > b) ? b : a;
}

bool judge(int wid, int tid) {
	pdd wLoc = spatioLoc(Workers[wid].gridid), tLoc = spatioLoc(Tasks[tid].gridid);
	int wAt = temporalAt(Workers[wid].slotid), tAt = temporalAt(Tasks[tid].slotid);

	// judge deadline constraints
	if (wAt > tAt + Tasks[tid].ddl || tAt > wAt + Workers[wid].ddl) return false;
	
	//  range constraints: (1) already within the range
	double dist = distance(wLoc, tLoc);
	if (dist <= Workers[wid].rad) return true;
	
	// range constraints: (2) it needs to move towards to the location of task
	double travelTime = (dist - Workers[wid].rad) / speed;
	
	double slackTime = min(tAt + Tasks[tid].ddl, wAt + Workers[wid].ddl) - wAt;
	
	if (dcmp(travelTime - slackTime) <= 0)
		return true;
	else
		return false;
}

void getGridSlotId(pdd loc, int at, int & gridId, int & slotId) {
	gridId = int(loc.y / gridSizeY) * gridPerX + int(loc.x / gridSizeX);
	slotId = at / slotSize;
}

void readInput_predict(const string &predictFileName){
	int tot;

	ifstream fin(predictFileName.c_str(), ios::in);
	
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", predictFileName.c_str());
		exit(1);
	}

	fin >> tot >> slotSize >> gridSizeX >> gridSizeY >> gridPerX >> gridPerY;

	for (int i = 0; i < tot; i++) {
		int wSlotId, wGridId;
		int tSlotId, tGridId;
		fin >> wGridId >> wSlotId >> tGridId >> tSlotId;

		if (predictMap.count(make_pair(wGridId, wSlotId))) {
			predictMap[make_pair(wGridId, wSlotId)].push_back(make_pair(tGridId, tSlotId));
		}
		else {
			predictMap[make_pair(wGridId, wSlotId)].push_back(make_pair(tGridId, tSlotId));

		}
	}

	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif
}

void readInput_data(const string &dataFileName) {

	ifstream fin(dataFileName.c_str(), ios::in);
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", dataFileName.c_str());
		exit(1);
	}

	fin >> taskN >> workerN;

	if (taskN == 0 || workerN == 0) {
		cout << "NO PAIRS CAN BE MATCHED" << endl;
		exit(1);
	}

	Workers = new worker_t[workerN];
	Tasks = new task_t[taskN];
	responses = new double[taskN];

	for(int i = 0; i < taskN; i++){
		responses[i] = -1.0;
	}



	int at;
	int wcnt, tcnt;
	int flag = 0;
	wcnt = 0;
	tcnt = 0;

	char stmp[16];
	
	for (int i = 0; i < workerN + taskN; i++) {
		fin >> at;
		fin >> stmp;

		if (stmp[0] == 't') {

			fin >> Tasks[tcnt].loc.x >> Tasks[tcnt].loc.y >> Tasks[tcnt].ddl;
			Tasks[tcnt].at = at;


			tcnt++;
		}
		else {
			fin >> Workers[wcnt].loc.x >> Workers[wcnt].loc.y >> Workers[wcnt].ddl >> Workers[wcnt].rad;
			Workers[wcnt].at = at;

			wcnt++;


		}
	}

	begTime = (Tasks[0].at < Workers[0].at) ? Tasks[0].at : Workers[0].at;
	int taskEtime, workerEtime;
	taskEtime = Tasks[tcnt - 1].at + ((int)Tasks[tcnt - 1].ddl);
	workerEtime = Workers[wcnt - 1].at + ((int)Workers[wcnt].ddl);
	endTime = (taskEtime < workerEtime) ? workerEtime : taskEtime;

	if (wcnt != workerN || tcnt != taskN) {
		cout << "wcnt workerN tcnt taskN " << wcnt << " " << workerN << " " << tcnt << " " << taskN << endl;
		cout << "NUMBER OF WORKER/TASK NOT MATCH" << endl;
		exit(1);
	}

	for (int i = 0; i < workerN; i++) {			
		getGridSlotId(Workers[i].loc, Workers[i].at, Workers[i].gridid, Workers[i].slotid);
	}
	for (int i = 0; i < taskN; i++) {
		getGridSlotId(Tasks[i].loc, Tasks[i].at, Tasks[i].gridid, Tasks[i].slotid);
	}
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif


}

void init(const string & predictFileName, const string & dataFileName) {

	readInput_predict(predictFileName);

	readInput_data(dataFileName);

}


void calcResult() {
	int cnt = 0;
			
	totTime = calc_time(begProg, endProg);
	for (int i=0; i<taskN; ++i) {
		if (responses[i] >= 0) {
			avgTime += responses[i] - Tasks[i].at;
			++cnt;
		}
		else{
			avgTime += Tasks[i].ddl;
			++cnt;
		}
	}
						
	if (cnt > 0)
		avgTime /= cnt;
}

int workerArrive(int wid) {
	int gridid, slotid;
	gridid = Workers[wid].gridid;
	slotid = Workers[wid].slotid;
	pair<int, int> spatio = make_pair(gridid, slotid);
	if (predictMap.count(spatio)) {

		int disGridId, disSlotId;
		disGridId = predictMap[spatio][0].first;
		disSlotId = predictMap[spatio][0].second;

		predictMap[spatio].push_back(predictMap[spatio][0]);
		predictMap[spatio].erase(predictMap[spatio].begin());
		pair<int, int> randSpatio = make_pair(disGridId, disSlotId);
		if (waitMap.count(randSpatio)) {
			vector<int> taskid;
			vector<int> inRadTask;
			taskid = waitMap[randSpatio];
			for (int i = 0; i < taskid.size(); i++) {
				if (judge(wid, taskid[i]))
					inRadTask.push_back(i);
			}
			if (inRadTask.size() > 0) {
				int chose = rand() % inRadTask.size();
				chose = inRadTask[chose];
				responses[taskid[chose]] = curTime;
				waitMap[randSpatio].erase(waitMap[randSpatio].begin() + chose);
				return 1;
			}
			else {
				dispatchedMap[randSpatio].push_back(wid);
				return 0;
			}
		}
		else {
			dispatchedMap[randSpatio].push_back(wid);
			return 0;
		}
	}
}
int taskArrive(int tid) {
	int gridid, slotid;
	gridid = Tasks[tid].gridid;
	slotid = Tasks[tid].slotid;
	pair<int, int> spatio = make_pair(gridid, slotid);
	if (dispatchedMap.count(spatio)) {
		vector<int> workerid;
		vector<int> touchableWorker;
		workerid = dispatchedMap[spatio];
		for (int i = 0; i < workerid.size(); i++) {
			if (judge(workerid[i], tid)) {
				touchableWorker.push_back(i);
			}
		}

		if (touchableWorker.size() > 0) {
			int chose = rand() % touchableWorker.size();		//randomize
			chose = touchableWorker[chose];
			dispatchedMap[spatio].erase(dispatchedMap[spatio].begin() + chose);
			responses[tid] = curTime;
			return 1;
		}
		else {
			waitMap[spatio].push_back(tid);
			return 0;
		}

	}
	else {
		waitMap[spatio].push_back(tid);
		return 0;
	}
	
}

int solve(const string & predictFileName, const string & dataFileName){
	init(predictFileName, dataFileName);	//init the environment

	int tid = 0;
	int wid = 0;
	int ret = 0;
	save_time(begProg);	

	for (int i = 0; i < workerN + taskN; i++) {
		if (Workers[wid].at <= Tasks[tid].at) {
			curTime = Workers[wid].at;
			ret += workerArrive(wid);
			wid++;
		}
		else {
			curTime = Tasks[tid].at;
			ret += taskArrive(tid);
			tid++;
		}
	}

	save_time(endProg);	
	#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
	#endif

	return ret;
}

int main(int argc, char **argv) {
	ios::sync_with_stdio(false);
	cin.tie(0);
	//program_t begProg, endProg;
	string predictFileName, bipartiteFileName;

	if (argc > 1) {			
		bipartiteFileName = string(argv[1]);
	} else {
		bipartiteFileName = "./data.txt";
	}

	if (argc > 2) {		
		predictFileName = string(argv[2]);
	} else {
		predictFileName = "./guide.txt";
	}
	
	int nPairs = solve(predictFileName, bipartiteFileName);	
	

	calcResult();
	
	double usedTime = calc_time(begProg, endProg); 
	#ifdef WATCH_MEM
	printf("POLAR-OP %d %.4lf %.4lf %d %d\n", nPairs, totTime, avgTime, usedMemory, taskN);
	#else
	printf("POLAR-OP %d %.4lf\n", nPairs, usedTime);
	#endif
	
	return 0;
}
