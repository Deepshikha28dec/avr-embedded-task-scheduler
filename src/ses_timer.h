#ifndef SES_TIMER_H_
#define SES_TIMER_H_

/**
 * Function pointer type for Timer0 callbacks.
 */
typedef void (*pTimerCallback)(void);

/**
 * Registers a callback function executed on every
 * Timer0 compare-match interrupt.
 *
 * Passing NULL disables callback execution.
 *
 * @param callback Pointer to callback function.
 */
void timer0_setCallback(pTimerCallback callback);

/**
 * Configures and starts AVR Timer0 in CTC mode
 * with a 1 ms interrupt period.
 *
 * Assumes a 16 MHz CPU clock.
 */
void timer0_start(void);

/**
 * Stops Timer0 and disables its compare-match interrupt.
 */
void timer0_stop(void);

#endif /* SES_TIMER_H_ */