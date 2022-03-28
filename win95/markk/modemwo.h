//-----------------------------------------------------------------------//
// WIPEOUT PC Modem game routines header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#define					_MODEM_STRING_SIZE			256
#define					_DIAL						1
#define					_ANSWER						2

//-----------------------------------------------------------------------//

extern long				CommModemMachine;
extern char				CommModemInitString[_MODEM_STRING_SIZE];
extern char				CommModemSetupString[_MODEM_STRING_SIZE];
extern char				CommModemDialupString[_MODEM_STRING_SIZE];
extern char				CommModemAnswerString[_MODEM_STRING_SIZE];
extern char				CommModemHangupString[_MODEM_STRING_SIZE];

//-----------------------------------------------------------------------//
