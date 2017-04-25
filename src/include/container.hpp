/*
 * container.h
 *
 *  Created on: Nov 11, 2013
 *      Author: bdaw
 */

#ifndef CONTAINER_H
#define CONTAINER_H

/*
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
*/
#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include <pthread.h>
#include <memory.h>
#include <errno.h>
#include "pbs_error.h"

extern bool exit_called;


#define THING_NOT_FOUND    -2
#define ALREADY_IN_LIST     9
#define ALWAYS_EMPTY_INDEX  0


//#define CHECK_LOCKING

#ifdef CHECK_LOCKING
#define CHECK_LOCK if(!locked){char *ptr = NULL;while(1) *ptr++ = (char)0xff;}
#else
#define CHECK_LOCK
#endif

namespace container{ //Creating a scope to prevent my using from spilling past the include file.


//using namespace ::boost::multi_index;
//using namespace ::boost::multi_index::detail;

template <class T>
class item
  {
  public:

  item(std::string const &idString, T p): id(idString), ptr(p)
    {
    }

  bool operator == (
      
    const std::string &rhs) const

    {
    if (&rhs == NULL)
      return false;

    return id == rhs;
    }

  T get() const
    {
    return ptr;
    }

  std::string id;
  private:
  item(){}
  T ptr;
  };

template <class T> class slot
  {
  public:
  item<T> *pItem;
  int     next;
  int     prev;
  };

template <class T>
class item_container
  {
  public:

#if 0
  typedef multi_index_container<item<T>,
      indexed_by<
      sequenced<>,
      hashed_unique<member<item<T>, std::string, &item<T>::id > > > > indexed_container;

  typedef typename indexed_container::template nth_index<0>::type& sequenced_index;
  typedef typename indexed_container::template nth_index<0>::type::iterator sequenced_iterator;
  typedef typename indexed_container::template nth_index<0>::type::reverse_iterator sequenced_reverse_iterator;
  typedef typename indexed_container::template nth_index<1>::type& hashed_index;
  typedef typename indexed_container::template nth_index<1>::type::iterator hashed_iterator;
#endif

  class item_iterator
    {
  public:
    T get_next_item()
      {
#ifdef CHECK_LOCKING
      if (!*pLocked)
        {
        char *p = NULL;
        while(1)
          {
          *p++ = (char)0xff;
          }
        }
#endif
      if (exit_called)
        {
        return(NULL);
        }

      if (endHit)
        return(NULL);

      if (iter == ALWAYS_EMPTY_INDEX)
        {
        endHit = true;
        return(NULL);
        }
      item<T> *pItem;
      if (reversed)
        {
        pItem = pContainer->next_thing_from_back(&iter);
        
        if (pItem == NULL)
          {
          endHit = true;
          return(NULL);
          }
        return pItem->get();
        }
      
      pItem = pContainer->next_thing(&iter);

      if (pItem == NULL)
        {
        endHit = true;
        return(NULL);
        }

      return(pItem->get());
      } // END get_next_item()



    item_iterator(item_container<T> *pCtner,
#ifdef CHECK_LOCKING
        bool *locked,
#endif
        bool reverse = false)
      {
#ifdef CHECK_LOCKING
      pLocked = locked;
#endif
      pContainer = pCtner;
      iter = -1;
      reversed = reverse;
      pContainer->initialize_ra_iterator(&iter, this->reversed);
      endHit = false;
      }
    void reset(void) //Reset the iterator;
      {
#ifdef CHECK_LOCKING
    if(!*pLocked)
      {
      char *p = NULL;
      while(1)
        {
        *p++ = (char)0xff;
        }
      }
#endif
      iter = -1;
      pContainer->initialize_ra_iterator(&iter, this->reversed);
      endHit = false;
      }
  private:
    item_container<T> *pContainer;
    int iter;
    bool endHit;
    bool reversed;
#ifdef CHECK_LOCKING
    bool *pLocked;
#endif
    };

  item_container():

