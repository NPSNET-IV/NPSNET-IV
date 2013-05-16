#ifndef __envirvalues_h_
#define __envirvalues_h_

/*MOst of the values in this file come from ModSAF's dis203exp_values.h*/

/* COPYRIGHT 1993 Loral Advanced Distributed Simulation
 * 50 Moulton Street
 * Cambridge, Ma. 02138
 * 617-873-3600
 *
 *
 * Please note!  All hard-coded values that are found in the appendices
 * of the DIS document "Military Standard (Final Draft) Protocol Data
 * Units for Entity Information and Entity Interaction in a Distributed
 * Interactive Simulation" are defined in "dis203_values.drn".
 *
 * Also note that commonly used types and headers which are part of the
 * official DIS standard are defined in "dis203_types.drn".
 */
#define ENVIRONMENTAL_PDU_KIND 154
typedef unsigned long TIME_TYPE;

typedef unsigned char    SP_DIS_EXP_PDU_KIND; 

 /* Constants should match their SIMNET counterparts */

#define ENV_WEATHER_PRECIPITATION_NONE 0
#define ENV_WEATHER_PRECIPITATION_OTHER 1
#define ENV_WEATHER_PRECIPITATION_RAIN 2
#define ENV_WEATHER_PRECIPITATION_SLEET 3
#define ENV_WEATHER_PRECIPITATION_HAIL 4
#define ENV_WEATHER_PRECIPITATION_SNOW 5

typedef unsigned long    ENV_WEATHER_PRECIPITATION_TYPE; 

#define ENV_WEATHER_EXTINCTION_NONE 0
#define ENV_WEATHER_EXTINCTION_OTHER 1
#define ENV_WEATHER_EXTINCTION_RURAL 2
#define ENV_WEATHER_EXTINCTION_MARITIME 3
#define ENV_WEATHER_EXTINCTION_URBAN 4
#define ENV_WEATHER_EXTINCTION_TROPOSPHERIC 5
#define ENV_WEATHER_EXTINCTION_ADVECTIVEFOG 6
#define ENV_WEATHER_EXTINCTION_RADIATIVEFOG 7
#define ENV_WEATHER_EXTINCTION_DESERT 8
#define ENV_weatherExtinctionDesertSummer 9
#define ENV_weatherExtinctionDesertWinter 10
#define ENV_weatherExtinctionTemperateSummerDay 11
#define ENV_weatherExtinctionTemperateSummerNight 12
#define ENV_weatherExtinctionTemperateWinter 13

typedef unsigned long    ENV_WEATHER_EXTINCTION_TYPE; 

typedef unsigned long    ENV_WAVEBAND_TYPE;
typedef struct {
   ENV_WAVEBAND_TYPE waveband;
   float         coeff;
} ENV_EXTINCTION_RECORD;

#define ENV_STATUS_OFF   0    
#define ENV_STATUS_ON    1    

typedef unsigned long    ENV_STATUS_TYPE; 

#define ENV_VEHICLE_TRACKED 0 
#define ENV_VEHICLE_WHEELED 1 

typedef unsigned short   ENV_VEHICLE_TYPE; 

#define ENV_NODE_OFF     0    
#define ENV_NODE_ON      1    

typedef unsigned short   ENV_NODE_FLAG_TYPE; 

#define ENV_SOIL_ROCK    0    
#define ENV_SOIL_DRY_SOIL 1   
#define ENV_SOIL_DRY_SAND 2   
#define ENV_SOIL_MEDIUM_SAND 3
#define ENV_SOIL_WET_SAND 4   
#define ENV_SOIL_WET_SOIL 5   

typedef unsigned short   ENV_SOIL_TYPE; 

