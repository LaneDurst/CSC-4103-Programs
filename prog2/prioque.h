// ** VERSION 8.2 **
//
// Priority Queue (c) 1985-2024 by Golden G. Richard III (@nolaforensix)
//
// Major update October 2004: The package is now thread-safe.  The
// package now depends on the pthreads library (for access to
// mutexes). All functions now take one or more pointers to a queue or
// context, so existing applications will need to be modified
// slightly.
//
// April 2007: Added an option to init_queue to allow priority field
// to serve only as a tag--the queue is not sorted by the priority
// field when 'priority_is_tag_only' is set and the queue operates as
// a strict FIFO data structure.
//
// September 2017: Minor code cleanup and fix to add_to_queue to
// ensure strict add-to-rear for queues with equal priority elements.
//
// Minor cleanup in March 2021 and changes to API that may break older
// code:
//
// o remove_from_front(), peek_at_current(), and
// pointer_to_current() now return a non-NULL value if the queue isn't
// empty or return NULL if the queue is empty (rather than failing
// with a consistency check). This behavior is more multithreading
// friendly and in particular, for remove_from_front(), provides an
// atomic method for checking for an element in the queue and
// removing the element.
//
// o add_to_queue() was rewritten to simplify the implementation.  See
// the documentation for add_to_queue() below to verify that the new
// behavior is what you expect.
//
// o Based on valgrind analysis, some potential thread safety issues
// were addressed.
//
// o Queue lengths are now larger to support "big data".  :)
//
// August 2021: Optimized insertion speed for strict FIFO
// queues. Deleted local view functions, as the semantics are too
// complicated in the face of global queue updates and they are no
// longer used in any production code that I'm aware of.  Rewrote
// copy_queue().
//
// March 2022: Added serialization functions for reading and writing
// queues to disk. Serialization functions store binary data, so they
// are not platform-independent.  Added remove_from_front_sync(),
// which allows atomically calling a function while the queue lock is
// held if an element is successfully removed.
//
// June 2023: Reworked element serialization functions for queue
// serialization/deserialization to make them more flexible.  Also
// added queue initialization detection.  peek_at_current() now
// extracts priority as well as queue element data. Added global
// locking functions lock_queue() and unlock_queue() to allow complete
// control over queue thread safety via the nolock_* versions of the
// queueing functions.
//
// April 2024: Now uses fixed-sized int types and stdbool.h Boolean
// values.

// for queue serialization / deserialization--this type is used
// frequently in some external packages that depend on prioque and a
// check is necessary to make sure the type isn't already defined.
#if ! defined(STATE_SERIALIZATION_DEF)
#define STATE_SERIALIZATION_DEF
// serialization mode for reading/writing scalpel state
typedef enum StateSerialization {
  SERIALIZE, DESERIALIZE
} StateSerialization;
#endif

#if ! defined(QUEUE_TYPE_DEFINED)
#define QUEUE_TYPE_DEFINED

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#define CONSISTENCY_CHECKING 1

// type of one element in a queue 
typedef struct _Queue_element {
  void *info;
  int32_t priority;
  struct _Queue_element *next;
} *Queue_element;

// basic queue type 
typedef struct Queue {
  Queue_element queue;		                     // head of queue
  Queue_element tail;                                // tail of queue
  Queue_element current;	                     // current position for sequential access functions 
  Queue_element previous;	                     // one step back from current 
  uint64_t queuelength;	                             // # of elements in queue 
  uint32_t elementsize;	                             // 'sizeof()' one element 
  bool duplicates;	                             // are duplicates allowed? 
  int (*compare) (const void *e1, const void *e2);   // element comparision function 
  pthread_mutex_t lock;                              // lock on queue operations
  bool priority_is_tag_only;                         // if true, ignore priority and use strict FIFO
  uint64_t magic;                                    // set on initialization 
} Queue;


///////////////////////////////////////////////////////////////////////
//
// NOTE: init_queue() must be called for a new queue before any other
// "prioque.c" functions are called, aside from init_queue().
//
///////////////////////////////////////////////////////////////////////


// function prototypes and descriptions for visible "prioque.c" functions

////////////////////////////
// SECTION 1
////////////////////////////

// initializes a new queue 'q' to have elements of size 'elementsize'.
// If 'duplicates' is true, then duplicate elements in the queue are
// allowed, otherwise duplicates are silently deleted.  The
// element-comparing function 'compare' is required only if
// duplicates==false or either equal_queues() or element_in_queue()
// are used (otherwise, a null function is acceptable).  'compare'
// should be a standard qsort()-style element comparison function:
// returns 0 if elements match, otherwise a non-0 result (<, > cases
// are not used).
//
//   IMPORTANT: *Only* the 'compare' function is used for duplicate
//   detection.
void init_queue(Queue *q, uint32_t elementsize, bool duplicates,
		int (*compare) (const void *e1, const void *e2),
		bool priority_is_tag_only);

// returns true if init_queue() has been called on 'q', otherwise
// false.
bool queue_initialized(Queue q);


// destroys all elements in 'q'.  An empty queue has no associated
// dynamically allocated storage, so a destructor isn't required.
void destroy_queue(Queue *q);


// adds 'element' to the 'q' with position based on 'priority'.
// Elements with HIGHER-numbered priorities are placed closer to the
// front of the queue, with strict 'to the rear' placement for items
// with equal priority (that is, given two items with equal priority,
// the one most recently added will appear closer to the rear of the
// queue).
void add_to_queue(Queue *q, void *element, int32_t priority);


