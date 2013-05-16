// imclass.h
#ifndef IMCLASS_DOT_H
#define IMCLASS_DOT_H
#include <ulocks.h>

class InitManager {
public:
   //
   InitManager();	// Constructor
   ~InitManager();	// Destructor
   void parse_input_files();
   void process_environment_vars();
   void process_cl_args(int, char**);
   void process_Master_Configs(int, char**);
   void validate_data();
   int dump_inits();
   int query(int, union query_data*, int);
   int get_blinking_lights(struct BL**, int*);
   int get_filters(struct FIL**, int*);
   int get_landmarks(struct LAND**, int*);
   int get_hosts(struct Host**, int*);
   int get_round_world_data(struct RW*);
   void get_transport_locs(struct TP**, int*);
   void get_radar_icons(struct RI**, int*);
   int get_sounds(struct Sound**);
   int get_terrain_masks(struct TM**, int*);
   int get_animations(struct Anim**);
   int get_models(struct Model **, int*);
   void get_dynamic_objs(struct Dyn**, int*);
private:
   char		name_array[128];
   char		share_file[60];
   ofstream	dumpfile;
   usptr_t *initArena;
   int		no_master;
   //
   void assign_defaults();
   void parse_file(char*, int);
   void process_environment_vars_filenames();
   void process_environment_vars_others();
   int get_boolean(char*);
   void cl_help(char*);
   void dump_heading(char*);
   void dump_config_vals();
   void dump_animation_vals();
   void dump_blinking_lights_vals();
   void dump_filter_vals();
   void dump_landmark_vals();
   void dump_location_vals();
   void dump_model_vals();
   void dump_network_vals();
   void dump_radar_icon_vals();
   void dump_round_world_vals();
   void dump_sound_vals();
   void dump_transport_locs_vals();
   void dump_terrain_vals();
   int put_query(int, union query_data*);
   int get_query(int, union query_data*);
};

// EOF
#endif // IMCLASS_DOT_H