    updateCounter(0),
    max(0),
    num(0),
    next_slot(1),
    last(0)

    {
    pthread_mutex_init(&mutex, NULL);
    max = 10;
    slots = (slot<T> *)calloc(max, sizeof(slot<T>));
#ifdef CHECK_LOCKING
    locked = false;
#endif
    }



  ~item_container()
    {
    if (exit_called)
      {
      //If exit is called, don't free the slots.
      lock();
      map.clear();
      unlock();
      return;
      }

    if (slots != NULL)
      {
      clear();
      free(slots);
      slots = NULL;
      }
    }



  bool insert(
      
    T                  it,
    std::string const &id,
    bool               replace = false)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int index = map[id];
    if (index != ALWAYS_EMPTY_INDEX)
      {
      if (!replace) return false;
      remove_thing_from_index(index);
      }

    item<T> *pItem = new item<T>(id,it);
    if (insert_thing(pItem) < 0)
      {
      delete pItem;
      return false;
      }

    return true;
    }



  bool insert_after(
      
    std::string const &location_id,
    T                  it,
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int index = map[location_id];
    if (index == ALWAYS_EMPTY_INDEX)
      return false;
    
    item<T> *pItem = new item<T>(id,it);

    if (insert_thing_after(pItem,index) < 0)
      {
      delete pItem;
      return false;
      }

    return true;
    }



  bool insert_at(
      
    int                index,
    T                  it,
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int iter = -1;
    initialize_ra_iterator(&iter, false);
    while(index--)
      {
      item<T> *pItem = next_thing(&iter);
      if (pItem == NULL)
        return false;
      }

    item<T> *pItem = new item<T>(id,it);

    if (insert_thing_before(pItem,iter) < 0)
      {
      delete pItem;
      return false;
      }

    return true;
    }



  bool insert_first(
      
    T                  it,
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    return insert_at(0,it,id);
    }



  bool insert_before(
      
    std::string const &location_id,
    T                  it,
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int index = map[location_id];
    if (index == ALWAYS_EMPTY_INDEX)
      return false;

    item<T> *pItem = new item<T>(id,it);
    if (insert_thing_before(pItem,index) != PBSE_NONE)
      {
      delete pItem;
      return false;
      }

    return true;
    }



  bool remove(
      
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int index = map[id];
    if (index == ALWAYS_EMPTY_INDEX)
      return false;
    
    if (remove_thing_from_index(index) != PBSE_NONE)
      return false;

    return true;
    }



  T find(
      
    std::string const &id)

    {
    CHECK_LOCK
    if (exit_called)
      return  empty_val();

    int index = map[id];
    if (index == ALWAYS_EMPTY_INDEX)
      {
      return empty_val();
      }
    item<T> *pItem = slots[index].pItem;
    if (pItem == NULL)
      {
      return empty_val();
      }
    return pItem->get();
    }



  T pop(void)
    {
    CHECK_LOCK
    if (exit_called)
      return  empty_val();
    
    T pT = pop_thing();
    
    if (pT == NULL)
      return empty_val();

    return pT;
    }



  T pop_back(void)
    {
    CHECK_LOCK
    if (exit_called)
      return  empty_val();

    T pT = pop_back_thing();

    if (pT == NULL)
      return empty_val();

    return pT;
    }



  bool swap(
      
    std::string const &id1,
    std::string const &id2)

    {
    CHECK_LOCK
    if (exit_called)
      return false;

    int ind1 = map[id1];
    int ind2 = map[id2];

    if ((ind1 == ALWAYS_EMPTY_INDEX)||
        (ind2 == ALWAYS_EMPTY_INDEX)||
        (ind1 == ind2))
      {
      return false;
      }

    item<T> *pTmp = slots[ind1].pItem;
    slots[ind1].pItem = slots[ind2].pItem;
    slots[ind2].pItem = pTmp;
    map[id1] = ind2;
    map[id2] = ind1;

    return true;
    }



  item_iterator *get_iterator(
      
    bool reverse = false)

