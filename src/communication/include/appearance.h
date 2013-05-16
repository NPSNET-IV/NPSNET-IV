// File: <appearance.h>

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
/* File:	appearance.h
 * Description:	DIS Entity State PDU Appearance Field Enumerations
 * Revision:	DIS Version 2.0, Third & Fourth Draft 
 *
 * Refs:   a)	Enumeration and Bit Encoded Values for Use with IEEE 1278.1-1994,
 *		Standard for Distributed Interactive Simulation --
 *              Applications Protocols
 *		March 1994
 *		(IST-CR-93-46)
 *
 *         b)	Standard for Distributed Interactive Simulation --
 *		Application Protocols
 *		Version 2.0, Fourth Draft
 *		February 4, 1994
 *		(IST-CR-93-40)              
 *
 * Author:	Paul T. Barham
 *		CS Department, Naval Postgraduate School
 * Date:	September 10, 1994
 * Internet:	barham@cs.nps.navy.mil
 */

#ifdef DIS_2v4

/* General Appearance */
#define Appearance_Paint_Mask			0x00000001
#define Appearance_CamoPaint			0x00000001

#define Appearance_Mobility_Mask		0x00000002
#define Appearance_MobilityKill			0x00000002

#define Appearance_FirePower_Mask		0x00000004
#define Appearance_FirePowerKill		0x00000004

#define Appearance_Damage_Mask			0x00000018
#define Appearance_DamageSlight			0x00000008
#define Appearance_DamageModerate		0x00000010
#define Appearance_DamageDestroyed		0x00000018

#define Appearance_Smoke_Mask			0x00000060
#define Appearance_SmokePlume			0x00000020
#define Appearance_SmokeEngine			0x00000040
#define Appearance_SmokePlume_Engine		0x00000060

#define Appearance_TrailingEffects_Mask		0x00000180
#define Appearance_TrailingEffectsSmall		0x00000080
#define Appearance_TrailingEffectsMedium	0x00000100
#define Appearance_TrailingEffectsLarge		0x00000180

#define Appearance_Hatch_Mask			0x00000E00
#define Appearance_HatchClosed			0x00000200
#define Appearance_HatchPopped			0x00000400
#define Appearance_HatchPoppedWithHead		0x00000600
#define Appearance_HatchOpen			0x00000800
#define Appearance_HatchOpenWithHead		0x00000A00

#define Appearance_Lights_Mask			0x00007000
#define Appearance_LightsRunning		0x00001000
#define Appearance_LightsNavigation		0x00002000
#define Appearance_LightsFormation		0x00003000

#define Appearance_Flaming_Mask			0x00008000
#define Appearance_Flaming			0x00008000

/* Specific Appearance */

/* All Platforms */
#define Appearance_Platform_Status_Mask		0x00E00000
#define Appearance_Platform_Frozen		0x00200000
#define Appearance_Platform_PowerOn		0x00400000
#define Appearance_Platform_Deactivated		0x00800000

/* Land Platform Appearances */
#define Appearance_PlatformLand_Launcher_Mask	0x00010000
#define Appearance_PlatformLand_Launcher_Up	0x00010000

#define Appearance_PlatformLand_Camo_Mask	0x00060000
#define Appearance_PlatformLand_Camo_Desert	0x00000000
#define Appearance_PlatformLand_Camo_Winter	0x00020000
#define Appearance_PlatformLand_Camo_Forest	0x00040000	/*JO 03/01/96*/
/*#define Appearance_PlatformLand_Camo_Forest	0x00060000 */

#define Appearance_PlatformLand_Concealed_Mask	0x00080000
#define Appearance_PlatformLand_Concealed	0x00080000

/* Air Platform Appearances */
#define Appearance_PlatformAir_AfterBurner_Mask	0x00010000
#define Appearance_PlatformAir_AfterBurner	0x00010000

/* Surface Platform Appearances    no specific bits defined */

/* Subsurface Platform Appearances no specific bits defined */

/* Space Platform Appearances      no specific bits defined */

