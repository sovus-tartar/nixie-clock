#ifndef Hacks_h
#define Hacks_h
#include <Arduino.h>

#define ALLOW_ANYPWM 0	// - включить или отключить ANYPWM
// необходимо отключить, если этот режим не нужен и вы сами используете прерывания timer2 (COMPA_vect)

/*	
	Hacks - библиотека с некоторыми удобными хаками для Arduino UNO/NANO/MINI (atmega328):
	- Быстрые аналоги стандартных функций чтения/записи 
	- Изменение частоты ШИМ пинов (3, 5, 6, 9, 10, 11)
	- Изменение режима ШИМ
	- Установка ШИМ на пинах 9 и 10 в режим 10 бит (abalogWrite 0-1023)
	- Генерация ШИМ на ЛЮБОМ пине (частота ~150 Гц)
	- Измерение напряжения питания + калибровка константы
	- Перевод напряжения питания в проценты по графику разряда для разных типов АКБ
	- Измерение температуры ядра
	
	Текущая версия: 2.8 от 02.04.2019
	- Поправлен критический баг в работе функции readPin
*/

// ********************************************* Хаки с ШИМ *********************************************

void setPWMmode(byte pin, byte mode);				// установка режима ШИМ на пине 
													// пины 3, 5, 6, 9, 10, 11
													// режимы: 0 - FastPWM, 1 - Phase-correct PWM


void setPWMprescaler(uint8_t pin, uint16_t mode);	// установка частоты ШИМ на пине
/*
Timer 0 (пины 5 и 6) (ВЛИЯЕТ НА РАБОТУ millis() и delay())
Timer 1 (пины 9 и 10) (ВЛИЯЕТ НА РАБОТУ servo)
Timer 2 (пины 3 и 11) (ВЛИЯЕТ НА РАБОТУ tone())
*/
/*
____________________________________________________________________________________________
	| Timer 0 (пины 5 и 6)		| Timer 1 (пины 9 и 10) 		| Timer 2 (пины 3 и 11)		| 
	| Timer 1 (пины 9 и 10)		| в режиме 10 bit				|							| 
____|___________________________|_______________________________|___________________________|
mode| Phase-correct	| Fast PWM	| Phase-correct		| Fast PWM	| Phase-correct	| Fast PWM	|
____|_______________|___________|___________________|___________|_______________|___________|
1	| 31.4 kHz		| 62.5 kHz	| 7.8 kHz			| 15.6 kHz	| 31.4 kHz		| 62.5 kHz	| 
2	| 4 kHz			| 7.8 kHz	| 977 Hz			| 2 kHz		| 4 kHz			| 8 kHz		| 
3	| 490 Hz		| 976 Hz	| 122 Hz			| 244 Hz	| 980 Hz		| 2 kHz		| 
4	| 122 Hz		| 244 Hz	| 30 Hz				| 61 Hz		| 490 Hz		| 980 Hz	| 
5	| 30 Hz			| 61 Hz		| 7.6 Hz			| 15 Hz		| 245 Hz		| 490 Hz	| 
6	| -				| -			| -					| -			| 122 Hz		| 244 Hz	| 
7	| -				| -			| -					| -			| 30 Hz			| 60 Hz		| 
____|_______________|___________|___________________|___________|_______________|___________|

*/

void delayFix(uint32_t delayTime);					// аналог delay для корректной работы с изменённой частотой ШИМ пинов 5 и 6
void delayMicrosecondsFix(uint32_t delayTime);		// аналог delayMicroseconds для корректной работы с изменённой частотой ШИМ пинов 5 и 6

uint32_t millisFix(); 								// аналог millis для корректной работы с изменённой частотой ШИМ пинов 5 и 6
uint32_t microsFix();								// аналог micros для корректной работы с изменённой частотой ШИМ пинов 5 и 6

void set8bitPWM();									// установка ШИМ на пинах 9 и 10 в режим 8 бит (analogWrite 0-255) (по умолчанию)

void set10bitPWM();									// установка ШИМ на пинах 9 и 10 в режим 10 бит (analogWrite 0-1023)

