#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;
	*pathN = EN + 1;
	path = (int*)malloc(sizeof(int) * (*pathN));
	int tmp = 0; int w = 0; int cnt = 0; int src = 0;

	
	//degree가 홀수인 vertex의 개수를 count
	for (int i = 0; i < VN; i++) {
		if (V[i].degree % 2 == 1) {
			cnt++;
		}
	}

	//다 짝수면 euler cycle 존재, 두개만 짝수면 euler path, 그 이외의 경우 존재x
	//euler path의 경우 시작 vertex src에 저장
	//euler cycle의 경우 시작 vertex 0으로 정함
	if (cnt == 0)
		*flag = 0;
	else if (cnt == 2) {
		*flag = 1;
		for (int i = 0; i < VN; i++) {
			if (V[i].degree % 2 == 1) {
				src = i;
				break;
			}
		}
	}
	else
		*flag = 2;

	
	//빈 stack S와 크기가 |E|+1인 배열 path에 대해 flag가 0과 1일때 실행
	if (*flag != 2) {
		ST.push(src);
		while (ST.empty() != true) {
			int v = ST.top();
			if (V[v].degree == 0) {
				//v를 path에 추가하고 stack에서 제거
				path[tmp++] = v;
				ST.pop();
			}
			else {
				//v와 인접한 에지 e=(v,w) 선택 후 graph에서 제거
				DBL* p = V[v].S.top();
				w = (E[p->d].v1 != v) ? E[p->d].v1 : E[p->d].v2;

				if (p == NULL) {
					path[tmp++] = ST.top();
				}
				V[w].S.remove(p->twin); V[w].degree--; pool.DBL_cnt--;
				V[v].S.remove(p); V[v].degree--; pool.DBL_cnt--;
				ST.push(w);
			}
		}
		*pathN = tmp;
	}
	else
		*pathN = 0;

	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;
	int i;

	for (i = 0; i < VN; i++) {
		while (V[i].S.empty() == 0) {
			p = V[i].S.pop();
			pool.freeDBL(p);
		}
	}

	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;

	for (int i = 0; i < EN; i++) {
		v1 = E[i].v1;
		v2 = E[i].v2;
		adjE1 = pool.allocDBL();
		adjE1->d = i;
		(V[v1].S).push(adjE1);
		adjE2 = pool.allocDBL();
		adjE2->d = i;
		(V[v2].S).push(adjE2);
		adjE1->twin = adjE2;
		adjE2->twin = adjE1;
	}

}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

DBL *DBList::allocDBL(void) {		// allocate a DBL element
	DBL *p;

	if (DBL_pool == NULL) {
		p = new DBL;
		if (p == NULL) {
			Error_Exit("Memory alloc error(Alloc_DBL)");
		}
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}
	p->rp = p->lp = p->twin = NULL;

	++DBL_cnt;

	return(p);
}

void DBList::freeDBL(DBL *p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;

	--DBL_cnt;		// reduce # of active DBL elements by one 
}

void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL *p) {

	if (ST != NULL) {
		ST->lp = p;
	}
	p->rp = ST;
	p->lp = NULL;
	ST = p;


}

DBL *dblStack::pop(void) {	// remove and return p in front of Q
	DBL *p;

	p = ST;
	if (ST->rp == NULL)
		ST = NULL;
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}
	return p;

}

void dblStack::remove(DBL *p) {	// delete p from ST

	if (ST == p) {
		ST = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = NULL;
		}
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = p->lp;
		}
	}

}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
