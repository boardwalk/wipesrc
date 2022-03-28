//-----------------------------------------------------------------------//
// Modem low-level library.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include		"c:\wc10\h\stdio.h"
#include		"c:\wc10\h\string.h"
#include		"..\markk\serialwo.h"
#include		"..\markk\modemwo.h"
#include		"..\markk\comio.h"
#include		"..\markk\modem.h"
#include		"..\markk\merror.h"
#include		"..\markk\debug.h"

//------------------------------------------------------------------------

char			*CommandMode	= "+++";
char			*Enter 			= "\n";
char			*InitStringA 	= "ATZ4\n";				// sportster
char			*InitStringB	= "ATZ0\n";				// accura
char			*SetupString1	= "ATV0\n";
char			*SetupString2	= "ATM1X0E0\n";
char			*DialupStringA 	= "ATDT901512823380\n";
char			*DialupStringB 	= "ATDT901512823381\n";
char			*AnswerString 	= "ATS0=4\n";
char			*HangupString	= "ATH\n";
char			ResultCode[_MODEM_STRING_SIZE];

//------------------------------------------------------------------------

/* Try to establish computer connection to modem device. */
long			initmodem(char *init)
{
		char	error = OK;
		
		fn_track("mak_initmodem\n")

		cwrite(CommSerialDevice, init, strlen(init));
		error = waitresultcode(OK);
		
		if(error) return (_ERR_INITIALIZING_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------

/* Try to establish computer connection to modem device. */
long			setupmodem(char *setup)
{
		char	error = OK;

		fn_track("mak_setupmodem\n")
		
//		cwrite(CommSerialDevice, CommandMode, strlen(CommandMode));
//		cwrite(CommSerialDevice, Enter, strlen(Enter));
		cwrite(CommSerialDevice, setup, strlen(setup));
		error = waitresultcode(OK);
		
		if(error) return (_ERR_INITIALIZING_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------

long			dialup(char *dial)
{
		long	i;
		char	error = OK;
		
		fn_track("mak_dialup\n")
		
		cwrite(CommSerialDevice, dial, strlen(dial));
		for(i=0; i<NUM_RING_RETRIES; i++) {
			error = waitresultcode(RING);
			if(error) return (_ERR_DIALLING_MODEM);
		}

		delay(_1_FRAME*30*1);
		error = waitresultcode(CONNECT);
		if(error) return (_ERR_CONNECTING_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------

long			answer(char *answ)
{
		long	i;
		char	error = OK;

		fn_track("mak_answer\n")

		delay(_1_FRAME*30*1);
		cwrite(CommSerialDevice, answ, strlen(answ));
		for(i=0; i<NUM_ANSWER_RETRIES; i++) {
			error = waitresultcode(CONNECT);
		}
		
		if(error) return (_ERR_ANSWERING_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------

long			hangup(void)
{

		//cwrite(CommSerialDevice, HangupString, strlen(HangupString));
		return (OK);
}
//------------------------------------------------------------------------

#ifndef USE_ATI
long			connectmodem(void)
{
		long	error = OK;
		char	modem_status = 0;

		fn_track("mak_connectmodem\n")

		modem_status = inp(com_PortSelect + _MODEM_CONTROL);
		outp(com_PortSelect + _MODEM_CONTROL, modem_status | _DTR | _RTS);
		delay(2);
		modem_status = inp(com_PortSelect + _MODEM_STATUS);
		if(!(modem_status & (_DSR | _CTS))) return (_ERR_INITIALIZING_MODEM);
		
		write_command("+++");		// command mode
		write_command("AT\n");
		write_command("ATZ0\x0a\x0d");
		write_command("ATE0\x0a\x0d");		// echo off
		write_command("ATV0\x0a\x0d");		// return numeric codes
		write_command("ATX0\x0a\x0d");		// return numeric group X0
		write_command("ATM1\x0a\x0d");		// speaker on until connect
		write_command("ATS0=0\x0a\x0d");
		write_command("ATDT901512823380\x0a\x0d");

		error = setupmodem(CommModemSetupString);
		if(error) return (error);

		error =	initmodem(CommModemInitString);
		if(error) return (error);
		if(CommModemMachine == _DIAL) {
			error = dialup(CommModemDialupString);
			if(error) return (error);
		}
		if(CommModemMachine == _ANSWER) {
			error = answer(CommModemAnswerString);
			if(error) return (error);
		}
	
		return (OK);
}
#endif //USE_ATI

//------------------------------------------------------------------------

long			disconnectmodem(void)
{
		outp(com_PortSelect + _MODEM_CONTROL, 0);
		
		return (OK);
}

//------------------------------------------------------------------------

long			getresultcode(char *var)
{
		long	read_error = OK;

		fn_track("mak_getresultcode\n")

		delay(_1_FRAME*5);
	   	read_error = cread(CommSerialDevice, var, 1);
		
		if(read_error) return (_ERR_TIMEOUT_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------

long			waitresultcode(char code)
{
		long	not_code = 1;
		long	timeout = 0;
		char	result;

		fn_track("mak_waitresultcode\n")

		while((not_code) && (!timeout))
		{
			timeout = getresultcode(&result);
			if(result == code) not_code = 0;
		}
	
		if(!timeout) return (_ERR_TIMEOUT_MODEM);
		else return (OK);
}

//------------------------------------------------------------------------
