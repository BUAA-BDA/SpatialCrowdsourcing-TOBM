/**
	\author: Trasier
	\date: 2019.01.07
*/
#include <bits/stdc++.h>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include "global.h"
#include "monitor.h"

typedef pair<int,int> pii; 


struct piicomp : public binary_function<const pii, const pii, bool>
{
public:
	result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
	{
		return((_Left.first == _Right.first && _Left.second == _Right.second) ? true : false);
	}
};
typedef map<pii, int> HashMap;

struct edge_t {
	int v, f, nxt;

	edge_t(int v=0, int f=0, int nxt=0):
		v(v), f(f), nxt(nxt) {}
};

vector<int> head, head_;
vector<int> dis;
vector<edge_t> E;
int wnodeN, tnodeN;
HashMap wHash, tHash;
pdd gridSize, gridPer;
int slotSize;
int st, ed;
double wddl = 0.0, tddl = 0.0, wrad = 0.0;
string srcFileName, desFileName;

///////////////////////////////////////////////
//// related to max flow network
void addEdge(int u, int v, int c);
void init_network();
bool bfs();
int dfs(int u, int val);
int Dinic();
void dumpGuide(int maxFlow);
///////////////////////////////////////////////


bool judge(const pii& wp, const pii& tp) {
	pdd wLoc = spatioLoc(wp.first, gridSize, gridPer), tLoc = spatioLoc(tp.first, gridSize, gridPer);
	int wAt = temporalAt(wp.second, slotSize), tAt = temporalAt(tp.second, slotSize);
	
	// deadline constraint
	if (wAt>tAt+tddl || tAt>wAt+wddl) return false;
	
	// range constraint: (1) it is already within the range
	double dist = distance(wLoc, tLoc);
	if (dist <= wrad) return true;
	
	// range constraint: (2) it needs to move towards to the location of task
	double travelTime = (dist-wrad) / speed;
		// task will expire at tAt+tddl and the worker will expire at wAt+wddl
	double slackTime = min(tAt+tddl, wAt+wddl) - wAt;
	
	if (dcmp(travelTime - slackTime) <= 0) 
		return true;
	else
		return false;
}

/**
Line 1:      n m slotSize gridSize.x gridSize.y gridNum_perRow gridNum_perColumn // number of task, number of worker
Line 2~n+m+1:  tid 't'('w') loc.x loc.y ddl (rad) 
*/
void readPrediction(const string& fileName) {
	ifstream fin(fileName.c_str(), ios::in);
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s is invalid.", fileName.c_str());
		exit(1);
	}
	
	fin >> taskN >> workerN >> slotSize >> gridSize.x >> gridSize.y >> gridPer.x >> gridPer.y;

	if (tasks == NULL)
		tasks = new task_t[taskN];
	if (workers == NULL)
		workers = new worker_t[workerN];

	char stmp[16];
	int tid = 0, wid = 0, at;
	for (int i=0; i<taskN+workerN; ++i) {
		fin >> at >> stmp;
		if (stmp[0] == 't') {
			if(tid == taskN)
				cout << "tid == taskN" << endl;
			tasks[tid].at = at;
			fin >> tasks[tid].loc.x >> tasks[tid].loc.y >> tasks[tid].ddl;
			tddl = tasks[tid].ddl;
			++tid;
		} else {
			if(wid == workerN)
				cout << "wid == workerN" << endl;
			workers[wid].at = at;
			fin >> workers[wid].loc.x >> workers[wid].loc.y >> workers[wid].ddl >> workers[wid].rad;
			wddl = workers[wid].ddl, wrad = workers[wid].rad;
			++wid;
		}
	}
	fin.close();
}

void init() {
	int sid, tid;
	
	for (int i=0; i<workerN; ++i) {
		sid = spatioId(workers[i].loc, gridSize, gridPer);
		tid = temporalId(workers[i].at, slotSize);
		pii g = make_pair(sid, tid);
		wHash[g]++;
	}
	
	for (int i=0; i<taskN; ++i) {
		sid = spatioId(tasks[i].loc, gridSize, gridPer);
		tid = temporalId(tasks[i].at, slotSize);
		pii g = make_pair(sid, tid);
		tHash[g]++;		
	}
	
	init_network();		
}

void Delete() {
	delete[] tasks;
	delete[] workers;
}




void addEdge(int u, int v, int c) {		
	E.push_back(edge_t(v, c, head[u]));		
	head[u] = E.size() - 1;		

	E.push_back(edge_t(u, 0, head[v]));		
	head[v] = E.size() - 1;				
}

