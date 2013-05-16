// File: <fontdef.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */
/*this was stolen from Zyda, incase you cant't tell by the comment style*/
/* this is an IRIS-4d program */
/* this is routine fontdef.c

   It defines a new raster character font.
   It reads a specified font from a font file.

   Modified by bcstewar on 4/21/96 to add openGL font routines
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <bstring.h>
#include <string.h>

#include <pf.h>
#include <pfutil.h>
#include <gl.h>

#include "fontdef.h"    /* get the declarations for the font */

#define MAXRASTER 16384   /* max number of raster words available */
                          /* We compute this value in the following fashion:
                             The maxwidth of each char is computed in 16 bit
                             words. That value is multiplied by the maxheight.
                             That value is then multiplied by 128 chars in the
                             set. For example, 48 bit by 48 bit chars need
                             18432 raster spots. 64 bit by 64 bit chars need
                             32768 raster spots.
                          */
                          /* Al Casarez of SGI says there are only 16K words
                             available... */

 Fontchar chars[128];      /* the Font table */

 unsigned short raster[MAXRASTER];  /* the raster defs for this font        */

#ifdef OPENGL
 GLubyte byteRaster[MAXRASTER*2];  //the OPENGL raster defs for this font
#endif
 static pfuXFont        *npsCurXFont;
 static char dftFontName[] ="-*-courier-bold-r-normal--12-*-*-*-m-70-iso8859-1";
 static pfuXFont *fontArray[MAXFONTS];
 static int totalFonts;

 long maxheight;           /* the max pixel height for this font   */

 long maxwidth;            /* the max pixel width for this font */

 long chardefined[128];    /* TRUE if the char is defined, FALSE
                                   otherwise */

 long rptr;                /* the last written spot in array raster */

 unsigned long temp[1000];  /* this array is used to reverse the char
                                    defs. It must equal maxwidth in 16 bit
                                    words times maxheight. 256 is good for
                                    max 64 by 64 chars.*/

