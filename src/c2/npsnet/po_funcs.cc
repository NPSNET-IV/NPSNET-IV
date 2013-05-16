// File: <po_funcs.cc>
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

//  Modifications:  Major modifications have been made by Sam Kirby.
//  The modification allow the construction and updating of po_measures
//  in real time rather than writing into and out of a file.

#ifndef NOC2MEASURES

#include <fstream.h>
#include <iostream.h>
#include <string.h>
#include "globals.h"
#include "common_defs.h"
#include "po_funcs.h"
#include "picking.h"
#include "po_interface.h"
#include "menu_funcs.h"

#ifdef OPENGL
#include "fontdef.h"
#endif

extern "C" {
#include "libroute.h"
}

float64 POF_nps_northing_shift = 0.0;
float64 POF_nps_easting_shift = 0.0;

// Globals used by this library
extern           pfGroup          *G_measures;
extern volatile  STATIC_DATA      *G_static_data;

// Static local routines
static void toggle_po_display();


// static SP_SimulationAddress slaveSimulator;
                                         //  This captured simulator will
                                         //  be used to handle our po measures.
static SP_SimulationAddress overlayOwner;       //  This is the owner of the 
                                         //  overlay.  I don't want to 
                                         //  own it but I can.

//Overloaded = operator to compare of type poLookUp.  The function acts
//as an assignment operator for poLookUp type.
poLookUp& poLookUp::operator=(const poLookUp& thatCopied){
   key.v.dkey = thatCopied.key.v.dkey;
   poPointer = thatCopied.poPointer;
   return (*this);
}  //  End of poLookUp&.

//  Removes the po_measures tree from the global tree.  It doesn't delete
//  them, just removes from the tree.
void po_hide_measures()
{
   if((L_po_meas == NULL) ){
      //there is no measure file defined
      return;
   }
   if(pfGetNumParents(L_po_meas) != 0)
      pfRemoveChild(G_measures,L_po_meas);
   
}  // End of po_hide_measures.


//Attach the measures to the scene graph.
void po_display_measures()
{

   if(L_po_meas != NULL){
      pfAddChild(G_measures,L_po_meas);
   }
   else{
      cerr<<"The measures tree is empty."<<endl;
   }

}  //  End of po_display_measures.


//SAK.  This function reads in the dtdb. At this point it's hardwired to
//the ctdb file.  In the future it MUST be included in the config file.
//It also initializes the captured simulator to zero.

