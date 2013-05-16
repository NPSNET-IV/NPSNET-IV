#ifndef DI_APPERANCE_DOT_H
#define DI_APPERANCE_DOT_H

/* Life Form Appearances  - ONLY BITS 16 & 17 IN 2.0.3 */
#define Appearance_LifeForm_State_Mask_v203          0x00030000

/* ALL UPRIGHTS IN 2.0.3 ARE THE SAME */
#define Appearance_LifeForm_Upright_v203             0x00010000
#define Appearance_LifeForm_UprightStill_v203        0x00010000
#define Appearance_LifeForm_UprightWalking_v203      0x00010000
#define Appearance_LifeForm_UprightRunning_v203      0x00010000
#define Appearance_LifeForm_Kneeling_v203            0x00020000
#define Appearance_LifeForm_Prone_v203               0x00030000

/* NOT IN 2.0.3                      *** COMMENTED OUT ***
* #define Appearance_LifeForm_Crawling_v203          0x00060000
* #define Appearance_LifeForm_Swimming_v203          0x00070000
* #define Appearance_LifeForm_Parachuting_v203       0x00080000
* #define Appearance_LifeForm_Jumping_v203           0x00090000
*
*/

/* IN 2.0.3, ONLY STOWED AND DEPLOYED IN SPECIFIC BIT 18 */
#define Appearance_LifeForm_Weapon1_Mask_v203        0x00040000
#define Appearance_LifeForm_Weapon1_Stowed_v203      0x00000000
#define Appearance_LifeForm_Weapon1_Deployed_v203    0x00040000
#define Appearance_LifeForm_Weapon1_Firing_v203      0x00040000

#endif // DI_APPERANCE_DOT_H
