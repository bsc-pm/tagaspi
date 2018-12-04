/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_HPP
#define WAITING_RANGE_HPP

#include <GASPI.h>
#include <TAGASPI.h>

#include <cassert>


class WaitingRange {
private:
	typedef gaspi_segment_id_t segment_id_t;
	typedef gaspi_notification_id_t notification_id_t;
	typedef gaspi_number_t number_t;
	typedef gaspi_notification_t notification_t;
	
public:
	segment_id_t _segment;
	notification_id_t _firstId;
	number_t _numIds;
	notification_t *_notifiedValues;
	number_t _remaining;
	void *_eventCounter;
	
	WaitingRange(segment_id_t segment, notification_id_t firstId, number_t numIds, notification_t *notifiedValues, void *eventCounter) :
		_segment(segment),
		_firstId(firstId),
		_numIds(numIds),
		_notifiedValues(notifiedValues),
		_remaining(numIds),
		_eventCounter(eventCounter)
	{}
	
	bool checkNotifications()
	{
		gaspi_return_t eret;
		gaspi_notification_id_t notifiedId;
		gaspi_notification_t notifiedValue;
		
		eret = gaspi_notify_waitsome(_segment, _firstId, _numIds, &notifiedId, GASPI_TEST);
		assert(eret == GASPI_SUCCESS || eret == GASPI_TIMEOUT);
		
		bool completed = false;
		if (eret == GASPI_SUCCESS) {
			eret = gaspi_notify_reset(_segment, notifiedId, &notifiedValue);
			assert(eret == GASPI_SUCCESS);
			
			if (notifiedValue != 0) {
				if (_notifiedValues != GASPI_NOTIFICATION_IGNORE) {
					_notifiedValues[notifiedId - _firstId] = notifiedValue;
				}
				
				completed = (--_remaining == 0);
			}
		}
		return completed;
	}
};

#endif /* WAITING_RANGE_HPP */