int initPoStuff ( char *ctdb_filename, unsigned char exercise,
                  unsigned char database, unsigned short port,
                  float64 nps_northing, float64 nps_easting ) {
   char ctdb_filepath[PF_MAXSTRING];
   int success = TRUE;
  
   L_color = (pfVec4 *)pfMalloc(sizeof(pfVec4)*8,pfGetSharedArena()); 
   pfSetVec4(L_color[DEFAULT_M], 0.0f,0.0f,0.0f,1.0f); //default color
   pfSetVec4(L_color[BLACK_M], 0.0f,0.0f,0.0f,1.0f); //black
   pfSetVec4(L_color[YELLOW_M], 1.0f,1.0f,0.0f,1.0f); //Yellow
   pfSetVec4(L_color[RED_M], 1.0f,0.0f,0.0f,1.0f); //Red
   pfSetVec4(L_color[GREEN_M], 0.0f,0.5f,0.0f,1.0f); //Green
   pfSetVec4(L_color[BLUE_M], 0.0f,0.0f,0.4f,1.0f); //Blue.
                                              // Pure blue blends into the sky.
   pfSetVec4(L_color[WHITE_M], 1.0f,1.0f,1.0f,1.0f);     //white
   pfSetVec4(L_color[HIGHLIGHT], 1.0f,0.0f,1.0f,1.0f);     //highlight

   slaveSim.site = 0;                    //  Haven't captured one yet.
   slaveSim.host = 0;

   L_po_meas = pfNewGroup();
   pfNodeName(L_po_meas, "L_po_meas");

//   poProtocol = SP_persistentObjectProtocolVersionDec94;
//   poProtocol = 18;
   poProtocol = SP_persistentObjectProtocolVersionSep95;


   L_textblock = NULL;
   L_textcnt = 0;

   bzero ( (char *)poArray, 1000*sizeof(poLookUp) );
   poCount = 0;
  
   poSite = 99;
   poHost = 1;

   object = 1;

   L_menu_root = NULL;

   if ( exercise < 1 ) {
      L_exercise = 1;
   }
   else {
      L_exercise = exercise;    // excercise ID
   }

   if ( database < 1 ) {
      L_database = 1;
   }
   else {
      L_database = database;    // database ID
   }

   pickedGroup = NULL;

   if ( pfFindFile(ctdb_filename, ctdb_filepath, R_OK) ) {
      ctdb_read(ctdb_filepath, &L_ctdb,0);
   }
   else {
      cerr << "ERROR, File not found: " << ctdb_filename << endl;
      cerr << "\tC2/PO Measures could not open required CTDB file." << endl;
      success = FALSE;
   }

   if ( success ) {
      success = po_net_open(port);
   }


   if ( success ) {
      POF_nps_northing_shift = nps_northing -  L_ctdb.origin_northing;
      POF_nps_easting_shift = nps_easting -  L_ctdb.origin_easting;
      cerr << "\tDatabase origin difference between ModSAF and NPSNET: "
           << POF_nps_northing_shift << "N "
           << POF_nps_easting_shift << "E" << endl;
      cerr << "----------------------------------------" << endl;

      initArrowGsets();
      initMenu();
   }


   return success;

}  //  End of initPoStuff.


//Remove all of the active po measures and delete the tree.
void po_delete()
{
   po_hide_measures();
   pfDelete(L_po_meas);
   L_po_meas = NULL;
}  //  End of po_delete.



int place_text(pfTraverser *, void *data)
//draw the text in the scene
{
  TEXTBLOCK *tblock = (TEXTBLOCK *)data;
  pfPushState();
  pfBasicState();

#ifdef IRISGL
    cmov(tblock->location[X],tblock->location[Y], tblock->location[Z]);
    cpack(tblock->c.color);
    charstr (tblock->text);
#else //OPENGL
    npsFont(DEFFONT);
// Need to create a routine that will convert an unsigned int to 
// four indiviual components of RGBA values
    int rval, gval, bval, aval;
    rval = (tblock->c.color & 0x000000FF);
    gval = (tblock->c.color & 0x0000FF00)>>2;
    bval = (tblock->c.color & 0x00FF0000)>>4;
    aval = (tblock->c.color & 0xFF000000)>>6;
    glColor4i(rval,gval,bval,aval);
    npsDrawStringPos(tblock->text,tblock->location[X],
                                  tblock->location[Y],
                                  tblock->location[Z]);
#endif

  pfPopState();

  //We realy don't have anything other then the text to draw, so prune the tree
  return(PFTRAV_PRUNE);
}  //  End of place_text.


