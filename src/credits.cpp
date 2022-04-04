#include <stdint.h>
#include "hud.h"
#include "global.h"

char Mds[32] = "MANAGING DIRECTORS";
char DirDev[32] = "DIRECTOR OF DEVELOPMENT";
char Prod[32] = "PRODUCERS";
char ProdMan[32] = "PRODUCT MANAGER";
char TeamL[32] = "TEAM LEADER";
char GameD[32] = "GAME DESIGNER";

char PSX[32] = "PLAYSTATION VERSION";
char w95[32] = "ATI 3D RAGE VERSION";
char bsd[32] = "BROADSWORD INTERACTIVE LTD";
char pro[32] = "PRODUCER";
char atidev[32] = "DEVELOPED BY";
char ati[32] = "ATI LIASON";
char ati2[32] = "ATI QA";
char Prog[32] = "PROGRAMMERS";
char Artists[32] = "ORIGINAL ARTISTS";
char Intro[32] = "INTRO SEQUENCE";
char Concept[32] = "CONCEPTUAL ARTIST";
char GDesign[32] = "ADDITIONAL GRAPHIC DESIGN";
char Music[32] = "MUSIC";
char MusicL[32] = "MUSIC LIAISON";
char Sound[32] = "SOUND EFFECTS";
char Manual[32] = "MANUAL WRITTEN BY";
char Pack[32] = "PACKAGING DESIGN";
char Tech[32] = "TECHNICAL ASSISTANT";
char Sec[32] = "DEVELOPMENT SECRETARY";
char QA[32] = "QUALITY ASSURANCE";
char Thanks[32] = "SPECIAL THANKS TO";
char LOrbital[32] = "ORBITAL";
char LChem[32] = "CHEMICAL BROTHERS";
char LLeft[32] = "LEFTFIELD";

char blank[32] = "";
char PCVersion[32] = "PC VERSION";
char PCArtists[32] = "CONVERSION ARTISTS";
char PCSndProg[32] = "SOUND PROGRAMMING";
char PCNetwork[32] = "NETWORK PROGRAMMING";
char PCMovies[32] = "MOVIE PROGRAMMING";

char PCAndyY[32] = "ANDY YELLAND";
char PCAndyS[32] = "ANDY SATTERTHWAITE";
char PCDaveS[32] = "DAVE SMITH";
char PCJedA[32] = "JED ADAMS";
char PCMarkK[32] = "MARK KELLY";
char PCSteveW[32] = "STEVE WARD";
char PCChrisE[32] = "CHRIS EDEN";
char PCSalim[32] = "SALIM SIWANI";
char PCAndyC[32] = "ANDY CROWLEY";
char PCSteveD[32] = "STEVE DAVIES";
char PCMikeA[32] = "MIKE ANTHONY";
char PCGaryB[32] = "GARY BURLEY";
char PCJohnD[32] = "JOHN DWYER";
char ATISteve[32] = "STEPHEN ROSE";
char ATIJohn[32] = "JOHN JONES STEELE";
char ATIBill[32] = "BILL ALLEN";
//char ATIGordon[32]	=	"GORDON GRIGOR";
//char ATIVince[32]	=	"VINCENT CHAN";
//char ATIJohn2[32]	=	"JOHN ZANETTOS";

char Dave[32] = "DAVE ROSE";
char Stu[32] = "STEWART SOCKETT";
char Rob[32] = "ROB SMITH";
char Jas[32] = "JASON DENTON";
char IanH[32] = "IAN HETHERINGTON";
char JohnE[32] = "JONATHAN ELLIS";
char JohnW[32] = "JOHN WHITE";
char Dom[32] = "DOMINIC MALLINSON";
char Sue[32] = "SUE CAMPBELL";
char Nicky[32] = "NICKY CARUS WESTCOTT";
char Nik[32] = "NICK BURCOMBE";
char Laura[32] = "LAURA GRIEVE";
char Lou[32] = "LOUISE SMITH";
char Darren[32] = "DARREN DOUGLAS";
char Pol[32] = "POL SIGERSON";
char Lee[32] = "LEE CARUS WESTCOTT";
char Jim[32] = "JIM BOWERS";
char DR[32] = "THE DESIGNERS REPUBLIC";
char Orbital[32] = "ORBITAL";
char ChemBro[32] = "THE CHEMICAL BROTHERS";
char LeftF[32] = "LEFTFIELD";
char ColdS[32] = "COLD STORAGE";
char PaulMG[32] = "PAUL MCGARVEY";
char TimW[32] = "TIM WRIGHT";
char Damon[32] = "DAMON FAIRCLOUGH";
char Keith[32] = "KEITH HOPWOOD";
char Paul[32] = "PAUL CHARSLEY";
char Jen[32] = "JENNIFER REES";
char Test0[32] = "STUART ALLEN";
char Test1[32] = "CHRIS GRAHAM";
char Test2[32] = "THOMAS REES";
char Test3[32] = "BRIAN WALSH";
char Test4[32] = "CARL BERRY";
char Test5[32] = "MARK INMAN";
char Test6[32] = "PAUL TWEEDLE";
char Test7[32] = "ANTHONY CROSS";
char Test8[32] = "EDWARD HAY";
char Test9[32] = "ROB WOLFE";
char Hack[32] = "THE HACKERS TEAM MGM";
char Soft[32] = "SOFTIMAGE";
char SGI[32] = "SGI";
char Glen[32] = "GLEN OCONNELL";
char Jo[32] = "JOANNE GALVIN";
char All[32] = "ALL AT PSYGNOSIS";
char Pizza[32] = "PERFECT PIZZA";

