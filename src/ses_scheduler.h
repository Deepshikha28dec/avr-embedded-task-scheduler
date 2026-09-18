#ifndef SES_SCHEDULER_H_
#define SES_SCHEDULER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Function pointer type for scheduled tasks.
 *
 * @param param Optional parameter passed to the task.
 */
typedef void (*task_t)(void *param);

/**
 * Descriptor for a scheduled task.
 *
 * Memory for the descriptor is supplied by the application.
 * Dynamic allocation is therefore not required.
 */
typedef struct task_descriptor_s
{
    task_t task;                       /* Function to execute */
    void *param;                       /* Optional task parameter */
    uint16_t expire;                   /* Time until execution in ms */
    uint16_t period;                   /* Period in ms; 0 = one-shot */
    uint8_t execute : 1;               /* Internal execution flag */
    uint8_t reserved : 7;
    struct task_descriptor_s *next;    /* Next task in linked list */
} task_descriptor_t;

/**
 * Initializes the scheduler and configures AVR Timer0
 * as a 1 ms system time base.
 */
void scheduler_init(void);

/**
 * Executes one ready task, if available.
 *
 * The task itself is executed outside the critical section so
 * interrupts remain enabled during normal task execution.
 */
void scheduler_run(void);

/**
 * Adds a task to the scheduler.
 *
 * The descriptor must remain valid until the task is removed
 * or, for a one-shot task, until it has been executed.
 *
 * @param td Pointer to the task descriptor.
 *
 * @return true if successfully added.
 * @return false if td is NULL, td->task is NULL,
 *         or the descriptor is already scheduled.
 */
bool scheduler_add(task_descriptor_t *td);

/**
 * Removes a task from the scheduler.
 *
 * @param td Pointer to the task descriptor to remove.
 */
void scheduler_remove(const task_descriptor_t *td);

#endif /* SES_SCHEDULER_H_ */