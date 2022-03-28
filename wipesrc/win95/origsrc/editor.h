/* the tools */
#define fDynamics    0
#define fTrans       1
#define fBug         2
#define fSky         3
#define fLand        4
#define fSave        5

#define fDumpVram    6

/* the popouts attached to 0 */
#define fCamera      7
#define fTexSelect   8
#define fDpa         9
#define fRadius      10
#define fTrakView    11

/* used in the texture viewer */
#define fEmpty       12



#define numTools        7
#define maxPopouts      5


typedef struct Tools
{
   short       type;                         /* -1 for popout otherwise toggle type */
   short       onOff;                        /* whether icon is toggled on or off for toggle type */

   short       popOut[ maxPopouts ];         /* the popout indices */
   short       currentPopout;                /* the curent selected popout */
   short       poppedOut;                    /* are we popped out or not */
   POLY_GT4    popIcons[ 2 ][ maxPopouts ];  /* the popouts */
   short       numPopouts;                   /* the number of icons that pop out */

   short       leftPos;                      /* position of popped out highlighter ( 0 right ) */
   POLY_G4     poppedHighlight[ 2 ];         /* the popped out highlight */

} Tools;


typedef struct Editor
{
   short       status;                    /* editor on 1 / off 0 */

   short       iconTextures;              /* index into texture table */

   POLY_GT4    icons[ 2 ][ numTools ];    /* the actual icons */
   POLY_G4     selected[ 2 ][ numTools ]; /* the always selected blue overlays */

   short       downPos;                   /* position of highlighter ( 0 top )*/
   POLY_G4     highlight[ 2 ];            /* the cursor highlight */

   Tools       toolBar[ numTools ];

   short       justSaved;                 /* timer for the just saved message */
   short       justDumped;                /* timer for the just dumped vram message */
} Editor;



void InitEditor( Editor *editor );
void EditorUpdate( Editor *editor, char level );
short EditorFlyMode( Editor *editor );
