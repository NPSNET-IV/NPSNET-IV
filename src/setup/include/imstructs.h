/* imstructs.h */

#include <pdu.h>
#include "setup_const.h"

#ifndef MARKINGS_LEN
#define MARKINGS_LEN 11	/* DIS Marking field */
#endif

struct symbol {
   char		*name;		/* The string to match */
   int		value;		/* Token value to return to parser */
   int		numchars;	/* Number of chars to match, if less than */
				/*   full length of token; 0 mandates */
				/*   full length match; by convention, */
				/*   minimum abbreviation of tokens is */
				/*   3 chars to avoid conflict with strings */
};

#ifndef IM_MAX_STRING_LEN
#define IM_MAX_STRING_LEN 256
#endif

#define STRING32	32
#define STRING64	64
#define STRING128	128

union query_data {
   int          ints[3];
   char         string[MAX_SEARCH_PATH];	/* Set to longest val */
   float        floats[4];
};

struct MC {
   int	animations;
   char	animation_file[STRING128];
   char auto_file[STRING128];
   int  auto_state;
   int bdihml;
   struct {
      float speed_ratio;
      float position_error;
      float time_constant;
      int trans_smoothness;
      int position_mode;
      int position_mechanism;
   } diguy_info;
   int	bird_control;
   char	bird_port[STRING64];
   char	blinking_lights_file[STRING128];
   struct {
      float	xmin;
      float	xmax;
      float	ymin;
      float	ymax;
   } BoundingBox;
   int	channel;
   struct {
      float	cp_near;
      float	cp_far;
   } ClipPlanes;
   int	control;
   int	cpu_lock;
   char cursor_file[STRING128];
   int  db_paging;
   int  db_pages_allowed;
   int  db_offset;
   char db_tsg_file[STRING128];
   int  dead_switch;
   struct {
      int	exercise;
      float	heartbeat;
      float	posdelta;
      float	rotdelta;
      float	timeout;
   } DIS;
   int dis_enum;
   char	dump_file[STRING128];
   char	dvw_file[STRING128];
   int	dynamic_objs;
   int	es_mode;
   char	environmental_file[STRING128];
   char entity_file[STRING128];
   char	ether_interf[STRING32];
   struct {
      int	check;
      char	port[STRING32];
      int	prompt;
      int       mp;
   } fcs;
   struct {
      float	x;
      float	y;
   } FoV;
   char	filter_file[STRING128];
   int  flat_world;
   int	flybox;
   char	flybox_port[STRING32];
   int	fog;
   int  fog_type;
   float fog_near;
   float fog_far;
   int	force;
   float	frame_rate;
   char fs_port[STRING32];
   char fs_config[STRING128];
   float fs_scale;
   float	ground_height;
   int	hires;
   int	hmd;
   char hmd_file[STRING128];
   float	horizon_angle;
   char	hud_font_file[STRING128];
   int hud_level;
   char	identify_file[STRING128];
   char	intro_font_file[STRING128];
   int jack;
   char	landmark_file[STRING128];
   int	leather_flybox_check;
   char	leather_flybox_port[STRING32];
   char	location_file[STRING128];
   float lod_fade;
   float lod_scale;
   int lw_rport;
   int lw_sport;
   char lw_toip[STRING128];
   char	marking[MARKINGS_LEN+1];
   char	master[STRING64];
   struct {
      float max;
   } alt;
   char	model_file[STRING128];
   char	modsaf_map[STRING128];
   struct {
      int	enable;
      char	group[STRING32];
      int	ttl;
   } multicast;
   int	multisample;
   int	networking;
   char	network_file[STRING128];
   int	notify_level;
   char	nps_pic_dir[STRING128];
   char odt_port[STRING32];
   int odt_mp;
   struct {
      char      group[STRING32];
      int       ttl;
      int	port;
   } panel;
   int	pdu_position;
   int	phase;
   int	pictures;
   int	pipe;
   struct {
      char	ctdb_file[STRING128];
      int	database;
      int	exercise;
      int	port;
   } PO;
   int	port;
   int	process_mode;
   char	radar_icon_file[STRING128];	/* No equivalent MC command */
   float range_filter;
   int	remote_input_panel;
   char	road_file[STRING128];
   char	round_world_file[STRING128];
   int	scoring;
   char	search_path[MAX_SEARCH_PATH];
   int	sound;
   char	sound_file[STRING128];
   int	static_objs;
   int	stealth;
   int	stereo;
   struct {
      int	site;
      int	host;
      int	entity;
   } target_entity;
   int target_bbox;
   char	terrain_file[STRING128];
   char	testport[STRING32];
   char	traker_file[STRING128];
   char	traker_port[STRING32];
   char	transport_file[STRING128];
   char	treadport[STRING32];
   char	uniport[STRING32];
   char upperbody[STRING32];
   int  use_round;
   char	vehicle_driven[STRING32];
   int	view;
   struct {
      float	xmin;
      float	xmax;
      float	ymin;
      float	ymax;
   } ViewPort;
   struct {
      float	heading;
      float	pitch;
      float	roll;
   } ViewHPR;
   struct {
      float	x;
      float	y;
      float	z;
   } ViewXYZ;
   int waypoints;
   int weapon_view;
   struct {
      int	type;
      float	xmin;
      float	xmax;
      float	ymin;
      float	ymax;
   } window;
};

