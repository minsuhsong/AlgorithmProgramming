#include <math.h>
#include <float.h>

//mergersort를 위한 merge 함수
void merge(int low, int mid, int high, double* X, unsigned* Xid, unsigned* TMP) {
    int i, j, k;
    i = low;
    j = mid + 1;
    k = low;

    while (i <= mid && j <= high) {
        if (X[Xid[i]] <= X[Xid[j]])		
            TMP[k++] = Xid[i++];
        else
            TMP[k++] = Xid[j++];
    }
    if (i > mid) {						
        for (; j <= high; j++, k++)
            TMP[k] = Xid[j];
    }
    else {								
        for (; i <= mid; i++, k++)
            TMP[k] = Xid[i];
    }
    for (int l = low; l <= high; l++)	
        Xid[l] = TMP[l];
}

void mergesort(int low, int high, double* X, unsigned* Xid, unsigned* TMP) { 
    int mid;
    if (low < high) {
        mid = (low + high) / 2;		

        mergesort(low, mid, X, Xid, TMP);
        mergesort(mid + 1, high, X, Xid, TMP);

        merge(low, mid, high, X, Xid, TMP);
    }
}

double combine(unsigned L, unsigned R, double d_LR, unsigned* pt1, unsigned* pt2, double* X, double* Y, unsigned* Xid, unsigned* Yid, unsigned* TMP) {
    unsigned i = L;
    unsigned j;
    unsigned k = 0;
    double mid, l, r, tmp;
    double d_min = d_LR;
       
    mid = X[Xid[(L + R) / 2]];
    l = mid - d_LR;
    r = mid + d_LR;

    while (i <= R) {
        if (X[Yid[i]] >= l && X[Yid[i]] <= r) {
            TMP[k++] = Yid[i];
        }
        i++;
    }

    for (i = 0; i < k; i++) {
        for (j = i + 1; j < k; j++) {
            if (Y[TMP[i]] - Y[TMP[j]] >= d_min)
                break;
                tmp = sqrt((X[TMP[i]] - X[TMP[j]]) * (X[TMP[i]] - X[TMP[j]]) + (Y[TMP[i]] - Y[TMP[j]]) * (Y[TMP[i]] - Y[TMP[j]]));
            if (tmp < d_min) {
                d_min = tmp;
                *pt1 = TMP[i];
                *pt2 = TMP[j];
            }
        }
    }

    return d_min;
}


void   sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
    mergesort(0, N - 1, X, Xid, TMP);
}
// input  : X[]   (x position array of N points)
//          Xid[] (index array of X (initially [0,1,...,N-1])
//          TMP[] (temporary array of size N. may or may not be used)
//          N   number of points to sort
// output : Xid[] (sorted indices by x position in X[])


double CP_brute(//점이 THR보다 적을 때
    unsigned L, unsigned R,	  // current leftmost and rightmost indices
    unsigned* pt1, unsigned* pt2, // closest pair points indices
    double* X, double* Y,         // (x,y) positions array(input)
    unsigned* Xid,  // point index array(sorted by x coordinates)
    unsigned* Yid  // point index array(gradually sorted by y coordinates)
  ) 
{
    unsigned i, j;
    double tmp, distance;
    distance = DBL_MAX;    // 점들 간의 최소 거리
    for (i = L; i < R; i++) {
        j = i + 1;
        tmp = sqrt((X[Xid[i]] - X[Xid[j]]) * (X[Xid[i]] - X[Xid[j]]) + (Y[Xid[i]] - Y[Xid[j]]) * (Y[Xid[i]] - Y[Xid[j]]));
        if (tmp < distance) {
            distance = tmp;
            *pt1 = Xid[i];
            *pt2 = Xid[j];
        }
    }

    for (i = L; i <= R; i++) {
        Yid[i] = Xid[i];
    }

    for (i = L; i <= R; i++) {
        unsigned swap, t;
        t = i;
        for (j = i + 1; j <= R; j++) {
            if (Y[Yid[t]] < Y[Yid[j]])
                t = j;
        }
        swap = Yid[i];
        Yid[i] = Yid[t];
        Yid[t] = swap;
    }   //정렬(선택 정렬)
    return distance;
}


double closestPairDC(
    unsigned L, unsigned R,	  // current leftmost and rightmost indices
    unsigned* pt1, unsigned* pt2, // closest pair points indices
    double* X, double* Y,         // (x,y) positions array(input)
    unsigned* Xid,  // point index array(sorted by x coordinates)
    unsigned* Yid,  // point index array(gradually sorted by y coordinates)
    unsigned* TMP,  // temporal index array
    unsigned THR)	// threshold value
{
    unsigned i, j;
    double tmp, d;

    //점이 THR보다 적을 때 CP_brute 함수 호출
    if (R - L + 1 <= THR) {
        d = CP_brute(L, R, pt1, pt2, X, Y, Xid, Yid);
        return d;
    }

    double d_L, d_R, d_LR, d_min;
    unsigned pt1_L, pt2_L, pt1_R, pt2_R, pt1_min, pt2_min;
    unsigned M = (L + R) / 2;

    d_R = closestPairDC(M + 1, R, &pt1_R, &pt2_R, X, Y, Xid, Yid, TMP, THR);
    d_L = closestPairDC(L, M, &pt1_L, &pt2_L, X, Y, Xid, Yid, TMP, THR);
   
    merge(L, M, R, Y, Yid, TMP);
       
    if (d_L < d_R) {
        d_LR = d_L;
        *pt1 = pt1_L;
        *pt2 = pt2_L;
    }

    else {
        d_LR = d_R;
        *pt1 = pt1_R;
        *pt2 = pt2_R;
    }

    d_min = combine(L, R, d_LR, &pt1_min, &pt2_min, X, Y, Xid, Yid, TMP);
    if (d_LR <= d_min)
        return d_LR;
    else {
        *pt1 = pt1_min;
        *pt2 = pt2_min;
        return d_min;
    }
}


// Input : L, R, X[], Y[], Xid[]
// Output : pt1, pt2 ( the distance between (X[pt1],Y[pt1]) and
//                     (X[pt2],Y[pt2]) is the closest )
//          return the distance value
// Others ; Yid[], TMP[] (scratch arrays for D&C algorithm)