// removes the element at the front of the 'q' and places it in
// 'element'.  If the queue is empty, returns NULL, otherwise a
// non-NULL value.
void *remove_from_front(Queue *q, void *element);


// removes the element at the front of the 'q' and places it in
// 'element'.  If the queue is empty, returns NULL, otherwise a
// non-NULL value.  If an element is removed, 'change' is added to the
// atomic_uint 'j' before the lock on the queue is released.
void *remove_from_front_sync(Queue *q, void *element, 
			     atomic_uint *j, int32_t change);


// returns true if the 'element' exists in the 'q', otherwise false.
// The 'compare' function is used for matching.  As a side-effect, the
// current position in the queue is set to matching element, so
// 'update_current()' can be used to update the value of the
// 'element'.  If the element is not found, the current position is
// set to the first element in the queue.
bool element_in_queue(Queue *q, void *element);


// delete an element from the queue.  If the element isn't in the
// queue, no action is performed and false is returned, otherwise true
// is returned.
bool delete_from_queue(Queue *q, void *element);


// returns true if 'q' is empty, false otherwise.
bool empty_queue(Queue *q);


// returns the number of elements in the 'q'.
uint64_t queue_length(Queue *q);


// makes a copy of 'q2' into 'q1'.  'q2' is not modified.
void copy_queue(Queue *q1, Queue *q2);


// determines if 'q1' and 'q2' are equivalent.  Uses the 'compare'
// function of the first queue, which should match the 'compare' for
// the second!  Returns true if the queues are equal, otherwise
// returns false.
bool equal_queues(Queue *q1, Queue *q2);


// merge 'q2' into 'q1'.   'q2' is not modified.
void merge_queues(Queue *q1, Queue *q2);


// write a queue to a file handle 'fp'.  Requires specification of a
// function that can write a single element to disk--this function
// should return true if the function successfully writes the element,
// otherwise false.  'serialize_queue' returns true if all elements
// are successfully written, otherwise false.  The serialize_element()
// function will be passed a mode of SERIALIZE.
bool serialize_queue(Queue *q,
		     bool (*serialize_element)(void *element,
					      int32_t *priority,
					      FILE *fp,
					      StateSerialization mode),
		     FILE *fp);


// read a queue from a file handle 'fp'.  Requires specification of a
// function that can read a single element from disk--this function
// should return true if the function successfully reads the element, 0
// on EOF, -1 on error.  'deserialize_queue' returns true if all
// elements are successfully read, otherwise false.  The
// deserialize_element() function will be passed a mode of
// DESERIALIZE.  init_queue() *must* have been called on the queue
// before calling deserialize_queue().
bool deserialize_queue(Queue *q,
		       bool (*deserialize_element)(void *element,
						   int32_t *priority,
						   FILE *fp,
						   StateSerialization mode),
		       FILE *fp);


////////////////////////////
// SECTION 2
////////////////////////////

// The following are functions used to "walk" the queue (globally)
// like a linked list, examining or deleting elements along the way.
// Current position is rewound to the beginning by functions above (in
// SECTION 1), except for 'queue_initialized()', 'empty_queue()' and
// 'queue_length()', which do not modify the global current
// position. While these functions are thread-safe, obviously the
// global position will be changed in potentially unpredictable ways
// by overlapping access, so external synchronization is supported via
// lock_queue() and unlock_queue() (see Section 3, below).

// move to the first element in the 'q' 
void rewind_queue (Queue *q);


// move to the next element in the 'q'.
void next_element (Queue *q);


// allows update of current element.  The priority should not be
// changed by this function!
void update_current (Queue *q, void *element);


// peek at the element stored at the current position in the 'q' and
// copy its data to 'element' and priority to 'priority'.  Returns
// NULL if the queue is empty or a non-NULL value otherwise.
void *peek_at_current (Queue *q, void *element, int32_t *priority);


// return a pointer to the current element.  Returns NULL if the queue
// is empty or there is no current element. 
void *pointer_to_current (Queue *q);


// return priority of current element in the 'q'.
int32_t current_priority (Queue *q);


// delete the element stored at the current position.
void delete_current (Queue *q);


// has the current position in 'q' moved beyond the last valid
// element?  Returns true if so, false otherwise.
bool end_of_queue (Queue *q);


////////////////////////////
// SECTION 3
////////////////////////////

// The following functions allow complete control over queue locking
// and thread-safety issues.  You should generally avoid these unless
// you *really* need them and know what you're doing.  You must not
// call functions in Section 1 or 2 between a lock_queue() and
// unlock_queue() call or very bad things will happen.

// lock the queue globally, to allow use of nolock_* functions
void lock_queue(Queue *q);

// unlock the queue
void unlock_queue(Queue *q);

// thread-unsafe function versions (unless used with lock_queue() and unlock_queue()).
void nolock_next_element(Queue *q);
void nolock_rewind_queue(Queue *q);
bool nolock_element_in_queue(Queue *q, void *element);
void nolock_destroy_queue(Queue *q);
void nolock_add_to_queue(Queue *q, void *element, int32_t priority);
void *nolock_pointer_to_current(Queue *q);
int32_t nolock_current_priority(Queue *q);
bool nolock_end_of_queue(Queue *q);
uint64_t nolock_queue_length(Queue *q);
bool nolock_empty_queue(Queue *q);
void *nolock_nosync_remove_from_front(Queue *q, void *element);
void *nolock_peek_at_current(Queue *q, void *element, int32_t *priority);
void nolock_delete_current(Queue *q);
bool nolock_delete_from_queue(Queue *q, void *element);
void nolock_update_current(Queue *q, void *element);
#endif

