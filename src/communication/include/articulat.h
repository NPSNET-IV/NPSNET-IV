// File: <articulat.h>

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
/* File:	articulat.h
 * Description:	DIS Articulation Parameters
 *
 * Reference:	Enumeration and Bit-encoded Values for use with
 *		IEEE 1278.1-1995, Standard for Distributed 
 *		Interactive Simulation--Application Protocols
 *		IST-CR-95-05
 *
 * Author:	John Locke / Paul T. Barham
 *		CS Department, Naval Postgraduate School
 * Internet:	jxxl@cs.nps.navy.mil
 *		barham@cs.nps.navy.mil
 */


/* Parameter Types */

#define AP_Type_Mask            0x0000001F

#define AP_Position		1
#define AP_PositionRate		2
#define AP_Extension		3
#define AP_ExtensionRate	4
#define AP_X			5
#define AP_XRate		6
#define AP_Y			7
#define AP_YRate		8
#define AP_Z			9
#define AP_ZRate		10
#define AP_Azimuth		11
#define AP_AzimuthRate		12
#define AP_Elevation		13
#define AP_ElevationRate	14
#define AP_Rotation		15
#define AP_RotationRate		16

/* Articulating Parts */

#define AP_Part_Mask		0xFFFFFFE0

#define AP_Rudder		1024
#define AP_LeftFlap		1056
#define AP_RightFlap		1088
#define AP_LeftAileron		1120
#define AP_RightAileron		1152
#define AP_OtherAirCtrlSurface	1184

#define AP_Periscope		2048
#define AP_GenericAntenna	2080
#define AP_Snorkel		2112
#define AP_OtherExtendablePart	2144

#define AP_LandingGear		3072			/* JO 02/28/96 */
#define AP_TailHook		3104			/* JO 02/28/96 */
#define AP_SpeedBrake		3136			/* JO 02/28/96 */
#define AP_LeftWeaponBayDoor	3168			/* JO 02/28/96 */
#define AP_RightWeaponBayDoor	3200			/* JO 02/28/96 */
#define AP_TankAPCHatch		3232			/* JO 02/28/96 */
#define AP_Wingsweep		3264			/* JO 02/28/96 */
#define AP_OtherFixedPosPart	3296			/* JO 02/28/96 */

#define AP_PrimaryTurret_1	4096
#define AP_PrimaryTurret_2	4128
#define AP_PrimaryTurret_3	4160
#define AP_PrimaryTurret_4	4192
#define AP_PrimaryTurret_5	4224
#define AP_PrimaryTurret_6	4256
#define AP_PrimaryTurret_7	4288
#define AP_PrimaryTurret_8	4320
#define AP_PrimaryTurret_9	4352
#define AP_PrimaryTurret_10	4384

#define AP_PrimaryGun_1		4416
#define AP_PrimaryGun_2		4448
#define AP_PrimaryGun_3		4480
#define AP_PrimaryGun_4		4512
#define AP_PrimaryGun_5		4544
#define AP_PrimaryGun_6		4576
#define AP_PrimaryGun_7		4608
#define AP_PrimaryGun_8		4640
#define AP_PrimaryGun_9		4672
#define AP_PrimaryGun_10	4704

#define AP_PrimaryLauncher_1	4736
#define AP_PrimaryLauncher_2	4768
#define AP_PrimaryLauncher_3	4800
#define AP_PrimaryLauncher_4	4832
#define AP_PrimaryLauncher_5	4864
#define AP_PrimaryLauncher_6	4896
#define AP_PrimaryLauncher_7	4928
#define AP_PrimaryLauncher_8	4960
#define AP_PrimaryLauncher_9	4992
#define AP_PrimaryLauncher_10	5024

#define AP_PrimaryDefense_1	5056
#define AP_PrimaryDefense_2	5088
#define AP_PrimaryDefense_3	5120
#define AP_PrimaryDefense_4	5152
#define AP_PrimaryDefense_5	5184
#define AP_PrimaryDefense_6	5216
#define AP_PrimaryDefense_7	5248
#define AP_PrimaryDefense_8	5280
#define AP_PrimaryDefense_9	5312
#define AP_PrimaryDefense_10	5344

#define AP_PrimaryRadar_1	5376
#define AP_PrimaryRadar_2	5408
#define AP_PrimaryRadar_3	5440
#define AP_PrimaryRadar_4	5472
#define AP_PrimaryRadar_5	5504
#define AP_PrimaryRadar_6	5536
#define AP_PrimaryRadar_7	5568
#define AP_PrimaryRadar_8	5600
#define AP_PrimaryRadar_9	5632
#define AP_PrimaryRadar_10	5664

#define AP_SecondaryTurret_1	5696
#define AP_SecondaryTurret_2	5728
#define AP_SecondaryTurret_3	5760
#define AP_SecondaryTurret_4	5792
#define AP_SecondaryTurret_5	5824
#define AP_SecondaryTurret_6	5856
#define AP_SecondaryTurret_7	5888
#define AP_SecondaryTurret_8	5920
#define AP_SecondaryTurret_9	5952
#define AP_SecondaryTurret_10	5984

#define AP_SecondaryGun_1	6016
#define AP_SecondaryGun_2	6048
#define AP_SecondaryGun_3	6080
#define AP_SecondaryGun_4	6112
#define AP_SecondaryGun_5	6144
#define AP_SecondaryGun_6	6176
#define AP_SecondaryGun_7	6208
#define AP_SecondaryGun_8	6240
#define AP_SecondaryGun_9	6272
#define AP_SecondaryGun_10	6304

#define AP_SecondaryLauncher_1	6336
#define AP_SecondaryLauncher_2	6368
#define AP_SecondaryLauncher_3	6400
#define AP_SecondaryLauncher_4	6432
#define AP_SecondaryLauncher_5	6464
#define AP_SecondaryLauncher_6	6496
#define AP_SecondaryLauncher_7	6528
#define AP_SecondaryLauncher_8	6560
#define AP_SecondaryLauncher_9	6592
#define AP_SecondaryLauncher_10	6624

#define AP_SecondaryDefense_1	6656
#define AP_SecondaryDefense_2	6688
#define AP_SecondaryDefense_3	6720
#define AP_SecondaryDefense_4	6752
#define AP_SecondaryDefense_5	6784
#define AP_SecondaryDefense_6	6816
#define AP_SecondaryDefense_7	6848
#define AP_SecondaryDefense_8	6880
#define AP_SecondaryDefense_9	6912
#define AP_SecondaryDefense_10	6944

#define AP_SecondaryRadar_1	6976
#define AP_SecondaryRadar_2	7008
#define AP_SecondaryRadar_3	7040
#define AP_SecondaryRadar_4	7072
#define AP_SecondaryRadar_5	7104
#define AP_SecondaryRadar_6	7136
#define AP_SecondaryRadar_7	7168
#define AP_SecondaryRadar_8	7200
#define AP_SecondaryRadar_9	7232
#define AP_SecondaryRadar_10	7264

/* EOF */
