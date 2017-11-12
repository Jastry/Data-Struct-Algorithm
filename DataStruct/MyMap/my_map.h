#ifndef __MYMAP_H__
#define __MYMAP_H__

#include "rb_tree.h"

template <class K, class V>
struct MapKeyOfValue{
	K operator()(const V& kv){
		return kv.first;
	}
};

template < class Key, class Value, class KeyOfValue = MapKeyOfValue<Key,std::pair<Key,Value> > >
class my_map{
public:	
	typedef rb_tree<Key, std::pair<Key, Value>, KeyOfValue > rb_type;
	typedef typename rb_type::Iterator Iterator;

public:
	my_map(){}
	
	~my_map(){}
	
	std::pair<Iterator, bool> insert(const std::pair<Key, Value>& kv)
	{
		//return std::pair<Iterator, bool>(Iterator(kv), _t.insert(kv));
		return _t.insert(kv);
	}
	
	Value& operator[](const Key& key)
	{
		std::pair<Iterator, bool> ret = insert(std::make_pair(key,Value()));
		return ret.first->second;
	}
	
	Iterator Begin()
	{
		return _t.Begin();
	}

	Iterator End()
	{
		return _t.End();
	}
	
	Iterator rBegin() {
		return _t.rBegin();
	}
	
	Iterator rEnd(){
		return _t.rEnd();
	}
protected:
	rb_type _t;
};

#endif
