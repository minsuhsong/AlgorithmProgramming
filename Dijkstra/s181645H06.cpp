#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;


void swap(int* a, int* b) {
	int tmp = 0;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void heapify(int* arr, int n, int i, int Vnum, vertex* V, int Enum, edge* E)
{
	int minVal = i;
	int minValD = V[arr[i]].distance;
	int l = 2 * i + 1; 
	int r = 2 * i + 2;  

	if (l < n && V[arr[l]].distance < minValD) { //minVal보다 leftchild가 더 작을 때
		minVal = l;
		minValD = V[arr[l]].distance;
	}
	
	if (r < n && V[arr[r]].distance < minValD) { //minVal보다 rightchild가 더 작을 때
		minVal = r;
		minValD = V[arr[r]].distance;
	}
	//minVal가 root에 있도록 swap
	if (minVal != i) {
		swap(&arr[i], &arr[minVal]);
		swap(&V[arr[i]].heap_idx, &V[arr[minVal]].heap_idx);
		
		heapify(arr, n, minVal, Vnum, V, Enum, E);
	}
}

void buildHeap(int* arr, int n,	int Vnum, vertex* V, int Enum, edge* E) //주어진 배열로 Minheap을 만드는 함수
{
	//Reverse level order traversal로 한다.
	int startIdx = (n / 2) - 1; //start index
	
	for (int i = startIdx; i >= 0; i--) {
		heapify(arr, n, i, Vnum, V, Enum, E);
	}
}

void deleteHeap(int* arr, int* n, int Vnum, vertex* V,int Enum, edge* E) //Min heap에서의 삭제 함수
{
	int lastElement = arr[*n - 1];
	arr[0] = lastElement; //root를 첫 번째 노드로 바꾼다
	V[arr[0]].heap_idx = 1;
	*n = *n - 1; //size 하나 줄이고
	heapify(arr, *n, 0, Vnum, V, Enum, E); //heapify
}

void adjHeap(int* arr, int n, int i, int Vnum, vertex* V, int Enum, edge* E) //heap을 구성하는 함수
{
	int Val = i - 1;
	int p = i / 2;
	if (Val == 0) {
		return;
	}

	if (V[arr[i - 1]].distance < V[arr[p - 1]].distance) {
		Val = p - 1;
	}
	if (Val != i - 1) {
		swap(&arr[i - 1], &arr[Val]);
		swap(&V[arr[i - 1]].heap_idx, &V[arr[Val]].heap_idx);
		adjHeap(arr, n, Val + 1, Vnum, V, Enum, E);
	}
}



int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)
	
	int heapIdx = 0; int u = 0;
	int i; int tmp = 0, cnt = 0, minCost;

	// Initialize min heap with all vertices. dist value of all vertices  
	for (i = 0; i < Vnum; i++) {
		V[i].inS = false;
		V[i].distance = INT_MAX;
	}
	V[src].inS = true;
	V[src].distance = 0;
	int f = V[src].f_hd;
	int r = V[src].r_hd;

	while (f != -1) {
		V[E[f].vr].distance = V[src].distance + E[f].cost;
		f = E[f].fp;
	}
	while (r != -1) {
		V[E[r].vf].distance = V[src].distance + E[r].cost;
		r = E[r].rp;
	}
	//V-{S} min heap을 만든다
	for (i = 0; i < Vnum; i++) {
		if (V[i].inS == false) {
			V[i].heap_idx = heapIdx + 1;
			minHeap[heapIdx++] = i;
		}
	}
	buildHeap(minHeap, heapIdx, Vnum, V, Enum, E);

	//dijsktra//
	for (i = 0; i < Vnum - 1; i++) {
		u = minHeap[0]; //distance[u]가 가장 작은 노드를 찾는다.
		V[u].inS = true; V[u].heap_idx = -1;
		deleteHeap(minHeap, &heapIdx, Vnum, V, Enum, E); //heap에서 u를 삭제
		f = V[u].f_hd;
		r = V[u].r_hd;

		while (f != -1) {
			if (V[E[f].vr].inS == false &&
				V[u].distance + E[f].cost < V[E[f].vr].distance) {
				V[E[f].vr].distance = V[u].distance + E[f].cost;
				adjHeap(minHeap, heapIdx, V[E[f].vr].heap_idx, Vnum, V, Enum, E);
			}
			f = E[f].fp;
		}
		while (r != -1) {
			if (V[E[r].vf].inS == false &&
				V[u].distance + E[r].cost < V[E[r].vf].distance) {
				V[E[r].vf].distance = V[u].distance + E[r].cost;
				adjHeap(minHeap, heapIdx, V[E[r].vf].heap_idx, Vnum, V, Enum, E);
			}
			r = E[r].rp;
		}
	}

	//backtracing
	int tmp2 = -1;
	int tmp3;
	for (i = 0; i < Vnum; i++) {
		tmp = i;
		while (tmp != src) {
			minCost = INT_MAX;
			f = V[tmp].f_hd; r = V[tmp].r_hd;
			while (f != -1) {
				if (V[tmp].distance - V[E[f].vr].distance == E[f].cost) {
					if (minCost > E[f].cost) {
						minCost = E[f].cost;
						tmp2 = f;
						tmp3 = E[f].vr;
					}

				}
				f = E[f].fp;
			}
			while (r != -1) {
				if (V[tmp].distance - V[E[r].vf].distance == E[r].cost) {
					if (minCost > E[r].cost) {
						minCost = E[r].cost;
						tmp2 = r;
						tmp3 = E[r].vf;
					}
				}
				r = E[r].rp;
			}

			if (E[tmp2].flag == 0) {
				E[tmp2].flag = 1;
				treeCost += E[tmp2].cost;
			}
			tmp = tmp3;
		}
	}
	return treeCost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.

	for (int n = 0; n < Vnum; n++) {
		V[n].inS = false;													// V[].flag false로 초기화
		V[n].f_hd = V[n].r_hd = NONE;										
	}
	for (int n = 0; n < Enum; n++) {
		E[n].flag = false;													// E[].flag false로 초기화
		E[n].fp = E[n].rp = E[n].vf = E[n].vr = NONE;						
	}

	int i, j, c, tmp;
	for (int n = 0; n < Enum; n++) {
		scanf_s("%d %d %d", &i, &j, &c);
		E[n].vf = i; E[n].vr = j; E[n].cost = c;							

		// V[i]의 front header에 인접 edge index가 저장되어 있지 않을 때
		if (V[i].f_hd == NONE) {
			V[i].f_hd = n;													
		}
		else {																
			tmp = V[i].f_hd;												
			V[i].f_hd = n;													
			E[n].fp = tmp;													
		}

		// V[j]의 rear header에 인접 edge index가 저장되어 있지 않을 때
		if (V[j].r_hd == NONE) {
			V[j].r_hd = n;													
		}
		else {																
			tmp = V[j].r_hd;												
			V[j].r_hd = n;													
			E[n].rp = tmp;													 
		}

	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main ( void ) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if ( V == NULL || E == NULL || minHeap == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if ( t != 0 ) 
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n", 
			t+1, Vnum, Enum, cmpt, Tree_cost);

		delete [] minHeap; delete [] V; delete [] E;
	}
	return 0;
}

void Error_Exit (const char *s ) {
  printf("%s\n", s);
  exit(-1);
}
