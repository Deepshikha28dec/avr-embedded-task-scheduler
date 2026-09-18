#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include "ses_scheduler.h"

/*
 * The SES board's red LED is connected to PF5
 * and is active-low.
 */
#define LED_DDR     DDRF
#define LED_PORT    PORTF
#define LED_PIN     PF5


static task_descriptor_t blinkTask;
static task_descriptor_t stopTask;


/**
 * Toggles the red LED.
 */
static void blink_led(void *param)
{
    (void)param;

    LED_PORT ^= (1U << LED_PIN);
}


/**
 * Stops the periodic LED task.
 *
 * Demonstrates a one-shot task interacting with
 * another scheduled task.
 */
static void stop_blinking(void *param)
{
    task_descriptor_t *task =
        (task_descriptor_t *)param;

    scheduler_remove(task);

    /*
     * LED is active-low, therefore HIGH = off.
     */
    LED_PORT |= (1U << LED_PIN);
}


int main(void)
{
    /*
     * Configure LED pin as output and start with LED off.
     */
    LED_DDR |= (1U << LED_PIN);
    LED_PORT |= (1U << LED_PIN);

    scheduler_init();

    /*
     * Periodic task:
     * Toggle LED every 1000 ms.
     *
     * A complete ON/OFF cycle therefore takes 2 seconds,
     * corresponding to a blink frequency of 0.5 Hz.
     */
    blinkTask.task = blink_led;
    blinkTask.param = NULL;
    blinkTask.expire = 1000U;
    blinkTask.period = 1000U;
    blinkTask.execute = 0U;
    blinkTask.next = NULL;

    /*
     * One-shot task:
     * Stop LED blinking after 10 seconds.
     */
    stopTask.task = stop_blinking;
    stopTask.param = &blinkTask;
    stopTask.expire = 10000U;
    stopTask.period = 0U;
    stopTask.execute = 0U;
    stopTask.next = NULL;

    scheduler_add(&blinkTask);
    scheduler_add(&stopTask);

    /*
     * Timer interrupts can now drive the scheduler.
     */
    sei();

    while (1)
    {
        scheduler_run();
    }

    return 0;
}