char Pub[32] = "PUBLISHED BY";
char Hart[32] = "HARTNOLL HARTNOLL";
char Rol[32] = "ROLANDS SIMONS";
char Bar[32] = "BARNEY DALMY COLE";
char Sony[32] = "SONY MUSIC PUBLISHING";
char BMG[32] = "BMG MUSIC SIMONS";
char Hard[32] = "HARD HANDS UK PUBLISHING";
char Rhth[32] = "RHYTHM KING MUSIC";
char Chys[32] = "CHRYSALIS MUSIC";
char Wipe[32] = "WIPEOUT";
char Orig[32] = "ORIGINAL SOUND RECORDING";
char Lisen[32] = "LICENSED FROM";
char London[32] = "LONDON RECORDS";
char ChemBeat[32] = "CHEMICAL BEATS";
char Virgin[32] = "VIRGIN RECORDS LIMITED";
char Afro[32] = "AFRO RIDE";
char LisEx[32] = "LICENSED EXCLUSIVELY TO";
char SME[32] = "SONY MUSIC ENTERTAINMENT UK";
char Limited[32] = "LIMITED";
char Cour[32] = "COURTESY OF ";
char HHands[32] = "HARD HANDS LIMITED";
char Col[32] = "COLUMBIA RECORDS";
char Psyg[32] = "LICENSED TO PSYGNOSIS BY";

typedef struct _name {
  char name[32];
  struct _name* nextName;
} Name;

typedef struct Title {
  char* title;
  char* name[16];
  int32_t noOfNames;
} Title;

#define NO_OF_CREDITS 27

Title credits[NO_OF_CREDITS];

