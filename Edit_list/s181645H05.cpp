#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int min(int a, int b, int c) {
	if (a > b) {
		if (b >= c) 
			return c;
		else
			return b;
	}
	else {
		if (a >= c)
			return c;
		else
			return a;
	}
}


void Edit_Distance(
	// inputs
	char* SS,		// Source string array
	char* TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
// outputs
int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
char** SR,		// Source string with insertion to T marks '*'
char** OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
char** TR,       // Edited string with deletion form S mark '*'
				// NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int* Mem_Allocated
) {
	int i, j;
	int SSlen, TSlen;
	SSlen = strlen(SS);
	TSlen = strlen(TS);

	for (i = 0; i <= SSlen; i++)
		Table[i][0] = i * del_cost;	
	for (i = 0; i <= TSlen; i++)
		Table[0][i] = i * ins_cost;

	//Table�� dynaminc programming�� �̿��Ͽ� �ϼ���Ų��.
	for (i = 1; i <= SSlen; i++) {
		for (j = 1; j <= TSlen; j++) {
			if (SS[i - 1] == TS[j - 1])
				Table[i][j] = min(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1]);
			else
				Table[i][j] = min(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + sub_cost);
		}
	}

	//ins_cost <= del_cost �̸� �ݵ�� insert ����
	int tmp = SSlen;
	i = SSlen, j = TSlen;
	while (i && j) {
		if (ins_cost <= del_cost) {
			if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				tmp++; j--;
			}
			else if (Table[i][j] == Table[i - 1][j] + del_cost)
				i--;
			else {
				i--; j--;
			}
		}
		else {
			if (Table[i][j] == Table[i - 1][j] + del_cost) {
				i--;
			}
			else if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				tmp++; j--;
			}
			else {
				i--; j--;
			}
		}
	}
	while (j > 0) {
		j--; tmp++;
	}

	//���⼭���� backtracing
	*SR = new char[tmp + 1];
	*TR = new char[tmp + 1];
	*OP = new char[tmp + 1];
	*Mem_Allocated = 3 * (tmp + 1);

	*(*SR + tmp) = '\0';
	*(*TR + tmp) = '\0';
	*(*OP + tmp) = '\0'; //EOS

	int tmp2 = tmp - 1;
	i = SSlen, j = TSlen;
	while (i && j) {
		if (ins_cost <= del_cost) {//parent�� ���ÿ� substitution�� ���ԵǾ� �ִٸ� ins_cost <= del_cost�� ��� insert ����
			if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				*(*OP + tmp2) = 'i';
				*(*SR + tmp2) = '*';
				*(*TR + tmp2) = TS[j - 1];
				tmp2--; j--;
			}
			else if (Table[i][j] == Table[i - 1][j] + del_cost) {
				*(*OP + tmp2) = 'd';
				*(*SR + tmp2) = SS[i - 1];
				*(*TR + tmp2) = '*';
				tmp2--; i--;
			}
			else {
				*(*OP + tmp2) = (SS[i - 1] == TS[j - 1]) ? '.' : 's';
				*(*SR + tmp2) = SS[i - 1];
				*(*TR + tmp2) = TS[j - 1];
				tmp2--; i--; j--;
			}
		}
		else {//parent�� ���ÿ� substitution�� ���ԵǾ� �ִٸ� ins_cost > del_cost�� ��� delete ����
			if (Table[i][j] == Table[i - 1][j] + del_cost) {
				*(*OP + tmp2) = 'd';
				*(*SR + tmp2) = SS[i - 1];
				*(*TR + tmp2) = '*';
				tmp2--; i--;
			}
			else if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				*(*OP + tmp2) = 'i';
				*(*SR + tmp2) = '*';
				*(*TR + tmp2) = TS[j - 1];
				tmp2--; j--;
			}
			else {
				*(*OP + tmp2) = SS[i - 1] == TS[j - 1] ? '.' : 's';
				*(*SR + tmp2) = SS[i - 1];
				*(*TR + tmp2) = TS[j - 1];
				tmp2--; i--; j--;
			}
		}
	}
	//Source string�� ������ ���� delete
	while (i) {
		*(*TR + tmp2) = '*';
		*(*SR + tmp2) = SS[i - 1];
		*(*OP + tmp2) = 'd';
		tmp2--; i--;
	}

	//Edit string�� ������ ���� insert
	while (j) {
		*(*TR + tmp2) = TS[j - 1];
		*(*SR + tmp2) = '*';
		*(*OP + tmp2) = 'i';
		tmp2--; j--;
	}

	return;
}