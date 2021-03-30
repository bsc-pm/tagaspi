/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_LIST_HPP
#define WAITING_RANGE_LIST_HPP

#include "WaitingRange.hpp"

#include <boost/intrusive/list.hpp>

#include <deque>
#include <vector>


class WaitingRangeList {
private:
	typedef boost::intrusive::member_hook<WaitingRange, typename WaitingRange::links_t, &WaitingRange::_listLinks> hook_option_t;
	typedef boost::intrusive::list<WaitingRange, hook_option_t> list_t;

	list_t _list;

public:
	inline WaitingRangeList() :
		_list()
	{
	}

	inline ~WaitingRangeList()
	{
		assert(_list.empty());
	}

	inline void add(WaitingRange *range)
	{
		_list.push_back(*range);
	}

	inline void checkNotifications(std::vector<WaitingRange*> &completeRanges)
	{
		auto it = _list.begin();
		while (it != _list.end()) {
			WaitingRange &wr = *it;

			if (wr.checkNotifications()) {
				completeRanges.push_back(&wr);
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
