#include <bstring.h>
#include "bbox.h"

typedef ushort quad_index_list[4];

pfSwitch
*make_bounding_box ( const pfBox *box, int wireframe, pfVec4 *color )
{
   void *shared_arena;
   pfVec3 *vertex_list; // The eight vertices of the bounding box
   quad_index_list *vertex_index; // The list of the indices for the six sides
   pfVec4 *color_list; // The color of the box
   quad_index_list *color_index;
   pfVec3 *normal_list;
   ushort *normal_index;
   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode *geode;
   pfSwitch *the_switch;
   
   shared_arena = pfGetSharedArena();
   
   vertex_list = (pfVec3 *)pfMalloc(8*sizeof(pfVec3), shared_arena);
   if (vertex_list == NULL )
      return NULL;
   pfSetVec3 ( vertex_list[0],
               box->min[PF_X], box->min[PF_Y], box->min[PF_Z] );
   pfSetVec3 ( vertex_list[1],
               box->max[PF_X], box->min[PF_Y], box->min[PF_Z] );
   pfSetVec3 ( vertex_list[2],
               box->max[PF_X], box->min[PF_Y], box->max[PF_Z] );
   pfSetVec3 ( vertex_list[3],
               box->min[PF_X], box->min[PF_Y], box->max[PF_Z] );
   pfSetVec3 ( vertex_list[4],
               box->min[PF_X], box->max[PF_Y], box->min[PF_Z] );
   pfSetVec3 ( vertex_list[5],
               box->max[PF_X], box->max[PF_Y], box->min[PF_Z] );
   pfSetVec3 ( vertex_list[6],
               box->max[PF_X], box->max[PF_Y], box->max[PF_Z] );
   pfSetVec3 ( vertex_list[7],
               box->min[PF_X], box->max[PF_Y], box->max[PF_Z] );
   
   vertex_index = (quad_index_list *)pfMalloc(6*sizeof(quad_index_list), 
                                            shared_arena);
   if ( vertex_index == NULL )
      {
      pfFree(vertex_list);
      return NULL;
      }
   
   vertex_index[0][0] = 0;
   vertex_index[0][1] = 1;
   vertex_index[0][2] = 2;
   vertex_index[0][3] = 3;
   vertex_index[1][0] = 1;
   vertex_index[1][1] = 5;
   vertex_index[1][2] = 6;
   vertex_index[1][3] = 2;
   vertex_index[2][0] = 5;
   vertex_index[2][1] = 4;
   vertex_index[2][2] = 7;
   vertex_index[2][3] = 6;
   vertex_index[3][0] = 4;
   vertex_index[3][1] = 0;
   vertex_index[3][2] = 3;
   vertex_index[3][3] = 7;
   vertex_index[4][0] = 3;
   vertex_index[4][1] = 2;
   vertex_index[4][2] = 6;
   vertex_index[4][3] = 7;
   vertex_index[5][0] = 1;
   vertex_index[5][1] = 0;
   vertex_index[5][2] = 4;
   vertex_index[5][3] = 5;
   
   
   color_list = (pfVec4 *)pfMalloc(1*sizeof(pfVec4), shared_arena);
   
   if ( color_list == NULL)
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      return NULL;
      }
   
   if ( color == NULL )
      {
      // White is the only color we will use
      color_list[0][0] = 1.0f;
      color_list[0][1] = 1.0f;
      color_list[0][2] = 1.0f;
      color_list[0][3] = 1.0f;
      }
   else
      pfCopyVec4 ( color_list[0], *color );
   
   color_index = (quad_index_list *)pfMalloc(6*sizeof(quad_index_list),
                                            shared_arena);
   if ( color_index == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      return NULL;
      }
   
   // All quads will use color 0 which is white
   bzero ( (char *)color_index, 6*sizeof(quad_index_list) );
   
   normal_list = (pfVec3 *)pfMalloc(6*sizeof(pfVec3), shared_arena );
   
   if ( normal_list == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      return NULL;
      }
   
   pfSetVec3 ( normal_list[0], 0.0f, -1.0f, 0.0f );
   pfSetVec3 ( normal_list[1], 1.0f, 0.0f, 0.0f );
   pfSetVec3 ( normal_list[2], 0.0f, 1.0f, 0.0f );
   pfSetVec3 ( normal_list[3], -1.0f, 0.0f, 0.0f );
   pfSetVec3 ( normal_list[4], 0.0f, 0.0f, 1.0f );
   pfSetVec3 ( normal_list[5], 0.0f, 0.0f, -1.0f );
   
   normal_index = (unsigned short *)pfMalloc(5*sizeof(unsigned short),
                                             shared_arena);
   if ( normal_index == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      pfFree(normal_list);
      return NULL;
      }
   
   normal_index[0] = 0;
   normal_index[1] = 1;
   normal_index[2] = 2;
   normal_index[3] = 3;
   normal_index[4] = 4;
   normal_index[5] = 5;
   
   gset = pfNewGSet(shared_arena);
   
   if ( gset == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      pfFree(normal_list);
      pfFree(normal_index);
      return NULL;
      }
   
   /*
   * set the coordinate, normal and color arrays
   * and their cooresponding index arrays
   */
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
              vertex_list, (ushort *)vertex_index);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM, normal_list, normal_index);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_PER_VERTEX, 
              color_list, (ushort *)color_index);
   pfGSetPrimType(gset, PFGS_QUADS);
   pfGSetNumPrims(gset, 6);
   
   /*
   * create a new geostate from shared memory,
   * disable texturing and enable transparency
   */
   gst = pfNewGState(shared_arena);
   
   if ( gst == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      pfFree(normal_list);
      pfFree(normal_index);
      pfFree(gset);
      return NULL;
      }
   
   if ( wireframe )
      {
      pfGStateMode(gst, PFSTATE_ENWIREFRAME, PFTR_ON);
      }
   pfGStateMode(gst, PFSTATE_ENTEXTURE, PFTR_OFF);
   
   
   
   pfGSetGState(gset, gst);
   
   geode = pfNewGeode();
   if ( geode == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      pfFree(normal_list);
      pfFree(normal_index);
      pfFree(gset);
      pfFree(gst);
      return NULL;
      }
   pfAddGSet(geode,gset);
   
   the_switch = pfNewSwitch();
   if ( the_switch == NULL )
      {
      pfFree(vertex_list);
      pfFree(vertex_index);
      pfFree(color_list);
      pfFree(color_index);
      pfFree(normal_list);
      pfFree(normal_index);
      pfFree(gset);
      pfFree(gst);
      pfFree(geode);
      return NULL;
      }
   
   pfAddChild(the_switch,geode);
   pfSwitchVal(the_switch,PFSWITCH_ON);
   
   return the_switch;
   
}
