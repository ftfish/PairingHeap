//============================================================================
// Name        : PairingHeap.h
// Author      : ftfish (ftfish@gmail.com)
// Version     : 0.9
// Description : implementation of pairing heaps
//============================================================================

#ifndef PAIRINGHEAP_H_
#define PAIRINGHEAP_H_

#include <cstring>
#include <functional>
#include <exception>
#include <algorithm>

/**
 * The class of possible exceptions when using @PairingHeap.
 */
class PH_Exception: public std::exception {
	const char *str;
	const char *what() const throw () {
		return str;
	}
public:
	PH_Exception(const char *s) :
			str(s) {
	}
};

/**
 * The class of pairing heaps.
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
 * 			Throws an exception if the id is invalid or there's no element with that id.
 * 			It does nothing if newkey is larger than the current key of the element.
 * 			If the new key is as small as the root, that node will be the made the new root, even if its current key equals to the new one.
 * Time:
 * 		Delete_min takes O(logn) amortized time.
 * 		Decrease_key is shown to run in O(loglogn) <= T <= O(logn) amortized time.
 * 		All other methods take constant amortized time.
 */
template<typename T, typename Comparator = std::less<T> >
class PairingHeap {

	/**
	 * The representation of an element: a pair (id, key).
	 * Every element has a unique id.
	 */
	struct Element {
		int id;
		T key;
		Element(int id, T key) :
				id(id), key(key) {
		}
	};
	/**
	 * The internal structure of a node in the heap.
	 * Each node maintains 4 pointers:
	 * 		the parent (0 if the current node is the root)
	 * 		one of the children (0 if none)
	 * 		the left and right siblings. The list of siblings is circular.
	 *
	 */
	struct PHNode {
		Element elem;
		PHNode *parent, *left, *right, *son;
		PHNode(Element elem) :
				elem(elem), parent(0), left(this), right(this), son(0) {
		}
	};

	// the current and maximal size.
	int sz, maxsz;
	// the comparator functor
	Comparator less;
	// pos[i] is the address of the node with element with id = i.
	PHNode **pos;
	// the pointer to the root.
	PHNode *root;

	// the possible exceptions. (initialized below out of the class)
	static const PH_Exception PH_EX_EMPTY, PH_EX_BAD_ID, PH_EX_ALREADY_EXISTS,
			PH_EX_NO_SUCH_ELEMENT;

	// ensures id is valid and there's NO element with this id.
	void ensure_not_existing(int id) const {
		if (id < 0 || id >= maxsz)
			throw PH_EX_BAD_ID;
		if (pos[id])
			throw PH_EX_ALREADY_EXISTS;
	}
	// ensures id is valid and there's ONE element with this id.
	void ensure_existing(int id) const {
		if (id < 0 || id >= maxsz)
			throw PH_EX_BAD_ID;
		if (pos[id] == 0)
			throw PH_EX_NO_SUCH_ELEMENT;
	}
	// ensures the heap is non-empty
	void ensure_nonempty() const {
		if (sz == 0)
			throw PH_EX_EMPTY;
	}

	/**
	 * Merges nodes x and y.
	 * They must not have parent or siblings.
	 * Returns the resulting root.
	 * If nodes x and y have the same key, x will be the result.
	 */
	PHNode *merge(PHNode *x, PHNode *y) {
		if (less(y->elem.key, x->elem.key))
			std::swap(x, y);
		y->parent = x;
		PHNode *xson = x->son;
		if (xson == 0) {
			x->son = y;
		} else {
			y->left = xson->left;
			xson->left->right = y;
			y->right = xson;
			xson->left = y;
			x->son = y;
		}
		return x;
	}

