//-----------------------------------------------------------------------//
// WIPEOUT PC Modem game routines.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include			"c:\wc10\h\stdio.h"
#include			"..\markk\modemwo.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

//-----------------------------------------------------------------------//

long				CommModemMachine;
char				CommModemInitString[_MODEM_STRING_SIZE];
char				CommModemSetupString[_MODEM_STRING_SIZE];
char				CommModemDialupString[_MODEM_STRING_SIZE];
char				CommModemAnswerString[_MODEM_STRING_SIZE];
char				CommModemHangupString[_MODEM_STRING_SIZE];

//-----------------------------------------------------------------------//
