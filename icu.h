#ifndef ICU_H
#define ICU_H

void setup_icu(void)
{
	TCCR1A = 0;      // Normal mode
	TCCR1B = 0x44;   // Rising edge trigger, Timer = CPU Clock/256 -or- 16000000/256 = 62.5 kHz
	TCCR1C = 0;      // Normal mode
	TIMSK1 = 0x21;   // Input capture and overflow interupts enabled
	TCNT1  = 0;      // Start from 0
}

#endif // ICU_H

