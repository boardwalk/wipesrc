#include "standard.h"
#include <string.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "wtl.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "fileops.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "ajs.h"

extern TexChar CharSet16[];

HiScores scoreTable[(HiScoreTracks * 2)];
extern ChampionShip winTable[];
extern int32_t ajscount;
#define AJSDELAY 60 * 30

void FillHighScores(HiScores* scoreTable, int32_t lap1, int32_t lap2, int32_t lap3, int32_t lap4, int32_t lap5) {
  lap1 = (lap1 / 60) * FR60 + 200;
  lap2 = (lap2 / 60) * FR60 + 200;
  lap3 = (lap3 / 60) * FR60 + 200;
  lap4 = (lap4 / 60) * FR60 + 200;
  lap5 = (lap5 / 60) * FR60 + 200;

  scoreTable->lapTimes[0] = lap1;
  scoreTable->lapTimes[1] = lap2;
  scoreTable->lapTimes[2] = lap3;
  scoreTable->lapTimes[3] = lap4;
  scoreTable->lapTimes[4] = lap5;
}

void FillHighScoreNames(HiScores* scoreTable, const char* n1, const char* n2, const char* n3, const char* n4, const char* n5) {
  strcpy(scoreTable->names[0], n1);
  strcpy(scoreTable->names[1], n2);
  strcpy(scoreTable->names[2], n3);
  strcpy(scoreTable->names[3], n4);
  strcpy(scoreTable->names[4], n5);
}

