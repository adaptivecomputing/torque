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
#include <boost/multi_index/identity.hpp>
#include <string>
#include <pthread.h>
#include <memory.h>

#define THING_NOT_FOUND    -2
#define ALREADY_IN_LIST     9


namespace container{ //Creating a scope to prevent my using from spilling past the include file.

using namespace ::boost::multi_index;
using namespace ::boost::multi_index::detail;

template <class T>
class item
{
public:
	item(std::string idString):id(idString){}
	item(std::string idString,T p):id(idString),ptr(p){}
	item(const char *idString,T p):id(idString),ptr(p){}
    bool operator == (const item& rhs) const
    {
    	return id == rhs.id;
    }
    std::string idString() const
    {
    	return id;
    }
    T get()
    {
    	return ptr;
    }
private:
	item(){}
	std::string id;
	T ptr;
};

template <class T>
class item_container
{
public:

	typedef multi_index_container<item<T>,
				indexed_by<
				sequenced<>,
				hashed_unique<identity<item<T> > > > > indexed_container;

	typedef typename indexed_container::template nth_index<0>::type& sequenced_index;
	typedef typename indexed_container::template nth_index<0>::type::iterator sequenced_iterator;
	typedef typename indexed_container::template nth_index<1>::type& hashed_index;
	typedef typename indexed_container::template nth_index<1>::type::iterator hashed_iterator;

	class item_iterator
	{
	public:
		T get_next_item()
		{
			if(reversed)
			{
				if(iter == end)
				{
				  return NULL;
				}
				iter--;
				item<T> itm = *iter;
				return itm.get();
			}
			if(iter == end) return NULL;
			item<T> itm = *iter;
			iter++;
			return itm.get();
		}
		item_iterator(sequenced_index ind, bool reverse = false)
		{
			reversed = reverse;
			if(reverse)
			{
				iter = ind.end();
				end = ind.begin();
			}
			else
			{
				iter = ind.begin();
				end = ind.end();
			}
		}
	private:
		sequenced_iterator iter;
		sequenced_iterator end;
		bool reversed;
	};

	item_container()
	{
		memset(&mutex,0,sizeof(mutex));
	}
    bool insert(T it,const char *id,bool replace = false)
      {
      if(id == NULL) return false;
      return insert(it,std::string(id),replace);
      }
    bool insert(T it,std::string id,bool replace = false)
	{
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
      if((id == NULL)||(location_id == NULL)) return false;
      return insert_after(std::string(location_id),it,std::string(id));
      }
    bool insert_after(std::string location_id,T it,std::string id)
	{
		sequenced_index ind = container.get<0>();
		sequenced_iterator iter = ind.begin();
		while(iter != ind.end())
		{
			item<T> itm = *iter;
			if(itm.idString() == location_id)
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
      if(id == NULL) return false;
      return insert_at(index,it,std::string(id));
      }
    bool insert_at(int index,T it,std::string id)
    {
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
      if(id == NULL) return false;
      return insert_first(it,std::string(id));
      }
    bool insert_first(T it,std::string id)
    {
        sequenced_index ind = container.get<0>();
        sequenced_iterator iter = ind.begin();
        std::pair<sequenced_iterator,bool> ret;
        ret = container.get<0>().insert(iter,item<T>(id,it));
        return ret.second;
    }
    bool insert_before(const char *location_id,T it,const char *id)
      {
      if((id == NULL)||(location_id == NULL)) return false;
      return insert_before(std::string(location_id),it,std::string(id));
      }
	bool insert_before(std::string location_id,T it,std::string id)
	{
		sequenced_index ind = container.get<0>();
		sequenced_iterator iter = ind.begin();
		while(iter != ind.end())
		{
			item<T> itm = *iter;
			if(itm.idString() == location_id)
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
	  if(id == NULL) return false;
	  return remove(std::string(id));
	}
	bool remove(std::string id)
	{
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
	  if(id == NULL) return empty_val();
		return find(std::string(id));
	}
	T find(std::string id)
	{
		hashed_index hi = container.get<1>();
		hashed_iterator it = hi.find(id);
		if(it == hi.end())
		{
			return empty_val();
		}
		item<T> ret = *it;
		return ret.get();
	}
	T pop(void)
	{
		sequenced_index ind = container.get<0>();
		sequenced_iterator it = ind.begin();
		if(it == ind.end()) return empty_val();
		item<T> itm = *it;
		ind.erase(it);
		return itm.get();
	}
	T pop_back(void)
	{
		sequenced_index ind = container.get<0>();
		sequenced_iterator it = ind.end();
		if(ind.size() == 0) return empty_val();
		it--;
		item<T> itm = *it;
		ind.erase(it);
		return itm.get();
	}

    bool swap(const char *id1,const char *id2)
      {
      if((id1 == NULL)||(id2 == NULL)) return false;
      return swap(std::string(id1),std::string(id2));
      }
    bool swap(std::string id1,std::string id2)
	{
		sequenced_index ind = container.get<0>();
		sequenced_iterator it1 = ind.begin();
		while(it1 != ind.end())
		{
			item<T> itm = *it1;
			if(itm.idString() == id1)
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
			item<T> itm = *it2;
			if(itm.idString() == id2)
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
		return new item_iterator(container.get<0>(),reverse);
	}
	void clear()
	{
		container.get<0>().clear();
	}
	size_t count()
	  {
	  return container.size();
	  }
	void lock(void)
	{
		pthread_mutex_lock(&mutex);
	}
	void unlock(void)
	{
		pthread_mutex_unlock(&mutex);
	}
	int trylock(void)
	{
	    return pthread_mutex_trylock(&mutex);
	}
private:
	T empty_val(void)
	{
		return NULL;
	}
	indexed_container container;
	pthread_mutex_t mutex;
};

template <class T>
inline size_t hash_value(const item<T>& rhs)
{
    return (boost::hash_value(rhs.idString()));
}

} //End of namespace scope.

#endif
