#include <stddef.h>
#include <util/atomic.h>

#include "ses_scheduler.h"
#include "ses_timer.h"

/*
 * Head of the scheduler's singly linked task list.
 *
 * Task descriptors are allocated by the application rather than
 * dynamically inside the scheduler, which keeps memory usage
 * deterministic for an embedded system.
 */
static task_descriptor_t *taskList = NULL;


/**
 * Timer callback executed every 1 ms.
 *
 * The callback performs only lightweight scheduler bookkeeping.
 * Actual task functions are executed later from scheduler_run()
 * in normal program context.
 */
static void scheduler_update(void)
{
    task_descriptor_t *current = taskList;

    while (current != NULL)
    {
        if (current->expire > 0U)
        {
            current->expire--;
        }

        if (current->expire == 0U)
        {
            current->execute = 1U;

            /*
             * Periodic tasks are re-armed immediately.
             * One-shot tasks remain at zero and are removed
             * when scheduler_run() dispatches them.
             */
            if (current->period > 0U)
            {
                current->expire = current->period;
            }
        }

        current = current->next;
    }
}


void scheduler_init(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        taskList = NULL;
    }

    timer0_setCallback(scheduler_update);
    timer0_start();
}


bool scheduler_add(task_descriptor_t *td)
{
    if ((td == NULL) || (td->task == NULL))
    {
        return false;
    }

    bool added = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        task_descriptor_t *current = taskList;
        task_descriptor_t *last = NULL;

        /*
         * Prevent the same task descriptor from being inserted twice.
         */
        while (current != NULL)
        {
            if (current == td)
            {
                return false;
            }

            last = current;
            current = current->next;
        }

        td->execute = (td->expire == 0U) ? 1U : 0U;
        td->next = NULL;

        if (taskList == NULL)
        {
            taskList = td;
        }
        else
        {
            last->next = td;
        }

        added = true;
    }

    return added;
}


void scheduler_remove(const task_descriptor_t *td)
{
    if (td == NULL)
    {
        return;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        task_descriptor_t *current = taskList;
        task_descriptor_t *previous = NULL;

        while (current != NULL)
        {
            if (current == td)
            {
                if (previous == NULL)
                {
                    taskList = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                break;
            }

            previous = current;
            current = current->next;
        }
    }
}


void scheduler_run(void)
{
    task_descriptor_t *taskToRun = NULL;
    task_t functionToRun = NULL;
    void *parameter = NULL;

    /*
     * Only scheduler bookkeeping is protected.
     * The task function itself must NOT run with interrupts disabled.
     */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        task_descriptor_t *current = taskList;
        task_descriptor_t *previous = NULL;

        while (current != NULL)
        {
            if (current->execute != 0U)
            {
                current->execute = 0U;

                taskToRun = current;
                functionToRun = current->task;
                parameter = current->param;

                /*
                 * A one-shot task is removed before execution.
                 */
                if (current->period == 0U)
                {
                    if (previous == NULL)
                    {
                        taskList = current->next;
                    }
                    else
                    {
                        previous->next = current->next;
                    }
                }

                break;
            }

            previous = current;
            current = current->next;
        }
    }

    /*
     * Execute outside the atomic section.
     */
    if ((taskToRun != NULL) && (functionToRun != NULL))
    {
        functionToRun(parameter);
    }
}