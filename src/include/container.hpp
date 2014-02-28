/*
 * container.h
 *
 *  Created on: Nov 11, 2013
 *      Author: bdaw
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#include <string>
#include <vector>
#include <pthread.h>
#include <memory.h>

#define THING_NOT_FOUND    -2
#define ALREADY_IN_LIST     9

#define CHECK_LOCKING

#ifdef CHECK_LOCKING
#define CHECK_LOCK if(!locked){char *ptr = NULL;while(1) *ptr++ = (char)0xff;}
#else
#define CHECK_LOCK
#endif

namespace container{ //Creating a scope to prevent my using from spilling past the include file.


using namespace ::boost::multi_index;
using namespace ::boost::multi_index::detail;

template <class T>
class item
  {
  public:
  item(std::string idString,T p):id(idString),ptr(p)
    {
    pointer_check();
    }
  item(const char *idString,T p):ptr(p)
    {
    if(idString == NULL)
      {
      id = "";
      }
    else
      {
      id = idString;
      }
    pointer_check();
    }
  bool operator == (const std::string& rhs) const
    {
    if(&rhs == NULL) return false;
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
  void pointer_check(void){}
  };

template <class T>
class item_container
  {
  public:

  typedef multi_index_container<item<T>,
      indexed_by<
      sequenced<>,
      hashed_unique<member<item<T>, std::string, &item<T>::id > > > > indexed_container;

  typedef typename indexed_container::template nth_index<0>::type& sequenced_index;
  typedef typename indexed_container::template nth_index<0>::type::iterator sequenced_iterator;
  typedef typename indexed_container::template nth_index<0>::type::reverse_iterator sequenced_reverse_iterator;
  typedef typename indexed_container::template nth_index<1>::type& hashed_index;
  typedef typename indexed_container::template nth_index<1>::type::iterator hashed_iterator;

  class item_iterator
    {
  public:
    T get_next_item()
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
    hashed_index hi = pContainer->get<1>();
    while(snapshot_iter != snapshot.end())
      {
      hashed_iterator it = hi.find(*snapshot_iter);
      snapshot_iter++;
      if(it != hi.end())
        {
        return it->get();
        }
      }
    return NULL;
      }
    item_iterator(indexed_container *pCtner,
#ifdef CHECK_LOCKING
        bool *locked,
#endif
        bool reverse = false)
      {
#ifdef CHECK_LOCKING
      pLocked = locked;
#endif
      pContainer = pCtner;
      if(reverse)
        {
        for(sequenced_reverse_iterator riter = pContainer->get<0>().rbegin();riter != pContainer->get<0>().rend();riter++)
          {
          snapshot.push_back(riter->id);
          }
        }
      else
        {
        for(sequenced_iterator iter = pContainer->get<0>().begin();iter != pContainer->get<0>().end();iter++)
          {
          snapshot.push_back(iter->id);
          }
        }
      snapshot_iter = snapshot.begin();
      }
  private:
    std::vector<std::string> snapshot;
    std::vector<std::string>::iterator snapshot_iter;
    indexed_container *pContainer;
#ifdef CHECK_LOCKING
    bool *pLocked;
#endif
    };

  item_container()
    {
    memset(&mutex,0,sizeof(mutex));
#ifdef CHECK_LOCKING
    locked = false;
#endif
    }
  bool insert(T it,const char *id,bool replace = false)
    {
    CHECK_LOCK
    if(id == NULL) return false;
    return insert(it,std::string(id),replace);
    }
  bool insert(T it,std::string id,bool replace = false)
    {
    CHECK_LOCK
    std::pair<hashed_iterator,bool> ret;
    ret = container.get<1>().insert(item<T>(id,it));
    if(!ret.second && replace)
      {
      return container.get<1>().replace(ret.first,item<T>(id,it));
      }
    return ret.second;
    }
  bool insert_after(const char *location_id,T it,const char *id)
    {
    CHECK_LOCK
    if((id == NULL)||(location_id == NULL)) return false;
    return insert_after(std::string(location_id),it,std::string(id));
    }
  bool insert_after(std::string location_id,T it,std::string id)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator iter = ind.begin();
    while(iter != ind.end())
      {
      if(*iter == location_id)
        {
        break;
        }
      iter++;
      }
    if(iter == ind.end()) return false;
    iter++;
    std::pair<sequenced_iterator,bool> ret;
    ret = container.get<0>().insert(iter,item<T>(id,it));
    return ret.second;
    }
  bool insert_at(int index,T it,const char *id)
    {
    CHECK_LOCK
    if(id == NULL) return false;
    return insert_at(index,it,std::string(id));
    }
  bool insert_at(int index,T it,std::string id)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator iter = ind.begin();
    while(index--)
      {
      if(iter == ind.end()) return false;
      iter++;
      }
    std::pair<sequenced_iterator,bool> ret;
    ret = container.get<0>().insert(iter,item<T>(id,it));
    return ret.second;
    }

  bool insert_first(T it,const char *id)
    {
    CHECK_LOCK
    if(id == NULL) return false;
    return insert_first(it,std::string(id));
    }
  bool insert_first(T it,std::string id)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator iter = ind.begin();
    std::pair<sequenced_iterator,bool> ret;
    ret = container.get<0>().insert(iter,item<T>(id,it));
    return ret.second;
    }
  bool insert_before(const char *location_id,T it,const char *id)
    {
    CHECK_LOCK
    if((id == NULL)||(location_id == NULL)) return false;
    return insert_before(std::string(location_id),it,std::string(id));
    }
  bool insert_before(std::string location_id,T it,std::string id)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator iter = ind.begin();
    while(iter != ind.end())
      {
      if(*iter == location_id)
        {
        break;
        }
      iter++;
      }
    if(iter == ind.end()) return false;
    std::pair<sequenced_iterator,bool> ret;
    ret = container.get<0>().insert(iter,item<T>(id,it));
    return ret.second;
    }
  bool remove(const char *id)
    {
    CHECK_LOCK
    if(id == NULL) return false;
    return remove(std::string(id));
    }
  bool remove(std::string id)
    {
    CHECK_LOCK
    hashed_index hi = container.get<1>();
    hashed_iterator it = hi.find(id);
    if(it == hi.end())
      {
      return false;
      }
    hi.erase(it);
    return true;
    }
  T find(const char *id)
    {
    CHECK_LOCK
    if(id == NULL) return empty_val();
    return find(std::string(id));
    }
  T find(std::string id)
    {
    CHECK_LOCK
    hashed_index hi = container.get<1>();
    hashed_iterator it = hi.find(id);
    if(it == hi.end())
      {
      return empty_val();
      }
    return it->get();
    }
  T pop(void)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator it = ind.begin();
    if(it == ind.end()) return empty_val();
    T pT = it->get();
    ind.erase(it);
    return pT;
    }
  T pop_back(void)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator it = ind.end();
    if(ind.size() == 0) return empty_val();
    it--;
    T pT = it->get();
    ind.erase(it);
    return pT;
    }

  bool swap(const char *id1,const char *id2)
    {
    CHECK_LOCK
    if((id1 == NULL)||(id2 == NULL)) return false;
    return swap(std::string(id1),std::string(id2));
    }
  bool swap(std::string id1,std::string id2)
    {
    CHECK_LOCK
    sequenced_index ind = container.get<0>();
    sequenced_iterator it1 = ind.begin();
    while(it1 != ind.end())
      {
      if(*it1 == id1)
        {
        break;
        }
      it1++;
      }
    if(it1 == ind.end())
      {
      return false;
      }
    sequenced_iterator it2 = ind.begin();
    while(it2 != ind.end())
      {
      if(*it2 == id2)
        {
        break;
        }
      it2++;
      }
    if(it2 == ind.end())
      {
      return false;
      }
    sequenced_iterator tmp = it2;
    tmp++;
    ind.relocate(it1,it2);
    ind.relocate(tmp,it1);
    return true;
    }
  item_iterator *get_iterator(bool reverse = false)
    {
    CHECK_LOCK
    return new item_iterator(&container,
#ifdef CHECK_LOCKING
        &locked,
#endif
        reverse);
    }
  void clear()
    {
    CHECK_LOCK
    container.get<0>().clear();
    }
  size_t count()
    {
    CHECK_LOCK
    return container.size();
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
    if(!ret)
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
  void integrity_check(void){}
  indexed_container container;
  pthread_mutex_t mutex;
#ifdef CHECK_LOCKING
  bool locked;
#endif
  };

} //End of namespace scope.

#endif
