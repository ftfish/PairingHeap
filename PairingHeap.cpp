//============================================================================
// Name        : PairingHeap.cpp
// Author      : ftfish (ftfish@gmail.com)
// Version     : 0.1
// Description : Test program for PairingHeap.h
//============================================================================

#include "PairingHeap.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <functional>
using namespace std;

const int mn = 200;

int a[mn], b[mn], c[mn];

bool sorted(int n, int *a) {
	for (int i = 1; i < n; ++i)
		if (a[i] < a[i - 1])
			return 0;
	return 1;
}

int main() {
	srand(time(0));
	PairingHeap<int> pq(mn);
	for (int i = 0; i < mn; ++i) {
		a[i] = (rand() << 15) + rand();
//		a[i] = i;
		pq.insert(i, a[i]);
		cout << a[i] << " ";
	}
	cout << endl;
	
	cout << "size = " << pq.size() << endl;
	for (int i = 0; i < mn; ++i) {
		PairingHeap<int>::Element tmp = pq.delete_min();
		b[i] = tmp.key;
		c[i] = tmp.id;
//		cout << b[i] << " ";
	}
	cout << "size = " << pq.size() << endl;
	for (int i = mn - 1; i >= 0; --i) {
		pq.insert(i, a[i]);
	}
	cout << "size = " << pq.size() << endl;
	for (int i = 0; i < mn; ++i) {
		try{
			PairingHeap<int>::Element tmp = pq.remove(c[i]);
			if(tmp.key != b[i]) {
				cout << "your remove is wrong!!!" << endl;
			}
		} catch (exception &e) {
			cout << "c[i] = " <<c[i] <<endl;
			break;
		} 
	}
	cout << "size = " << pq.size() << endl;
//	cout<<endl;
	cout << sorted(mn, b) << endl;
	return 0;
}