void fontdef(long fontnum,char *filename)
/* you select the number you want to call this guy */
/* passed in file name */
{
   long i,j,k;   /* temp loop index */
   long jj;      /* temp loop variable */
   FILE *rfp;           /* file pointer for the font file */
   char charval;        /* temp char value */
   long width,height;   /* size of this bitmap (real size) */
   long xoffset,yoffset;/* xoffset and yoffset for the char*/
   long skipwidth;      /* amount to skip after this char is in */
   char tmp[150];       /* temp char array */
#ifdef IRISGL
   //long xoffset,yoffset;/* xoffset and yoffset for the char*/
   long words;          /* number of words per row */
#else //OPENGL
   //signed char xoffset,yoffset;/* xoffset and yoffset for the char*/
   int bytes;           // number of bytes per row
#endif
   long icnt;           /* temp counter to read in the bitmaps */


   /* clear the char table... */
   for(i=0; i < 128; i=i+1)
   {
      chars[i].offset=0;   /* no space for this char def */
      chars[i].w=0;        /* bitmap is zero in width */
      chars[i].h=0;        /* bitmap is zero in height */
      chars[i].xoff=0;     /* no x offset */
      chars[i].yoff=0;     /* no y offset */
      chars[i].width=0;    /* no skip width */
      chardefined[i] = FALSE;
   }

   /* clear the raster array */
   for(i=0; i < MAXRASTER; i=i+1)
   {
      raster[i]=0;
   }

   /* no max width yet... */
   maxwidth=0;

   /* open the named font file */
   rfp=fopen(filename,"r");

   if(rfp == NULL)
   {
      perror("FONTDEF:");
      printf("FONTDEF: cannot open file %s!\n",filename);
   }

   /* read the max height in pixels */
   fscanf(rfp,"%d",&maxheight);

   /* scan past the end of the line */
   fgets(tmp,150,rfp);

   /* say that we havent used any raster space yet */
   rptr = -1;

   /* read until we run out of file */
   while(TRUE)
   {
      /* get a char def line */
      i=fscanf(rfp,"%c %d %d %d %d %d",&charval,&width,&height,
               &xoffset,&yoffset,&skipwidth);

      /* scan past the end of the line */
      fgets(tmp,150,rfp);

      if(i <= 0)
      {
         /* eof */
         break;
      }

      /* we have a character def... */

      /* do we have a new max width? */
      if(width > maxwidth)
      {
         maxwidth=width;
      }

      /* we have a character def */
      j=charval;

      /* say the char spot is defined */
      chardefined[j]= TRUE;

      chars[j].offset=(unsigned short)rptr+1;   /* start of this bit map */

      chars[j].w=(unsigned char)width;         /* width of this bitmap */

      chars[j].h=(unsigned char)height;        /* height of this bit map */

      chars[j].xoff=(signed char)xoffset;    /* x offset for the char */

      chars[j].yoff=(signed char)yoffset;    /* y offset for the char */

      chars[j].width=(short)skipwidth; /* skip this many pixels after
                                   you draw the char */

      /* we need to read 'height' rows of data.
         the first row we read is the last one to go into
         array raster.
      */
#ifdef IRISGL
      /* compute number of words per row */
      words = ((width-1)/16)+1;

      /* the total space we need is i times height */
      i=words*height;
#else //OPENGL
      // Compute the number of bytes per row needed
      bytes = ((width-1)/8)+1;

      // the total space we need
      i=bytes*height;
#endif

      /* read each row... */
      icnt = -1;

      for(k=0; k < height; k=k+1)
      {

         icnt=icnt+1;

#ifdef IRISGL
         /* we read across the row but its backwards... */
         for(jj=1; jj <= words; jj=jj+1)
         {
            fscanf(rfp,"%4x",&temp[icnt+words-jj]);
         }
         icnt=icnt+words-1;

#else //OPENGL
         // OpenGL is byte delimited so we have do something different
         for(jj=1; jj <= bytes; jj=jj+1)
         {
            fscanf(rfp,"%2x",&temp[icnt+bytes-jj]);
         }
         icnt=icnt+bytes-1;

#endif

         /* skip past end of line */
         fgets(tmp,150,rfp);
      }

#ifdef IRISGL
      /* reverse the values in the temp array */
      for(k=i-1; k >= 0; k=k-1)
      {
         rptr=rptr+1;
         raster[rptr]=(unsigned short)temp[k];
      }
#else //OPENGL
      // Reverse the order of the values in temp array by rows
      for(k=i-1; k >= 0; k=k-1)
      {
         rptr=rptr+1;
         byteRaster[rptr]=(GLubyte)temp[k];
      }
#endif

   }   /* end while there are char defs in the file */

   /* check to see if we wrote past the end of raster... */
#ifdef IRISGL
   if(rptr >= MAXRASTER)
#else //OPENGL
   if(rptr >= MAXRASTER*2)
#endif
   {
      printf("FONTDEF: We have written beyond the end of array raster!\n");
      exit(0);
   }

   /* call routine to set up raster font definition */
   /* fontnum = the font number to use to call up this font.
      maxheight = the max height in pixels of characters in this font.
      128 = the number of characters in this font.
      chars = the character table.
      rptr+1 = the number of words in array raster.
      raster = the bit maps for the chars.
   */
#ifdef IRISGL
   defrasterfont((short)fontnum,(short)maxheight,128,chars,(short)rptr+1,raster);
#else //OPENGL
   fontnum; // To remove compiler warning message
   int ix=0, xoff, yoff; 
   
   // Place font in display list for 
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   fontArray[ICONFONT]=(pfuXFont *)pfMalloc(sizeof(pfuXFont),
                                            pfGetSharedArena());
   fontArray[ICONFONT]->handle = glGenLists(128);
   for (ix = 32; ix < 127; ix++) {
      if(chardefined[ix]) {
         // Need to redefine offset due to char type does not present
         // negative numbers.  This is a hack to make it work.
         if(chars[ix].xoff > 128) {
            xoff = chars[ix].xoff - 256;
         }
         else xoff = chars[ix].xoff;

         if(chars[ix].yoff > 128) {
            yoff = chars[ix].yoff - 256;
         }
         else yoff = chars[ix].yoff;

         glNewList(ix+fontArray[ICONFONT]->handle, GL_COMPILE);
            glBitmap(chars[ix].w, chars[ix].h,
                     float(-xoff), float(-yoff),
                     chars[ix].width, 0.0f, 
                     &byteRaster[chars[ix].offset]); //used to be offset*2
         glEndList();
      }
   }
   fontArray[ICONFONT]->info=(XFontStruct *)pfMalloc(sizeof(XFontStruct),
                                                     pfGetSharedArena()); 
#endif

   fclose(rfp);

}

