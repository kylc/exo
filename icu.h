#ifndef ICU_H
#define ICU_H

void setup_icu(void)
{
	TCCR3A = 0;      // Normal mode
	TCCR3B = 0x44;   // Rising edge trigger, Timer = CPU Clock/256 -or- 16000000/256 = 62.5 kHz
	TCCR3C = 0;      // Normal mode
	TIMSK3 = 0x21;   // Input capture and overflow interupts enabled
	TCNT3  = 0;      // Start from 0
}

#endif // ICU_H

