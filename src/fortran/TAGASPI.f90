!
!   This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
!
!   Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
!

!-----------------------------------------------------------------------
module TAGASPI
!-----------------------------------------------------------------------

    use, intrinsic :: ISO_C_BINDING
    use GASPI

    implicit none

    integer, parameter :: gaspi_queue_group_policy_t = c_long
    integer, parameter :: gaspi_queue_group_id_t = c_signed_char

    type(c_ptr), parameter :: GASPI_NOTIFICATION_IGNORE = C_NULL_PTR

    enum, bind(C) !:: gaspi_queue_group_policy_t
        ! Distribution of the queues using round-robin.
        ! Each call gets a different queue.
        enumerator :: GASPI_QUEUE_GROUP_POLICY_DEFAULT = 0
        ! Distribution of the queues across CPUs using
        ! round-robin. Each CPU will always get the same
        ! queue. If the number of CPUs exceeds the number of
        ! queues, the queues are distributed in a round-robin
        ! way. This policy avoids assigning the same queue to
        ! CPUs that are in different NUMA nodes.
        enumerator :: GASPI_QUEUE_GROUP_POLICY_CPU_RR = 1
    end enum

    interface ! tagaspi_proc_init
      function tagaspi_proc_init(timeout_ms) &
&         result( res ) bind(C, name="tagaspi_proc_init")
    import
    integer(gaspi_timeout_t), value :: timeout_ms
    integer(gaspi_return_t) :: res
      end function tagaspi_proc_init
    end interface

    interface ! tagaspi_proc_term
      function tagaspi_proc_term(timeout_ms) &
&         result( res ) bind(C, name="tagaspi_proc_term")
    import
    integer(gaspi_timeout_t), value :: timeout_ms
    integer(gaspi_return_t) :: res
      end function tagaspi_proc_term
    end interface

    interface ! tagaspi_write
      function tagaspi_write(segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote,size,queue) &
&         result( res ) bind(C, name="tagaspi_write")
    import
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_offset_t), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_offset_t), value :: offset_remote
    integer(gaspi_size_t), value :: size
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_write
    end interface

    interface ! tagaspi_read
      function tagaspi_read(segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote,size,queue) &
&         result( res ) bind(C, name="tagaspi_read")
    import
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_offset_t), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_offset_t), value :: offset_remote
    integer(gaspi_size_t), value :: size
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_read
    end interface

    interface ! tagaspi_write_list
      function tagaspi_write_list(num,segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote,size,queue) &
&         result( res ) bind(C, name="tagaspi_write_list")
    import
    integer(gaspi_number_t), value :: num
    type(c_ptr), value :: segment_id_local
    type(c_ptr), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    type(c_ptr), value :: segment_id_remote
    type(c_ptr), value :: offset_remote
    type(c_ptr), value :: size
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_write_list
    end interface

    interface ! tagaspi_read_list
      function tagaspi_read_list(num,segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote,size,queue) &
