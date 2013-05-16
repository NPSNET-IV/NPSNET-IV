/* Copyright (c) 1996, Carnegie Mellon University
 *
 *    Digital Mapping Laboratory (MAPSLab)
 *    Computer Science Department
 *    Carnegie Mellon University
 *    5000 Forbes Avenue
 *    Pittsburgh, PA 15213-3890
 *    maps@cs.cmu.edu
 *    http://www.cs.cmu.edu/~MAPSLab
 *
 *
 * Permission to use, copy, and modify this software and its
 * documentation for any NON-COMMERCIAL purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Digital Mapping Laboratory 
 * (MAPSLab) Computer Science Department, Carnegie Mellon University
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *
 * We request that users of this software to return to maps@cs.cmu.edu
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 * Redistribution of this code is forbidden without the express written 
 * consent of the MAPSLab Research Group. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 */
/* Intersection masks 
   */

#ifndef _tsg_Imask_h_
#define _tsg_Imask_h_

#define TSG_PERMANENT_MASK        0x0000FF00 /* Second byte */
#define TSG_PERMANENT_SHIFT       8
#define TSG_PERMANENT_OTHER       (0<<TSG_PERMANENT_SHIFT)
#define TSG_PERMANENT_DIRT        (1<<TSG_PERMANENT_SHIFT)
#define TSG_PERMANENT_WATER       (2<<TSG_PERMANENT_SHIFT)
#define TSG_PERMANENT_FOILAGE     (4<<TSG_PERMANENT_SHIFT)
#define TSG_PERMANENT_BRIDGE      (8<<TSG_PERMANENT_SHIFT)
#define TSG_PERMANENT_TUNNEL      (16<<TSG_PERMANENT_SHIFT)

#define TSG_DESTRUCT_MASK         0x00FF0000 /* Third byte */
#define TSG_DESTRUCT_SHIFT        16
#define TSG_DESTRUCT_OTHER        (0<<TSG_DESTRUCT_SHIFT)
#define TSG_DESTRUCT_BUILDING     (1<<TSG_DESTRUCT_SHIFT)
#define TSG_DESTRUCT_FOLIAGE      (2<<TSG_DESTRUCT_SHIFT)
#define TSG_DESTRUCT_PLATFORM     (4<<TSG_DESTRUCT_SHIFT)
#define TSG_DESTRUCT_LIFEFORM     (8<<TSG_DESTRUCT_SHIFT)

#endif