void init_network() {
	HashMap::iterator wIter, tIter;		
	int k;
	
	wnodeN = wHash.size();	
	tnodeN = tHash.size();
	st = wnodeN + tnodeN;		
	ed = wnodeN + tnodeN + 1;		
	int vertexN = wnodeN + tnodeN + 2;		

	dis.clear();	
	dis.resize(vertexN, 0);
	head.clear();		
	head.resize(vertexN, -1);  
	head_.clear();				
	head_.resize(vertexN, -1);	
	E.clear();		
	
	for (k=0,wIter=wHash.begin(); wIter!=wHash.end(); ++k,++wIter) {		
		addEdge(st, k, wIter->second);		
	}
	for (k=0,tIter=tHash.begin(); tIter!=tHash.end(); ++k,++tIter) {
		addEdge(wnodeN+k, ed, tIter->second);	
	}
	
	int i, j;
	for (i=0,wIter=wHash.begin(); wIter!=wHash.end(); ++i,++wIter) {
		for (j=0,tIter=tHash.begin(); tIter!=tHash.end(); ++j,++tIter) {
			if (judge(wIter->first, tIter->first)) {		
				addEdge(i, wnodeN+j, min(wIter->second, tIter->second));	
			}
		}
	}
}

bool bfs() {
    int u, v, k;
    queue<int> Q;
    
    Q.push(st);
    fill(dis.begin(), dis.end(), 0);
    dis[st] = 1;
    
    while (!Q.empty()) {
        u = Q.front();
        Q.pop();
        if (u == ed) return true;
        for (k=head[u]; k!=-1; k=E[k].nxt) {
        	v = E[k].v;
        	if (dis[v]==0 && E[k].f) {
        		dis[v] = dis[u] + 1;
        		Q.push(v);
        	}
        }
    }
    
    return false;
}

int dfs(int u, int val) {
	if (u==ed || val==0)
		return val;

	int ret = 0, tmp;
	int v;

	for (int& k=head_[u]; k!=-1; k=E[k].nxt) {
		v = E[k].v;
		if (dis[v]==dis[u]+1 && E[k].f && (tmp=dfs(v, min(val, E[k].f)))>0) {
			E[k].f -= tmp;
			E[k^1].f += tmp;
			ret += tmp;
			val -= tmp;
			if (val == 0)
				break;
		}
	}

	return ret;
}

int Dinic() {
	const int vertexN = head.size();
	int k, u, v;
	int maxFlow = 0, tmp;


	while (bfs()) {
		for (int i=0; i<vertexN; ++i)
			head_[i] = head[i];

        while (true) {
        	tmp = dfs(st, 1<<28);
        	if (tmp == 0)
        		break;
        	maxFlow += tmp;
        }
    }

    return maxFlow;
}

void dumpGuide(int maxFlow) {

	ofstream fout(desFileName.c_str(), ios::out);
	
	vector<pii> wps, tps;
	vector<int> wcaps, tcaps;
	vector<vector<int> > M;
	int u, v, k, f;
	
	for (int i=0; i<wnodeN; ++i) {
		M.push_back(vector<int>());		
	}
	for (HashMap::iterator iter=wHash.begin(); iter!=wHash.end(); ++iter) {
		wps.push_back(iter->first);		
		wcaps.push_back(iter->second);	
	}
	for (HashMap::iterator iter=tHash.begin(); iter!=tHash.end(); ++iter) {
		tps.push_back(iter->first);
		tcaps.push_back(iter->second);
	}
	
	int tot = 0;
	
	for (u=0; u<wnodeN; ++u) {
		for (k=head[u]; k!=-1; k=E[k].nxt) {
			if (k & 1) continue;
			v = E[k].v - wnodeN;
			f = min(wcaps[u], tcaps[v]) - E[k].f;
			if (E[k].f>=0 && f>0) {
				for (int i=0; i<f; ++i) {
					M[u].push_back(v);
					++tot;
				}
			}
		}
	}
	// output 
	fout << tot << " " << slotSize << " " << gridSize.x << " " << gridSize.y << " " << gridPer.x << " " << gridPer.y << endl;
	
	for (u=0; u<wnodeN; ++u) {
		pii wp = wps[u];
		for (k=0; k < M[u].size(); ++k) {
			v = M[u][k];
			pii tp = tps[v];
			fout << wp.first << " " << wp.second << " " << tp.first << " " << tp.second << endl;
		}
	}
	
	fout.close();
}

void solve() {
	init();

	int maxFlow = Dinic();
	dumpGuide(maxFlow);
}

int main(int argc, char **argv) {

	if (argc > 1) {
		srcFileName = string(argv[1]);
	}
	else {
		srcFileName = "./predict.txt";
	}
	if (argc > 2) {
		desFileName = string(argv[2]);
	}
	else {
		desFileName = "./guide.txt";
	}

	readPrediction(srcFileName);

	solve();

	Delete();
	
	return 0;
}
