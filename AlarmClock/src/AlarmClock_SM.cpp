/*.$file${HSMs::../src::AlarmClock_SM.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: AlarmClock.qm
* File:  ${HSMs::../src::AlarmClock_SM.cpp}
*
* This code has been generated by QM 5.1.1 <www.state-machine.com/qm/>.
* DO NOT EDIT SECTIONS BETWEEN THE COMMENTS "$...vvv".."$end...^^^".
* All your changes in these sections will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*.$endhead${HSMs::../src::AlarmClock_SM.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include <Arduino.h>
#include "qpn.h"
#include "lcd.h"
#include "AlarmClock_SM.h"
/*.$declare${HSMs::AlarmClock} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::AlarmClock} .....................................................*/
typedef struct AlarmClock {
/* protected: */
    QHsm super;

/* private: */
    uint32_t Temp_time;
    uint32_t Alarm_time;
    uint8_t Alarm_status;
    uint8_t Time_mode;

/* private state histories */
    QStateHandler hist_Clock;
} AlarmClock;

/* public: */
static uint32_t AlarmClock_Get_Curr_Time(void);
static void AlarmClock_Update_Curr_Time(void);
static void AlarmClock_Set_Curr_Time(uint32_t New_Curr_Time);
static void AlarmClock_display_curr_time(AlarmClock * const me, uint8_t row, uint8_t col);
extern uint32_t AlarmClock_Curr_time;
extern AlarmClock AlarmClock_Obj;

/* protected: */
static QState AlarmClock_initial(AlarmClock * const me);
static QState AlarmClock_Clock(AlarmClock * const me);
static QState AlarmClock_Ticking(AlarmClock * const me);
static QState AlarmClock_Settings(AlarmClock * const me);
static QState AlarmClock_Clock_Settings(AlarmClock * const me);
static QState AlarmClock_Alarm_Settings(AlarmClock * const me);
static QState AlarmClock_Alarm_Notify(AlarmClock * const me);
/*.$enddecl${HSMs::AlarmClock} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
String get_am_or_pm(uint32_t time24h);
void  display_write(String str_, uint8_t r, uint8_t c);
String integertime_to_string(uint32_t time_);
uint32_t convert_24hformat_to_12h(uint32_t time24h);
/*.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*. Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpn version 6.9.0 or higher required
#endif
/*.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$define${HSMs::AlarmClock} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::AlarmClock} .....................................................*/
uint32_t AlarmClock_Curr_time;
AlarmClock AlarmClock_Obj;
/*.${HSMs::AlarmClock::Get_Curr_Time} ......................................*/
static uint32_t AlarmClock_Get_Curr_Time(void) {
            uint8_t save_sreg= SREG;
            uint32_t temp;
            cli();
            temp=AlarmClock_Curr_time;
            SREG=save_sreg;
            return temp;
}

/*.${HSMs::AlarmClock::Update_Curr_Time} ...................................*/
static void AlarmClock_Update_Curr_Time(void) {
        if(++AlarmClock_Curr_time==MAX_TIME){
            AlarmClock_Curr_time=0;
        }
}

/*.${HSMs::AlarmClock::Set_Curr_Time} ......................................*/
static void AlarmClock_Set_Curr_Time(uint32_t New_Curr_Time) {
        uint8_t save_sreg= SREG;
        cli();
        AlarmClock_Curr_time=New_Curr_Time;
        SREG=save_sreg;
}

/*.${HSMs::AlarmClock::display_curr_time} ..................................*/
static void AlarmClock_display_curr_time(AlarmClock * const me, uint8_t row, uint8_t col) {
            String time_as_string;
            uint32_t time_;
            uint32_t time24h = AlarmClock_Get_Curr_Time()/10; //convert to number of seconds
            uint8_t ss = time24h % 10U;       //extract sub-second to append later
            time_ = (me->Time_mode == MODE_24H)?time24h:convert_24hformat_to_12h(time24h);
            time_as_string = integertime_to_string(time_); //hh:mm:ss
            time_as_string.concat('.');
            time_as_string.concat(ss);

            /*if mode is 12H , concatenate  am/pm information */
            if(me->Time_mode == MODE_12H){
                time_as_string.concat(' ');
                time_as_string.concat(get_am_or_pm(time24h));
            }

            display_write(time_as_string,row,col);
}

