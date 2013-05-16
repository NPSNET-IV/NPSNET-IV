#include <Performer/pf.h>
#include <stdio.h>
#include <ctype.h>

#include "loadBBoard.h"
    
long		bblength[] = { MAXBVERTS };
pfVec4		BBColors[] = {{1.0, 1.0, 1.0, 1.0}};
pfVec3 zeropos = {0.0f,0.0f,0.0f};

int readBBfile(char *filename,BBflagsEnum *texapp, BBflagsEnum *btype,
               float rotvec[3],float vert[MAXBVERTS][3],
               char texname[],float texcoord[MAXBVERTS][2])
{
   int vc = 0;
   FILE *in;
   char token;
   char dummy[255];
   char wfilename[255];
   
   pfFindFile(filename,wfilename,R_OK);
   
   //read the header file
   if(NULL == (in = fopen(wfilename,"r"))){
     printf("Unable to open Billboard file %s\n",filename);
     return(NULL);
   }
   //defaults
   *texapp = BNONE;
   *btype = BNONE;

   while ( 1 == fscanf(in,"%c",&token)){
     token = (char)toupper((int)token); 
     switch(token){
       case '#': //comment
         fgets(dummy,255,in);
         break;
       case 'M': //Modulate texture
         fscanf(in,"%s %s ",dummy, texname);
         *texapp = MODULATE;
         break;
       case 'D': //Decal texture 
         fscanf(in,"%s %s ",dummy, texname);
         *texapp = DECAL;
         break;
       case 'A': //Axis rotation
         fscanf(in,"%s %f %f %f ",dummy,&rotvec[0],&rotvec[1],&rotvec[2]);
         *btype = AXIS;
         break;
       case 'P': //Point rotation
         fscanf(in,"%s %f %f %f ",dummy,&rotvec[0],&rotvec[1],&rotvec[2]);
         *btype = POINT;
         break;
       case 'V': //Vertex
         if(vc == MAXBVERTS){
           printf("Error in Billboard file %s, too many vertices\n",filename);
           fclose(in);
           return(0);
         }
         fscanf(in," %f %f %f %f %f ",
              &vert[vc][0],&vert[vc][1],&vert[vc][2],
              &texcoord[vc][0],&texcoord[vc][1]);

         vc++;
         break;
       default: //error
         fgets(dummy,255,in);
         printf("Error in Billboard file %s %c%s\n",filename,token,dummy);
         break;
     } 
   }
   fclose(in);
   if(vc != (MAXBVERTS)){
     printf("Error in Billboard file %s, Not enough vertices\n",filename);
     return(0);
   }
   if(*texapp == BNONE){
     printf("Error in Billboard file %s, No Texture / Texture mode defined\n",
          filename);
     return(0);
   }
   if(*btype== BNONE){
     printf("Error in Billboard file %s, No Rotation defined\n",filename);
     return(0);
   }

  //if we are here, it works
  return(1);
}
   
pfGroup * LoadBBoard(char filename[])
{

   pfVec2 *texcoord;
   pfVec3 *vert;
   pfVec3 rotvec;
   BBflagsEnum texapp,btype;
   char texname[255]; 
   char wtexname[255]; 
   pfGeoSet    *gset;
   pfGeoState  *gstate;
   pfTexture   *tex;
   pfTexEnv    *tev;
   pfGroup * bbgroup;
   pfBillboard *bill;

    
   if(NULL == (texcoord = (pfVec2 *)pfMalloc(
           sizeof(pfVec2)*MAXBVERTS,pfGetSharedArena()))){
     printf("Unable to malloc memory for Billboard tex coord\n");
     return(NULL);
   }
   if(NULL == (vert = (pfVec3 *)pfMalloc(
           sizeof(pfVec3)*MAXBVERTS,pfGetSharedArena()))){
     printf("Unable to malloc memory for Billboard vert coord\n");
     return(NULL);
   }
   
   if(!readBBfile(filename,&texapp,&btype,rotvec,vert,texname,texcoord)){
     printf("Error reading Billboard file %s\n",filename);
     return(NULL);
   }

   //read the texture file
   tev = pfNewTEnv (pfGetSharedArena());
   if(texapp == MODULATE)
     pfTEnvMode(tev,PFTE_MODULATE);
   else
     pfTEnvMode(tev,PFTE_DECAL);

   tex = pfNewTex (pfGetSharedArena());
   pfTexFilter(tex,PFTEX_MINFILTER,PFTEX_BILINEAR);
   pfTexFilter(tex,PFTEX_MAGFILTER,PFTEX_POINT);   
   
   pfFindFile(texname,wtexname,R_OK);
   if (pfLoadTexFile (tex, wtexname)) {
     //create a geostate
     gstate = pfNewGState (pfGetSharedArena());
     pfGStateMode (gstate, PFSTATE_ENTEXTURE, 1);
     pfGStateAttr (gstate, PFSTATE_TEXENV, tev);
     pfGStateAttr (gstate, PFSTATE_TEXTURE, tex);
     //set alpha function to block pixels of 0 alpha for transparent textures
     pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
     pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);

   }
   else{
     printf("Unable to load Billboard texture file %s\n",texname);
     return(NULL);
   } 

   //create a geoset
   gset = pfNewGSet(pfGetSharedArena());

   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, vert, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX, texcoord, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, BBColors, NULL);
   pfGSetPrimType(gset, PFGS_QUADS);
   pfGSetNumPrims(gset, 1);
//   pfGSetPrimLengths(gset, bblength);

   //now for a billBoard
   bill = pfNewBboard();
   pfBboardAxis(bill, rotvec);
   if (btype == AXIS){
     pfBboardMode(bill, PFBB_ROT, PFBB_AXIAL_ROT);
   }
   else{
     pfBboardMode(bill, PFBB_ROT, PFBB_POINT_ROT_WORLD);
   }
     

   //put it all together and return
   pfGSetGState (gset, gstate);
   bbgroup = pfNewGroup();
   pfAddGSet(bill, gset);
   pfAddChild(bbgroup,bill);
 //  pfBboardPos(bill,0,zeropos);
   return(bbgroup); 
}

pfBillboard * placeBBoard (pfGroup *root,pfGroup *bboard,float x,float y,float z)
{
  pfGroup *newg;
  pfVec3 pos;
  pfDCS *dcs; 
  
  dcs = pfNewDCS();
  newg = (pfGroup *)pfClone(bboard,0);
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
  
  pfAddChild(root,dcs);
  pfAddChild(dcs,newg);
  pfDCSTrans(dcs, x, y, z);
  //pfBboardPos((pfBillboard *)pfGetChild(newg,0),0, pos);
  pfFlatten(root,0);
  
  return((pfBillboard *)pfGetChild(newg,0));
} 