void anyPWMinit(byte prescaler);					// инициализация ШИМ на любом пине
													// prescaler: 4 - 311 Гц, 5 - 244 Гц, 6 - 122 Гц, 7 - 30 Гц
													// если пинов много - понижайте частоту (20 пинов работают отлично на 6 режиме)													
													
void anyPWMpin(uint8_t pin);						// настроить ЛЮБОЙ пин для генерации ШИМ

void anyPWM(byte pin, byte duty); 					// включить ШИМ на ЛЮБОМ пине (настроенном выше)

// ********************************************* Ускоряем функции *********************************************

void setPWM(uint8_t pin, uint16_t duty);				// быстрый аналог analogWrite (в 7 раз быстрее)

void setPin(uint8_t pin, uint8_t x);				// быстрый аналог digitalWrite (в 10 раз быстрее)

boolean readPin(uint8_t pin);						// быстрый аналог digitalRead (в 11 раз быстрее)

void setADCrate(byte mode);							// установка скорости работы АЦП (analogRead)
													// mode 1: 3.04 мкс (частота оцифровки 329 000 кГц)
													// mode 2: 4.72 мкс (частота оцифровки 210 000 кГц)
													// mode 3: 8.04 мкс (частота оцифровки 125 000 кГц)
													// mode 4: 15.12 мкс (частота оцифровки 66 100 кГц)
													// mode 5: 28.04 мкс (частота оцифровки 35 600 кГц)
													// mode 6: 56.04 мкс (частота оцифровки 17 800 кГц)													
													// mode 7: 112 мкс (частота оцифровки 8 900 Гц)

// ********************************************* Точный вольтметр *********************************************

int getVCC();						// возвращает опорное напряжение (напряжение питания)
									// константа по умолчанию равна 1100, нужно калибровать!

void setConst(int new_const);		// установить константу вручную (по умолч. 1100)

int getConst();						// вывести текущую константу
	
void constantWizard();				// помошник калибровки константы (смотри пример)

void restoreConstant(int adr);		// восстановить константу из памяти

int getVoltage(uint8_t pin);		// измерить напряжение на пине с учётом опорного

// функции получения процента заряда из напряжения, линеаризованы вручную по графикам разряда АКБ
// использовать вот так: lithiumPercent(getVCC()); - ардуина питается от лития

byte lithiumPercent(int volts);		// возвращает процент заряда Li-ion аккумулятора (4,2-2,8 В)

byte alkaline3Percent(int volts);	// возвращает процент заряда 3х алкалиновых батареек (4,6-3,3 В)

byte nickel3Percent(int volts);		// возвращает процент заряда 3х Ni-Cd аккумуляторов (4.2-3.0 В)

byte nickel4Percent(int volts);		// возвращает процент заряда 4х Ni-Cd аккумуляторов (5.6-4.0В)

// функция для расчёта заряда батареи в процентах
// принимает напряжение в милливолльтах (volts), а также напряжения, при которых заряд равен 100, 80... 0%
byte mVtoPercent(int volts, int volt100, int volt80, int volt60, int volt40, int volt20, int volt0);

// ********************************************* Прочее *********************************************
float getTemp();		// получить примерную температуру ядра процессора (температура окружающей среды или температура внутри корпуса)

/*
	Источники:
	http://arduino.ru/forum/programmirovanie/analogwrite-na-lyubom-vyvode
	http://mypractic-forum.ru/viewtopic.php?t=15
	http://blog.unlimite.net/?p=25
	http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html?m=1	
*/

// оставлено для совместимости со старыми версиями
class GTimer
{
  public:
	GTimer();
	GTimer(uint32_t);			// объявление таймера с указанием интервала
	void setInterval(uint32_t);	// установка интервала
	void setMode(boolean);		// установка типа работы: AUTO или MANUAL (MANUAL нужно вручную сбрасывать reset)
	boolean isReady();			// возвращает true, когда пришло время. Сбрасывается в false сам (AUTO) или вручную (MANUAL)
	void reset();				// ручной сброс таймера на установленный интервал
  private:
	uint32_t _timer;
	uint32_t _interval;
	boolean _mode = true;
};

#define MANUAL 0
#define AUTO 1

#endif