#define ENV_WEATHER_MODEL_SIMPLE 0x100
#define ENV_EPHEMERIS_MODEL_SIMPLE 0x500
#define ENV_SMOKE_MODEL_OTHER 0x1000
#define ENV_SMOKE_MODEL_SIMPLE 0x1001
#define ENV_SMOKE_MODEL_ATLAS 0x1002
#define ENV_SMOKE_MODEL_COMBIC 0x1003
#define ENV_OBSCURANT_MODEL_OTHER 0x1010
#define ENV_OBSCURANT_MODEL_SIMPLE 0x1011

typedef unsigned short   ENV_MODEL_TYPE; 

#define ENV_PADDING_KIND 0x0   /* State Records */
#define ENV_STATE_SIMPLE_ILLUMINATION_KIND 0x1
#define ENV_STATE_SIMPLE_SOUND_KIND 0x2
#define ENV_STATE_SIMPLE_OBSCURANT_KIND 0x3
#define ENV_STATE_SIMPLE_WEATHER_KIND 0x4
#define ENV_STATE_COMBIC_PLUME_KIND 0x100
#define ENV_STATE_COMBIC_PUFF_KIND 0x101
#define ENV_STATE_EPHEMERIS_KIND 0x102
#define ENV_STATE_FLARE_KIND 0x103
#define ENV_STATE_SIMULATION_DATE_KIND 0x500
#define ENV_VEHICLE_DUST_KIND 0x501 /* Geometry Records */
#define ENV_GEOMETRY_UNIFORM_KIND 0x10000000
#define ENV_GEOMETRY_POINT_KIND 0x10000001
#define ENV_GEOMETRY_SPHEROID_KIND 0x10000002
#define ENV_GEOMETRY_POLYGON_KIND 0x10000003
#define ENV_GEOMETRY_EXTENTS_KIND 0x10000004
#define ENV_GEOMETRY_SPHERICAL_COORDS_KIND 0x10000005
#define ENV_GEOMETRY_GAUSSIAN_PLUME_KIND 0x10000100
#define ENV_GEOMETRY_GAUSSIAN_PUFF_KIND 0x10000200
#define ENV_GEOMETRY_CONE_KIND 0x10000300

typedef unsigned long    ENV_RECORD_KIND_TYPE; 

 /* Number of records in packet */
typedef unsigned long ENV_NUMBER_OF_RECORDS_TYPE; 
 /* Relative distances */
typedef float ENV_DISTANCE_TYPE; 
 /* Velocity */
typedef float ENV_VELOCITY_TYPE; 

typedef float ENV_VELOCITY_VECTOR[3]; 
 /* Acceleration */
typedef float ENV_ACCELERATION_TYPE; 
 /* Density */
typedef float ENV_DENSITY_TYPE; 
 /* Index in a state record used to find associated geometry record */
typedef unsigned short ENV_GEOMETRY_INDEX_TYPE; 
 /* Number of vertices stored in record */
typedef long ENV_NUMBER_OF_VERTICES_TYPE; 
 /* Paddng to fill out max record size */
typedef struct {
    unsigned long _unused_10_11;         /* padding */
    unsigned long _unused_10_12;         /* padding */
    unsigned long _unused_10_13;         /* padding */
    unsigned long _unused_10_14;         /* padding */
    unsigned long _unused_10_15;         /* padding */
    unsigned long _unused_10_16;         /* padding */
    unsigned long _unused_10_17;         /* padding */
    unsigned long _unused_10_18;         /* padding */
    unsigned long _unused_10_19;         /* padding */
    unsigned long _unused_10_20;         /* padding */
    unsigned long _unused_10_21;         /* padding */
    unsigned long _unused_10_22;         /* padding */
    unsigned long _unused_10_23;         /* padding */
    unsigned long _unused_10_24;         /* padding */
    unsigned long _unused_10_25;         /* padding */
    unsigned long _unused_10_26;         /* padding */
    unsigned long _unused_10_27;         /* padding */
    unsigned long _unused_10_28;         /* padding */
    unsigned long _unused_10_29;         /* padding */
    unsigned long _unused_10_30;         /* padding */
    unsigned long _unused_10_31;         /* padding */
    unsigned long _unused_10_32;         /* padding */
    unsigned long _unused_10_33;         /* padding */
    unsigned long _unused_10_34;         /* padding */
    unsigned long _unused_10_35;         /* padding */
    unsigned long _unused_10_36;         /* padding */
    unsigned long _unused_10_37;         /* padding */
    unsigned long _unused_10_38;         /* padding */
    unsigned long _unused_10_39;         /* padding */
    unsigned long _unused_10_40;         /* padding */
    unsigned long _unused_10_41;         /* padding */
    unsigned long _unused_10;            /* padding */
} ENV_PADDING_VARIANT; 

