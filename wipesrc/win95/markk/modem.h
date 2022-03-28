//-----------------------------------------------------------------------//
// Serial low-level library header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#define					_MODEM_STRING_SIZE			256
#define					_MODEM_INIT_STRING0			"AT&F1"
#define					_MODEM_DIAL_STRING0			"ATDT"

#define					NUM_RING_RETRIES			8
#define					NUM_ANSWER_RETRIES			16

#define					OK							0
#define					CONNECT						1
#define					RING						2
#define					NO_CARRIER					3
#define					ERROR						4

//-----------------------------------------------------------------------//

#define					write_command(x)			cwrite(CommSerialDevice, x, strlen(x))

//-----------------------------------------------------------------------//

extern char				*CommandMode;
extern char				*Enter;
extern char				*InitStringA;
extern char				*InitStringB;
extern char				*SetupString1;
extern char				*SetupString2;
extern char				*DialupStringA;
extern char				*DialupStringB;
extern char				*AnswerString;
extern char				*HangupString;
extern char				ResultCode[];

//-----------------------------------------------------------------------//

extern long				initmodem(char *init);
extern long				setupmodem(char *setup);
extern long				dialup(char *dial);
extern long				answer(char *answ);
extern long				hangup(void);
extern long				connectmodem(void);
extern long				disconnectmodem(void);
extern long				getresultcode(char *var);
extern long				waitresultcode(char code);

//-----------------------------------------------------------------------//
