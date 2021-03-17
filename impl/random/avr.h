//#include <Arduino.h>
#include <atmel_start.h>
#include <hal_init.h>
#include <hpl_pmc.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include "C:\Users\uic72887\Documents\Atmel Studio\7.0\AES Demo_1\AES Demo_1\hpl\tc\hpl_tc.h"
#include <hal_timer.h>

//static bool
struct timer_descriptor       TIMER_0;
extern int counterTc;
hydro_random_rbit(uint16_t x)
{
    uint8_t x8;

    x8 = ((uint8_t)(x >> 8)) ^ (uint8_t) x;
    x8 = (x8 >> 4) ^ (x8 & 0xf);
    x8 = (x8 >> 2) ^ (x8 & 0x3);
    x8 = (x8 >> 1) ^ x8;

    return (bool) (x8 & 1);
}

void TIMER_0_PORT_init(void)
{
}
/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */static struct timer_task TIMER_0_task1, TIMER_0_task2;

 static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
 {
	 if(counterTc < 0xFF)
	 {
		counterTc++; 
	 }
	 else
	 {
		counterTc = 0; 
	 }
 }

 static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
 {
 }

void TIMER_0_example(void)
{
	TIMER_0_task1.interval = 50;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 10;
	TIMER_0_task2.cb       = TIMER_0_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;
	counterTc = 0; 
	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}
static void TIMER_0_init(void)
{
	_pmc_enable_periph_clock(ID_TC0_CHANNEL0);
	TIMER_0_PORT_init();
	timer_init(&TIMER_0, TC0, _tc_get_timer());
}


static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    uint16_t         ebits = 0;
    uint16_t         tc;
    bool             a, b;
	
	TIMER_0_init();
    TIMER_0_example();
  /*  cli();
    MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = _BV(WDIE);
    sei();
*/

    hydro_hash_init(&st, ctx, NULL);

    while (ebits < 256) {
		
        delay_ms(2);
	    timer_stop(&TIMER_0);
        tc = counterTc;
        hydro_hash_update(&st, (const uint8_t *) &tc, sizeof tc);
        a = hydro_random_rbit(tc);
		timer_start(&TIMER_0);
        delay_ms(3);
		timer_stop(&TIMER_0);
        tc = counterTc;
        b  = hydro_random_rbit(tc);
        hydro_hash_update(&st, (const uint8_t *) &tc, sizeof tc);
		
        if (a == b) {
			timer_start(&TIMER_0);
            continue;
        }
        hydro_hash_update(&st, (const uint8_t *) &b, sizeof b);
        ebits++;
		timer_start(&TIMER_0);
		
    }

  /*  cli();
    MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
    sei();
*/
	timer_stop(&TIMER_0);
    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);
	
    return 0;
}

//ISR(WDT_vect) {}