typedef long ENV_NUMBER_OF_SOURCES_TYPE; 
/* obsolete:
typedef struct {
    TIME_TYPE                  time_since_detonation; 
    EntityType                 source; 
    ENV_NUMBER_OF_SOURCES_TYPE number_of_sources; 
    ENV_GEOMETRY_INDEX_TYPE    geometry_index; 
} ENV_STATE_COMBIC_VARIANT; */

typedef unsigned long ENV_SOURCE_TYPE;

typedef struct {
    TIME_TYPE                  time_since_detonation;
    EntityTypeRecord           source;
    ENV_NUMBER_OF_SOURCES_TYPE number_of_sources;
    ENV_GEOMETRY_INDEX_TYPE    geometry_index;
    unsigned short             _unused_44;  /* padding */
    ENV_SOURCE_TYPE            source_type;
    float                      barrage_rate;/*only valid if source_type*/
    float                      barrage_duration; /* is barrage. */
    float                      crosswind_length; /* dimensions of */
    float                      downwind_length;  /* impact region. */
    ENV_VELOCITY_VECTOR        detonate_velocity;
    /*angle from which detonation occurred, currently used w/ barrages*/
} ENV_STATE_COMBIC_VARIANT; /* a.k.a. SmokeVariant */




typedef struct {
    TIME_TYPE                  time_since_detonation; 
    EntityType                 source; 
    float                      source_intensity;
    ENV_NUMBER_OF_SOURCES_TYPE number_of_sources; 
    ENV_GEOMETRY_INDEX_TYPE    geometry_index; 
} ENV_STATE_FLARE_VARIANT; 
 /* a little wierd, but might be useful for small databases */
typedef struct {
    ENV_GEOMETRY_INDEX_TYPE sun_geometry_index; 
    ENV_GEOMETRY_INDEX_TYPE moon_geometry_index; 
    float                             moon_phase; 
} ENV_STATE_EPHEMERIS_VARIANT; 

typedef struct {
    unsigned long                 simulation_time; 
    ENV_WEATHER_PRECIPITATION_TYPE precipitation_type; 
    /* precipitation rate (mm/hr) */
    float                         precipitation_rate; 
    ENV_WEATHER_EXTINCTION_TYPE extinction_type;  /* units tbd */
    float                         extinction_amount;
    /* deg C */
    float                         temperature; 
    float                         dewpoint;
    /* relative humidity percent */
    float                         relative_humidity; 
    /* barometric pressure in N/M2 (1 N/m2 == 7.501E-4 cm Hg) */
    float                         barometric_pressure; 
    /* illumination level in ft-candles, abstraction of tod/cloud cover */
    float                         ambient_light;
    /* wind velocity in m/s */
    ENV_VELOCITY_VECTOR wind; 
    /* 0.0 is no clouds, 1.0 is complete cloud cover */
    float                         cloud_cover;
    /* altitude ASL */
    float                         cloud_top; 
    float                         cloud_base;
    /* extinction coefficients(3) and the wavebands they appy to */
    ENV_EXTINCTION_RECORD extinctionRecord[3]; 
    /* where does this weather apply? */
    unsigned short                geometry_index; 
} ENV_STATE_SIMPLE_WEATHER_VARIANT; 


/*
 * Used for padding out to max size
 *
 */

