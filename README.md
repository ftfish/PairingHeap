PairingHeap
============================================

An implementation of Pairing Heaps.
--------------------------------------------

A pairing heap is a rooted tree satisfying the heap property (key of a parent is no larger than that of its son).
It supports the operations of usual (meldable/mergable) priority queues.
This implementation is addressable: The address of an element will be returned at insertion and will not change thereafter.

The following operations are supported: 

- `insert`: insert an element (a pair of key and id) into the heap.
- `find_min`: return the minimal element in the heap.
- `delete_min`: delete and return the minimal element in the heap.
- `remove`: remove an element given its address.
- `decrease_key`: decrease the key of an element given the address of the node.
- `merge`: merge two pairing heaps.
	
Time complexity:

- `Delete_min` and `remove` take O(logn) amortized time.
- `Decrease_key` is shown to run in O(loglogn) <= T <= O(logn) amortized time.
- All other methods take constant amortized time.
	

Latest version: 	0.99
-----------------------

Known issues:
-------------
- None

Change log:
-----------

- 0.99  2012.05
	- Improved the structure and usability (not id-based any more).
	- No maximum size is needed at creation of a pairing heap.
	- Now supports merging of two pairing heaps.
	
- 0.95  2012.04.27
	- Fixed the bug in `remove`
	
- 0.9
	- Initial release

Future plan:
------------
