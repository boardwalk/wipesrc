#ifndef __CGL_DEF__
#define __CGL_DEF__

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

/////////////////////////////////////////////////////////////////////////
//
//   @@ File contents
//
//   Filename      :  cgldef.h
//
//   Description   :  This file includes all the constants and
//                    macros definition used.
//
//   Author        :  P.H.Teo
//
//   BaseLine      :  1 December 1994 (Rev 0.0.1)
//
//   History       :  Nil
//
//   @@ End contents
//
/////////////////////////////////////////////////////////////////////////

//====================//
// CGL_SendStream     //
//====================//

////////////////////////
// Stream Handling    //
////////////////////////

/////////////////////
//  Operation Type //
/////////////////////
#define CGL_RENDER                 1
#define CGL_COPY                   2

//////////////////////
// Primitive Types  //
//////////////////////           

#define CGL_POINT                  (CGL_INT16) 0x0000
#define CGL_LINE                   (CGL_INT16) 0x0100
#define CGL_LINE_STRIP             (CGL_INT16) 0x0101
#define CGL_LINE_LOOP              (CGL_INT16) 0x0102
#define CGL_TRIANGLE               (CGL_INT16) 0x0200
#define CGL_TRIANGLE_STRIP         (CGL_INT16) 0x0201
#define CGL_TRIANGLE_FAN           (CGL_INT16) 0x0202
#define CGL_QUAD                   (CGL_INT16) 0x0300
#define CGL_QUAD_STRIP             (CGL_INT16) 0x0301
#define CGL_RECTANGLE              (CGL_INT16) 0x0400
#define CGL_SPAN_POINT             (CGL_INT16) 0x0500
#define CGL_SPAN_LINE              (CGL_INT16) 0x0501



/////////////////////////
// Primitive Property  //
/////////////////////////

/////////////////////
// Alpha Blend     //
/////////////////////

// blend operation is 84 (0x54)

#define CGL_BLEND_SRC              4
#define CGL_BLEND_DST              5


//////////////////
//  Shading     //
//////////////////
#define CGL_GOURAUD                1
#define CGL_FLAT                   0

//////////////////////////
//  Stencil Read/Write //
/////////////////////////

#define CGL_WRITE_ENABLE           1
#define CGL_READ_ENABLE            2


//===================//
//  CGL_InitScreen   //
//===================//

///////////////////////
//  Screen Reference //
///////////////////////

#define CGL_TOP_LEFT               0
#define CGL_BOTTOM_LEFT            1

///////////////////////
//  Screen Mode      //
///////////////////////

// multisync monitor required
#define CGL_M640x480x75Hz          2
#define CGL_M800x600x75Hz          3
#define CGL_M1024x768x75Hz         4
#define CGL_M1280x1024x75Hz        5
// standard SVGA range
#define CGL_M640x480x72Hz          6
#define CGL_M800x600x56Hz          7
#define CGL_M800x600x60Hz          8
#define CGL_M800x600x72Hz          9
#define CGL_M1024x768x60Hz         10
#define CGL_M1280x1024x57Hz        11

// standard VGA range 
#define CGL_M640x480x60Hz          13
#define CGL_M640x400x70Hz          14
#define CGL_M640x350x70Hz          15 
 
///////////////////////
//  Color Order      //
///////////////////////
#define CGL_RGB_MODE               1
#define CGL_BGR_MODE               0
                                       
                                       
///////////////////////
// Color Format      //
///////////////////////

#define CGL_CI8                    14
#define CGL_RGB232                 11
#define CGL_RGB2321                9
#define CGL_RGB332                 5
#define CGL_RGB4444                3
#define CGL_RGB5551                1
#define CGL_RGB8888                0

/////////////////////////////
// Single/Double Buffering //
/////////////////////////////

#define CGL_SINGLE                 1
#define CGL_DOUBLE                 2

/////////////////////////////
//  Stencil/Depth          //
/////////////////////////////

#define CGL_D0S0                   0
#define CGL_D16S0                  1
#define CGL_D15S1                  2


//===================================//
// cglSetDepthMode/cglSetStencilMode //
//===================================//

/////////////////////////////
//  Depth/Stencil Test     //
/////////////////////////////
                                                        
#define CGL_NEVER                  0
#define CGL_LESS                   1
#define CGL_EQUAL                  2
#define CGL_LESS_OR_EQUAL          3
#define CGL_GREATER                4
#define CGL_NOT_EQUAL              5
#define CGL_GREATER_OR_EQUAL       6
#define CGL_ALWAYS                 7

#define CGL_KEEP                   0
#define CGL_RESET                  1
#define CGL_REPLACE                2
#define CGL_INVERT                 5

///////////////////////
// Buffer Type/Mask  //
///////////////////////

#define CGL_FRAME_BUFFER           0x10
#define CGL_DEPTH_BUFFER           0x08
#define CGL_STENCIL_BUFFER         0x04

///////////////////////////
// Logical Operation     //
///////////////////////////

#define CGL_LOGICOP_CLEAR          0
#define CGL_LOGICOP_AND            1
#define CGL_LOGICOP_AND_NOT        2
#define CGL_LOGICOP_COPY           3
#define CGL_LOGICOP_NOT_AND        4
#define CGL_LOGICOP_NOP            5
#define CGL_LOGICOP_XOR            6
#define CGL_LOGICOP_OR             7
#define CGL_LOGICOP_NOR            8
#define CGL_LOGICOP_NOT_XOR        9
#define CGL_LOGICOP_INVERT         10
#define CGL_LOGICOP_OR_NOT         11
#define CGL_LOGICOP_NOT            12
#define CGL_LOGICOP_NOT_OR         13
#define CGL_LOGICOP_NAND           14
#define CGL_LOGICOP_SET            15
                   
//===========================//
// CGL_SetTextureFilter      //
//===========================//

#define CGL_TEXTURE_DISABLE_FILTER  1
#define CGL_TEXTURE_NORMAL_FILTER   2
#define CGL_TEXTURE_ZOOM_FILTER     3

#define CGL_MODULATE_TEXTURE        0
#define CGL_DECAL_TEXTURE           1
#define CGL_COPY_TEXTURE            3

#define CGL_TEXTURE_CLAMP           0
#define CGL_TEXTURE_REPEAT          1
#define CGL_TEXTURE_MIRROR          2

//===========================//
// CGL_SetPickRegion         //
// CGL_SetExtentRegion       //
//===========================//

///////////////////////////////
//  Query Control Parameters //
///////////////////////////////


#define CGL_INSIDE_REGION           0x0
#define CGL_OUTSIDE_REGION          0x1

#define CGL_EXCLUDE_CULL_PIXEL      0x0
#define CGL_INCLUDE_CULL_PIXEL      0x2

#define CGL_EXCLUDE_DRAWN_PIXEL     0x4
#define CGL_INCLUDE_DRAWN_PIXEL     0x0
       
#define CGL_EAGLE1_REV01            0x0001
#define CGL_EAGLE1_REV02            0x0002
#define CGL_GLINT300SX_REV01        0x0100
#define CGL_GLINT300SX_REV02        0x0200

//////////////////////////////
// MISCELLANEOUS            //
//////////////////////////////       

#define CGL_ENABLE                  1
#define CGL_DISABLE                 0

#endif // __CGL_DEF__
