#ifndef __CGL_TYPE__
#define __CGL_TYPE__


/************************************************************************
 *                                                                      *
 *                      Creative Graphic Library                        *
 *                                                                      *
 *             Copyright (C) 1994  Creative Technology Ltd              *
 *                                                                      *
 * This software and its associated documentation contains proprietary, *
 * confidential and trade secret information of Creative Technology Ltd *
 * and except as provided by written agreement with Creative Technolgy  *
 * Ltd.                                                                 *
 *                                                                      *
 * a) no part may be disclosed, distributed, reproduced, transmitted,   *
 *    transcribed, stored in a retrieval system, adapted or translated  *
 *    in any form or for any purpose other than running the             *
 *    supplied programming examples                                     *
 *                                                                      *
 *    and                                                               *
 *                                                                      *
 * b) this software may be used, duplicated and distributed in          *
 *    conjunction with the supplied programming examples provided that  *
 *    the above copyright notice and this statement appear in all       *
 *    copies and supporting documentation, and that the name Creative   *
 *    is not used in advertising or publicity pertaining to the         *
 *    distribution of this software without the express written         *
 *    permission of Creative Technology Ltd.                            *
 *                                                                      *
 *    and                                                               *
 *                                                                      *
 * c) the recipient is not entitled to discover through reverse         *
 *    engineering or reverse compiling or other such techniques or      *
 *    processes the trade secrets contained therein or in the           *
 *    documentation.                                                    *
 *                                                                      *
 * THIS SOFTWARE IS SUPPLIED "AS IS" WITHOUT WARRANTY, IMPLIED OR       *
 * OTHERWISE INCLUDING WITHOUT RESTRICTION THE FITNESS FOR ANY          *
 * PARTICULAR PURPOSE. Creative Technology Ltd SHALL NOT BE LIABLE FOR  *
 * ANY DAMAGES OF ANY KIND HOWEVER CAUSED ARISING OUT OF OR IN          *
 * CONNECTION WITH THE USE, POSSESSION OR PERFORMANCE OF THIS SOFTWARE. *
 *                                                                      *
 ************************************************************************/


///////////////////////////////////////////////////////////////////////
//
//   @@ File contents
//
//   Filename      :  cgltype.h
//
//   Description   :  This file includes all definition of structure
//                    and datatype used.
// 
//   BaseLine      :  1 December 1994
//
//   History       :  10 March 1995  - RGB and Color Index will use
//                                     same CGL_COLOR_ST.
//                                     Red componenet of CGL_COLOR_ST
//                                     will be used as color index.
//
//   @@ End contents
//
////////////////////////////////////////////////////////////////////////


/////////////////////////
// Type Definition     //
/////////////////////////


#define CGLAPI 

#ifdef M_I86
#define PTR           far *
typedef void PTR      CGL_PTR;
#endif

#ifdef __WATCOMC__
#define PTR           *
typedef void PTR     CGL_PTR;
#endif

#define CGL_UINT8     unsigned char
#define CGL_STRING    unsigned char
#define CGL_INT16     short
#define CGL_UINT16    unsigned short
#define CGL_INT32     long
#define CGL_UINT32    unsigned long
#define CGL_FLOAT32   float
#define CGL_FLOAT64   double

#define CGL_UINT8P    CGL_UINT8   PTR
#define CGL_INT16P    CGL_INT16   PTR
#define CGL_UINT16P   CGL_UINT16  PTR
#define CGL_INT32P    CGL_INT32   PTR
#define CGL_UINT32P   CGL_UINT32  PTR
#define CGL_FLOAT32P  CGL_FLOAT32 PTR
#define CGL_FLOAT64P  CGL_FLOAT64 PTR

//////////////////////////
//  Vertices Structure  //
//////////////////////////
 
typedef struct CGL_VERTEX2D
{   CGL_FLOAT32 x,y;
}    CGL_VERTEX2D_ST,
 PTR CGL_VERTEX2D_PST;
                            
typedef struct CGL_VERTEX3D
{   CGL_FLOAT32 x,y,z;
}    CGL_VERTEX3D_ST,
 PTR CGL_VERTEX3D_PST;