void HighScore(ConfigData* gameData) {
  BlkFill* clear[2];
  TimData* timInfo;
  POLY_FT4 scorePrim[2];
  POLY_FT4 scorePrim1[2];
  int16_t charX = 40, charY = 185;
  char nameEntered[9];
  char currChar = 'A';
  int16_t charVal = 0, numChars = 0;
  char dispChar[2];
  int32_t toggleDisplay = 0, i, j;
  static char firstUsed = 1;
  char userEntry = 1;
  int32_t bestLap = 0;
  char highScore = 0;
  char checkVal = 0;
  char enterHs = 1;
  int32_t count = 0;
  int32_t padDelay = 0;

  /*	timInfo = Load16BitTexture ("c:/wipeout/textures/hiscore.tim", VRAM) ;
*/

  /*
   clear[ 0 ] = BlockFill( 0, 0, 320, 240, 0x00, 0x00, 0x00 );
   clear[ 1 ] = BlockFill( 0, 240, 320, 240, 0x00, 0x00, 0x00 );

	ClearOTagR( OT[ CurrentScreen ], OT_SIZE );
*/
  if (firstUsed) {
    for (j = 0; j < (HiScoreTracks * 2); j++) {
      for (i = 0; i < NumNames; i++) {
        scoreTable[j].names[i][0] = 0;
        scoreTable[j].lapTimes[i] = 2000000000;
        scoreTable[j].position[i] = 0;
      }
#if 0
			scoreTable[j].lapTimes[0] = 18000 ;
			scoreTable[j].lapTimes[1] = 18100 ;
			scoreTable[j].lapTimes[2] = 18200 ;
			scoreTable[j].lapTimes[3] = 18300 ;
			scoreTable[j].lapTimes[4] = 18400 ;
			strcpy (scoreTable[j].names[0], "NIK") ;
			strcpy (scoreTable[j].names[1], "DJR") ;
			strcpy (scoreTable[j].names[2], "ROB") ;
			strcpy (scoreTable[j].names[3], "JAS") ;
			strcpy (scoreTable[j].names[4], "STU") ;
#endif
    }

    FillHighScores(&(scoreTable[0]), 15270, 16270, 17370, 17670, 18870);
    FillHighScores(&(scoreTable[1]), 9560, 10360, 11460, 12460, 13160);
    FillHighScores(&(scoreTable[2]), 10260, 11360, 12160, 13160, 13960);
    FillHighScores(&(scoreTable[3]), 15080, 15780, 16980, 17680, 18889);
    FillHighScores(&(scoreTable[4]), 14170, 15190, 15740, 16960, 17890);
    FillHighScores(&(scoreTable[5]), 10940, 11780, 12890, 13730, 14660);
    FillHighScores(&(scoreTable[6]), 11724, 12554, 13674, 14394, 14444);
    FillHighScores(&(scoreTable[7]), 12040, 12810, 13720, 14860, 15780);
    FillHighScores(&(scoreTable[8]), 8075, 8820, 9735, 10985, 11895);
    FillHighScores(&(scoreTable[9]), 8525, 9565, 10685, 11415, 12395);
    FillHighScores(&(scoreTable[10]), 13450, 14220, 15450, 16370, 17110);
    FillHighScores(&(scoreTable[11]), 11460, 12220, 13310, 14340, 15270);
    FillHighScores(&(scoreTable[12]), 9400, 10220, 11330, 12060, 13290);
    FillHighScores(&(scoreTable[13]), 9745, 10775, 11685, 12535, 13475);

    FillHighScores(&(scoreTable[14]), 15270, 16270, 17370, 17670, 18870);
    FillHighScores(&(scoreTable[15]), 9560, 10360, 11460, 12460, 13160);
    FillHighScores(&(scoreTable[16]), 10260, 11360, 12160, 13160, 13960);
    FillHighScores(&(scoreTable[17]), 15080, 15780, 16980, 17680, 18889);
    FillHighScores(&(scoreTable[18]), 14170, 15190, 15740, 16960, 17890);
    FillHighScores(&(scoreTable[19]), 10940, 11780, 12890, 13730, 14660);
    FillHighScores(&(scoreTable[20]), 11724, 12554, 13674, 14394, 14444);
    FillHighScores(&(scoreTable[21]), 12040, 12810, 13720, 14860, 15780);
    FillHighScores(&(scoreTable[22]), 8075, 8820, 9735, 10985, 11895);
    FillHighScores(&(scoreTable[23]), 8525, 9565, 10685, 11415, 12395);
    FillHighScores(&(scoreTable[24]), 13450, 14220, 15450, 16370, 17110);
    FillHighScores(&(scoreTable[25]), 11460, 12220, 13310, 14340, 15270);
    FillHighScores(&(scoreTable[26]), 9400, 10220, 11330, 12060, 13290);
    FillHighScores(&(scoreTable[27]), 9745, 10775, 11685, 12535, 13475);

    FillHighScoreNames(&(scoreTable[0]), "WIP", "EOU", "TPC", "NOT", "PSX");
    FillHighScoreNames(&(scoreTable[1]), "AJY", "AJS", "DLS", "MAK", "JED");
    FillHighScoreNames(&(scoreTable[2]), "JD", "AJC", "MSA", "SD", "TIM");
    FillHighScoreNames(&(scoreTable[3]), "POL", "DAR", "JAS", "ROB", "DJR");
    FillHighScoreNames(&(scoreTable[4]), "NIK", "SAL", "DOM", "LG", "LNK");
    FillHighScoreNames(&(scoreTable[5]), "HAN", "PER", "FEC", "TPI", "ZZA");
    FillHighScoreNames(&(scoreTable[6]), "CAN", "WEH", "AVE", "ABO", "NUS");
    FillHighScoreNames(&(scoreTable[7]), "AJY", "DLS", "AJS", "MAK", "JED");
    FillHighScoreNames(&(scoreTable[8]), "BOR", "ING", "HIS", "COR", "ES");
    FillHighScoreNames(&(scoreTable[9]), "AJS", "DLS", "MAK", "JED", "AJY");
    FillHighScoreNames(&(scoreTable[10]), "DLS", "DJR", "LEE", "MOC", "MPI");
    FillHighScoreNames(&(scoreTable[11]), "MAK", "STU", "JAS", "ROB", "DOM");
    FillHighScoreNames(&(scoreTable[12]), "JED", "NCW", "LOU", "DAR", "POL");
    FillHighScoreNames(&(scoreTable[13]), "PC", "POL", "DAR", "DAR", "MSC");
    FillHighScoreNames(&(scoreTable[14]), "MVE", "ALM", "POL", "NIK", "DAR");
    FillHighScoreNames(&(scoreTable[15]), "DAR", "STU", "MOC", "DOM", "NIK");
    FillHighScoreNames(&(scoreTable[16]), "PHO", "ENI", "XR", "ISI", "NG");
    FillHighScoreNames(&(scoreTable[17]), "DOM", "DJR", "MPI", "GOC", "SUE");
    FillHighScoreNames(&(scoreTable[18]), "NIK", "ROB", "AM", "JAS", "DAR");
    FillHighScoreNames(&(scoreTable[19]), "FC", "SUE", "ROB", "JEN", "NT");
    FillHighScoreNames(&(scoreTable[20]), "DJR", "NIK", "JAS", "NCW", "LOU");
    FillHighScoreNames(&(scoreTable[21]), "NCW", "LEE", "STU", "JAS", "ROB");
    FillHighScoreNames(&(scoreTable[22]), "NIK", "POL", "DAR", "STU", "ROB");
    FillHighScoreNames(&(scoreTable[23]), "POL", "JIM", "TIM", "MOC", "PC");
    FillHighScoreNames(&(scoreTable[24]), "TIM", "JIM", "NIK", "JAS", "LG");
    FillHighScoreNames(&(scoreTable[25]), "LG", "LOU", "JIM", "HAN", "NT");
    FillHighScoreNames(&(scoreTable[26]), "STU", "DAV", "DOM", "MOR", "GAN");
    FillHighScoreNames(&(scoreTable[27]), "THA", "NKS", "FOR", "PLA", "YIN");

#if PAL
    scoreTable[0].lapRecord = (4950 / 60) * FR60 + 200;
    scoreTable[1].lapRecord = (3120 / 60) * FR60 + 200;
    scoreTable[2].lapRecord = (3250 / 60) * FR60 + 200;
    scoreTable[3].lapRecord = (4910 / 60) * FR60 + 200;
    scoreTable[4].lapRecord = (4610 / 60) * FR60 + 200;
    scoreTable[5].lapRecord = (3500 / 60) * FR60 + 200;
    scoreTable[6].lapRecord = (3630 / 60) * FR60 + 200;
    scoreTable[7].lapRecord = (3970 / 60) * FR60 + 200;
    scoreTable[8].lapRecord = (2640 / 60) * FR60 + 200;
    scoreTable[9].lapRecord = (2670 / 60) * FR60 + 200;
    scoreTable[10].lapRecord = (4405 / 60) * FR60 + 200;
    scoreTable[11].lapRecord = (3745 / 60) * FR60 + 200;
    scoreTable[12].lapRecord = (3354 / 60) * FR60 + 200;
    scoreTable[13].lapRecord = (3100 / 60) * FR60 + 200;

    scoreTable[14].lapRecord = (4950 / 60) * FR60 + 200;
    scoreTable[15].lapRecord = (3120 / 60) * FR60 + 200;
    scoreTable[16].lapRecord = (3250 / 60) * FR60 + 200;
    scoreTable[17].lapRecord = (4910 / 60) * FR60 + 200;
    scoreTable[18].lapRecord = (4610 / 60) * FR60 + 200;
    scoreTable[19].lapRecord = (3500 / 60) * FR60 + 200;
    scoreTable[20].lapRecord = (3630 / 60) * FR60 + 200;
    scoreTable[21].lapRecord = (3970 / 60) * FR60 + 200;
    scoreTable[22].lapRecord = (2640 / 60) * FR60 + 200;
    scoreTable[23].lapRecord = (2670 / 60) * FR60 + 200;
    scoreTable[24].lapRecord = (4405 / 60) * FR60 + 200;
    scoreTable[25].lapRecord = (3745 / 60) * FR60 + 200;
    scoreTable[26].lapRecord = (3354 / 60) * FR60 + 200;
    scoreTable[27].lapRecord = (3100 / 60) * FR60 + 200;
#else
    scoreTable[0].lapRecord = (4950 / 60) * FR60 + 200;
    scoreTable[1].lapRecord = (3120 / 60) * FR60 + 200;
    scoreTable[2].lapRecord = (3250 / 60) * FR60 + 200;
    scoreTable[3].lapRecord = (4910 / 60) * FR60 + 200;
    scoreTable[4].lapRecord = (4610 / 60) * FR60 + 200;
    scoreTable[5].lapRecord = (3500 / 60) * FR60 + 200;
    scoreTable[6].lapRecord = (3630 / 60) * FR60 + 200;
    scoreTable[7].lapRecord = (3970 / 60) * FR60 + 200;
    scoreTable[8].lapRecord = (2640 / 60) * FR60 + 200;
    scoreTable[9].lapRecord = (2670 / 60) * FR60 + 200;
    scoreTable[10].lapRecord = (4405 / 60) * FR60 + 200;
    scoreTable[11].lapRecord = (3745 / 60) * FR60 + 200;
    scoreTable[12].lapRecord = (3354 / 60) * FR60 + 200;
    scoreTable[13].lapRecord = (3100 / 60) * FR60 + 200;

    scoreTable[14].lapRecord = (4950 / 60) * FR60 + 200;
    scoreTable[15].lapRecord = (3120 / 60) * FR60 + 200;
    scoreTable[16].lapRecord = (3250 / 60) * FR60 + 200;
    scoreTable[17].lapRecord = (4910 / 60) * FR60 + 200;
    scoreTable[18].lapRecord = (4610 / 60) * FR60 + 200;
    scoreTable[19].lapRecord = (3500 / 60) * FR60 + 200;
    scoreTable[20].lapRecord = (3630 / 60) * FR60 + 200;
    scoreTable[21].lapRecord = (3970 / 60) * FR60 + 200;
    scoreTable[22].lapRecord = (2640 / 60) * FR60 + 200;
    scoreTable[23].lapRecord = (2670 / 60) * FR60 + 200;
    scoreTable[24].lapRecord = (4405 / 60) * FR60 + 200;
    scoreTable[25].lapRecord = (3745 / 60) * FR60 + 200;
    scoreTable[26].lapRecord = (3354 / 60) * FR60 + 200;
    scoreTable[27].lapRecord = (3100 / 60) * FR60 + 200;
#endif

    firstUsed = 0;
    return;
  }

  highScore = 0;

  for (j = 0; j < 3; j++) {
    bestLap += tempLapTimes[j];
  }

  for (i = 0; i < NumNames; i++) {
    if (bestLap < scoreTable[(gameData->trackNum - 1)].lapTimes[i]) {
      highScore = 1;
    }
  }

  if (!highScore)
    return;

  nameEntered[0] = 0;

  while (1) {
    textPrim = CurrentScreen * 300;

    ClearOTagR(OT[CurrentScreen], OT_SIZE);
    UpdateNotes(NOT_PAUSED);

    ScreenFooter(70, 210, WhiteText);

    if (numChars != (nameLetters + 1) && (userEntry)) {
      dispChar[0] = currChar;
      dispChar[1] = 0;

      if (userEntry) {
        if ((toggleDisplay % 3) == 0) {
          if (charVal == 26 || charVal == 27)
            AddDelEnd((charVal - 26), text_data(charX, charY, 16), WhiteText);
          else
            AddText(&(dispChar[0]), text_data(charX, charY, 16), WhiteText);
        } else {
          if (charVal == 26 || charVal == 27)
            AddDelEnd((charVal - 26), text_data(charX, charY, 16), RedText);
          else
            AddText(&(dispChar[0]), text_data(charX, charY, 16), RedText);
        }
      }

      charVal = currChar - 'A';

      if ((pad & PadRight0) && (padDelay == 0)) {
        PlayNote(MENU_MOVE, 1, 20);
        padDelay = 5;
        charVal++;
        if (charVal > 27) /* 25 */
        {
          if (numChars != 3)
            charVal = 0;
          else
            charVal = 26;
        }
      }

      if ((pad & PadLeft0) && (padDelay == 0)) {
        PlayNote(MENU_MOVE, 1, 20);
        padDelay = 5;
        charVal--;
        if (numChars != 3)
          checkVal = 0;
        else
          checkVal = 26;

        if (charVal < checkVal) {
          charVal = 27; /* 25 */
        }
      }

      if (padDelay > 0)
        padDelay--;

      if (neg1.head == 8960) {
        if (xpad & NEG_B) {
          PlayNote(MENU_SELECT, 1, 20);
          numChars--;
          charX -= CharSet16[(nameEntered[numChars] - 'A')].width;
          nameEntered[numChars] = 0;
          charVal = 0;
        } else {
          currChar = charVal + 'A';
          if (xpad & NEG_A) {
            nameEntered[numChars] = currChar;
            nameEntered[numChars + 1] = 0;
            numChars++;
            charX += CharSet16[charVal].width;
          }
        }

      } else {
        if ((xpad & PadCross0) && (charVal == 26)) {
          if (numChars >= 1) {
            PlayNote(MENU_SELECT, 1, 20);
            numChars--;
            charX -= CharSet16[(nameEntered[numChars] - 'A')].width;
            nameEntered[numChars] = 0;
            charVal = 0;
          }
        } else {
          currChar = charVal + 'A';
          if ((xpad & PadCross0) && (numChars != (nameLetters + 1)) && (charVal != 26) && (charVal != 27)) {
            PlayNote(MENU_SELECT, 1, 20);
            nameEntered[numChars] = currChar;
            nameEntered[numChars + 1] = 0;
            numChars++;
            charX += CharSet16[charVal].width;
            if (numChars == 3) {
              charVal = 27;
              currChar = ('Z' + 2);
            }
          } else if ((xpad & PadCross0) && (charVal == 27)) {
            nameEntered[numChars] = currChar;
            nameEntered[numChars + 1] = 0;
            userEntry = 0;
          }
        }
      }
    } else {
      if (enterHs) {
        userEntry = 0;
        nameEntered[(numChars)] = 0;
        AddEntry2Table(gameData, bestLap, nameEntered);
        numChars = 0;
        nameEntered[0] = 0;
        charX = 80;
        charVal = 0;
        enterHs = 0;
      }
    }

    /* Display hi-score entries */
    /*
			for (i=0; i < NumNames; i++)
			{
				AddText (&(scoreTable[(gameData->trackNum-1)].names[i][0]), text_data (40, (80+(spacing*i)), 16), WhiteText) ;

				if (scoreTable[(gameData->trackNum-1)].lapTimes[i] == 2000000000)
					AddTime(0,
								 text_data (165, (80+(spacing*i)), 16), RedText);
				else
					AddTime(scoreTable[(gameData->trackNum-1)].lapTimes[i],
							 	text_data (165, (80+(spacing*i)), 16), RedText);
			}

*/
    DisplayHiScore((gameData->trackNum - 1));

    if (userEntry) {
      AddText(&(nameEntered[0]), text_data(40, charY, 16), WhiteText);
      AddTime(bestLap,
              text_data(165, charY, 16), RedText);
    }

    ShowMenuBackground(&(scorePrim[0]), &(scorePrim1[0]), timInfo);

    AddPrim(OT[CurrentScreen], OT_SIZE - 1, (P_TAG*)clear[CurrentScreen]);
    Swap();

    toggleDisplay++;
    firstUsed = 0;

    if (!userEntry) {
      if (count == 200)
        break;

      if (xpad & PadCross0)
        break;

      //			VSync (0) ; // PCwipeout
      count++;
    }
  }

  DrawSync(0);

  return;
}