	/**
	 * appends element b to the list ending at a. (using pointers left and right as a linked list)
	 * Returns the new end of the list. If a is 0, return b itself.
	 */
	PHNode *append(PHNode *a, PHNode *b) {
		if (a == 0)
			return b;
		a->right = b;
		b->left = a;
		return b;
	}
	/**
	 * combines the siblings of x to get a single root.
	 * Returns the resulting root.
	 * Algo:
	 *		1. First merge the nodes in pairs (thus "pairing heaps") from left to right
	 *		2. Then merge the resulting list of heaps from right to left.
	 */
	PHNode *combine_siblings(PHNode *x) {
		if (x == 0)
			return 0;
		PHNode *p = x, *end = 0;
		do { // the forward merging
			PHNode *n1 = p, *n2 = p->right;
			n1->parent = 0;
			n1->left = n1->right = n1;
			if (n2 == x) {
				n1->left = 0;
				end = append(end, n1);
				break;
			}
			p = n2->right;
			n2->parent = 0;
			n2->left = n2->right = n2;
			n1 = merge(n1, n2);
			n1->left = 0;
			end = append(end, n1);
		} while (p != x);

		PHNode *res = end;
		p = end->left;
		res->left = res->right = res;
		while (p) { //backwards
			PHNode *p2 = p->left;
			p->left = p->right = p;
			res = merge(res, p);
			p = p2;
		}
		return res;
	}
	/**
	 * disposes the entire heap recursively.
	 * TODO: eliminate recursion.
	 */
	void recursively_destruct(PHNode *x) {
		PHNode *xson = x->son;
		if (xson == 0) // the heap has only a root.
			delete x;
		else {
			PHNode *p = xson;
			do {
				PHNode *p2 = p->right;
				recursively_destruct(p);
				p = p2;
			} while (p != xson);
			delete x;
		}
	}
public:
	typedef Element Element;
	PairingHeap(int max_size) :
			sz(0), maxsz(max_size), root(0) {
		pos = new PHNode*[max_size];
		memset(pos, 0, max_size * sizeof(PHNode*));
	}
	~PairingHeap() {
		delete[] pos;
		if (root)
			recursively_destruct(root);
	}
	/**
	 * returns the current size.
	 */
	size_t size() const {
		return sz;
	}
	/**
	 * returns the maximal size.
	 */
	size_t max_size() const {
		return maxsz;
	}
	/**
	 * determines whether there's an element in the pq with the given id.
	 * This won't throw any exceptions.
	 */
	bool contains(int id) {
		return id >= 0 && id < maxsz && pos[id] != 0;
	}
	/**
	 * returns the current key of the element with the given id.
	 * Throws an exception if the id is invalid or there's no element with that id.
	 */
	const T &get_key(int id) const {
		ensure_existing(id);
		return pos[id]->elem.key;
	}
	/**
	 * try to insert an element with the given id and key.
	 * Throws an exception if the id is invalid or there's already an element with that id.

	 * Algo:
	 *		make a new node and merge it with the current root.
	 */
	void insert(int id, const T& key) {
		ensure_not_existing(id);
		PHNode *p = new PHNode(Element(id, key));
		pos[id] = p;
		root = root ? merge(root, p) : p;
		++sz;
	}
	/**
	 * returns the current minimal element in the priority queue.
	 * Throws an exception if the pq is empty.
	 */
	const Element &find_min() const {
		ensure_nonempty();
		return root->elem;
	}
	/**
	 * removes and returns the current minimum in the pq.
	 * Throws an exception if the pq is empty.
	 * Algo:
	 * 		remove the root and combine the children of the root.
	 */
	Element delete_min() {
		ensure_nonempty();
		Element r = root->elem;
		pos[r.id] = 0;
		PHNode *rson = root->son;
		delete root;
		--sz;
		root = combine_siblings(rson);
		return r;
	}

	/**
	 * removes and returns the element with the given id.
	 * Throws an exception if the id is invalid or there's no element with that id.
	 * Algo:
	 * 		1. Decrease the key to the current minimum;
	 * 			Note that decrease_key will make sure the node to remove will be the new root.
	 *		2. Delete_min
	 */
	Element remove(int id) {
		ensure_existing(id);
		decrease_key(id, root->elem.key);
		return delete_min();
	}

	/**
	 * try to decrease the key of the element with the given id.
	 * Throws an exception if the id is invalid or there's no element with that id.
	 * It does nothing if newkey is larger than the current key of the element.
	 * If the new key is as small as the root, that node will be the made the new root, even if its current key equals to the new one.
	 * Algo:
	 * 		cut the node with that id from the heap and then merge the subtree with the root.
	 */
	void decrease_key(int id, const T& newkey) {
		if (!less(get_key(id), newkey)) {
			PHNode *p = pos[id];
			// if the node is the root, we're happy.
			if (p == root)
				p->elem.key = newkey;
			else {
				// if the node is the only child of its parent
				if (p->left == p) {
					PHNode *pp = p->parent;
					pp->son = 0;
					p->parent = 0;
				} else { // else, it has siblings
					PHNode *pp = p->parent;
					// if its parent points to this node, point to another sibling.
					if (pp->son == p)
						pp->son = p->left;
					p->parent = 0;
					p->left->right = p->right;
					p->right->left = p->left;
					p->left = p->right = p;
				}
				// do the change, and merge.
				p->elem.key = newkey;
				// this ordering of the arguments makes sure that if they have the same key, the new node will be the root.
				root = merge(p, root);
			}
		}
	}
};

/**
 * The following are possible exceptions.
 */
template<typename T, typename Comparator>
const PH_Exception PairingHeap<T, Comparator>::PH_EX_ALREADY_EXISTS(
		"An element with the same ID already exists.");
template<typename T, typename Comparator>
const PH_Exception PairingHeap<T, Comparator>::PH_EX_EMPTY(
		"The heap is empty!");
template<typename T, typename Comparator>
const PH_Exception PairingHeap<T, Comparator>::PH_EX_BAD_ID("ID out of range!");
template<typename T, typename Comparator>
const PH_Exception PairingHeap<T, Comparator>::PH_EX_NO_SUCH_ELEMENT(
		"The heap contains no element with this ID!");

#endif /* PAIRINGHEAP_H_ */