/* this is file fontdefout.c
   it writes out the font in array raster */

void fontdefout(char filename[])
/* passed in file name */
{

   long i,k;   /* temp loop index */

   long jj;      /* temp loop variable */

   FILE *wfp;           /* file pointer for the font file */

   long charval;        /* temp char value */

   long words;          /* number of words per row */

   long icnt;           /* temp counter to read in the bitmaps */


   /* open the named font file */
   wfp=fopen(filename,"w");

   if(wfp == NULL)
   {
      printf("FONTDEFOUT: cannot open file %s!\n",filename);
      exit(0);
   }

   /* write the max height in pixels */
   fprintf(wfp,"%d\n",maxheight);

   /* for all possible chars...*/
   for(charval=0; charval < 128; charval=charval+1)
   {
      /* is the char defined ? */
      if(!chardefined[charval])
      {
         continue;   /* skip this guy */
      }

      /* we have a character def... */
      /* output the table values to the file */
      fprintf(wfp,"%c %d %d %d %d %d\n",
              charval,chars[charval].w,chars[charval].h,
              chars[charval].xoff,chars[charval].yoff,
              chars[charval].width);

      /* we need to read 'height' rows of data.
         the first row we read is the last one to go into
         array raster.
      */
      /* compute number of words per row */
      words = ((chars[charval].w -1)/16)+1;

      /* the total space we need is i times height */
      i=words*chars[charval].h;

      /* read the words from raster and put into temp reversed */
      icnt=chars[charval].offset -1;

      for(k=i-1; k >=0; k=k-1)
      {
         icnt=icnt+1;
         temp[k]=raster[icnt];
      }

      /* write out each row */
      icnt = -1;

      for(k=0; k < chars[charval].h; k=k+1)
      {

         icnt=icnt+1;

         /* we read across the row but its backwards... */
         for(jj=1; jj <= words; jj=jj+1)
         {
            fprintf(wfp,"%04x",temp[icnt+words-jj]);
         }

         fprintf(wfp,"\n");

         icnt=icnt+words-1;

      }

   }   /* endfor all chars... */

   fclose(wfp);

}

// **************************************************************************
//    OPENGL Font Routines
// **************************************************************************



void
npsLoadXFont(char *fontName, pfuXFont *fnt)
{
    Display *xdisplay;

    xdisplay = pfGetCurWSConnection();
    if (!fontName)
	fontName = dftFontName;
    if (fnt)
    {
	fnt->info = XLoadQueryFont(xdisplay,fontName);
	if (fnt->info == NULL) {
	    pfNotify(PFNFY_WARN, PFNFY_RESOURCE, 
		"pfuLoadXFont - couldn't find font %s at display %s.\n", 
			fontName, DisplayString(xdisplay));
	}
    }
    else
	pfNotify(PFNFY_WARN, PFNFY_USAGE, 
	    "pfuLoadXFont - null pfuXFont*");
}

void
npsMakeXFontBitmaps(pfuXFont *fnt)
{
    Font id;
    unsigned int first, last;
    
    if (fnt && fnt->info)
    {
	id = fnt->info->fid;
//cerr<<"fnt->max/min:"<<fnt->info->max_char_or_byte2<<"/"
//              <<fnt->info->min_char_or_byte2<<endl;
	first = 32; //fnt->info->min_char_or_byte2;  32
	last = 128; //fnt->info->max_char_or_byte2;  128
#ifndef IRISGL /* OPENGL */    
	if (!fnt->handle)
	{
	    fnt->handle = glGenLists((GLuint) last+1);
	    if (fnt->handle == 0) 
	    {
	    pfNotify(PFNFY_WARN, PFNFY_RESOURCE, 
		    "pfuMakeXFontBitmaps - couldn't get %d lists.\n", 
		    last+1);
		return;
	    }
	}
	glXUseXFont(id, first, last-first+1, fnt->handle+first);
        //*height = fnt->info->ascent + fnt->info->descent;
	//*width = fnt->info->max_bounds.width;  
    
#endif /* GL type */
    }
    else
	pfNotify(PFNFY_WARN, PFNFY_USAGE, 
	    "pfuMakeXFontBitmaps - null pfuXFont*");
}