void AddEntry2Table(ConfigData* gameData, int32_t bestLap, char nameEntered[]) {
  int32_t i, j;
  int32_t scoreTrack = 0;

  //	printf ("track number in add entry to table = %d \n", gameData->trackNum) ;

  if (gameData->gameType == TIME_TRIAL)
    scoreTrack = ((gameData->trackNum - 1) + HiScoreTracks);
  else if (gameData->gameType == CHAMPIONSHIP)
    scoreTrack = ((gameData->trackNum - 1) - gameData->trackInc);
  else
    scoreTrack = (gameData->trackNum - 1);

  //	printf ("Hiscore stored for track  %d \n", scoreTrack) ;

  for (i = 0; i < NumNames; i++) {
    if (bestLap < scoreTable[scoreTrack].lapTimes[i]) {
      if (scoreTable[scoreTrack].position[i] == -1) {
        //				printf ("Position -1 \n") ;

        scoreTable[scoreTrack].position[i] = i;
        scoreTable[scoreTrack].lapTimes[i] = bestLap;
        scoreTable[scoreTrack].lapTimesPilot[i] = gameData->shipNum;
        strcpy(scoreTable[scoreTrack].names[i], nameEntered);
      } else {
        if (i < (NumNames)) {
          for (j = (NumNames - 1); j > i; j--) {
            scoreTable[scoreTrack].position[j] = scoreTable[scoreTrack].position[j - 1];
            scoreTable[scoreTrack].lapTimes[j] = scoreTable[scoreTrack].lapTimes[j - 1];
            strcpy(scoreTable[scoreTrack].names[j], scoreTable[scoreTrack].names[j - 1]);
          }
        }

        scoreTable[scoreTrack].position[i] = j;
        scoreTable[scoreTrack].lapTimes[i] = bestLap;
        strcpy(scoreTable[scoreTrack].names[i], nameEntered);
      }
      break;
    }
  }
}

