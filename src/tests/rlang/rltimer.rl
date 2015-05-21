#include "arch/va/int.rl"
#include "arch/va/timer.rl"

#define TIMER_INT_NO 130

STATIC cycles := 5;

PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
	?? Timer delay is initialized to 1000ms
	IF (timerGetDelay [ NONE ] != 1000) THEN
		FAIL;
	END
	
	?? Set timer delay to 2000ms
	timerSetDelay [ 2000; ];
	
	?? Register the ISR
	intRegisterIsr [ TIMER_INT_NO; .isrTimer; ];
	
	?? Enable Hardware Interrupts
	intEnable [ NONE ];
	
	?? Enable the Timer
	timerEnable [ NONE ];
	
	WHILE (@REG:.cycles) LOOP
		NONE;
	END
	
	STOP;
END

IPROCEDURE isrTimer IS
	SAVE;
	
	@REG:.cycles := ((@REG:.cycles) - 1);
	
	RESTORE;
	
	$IRET;
END
