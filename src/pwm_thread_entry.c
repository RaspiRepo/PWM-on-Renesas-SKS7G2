#include "pwm_thread.h"

/* PWM Control entry function */
void pwm_thread_entry(void)
{
    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, 5, TIMER_PWM_UNIT_PERCENT, 1);
    g_timer0.p_api->start(g_timer0.p_ctrl);
    uint8_t dutycycle = 2;
    while (1) {
        tx_thread_sleep (10);
        dutycycle += 2;
        g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, (dutycycle %100), TIMER_PWM_UNIT_PERCENT, 1);
    }
}