char CheckLapRecord(ConfigData* gameData, ShipData* shipInfo, int32_t shipNo) {
  int32_t scoreTrack = 0;
  int32_t check = 0;

  if (gameData->gameType == TIME_TRIAL)
    scoreTrack = ((gameData->trackNum - 1) + HiScoreTracks);
  else
    scoreTrack = (gameData->trackNum - 1);

  //	printf ("track = %d \n", scoreTrack) ;
  //	printf ("score table = %d \n", scoreTable[scoreTrack].lapRecord) ;
  //	printf ("lap number = %d \n", shipInfo[ownShip].lapNo) ;
  //	printf ("lap time = %d \n",shipInfo[ownShip].lapTimes[((shipInfo[ownShip].lapNo)-2)]) ;

  if ((raceType == TWO_PLAYER_RACE) || (raceType == HEAD_TO_HEAD)) {
    check = gameData->newLapRecord;

    if (shipInfo[shipNo].lapTimes[((shipInfo[shipNo].lapNo) - 2)] < scoreTable[scoreTrack].lapRecord) {
      scoreTable[scoreTrack].lapRecord = shipInfo[shipNo].lapTimes[((shipInfo[shipNo].lapNo) - 2)];
      scoreTable[scoreTrack].lapRecordPilot = gameData->shipNum;
      if (shipNo == ownShip)
        check = 1;
      else if (shipNo == serialShip)
        check = 0;
    }

    return (check);
  } else {
    if (shipInfo[ownShip].lapTimes[((shipInfo[ownShip].lapNo) - 2)] < scoreTable[scoreTrack].lapRecord) {
      scoreTable[scoreTrack].lapRecord = shipInfo[ownShip].lapTimes[((shipInfo[ownShip].lapNo) - 2)];
      scoreTable[scoreTrack].lapRecordPilot = gameData->shipNum;

      return (1);
    }

    return (0);
  }
}