&         result( res ) bind(C, name="tagaspi_read_list")
    import
    integer(gaspi_number_t), value :: num
    type(c_ptr), value :: segment_id_local
    type(c_ptr), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    type(c_ptr), value :: segment_id_remote
    type(c_ptr), value :: offset_remote
    type(c_ptr), value :: size
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_read_list
    end interface

    interface ! tagaspi_notify
      function tagaspi_notify(segment_id_remote,rank,notification_id, &
&         notification_value,queue) &
&         result( res ) bind(C, name="tagaspi_notify")
    import
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_notification_id_t), value :: notification_id
    integer(gaspi_notification_t), value :: notification_value
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_notify
    end interface

    interface ! tagaspi_write_notify
      function tagaspi_write_notify(segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote, &
&         size,notification_id,notification_value,queue) &
&         result( res ) bind(C, name="tagaspi_write_notify")
    import
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_offset_t), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_offset_t), value :: offset_remote
    integer(gaspi_size_t), value :: size
    integer(gaspi_notification_id_t), value :: notification_id
    integer(gaspi_notification_t), value :: notification_value
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_write_notify
    end interface

    interface ! tagaspi_write_list_notify
      function tagaspi_write_list_notify(num,segment_id_local,offset_local, &
&         rank,segment_id_remote,offset_remote,size,segment_id_notification, &
&         notification_id,notification_value,queue) &
&         result( res ) bind(C, name="tagaspi_write_list_notify")
    import
    integer(gaspi_number_t), value :: num
    type(c_ptr), value :: segment_id_local
    type(c_ptr), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    type(c_ptr), value :: segment_id_remote
    type(c_ptr), value :: offset_remote
    type(c_ptr), value :: size
    integer(gaspi_segment_id_t), value :: segment_id_notification
    integer(gaspi_notification_id_t), value :: notification_id
    integer(gaspi_notification_t), value :: notification_value
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_write_list_notify
    end interface

    interface ! tagaspi_notify_async_wait
      function tagaspi_notify_async_wait(segment_id_local,notification_id, &
&         old_notification_value) &
&         result( res ) bind(C, name="tagaspi_notify_async_wait")
    import
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_notification_id_t), value :: notification_id
    type(c_ptr), value :: old_notification_value
    integer(gaspi_return_t) :: res
      end function tagaspi_notify_async_wait
    end interface

    interface ! tagaspi_notify_async_waitall
      function tagaspi_notify_async_waitall(segment_id_local,notification_begin, &
&         num,old_notification_values) &
&         result( res ) bind(C, name="tagaspi_notify_async_waitall")
    import
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_notification_id_t), value :: notification_begin
    integer(gaspi_number_t), value :: num
    type(c_ptr), value :: old_notification_values
    integer(gaspi_return_t) :: res
      end function tagaspi_notify_async_waitall
    end interface

    interface ! tagaspi_ack_write
      function tagaspi_ack_write(ack_segment_id, &
&         ack_notification_id,ack_notification_value, &
&         segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote, &
&         size,queue) &
&         result( res ) bind(C, name="tagaspi_ack_write")
    import
    integer(gaspi_segment_id_t), value :: ack_segment_id
    integer(gaspi_notification_id_t), value :: ack_notification_id
    type(c_ptr), value :: ack_notification_value
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_offset_t), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_offset_t), value :: offset_remote
    integer(gaspi_size_t), value :: size
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_ack_write
    end interface

    interface ! tagaspi_ack_notify
      function tagaspi_ack_notify(ack_segment_id, &
&         ack_notification_id,ack_notification_value, &
&         segment_id_remote,rank, &
&         notification_id,notification_value,queue) &
&         result( res ) bind(C, name="tagaspi_ack_notify")
    import
    integer(gaspi_segment_id_t), value :: ack_segment_id
    integer(gaspi_notification_id_t), value :: ack_notification_id
    type(c_ptr), value :: ack_notification_value
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_notification_id_t), value :: notification_id
    integer(gaspi_notification_t), value :: notification_value
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_ack_notify
    end interface

    interface ! tagaspi_ack_write_notify
      function tagaspi_ack_write_notify(ack_segment_id, &
&         ack_notification_id,ack_notification_value, &
&         segment_id_local,offset_local,rank, &
&         segment_id_remote,offset_remote, &
&         size,notification_id,notification_value,queue) &
&         result( res ) bind(C, name="tagaspi_ack_write_notify")
    import
    integer(gaspi_segment_id_t), value :: ack_segment_id
    integer(gaspi_notification_id_t), value :: ack_notification_id
    type(c_ptr), value :: ack_notification_value
    integer(gaspi_segment_id_t), value :: segment_id_local
    integer(gaspi_offset_t), value :: offset_local
    integer(gaspi_rank_t), value :: rank
    integer(gaspi_segment_id_t), value :: segment_id_remote
    integer(gaspi_offset_t), value :: offset_remote
    integer(gaspi_size_t), value :: size
    integer(gaspi_notification_id_t), value :: notification_id
    integer(gaspi_notification_t), value :: notification_value
    integer(gaspi_queue_id_t), value :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_ack_write_notify
    end interface

    interface ! tagaspi_queue_group_create
      function tagaspi_queue_group_create(queue_group,queue_begin,queue_num,policy) &
&         result( res ) bind(C, name="tagaspi_queue_group_create")
    import
    integer(gaspi_queue_group_id_t), value :: queue_group
    integer(gaspi_queue_id_t), value :: queue_begin
    integer(gaspi_number_t), value :: queue_num
    integer(gaspi_queue_group_policy_t), value :: policy
    integer(gaspi_return_t) :: res
      end function tagaspi_queue_group_create
    end interface

    interface ! tagaspi_queue_group_delete
      function tagaspi_queue_group_delete(queue_group) &
&         result( res ) bind(C, name="tagaspi_queue_group_delete")
    import
    integer(gaspi_queue_group_id_t), value :: queue_group
    integer(gaspi_return_t) :: res
      end function tagaspi_queue_group_delete
    end interface

    interface ! tagaspi_queue_group_get_queue
      function tagaspi_queue_group_get_queue(queue_group,queue) &
&         result( res ) bind(C, name="tagaspi_queue_group_get_queue")
    import
    integer(gaspi_queue_group_id_t), value :: queue_group
    integer(gaspi_queue_id_t) :: queue
    integer(gaspi_return_t) :: res
      end function tagaspi_queue_group_get_queue
    end interface

    interface ! tagaspi_queue_group_max
      function tagaspi_queue_group_max(queue_group_max) &
&         result( res ) bind(C, name="tagaspi_queue_group_max")
    import
    integer(gaspi_number_t) :: queue_group_max
    integer(gaspi_return_t) :: res
      end function tagaspi_queue_group_max
    end interface

end module TAGASPI
