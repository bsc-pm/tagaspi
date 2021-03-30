/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_HPP
#define WAITING_RANGE_HPP

#include <GASPI.h>
#include <TAGASPI.h>

#include <boost/intrusive/list.hpp>

#include "TaskingModel.hpp"

#include <cassert>
#include <cstdio>

class WaitingRange {
protected:
	gaspi_segment_id_t _segment;
	gaspi_notification_id_t _firstId;
	gaspi_number_t _numIds;
	gaspi_notification_t *_notifiedValues;
	gaspi_notification_t _notifiedValue;

	gaspi_number_t _remaining;

	void *_eventCounter;

public:
	typedef boost::intrusive::link_mode<boost::intrusive::normal_link> link_mode_t;
	typedef boost::intrusive::list_member_hook<link_mode_t> links_t;

	links_t _listLinks;

	inline WaitingRange(
		gaspi_segment_id_t segment,
		gaspi_notification_id_t firstNotificationId,
		gaspi_number_t numNotifications,
		gaspi_notification_t *notifiedValues,
		gaspi_number_t remainingNotifications,
		void *eventCounter
	) :
		_segment(segment),
		_firstId(firstNotificationId),
		_numIds(numNotifications),
		_notifiedValues(notifiedValues),
		_remaining(remainingNotifications),
		_eventCounter(eventCounter)
	{
	}

	virtual inline ~WaitingRange()
	{
		assert(_remaining == 0);
	}

	inline void *getEventCounter() const
	{
		return _eventCounter;
	}

	inline bool checkNotifications()
	{
		if (_numIds == 1) {
			_remaining = checkNotification(_segment, _firstId, &_notifiedValue);
			if (_notifiedValues != GASPI_NOTIFICATION_IGNORE)
				*_notifiedValues = _notifiedValue;
		} else {
			_remaining = WaitingRange::checkNotifications(
				_segment, _firstId, _numIds, _notifiedValues,
				_remaining);
		}
		return (_remaining == 0);
	}

	static inline gaspi_number_t checkNotification(
		gaspi_segment_id_t segment,
		gaspi_notification_id_t notificationId,
		gaspi_notification_t *notifiedValue
	) {
		gaspi_notification_id_t id;
		gaspi_notification_t value;
		gaspi_return_t eret;

		eret = gaspi_notify_waitsome(segment, notificationId, 1, &id, GASPI_TEST);
		if (eret == GASPI_SUCCESS) {
			assert(id == notificationId);

			eret = gaspi_notify_reset(segment, notificationId, &value);
			if (eret != GASPI_SUCCESS) {
				fprintf(stderr, "Error: Return code %d from gaspi_notify_reset\n", eret);
				abort();
			}

			if (value != 0) {
				if (notifiedValue != GASPI_NOTIFICATION_IGNORE)
					*notifiedValue = value;

				return 0;
			}
		} else if (eret != GASPI_TIMEOUT) {
			fprintf(stderr, "Error: Return code %d from gaspi_notify_waitsome\n", eret);
			abort();
		}
		return 1;
	}

	static inline gaspi_number_t checkNotifications(
		gaspi_segment_id_t segment,
		gaspi_notification_id_t firstId,
		gaspi_number_t numIds,
		gaspi_notification_t notifiedValues[],
		gaspi_number_t remaining
	) {
		assert(remaining > 0);
		assert(remaining <= numIds);

		bool cont;
		gaspi_return_t eret;
		gaspi_notification_id_t notifiedId;
		gaspi_notification_t notifiedValue;

		do {
			cont = false;

			eret = gaspi_notify_waitsome(segment, firstId, numIds, &notifiedId, GASPI_TEST);
			if (eret == GASPI_SUCCESS) {
				assert(notifiedId >= firstId && notifiedId < firstId + numIds);

				eret = gaspi_notify_reset(segment, notifiedId, &notifiedValue);
				if (eret != GASPI_SUCCESS) {
					fprintf(stderr, "Error: Return code %d from gaspi_notify_reset\n", eret);
					abort();
				}

				if (notifiedValue != 0) {
					if (notifiedValues != GASPI_NOTIFICATION_IGNORE)
						notifiedValues[notifiedId - firstId] = notifiedValue;

					cont = true;
					--remaining;
				}
			} else if (eret != GASPI_TIMEOUT) {
				fprintf(stderr, "Error: Return code %d from gaspi_notify_waitsome\n", eret);
				abort();
			}
		} while (cont && remaining > 0);

		return remaining;
	}

	virtual inline void complete()
	{
		TaskingModel::decreaseTaskEventCounter(_eventCounter, 1);
	}

	virtual inline bool isAckWaitingRange() const
	{
		return false;
	}
};

class AckWaitingRange : public WaitingRange {
public:
	typedef void (*callback_t)(gaspi_notification_t notifiedValue, void *args);

	struct AckActionInfo {
		gaspi_segment_id_t segment_id_local;
		gaspi_offset_t offset_local;
		gaspi_rank_t rank;
		gaspi_segment_id_t segment_id_remote;
		gaspi_offset_t offset_remote;
		gaspi_size_t size;
		gaspi_notification_id_t notification_id;
		gaspi_notification_t notification_value;
		gaspi_queue_id_t queue;
	};

private:
	callback_t _ackActionCallback;

	void *_ackActionArgs;

	AckActionInfo _ackActionInfo;

	bool _unregister;

public:
	static __thread AckWaitingRange *_currentWaitingRange;

	inline AckWaitingRange(
		gaspi_segment_id_t segment,
		gaspi_notification_id_t firstNotificationId,
		gaspi_number_t numNotifications,
		gaspi_notification_t *notifiedValues,
		gaspi_number_t remainingNotifications,
		void *eventCounter
	) :
		WaitingRange(segment, firstNotificationId,
			numNotifications, notifiedValues,
			remainingNotifications, eventCounter),
		_ackActionCallback(nullptr),
		_ackActionArgs(nullptr),
		_ackActionInfo(),
		_unregister(false)
	{
	}

	inline void setAckActionCallback(callback_t callback)
	{
		_ackActionCallback = callback;
	}

	inline void setAckActionArgs(void *args)
	{
		_ackActionArgs = args;
	}

	inline void *getAckActionArgs() const
	{
		return _ackActionArgs;
	}

	inline AckActionInfo &getAckActionInfo()
	{
		return _ackActionInfo;
	}

	inline const AckActionInfo &getAckActionInfo() const
	{
		return _ackActionInfo;
	}

	inline void setUnregister()
	{
		_unregister = true;
	}

	inline void complete() override
	{
		assert(_ackActionCallback != nullptr);
		assert(_currentWaitingRange == nullptr);

		_currentWaitingRange = this;
		_ackActionCallback(_notifiedValue, _ackActionArgs);
		_currentWaitingRange = nullptr;

		if (_unregister)
			WaitingRange::complete();
	}

	inline bool isAckWaitingRange() const override
	{
		return true;
	}
};


#endif // WAITING_RANGE_HPP
