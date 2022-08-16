/**
 * @file timers__.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module timers_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifndef INCLUDE_MINUTERIES_H_
#define INCLUDE_MINUTERIES_H_

    void timers_ini();
    String timers_menu();
    String timers_stop();
    String timers_restart();
    String timers_interpret(String line);

    String timers_intervals();
    uint32_t timers_interval_ms(uint32_t interval, String units);

#endif //MINUTERIES_H_