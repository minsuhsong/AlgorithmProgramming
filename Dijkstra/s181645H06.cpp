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

	if (l < n && V[arr[l]].distance < minValD) { //minVal���� leftchild�� �� ���� ��
		minVal = l;
		minValD = V[arr[l]].distance;
	}
	
	if (r < n && V[arr[r]].distance < minValD) { //minVal���� rightchild�� �� ���� ��
		minVal = r;
		minValD = V[arr[r]].distance;
	}
	//minVal�� root�� �ֵ��� swap
	if (minVal != i) {
		swap(&arr[i], &arr[minVal]);
		swap(&V[arr[i]].heap_idx, &V[arr[minVal]].heap_idx);
		
		heapify(arr, n, minVal, Vnum, V, Enum, E);
	}
}

void buildHeap(int* arr, int n,	int Vnum, vertex* V, int Enum, edge* E) //�־��� �迭�� Minheap�� ����� �Լ�
{
	//Reverse level order traversal�� �Ѵ�.
	int startIdx = (n / 2) - 1; //start index
	
	for (int i = startIdx; i >= 0; i--) {
		heapify(arr, n, i, Vnum, V, Enum, E);
	}
}

void deleteHeap(int* arr, int* n, int Vnum, vertex* V,int Enum, edge* E) //Min heap������ ���� �Լ�
{
	int lastElement = arr[*n - 1];
	arr[0] = lastElement; //root�� ù ��° ���� �ٲ۴�
	V[arr[0]].heap_idx = 1;
	*n = *n - 1; //size �ϳ� ���̰�
	heapify(arr, *n, 0, Vnum, V, Enum, E); //heapify
}

void adjHeap(int* arr, int n, int i, int Vnum, vertex* V, int Enum, edge* E) //heap�� �����ϴ� �Լ�
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
	// *** �� �Լ��� �ۼ����� ***
	// �ݵ�� min-heap�� ����Ͽ� O((n+m)logn) �˰����� �����ؾ� �Ѵ�(�ƴϸ� trivial�� ���α׷���)
	// heap ���� �� �ʿ��� �Լ��� �����Ӱ� �ۼ��Ͽ� �߰��Ѵ�.
	// �׷��� global ����, dynamic array ���� �߰��� ������� �ʴ´�(������ �ʿ� ����)
	
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
	//V-{S} min heap�� �����
	for (i = 0; i < Vnum; i++) {
		if (V[i].inS == false) {
			V[i].heap_idx = heapIdx + 1;
			minHeap[heapIdx++] = i;
		}
	}
	buildHeap(minHeap, heapIdx, Vnum, V, Enum, E);

	//dijsktra//
	for (i = 0; i < Vnum - 1; i++) {
		u = minHeap[0]; //distance[u]�� ���� ���� ��带 ã�´�.
		V[u].inS = true; V[u].heap_idx = -1;
		deleteHeap(minHeap, &heapIdx, Vnum, V, Enum, E); //heap���� u�� ����
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
	// Graph �ڷᱸ���� ����� �Լ�
	// *** �� �Լ��� �߰����� ***
	// PHW02�� Read_Graph_adj_array()�� �� ������ �ڷᱸ���� ���� ��¦ �����Ͽ� ����Ѵ�
	// ��, Read_Graph_adj_array()���� ���ʿ��� ������ ������ �� ���.

	for (int n = 0; n < Vnum; n++) {
		V[n].inS = false;													// V[].flag false�� �ʱ�ȭ
		V[n].f_hd = V[n].r_hd = NONE;										
	}
	for (int n = 0; n < Enum; n++) {
		E[n].flag = false;													// E[].flag false�� �ʱ�ȭ
		E[n].fp = E[n].rp = E[n].vf = E[n].vr = NONE;						
	}

	int i, j, c, tmp;
	for (int n = 0; n < Enum; n++) {
		scanf_s("%d %d %d", &i, &j, &c);
		E[n].vf = i; E[n].vr = j; E[n].cost = c;							

		// V[i]�� front header�� ���� edge index�� ����Ǿ� ���� ���� ��
		if (V[i].f_hd == NONE) {
			V[i].f_hd = n;													
		}
		else {																
			tmp = V[i].f_hd;												
			V[i].f_hd = n;													
			E[n].fp = tmp;													
		}

		// V[j]�� rear header�� ���� edge index�� ����Ǿ� ���� ���� ��
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

		// �Ʒ� Tree_Check�� SPT_test �Լ��� ÷���� SPT_test.obj ���Ͽ� �ִ�.
		// �� �׽�Ʈ���� ������ ���Ͽ� ���α׷��� �����ϸ� ���� �߸��� ���̴�(�����ص� 0��)
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
