/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_HPP
#define WAITING_RANGE_HPP

#include <GASPI.h>
#include <TAGASPI.h>

#include "Polling.hpp"

#include <cassert>
#include <cstdio>

class WaitingRange {
private:
	typedef gaspi_segment_id_t segment_id_t;
	typedef gaspi_notification_id_t notification_id_t;
	typedef gaspi_number_t number_t;
	typedef gaspi_notification_t notification_t;


	segment_id_t _segment;

	notification_id_t _firstID;

	number_t _numIDs;

	notification_t *_valueBuffer;

	number_t _remaining;

	void *_eventCounter;

public:
	inline WaitingRange(segment_id_t segment,
			notification_id_t firstNotificationID,
			number_t numNotifications,
			notification_t *valueBuffer,
			void *eventCounter) :
		_segment(segment),
		_firstID(firstNotificationID),
		_numIDs(numNotifications),
		_valueBuffer(valueBuffer),
		_remaining(numNotifications),
		_eventCounter(eventCounter)
	{}

	inline ~WaitingRange()
	{
		assert(_remaining == 0);
	}

	inline void *getEventCounter() const
	{
		return _eventCounter;
	}

	inline bool checkNotifications()
	{
		gaspi_return_t eret;
		gaspi_notification_id_t notifiedID;
		gaspi_notification_t notifiedValue;

		eret = gaspi_notify_waitsome(_segment, _firstID, _numIDs, &notifiedID, GASPI_TEST);
		if (eret != GASPI_SUCCESS && eret != GASPI_TIMEOUT) {
			fprintf(stderr, "Error: Unexpected return error from gaspi_notify_waitsome\n");
			abort();
		}

		bool completed = false;
		if (eret == GASPI_SUCCESS) {
			eret = gaspi_notify_reset(_segment, notifiedID, &notifiedValue);
			if (eret != GASPI_SUCCESS) {
				fprintf(stderr, "Error: Unexpected return error from gaspi_notify_reset\n");
				abort();
			}

			if (notifiedValue != 0) {
				if (_valueBuffer != GASPI_NOTIFICATION_IGNORE) {
					_valueBuffer[notifiedID - _firstID] = notifiedValue;
				}

				completed = (--_remaining == 0);
			}
		}
		return completed;
	}
};

#endif // WAITING_RANGE_HPP
