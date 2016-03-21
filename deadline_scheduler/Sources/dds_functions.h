#ifndef DDS_FUNCTIONS_H
#define DDS_FUNCTIONS_H

#include "dds_structures.h"

#define CREATE_QUEUE 11
#define DELETE_QUEUE 12


MUTEX_ATTR_STRUCT _create_m_attr;
MUTEX_STRUCT _create_m;
_queue_id delete_qid;

void dd_init() {
	_mutatr_init(&_write_task_m_attr);
	_mutex_init(&_write_task_m, &_write_task_m_attr);


}

_task_id dd_tcreate(uint32_t template_index, uint32_t deadline, uint32_t runtime, task_type_e task_type, _task_id gid) {

	_mutex_lock(&_create_m);
	 DDS_TASK_MSG_PTR dds_msg_ptr;
	 DDS_RESP_MSG_PTR resp_msg_ptr;
	  bool result;

		_queue_id create_qid;
		create_qid = _msgq_open(CREATE_QUEUE, 0);

		if (create_qid == 0) {
			printf(" dd_create: Could not open the create queue (%d)\n", _task_get_error());
			_task_block();
		}

	  dds_msg_ptr = (DDS_TASK_MSG_PTR)_msg_alloc(dds_message_pool);

	  if (dds_msg_ptr == NULL) {
		  printf("\n dd_create: Could not allocate a dds_msg_ptr\n");
		  _task_block();
	  }




	  // allocate and set a strcut for create_command_data_t
	  create_command_data_t* command_data = _mem_alloc_system(sizeof(create_command_data_t));
	  command_data->deadline = deadline;
	  command_data->template_index = template_index;
	  command_data->runtime = runtime;
	  command_data->task_type = task_type;

	  if (gid != 0 && task_type == TASK_TYPE_PERIODIC) {
		  command_data->gid = gid;
	  }

	  dds_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DDS_MSG_QUEUE);
	  dds_msg_ptr->HEADER.SOURCE_QID = create_qid;
	  dds_msg_ptr->HEADER.SIZE = sizeof(DDS_TASK_MSG);
	  dds_msg_ptr->data = command_data;
	  dds_msg_ptr->dds_command = DDS_CREATE;

	  result = _msgq_send(dds_msg_ptr);

	  if (result != TRUE) {
		  printf("\n dd_create: Could not send dds message\n");
		  _task_block();
	  }

	  resp_msg_ptr = _msgq_receive(create_qid, 0);

	  if (resp_msg_ptr == NULL) {
		  printf("\n dd_create: Receiving resp message failed\n");
		  _task_block();
	  }

	  _task_id to_return = resp_msg_ptr->success;
	  _msg_free(resp_msg_ptr);
	  _msgq_close(create_qid);
	  _mutex_unlock(&_create_m);
	  return to_return;



}

uint32_t dd_delete(uint32_t tid) {

	 DDS_TASK_MSG_PTR dds_msg_ptr;
	 DDS_RESP_MSG_PTR resp_msg_ptr;
	  bool result;

		delete_qid = _msgq_open(DELETE_QUEUE, 0);

		if (delete_qid == MSGQ_INVALID_QUEUE_NUMBER) {
			printf("dd_delete: Could not open the delete queue\n");
			_task_block();
		}

	  dds_msg_ptr = (DDS_TASK_MSG_PTR)_msg_alloc(dds_message_pool);

	  if (dds_msg_ptr == NULL) {
		  printf("\n dd_delete: Could not allocate a dds_msg_ptr\n");
		  _task_block();
	  }

	  // allocate and set a strcut for create_command_data_t
	  delete_command_data_t* command_data = _mem_alloc_system(sizeof(delete_command_data_t));
	  command_data->tid = tid;
	  //command_data->template_index = 42;

	  dds_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DDS_MSG_QUEUE);
	  dds_msg_ptr->HEADER.SOURCE_QID = delete_qid;
	  dds_msg_ptr->HEADER.SIZE = sizeof(DDS_TASK_MSG);
	  dds_msg_ptr->data = command_data;
	  dds_msg_ptr->dds_command = DDS_DELETE;

	  result = _msgq_send(dds_msg_ptr);

	  if (result != TRUE) {
		  printf("\n dd_delete: Could not send dds message\n");
		  _task_block();
	  }

	  resp_msg_ptr = _msgq_receive(delete_qid, 0);

	  if (resp_msg_ptr == NULL) {
		  printf("\n dd_delete: Receiving resp message failed\n");
		  _task_block();
	  }

	  _task_id to_return = resp_msg_ptr->success;
	  _msgq_close(delete_qid);
	  return to_return;
}

uint32_t dd_return_active_list(task_list_entry_t** list) {
	*list = active_tasks;
}

uint32_t dd_return_overdue_list(task_list_entry_t** list) {
	*list = overdue_tasks;
}

#endif