#ifndef MAX_ANIM_COPIES
#define MAX_ANIM_COPIES 25
#endif
#ifndef MAX_ANIMATIONS
#define MAX_ANIMATIONS 15
#endif

struct Eff {
   int		effect_type;	/* TYPE_MODEL = conventional
				   TYPE_BB = billboard */
   char		filename[STRING128];	/* "explosion1.flt", etc. */
   float	delay;
};

struct Anim {
   int		count;
   char		identifier[STRING32];	/* "explosion", "small_gun", etc. */
   int		mode;
   int		reps;
   int		num_effects;
   struct Eff	Effect[MAX_ANIM_COPIES];
};

#ifndef MAX_BLINKING_LIGHTS
#define MAX_BLINKING_LIGHTS 10
#endif

struct BL {
   struct {
      float	x;
      float	y;
      float	z;
   } Position;
   struct {
      float	r;
      float	g;
      float	b;
   } Color;
   float	size;
   float	blink_rate;
};

#ifndef MAX_PDU_FILTERS
#define MAX_PDU_FILTERS 15
#endif

struct FIL {
   int		filter_type;	/* TYPE_BLOCK or TYPE_FORCE */
   union {
      struct {
         int	site;
         int	host;
         int	entity;
      } Block;
      struct {
         int	x;
         int	y;
         int	z;
         unsigned int	and_mask;
         unsigned int	or_mask;
         int	range;
      } Force;
   } Type;
};

#ifndef MAX_LANDMARKS
#define MAX_LANDMARKS 10
#endif

struct LAND {
   char		icon;
   float	x;
   float	y;
};

#ifndef MAX_STATIC_OBJ_TYPES
#define MAX_STATIC_OBJ_TYPES 100
#endif

#define MAX_POINTS 2000

struct Pos {
   float	x;
   float	y;
   float	z;
   float	heading;
   float	pitch;
   float	roll;
};

struct Model {
   char		flightfile[STRING128];
   char		path[STRING128];
   int		alive_mod;
   int		dead_mod;
   int		otype;
   int		num_points;
   struct Pos	points[MAX_POINTS];
};

#ifndef MAX_VEHTYPES
#define MAX_VEHTYPES 230
#endif
#ifndef MAX_FIRE_INFO
#define MAX_FIRE_INFO 3
#endif

struct Weap {
   int		index;
   int		wclass;
   float	x;
   float	y;
   float	z;
   float	delay;
};

struct Dyn {
   char		filename[STRING128];
   char		jack_model[STRING64];	/* Only if model is a Jack */
   char		path[STRING128];
   int		alive_mod;
   int		dead_mod;
   int		vtype;
   EntityType	etype;
   int		mtype; /* This allows us to determine 
                          which model type to load */
   int		num_weapons;
   struct Weap	weapons[MAX_FIRE_INFO];
};

/* Network File */

#define MAX_HOSTS 64

struct Host {
   char		hostname[STRING64];
   int		siteno;
   int		hostno;
   char		interf[STRING32];
};

#ifndef MAX_RADAR_DIVS
#define MAX_RADAR_DIVS 30
#endif

struct RI {
   int		min;
   int		max;
   char		ident;
};

struct RW {
   double	northing;
   double	easting;
   int		zone_num;
   char		zone_letter;
   int		map_datum;
   int		width;
   int		height;
};

#ifndef MAX_SOUND_EVENTS
#define MAX_SOUND_EVENTS 300
#endif
#ifndef MAX_SOUNDS_PER_EVENT
#define MAX_SOUNDS_PER_EVENT 5
#endif

struct Event {
   char		filename[STRING128];
   float	range;
};

struct Sound {
   int		num_events;
   struct Event	Events[MAX_SOUNDS_PER_EVENT];
};

#ifndef MAX_TRANSPORT
#define MAX_TRANSPORT 16
#endif

struct TP {
   float	x;
   float	y;
   float	z;
   float	heading;
   float	pitch;
   float	roll;
};

#define MAX_MASKS 16

struct TM {
   char		filename[STRING128];
   int		mask;
};


struct defs {
   struct MC	MasterConfig;
   int		num_animations;
   struct Anim	Animations[MAX_ANIMATIONS];
   int		num_blinking_lights;
   struct BL	BlinkingLights[MAX_BLINKING_LIGHTS];
   int		num_filters;
   struct FIL	Filters[MAX_PDU_FILTERS];
   int		num_landmarks;
   struct LAND	Landmarks[MAX_LANDMARKS];
   int		num_static_objs;
   struct Model	SObjects[MAX_STATIC_OBJ_TYPES];
   int		num_dynamic_objs;
   struct Dyn	DObjects[MAX_VEHTYPES];
   int		num_hosts;
   struct Host	Hosts[MAX_HOSTS];			/* Network File */
   int		num_radar_icons;
   struct RI	RadarIcons[MAX_RADAR_DIVS];
   struct RW	RoundWorld;
   struct Sound	Sounds[MAX_SOUND_EVENTS];
   int		num_transport_locs;
   struct TP	TransportLocs[MAX_TRANSPORT];
   int		num_masks;
   struct TM	Masks[MAX_MASKS];			/* Terrain File */
   int		InitComplete;
};

/* EOF */
