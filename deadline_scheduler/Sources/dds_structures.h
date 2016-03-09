#ifndef DDS_STRUCTURES_H
#define DDS_STRUCTURES_H

typedef enum task_type {
	TASK_TYPE_PERIODIC,
	TASK_TYPE_APERIODIC
} task_type_e;

typedef struct task_list_data {

	uint32_t tid;
	uint32_t deadline;
	task_type_e task_type;
	uint32_t creation_time;

} task_list_data_t;


typedef struct task_list_entry {

	struct task_list_data_t* data;
	struct task_list_t* next;
//	struct task_list_t* prev;

} task_list_entry_t;

extern task_list_entry_t* active_tasks = NULL;
extern task_list_entry_t* overdue_tasks = NULL;

/* Functions to manipulate task lists */

void add_task_list_entry(task_list_entry_t** head, task_list_data_t* new_data) {

	task_list_entry_t* old_head = *head;
	task_list_entry_t* new_entry = _mem_alloc_system(sizeof(task_list_entry_t));
	new_entry->data = new_data;
	new_entry->next = old_head;
	*head = new_entry;

}

void task_list_summary_str(task_list_entry_t* head, char * buffer) {
	// assume the buffer is large enough
	uint16_t buffer_offset = 0;
	task_list_entry_t* pos = head;
	while (pos != NULL) {

		buffer_offset += sprintf(buffer+buffer_offset, "%p -> %p: (tid=%d; deadline=%d; task_type=%d; creation_time=%d)\n",
				pos,
				pos->next,
				((task_list_data_t*)pos->data)->tid,
				((task_list_data_t*)pos->data)->deadline,
				((task_list_data_t*)pos->data)->task_type,
				((task_list_data_t*)pos->data)->creation_time);

		pos = pos->next;
	}

}

void delete_task_list_entry(task_list_entry_t** head, uint32_t tid) {

	task_list_entry_t* pos = *head;
	task_list_entry_t* prev = NULL;
	task_list_entry_t* to_free = NULL;

	while (pos != NULL) {

		if (((task_list_data_t*)pos->data)->tid == tid) {

			if (prev == NULL) {
				// we want to remove the head
				*head = pos->next;
			} else {
				// we want to remove something other than the head
				prev->next = pos->next;
			}

			to_free = pos;
		}

		prev = pos;
		pos = pos->next;
	}

	if (to_free != NULL) {
		_mem_free(to_free->data);
		_mem_free(to_free);
	}
}



/* FOR TESTING
 *
 *
 *
 * task_list_data_t* test1 =_mem_alloc_system(sizeof(task_list_data_t));
test1->tid = 10;
test1->deadline = 5;
test1->creation_time = 0;
test1->task_type = TASK_TYPE_PERIODIC;
add_task_list_entry(&active_tasks, test1);

task_list_data_t* test2 =_mem_alloc_system(sizeof(task_list_data_t));
test2->tid = 14;
test2->deadline = 5;
test2->creation_time = 4;
test2->task_type = TASK_TYPE_APERIODIC;
add_task_list_entry(&active_tasks, test2);

task_list_data_t* test3 =_mem_alloc_system(sizeof(task_list_data_t));
test3->tid = 11;
test3->deadline = 5;
test3->creation_time = 4;
test3->task_type = TASK_TYPE_APERIODIC;
add_task_list_entry(&active_tasks, test3);

char test[256];
task_list_summary_str(active_tasks, &test);
printf("%s\n", test);

printf("\n\n\n");

delete_task_list_entry(&active_tasks, 10);

task_list_summary_str(active_tasks, &test);
printf("%s\n", test);

printf("\n\n\n");

delete_task_list_entry(&active_tasks, 11);

task_list_summary_str(active_tasks, &test);
printf("%s\n", test);

printf("\n\n\n");

delete_task_list_entry(&active_tasks, 14);

task_list_summary_str(active_tasks, &test);
printf("%s\n", test);

printf("\n\n\n");


task_list_data_t* test4 =_mem_alloc_system(sizeof(task_list_data_t));
test4->tid = 12;
test4->deadline = 5;
test4->creation_time = 4;
test4->task_type = TASK_TYPE_APERIODIC;
add_task_list_entry(&active_tasks, test4);
task_list_summary_str(active_tasks, &test);
printf("%s\n", test);

 * ***/
 */
#endif
