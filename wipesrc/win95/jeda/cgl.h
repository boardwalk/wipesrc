#ifndef __CGL__
#define __CGL__


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

///////////////////////////////////////////////////////////////////////////
//
//   @@ File contents
//
//   Filename      :  cgl.h
//
//   Description   :  This file includes the function prototypes of all
//                    the graphic library supported.  
//
//                    It must be included by the application software
//                    to access the creative graphic library.
//
//   Author        :  P.H.Teo
//
//   BaseLine      :  1 December 1994 (Rev 0.0.1)
//
//   History       :  Nil
//
//   @@ End contents
//
///////////////////////////////////////////////////////////////////////////

#include "cgltype.h"
#include "cglerr.h"
#include "cgldef.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////
// Prototypes declaration  //
/////////////////////////////



////////////////////////////////////////////////////////////////////
// Primitive Operation Function                                   //
////////////////////////////////////////////////////////////////////
//                                                                //
// cglSendStream    - Function to perform block operation for     //
//                    render, copy, upload and download           //
// cglGetPixel      - Function to get pixel color                 //
// cglPutPixel      - Function to put pixel color                 //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

extern CGL_INT16 CGLAPI cglSendStream(CGL_PTR, CGL_UINT16);
extern CGL_INT16 CGLAPI cglGetPixel(CGL_UINT16, CGL_UINT16, CGL_COLOR_PST);
extern CGL_INT16 CGLAPI cglPutPixel(CGL_UINT16, CGL_UINT16 ,CGL_COLOR_PST);



////////////////////////////////////////////////////////////////////
// Screen/Window Functions                                        //
////////////////////////////////////////////////////////////////////
//                                                                //
// cglInitScreen     - Initialise screen device &  configuration  //
// cglQueryScreen    - Query for valid screen configuration       //
// cglClearScreen    - Clear thes screen                          //
// cglCloseScreen    - Close the screen device                    //
//                                                                //
// cglSetWindowClip  - Set the clipping window within the screen  //
//                                                                //
////////////////////////////////////////////////////////////////////

extern CGL_INT16 CGLAPI cglInitScreen(CGL_SCREEN_PST);
extern CGL_INT16 CGLAPI cglQueryScreen(CGL_SCREEN_PST);
extern CGL_INT16 CGLAPI cglClearScreen(void);
extern CGL_INT16 CGLAPI cglCloseScreen(void);
extern CGL_INT16 CGLAPI cglSetWindowClip(CGL_RECT_PST);

////////////////////////////////////////////////////////////////////
// Primitive Property Functions                                   //
////////////////////////////////////////////////////////////////////
//                                                                //
//  cglSetLineStyle       - Set the line style pattern for line   //
//                          primitive                             //
//  cglSetFillPattern     - Set the fill area pattern for area    //
//                          primitive                             //
//  cglSetDepthMode       - Set the depth comparision mode for    //
//                          depth test.                           //
//  cglSetStencilMode     - Set the stencil comparision and       //
//                          update methods for stencil test.      //
//  cglSetLogicalOpsMode  - Set the comparision mode for logical  //
//                          operation for frame buffer            //
//  cglSetDitherMode      - Enable or disable dither function     //
//                                                                //
////////////////////////////////////////////////////////////////////

extern CGL_INT16 CGLAPI cglSetLineStyle(CGL_PTR, CGL_UINT32);
extern CGL_INT16 CGLAPI cglSetFillPattern(CGL_PTR, CGL_UINT32);
extern CGL_INT16 CGLAPI cglSetDepthMode(CGL_UINT16);
extern CGL_INT16 CGLAPI cglSetStencilMode(CGL_UINT16,CGL_UINT16,
                                          CGL_UINT16,CGL_UINT16);
extern CGL_INT16 CGLAPI cglSetLogicalOpsMode(CGL_UINT16);
extern CGL_INT16 CGLAPI cglSetDepthCueColor(CGL_COLOR_PST);
extern CGL_INT16 CGLAPI cglSetDitherMode(CGL_UINT16);


///////////////////////////////////////////////////////////////////
// Texture Mangement Function                                    //
///////////////////////////////////////////////////////////////////
//                                                               //
//  cglLoadTextureMap     - Load the texture into free memory    //
//                          space.                               //
//                                                               //
//  cglTextureFilterMode  - Set the filter mode for texture      //
//                          mapping.                             //
//                                                               //
///////////////////////////////////////////////////////////////////

extern CGL_INT16 CGLAPI cglLoadTextureMap(CGL_TEXTURELD_PST);
extern CGL_INT16 CGLAPI cglSetTextureFilter(CGL_UINT16);




//////////////////////////////////////////////////////////////////          
// Buffer Management Function                                   //
//////////////////////////////////////////////////////////////////
//                                                              //
//  cglSetConstant        - Set the constant value for clear    //
//                          buffers.                            //
//  cglClearBuffer        - Clear multiple buffers to constant  //
//                          value respectively                  //
//  cglSwapBuffer         - Swap buffers for double buffering   //
//                                                              //
//////////////////////////////////////////////////////////////////


extern CGL_INT16 CGLAPI cglClearBuffer(CGL_UINT32,CGL_RECT_PST);
extern CGL_INT16 CGLAPI cglSetConstant(CGL_COLOR_PST,CGL_UINT32,CGL_UINT32);
extern CGL_INT16 CGLAPI cglSwapBuffer(void);


//////////////////////////////////////////////////////////////////
// Color Management Function                                    //                             
//////////////////////////////////////////////////////////////////
//                                                              //
// cglUpLoad         - Retrieve data to host buffer             //
// cglDownLoad       - Send data from host buffer               //
// cglSetPalette     - Set the color palette table              //
// cglGetPalette     - Retrieve color data from palette table   //
//                                                              //
////////////////////////////////////////////////////////////////// 

extern CGL_INT16 CGLAPI cglUpLoad(CGL_UPDN_PST);
extern CGL_INT16 CGLAPI cglDownLoad(CGL_UPDN_PST);
extern CGL_INT16 CGLAPI cglSetPalette(CGL_PTR, CGL_UINT16, CGL_UINT16, 
                                      CGL_UINT16);
extern CGL_INT16 CGLAPI cglGetPalette(CGL_PTR, CGL_UINT16, CGL_UINT16,
                                      CGL_UINT16);


//////////////////////////////////////////////////////////////////
// Querey Control Function                                      //
//////////////////////////////////////////////////////////////////
//                                                              //
//  cglSetPickRegion   - Set the condition and region of        //
//                       interest to pick                       //
//  cglSetExtentRegion - Set the extent condition               // 
//  cglGetPickResult   - Get the result of picking              //
//  cglGetExtentRegion - Get the extented region                //
//  cglGetConfig       - Get the version of software and        //
//                       hardware supported information         //
//                                                              //
//////////////////////////////////////////////////////////////////

extern CGL_INT16 CGLAPI cglSetPickRegion(CGL_PICK_PST);
extern CGL_INT16 CGLAPI cglGetPickResult(CGL_UINT8P);
extern CGL_INT16 CGLAPI cglSetExtentRegion(CGL_EXTENT_PST);
extern CGL_INT16 CGLAPI cglGetExtentResult(CGL_RECT_PST);
extern CGL_INT16 CGLAPI cglGetConfig(CGL_GPCONFIG_PST);

#ifdef __cplusplus
}
#endif

#endif  // __CGL_
