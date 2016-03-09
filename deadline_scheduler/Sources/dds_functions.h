#ifndef DDS_FUNCTIONS_H
#define DDS_FUNCTIONS_H

#include "dds_structures.h"


_task_id dd_tcreate(uint32_t template_index, uint32_t deadline) {



}

uint32_t dd_delete(uint32_t tid) {

}

uint32_t dd_return_active_list(task_list_entry_t** list) {
	*list = active_tasks;
}

uint32_t dd_return_overdue_list(task_list_entry_t** list) {
	*list = overdue_tasks;
}

#endif