    {
    CHECK_LOCK

    if (exit_called)
      return(NULL);

    return new item_iterator(this,
#ifdef CHECK_LOCKING
        &locked,
#endif
        reverse);
    }



  void clear()
    {
    CHECK_LOCK
    if (exit_called)
      return;

    for (int i = 0; i < max; i++)
      {
      if (slots[i].pItem != NULL)
        {
        map.erase(slots[i].pItem->id);
        delete slots[i].pItem;
        slots[i].pItem = NULL;
        }

      slots[i].next = ALWAYS_EMPTY_INDEX;
      slots[i].prev = ALWAYS_EMPTY_INDEX;
      }

    num = 0;
    next_slot = 1;
    last = 0;
    }



  size_t count()
    {
    CHECK_LOCK
    if (exit_called)
      return 0;
    return num;
    }



  void lock(void)
    {
    pthread_mutex_lock(&mutex);
#ifdef CHECK_LOCKING
    locked = true;
#endif
    }



  void unlock(void)
    {
#ifdef CHECK_LOCKING
    locked = false;
#endif
    pthread_mutex_unlock(&mutex);
    }



  int trylock(void)
    {
#ifdef CHECK_LOCKING
    int ret = pthread_mutex_trylock(&mutex);
    if (!ret)
      {
      locked = true;
      }
    return ret;
#else
    return pthread_mutex_trylock(&mutex);
#endif
    }



  private:
  T empty_val(void)
    {
    return NULL;
    }


  int swap_things(
      
    item<T> *thing1,
    item<T> *thing2)

    {
    int index1 = get_index(thing1);
    int index2 = get_index(thing2);

    if ((index1 == THING_NOT_FOUND) ||
        (index2 == THING_NOT_FOUND))
      {
      return THING_NOT_FOUND;
      }

    slots[index1].item = thing2;
    slots[index2].item = thing1;

    return(PBSE_NONE);
    } /* END swap_things() */



  int check_and_resize()

    {
    slot<T>        *tmp;
    size_t       remaining;
    size_t       size;

    if (max == num + 1)
      {
      /* double the size if we're out of space */
      size = (max * 2) * sizeof(slot<T>);

      if ((tmp = (slot<T> *)realloc(slots,size)) == NULL)
        {
        //log_err(ENOMEM,__func__,"No memory left to resize the array");
        return(ENOMEM);
        }

      remaining = max * sizeof(slot<T>);

      memset(tmp + max, 0, remaining);

      slots = tmp;

      max = max * 2;
      }

    return(PBSE_NONE);
    } /* END check_and_resize() */



  void update_next_slot() /* M */

    {
    while ((next_slot < max) &&
           (slots[next_slot].pItem != NULL))
      next_slot++;
    } /* END update_next_slot() */



  /*
   * inserts an item, resizing the array if necessary
   *
   * @return the index in the array or -1 on failure
   */

  int insert_thing(

    item<T>  *thing)

    {
    int rc;

    /* check if the array must be resized */
    if ((rc = check_and_resize()) != PBSE_NONE)
      {
      return(-1);
      }

    slots[next_slot].pItem = thing;
    map[thing->id] = next_slot;

    /* save the insertion point */
    rc = next_slot;

    /* handle the backwards pointer, next pointer is left at zero */
    slots[rc].prev = last;

    /* make sure the empty slot points to the next occupied slot */
    if (last == ALWAYS_EMPTY_INDEX)
      {
      slots[ALWAYS_EMPTY_INDEX].next = rc;
      }

    /* update the last index */
    slots[last].next = rc;
    last = rc;
    slots[ALWAYS_EMPTY_INDEX].prev = last;

    /* update the new item's next index */
    slots[rc].next = ALWAYS_EMPTY_INDEX;

    /* increase the count */
    num++;

    update_next_slot();

    return(rc);
    } /* END insert_thing() */



  /*
   * inserts a thing after the thing in index
   * NOTE: index must represent a valid index
   */