typedef struct {
    unsigned long _unused_42_43;         /* padding */
    unsigned long _unused_42_44;         /* padding */
    unsigned long _unused_42_45;         /* padding */
    unsigned long _unused_42_46;         /* padding */
    unsigned long _unused_42_47;         /* padding */
    unsigned long _unused_42_48;         /* padding */
    unsigned long _unused_42_49;         /* padding */
    unsigned long _unused_42_50;         /* padding */
    unsigned long _unused_42_51;         /* padding */
    unsigned long _unused_42_52;         /* padding */
    unsigned long _unused_42_53;         /* padding */
    unsigned long _unused_42_54;         /* padding */
    unsigned long _unused_42_55;         /* padding */
    unsigned long _unused_42_56;         /* padding */
    unsigned long _unused_42_57;         /* padding */
    unsigned long _unused_42_58;         /* padding */
    unsigned long _unused_42_59;         /* padding */
    unsigned long _unused_42_60;         /* padding */
    unsigned long _unused_42_61;         /* padding */
    unsigned long _unused_42_62;         /* padding */
    unsigned long _unused_42_63;         /* padding */
    unsigned long _unused_42_64;         /* padding */
    unsigned long _unused_42_65;         /* padding */
    unsigned long _unused_42_66;         /* padding */
    unsigned long _unused_42_67;         /* padding */
    unsigned long _unused_42_68;         /* padding */
    unsigned long _unused_42_69;         /* padding */
    unsigned long _unused_42_70;         /* padding */
    unsigned long _unused_42_71;         /* padding */
    unsigned long _unused_42_72;         /* padding */
    unsigned long _unused_42_73;         /* padding */
    unsigned long _unused_42;            /* padding */
} ENV_GEOMETRY_UNIFORM_VARIANT; 

typedef struct {
    double x; 
    double y; 
} ENV_HORZ_WORLD_COORDINATES_RECORD; 

typedef struct {
    double location; 
} ENV_GEOMETRY_POINT_VARIANT; 

typedef struct {
    double  z0; 
    double  z1; 
    ENV_NUMBER_OF_VERTICES_TYPE       number_of_vertices; 
    unsigned long                               _unused_74_75;  /* padding */
    unsigned long                               _unused_74_76;  /* padding */
    unsigned long                               _unused_74;  /* padding */
    ENV_HORZ_WORLD_COORDINATES_RECORD vertex[1]; 
} ENV_GEOMETRY_POLYGON_VARIANT; 

typedef struct {
    EntityLocation location; 
    ENV_DISTANCE_TYPE            distance; 
    float                   azimuth; 
    ENV_DISTANCE_TYPE            sigma_y; 
    ENV_DISTANCE_TYPE            sigma_z; 
    ENV_DISTANCE_TYPE            height; 
    ENV_VELOCITY_TYPE            horz_velocity; 
    ENV_VELOCITY_TYPE            vert_velocity; 
    ENV_VELOCITY_TYPE            d_sigma_y_dt; 
    ENV_VELOCITY_TYPE            d_sigma_z_dt; 
} ENV_GEOMETRY_GAUSSIAN_PLUME_VARIANT; 

typedef struct {
    EntityLocation location; 
    ENV_DISTANCE_TYPE            distance; 
    float                   azimuth; 
    ENV_DISTANCE_TYPE            sigma_x; 
    ENV_DISTANCE_TYPE            sigma_y; 
    ENV_DISTANCE_TYPE            sigma_z; 
    ENV_DISTANCE_TYPE            height; 
    ENV_VELOCITY_TYPE            horz_velocity; 
    ENV_VELOCITY_TYPE            vert_velocity; 
    ENV_VELOCITY_TYPE            d_sigma_x_dt; 
    ENV_VELOCITY_TYPE            d_sigma_y_dt; 
    ENV_VELOCITY_TYPE            d_sigma_z_dt; 
} ENV_GEOMETRY_GAUSSIAN_PUFF_VARIANT; 