typedef struct CGL_SPAN2D
{   CGL_FLOAT32 x;
}    CGL_SPAN2D_ST,
 PTR CGL_SPAN2D_PST;
          
typedef struct CGL_SPAN3D
{   CGL_FLOAT32 x;
    CGL_FLOAT32 z;
}    CGL_SPAN3D_ST,
 PTR CGL_SPAN3D_PST;

typedef struct CGL_TEXTURE2D
{
    CGL_FLOAT32 s,t;
}    CGL_TEXTURE2D_ST,
 PTR CGL_TEXTURE2D_PST;

typedef struct CGL_TEXTURE3D
{
    CGL_FLOAT32 s,t,q;
}    CGL_TEXTURE3D_ST,
 PTR CGL_TEXTURE3D_PST;

typedef struct CGL_COLOR
{
    CGL_UINT8 bRed,bGreen,bBlue,bAlpha;
}    CGL_COLOR_ST,
 PTR CGL_COLOR_PST;


typedef struct CGL_DEPTHCUE
{  CGL_FLOAT32 d;
}    CGL_DEPTHCUE_ST,
 PTR CGL_DEPTHCUE_PST;



////////////////////////////
// Primitive Operation    //
////////////////////////////

//*************************
//  Property Enable Mask
//*************************

union CGL_PROPERTY
{
    CGL_UINT16  i;
    struct {
        unsigned shade          : 1;
        unsigned depth          : 1;
        unsigned stencil        : 1;
        unsigned blend          : 1;
        unsigned texture        : 1;
        unsigned depthcue       : 1;
        unsigned logicalOps     : 1;
        unsigned bitMask        : 1;
        unsigned halfTone       : 1;
        unsigned reserved       : 7;
    } u;
};

//***********************
//  Primitive Property
//***********************

///////////////////////////////
// Stencil Structure         //
///////////////////////////////

typedef struct CGL_STENCIL
{
    CGL_UINT16 writeMask;
    CGL_UINT32 dwStencilRef;
     
}    CGL_STENCIL_ST,
 PTR CGL_STENCIL_PST;
                    
                    
///////////////////////////////
// Depth Structure           //
///////////////////////////////

typedef struct CGL_DEPTH
{
    CGL_UINT16 writeEnable;
    
}    CGL_DEPTH_ST,
 PTR CGL_DEPTH_PST;
        
                        
///////////////////////////////
// Alpha Blending Structure  //
///////////////////////////////

typedef struct CGL_ALPHABLEND
{
     CGL_UINT16 wSrcBlend;
     CGL_UINT16 wDestBlend;
     CGL_UINT8  bSrcAlphaValue;
     
}    CGL_ALPHABLEND_ST,
 PTR CGL_ALPHABLEND_PST;


////////////////////////////////////
// Texture Environment Structure  //
////////////////////////////////////

typedef struct CGL_TEXTUREENV
{
    CGL_UINT32    dwTextureAddress;
    CGL_UINT16    wBufferWidth;
    CGL_UINT16    wWidth;
    CGL_UINT16    wHeight;
    CGL_UINT16    wColorFormat;
    CGL_UINT16    wTextureMode;
    CGL_UINT16    wS_Wrap;
    CGL_UINT16    wT_Wrap;
    CGL_UINT16    wOverlay;
    CGL_UINT16    wChroma;
    CGL_COLOR_ST  stChromaColor;

}     CGL_TEXTUREENV_ST,
  PTR CGL_TEXTUREENV_PST;


//******************************
// Primitive Operation Block
//******************************

/////////////////////////
// Render Fixed Header //
/////////////////////////

typedef struct CGL_RENDERFIX
{
    CGL_UINT16         wOperation;
    CGL_UINT32         dwNoOfVertice;
    CGL_UINT16         wPrimitiveType;
    union CGL_PROPERTY uPropertyEnableMask;
    CGL_DEPTH_PST      pDepth;
    CGL_STENCIL_PST    pStencil;
    CGL_ALPHABLEND_PST pBlend;
    CGL_TEXTUREENV_PST pTextureEnv;
    CGL_PTR            pVertex;
    CGL_PTR            pTexture;
    CGL_PTR            pColor;
    CGL_PTR            pDepthCue;
}     CGL_RENDERFIX_ST,
  PTR CGL_RENDERFIX_PST;
                                             
                                             