/* Guided Munition Appearances */
#define Appearance_Munition_LaunchFlash_Mask	0x00010000
#define Appearance_Munition_LaunchFlash		0x00010000
#define Appearance_Munition_Status_Mask		Appearance_Platform_Status_Mask
#define Appearance_Munition_Status_Frozen	Appearance_Platform_Frozen
#define Appearance_Munition_Status_Deactivated	Appearance_Platform_Deactivated

/* Life Form Appearances */
#define Appearance_LifeForm_State_Mask		0x000F0000
#define Appearance_LifeForm_UprightStill	0x00010000
#define Appearance_LifeForm_UprightWalking	0x00020000
#define Appearance_LifeForm_UprightRunning	0x00030000
#define Appearance_LifeForm_Kneeling		0x00040000
#define Appearance_LifeForm_Prone		0x00050000
#define Appearance_LifeForm_Crawling		0x00060000
#define Appearance_LifeForm_Swimming		0x00070000
#define Appearance_LifeForm_Parachuting		0x00080000
#define Appearance_LifeForm_Jumping		0x00090000

#define Appearance_LifeForm_Status_Mask		Appearance_Platform_Status_Mask
#define Appearance_LifeForm_Frozen		Appearance_Platform_Frozen
#define Appearance_LifeForm_Deactivated		Appearance_Platform_Deactivated

#define Appearance_LifeForm_Weapon1_Mask	0x03000000
#define Appearance_LifeForm_Weapon1_Stowed	0x01000000
#define Appearance_LifeForm_Weapon1_Deployed	0x02000000
#define Appearance_LifeForm_Weapon1_Firing	0x03000000

#define Appearance_LifeForm_Weapon2_Mask	0x0C000000
#define Appearance_LifeForm_Weapon2_Stowed	0x04000000
#define Appearance_LifeForm_Weapon2_Deployed	0x08000000
#define Appearance_LifeForm_Weapon2_Firing	0x0C000000

/* Environmental Appearances */
#define Appearance_Environ_Density_Mask		0x000F0000
#define Appearance_Environ_Density_Hazy		0x00010000
#define Appearance_Environ_Density_Dense	0x00020000
#define Appearance_Environ_Density_VeryDense	0x00030000
#define Appearance_Environ_Density_Opaque	0x00040000

/* Cultural Feature Appearances   no specific bits defined */

/* Supply Appearances   no specific bits defined */

/* Radio Appearances    no specific bits defines */

/********************** END DIS 2.0.4 *************************/

#else

/********************* START DIS 2.0.3 ************************/

/* General Appearance */
#define Appearance_Paint_Mask			0x00000001
#define Appearance_CamoPaint			0x00000001

#define Appearance_Mobility_Mask		0x00000002
#define Appearance_MobilityKill			0x00000002

#define Appearance_FirePower_Mask		0x00000004
#define Appearance_FirePowerKill		0x00000004

#define Appearance_Damage_Mask			0x00000018
#define Appearance_DamageSlight			0x00000008
#define Appearance_DamageModerate		0x00000010
#define Appearance_DamageDestroyed		0x00000018

#define Appearance_Smoke_Mask			0x00000060
#define Appearance_SmokePlume			0x00000020
#define Appearance_SmokeEngine			0x00000040
#define Appearance_SmokePlume_Engine		0x00000060

#define Appearance_TrailingEffects_Mask		0x00000180
#define Appearance_TrailingEffectsSmall		0x00000080
#define Appearance_TrailingEffectsMedium	0x00000100
#define Appearance_TrailingEffectsLarge		0x00000180

#define Appearance_Hatch_Mask			0x00000E00
#define Appearance_HatchClosed			0x00000200
#define Appearance_HatchPopped			0x00000400
#define Appearance_HatchPoppedWithHead		0x00000600
#define Appearance_HatchOpen			0x00000800
#define Appearance_HatchOpenWithHead		0x00000A00

#define Appearance_Lights_Mask			0x00007000
#define Appearance_LightsRunning		0x00001000
#define Appearance_LightsNavigation		0x00002000
#define Appearance_LightsFormation		0x00003000

#define Appearance_Flaming_Mask			0x00008000
#define Appearance_Flaming			0x00008000

/* Specific Appearance */