typedef struct {
    EntityLocation location; 
    ENV_DISTANCE_TYPE            radius; 
    VelocityVector                acceleration; 
    VelocityVector                velocity; 
    ENV_VELOCITY_TYPE            d_radius_dt; 
} ENV_GEOMETRY_SPHEROID_VARIANT; 

typedef struct {
    EntityLocation location; 
    float                   peak_angle; 
    ENV_VELOCITY_VECTOR          horz_velocity; 
    ENV_VELOCITY_TYPE            vert_velocity; 
} ENV_GEOMETRY_CONE_VARIANT; 

typedef struct {
    EntityLocation origin; 
    EntityLocation extent; 
} ENV_GEOMETRY_EXTENTS_VARIANT; 

typedef struct {
    float        azimuth; 
    float        elevation; 
    ENV_DISTANCE_TYPE range;  
} ENV_GEOMETRY_SPHERICAL_COORDS_VARIANT; 

typedef unsigned long ENV_STATE_SIMULATION_DATE_VARIANT; 

typedef float ENV_TIME_OFFSET_TYPE; 

typedef float ENV_COMPONENT_OFFSET_TYPE; 

typedef struct {
    ENV_COMPONENT_OFFSET_TYPE x_offset; 
    ENV_COMPONENT_OFFSET_TYPE y_offset; 
    ENV_COMPONENT_OFFSET_TYPE z_offset; 
} ENV_LOCATION_OFFSET_RECORD; 

typedef struct {
    ENV_NODE_FLAG_TYPE         node_flag; 
    ENV_SOIL_TYPE              soil_type; 
    TIME_TYPE                  time_since_creation; 
    ENV_LOCATION_OFFSET_RECORD location_offset; 
    ENV_VELOCITY_VECTOR        vehicle_velocity; 
} ENV_NODE_RECORD; 

typedef struct {
    EntityID             vehicle_id; 
    ENV_VEHICLE_TYPE             vehicle_type; 
    TIME_TYPE                    time_since_creation; 
    ENV_VELOCITY_VECTOR          vehicle_velocity; 
    EntityLocation location; 
    ENV_SOIL_TYPE                soil_type; 
    unsigned short                         _unused_77_78;  /* padding */
    unsigned long                          _unused_77;  /* padding */
    ENV_NODE_RECORD              location_node[2]; 
} ENV_VEHICLE_DUST_VARIANT; 

typedef struct {
    ENV_RECORD_KIND_TYPE record_kind; 
    unsigned long _unused_79;            /* padding */
    union {
        ENV_PADDING_VARIANT                 padding;
        ENV_STATE_COMBIC_VARIANT            combic_plume;
        ENV_STATE_COMBIC_VARIANT            combic_puff;
        ENV_STATE_FLARE_VARIANT             flare;
        ENV_STATE_EPHEMERIS_VARIANT         ephemeris;
        ENV_STATE_SIMPLE_WEATHER_VARIANT    simple_weather;
        ENV_STATE_SIMULATION_DATE_VARIANT   simulation_date;
        ENV_GEOMETRY_UNIFORM_VARIANT        uniform;
        ENV_GEOMETRY_POINT_VARIANT          point;
        ENV_GEOMETRY_SPHEROID_VARIANT       spheroid;
        ENV_GEOMETRY_GAUSSIAN_PLUME_VARIANT gaussian_plume;
        ENV_GEOMETRY_CONE_VARIANT           cone;
        ENV_GEOMETRY_GAUSSIAN_PUFF_VARIANT  gaussian_puff;
        ENV_GEOMETRY_POLYGON_VARIANT        polygon;
        ENV_GEOMETRY_EXTENTS_VARIANT        extents;
        ENV_VEHICLE_DUST_VARIANT            vehicle_dust;
    } variant;                          
} ENV_RECORD; 


/************ end of data units **********/

#endif