  int insert_thing_after(

    item<T>         *thing,
    int              index)

    {
    int rc;
    int next;

    /* check if the array must be resized */
    if ((rc = check_and_resize()) != PBSE_NONE)
      {
      return(-1);
      }

    /* insert this element */
    slots[next_slot].pItem = thing;
    map[thing->id] = next_slot;

    /* save the insertion point */
    rc = next_slot;

    /* move pointers around */
    slots[rc].prev = index;
    next = slots[index].next;
    slots[rc].next = next;
    slots[index].next = rc;
    slots[next].prev = rc;

    /* update the last index if needed */
    if (last == index)
      last = rc;

    /* increase the count */
    num++;

    update_next_slot();

    return(rc);
    } /* END insert_thing_after() */



  /*
   * inserts a thing before the thing in index
   * NOTE: index must represent a valid index
   */
  int insert_thing_before(

    item<T>         *thing,
    int              index)

    {
    int rc;
    int prev;

    /* check if the array must be resized */
    if ((rc = check_and_resize()) != PBSE_NONE)
      {
      return(-1);
      }

    /* insert this element */
    slots[next_slot].pItem = thing;
    map[thing->id] = next_slot;

    /* save the insertion point */
    rc = next_slot;

    /* move pointers around */
    prev = slots[index].prev;
    slots[rc].next = index;
    slots[rc].prev = prev;
    slots[index].prev = rc;
    slots[prev].next = rc;

    /* increase the count */
    num++;

    update_next_slot();

    return(rc);
    } /* END insert_thing_before() */


  bool is_present(

    item<T>      *thing)

    {
    int i = slots[ALWAYS_EMPTY_INDEX].next;

    while (i != 0)
      {
      if (slots[i].pItem == thing)
        return(true);

      i = slots[i].next;
      }

    return(false);
    } /* END is_present() */



  /*
   * fix the next pointer for the box pointing to this index
   *
   * @param ra - the array we're fixing
   * @param index - index of the slot we're unlinking
   */
  void unlink_slot(

    int              index)

    {
    int prev = slots[index].prev;
    int next = slots[index].next;

    if (map.find(slots[index].pItem->id) != map.end())
      map.erase(slots[index].pItem->id);

    slots[index].prev = ALWAYS_EMPTY_INDEX;
    slots[index].next = ALWAYS_EMPTY_INDEX;
    delete slots[index].pItem;
    slots[index].pItem = NULL;

    slots[prev].next = next;

    /* update last if necessary, otherwise update prev's next index */
    if (last == index)
      last = prev;
    else
      slots[next].prev = prev;
    } /* END unlink_slot() */



  /*
   * remove a thing from the array
   *
   * @param thing - the thing to remove
   * @return PBSE_NONE if the thing is removed
   */

  int remove_thing(

    item<T>            *thing)

    {
    int i = slots[ALWAYS_EMPTY_INDEX].next;
    bool found = false;

    /* find the thing */
    while (i != ALWAYS_EMPTY_INDEX)
      {
      if (slots[i].pItem == thing)
        {
        found = true;
        break;
        }

      i = slots[i].next;
      }

    if (!found)
      return(THING_NOT_FOUND);

    unlink_slot(i);

    num--;

    /* reset the next_slot index if necessary */
    if (i < next_slot)
      {
      next_slot = i;
      }

    return(PBSE_NONE);
    } /* END remove_thing() */



  item<T> *remove_thing_memcmp(

    item<T>           *thing,
    unsigned int     size)

    {
    int   i = slots[ALWAYS_EMPTY_INDEX].next;
    void *item = NULL;

    while (i != ALWAYS_EMPTY_INDEX)
      {
      /* check if equal */
      if (!memcmp(slots[i].pItem, thing, size))
        {
        item = slots[i].pItem;

        unlink_slot(i);

        num--;

        if (i < next_slot)
          next_slot = i;

        break;
        }

      i = slots[i].next;
      }

    return(item);
    } /* END remove_thing_memcmp() */

