#ifndef __SerialAPI_H
#define __SerialAPI_H

#include <mutex.h>

#define PUT_BUFFER_SIZE 128

_task_id _write_task = 0;
MUTEX_ATTR_STRUCT _write_task_m_attr;
MUTEX_STRUCT _write_task_m;

void SerialAPI_init() {
	_mutatr_init(&_write_task_m_attr);
	_mutex_init(&_write_task_m, &_write_task_m_attr);


}

_queue_id OpenW() {
	_mutex_lock(&_write_task_m);
	if (_write_task != 0) {
		return 0;
	} else {
		printf("Grant W to %d\n", _task_get_id());
		_write_task = _task_get_id();
	}
	_mutex_unlock(&_write_task_m);
	return TX_MSG_QUEUE;
}


bool _putline(_queue_id qid, char* str) {
	if (_write_task == _task_get_id()) {


		char buffer[PUT_BUFFER_SIZE];

		if (strlen(str)+2 > PUT_BUFFER_SIZE) {
			printf("_putline: buffer not big enough\n");
			_task_block();
		}

		memset(buffer, 0, PUT_BUFFER_SIZE);
		strncpy(buffer, str, strlen(str));
		buffer[strlen(str)] = '\n';
		buffer[strlen(str)+1] = '\r';
		buffer[strlen(str)+2] = '\0';
		printf("buffer is: %s\n", buffer);
		queue_str(buffer, strlen(str)+2, qid);
	}
}

bool Close() {
	_task_id my_id = _task_get_id();

	// clear W
	_mutex_lock(&_write_task_m);
	if (_write_task == my_id) {
		printf("Revoke W to %d\n", _task_get_id());
		_write_task = 0;
	}
	_mutex_unlock(&_write_task_m);

	//TODO clear R
}


#endif
