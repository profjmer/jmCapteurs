/**
 * @file rtc_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module rtc_.cpp, architecture jmObjean_2023
 * @version 1.0
 */

#ifndef INCLUDE_gestionRTC_H_
#define INCLUDE_gestionRTC_H_

	#define adresseRTC 0x68
	String rtc_write(String dateHeure);
	String rtc_reqNTP();
	String rtc_read();
	String rtc_menu();
	String rtc_interpret(String line);
	String rtc_ts();	
	void rtc_ini();
	void rtc_setNextWakeUpAlarm2();

	void rtc_espDontSleep();
	uint8_t rtc_getSleepHre();
	uint8_t rtc_getSleepMin();
	String rtc_getTimeZone();

#endif //INCLUDE_gestionRTC_H_


	