//  This function finds the site/host numbers and which interface this is.
void po_sitehost(char ether_interf[])
{
   char name[80],temp[80];
   int dummy;
   EntityID address;
   int found = FALSE;

   gethostname(name,80);

   ifstream hostfile ((char *)G_static_data->network_file);
   if (!hostfile)
      {
      cerr << "Unable to open network information file for PO:  "
         << (char *)G_static_data->network_file << endl;
      }
   else
      {
      while ( !found && (hostfile >> "HOST" >> temp) )
      {
         if(!strcmp(temp,name))
         {
            /*open the network for this host*/
            hostfile >> address.address.site >> address.address.host 
               >> ether_interf;
            poSite = address.address.site;
            poHost = address.address.host; 
            found = TRUE;
         }
         //trash this puppy
         hostfile >> dummy >> dummy >> temp;
      }
   }

   if ( !found )
   {
      cerr << "PO Host " << name << " was not found in the network" << endl;
      cerr << "\tinformation file, " << (char *)G_static_data->network_file 
           << endl;
      poSite = 99;
      poHost = 1;
      cerr << "\tPO Defaulting to Site 99, Host 1, default interface." << endl;
   }

}  //  End of po_sitehost.
  
 
//  This function used to be the main routine for the po_reader.  Now its
//  functiionality has been reduced to opening the po_net. (SAK)
//  This is separate from the NPSNET network library and it should
//  actually be brought together with the NPSNET library.  Much of 
//  this is hardwired and should be made more flexible.
int po_net_open( unsigned short port )
{
   int success = TRUE;
   char ether_interf[20];

   po_sitehost(ether_interf);

   cerr << "----------------------------------------" << endl;
   cerr << "C2/PO Settings:" << endl;
   cerr << "\tCTDB, Name=" << L_ctdb.name << ", version="
        << L_ctdb.version << ", date=" << L_ctdb.date << endl;
   cerr << "\tUTM Origin: " << L_ctdb.origin_northing << "N "
        << L_ctdb.origin_easting << "E" << endl;
   cerr << "\tDatabase " << int(L_database) << endl;
   cerr << "\tExercise "
        << int(L_exercise) << endl;
   cerr << "\tNetwork Config: Interface=" << ether_interf
        << ", Max PDUs=" << 32 << ", Port=" << int(port) << endl;
   cerr << "\tSite=" << poSite <<", Host=" << poHost << endl;

   success = netOpen(ether_interf,32,port);

   if ( success ) {
      sendSimPresent(); 
      makeOverlay();
   }
   else {
      cerr << "ERROR, PO/C2 unable to open network." << endl;
   }

   return success;

}  //  End po_net_open.
   

