/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_LIST_HPP
#define WAITING_RANGE_LIST_HPP

#include "WaitingRange.hpp"

#include <list>


class WaitingRangeList {
private:
	std::list<WaitingRange*> _list;
	
public:
	inline WaitingRangeList() :
		_list()
	{}
	
	inline ~WaitingRangeList()
	{
		assert(_list.empty());
	}
	
	inline void splice(std::list<WaitingRange*> &pendingRanges) {
		if (!pendingRanges.empty()) {
			_list.splice(_list.end(), pendingRanges);
		}
		assert(pendingRanges.empty());
	}
	
	inline void checkNotifications(std::list<WaitingRange*> &completeRanges)
	{
		auto it = _list.begin();
		while (it != _list.end()) {
			WaitingRange *wr = *it;
			assert(wr != nullptr);
			
			if (wr->checkNotifications()) {
				completeRanges.push_back(wr);
				it = _list.erase(it);
				continue;
			}
			++it;
		}
	}
	
	inline bool empty() const
	{
		return _list.empty();
	}
};

#endif // WAITING_RANGE_LIST_HPP
