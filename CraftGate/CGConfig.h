
#ifndef _CG_CONFIG_H_
#define _CG_CONFIG_H_

/*
    macro _CG_COMPILE_FILEFINDER

    description:
       compile file finder depends on platform. 
*/
#ifndef _CG_COMPILE_FILEFINDER
#define _CG_COMPILE_FILEFINDER 1
#endif

#ifndef _CG_USE_DUMP
//#define _CG_USE_DUMP 1
#endif

#ifndef _CG_USE_WX_BITMAP
#define _CG_USE_WX_BITMAP 0
#endif

#ifndef _CG_USE_OPENGL && !_CG_USE_WX_BITMAP
#define _CG_USE_OPENGL 1
#endif

#endif