void sendSimPresent(){
//cerr<<"in sendSimPresent"<<endl;
   SP_PersistentObjectPDU		*pdu;
   //unsigned short length = sizeof(SP_PersistentObjectPDU);
   unsigned short length;
   char name[80];
   char databaseID[SP_maxTerrainNameLength];

   sprintf ( databaseID, "%s-%d", L_ctdb.name, L_ctdb.version );

   gethostname(name,80);

   if(NULL == (pdu = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   pdu->version = poProtocol;
   pdu->kind = SP_simulatorPresentPDUKind;
   pdu->exercise = L_exercise;
   pdu->database = L_database;
   //pdu->length = sizeof(SP_SimulatorPresentVariant);
   
   pdu->variant.simulatorPresent.simulator.site = poSite;
   pdu->variant.simulatorPresent.simulator.host = poHost;
   pdu->variant.simulatorPresent.simulatorType = SP_simulatorUnknown;
   pdu->variant.simulatorPresent.databaseSequenceNumber = 0;
   pdu->variant.simulatorPresent.packetsSent = 1;    // HARD now, may use. 
   pdu->variant.simulatorPresent.load = 0;
   pdu->variant.simulatorPresent.simulationLoad = 1.0; 
   pdu->variant.simulatorPresent.time = 0.0;
   pdu->variant.simulatorPresent.relativeBattleScheme = 0;

   strcpy(pdu->variant.simulatorPresent.hostname, name);

   strcpy(pdu->variant.simulatorPresent.terrain.terrainName, databaseID);
   pdu->variant.simulatorPresent.terrain.terrainVersion = 
      SP_latestTerrainVersion;
   
   pdu->length = PRO_PO_SIM_PRESENT_SIZE(pdu);
   length = PRO_PO_SIM_PRESENT_SIZE(pdu);

   net_write((char*)pdu, length);
   
}


//  This function creates an "NPSNET" overlay.  It currently uses a hard
//  wired overlayID.  It builds the overlay and sends the PDU.  If the function
//  hasn't captured a simulator it will have to take ownership, otherwise it
//  will pass ownership ideally to a ModSAF station to maintain the overlay.
//  It the future, we should be able to pick an overlay to use as ModSAF does,
//  but for now we have ensured we have an overlay.
void makeOverlay(){
   
   SP_PersistentObjectPDU *pdu;

   char name[] = "NPSNET";
   SP_OverlayColor color = SP_OCBlack;
   unsigned scratch = 0;
   unsigned working = 0;

   SP_ForceID forceID = SP_forceIDIrrelevant;

   int length;

   static sequence = 0;

   if(NULL == (pdu = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  ){
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   pdu->version = poProtocol;
   pdu->kind = SP_describeObjectPDUKind;
   pdu->exercise = L_exercise;
   pdu->database = L_database;
      

   pdu->variant.describeObject.databaseSequenceNumber = 0;

   pdu->variant.describeObject.objectID.simulator.site = OVERLAY_SITE;
   pdu->variant.describeObject.objectID.simulator.host = OVERLAY_HOST;
   pdu->variant.describeObject.objectID.object = OVERLAY_OBJECT;

   //  Real time world state.
   pdu->variant.describeObject.worldStateID.simulator.site = 0;
   pdu->variant.describeObject.worldStateID.simulator.host = 0;
   pdu->variant.describeObject.worldStateID.object = 0;

   //  I may have captured a simulator to pass this overlay to.
   if (  !((slaveSim.site == 0) && (slaveSim.host == 0)) ){
      pdu->variant.describeObject.owner.site = slaveSim.site;
      pdu->variant.describeObject.owner.host = slaveSim.host;
      overlayOwner.site = slaveSim.site;
      overlayOwner.host = slaveSim.host;
//cerr<<"creating overlay but PASSING ownership"<<endl;
   }
   else{        //  This simulator has to take possession.
//cerr<<"creating overlay, ASSUMING ownership"<<endl;
      pdu->variant.describeObject.owner.site = poSite;
      pdu->variant.describeObject.owner.host = poHost;
      overlayOwner.site = poSite;
      overlayOwner.host = poHost;
   }

   pdu->variant.describeObject.sequenceNumber = sequence++;
   pdu->variant.describeObject.mclass = SP_objectClassOverlay;
   pdu->variant.describeObject.missingFromWorldState = 0;

   strcpy(pdu->variant.describeObject.variant.overlay.name, name);
   pdu->variant.describeObject.variant.overlay.color = color;
   pdu->variant.describeObject.variant.overlay.scratch = scratch;
   pdu->variant.describeObject.variant.overlay.working = working;
   pdu->variant.describeObject.variant.overlay.forceID = forceID;

   pdu->length = PRO_PO_DESC_OBJ_SIZE(pdu) + PRO_PO_OVERLAY_CLASS_SIZE;

   length = PRO_PO_DESC_OBJ_SIZE(pdu) + PRO_PO_OVERLAY_CLASS_SIZE;
   net_write((char*)pdu, length);

}  //  End of makeOverlay().



void po_net_read()
{
   //extern unsigned short host_id;
   read_po_stat	rstat;	
   
   STORE		*struct_ptr;
   SP_PersistentObjectPDU		*pdu_ptr;  
   unsigned short site;

   if ((struct_ptr = netRead(&rstat)) == NULL) {
       //No PDUs are available            
   } 
   else {
      pdu_ptr = (SP_PersistentObjectPDU *) &struct_ptr->v.po;

      //  We need to grab the protocol version but just once.  We need to get
      //  the version before we can send out a pdu.
      if (!poProtocol){
         poProtocol = pdu_ptr->version;
         cerr<<"Assigning PO protocol version.  Version "
             <<(int)poProtocol<<endl;
      }

//cerr << "Got a PO PDU" << endl;

      if((pdu_ptr->exercise == L_exercise) && 
         (pdu_ptr->database == L_database)){
         switch(pdu_ptr->kind){
            case SP_simulatorPresentPDUKind:
               site = pdu_ptr->variant.simulatorPresent.simulator.site;
               //cerr<<"PDU of type SP_simulatorPresentPDUKind read.SAK. ";
               //cerr<<"Site "<<site<<
               //   " Host "<<pdu_ptr->variant.simulatorPresent.simulator.host<<
               //   endl;
               captureSim(pdu_ptr->variant.simulatorPresent);
               break; 
            case SP_describeObjectPDUKind:
               //cerr << "PDU of type SP_describeObject read.SAK" << endl;
               process_pdu(struct_ptr);
               break;
            case SP_objectsPresentPDUKind:
               //cerr<<"PDU of type SP_objectsPresentPDUKind read.SAK."<<endl;
               break; 
            case SP_objectRequestPDUKind:
               //cerr<<"PDU of type SP_objectRequestPDUKind read.SAK."<<endl;
               break;
            case SP_deleteObjectsPDUKind:
               //cerr << "PDU of type SP_deleteObjectsPDUKind read.SAK" << endl;
               delete_object(pdu_ptr);
               break;
            case SP_setWorldStatePDUKind:
               //cerr<<"PDU of type SP_setWorldStatePDUKind read.SAK."<<endl;
               break;
            case SP_nominationPDUKind:
               //cerr<<"PDU of type SP_nominationPDUKind read.SAK."<<endl;
               break;
            default:
               cerr << "Unknown PO PDU type " << pdu_ptr->kind << endl;
         }  //  End switch.
      }
   }  //  End else.

   //if (rstat.rs_wasted > 0){
      //fflush(stdout);
   //}

}  //  End of po_net_read.


//SAK.
//  This function processes the PDU's received.  It finds if the po described
//  by the pdu is in the array of po, if it needs updating or if a new po
//  needs to be constructed.
void process_pdu(STORE *p){

   DIS_ID newkey;
   char* temp;
   int update = 0;
   int length;

   //cerr<<"Read a SP_DescribeObjectVariant pdu length "<< p->length << endl;
   length = (int)p->length;

   //Build newkey
   newkey.v.okey.space = 0;
   newkey.v.okey.obj_id = p->v.po.variant.describeObject.objectID;

   for (int ix = 0; ix < poCount; ix++){
      //  Check if measure already in poArray.
      if (newkey.v.dkey == poArray[ix].key.v.dkey){
         update = 1;
        
         //  Now see if the pdu I now have is the same as pdu 
         //  already describing the object.  The pdu was stored as characters
         //  so I will just compare these.

         temp = (char *) &p->v.po.variant.describeObject;

         //  Cycle through for loop.  In will stop when they don't equal.
         //  if itt makes it all the way through the string jx will equal
         //  length.  The loop does all the work.
         for (int jx = 0; ((jx < length) && 
            (poArray[ix].poPointer->pdu[jx] == temp[jx])); jx++); 

         //  Object already up to date.
         if(jx == length){
         }

         // An object already in the array which needs updating.  The keys
         // matched but the PDU did not.
         else{    
            //  Update it with the received pdu.  Use the member function
            //  to update it accordingly.
cerr<<"poArray:"<<poArray[ix].poPointer->pdu<<"/temp:"<<temp<<endl;
            poArray[ix].poPointer->update(p->v.po.variant.describeObject,length);
                
            //  Reattach it.  It got detached in the update.
            pfAddChild(L_po_meas, poArray[ix].poPointer->polys); 

          } // End else. 

       }  //  End if, newkey.v.dkey ==.
   } // End for, ix.

   if (!update){    
      //  A new measure, we have to build it from scratch.  Construct the 
      //  correct measure, assin the key and put into the array of
      //  po measures.   
      switch (p->v.po.variant.describeObject.mclass){
         case SP_objectClassPoint:  
            if ((p->v.po.variant.describeObject.variant.point.overlayID.simulator.
               site == OVERLAY_SITE) &&
               (p->v.po.variant.describeObject.variant.point.overlayID.simulator.
               host == OVERLAY_HOST) &&
               (p->v.po.variant.describeObject.variant.point.overlayID.
               object == OVERLAY_OBJECT)){  
              
               poArray[poCount].key = newkey;
               poArray[poCount].poPointer =
                  new PO_POINT_CLASS(p->v.po.variant.describeObject, length);
               pfAddChild(L_po_meas, poArray[poCount].poPointer->polys);
               poCount++;
            }
            break;
         case SP_objectClassLine:

            if ((p->v.po.variant.describeObject.variant.line.overlayID.simulator.
               site == OVERLAY_SITE) &&
               (p->v.po.variant.describeObject.variant.line.overlayID.simulator.
               host == OVERLAY_HOST) &&
               (p->v.po.variant.describeObject.variant.line.overlayID.
               object == OVERLAY_OBJECT)){

               poArray[poCount].key = newkey;
               poArray[poCount].poPointer =
                  new PO_LINE_CLASS(p->v.po.variant.describeObject,length);
               pfAddChild(L_po_meas, poArray[poCount].poPointer->polys);
               poCount++;
            }
            break;
         case SP_objectClassText:                  
            poArray[poCount].key = newkey;
            poArray[poCount].poPointer =
               new PO_TEXT_CLASS(p->v.po.variant.describeObject, length);
            pfAddChild(L_po_meas, poArray[poCount].poPointer->polys);
            poCount++;
            break;
         case SP_objectClassMinefield:                    
            poArray[poCount].key = newkey;
            poArray[poCount].poPointer =
               new PO_MINE_CLASS(p->v.po.variant.describeObject, length);
            pfAddChild(L_po_meas, poArray[poCount].poPointer->polys);
            poCount++;
            break;
         case SP_objectClassOverlay:
            //cerr<<"Got SP_OverlayClass.  SAK"<<endl;
            captureOverlay(p->v.po.variant.describeObject);
            break;
         default:
            cerr<<"Something wrong got in the po.  Class "<<
               p->v.po.variant.describeObject.mclass<<" is not right."<<endl;
            break;
        
      }  //  End switch 
   } //  End if, !update.   
}  //  End process_pdu.


//  This function deletes a po from the array of po_measures. 
void delete_object(SP_PersistentObjectPDU *p){

   SP_DeleteObjectsVariant* deletePDU;
   SP_DescribeObjectVariant* storedPDU;

   
   deletePDU = (SP_DeleteObjectsVariant*) &(p->variant.deleteObjects);

   for (int i = 0; i < p->variant.deleteObjects.objectCount; i++){
      
      for (int j = 0; j < poCount; j++){
//cerr<<"in delete"<<endl;           
         storedPDU = (SP_DescribeObjectVariant*)(poArray[j].poPointer->pdu);

//cerr<<"sitePDU "<< deletePDU->objects[i].objectID.simulator.site<<endl;
//cerr<<"siteSTORD "<<storedPDU->objectID.simulator.site<<endl;
//cerr<<"hostPDU "<<deletePDU->objects[i].objectID.simulator.host<<endl;
//cerr<<"hostSTORD " <<storedPDU->objectID.simulator.host<<endl; 
//cerr<<"objectPDU "<<deletePDU->objects[i].objectID.object<<endl;
//cerr<<"objectSTORD "<<storedPDU->objectID.object<<endl;


         if ( (deletePDU->objects[i].objectID.simulator.site ==
            storedPDU->objectID.simulator.site) &&
            (deletePDU->objects[i].objectID.simulator.host ==
            storedPDU->objectID.simulator.host) && 
            (deletePDU->objects[i].objectID.object ==
            storedPDU->objectID.object) ){

//cerr<<"got a match to delete "<<endl;
               //  Make sure I'm not dragging what I'm about to delete. 
               //  If so skip the delete.
               if (!( (poArray[j].poPointer->polys == pickedGroup) && 
                  get_button(NPS_LEFTMOUSE) ) ){
                  pfRemoveChild(L_po_meas, poArray[j].poPointer->polys);
                  pfDelete(poArray[j].poPointer->polys);
                  pfFree(poArray[j].poPointer);
            
                  for (int k = j; k < poCount; k++){
                     poArray[k] = poArray[k+1];
                  }

                  poArray[poCount].poPointer = NULL;
                  poArray[poCount].key.v.dkey = 0;
                  poCount--;
               }  //  Check for drag.
            }  // End if, objects are the same.
         }  // End for, j.
      }  // End for, i. 
}  //  End of delete_object.


//  This function will capture an overlay.  UNUSED at this time.  It is
//  called when an overlay is captured but it does nothing.
void captureOverlay(SP_DescribeObjectVariant describeObject){

//cerr<<"overlay "<<describeObject.objectID.simulator.site<<" "<<
//describeObject.objectID.simulator.host<<" "<<
//describeObject.objectID.object<<endl;

   if ( !(strcmp("NPSNET", describeObject.variant.overlay.name)) ) {
//cerr<<"captured an NPSNET overlay "<<endl;
   }
}

//  Capture the first simulator out there I see.  I will need to be more
//  discriminating in the future.  Right now I assume that any simulator
//  that's not an unknown and is giving a pulse will be able to handle
//  the objects I create.
void captureSim(SP_SimulatorPresentVariant simulator){

   if ( (slaveSim.site == 0) && (slaveSim.host == 0) && 
      (simulator.simulatorType != SP_simulatorUnknown)){
      slaveSim.site = simulator.simulator.site;
      slaveSim.host = simulator.simulator.host;
//cerr<<"HAVE CAPTURED A SIM "<<slaveSim.site<<" "<<slaveSim.host<<endl; 
   }
}


//  This function is called everytime through the main function of NPSNET.  It
//  is used to get all mouse inputs an then send them to the appropriate 
//  function to do picking.
void processPick(){
//cerr<<"in process pick"<<endl;
   //menuPick();
   if (get_button(NPS_RIGHTMOUSE)) menuPick();
   if (get_button(NPS_RIGHTMOUSE)) ballPick();   
   HLPick();
//cerr<<"leaving process pick"<<endl;
}  

//  I want to maintain as little as possible.  But I will send a heartbeat out
//  on my simulator, and any po measures I got stuck owning.  The
//  only way I should own any is if I create them prior to finding a simulator
//  to pass them off to.
void poHeartBeat(){
   const double BEATLENGTH = 20.0;
   static double lastTime = pfGetTime();

   if ( (pfGetTime() - lastTime) >= BEATLENGTH ){

      sendSimPresent();                           //  I'm alive.
//cerr<<"overlay owner "<<overlayOwner.site<<" "<<overlayOwner.host<<endl;
      //  I don't want to maintain overlay, but if I do I must send the
      //  pdu.  Note that when I call 'makeOverlay()' it will again
      //  try to pawn off the overlay.
      if ( (overlayOwner.site == poSite) && (overlayOwner.host == poHost)){
//cerr<<"going to make overlay"<<endl;         
         makeOverlay();
      }  


      lastTime = pfGetTime();
   }

}

void updatePoStuff(){
   if ( display_po_key() ) {
      toggle_po_display();
   }
   po_net_read();
   processPick();
   poHeartBeat();
}   


void toggle_po_display()
{
   static int po_is_displaying = FALSE;
   
   if ( po_is_displaying ) {
      po_hide_measures();
      po_is_displaying = FALSE;
   }
   else {
      po_display_measures();
      po_is_displaying = TRUE;
   }

}   

int closePoNet(){

   netClose();
   return TRUE;

} 


#endif
