PairingHeap
===========

An implementation of Pairing Heaps.

//============================================================================
// Name        : PairingHeap.h
// Author      : ftfish (ftfish@gmail.com)
// Version     : 0.9
// Description : implementation of pairing heaps
//============================================================================
/**
 * A pairing heap is a rooted tree satisfying the heap property.
 * This implementation is addressable and supports the usual operations of priority queues:
 *
 * Template parameters:
 * 		typename T:
 * 			The type of the keys in the pairing heap.
 * 			An element is a pair (id, key) where id can be used to identify an element.
 * 		typename Comparator = std::less<T>:
 * 			The functor used to compare two keys. The default results in a min-priority queue
 *
 * Constructor:
 * 		PairingHeap(int max_size)
 * 		A maximal size must be specified for allocating space for the addressability.
 * 		After creation, the valid id's are: 0, 1, ..., max_size-1
 *
 * Public methods:
 * 		size_t size():
 * 			returns the current size.
 *			returns the maximal size.
 *		size_t max_size():
 *			determines whether there's an element in the pq with the given id.
 *			This won't throw any exceptions.
 *		bool contains(int id):
 *		const Element &find_min():
 * 			returns the current minimal element in the priority queue.
 * 			Throws an exception if the pq is empty.
 *		const T& get_key(int id):
 *			returns the current key of the element with the given id.
 *			Throws an exception if the id is invalid or there's no element with that id.
 *		void insert(int id, const T& key):
 *			try to insert an element with the given id and key.
 *			Throws an exception if the id is invalid or there's already an element with that id.
 *		Element delete_min():
 *			removes and returns the current minimum in the pq.
 * 			Throws an exception if the pq is empty.
 * 		Element remove(int id):
 *			removes and returns the element with the given id.
 *	  		Throws an exception if the id is invalid or there's no element with that id.
 *		void decrease_key(int id, const T& newkey):
 *			try to decrease the key of the element with the given id.
 *			Throws an exception if the id is invalid or there's no element with that id.
 *			It does nothing if newkey is not smaller than the current key of the element.
 * Time:
 * 		Delete_min takes O(logn) amortized time.
 * 		Decrease_key is shown to run in O(loglogn) <= T <= O(logn) amortized time.
 * 		All other methods take constant amortized time.
 */