PairingHeap
============================================

An implementation of Pairing Heaps.
--------------------------------------------

A pairing heap is a rooted tree satisfying the heap property.

This implementation is addressable with id's and supports the usual operations of priority queues:

- `find_min`
- `delete_min`
- `insert`
- `remove`
- `decrease_key`.
	
Time complexity:

- Delete_min takes `O(logn)` amortized time.
- Decrease_key was shown to run in `O(loglogn) <= T <= O(logn)` amortized time.
- All other methods take constant amortized time.
	

Latest version: 	0.95
-----------------------

Known issues:
-------------
- none

Change log:
-----------
- 0.95  2012.04.27
	- fixed the bug in `remove`
- 0.9
	- initial release

Future plan:
------------
- optimize the structure so that a maximum size is not needed in advance.
- implement merge for different pairing heaps.