void
npsMakeRasterXFont(char *fontName, pfuXFont *fnt)
{
    if (fnt)
    {
	npsLoadXFont(fontName, fnt);
	//npsCurXFont->info = fnt->info;
	npsMakeXFontBitmaps(fnt);
	//npsCurXFont->handle = fnt->handle;
    }
    else
	pfNotify(PFNFY_WARN, PFNFY_USAGE, 
	    "pfuMakeRasterXFont - null pfuXFont*");
}

int 
npsGetXFontWidth(pfuXFont *f, const char *str)
{
    if (f && f->info)
    {
	int len = strlen(str);
	/*  int w = (len * (f->info->max_bounds.rbearing - f->info->min_bounds.lbearing));  */
	int w = XTextWidth(f->info, str, len);
	return w;
    }
    else
	return 0;
}

int 
npsGetXFontHeight(pfuXFont *f)
{
    if (f && f->info)
    {
	int h =  f->info->ascent + f->info->descent;
	return h;
    }
    else
	return 0;
}

pfuXFont *npsGetCurXFont()
{
   return npsCurXFont;
}

void 
npsSetXFont(pfuXFont *f)
{
    if (f && f->info)
    {
	npsCurXFont = f;
	return;
    } else
	pfNotify(PFNFY_WARN, PFNFY_USAGE, 
	    "pfuSetXFont - pfuXFont has null info pointer.");
}

void 
npsCharPos(float x, float y, float z)
{
#ifdef IRISGL
    cmov(x,y,z);
#else /* OpenGL */
    glRasterPos3f(x,y,z);
#endif /* GL type */
}

void 
npsDrawString(const char *s)
{
#ifdef IRISGL
    charstr((char *)s);
#else /* OpenGL */
    if (!npsCurXFont->handle)
    {
pfNotify(PFNFY_NOTICE, PFNFY_PRINT, 
	"pfuDrawString - bad font handle in pid %d", getpid());
	return;
    }
    glPushAttrib (GL_LIST_BIT);
    glListBase(npsCurXFont->handle);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *)s);
    glPopAttrib ();
#endif /* GL type */
}

void 
npsDrawStringPos(const char *s,float x, float y, float z)
{
#ifdef IRISGL
    cmov(x,y,z);
    charstr((char *)s);
#else /* OpenGL */
    if (!npsCurXFont->handle)
    {
pfNotify(PFNFY_NOTICE, PFNFY_PRINT, 
	"pfuDrawStringPos - bad font handle in pid %d", getpid());
	return;
    }
    glRasterPos3f(x,y,z);
    glPushAttrib (GL_LIST_BIT);
    glListBase(npsCurXFont->handle);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *)s);
    glPopAttrib ();
#endif /* GL type */
}


// npsBuildFonts : Build font array for selection
void npsBuildFonts(float top, float bottom) 
{
   char fontName[255];

   // This is hard coded for the specific fonts needed for NPSNET
   // Default Font
   fontArray[DEFFONT]=(pfuXFont *)pfMalloc(sizeof(pfuXFont),pfGetSharedArena());
   npsMakeRasterXFont(NULL,fontArray[DEFFONT]);

   // Normal Large Text Font
   int font_height = (45.0f * ((top - bottom)/1024.0f));
   sprintf(fontName,"-*-times-bold-r-normal--%d-*-*-*-*-84-iso8859-1",
           font_height);

   fontArray[TEXTFONT]=(pfuXFont *)pfMalloc(sizeof(pfuXFont),pfGetSharedArena());
   npsMakeRasterXFont(fontName,fontArray[TEXTFONT]);

   totalFonts = ICONFONT;
   npsCurXFont = fontArray[DEFFONT];  //Make default font current font
}

// npsFont : make selected font the current font
void npsFont(int selection)
{
   if (selection <= totalFonts) {
      npsSetXFont(fontArray[selection]);
   }
}
