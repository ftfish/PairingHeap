//============================================================================
// Name        : PairingHeap.h
// Author      : ftfish (ftfish@gmail.com)
// Version     : 0.99
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
 * A pairing heap is a rooted tree satisfying the heap property (key of a parent is no larger than that of its son).
 * It supports the operations of usual (meldable/mergable) priority queues.
 * This implementation is addressable: The address of an element will be returned at insertion and will not change thereafter.
 *
 * Template parameters:
 * 		typename KeyType:
 * 			The type of the keys in the pairing heap.
 * 			An element is a pair (key, id) where id can be used to identify an element.
 * 		typename IdType:
 * 			The type of the id's in the pairing heap.
 * 			An element is a pair (key, id) where id can be used to identify an element.
 * 		typename Comparator = std::less<KeyType>:
 * 			The functor used to compare two keys. The default results in a min-priority queue
 *
 * Constructor:
 * 		PairingHeap():	creates an empty pairing heap.
 *
 *
 * The following operations are supported:
 * 		size_t size():
 * 			returns the current size.
 *		void insert(const KeyType& key, int id):
 *			insert an element with the given key and id.
 *		const Element &find_min():
 * 			returns the current minimal element in the priority queue.
 * 			Throws an exception if the priority queue is empty.
 *		Element delete_min():
 *			removes and returns the current minimum in the priority queue.
 * 			Throws an exception if the priority queue is empty.
 * 		Element remove(PHNode *node):
 *			removes and returns the element at the given address.
 *		void decrease_key(PHNode *node, const KeyType& newkey):
 *			trys to decrease the key of the element at the given address.
 * 			It does nothing if newkey is no smaller than the current key of the element.
 * 		void merge(PairingHeap<KeyType, IdType, Comparator> &ph)
 *			merges "this" PairingHeap with the argument.
 *			The PH in the argument will be made empty, but the elements in it will not be removed (but merged to "this").
 * 			Pointers to the elements of the argument remain valid.
 *
 * Time:
 * 		Delete_min and remove take O(logn) amortized time.
 * 		Decrease_key is shown to run in O(loglogn) <= T <= O(logn) amortized time.
 * 		All other methods take constant amortized time.
 */
template<typename KeyType = int, typename IdType = int, typename Comparator = std::less<KeyType> >
class PairingHeap {
	/**
	 * The class of elements.
	 * An element is a pair (key, id).
	 * It's a struct with only getters to avoid invalid modifications.
	 */
	class Element {
		KeyType key;
		IdType id;
		Element(const KeyType &key, const IdType &id) :
				key(key), id(id) {
		}
		friend class PairingHeap;
	public:
		const KeyType& get_key() const {
			return key;
		}
		const IdType& get_id() const {
			return id;
		}
	};

	/**
	 * The class of a node in the heap.
	 */
	class PHNode {
		/*
		 * Each node maintains 4 pointers:
		 * 		the parent (0 if the current node is the root)
		 * 		one of the children (0 if none)
		 * 		the left and right siblings. The list of siblings is circular.
		 */
		Element elem;
		PHNode *parent, *left, *right, *son;
		PHNode(const Element &elem) :
				elem(elem), parent(0), left(this), right(this), son(0) {
		}
		friend class PairingHeap;
	public:
		const Element& get_element() const {
			return elem;
		}
		const KeyType& get_key() const {
			return elem.key;
		}
		const IdType& get_id() const {
			return elem.id;
		}
	};

	// the current size.
	int sz;
	// the comparator functor
	Comparator less;
	// the pointer to the root.
	PHNode *root;

	// the possible exceptions. (initialized below out of the class)
	static const PH_Exception PH_EX_EMPTY;

	// ensures the heap is non-empty
	void ensure_nonempty() const {
		if (sz == 0)
			throw PH_EX_EMPTY;
	}

	/**
	 * Merges nodes x and y and returns the resulting root.
	 * x and y must not have parent or siblings.
	 * If either x or y is 0, the other node will be the result.
	 * If nodes x and y have the same key, x will be the result.
	 */
	PHNode *merge(PHNode *x, PHNode *y) {
		if(!x || !y) return x ? x : y;
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
	 * Deletes the root of the subtree rooted at p.
	 */
	PHNode *delete_min(PHNode *p) {
		PHNode *pson = p->son;
		delete p;
		return combine_siblings(pson);
	}

	/**
	 * Cuts node p from the heap.
	 * Does nothing if p is the root.
	 */
	void cut(PHNode *p) {
		//if p is already at the root, do nothing
		if (p->parent) {
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
		}
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
	typedef Element PH_Element;
	typedef PHNode PH_Node;
	PairingHeap() :
			sz(0), root(0) {
	}
	~PairingHeap() {
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
	 * insert an element with the given key and id.
	 * Algo:
	 *		make a new node and merge it with the current root.
	 */
	PHNode *insert(const KeyType& key, const IdType& id) {
		PHNode *p = new PHNode(Element(key, id));
		root = merge(root, p);
		++sz;
		return p;
	}
	/**
	 * returns the current minimal element in the priority queue.
	 * Throws an exception if the priority queue is empty.
	 */
	const PH_Element &find_min() const {
		ensure_nonempty();
		return root->elem;
	}
	/**
	 * removes and returns the current minimum in the priority queue.
	 * Throws an exception if the priority queue is empty.
	 * Algo:
	 * 		call delete_min on the root.
	 * 		(remove the root and combine the children of the root.)
	 */
	PH_Element delete_min() {
		ensure_nonempty();
		Element r = root->elem;
		root = delete_min(root);
		--sz;
		return r;
	}

	/**
	 * removes and returns the element at the given address.
	 * Algo:
	 * 		1. If the node is the root, call delete_min instead.
	 *		2. Otherwise, cut the node, call delete_min on it and then merge the result with the root.
	 */
	PH_Element remove(PHNode *node) {
		if (node->parent == 0)
			return delete_min();
		else {
			Element r = node->elem;
			cut(node);
			PHNode *p = delete_min(node);
			--sz;
			root = merge(root, p);
			return r;
		}
	}

	/**
	 * trys to decrease the key of the element at the given address.
	 * It does nothing if newkey is larger than the current key of the element.
	 * If newkey is as small as the root, the node will be made the new root.
	 * Algo:
	 * 		1. If the node is the root, just change the key.
	 * 		2. Otherwise, cut the node, change the key and then merge it with the root.
	 */
	void decrease_key(PHNode *node, const KeyType& newkey) {
		if (!less(node->elem.key, newkey)) {
			//if the node is the root, we're happy.
			if (node->parent == 0) {
				node->elem.key = newkey;
			} else {
				cut(node);
				node->elem.key = newkey;
				root = merge(node, root);
			}
		}
	}

	/**
	 * merges "this" PairingHeap with the argument.
	 * The PH in the argument will be made empty, but the elements in it will not be removed (but merged to "this").
	 * Pointers to the elements of the argument remain valid.
	 */
	void merge(PairingHeap<KeyType, IdType, Comparator> &ph) {
		if(&ph != this) {
			sz += ph.sz;
			root = merge(root, ph.root);
			ph.sz = 0;
			ph.root = 0;
		}
	}
};

/**
 * The following are possible exceptions.
 */
template<typename KeyType, typename IdType, typename Comparator>
const PH_Exception PairingHeap<KeyType, IdType, Comparator>::PH_EX_EMPTY("The heap is empty!");

#endif /* PAIRINGHEAP_H_ */
