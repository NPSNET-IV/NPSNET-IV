
#ifndef __NPS_COLLISION_CONST__
#define __NPS_COLLISION_CONST__

#define PRUNE_MASK            0x00000000
#define ALL_MASK              0x00FFFFFF
#define ENTITY_MASK           0x000000FF /* First byte */
#define ENTITY_SHIFT          0
#define ENTITY_KIND_MASK      0x00000007 /* Lower three bits, 0 - 7 */
#define ENTITY_KIND_SHIFT     0
#define ENTITY_DOMAIN_MASK    0x00000038 /* next three bits, 0 - 7 */
#define ENTITY_DOMAIN_SHIFT   3
#define ENTITY_FORCE_MASK     0x000000C0 /* next two bits, 0 - 3 */
#define ENTITY_FORCE_SHIFT    6

#define PERMANENT_MASK        0x0000FF00 /* Second byte */
#define PERMANENT_SHIFT       8
#define PERMANENT_OTHER       0
#define PERMANENT_DIRT        1
#define PERMANENT_WATER       2
#define PERMANENT_FOILAGE     4
#define PERMANENT_BRIDGE      8
#define PERMANENT_TUNNEL      16
#ifndef NOSUB
#define PERMANENT_MOVINGWATER 32
#endif // NOSUB

#define DESTRUCT_MASK         0x00FF0000 /* Third byte */
#define DESTRUCT_SHIFT        16
#define DESTRUCT_OTHER        0
#define DESTRUCT_BUILDING     1
#define DESTRUCT_FOILAGE      2
#define DESTRUCT_PLATFORM     4
#define DESTRUCT_LIFEFORM     8

#define COLLIDE_STATIC  0
#define COLLIDE_DYNAMIC 1

#define INTERSECT_OFFSET 0.33f

#endif
