#include <stdexcept>
#include <iostream>

#include "execution_slot_tracker.hpp"
#include "pbs_error.h"

const int  OUT_OF_RANGE = -1;
const int  INSUFFICIENT_FREE_EXECUTION_SLOTS = -2;
const int  SUBSET_TOO_LARGE = -3;

const bool OCCUPIED = true;
const bool FREE = false;

execution_slot_tracker::execution_slot_tracker(const execution_slot_tracker& est)
  {
  this->slots = est.slots;
  this->open_count = est.open_count;
  } /* END copy constructor */



execution_slot_tracker::execution_slot_tracker() : open_count(0)
  {
  } /* END default contructor */



execution_slot_tracker::execution_slot_tracker(
   
  const int size)

  {
  this->open_count = 0;
  
  for (int i = 0; i < size; i++)
    this->add_execution_slot();
  }


execution_slot_tracker& execution_slot_tracker::operator= (
	
  const execution_slot_tracker& est)

  {
  if (this == &est)
    return(*this);

  this->slots = est.slots;
  this->open_count = est.open_count;
  return(*this);
  } /* END = operator */



/*
 * unset_subset()
 * @pre-cond: subset must be of an equal or smaller size than this execution slot tracker object
 * @post-cond: all of the occupied slots in subset will become unoccupied in this
 */
int execution_slot_tracker::unset_subset(

  const execution_slot_tracker &subset)

  {
  if (subset.get_total_execution_slots() > this->get_total_execution_slots())
    return(SUBSET_TOO_LARGE);

  for (int i = 0; i < subset.get_total_execution_slots(); i++)
    {
    if (subset.slots[i] == OCCUPIED)
      this->mark_as_free(i);
    }

  return(PBSE_NONE);
  }



/*
 * mark_as_used() 
 * marks the slot at index index as currently occupied
 * @pre-cond: index must be a valid index into the vector
 * @post-cond: slots[index] will be occupied and open_count will be updated if needed
 * @return PBSE_NONE on success or OUT_OF_RANGE if index isn't a valid index
 */
int execution_slot_tracker::mark_as_used (
  
  int index)

  {
  int size = this->slots.size();

  if ((index < 0) ||
      (index >= size))
    return(OUT_OF_RANGE);

  try 
    {
    if (this->slots[index] == FREE)
      {
      this->slots[index] = OCCUPIED;
      this->open_count--;
      }

    return(PBSE_NONE);
    }
  catch (std::out_of_range &oor)
    {
    return(OUT_OF_RANGE);
    }
  }



/*
 * mark_as_free() 
 * marks the slot at index index as free 
 * @pre-cond: index must be a valid index into the vector
 * @post-cond: slots[index] will be free and open_count will be updated if needed
 * @return PBSE_NONE on success or OUT_OF_RANGE if index isn't a valid index
 */
int execution_slot_tracker::mark_as_free (

  int index)

  {
  int size = this->slots.size();

  if ((index < 0) ||
      (index >= size))
    return(OUT_OF_RANGE);
  
  try
    {
    if (this->slots[index] == OCCUPIED)
      {
      this->slots[index] = FREE;
      this->open_count++;
      }

    return(PBSE_NONE);
    }
  catch (std::out_of_range &oor)
    {
    return(OUT_OF_RANGE);
    }
  }
  


int execution_slot_tracker::reserve_execution_slot(
    
  int                     index,
  execution_slot_tracker &subset)

  {
  int rc;
 
  while (subset.get_total_execution_slots() < this->get_total_execution_slots())
    subset.add_execution_slot();

  if ((rc = this->mark_as_used(index)) == PBSE_NONE)
    {

    if ((rc = subset.mark_as_used(index)) != PBSE_NONE)
      {
      this->mark_as_free(index);
      }
      
    return(rc);
    }

  return(rc);
  }



/*
 * reserve_execution_slots()
 * reserves num_slots_to_reserve from this and marks the same ones as occupied in est
 * @pre-cond: est must be a valid execution_slot_tracker object
 * @post-cond: both this and est will have num_slots_to_reserve more slots set as
 * occupied. est will be resized if necessary to accomodate this functionality.
 * @returns INSUFFICIENT_FREE_EXECUTION_SLOTS if this object doesn't have enough
 * slots to accomodate the requested reservation or PBSE_NONE on success.
 */
int execution_slot_tracker::reserve_execution_slots(

  int                     num_slots_to_reserve,
  execution_slot_tracker &est)

  {
  int reserved_so_far = 0;

  if (this->open_count < num_slots_to_reserve)
    return(INSUFFICIENT_FREE_EXECUTION_SLOTS);

  while (est.get_total_execution_slots() < this->get_total_execution_slots())
    est.add_execution_slot();

  for (int i = 0; i < this->slots.size() && reserved_so_far < num_slots_to_reserve; i++)
    {
    if (this->slots[i] == FREE)
      {
      reserved_so_far++;
      this->mark_as_used(i);

      est.mark_as_used(i);
      }
    }

  return(PBSE_NONE);
  }


/* 
 * unreserve_execution_slots()
 *
 * @pre-cond:  subset must be smaller than or equal to this in size.
 * @post-cond: all occupied slots in subset will become free in this.
 * @return SUBSET_TOO_LARGE if subset is larger than this or PBSE_NONE
 * on success
 */
int execution_slot_tracker::unreserve_execution_slots(

  const execution_slot_tracker &subset)

  {
  if (this->slots.size() < subset.slots.size())
    return(SUBSET_TOO_LARGE);

  for (int i = 0; i < subset.slots.size(); i++)
    {
    if (subset.slots[i] == OCCUPIED)
      this->mark_as_free(i);
    }

  return(PBSE_NONE);
  }



int execution_slot_tracker::get_number_free() const
  {
  return(this->open_count);
  }



int execution_slot_tracker::get_total_execution_slots() const
  {
  return(this->slots.size());
  }



void execution_slot_tracker::add_execution_slot ()

  {
  this->slots.push_back(FREE);
  this->open_count++;
  }



int execution_slot_tracker::remove_execution_slot ()
  {
  try
    {
    if (this->slots[this->slots.size() - 1] == FREE)
      this->open_count--;

    this->slots.pop_back();

    return(PBSE_NONE);
    }
  catch (...)
    {
    return(-4);
    }
  }


/*
 * get_next_occupied_index()
 * used to iterate over this execution slot tracker object
 *
 * @param iterator: an index that we should start iterating from. -1 to begin.
 */
int execution_slot_tracker::get_next_occupied_index(

  int &iterator) const

  {
  int occupied_index = -1;

  if (iterator == -1)
    iterator = 0;

  while (iterator < this->slots.size())
    {
    if (this->slots[iterator] == OCCUPIED)
      {
      occupied_index = iterator;
      iterator++;
      break;
      }
    else
      iterator++;
    }

  return(occupied_index);
  }

bool execution_slot_tracker::is_occupied(

  int index) const

  {
  int  size = this->slots.size();
  bool occupied = false;

  if ((index < 0) ||
      (index >= size))
    return(false);
  
  try
    {
    occupied = this->slots[index] == OCCUPIED;
    }
  catch (std::out_of_range &oor)
    {
    return(false);
    }

  return(occupied);
  }