void DisplayHiScore(char trackNum) {
  char spacing = 20;
  int16_t i;
  static int32_t toggleDisplay = 0;

  /* Display hi-score entries */

  for (i = 0; i < NumNames; i++) {
    if (i == -1) {
      if ((toggleDisplay % 3) == 0) {
        AddText(&(scoreTable[trackNum].names[i][0]), text_data(40, (80 + (spacing * i)), 16), WhiteText);
        AddTime(scoreTable[trackNum].lapTimes[i],
                text_data(165, (80 + (spacing * i)), 16), WhiteText);
      } else {
        AddText(&(scoreTable[trackNum].names[i][0]), text_data(40, (80 + (spacing * i)), 16), RedText);
        AddTime(scoreTable[trackNum].lapTimes[i],
                text_data(165, (80 + (spacing * i)), 16), RedText);
      }
    } else {
      AddText(&(scoreTable[trackNum].names[i][0]), text_data(40, (80 + (spacing * i)), 16), WhiteText);
      AddTime(scoreTable[trackNum].lapTimes[i],
              text_data(165, (80 + (spacing * i)), 16), RedText);
    }
    /*
		if (scoreTable[trackNum].lapTimes[i] == 2000000000)
			AddTime(0,
						 text_data (165, (80+(spacing*i)), 16), RedText);
		else
*/
  }

  toggleDisplay++;
}

