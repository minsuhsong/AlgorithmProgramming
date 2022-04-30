
#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	
	int i;
	for (i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].flag = false;
		V[i].f_hd = NONE; V[i].r_hd = NONE;
	}
	for (i = 0; i < Enum; i++) {
		E[i].name = i;
		E[i].vf = NONE; E[i].vr = NONE;
		E[i].cost = 1;
		E[i].flag = false;
		E[i].fp = NONE; E[i].rp = NONE;
	}

	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &E[i].vf, &E[i].vr, &E[i].cost);
		if (V[E[i].vf].f_hd != -1)
			E[i].fp = V[E[i].vf].f_hd;

		V[E[i].vf].f_hd = i;

		if (V[E[i].vr].r_hd != -1)
			E[i].rp = V[E[i].vr].r_hd;

		V[E[i].vr].r_hd = i;
	}
}

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.

	V[src].flag = true;
	int i, n;
	int sum = 0, dfs = 0;	
	for (i = V[src].f_hd; i != NONE; i = E[i].fp) {
		if (E[i].vf == src)
			n = E[i].vr;
		else
			n = E[i].vf;
		if (V[n].flag == 0) {
			E[i].flag = true;
			dfs = DFS_Tree_adj_array(n, Vnum, V, Enum, E);
			sum += E[i].cost + dfs;
		}
	}
	for (i = V[src].r_hd; i != NONE; i = E[i].rp) {
		if (E[i].vf == src)
			n = E[i].vr;
		else
			n = E[i].vf;
	if (V[n].flag == 0) {
			E[i].flag = true;
			dfs = DFS_Tree_adj_array(n, Vnum, V, Enum, E);
			sum += E[i].cost + dfs;
		}
	}
	return sum;
}
