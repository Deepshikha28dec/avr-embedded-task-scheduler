#include <stddef.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "ses_timer.h"

/*
 * Timer0 configuration:
 *
 * CPU frequency = 16 MHz
 * Prescaler     = 64
 *
 * Timer frequency:
 * 16,000,000 / 64 = 250,000 Hz
 *
 * For a 1 ms period:
 * 250,000 / 1000 = 250 timer counts
 *
 * OCR0A therefore contains 250 - 1 = 249.
 */
#define TIMER0_COMPARE_VALUE 249U

static volatile pTimerCallback timer0Callback = NULL;


void timer0_setCallback(pTimerCallback callback)
{
    timer0Callback = callback;
}


void timer0_start(void)
{
    /*
     * Stop the timer while configuring it.
     */
    TCCR0A = 0U;
    TCCR0B = 0U;

    /*
     * Clear the counter.
     */
    TCNT0 = 0U;

    /*
     * CTC mode:
     * Timer resets when TCNT0 reaches OCR0A.
     */
    TCCR0A = (1U << WGM01);

    /*
     * Compare value for a 1 ms period.
     */
    OCR0A = TIMER0_COMPARE_VALUE;

    /*
     * Clear any pending compare-match interrupt.
     */
    TIFR0 = (1U << OCF0A);

    /*
     * Enable Timer0 Compare Match A interrupt.
     */
    TIMSK0 |= (1U << OCIE0A);

    /*
     * Start Timer0 with prescaler = 64.
     */
    TCCR0B = (1U << CS01) | (1U << CS00);
}


void timer0_stop(void)
{
    /*
     * Disable Timer0 Compare Match A interrupt.
     */
    TIMSK0 &= ~(1U << OCIE0A);

    /*
     * Stop timer by removing the clock source.
     */
    TCCR0B &= ~((1U << CS02) |
                (1U << CS01) |
                (1U << CS00));

    /*
     * Clear pending compare-match flag.
     */
    TIFR0 = (1U << OCF0A);
}


ISR(TIMER0_COMPA_vect)
{
    if (timer0Callback != NULL)
    {
        timer0Callback();
    }
}