///////////////////////
// Copy Fixed Header //
///////////////////////


typedef struct CGL_COPYFIX
{
    CGL_UINT16         wOperation;
    CGL_UINT32         dwNoOfVertice;
    CGL_UINT16         wPrimitiveType;
    CGL_UINT16         wBufferMask;
    CGL_UINT16         wXSrc;
    CGL_UINT16         wYSrc;
    CGL_VERTEX2D_PST   pVertex2D;
}     CGL_COPYFIX_ST,
  PTR CGL_COPYFIX_PST;



/////////////////////////////
//  Texture Management     //
/////////////////////////////

// Texture Loading Structure

typedef struct CGL_TEXTURELD
{
    CGL_UINT32    dwFreeByteAddress;
    CGL_UINT32    dwTextureAddress;
    CGL_UINT16    wBufferWidth;
    CGL_UINT16    wWidth;
    CGL_UINT16    wHeight;
    CGL_UINT16    wDither;
    CGL_UINT16    wColorFormat;
    CGL_UINT32    dwMemoryLeft;
    CGL_PTR       pHostBuffer;
    
}     CGL_TEXTURELD_ST,
  PTR CGL_TEXTURELD_PST;





/////////////////////////
//  Buffer Management  //
/////////////////////////

typedef struct CGL_RECT {
    CGL_UINT16 wX;
    CGL_UINT16 wY;
    CGL_UINT16 wWidth;
    CGL_UINT16 wHeight;
}     CGL_RECT_ST,
  PTR CGL_RECT_PST;


typedef struct CGL_UPDN {
    CGL_UINT16  wBufferType;
    CGL_UINT16  wColorFormat;
    CGL_PTR     pHostBuffer;
    CGL_RECT_ST stBox;
}    CGL_UPDN_ST,
 PTR CGL_UPDN_PST;





////////////////////////////
//  Screen Management     //
////////////////////////////


typedef struct CGL_SCREEN
{                 
  CGL_UINT16 wReference;            
  CGL_UINT16 wMode;
  CGL_UINT16 wColorFormat;
  CGL_UINT16 wColorOrder;
  CGL_UINT16 wBuffers;
  CGL_UINT16 wOption;
  CGL_UINT32 dwUB_BufferSize;
}    CGL_SCREEN_ST,
 PTR CGL_SCREEN_PST;



                                
//////////////////////////////////
// Query Mangement              //
//////////////////////////////////

// Pick/Extent

typedef struct CGL_PICK {
    CGL_RECT_ST   stRect;
    CGL_UINT16    wStatistic;
}     CGL_PICK_ST,
 PTR  CGL_PICK_PST;
  
typedef struct CGL_EXTENT {
    CGL_UINT16    wStatistic;
}     CGL_EXTENT_ST,
 PTR  CGL_EXTENT_PST;


//  Hardware Configuration

union CGL_HARDWARE
{     CGL_UINT32 i;
      struct
      {
      unsigned Texture     : 1;
      unsigned HalfTone    : 1;
      unsigned LineStipple : 1;
      unsigned DepthCue    : 1;
      unsigned Shade       : 1;
      unsigned Overlay     : 1;
      unsigned Chroma      : 1;
      unsigned Dither      : 1;
      unsigned AlphaBlend  : 1;
      unsigned Pick        : 1;
      unsigned Extent      : 1;                                   
      unsigned Bitmask     : 1;
      unsigned LogicOps    : 1;
      unsigned DepthTest   : 1;
      unsigned Clipping    : 1;
      unsigned Stencil     : 1;
      unsigned reserved    : 16;
     } u;
};

typedef struct CGL_GPCONFIG
{
   CGL_STRING          Version[80];
   CGL_UINT16          wBoard_ID;
   CGL_UINT32          dwVRAM_Size;
   CGL_UINT32          dwDRAM_Size;
   CGL_UINT16          wMaxColorFormat;
   CGL_UINT16          wCurrentMode;
   union CGL_HARDWARE  uHardwareSupport;

}     CGL_GPCONFIG_ST,
  PTR CGL_GPCONFIG_PST;
                               


#endif // __CGL_TYPE__