#if 1

void NewHighScore(ConfigData* gameData, HiScoreData* hiScoreData) {
  int16_t select;

  if (hiScoreData->numChars != (nameLetters + 1) && (hiScoreData->userEntry)) {
    hiScoreData->dispChar[0] = hiScoreData->currChar;
    hiScoreData->dispChar[1] = 0;

    hiScoreData->charVal = hiScoreData->currChar - 'A';

    if ((pad & PadRight0) && (hiScoreData->padDelay == 0)) {
      ajscount = 0;
      PlayNote(MENU_MOVE, 1, 20);
      hiScoreData->padDelay = 5;
      hiScoreData->charVal++;
      if (hiScoreData->charVal > 27) /* 25 */
      {
        if (hiScoreData->numChars != 3)
          hiScoreData->charVal = 0;
        else
          hiScoreData->charVal = 26;
      }
    }

    if ((pad & PadLeft0) && (hiScoreData->padDelay == 0)) {
      ajscount = 0;
      PlayNote(MENU_MOVE, 1, 20);
      hiScoreData->padDelay = 5;
      hiScoreData->charVal--;
      if (hiScoreData->numChars != 3)
        hiScoreData->checkVal = 0;
      else
        hiScoreData->checkVal = 26;
      if (hiScoreData->charVal < hiScoreData->checkVal) {
        hiScoreData->charVal = 27; /* 25 */
      }
    }

    if (hiScoreData->padDelay > 0)
      hiScoreData->padDelay--;

    select = GetMenuKey(M_SELECT);

    if ((select) && (hiScoreData->charVal == 26)) {
      ajscount = 0;
      if (hiScoreData->numChars >= 1) {
        PlayNote(MENU_SELECT, 1, 20);
        hiScoreData->numChars--;
        hiScoreData->charX -= CharSet16[(hiScoreData->nameEntered[hiScoreData->numChars] - 'A')].width;
        hiScoreData->nameEntered[hiScoreData->numChars] = 0;
        hiScoreData->charVal = 0;
      }
    } else {
      hiScoreData->currChar = hiScoreData->charVal + 'A';
      if ((select) && (hiScoreData->numChars != (nameLetters + 1)) && (hiScoreData->charVal != 26) && (hiScoreData->charVal != 27)) {
        ajscount = 0;
        PlayNote(MENU_SELECT, 1, 20);
        hiScoreData->nameEntered[hiScoreData->numChars] = hiScoreData->currChar;
        hiScoreData->nameEntered[hiScoreData->numChars + 1] = 0;
        hiScoreData->numChars++;
        hiScoreData->charX += CharSet16[hiScoreData->charVal].width;
        if (hiScoreData->numChars == 3) {
          hiScoreData->charVal = 27;
          hiScoreData->currChar = ('Z' + 2);
        }
      } else if ((select) && (hiScoreData->charVal == 27)) {
        ajscount = 0;
        PlayNote(MENU_SELECT, 1, 20);
        hiScoreData->nameEntered[hiScoreData->numChars] = hiScoreData->currChar;
        hiScoreData->nameEntered[hiScoreData->numChars + 1] = 0;
        hiScoreData->userEntry = 0;
        pad = 0;
      }
    }
  } else {
    if (hiScoreData->enterHs) {
      hiScoreData->userEntry = 0;
      hiScoreData->nameEntered[(hiScoreData->numChars)] = 0;
      AddEntry2Table(gameData, hiScoreData->bestLap, hiScoreData->nameEntered);
      hiScoreData->numChars = 0;
      hiScoreData->nameEntered[0] = 0;
      hiScoreData->charX = 80;
      hiScoreData->charVal = 0;
      hiScoreData->enterHs = 0;
    }
  }

  if (!hiScoreData->userEntry) {
    if (hiScoreData->count > 2) {
      if (GetMenuKey(M_SELECT) || (ajscount > AJSDELAY)) {
        ajscount = 0;
        hiScoreData->displayHiScore = 0;
        hiScoreData->charX = 40;
        hiScoreData->count = 0;
        hiScoreData->bestLap = 0;
        return;
      }
    }
    hiScoreData->count++;
  }

  if (ajscount > AJSDELAY) {
    ajscount = 0;
    hiScoreData->userEntry = 0;
    hiScoreData->count = 3;
  }

  ajscount++;
}

