//============================================================================
// Name        : PairingHeap.cpp
// Author      : ftfish (ftfish@gmail.com)
// Version     : 0.2
// Description : Test program for PairingHeap.h
//============================================================================

#include "PairingHeap.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <functional>
using namespace std;

const int mn = 1000000;

int a[mn], b[mn], c[mn];
PairingHeap<int>::PH_Node *pos[mn];

bool sorted(int n, int *a) {
	for (int i = 1; i < n; ++i)
		if (a[i] < a[i - 1])
			return 0;
	return 1;
}

int main() {
	srand(time(0));
	PairingHeap<int> pq;
	for (int i = 0; i < mn; ++i) {
		a[i] = (rand() << 15) + rand();
//		cin >> a[i];
		pos[i] = pq.insert(a[i], i);
//		cout << a[i] << " ";
	}
	//cout << endl;

	cout << "size = " << pq.size() << endl;
	for (int i = 0; i < mn; ++i) {
		PairingHeap<int>::PH_Element tmp = pq.delete_min();
		b[i] = tmp.get_key();
		c[i] = tmp.get_id();
		pos[c[i]] = 0;
//		cout << b[i] << " ";
	}
	cout << "size = " << pq.size() << endl;
	cout << "sorted? = " << sorted(mn, b) << endl;
	for (int i = 0; i < mn; ++i)
		if (pos[i])
			cout << "error happened" << endl;
	for (int i = mn - 1; i >= 0; --i) {
		pos[i] = pq.insert(a[i], i);
	}
	cout << "size = " << pq.size() << endl;
	for (int i = 0; i < mn; ++i) {
		try {
			PairingHeap<int>::PH_Element tmp = pq.remove(pos[c[i]]);
			if (tmp.get_key() != b[i]) {
				cout << "your remove is wrong!!!" << endl;
			}
		} catch (exception &e) {
//				FILE *fout = fopen("debugdata.txt", "w");
//				fprintf(fout, "%d\n", mn);
//				for (int j = 0; j < mn; ++j) {
//					fprintf(fout, "%d ", a[j]);
//				}
//				fclose(fout);
//				exit(1);

			cout << e.what() << endl;
			cout << "c[i] = " << c[i] << endl;
			break;
		}
	}
	cout << "size = " << pq.size() << endl;
//	cout<<endl;
	cout << "sorted? = " << sorted(mn, b) << endl;
	return 0;
}