void InitCredits() {
  int32_t i = 0;
  credits[i].noOfNames = 2;
  credits[i].title = Mds;
  credits[i].name[0] = IanH;
  credits[i++].name[1] = JohnE;

  credits[i].noOfNames = 1;
  credits[i].title = DirDev;
  credits[i++].name[0] = JohnW;

  credits[i].noOfNames = 2;
  credits[i].title = Prod;
  credits[i].name[0] = Dom;
  credits[i++].name[1] = PCAndyY;

  credits[i].noOfNames = 1;
  credits[i].title = ProdMan;
  credits[i++].name[0] = Sue;

  credits[i].noOfNames = 6;
  credits[i].title = GameD;
  credits[i].name[0] = Nik;
  credits[i].name[1] = blank;
  credits[i].name[2] = blank;
  credits[i].name[3] = blank;
  credits[i].name[4] = blank;
  credits[i++].name[5] = blank;

  credits[i].noOfNames = 0;
  credits[i++].title = w95;

  credits[i].noOfNames = 1;
  credits[i].title = pro;
  credits[i++].name[0] = ATIBill;

  credits[i].noOfNames = 0;
  credits[i++].title = atidev;

  credits[i].noOfNames = 2;
  credits[i].title = bsd;
  credits[i].name[0] = ATISteve;
  credits[i++].name[1] = ATIJohn;

  //redits [i].noOfNames = 1;
  //redits[i].title = ati;
  //credits[i++].name[0] = ATIGordon;

  //credits [i].noOfNames = 2;
  //credits[i].title = ati2;
  //credits[i].name[0] = ATIVince;
  //credits[i++].name[1] = ATIJohn2;

  credits[i].noOfNames = 0;
  credits[i++].title = PCVersion;

  credits[i].noOfNames = 8;
  credits[i].title = Prog;
  credits[i].name[0] = PCAndyY;
  credits[i].name[1] = PCAndyS;
  credits[i].name[2] = PCDaveS;
  credits[i].name[3] = PCMarkK;
  credits[i].name[4] = PCJedA;
  credits[i].name[5] = PCSteveW;
  credits[i].name[6] = PCChrisE;
  credits[i++].name[7] = PCSalim;

  credits[i].noOfNames = 1;
  credits[i].title = PCSndProg;
  credits[i++].name[0] = PCAndyC;

#if 0
	credits[i].noOfNames = 1;
	credits[i].title = PCNetwork;
	credits[i++].name[0] = PCSteveD;
#endif

  credits[i].noOfNames = 1;
  credits[i].title = PCMovies;
  credits[i++].name[0] = PCMikeA;

  credits[i].noOfNames = 7;
  credits[i].title = PCArtists;
  credits[i].name[0] = PCJohnD;
  credits[i].name[1] = PCGaryB;
  credits[i].name[2] = blank;
  credits[i].name[3] = blank;
  credits[i].name[4] = blank;
  credits[i].name[5] = blank;
  credits[i++].name[6] = blank;

  credits[i].noOfNames = 1;
  credits[i].title = Intro;
  credits[i++].name[0] = Lee;

  credits[i].noOfNames = 1;
  credits[i].title = Concept;
  credits[i++].name[0] = Jim;

  credits[i].noOfNames = 1;
  credits[i].title = GDesign;
  credits[i++].name[0] = DR;

  credits[i].noOfNames = 1;
  credits[i].title = Music;
  credits[i++].name[0] = ColdS;

  credits[i].noOfNames = 1;
  credits[i].title = Sound;
  credits[i++].name[0] = TimW;

  credits[i].noOfNames = 2;
  credits[i].title = Manual;
  credits[i].name[0] = Damon;
  credits[i++].name[1] = Nik;

  credits[i].noOfNames = 2;
  credits[i].title = Pack;
  credits[i].name[0] = DR;
  credits[i++].name[1] = Keith;

  credits[i].noOfNames = 6;
  credits[i].title = Sec;
  credits[i].name[0] = Jen;
  credits[i].name[1] = blank;
  credits[i].name[2] = blank;
  credits[i].name[3] = blank;
  credits[i].name[4] = blank;
  credits[i++].name[5] = blank;

  credits[i].noOfNames = 0;
  credits[i++].title = PSX;

  credits[i].noOfNames = 4;
  credits[i].title = Prog;
  credits[i].name[0] = Dave;
  credits[i].name[1] = Rob;
  credits[i].name[2] = Jas;
  credits[i++].name[3] = Stu;

  credits[i].noOfNames = 10;
  credits[i].title = Artists;
  credits[i].name[0] = Nicky;
  credits[i].name[1] = Laura;
  credits[i].name[2] = Lou;
  credits[i].name[3] = Darren;
  credits[i].name[4] = Pol;
  credits[i].name[5] = blank;
  credits[i].name[6] = blank;
  credits[i].name[7] = blank;
  credits[i].name[8] = blank;
  credits[i++].name[9] = blank;

  credits[i].noOfNames = 10;
  credits[i].title = QA;
  credits[i].name[0] = Test0;
  credits[i].name[1] = Test1;
  credits[i].name[2] = Test2;
  credits[i].name[3] = Test3;
  credits[i].name[4] = Test4;
  credits[i].name[5] = Test5;
  credits[i].name[6] = Test6;
  credits[i].name[7] = Test7;
  credits[i].name[8] = Test8;
  credits[i++].name[9] = Test9;

  credits[i].noOfNames = 6;
  credits[i].title = Thanks;
  credits[i].name[0] = Hack;
  credits[i].name[1] = Soft;
  credits[i].name[2] = SGI;
  credits[i].name[3] = Glen;
  credits[i].name[4] = Jo;
  credits[i++].name[5] = All;
}

void ShowCredits(int32_t attractDelay) {
  int32_t i, j;
  int32_t place;

  place = attractDelay - (60 * FR30); // whats set in race.c -400
  for (i = 0; i < NO_OF_CREDITS; i++) {
    if ((place > -10) && (place < 264))
      CentreText(credits[i].title, text_data(230, place, 8), RedText);
    place += 16;
    for (j = 0; j < credits[i].noOfNames; j++) {
      if ((place > -10) && (place < 264))
        CentreText(credits[i].name[j], text_data(230, place, 8), WhiteText);
      place += 12;
    }
    place += 16;
  }
}