void NewHighScoreDraw(ConfigData* gameData, HiScoreData* hiScoreData) {
  int16_t charY = 185;

  if (hiScoreData->numChars != (nameLetters + 1) && (hiScoreData->userEntry)) {
    if (hiScoreData->userEntry) {
      if ((hiScoreData->toggleDisplay % 3) == 0) {
        if (hiScoreData->charVal == 26 || hiScoreData->charVal == 27)
          AddDelEnd((hiScoreData->charVal - 26), text_data(hiScoreData->charX, charY, 16), WhiteText);
        else
          AddText(&(hiScoreData->dispChar[0]), text_data(hiScoreData->charX, charY, 16), WhiteText);
      } else {
        if (hiScoreData->charVal == 26 || hiScoreData->charVal == 27)
          AddDelEnd((hiScoreData->charVal - 26), text_data(hiScoreData->charX, charY, 16), RedText);
        else
          AddText(&(hiScoreData->dispChar[0]), text_data(hiScoreData->charX, charY, 16), RedText);
      }
    }

    if (hiScoreData->userEntry) {
      AddText(&(hiScoreData->nameEntered[0]), text_data(40, charY, 16), WhiteText);
      AddTime(hiScoreData->bestLap,
              text_data(165, charY, 16), RedText);
    }

    hiScoreData->toggleDisplay++;
  }
  CentreText("HALL OF FAME", text_data(107, 20, 16), WhiteText);

  if (gameData->gameType == TIME_TRIAL)
    DisplayHiScore(((gameData->trackNum - 1) + HiScoreTracks));
  else if (gameData->gameType == CHAMPIONSHIP)
    DisplayHiScore((gameData->trackNum - 1) - gameData->trackInc);
  else
    DisplayHiScore((gameData->trackNum - 1));
}

