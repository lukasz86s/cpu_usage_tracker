#include <stdio.h>
#include <stdlib.h>
#include "inc/wdt.h"

Wdt* create_wdt(unsigned int n_timers, void (* wdc_call)(void)){
    
    Wdt* temp_wdt = (Wdt*) malloc(sizeof(Wdt));
    //fill values
    temp_wdt->n_timers = n_timers;
    temp_wdt->timers = (unsigned long int*) malloc(sizeof(long int) * n_timers);
    temp_wdt->wdt_call = wdc_call;
    return temp_wdt;
}

void destroy_wdt(Wdt* wdt){

    free(wdt->timers);
    free(wdt);
}
void set_wdt_all_timers(Wdt* wdt, unsigned long int val)
{
        for(int i = 0; i < wdt->n_timers; i++){
        // if some timer reach 0 , call wdt_call
        wdt->timers[i] = val;
    }
}

void check_wdt(Wdt* wdt){
    for(int i = 0; i < wdt->n_timers; i++){
        // if some timer reach 0 , call wdt_call
        if(!(wdt->timers[i]--)) 
            wdt->wdt_call();
    }
    
}