/*.${HSMs::AlarmClock::SM} .................................................*/
static QState AlarmClock_initial(AlarmClock * const me) {
    /*.${HSMs::AlarmClock::SM::initial} */
    AlarmClock_Set_Curr_Time(INITIAL_CURR_TIME);
    me->Alarm_time= INITIAL_ALARM_TIME;
    me->Time_mode= MODE_12H;
    me->Alarm_status=ALARM_OFF;
    /* state history attributes */
    /* state history attributes */
    me->hist_Clock = Q_STATE_CAST(&AlarmClock_Ticking);
    return Q_TRAN(&AlarmClock_Ticking);
}
/*.${HSMs::AlarmClock::SM::Clock} ..........................................*/
static QState AlarmClock_Clock(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::AlarmClock::SM::Clock} */
        case Q_EXIT_SIG: {
            /* save deep history */
            me->hist_Clock = QHsm_state(me);
            status_ = Q_HANDLED();
            break;
        }
        /*.${HSMs::AlarmClock::SM::Clock::ALARM} */
        case ALARM_SIG: {
            status_ = Q_TRAN(&AlarmClock_Alarm_Notify);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.${HSMs::AlarmClock::SM::Clock::Ticking} .................................*/
static QState AlarmClock_Ticking(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::AlarmClock::SM::Clock::Ticking} */
        case Q_ENTRY_SIG: {
            AlarmClock_display_curr_time(me,TICKING_CURR_TIME_ROW,TICKING_CURR_TIME_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*.${HSMs::AlarmClock::SM::Clock::Ticking::    SET} */
        case     SET_SIG: {
            status_ = Q_TRAN(&AlarmClock_Clock_Settings);
            break;
        }
        /*.${HSMs::AlarmClock::SM::Clock::Ticking::    OK} */
        case     OK_SIG: {
            status_ = Q_TRAN(&AlarmClock_Alarm_Settings);
            break;
        }
        default: {
            status_ = Q_SUPER(&AlarmClock_Clock);
            break;
        }
    }
    return status_;
}
/*.${HSMs::AlarmClock::SM::Clock::Settings} ................................*/
static QState AlarmClock_Settings(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::AlarmClock::SM::Clock::Settings::ABORT} */
        case ABORT_SIG: {
            status_ = Q_TRAN(&AlarmClock_Ticking);
            break;
        }
        /*.${HSMs::AlarmClock::SM::Clock::Settings::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&AlarmClock_Ticking);
            break;
        }
        default: {
            status_ = Q_SUPER(&AlarmClock_Clock);
            break;
        }
    }
    return status_;
}
/*.${HSMs::AlarmClock::SM::Clock::Settings::Clock_Settings} ................*/
static QState AlarmClock_Clock_Settings(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = Q_SUPER(&AlarmClock_Settings);
            break;
        }
    }
    return status_;
}
/*.${HSMs::AlarmClock::SM::Clock::Settings::Alarm_Settings} ................*/
static QState AlarmClock_Alarm_Settings(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = Q_SUPER(&AlarmClock_Settings);
            break;
        }
    }
    return status_;
}
/*.${HSMs::AlarmClock::SM::Alarm_Notify} ...................................*/
static QState AlarmClock_Alarm_Notify(AlarmClock * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::AlarmClock::SM::Alarm_Notify::TRIG1} */
        case TICK_SIG: {
            status_ = Q_TRAN_HIST(me->hist_Clock);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.$enddef${HSMs::AlarmClock} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$define${HSMs::AlarmClock_Ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::AlarmClock_Ctor} ................................................*/
void AlarmClock_Ctor(void) {
    QHsm_ctor(&AlarmClock_Obj.super, Q_STATE_CAST(&AlarmClock_initial));
}
/*.$enddef${HSMs::AlarmClock_Ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

ISR(TIMER_COMPA_VECT){
    AlarmClock_Update_Curr_Time();
}

/*
 * Description : Decodes AM/PM information from given time in 24H format
 * param1: Integer time in 24H format
 * return : A string value("AM" or "PM")
 */
String get_am_or_pm(uint32_t time24h){
    String ampm;
    uint8_t h = GET_HOUR(time24h);
    if(h == 0U){
        ampm = "AM";
    }else if( h > 12U){
        ampm = "PM";
    }else if (h == 12U)
        ampm = "PM";
    else
        ampm = "AM";
    return ampm;
}

/*
 * Description: Writes a message to the LCD at given row and column number
 * param1 : Message to write in 'String' format
 * param2 : row number of the LCD
 * param2 : column number of the LCD
 */
void  display_write(String str_, uint8_t r, uint8_t c){
    lcd_set_cursor(c,r);
    lcd_print_string(str_);
}

/*
 * Description: converts an 'integer' time to 'String' time
 * param1 : time represented in terms of number of seconds
 * return : time as 'String' value in the format HH:MM:SS
 */
String integertime_to_string(uint32_t time_){
    uint8_t h,m,s;
    char buf[10]; //00:00:00+null
    h = GET_HOUR(time_); /* Extract how many hours the 'time_' represent */
    m = GET_MIN(time_);  /* Extract how many minutes the 'time_' represent */
    s = GET_SEC(time_);	 /* Extract how many seconds the 'time_' represent */
    sprintf(buf,"%02d:%02d:%02d",h,m,s);
    return (String)buf;
}

/*
 * Description: Converts given integer time in 24H format to integer time 12H format
 * param1 : Integer time in 24H format
 * return : Integer time in 12H format
 */
uint32_t convert_24hformat_to_12h(uint32_t time24h){
    uint8_t hour;
    uint32_t time12h;
    hour = GET_HOUR(time24h);

    if(hour == 0)
        time12h = time24h + (12UL * 3600UL);
    else{
        if((hour < 12UL) || (hour == 12UL))
            return time24h;
        else
            time12h = time24h - (12UL * 3600UL);
    }
    return time12h;
}