/* All Platforms - NOT IN 2.0.3      *** COMMENTED OUT ***
* #define Appearance_Platform_Status_Mask	0x00E00000
* #define Appearance_Platform_Frozen		0x00200000
* #define Appearance_Platform_PowerOn		0x00400000
* #define Appearance_Platform_Deactivated	0x00800000
*/

/* Land Platform Appearances */
#define Appearance_PlatformLand_Launcher_Mask	0x00010000
#define Appearance_PlatformLand_Launcher_Up	0x00010000

#define Appearance_PlatformLand_Camo_Mask	0x00060000
#define Appearance_PlatformLand_Camo_Desert	0x00000000
#define Appearance_PlatformLand_Camo_Winter	0x00020000
#define Appearance_PlatformLand_Camo_Forest	0x00060000

#define Appearance_PlatformLand_Concealed_Mask	0x00080000
#define Appearance_PlatformLand_Concealed	0x00080000

/* Air Platform Appearances */
#define Appearance_PlatformAir_AfterBurner_Mask	0x00010000
#define Appearance_PlatformAir_AfterBurner	0x00010000

/* Surface Platform Appearances    no specific bits defined */

/* Subsurface Platform Appearances no specific bits defined */

/* Space Platform Appearances      no specific bits defined */

/* Guided Munition Appearances */
#define Appearance_Munition_LaunchFlash_Mask	0x00010000
#define Appearance_Munition_LaunchFlash		0x00010000

/* NOT IN 2.0.3                      *** COMMENTED OUT ***
* #define Appearance_Munition_Status_Mask	Appearance_Platform_Status_Mask
* #define Appearance_Munition_Status_Frozen	Appearance_Platform_Frozen
* #define Appearance_Munition_Status_Deactivated	Appearance_Platform_Deactivated
*/

/* Life Form Appearances  - ONLY BITS 16 & 17 IN 2.0.3 */
#define Appearance_LifeForm_State_Mask		0x00030000

/* ALL UPRIGHTS IN 2.0.3 ARE THE SAME */
#define Appearance_LifeForm_Upright		0x00010000
#define Appearance_LifeForm_UprightStill	0x00010000
#define Appearance_LifeForm_UprightWalking	0x00010000
#define Appearance_LifeForm_UprightRunning	0x00010000

#define Appearance_LifeForm_Kneeling		0x00020000
#define Appearance_LifeForm_Prone		0x00030000

/* NOT IN 2.0.3                      *** COMMENTED OUT ***
* #define Appearance_LifeForm_Crawling		0x00060000
* #define Appearance_LifeForm_Swimming		0x00070000
* #define Appearance_LifeForm_Parachuting	0x00080000
* #define Appearance_LifeForm_Jumping		0x00090000
* 
* #define Appearance_LifeForm_Status_Mask	Appearance_Platform_Status_Mask
* #define Appearance_LifeForm_Frozen		Appearance_Platform_Frozen
* #define Appearance_LifeForm_Deactivated	Appearance_Platform_Deactivated
*/

/* IN 2.0.3, ONLY STOWED AND DEPLOYED IN SPECIFIC BIT 18 */
#define Appearance_LifeForm_Weapon1_Mask	0x00040000
#define Appearance_LifeForm_Weapon1_Stowed	0x00000000
#define Appearance_LifeForm_Weapon1_Deployed	0x00040000
#define Appearance_LifeForm_Weapon1_Firing	0x00040000

/* IN 2.0.3, ONLY STOWED AND DEPLOYED IN SPECIFIC BIT 19 */
#define Appearance_LifeForm_Weapon2_Mask	0x00080000
#define Appearance_LifeForm_Weapon2_Stowed	0x00000000
#define Appearance_LifeForm_Weapon2_Deployed	0x00080000
#define Appearance_LifeForm_Weapon2_Firing	0x00080000

/* Environmental Appearances */
#define Appearance_Environ_Density_Mask		0x000F0000
#define Appearance_Environ_Density_Hazy		0x00010000
#define Appearance_Environ_Density_Dense	0x00020000
#define Appearance_Environ_Density_VeryDense	0x00030000
#define Appearance_Environ_Density_Opaque	0x00040000

/* Cultural Feature Appearances   no specific bits defined */

/* Supply Appearances   no specific bits defined */

/* Radio Appearances    no specific bits defines */

#endif
