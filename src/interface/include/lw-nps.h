/*
 *! ID: %Z% %Y% %M% v%I% %G%
 *!
 *! NAME: lw-nps.h
 *! 
 *! DESCRIPTION:
 *!	This file details the message formats for messages exchanged
 *!	between the Hughes' Land Warrior system and the Naval Post
 *!	Graduate School for the 1996 AUSA show.
 *!
 *! CONSTRAINTS:
 *!
 *! SYMBOLS:
 *!
 *! EXAMPLES:
 *!
 *! NOTES:
 *!
 *! SEE ALSO:
 *!
 *! FILES:
 *!
 */

#ifndef _LW_NPS_H_
#define _LW_NPS_H_

/*
 * These are the formats of the messages sent between the NPS gateway and NPS.
 * They are sent as byte streams via TCP.
 */

  /*
   * The floating point data is being sent as two integers.
   */
  typedef struct
  {
	int	whole;		/* The whole number portion */
	int	decimal;	/* The fractional portion */
  } Float;

  /*
   * This is the Weapon System switch message.
   * It is sent from the NPS gateway to NPS.
   * It is only sent when one of the switches changes state.
   */
  typedef struct
  {
	short	host_id;	/* DIS Host ID */
	short	entity_id;	/* DIS Entity ID */
	short	site_id;	/* DIS Site ID */
	short	laser;		/* 0 - off, ~0 - switch engaged */
	short	trigger;	/* 0 - off, ~0 - trigger pulled */
	short	video;		/* 0 - off, ~0 - trigger pulled */
  } DIS_WSIF_rec;

  /*
   * This is the range and bearing message.
   * It is sent from NPS to the NPS gateway.
   */
  typedef struct
  {
	short	host_id;	/* DIS Host ID */
	short	entity_id;	/* DIS Entity ID */
	short	site_id;	/* DIS Site ID */
	short	padding;
	Float	elevation;
	Float	azimuth;
	Float	range;
  } AER_rec;


#endif   /* _LW_NPS_H_ */

