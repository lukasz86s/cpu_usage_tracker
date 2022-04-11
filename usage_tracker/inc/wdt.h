#ifndef WDT_H
#define WDT_H

typedef
        struct _Wdt{
            // pointer to the function call if wdt will work
            void (*wdt_call)(void);
            //timers
            unsigned long int* timers  ;
            //number of timers
            unsigned int n_timers;
        }Wdt;

Wdt* create_wdt(unsigned int n_timers);
void destroy_wdt(Wdt* wdt);
void check_wdt(Wdt* wdt);
void set_wdt_all_timers(Wdt* wdt, unsigned long int val);
void reset_wdt_timer(Wdt* wdt, unsigned int timer_number);
#endif
