#ifndef __SerialAPI_H
#define __SerialAPI_H

#include <mutex.h>

#define PUT_BUFFER_SIZE 256

_task_id _write_task = 0;
MUTEX_ATTR_STRUCT _write_task_m_attr;
MUTEX_STRUCT _write_task_m;

#define MAX_R_TASKS 16
_task_id _read_tasks[MAX_R_TASKS];
_queue_id _read_tasks_qid[MAX_R_TASKS];
char* _getline_addr[MAX_R_TASKS];
unsigned int _read_tasks_idx = 0;
MUTEX_ATTR_STRUCT _read_task_m_attr;
MUTEX_STRUCT _read_task_m;

void SerialAPI_init() {
	_mutatr_init(&_write_task_m_attr);
	_mutex_init(&_write_task_m, &_write_task_m_attr);

	_mutatr_init(&_read_task_m_attr);
	_mutex_init(&_read_task_m, &_read_task_m_attr);

	memset(_getline_addr, NULL, sizeof(char*) * MAX_R_TASKS);
}

_queue_id OpenW() {
	_mutex_lock(&_write_task_m);
	if (_write_task != 0) {
		return 0;
	} else {
		//printf("Grant W to %d\n", _task_get_id());
		_write_task = _task_get_id();
	}
	_mutex_unlock(&_write_task_m);
	return TX_MSG_QUEUE;
}

bool OpenR(uint16_t stream_no) {

	_task_id my_id = _task_get_id();

	if (_read_tasks_idx == MAX_R_TASKS) {
		printf("Max read tasks! \n");
		_task_block();
	}

	_mutex_lock(&_read_task_m);
	int i;
	for (i = 0; i < _read_tasks_idx; i++) {

		if (_read_tasks[i] == my_id) {
			_mutex_unlock(&_read_task_m);
			return false;
		}

	}
	_read_tasks[_read_tasks_idx] = my_id;
	_read_tasks_qid[_read_tasks_idx] = stream_no;
	_read_tasks_idx++;
	_mutex_unlock(&_read_task_m);
	return true;


}

bool _getline(char * str) {
	_task_id my_id = _task_get_id();
	int i, idx;
	idx = -1;
	for (i = 0; i < _read_tasks_idx; i++) {
		if (_read_tasks[i] == my_id) {
			idx = i;
		}
	}
	if (idx < 0) return FALSE;

	_getline_addr[idx] = str;
	_task_block();
	_getline_addr[idx] = NULL;
	return true;

}

bool _putline(_queue_id qid, char* str) {
	if (_write_task == _task_get_id()) {

		char buffer[PUT_BUFFER_SIZE];

		if (strlen(str)+2 > PUT_BUFFER_SIZE) {
			printf("_putline: buffer not big enough\n");
			return FALSE;
		}

		memset(buffer, 0, PUT_BUFFER_SIZE);
		strncpy(buffer, str, strlen(str));
		buffer[strlen(str)] = '\n';
		buffer[strlen(str)+1] = '\r';
		buffer[strlen(str)+2] = '\0';
		//printf("buffer is: %s\n", buffer);
		queue_str(buffer, strlen(str)+2, qid);
		return TRUE;
	}
	return FALSE;
}

bool Close() {
	bool success = FALSE;
	_task_id my_id = _task_get_id();

	// clear W
	_mutex_lock(&_write_task_m);
	if (_write_task == my_id) {
		//printf("Revoke W to %d\n", _task_get_id());
		_write_task = 0;
		success = TRUE;
	}
	_mutex_unlock(&_write_task_m);

	// clear R
	_mutex_lock(&_read_task_m);
	int idx = -1;
	int i;
	for (i = 0; i < _read_tasks_idx; i++) {

		if (_read_tasks[i] == my_id) {
			idx = i;
			break;
		}

	}

	if (idx > -1) {
		_read_tasks[idx] = 0;
		_read_tasks_qid[idx] = 0;

		for (; idx < _read_tasks_idx-1; idx++) {
			// shift until last index empty
			_read_tasks[idx] = _read_tasks[idx+1];
			_read_tasks_qid[idx] = _read_tasks_qid[idx+1];
		}

		_read_tasks_idx--;
		success = TRUE;
	}
	_mutex_unlock(&_read_task_m);
	return success;

}


#endif