  /*
   * pop the first thing from the array
   *
   * @return the first thing in the array or NULL if empty
   */

  T pop_thing()

    {
    item<T> *thing = NULL;
    int   i = slots[ALWAYS_EMPTY_INDEX].next;
    T pT = NULL;

    if (i != ALWAYS_EMPTY_INDEX)
      {
      /* get the thing we're returning */
      thing = slots[i].pItem;
      pT = thing->get();

      /* handle the deletion and removal */
      unlink_slot(i);

      num--;

      /* reset the next slot index if necessary */
      if (i < next_slot)
        {
        next_slot = i;
        }
      }

    return(pT);
    } /* END pop_thing() */


  T pop_back_thing()

    {
    item<T> *thing = NULL;
    int   i = slots[ALWAYS_EMPTY_INDEX].prev;
    T pT = NULL;

    if (i != ALWAYS_EMPTY_INDEX)
      {
      /* get the thing we're returning */
      thing = slots[i].pItem;
      pT = thing->get();

      /* handle the deletion and removal */
      unlink_slot(i);

      num--;

      /* reset the next slot index if necessary */
      if (i < next_slot)
        {
        next_slot = i;
        }
      }

    return(pT);
    } /* END pop_thing() */



  int remove_thing_from_index(

    int index)

    {
    int rc = PBSE_NONE;

    if (slots[index].pItem == NULL)
      rc = THING_NOT_FOUND;
    else
      {
      /* FOUND */
      unlink_slot(index);

      num--;

      if (index < next_slot)
        next_slot = index;
      }

    return(rc);
    } /* END remove_thing_from_index() */


  int remove_last_thing()

    {
    return(remove_thing_from_index(last));
    } /* END remove_last_thing() */



  /*
   * returns the next available item and increments *iter
   */
  item<T> *next_thing(

    int             *iter)

    {
    item<T> *thing;
    int   i = *iter;

    if (i == -1)
      {
      /* initialize first */
      i = slots[ALWAYS_EMPTY_INDEX].next;
      }

    thing = slots[i].pItem;
    *iter = slots[i].next;

    return(thing);
    } /* END next_thing() */



  /*
   * returns the next available item from the back and decrements *iter
   */
  item<T> *next_thing_from_back(

    int             *iter)

    {
    item<T> *thing;
    int   i = *iter;

    if (i == -1)
      {
      /* initialize first */
      i = last;
      }

    thing = slots[i].pItem;
    *iter = slots[i].prev;

    return(thing);
    } /* END next_thing_from_back() */

  /*
   * initialize the iterator for this array
   */
  void initialize_ra_iterator(

    int  *iter,
    bool  reversed)

    {
    if (reversed == false)
      *iter = slots[ALWAYS_EMPTY_INDEX].next;
    else
      *iter = slots[ALWAYS_EMPTY_INDEX].prev;
    } /* END initialize_ra_iterator() */



  /*
   * searches the array for thing, finding the index
   *
   * @param ra - the array to be searched
   * @param thing - the thing we're looking for
   * @return index if present, THING_NOT_FOUND otherwise
   */

  int get_index(

    item<T> *thing)

    {
    try
    {
      int i = map[thing->id];
      return i;
    }catch(...)
      {
      return(THING_NOT_FOUND);
      }
    } /* END get_index() */



  item<T> *get_thing_from_index(

    int index)

    {
    if (index == -1)
      index = slots[ALWAYS_EMPTY_INDEX].next;

    if (index >= max)
      return(NULL);
    else
      return(slots[index].pItem);
    } /* END get_thing_from_index() */

  //indexed_container container;
  pthread_mutex_t mutex;
  unsigned long updateCounter;
  slot<T> *slots;
  int max;
  int num;
  int next_slot;
  int last;
  boost::unordered_map<std::string, int> map;
#ifdef CHECK_LOCKING
  bool locked;
#endif
  };

} //End of namespace scope.

#endif
