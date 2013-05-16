// File: <datum.h>

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
/* File:	datum.h
 * Description:	DIS PDU Datum Parameters
 * Revision:	2.0 - 15Jul93
 *
 * Reference:	Military Standard--Protocol Data Units for Entity
 *		Information and Entity Interaction in a Distributed
 *		Interactive Simulation (DIS) (22Mar93)
 *
 * Author:	John Locke
 *		CS Department, Naval Postgraduate School
 * Internet:	jxxl@taurus.cs.nps.navy.mil
 */


#define DatumParam_VehMass		1
#define DatumParam_ForceID		2
#define DatumParam_EntTypeKind		3
#define DatumParam_EntTypeDomain	4
#define DatumParam_EntTypeCountry	5
#define DatumParam_EntTypeSubcat	6
#define DatumParam_EntTypeSpecific	7
#define DatumParam_EntTypeExtra		8
#define DatumParam_AltEntTypeKind	9		/* JO 02/28/96 */
#define DatumParam_AltEntTypeDomain	10		/* JO 02/28/96 */
#define DatumParam_AltEntTypeCountry	11		/* JO 02/28/96 */
#define DatumParam_AltEntTypeSubcat	12		/* JO 02/28/96 */
#define DatumParam_AltEntTypeSpecific	13		/* JO 02/28/96 */
#define DatumParam_AltEntTypeExtra	14		/* JO 02/28/96 */
#define DatumParam_EntLocX		15		/* JO 02/28/96 */
#define DatumParam_EntLocY		16		/* JO 02/28/96 */
#define DatumParam_EntLocZ		17		/* JO 02/28/96 */
#define DatumParam_EntLinVelX		18		/* JO 02/28/96 */
#define DatumParam_EntLinVelY		19		/* JO 02/28/96 */
#define DatumParam_EntLinVelZ		20		/* JO 02/28/96 */
#define DatumParam_EntOrientationX	21		/* JO 02/28/96 */
#define DatumParam_EntOrientationY	22		/* JO 02/28/96 */
#define DatumParam_EntOrientationZ	23		/* JO 02/28/96 */
#define DatumParam_DeadReckonAlgorithm	24		/* JO 02/28/96 */
#define DatumParam_DeadReckonLinAccelX	25		/* JO 02/28/96 */
#define DatumParam_DeadReckonLinAccelY	26		/* JO 02/28/96 */
#define DatumParam_DeadReckonLinAccelZ	27		/* JO 02/28/96 */
#define DatumParam_DeadReckonAngVelX	28		/* JO 02/28/96 */
#define DatumParam_DeadReckonAngVelY	29		/* JO 02/28/96 */
#define DatumParam_DeadReckonAngVelZ	30		/* JO 02/28/96 */
#define DatumParam_EntAppear		31		/* JO 02/28/96 */
#define DatumParam_EntMarkCharSet	32		/* JO 02/28/96 */
#define DatumParam_EntMark11Bytes	33		/* JO 02/28/96 */
#define DatumParam_Capability		34		/* JO 02/28/96 */
#define DatumParam_NumArticParams	35		/* JO 02/28/96 */
#define DatumParam_ArticParamID		36		/* JO 02/28/96 */
#define DatumParam_ArticParamType	37		/* JO 02/28/96 */
#define DatumParam_ArticParamValue	38		/* JO 02/28/96 */
#define DatumParam_AmtAmmoSupplies	39		/* JO 02/28/96 */
#define DatumParam_AmmoQuantity		40		/* JO 02/28/96 */
#define DatumParam_FuelQuantity		41		/* JO 02/28/96 */
#define DatumParam_RadarSysStat  	42		/* JO 02/28/96 */
#define DatumParam_RadioCommSysStat	43		/* JO 02/28/96 */
#define DatumParam_TimeStatRadioTrans	44		/* JO 02/28/96 */
#define DatumParam_TimeMovRadioTrans	45		/* JO 02/28/96 */
#define DatumParam_BodyPrtDamagedRadio	46		/* JO 02/28/96 */
#define DatumParam_TerrainDBFile	47		/* JO 02/28/96 */
#define DatumParam_LocalLoggerFile	48		/* JO 02/28/96 */

/* EOF */
