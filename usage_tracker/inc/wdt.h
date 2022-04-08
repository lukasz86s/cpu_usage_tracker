#ifndef WDT_H
#define WDT_H

typedef
        struct _Wdt{
            //number of timers
            unsigned int n_timers;
            //timers
            unsigned long int* timers;
            // pointer to the function call if wdt will work
            void (*wdt_call)(void);
        }Wdt;

Wdt* create_wdt(unsigned int n_timers, void (* wdc_call)(void));
void destroy_wdt(Wdt* wdt);
void check_wdt(void);
#endif