#ifndef __CGL_ERR__
#define __CGL_ERR__

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
//   Filename      :  cglerr.h
//   Description   :  This file define all the error code for
//                    function calls.
//                    Error code is in decimal value.
//
//   BaseLine      :  1 December 1994 (Rev 0.0.1)
//
//   History       :  Nil
//
//   @@ End contents
//
/////////////////////////////////////////////////////////////////////////
                                                                     
enum {

 CGL_SUCCESS = 0,
 
/////////////////////////////////////
// Hardware Related Error          //
/////////////////////////////////////
CGL_PCI_BIOS32_NOT_FOUND,
CGL_PCI_BIOS_NOT_PRESENT,
CGL_GLINT_NOT_FOUND,
CGL_GLINT_GENERAL_FAILURE,
CGL_INVALID_VENDOR_ID,
CGL_INVALID_DEVICE_ID,
CGL_INVALID_REVISION_ID,
CGL_INVALID_CLASS_CODE,
CGL_SET_GLINT_CONTROL_SPACE_ADDR_FAILED, 
CGL_SET_GLINT_LOCAL_BUFFER_0_ADDR_FAILED,    
CGL_SET_GLINT_FRAME_BUFFER_0_ADDR_FAILED,     
CGL_SET_GLINT_LOCAL_BUFFER_1_ADDR_FAILED,      
CGL_SET_GLINT_FRAME_BUFFER_1_ADDR_FAILED,      
CGL_SET_GLINT_EXPANSION_ROM_ADDR_FAILED,
CGL_SET_CFGCOMMAND_REGISTER_FAILED,
CGL_READ_GLINT_CONTROL_SPACE_ADDR_FAILED, 
CGL_READ_GLINT_LOCAL_BUFFER_0_ADDR_FAILED,    
CGL_READ_GLINT_FRAME_BUFFER_0_ADDR_FAILED,     
CGL_READ_GLINT_LOCAL_BUFFER_1_ADDR_FAILED,      
CGL_READ_GLINT_FRAME_BUFFER_1_ADDR_FAILED,      
CGL_READ_CFGCOMMAND_REGISTER_FAILED,
CGL_MAP_GLINT_CONTROL_SPACE_ADDR_FAILED, 
CGL_MAP_GLINT_LOCAL_BUFFER_0_ADDR_FAILED,    
CGL_MAP_GLINT_FRAME_BUFFER_0_ADDR_FAILED,     
CGL_MAP_GLINT_LOCAL_BUFFER_1_ADDR_FAILED,      
CGL_MAP_GLINT_FRAME_BUFFER_1_ADDR_FAILED,      
CGL_MAP_GLINT_EXPANSION_ROM_ADDR_FAILED,
CGL_SCREEN_RESOLUTION_NOT_SUPPORTED,
CGL_COLOR_RESOLUTION_NOT_SUPPORTED,


/////////////////
// Reserved    //
/////////////////

CGL_RESERVED                = 512,


/////////////////////////////////////
// Library Related Error           //
/////////////////////////////////////

CGL_UNUSED                  = 1000,
 
/////////////////////////////////////
// Primitive Operation Management  //
/////////////////////////////////////

// cglSendStream

CGL_ERR_STREAM_BEGIN_NULL,              // stream buffer pointer error
CGL_ERR_STREAM_TEXTURE_NULL,            // error in Texture Vertex
CGL_ERR_STREAM_VERTEX_NULL,             // error in Spatial Vertex
CGL_ERR_STREAM_COLOR_NULL,              // error in Color Vertex
CGL_ERR_STREAM_DEPTHCUE_NULL,           // error in DepthCue Vertex
CGL_ERR_NOT_CONFIGURE_FOR_DEPTH,        // error in Screen Configuration
CGL_ERR_NOT_CONFIGURE_FOR_STENCIL,      // error in Screen Configuration
CGL_ERR_PROPERTYMASK_TEXTUREENV_NULL,   // error in TextureEnv Structure
CGL_ERR_PROPERTYMASK_DEPTH,             // error in Depth Structure
CGL_ERR_PROPERTYMASK_STENCIL,           // error in Stencil Structure
CGL_ERR_PROPERTYMASK_BLEND,             // error in Blend Strcture
CGL_ERR_STREAM_VERTICE_NO,              // error in No of Vertices
CGL_ERR_OPERATION,                      // Invalid operation

CGL_ERR_STREAM_COPYFIX_NULL      = 1100, // null pointer for fix header
CGL_ERR_STREAM_UPLOADFIX_NULL,           // null pointer for fix header
CGL_ERR_STREAM_DOWNLOADFIX_NULL,          // null pointer for fix header
CGL_ERR_UPLOAD_PRIMITIVE_NOT_SUPPORT,    // primitive not supported
CGL_ERR_DOWNLOAD_PRIMITIVE_NOT_SUPPORT,  // primitive not supported

// cglPutPixel & cglGetPixel  (CGL_ERR_MUST_INIT_SCREEN)

// cglInitScreen

CGL_ERR_INIT_NOT_ENOUGH_MEMORY   = 1200, // Graph Memory insufficient
CGL_ERR_INIT_NOT_ENOUGH_DRAM,            // DRAM insufficient
CGL_ERR_INIT_NOT_ENOUGH_VRAM,            // VRAM insufficient
CGL_ERR_SCREEN_MODE,                     // Invalid screen mode
CGL_ERR_DEPTH_STENCIL_OPTION,            // Invalid configure option
CGL_ERR_COLOR_FORMAT,                    // Invalid color format
CGL_ERR_UNKNOWN_HARDWARE,                // Unable to init hardware

// cglSetLinePattern

CGL_ERR_SET_LINE_PATTERN          = 1300, // insufficient memory to
                                          // save the line pattern
// cglSetFillPattern

CGL_ERR_SET_FILL_PATTERN          = 1400, // insufficient memory to 
                                          // save the fill pattern

// cglSetDepthMode

CGL_ERR_NO_DEPTH_BUFFER           = 1500, // no depth buffer 

// cglSetStencilMode

CGL_ERR_NO_STENCIL_BUFFER         = 1600, // no stencil buffer

// cglSetTextureFilter

CGL_ERR_SET_TEXTURE_FILTER        = 1700, // error in filter selection

// cglLoadTextureMap  

CGL_ERR_TEXTURE_INVAID_HOSTBUFFER   = 1800,    // Null pointer
CGL_ERR_TEXTURE_INVALID_SIZE,                  // invalid width/height
CGL_ERR_TEXTURE_INVALID_BASEADDR,              // wrong addr
CGL_ERR_TEXTURE_BASEADDR_EXCEED,               // wrong addr

// cglUpLoad

CGL_ERR_UPLOAD_INVALID_SIZE          =  1900,   // invalid buffer size
CGL_ERR_UPLOAD_HOSTBUFFER,                      // invalid host buffer
CGL_ERR_UPLOAD_STENCIL_NOT_AVAILABLE,           // no stencil buffer
CGL_ERR_UPLOAD_DEPTH_NOT_AVAILABLE,              // no depth buffer

// cglDownLoad

CGL_ERR_DOWNLOAD_INVALID_SIZE        =  2000,    // invalid buffer size
CGL_ERR_DOWNLOAD_HOSTBUFFER,                     // invalid host buffer
CGL_ERR_DOWNLOAD_STENCIL_NOT_AVAILABLE,          // no stencil buffer
CGL_ERR_DOWNLOAD_DEPTH_NOT_AVAILABLE,            // no depth buffer
                                                    
// cglGetPickResult & cglGetExtentResult
                                                               
CGL_ERR_NOT_IN_PICK_MODE             =  3000,  
CGL_ERR_NOT_IN_EXTENT_MODE,
CGL_ERR_QUERY_MEMORY_NO_ENOUGH,
CGL_ERR_UNKNOWN_REVISION,
CGL_ERR_UNKNOWN_IN_HARDWARE, 
CGL_ERR_UNABLE_TO_GET_MAX_COLOR,

CGL_ERR_UNKNOWN_GLHW                 = 31999,    // Unknown Hardware Error Detected

// Common Error

CGL_ERR_MUST_INIT_SCREEN             = 32000,    // First must call cglInitScreen

};

#endif // __CGL_ERR__