int32_t CheckHiScore(ConfigData* gameData, HiScoreData* hiScoreData) {
  int32_t i, j;
  int32_t highScore = 0;
  int32_t scoreTrack = 0;
  int32_t serialRaceTime = 0, ownRaceTime = 0;
  char whichRecord = 0;

  if (gameData->gameType == TIME_TRIAL)
    scoreTrack = ((gameData->trackNum - 1) + HiScoreTracks);
  else if (gameData->gameType == CHAMPIONSHIP)
    scoreTrack = ((gameData->trackNum - 1) - gameData->trackInc);
  else
    scoreTrack = (gameData->trackNum - 1);

  highScore = 0;
  hiScoreData->bestLap = 0;

  if (gameType == SINGLE) {
    for (j = 0; j < 3; j++) {
      hiScoreData->bestLap += tempLapTimes[j];
    }

    for (i = 0; i < NumNames; i++) {
      if (hiScoreData->bestLap < scoreTable[scoreTrack].lapTimes[i]) {
        highScore = 1;
      }
    }
  } else if ((gameType == MASTER) || (gameType == SLAVE)) {
    for (j = 0; j < 3; j++) {
      ownRaceTime += tempLapTimes[j];
      serialRaceTime += serialLapTimes[j];
    }

    /*
		printf ("ownRacetime = %d  serRaceTime = %d \n", ownRaceTime, serialRaceTime) ;
*/
    if (ownRaceTime > serialRaceTime) {
      hiScoreData->bestLap = serialRaceTime;
      whichRecord = 1;
    } else {
      hiScoreData->bestLap = ownRaceTime;
    }

    /*
		printf ("Best lap = %d \n", hiScoreData->bestLap) ;
*/

    for (i = 0; i < NumNames; i++) {
      if (hiScoreData->bestLap < scoreTable[scoreTrack].lapTimes[i]) {
        if (whichRecord == 0)
          highScore = 1;
      }
    }
  }

  return (highScore);
}

void InitHiScoreData(HiScoreData* hiScoreData) {
  hiScoreData->nameEntered[0] = 0;
  hiScoreData->currChar = 'A';
  hiScoreData->charVal = 0;
  hiScoreData->numChars = 0;
  hiScoreData->dispChar[0] = 0;
  hiScoreData->toggleDisplay = 0;
  hiScoreData->userEntry = 1;
  hiScoreData->checkVal = 0;
  hiScoreData->enterHs = 1;
  hiScoreData->count = 0;
  hiScoreData->padDelay = 0;
  hiScoreData->bestLap = 0;
  hiScoreData->highScore = 0;
  hiScoreData->displayHiScore = 0;
  hiScoreData->charX = 40;
}

#endif
