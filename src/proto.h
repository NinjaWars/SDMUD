/***************************************************************************
*   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*   Michael Seifert, Hans Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                          *
*   Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
*   Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                          *
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       ROT 2.0 is copyright 1996-1999 by Russ Walsh                       *
*       DRM 1.0a is copyright 2000-2002 by Joshua Chance Blackwell         *
*        SD 2.0 is copyright 2004-2006 by Patrick Mylund Nielsen           *
***************************************************************************/

#ifndef PROTO_H
#define PROTO_H

/*
 * Our function prototypes.
 * One big lump ... this is every function in SD.
 * -Rhaelar
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define AD	AFFECT_DATA
#define MPC	MPROG_CODE

FILE *file_open args ( ( char *file, const char *mode ) );
//bool file_close args ( ( FILE * fp ) );
char *makedrunk args ( ( char *string, CHAR_DATA * ch ) );

/* arena.c */
void do_start_massarena args ( ( void ) );

/* db.c */
void reset_area args ( ( AREA_DATA * pArea ) );
void reset_room args ( ( ROOM_INDEX_DATA * pRoom ) );

/* string.c */
void string_edit args ( ( CHAR_DATA * ch, char **pString ) );
void string_append args ( ( CHAR_DATA * ch, char **pString ) );
char *string_replace args ( ( char *orig, char *old, char *new ) );
void string_add args ( ( CHAR_DATA * ch, char *argument ) );
char *format_string args ( ( char *oldstring ) );
char *first_arg args ( ( char *argument, char *arg_first, bool fCase ) );
char *string_unpad args ( ( char *argument ) );
char *string_proper args ( ( char *argument ) );

/* olc.c */
bool run_olc_editor args ( ( DESCRIPTOR_DATA * d ) );
char *olc_ed_name args ( ( CHAR_DATA * ch ) );
char *olc_ed_vnum args ( ( CHAR_DATA * ch ) );

/* effect.c */
void acid_effect args ( ( void *vo, int level, int dam, int target ) );
void cold_effect args ( ( void *vo, int level, int dam, int target ) );
void fire_effect args ( ( void *vo, int level, int dam, int target ) );
void poison_effect args ( ( void *vo, int level, int dam, int target ) );
void shock_effect args ( ( void *vo, int level, int dam, int target ) );

/* explore.c */
void setbit args ( ( char *explored, int index ) );
int getbit args ( ( char *explored, int index ) );
int bitcount args ( ( char ch ) );
int roomcount args ( ( CHAR_DATA * ch ) );
int areacount args ( ( CHAR_DATA * ch ) );
int arearooms args ( ( CHAR_DATA * ch ) );
void fwrite_rle args ( ( char *explored, FILE * fp ) );
void fread_rle args ( ( char *explored, FILE * fp ) );

/* fight.c */
bool is_safe args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_safe_mock args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_pk args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_voodood args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_safe_spell args ( ( CHAR_DATA * ch, CHAR_DATA * victim, bool area ) );
void violence_update args ( ( void ) );
void multi_hit args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
int xdamage args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int class, bool show, int suppress, bool stance ) );
bool damage args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int class, bool show, bool stance ) );
bool damage_mock args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int class, bool show, bool stance ) );
bool damage_old args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int class, bool show, bool stance ) );
void update_pos args ( ( CHAR_DATA * victim ) );
void stop_fighting args ( ( CHAR_DATA * ch, bool fBoth ) );
void check_killer args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
char *suppress_bit_name args ( ( int suppress_flags ) );
void do_push_drag args ( ( CHAR_DATA *ch, char *argument, char *verb ) );

/* Stance stuff - Start (fight.c) */
bool	check_stance	args( (CHAR_DATA *ch ) );
int     dambonus        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, bool realdam) );
int     damreduce        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, bool realdam) );
bool    can_counter     args( ( CHAR_DATA *ch, bool forreal) );
bool    can_bypass      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_counter   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt ) );
bool	check_stance	args( (CHAR_DATA *ch ) ); /* Checks that stance is being used */
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dispel	args( ( int dis_level, CHAR_DATA *victim, int sn) );
bool	check_dual_wield args ( ( CHAR_DATA *ch ) );
/* Stance stuff - End */


/* Rhaelar 11'2004 */
void improve_stance args ( ( CHAR_DATA *ch ) );
void skillstance args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* freeze.c */
void tag_update args ((void));
void end_tag args ((void));
void start_tag args ((void));
void check_team_frozen args ((CHAR_DATA * ch));
void auto_tag args ((void));
bool is_tagging args ((CHAR_DATA * ch));
void tag_channel args((CHAR_DATA *ch, char *message));


/* handler.c */
AD *affect_find args ( ( AFFECT_DATA * paf, int sn ) );
void affect_check args ( ( CHAR_DATA * ch, int where, int vector ) );
int count_users args ( ( OBJ_DATA * obj ) );
void deduct_cost args ( ( CHAR_DATA * ch, int cost, int value ) );
void add_cost args ( ( CHAR_DATA * ch, int cost, int value ) );
void affect_enchant args ( ( OBJ_DATA * obj ) );
int check_immune args ( ( CHAR_DATA * ch, int dam_type ) );
int liq_lookup args ( ( const char *name ) );
int material_lookup args ( ( const char *name ) );
int weapon_lookup args ( ( const char *name ) );
int weapon_type args ( ( const char *name ) );
char *weapon_name args ( ( int weapon_Type ) );
int item_lookup args ( ( const char *name ) );
char *item_name args ( ( int item_type ) );
int attack_lookup args ( ( const char *name ) );
int race_lookup args ( ( const char *name ) );
long wiznet_lookup args ( ( const char *name ) );
int class_lookup args ( ( const char *name ) );
bool is_clan args ( ( CHAR_DATA * ch ) );
bool is_clead args ( ( CHAR_DATA * ch ) );
bool is_pkill args ( ( CHAR_DATA * ch ) );
bool is_same_clan args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_clan_obj args ( ( OBJ_DATA * obj ) );
bool clan_can_use args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool is_class_obj args ( ( OBJ_DATA * obj ) );
bool class_can_use args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool is_old_mob args ( ( CHAR_DATA * ch ) );
int get_skill args ( ( CHAR_DATA * ch, int sn ) );
int get_weapon_sn args ( ( CHAR_DATA * ch ) );
int get_weapon_skill args ( ( CHAR_DATA * ch, int sn ) );
int get_age args ( ( CHAR_DATA * ch ) );
void reset_char args ( ( CHAR_DATA * ch ) );
int get_trust args ( ( CHAR_DATA * ch ) );
int get_curr_stat args ( ( CHAR_DATA * ch, int stat ) );
int get_max_train args ( ( CHAR_DATA * ch, int stat ) );
int can_carry_n args ( ( CHAR_DATA * ch ) );
int can_carry_w args ( ( CHAR_DATA * ch ) );
bool is_name args ( ( char *str, char *namelist ) );
bool is_full_name args( ( const char *str, char *namelist ) );
bool is_exact_name args ( ( char *str, char *namelist ) );
void affect_to_char args ( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_to_obj args ( ( OBJ_DATA * obj, AFFECT_DATA * paf ) );
void affect_remove args ( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_remove_obj args ( ( OBJ_DATA * obj, AFFECT_DATA * paf ) );
void affect_strip args ( ( CHAR_DATA * ch, int sn ) );
bool is_affected args ( ( CHAR_DATA * ch, int sn ) );
bool is_shielded args ( ( CHAR_DATA * ch, int sn ) );
void affect_join args ( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void char_from_room args ( ( CHAR_DATA * ch ) );
void char_to_room args ( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
void obj_to_char args ( ( OBJ_DATA * obj, CHAR_DATA * ch ) );
void obj_from_char args ( ( OBJ_DATA * obj ) );
int apply_ac args ( ( int level, OBJ_DATA * obj, int iWear, int type ) );
OD *get_eq_char args ( ( CHAR_DATA * ch, int iWear ) );
void equip_char args ( ( CHAR_DATA * ch, OBJ_DATA * obj, int iWear ) );
void unequip_char args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
int count_obj_list args ( ( OBJ_INDEX_DATA * obj, OBJ_DATA * list ) );
void obj_from_room args ( ( OBJ_DATA * obj ) );
void obj_to_room args ( ( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex ) );
void obj_to_obj args ( ( OBJ_DATA * obj, OBJ_DATA * obj_to ) );
void obj_from_obj args ( ( OBJ_DATA * obj ) );
void extract_obj args ( ( OBJ_DATA * obj ) );
void extract_char args ( ( CHAR_DATA * ch, bool fPull ) );
CD *get_char_room args ( ( CHAR_DATA * ch, char *argument ) );
CD *get_char_world args ( ( CHAR_DATA * ch, char *argument ) );
CD *get_char_mortal args ( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_type args ( ( OBJ_INDEX_DATA * pObjIndexData ) ); 
OD *get_obj_list args ( ( CHAR_DATA * ch, char *argument, OBJ_DATA * list ) );
OD *get_obj_exit args ( ( char *argument, OBJ_DATA * list ) );
OD *get_obj_item args ( ( char *argument, OBJ_DATA * list ) );
OD *get_obj_carry args ( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_wear args ( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_here args ( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_world args ( ( CHAR_DATA * ch, char *argument ) );
OD *create_money args ( ( int platinum, int gold, int silver ) );
int get_obj_number args ( ( OBJ_DATA * obj ) );
int get_obj_weight args ( ( OBJ_DATA * obj ) );
int get_true_weight args ( ( OBJ_DATA * obj ) );
bool room_is_dark args ( ( ROOM_INDEX_DATA * pRoomIndex ) );
bool is_room_owner args ( ( CHAR_DATA * ch, ROOM_INDEX_DATA * room ) );
bool room_is_private args ( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
bool can_see args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_see_obj args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool can_see_room args ( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
bool can_drop_obj args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
char *item_type_name args ( ( OBJ_DATA * obj ) );
char *affect_loc_name args ( ( int location ) );
char *affect_bit_name args ( ( int vector ) );
char *shield_loc_name args ( ( int location ) );
char *shield_bit_name args ( ( int vector ) );
char *extra_bit_name args ( ( int extra_flags ) );
char *wear_bit_name args ( ( int wear_flags ) );
char *room_bit_name args ( ( int room_flags ) );
char *act_bit_name args ( ( int act_flags ) );
char *act2_bit_name args ( ( int act2_flags ) );
char *plr2_bit_name args ( ( int act2_flags ) );
char *off_bit_name args ( ( int off_flags ) );
char *imm_bit_name args ( ( int imm_flags ) );
char *form_bit_name args ( ( int form_flags ) );
char *part_bit_name args ( ( int part_flags ) );
char *weapon_bit_name args ( ( int weapon_flags ) );
char *comm_bit_name args ( ( int comm_flags ) );
char *cont_bit_name args ( ( int cont_flags ) );
bool remove_voodoo args ( ( CHAR_DATA * ch ) );
/* Check if a player is a "real" PK'er */
bool is_real_pkiller args ( ( CHAR_DATA * ch ) );
/* Color checking stuff */
int count_color_string args( (char *string ) );
bool check_color_string args( (char *string ) );
char *smash_colour args ( ( char *buf ) );
/* DNS resolver stuff */
void lookup_address args( (DNS_LOOKUP *darg ) );
bool is_banned args( ( DESCRIPTOR_DATA *d ) );

/* interp.c */
void interpret args ( ( CHAR_DATA * ch, char *argument ) );
bool is_number args ( ( char *arg ) );
int number_argument args ( ( char *argument, char *arg ) );
int mult_argument args ( ( char *argument, char *arg ) );
char *one_argument args ( ( char *argument, char *arg_first ) );

/* magic.c */
int find_spell args ( ( CHAR_DATA * ch, const char *name ) );
int mana_cost ( CHAR_DATA * ch, int min_mana, int level );
int skill_lookup args ( ( const char *name ) );
int slot_lookup args ( ( int slot ) );
bool saves_spell args ( ( int level, CHAR_DATA * victim, int dam_type ) );
void obj_cast_spell args ( ( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ) );
void say_spell args( ( CHAR_DATA *ch, int sn ) );

/* mob_prog.c */
void program_flow args ( ( long vnum, char *source, CHAR_DATA * mob, CHAR_DATA * ch, const void *arg1, const void *arg2 ) );
void mp_act_trigger args ( ( char *argument, CHAR_DATA * mob, CHAR_DATA * ch, const void *arg1, const void *arg2, int type ) );
bool mp_percent_trigger args ( ( CHAR_DATA * mob, CHAR_DATA * ch, const void *arg1, const void *arg2, int type ) );
void mp_bribe_trigger args ( ( CHAR_DATA * mob, CHAR_DATA * ch, int amount ) );
bool mp_exit_trigger args ( ( CHAR_DATA * ch, int dir ) );
void mp_give_trigger args ( ( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj ) );
void mp_greet_trigger args ( ( CHAR_DATA * ch ) );
void mp_hprct_trigger args ( ( CHAR_DATA * mob, CHAR_DATA * ch ) );

/* mob_cmds.c */
void mob_interpret args ( ( CHAR_DATA * ch, char *argument ) );

/* note.c */
//void expire_notes args ( ( void ) );
//void note_append args ( ( CHAR_DATA * ch, char **pString ) );

/* save.c */
void save_char_obj args ( ( CHAR_DATA * ch ) );
bool load_char_obj args ( ( DESCRIPTOR_DATA * d, char *name ) );
/*bool	load_char_reroll  args( ( DESCRIPTOR_DATA *d, char *name ) );*/
void backup_char_obj args ( ( CHAR_DATA * ch ) );

/* skills.c */
bool parse_gen_groups args ( ( CHAR_DATA * ch, char *argument ) );
void list_group_costs args ( ( CHAR_DATA * ch ) );
void list_group_known args ( ( CHAR_DATA * ch ) );
long exp_per_level args ( ( CHAR_DATA * ch, int points ) );
void check_improve args ( ( CHAR_DATA * ch, int sn, bool success, int multiplier ) );
int group_lookup args ( ( const char *name ) );
void gn_add args ( ( CHAR_DATA * ch, int gn ) );
void gn_remove args ( ( CHAR_DATA * ch, int gn ) );
void group_add args ( ( CHAR_DATA * ch, const char *name, bool deduct ) );
void group_remove args ( ( CHAR_DATA * ch, const char *name ) );
int spell_avail args ( ( CHAR_DATA * ch, const char *name ) );

/* special.c */
SF *spec_lookup args ( ( const char *name ) );
char *spec_name args ( ( SPEC_FUN * function ) );
void taunt args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void NPC_set_stance args ( ( CHAR_DATA *ch ) );
void defensive_spell args ( ( CHAR_DATA *ch ) );


/* teleport.c */
RID *room_by_name args ( ( char *target, int level, bool error ) );

/* update.c */
void mobile_update args ( ( void ) );
void weather_update args ( ( void ) );
void char_update args ( ( void ) );
void obj_update args ( ( void ) );
void aggr_update args ( ( void ) );
void olcautosave args ( ( void ) );
void advance_level args ( ( CHAR_DATA * ch ) );
void advance_level_quiet args ( ( CHAR_DATA * ch ) );
void gain_exp args ( ( CHAR_DATA * ch, int gain ) );
void gain_condition args ( ( CHAR_DATA * ch, int iCond, int value ) );
void update_handler args ( ( void ) );
void random_spread args ( ( void ) );
void pkviolent args ( ( CHAR_DATA * ch, int time ) );
void pk_update args ( ( void ) );
void pksafe args ( ( CHAR_DATA * ch, int time ) );
void autosave args ( ( void ) );
void hint_update args ( ( void ) );
void kinky_update args ( ( void ) );
void fuck_update args( ( CHAR_DATA *ch ) );
void sync_stats args ( ( void ) );
void tournament_update args ( ( void ) );
void rtimer_update args ( ( void ) );

/* wizlist.c */
void update_wizlist args ( ( CHAR_DATA * ch, int level ) );
void save_wizlist args ( ( void ) );

/* anclist.c */
void update_anclist args ( ( CHAR_DATA * ch, int level ) );
void save_anclist args ( ( void ) );

/* social-edit.c */
void load_social_table args ( ( void ) );
void save_social_table args ( ( void ) );

/* quest.c */
void do_mob_tell
args ( ( CHAR_DATA * ch, CHAR_DATA * victim, char *argument ) );
void generate_quest args ( ( CHAR_DATA * ch, CHAR_DATA * questman ) );
void quest_update args ( ( void ) );
bool quest_level_diff args ( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
void end_quest args ( ( CHAR_DATA * ch, int time ) );
OD *has_questobj args ( ( CHAR_DATA * ch ) );
void update_questobjs args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
int qobj_cost args ( ( OBJ_DATA * obj ) );

/* act_comm.c */
void check_sex args ( ( CHAR_DATA * ch ) );
void add_follower args ( ( CHAR_DATA * ch, CHAR_DATA * master ) );
void force_quit args ( ( CHAR_DATA * ch, char *argument ) );
void stop_follower args ( ( CHAR_DATA * ch ) );
void nuke_pets args ( ( CHAR_DATA * ch ) );
void die_follower args ( ( CHAR_DATA * ch ) );
bool is_same_group args ( ( CHAR_DATA * ach, CHAR_DATA * bch ) );
void social_channel args ( ( const char *format, CHAR_DATA * ch, const void *arg2, int type ) );
void do_qquote args ( ( CHAR_DATA * ch ) );
void do_gmessage args ( ( char *argument ) );


/* act_enter.c */
RID *get_random_room args ( ( CHAR_DATA * ch ) );
RID *pull_random_room args ( (CHAR_DATA * ch) );

/* act_info.c */
void set_title args ( ( CHAR_DATA * ch, char *title ) );
int compare_mdeaths args ( ( const void *v1, const void *v2 ) );
int compare_mkills args ( ( const void *v1, const void *v2 ) );


/* act_move.c */
void announce_move args((CHAR_DATA *ch, char *message, ROOM_INDEX_DATA *room));
void move_char args ( ( CHAR_DATA * ch, int door, bool follow, bool quiet ) );
bool can_see_sneak args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* act_obj.c */
bool can_loot args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void wear_obj args ( ( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace ) );
BUFFER2 *get_obj
args ( ( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container ) ); 

/* act_wiz.c */
void wiznet
args ( ( char *string, CHAR_DATA * ch, OBJ_DATA * obj, long flag,
         long flag_skip, int min_level ) );

void reboot_recover args ( ( void ) );

ROOM_INDEX_DATA *find_location args ( ( CHAR_DATA * ch, char *arg ) );
bool can_pack args ( ( CHAR_DATA * ch ) );
void reboot_rot args ( ( void ) );

/* act_wiz2.c */
void maintenancecheck args ((void));

/* alias.c */
void substitute_alias args ( ( DESCRIPTOR_DATA * d, char *input ) );

/* auction.c */
void auction_channel args ( ( char *msg, bool iauc ) );
int count_auc ( CHAR_DATA * ch );
void reset_auc ( AUCTION_DATA * auc );
void update_auc ( void );

/* ban.c */
bool check_ban args ( ( char *site, int type ) );
bool check_adr args ( ( char *site, int type ) );

/* comm.c */
void show_string args ( ( struct descriptor_data * d, char *input ) );
void close_socket args ( ( DESCRIPTOR_DATA * dclose ) );
void close_socket args ( ( DESCRIPTOR_DATA * d ) );
void write_to_buffer
args ( ( DESCRIPTOR_DATA * d, const char *txt, int length ) );
void act
args ( ( const char *format, CHAR_DATA * ch, const void *arg1,
         const void *arg2, int type ) );
void act_new
args ( ( const char *format, CHAR_DATA * ch, const void *arg1,
         const void *arg2, int type, int min_pos ) );
char *colour_clear args ( ( CHAR_DATA * ch ) );
char *colour_channel args ( ( int colornum, CHAR_DATA * ch ) );
void send_to_char args ( ( const char *txt, CHAR_DATA * ch ) );
void page_to_char args ( ( const char *txt, CHAR_DATA * ch ) );
void bugf ( char *fmt, ... ) __attribute__ ( ( format ( printf, 1, 2 ) ) );
void sdlogf ( char *fmt, ... ) __attribute__ ( ( format ( printf, 1, 2 ) ) );
void printf_to_char args ( ( CHAR_DATA * ch, char *fmt, ... ) )
    __attribute__ ( ( format ( printf, 2, 3 ) ) );
void xsend_to_char args ( ( const char *txt, CHAR_DATA * ch, int suppress ) );
void xact_new args ( ( const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type, int min_pos, int suppress ) );
char *colour args ( ( char type, CHAR_DATA * ch ) );
void load_reboot_obj args( ( void ) );
void update_lasthost args( (CHAR_DATA *ch) );
void clear_host args( (CHAR_DATA *ch) );
void fwrite_host args( (CHAR_DATA *ch) );
void send_greeting args ( ( DESCRIPTOR_DATA * d ) );

/* db.c */
char *print_flags args ( ( int flag ) );
void boot_db args ( ( void ) );
void area_update args ( ( void ) );
CD *create_mobile args ( ( MOB_INDEX_DATA * pMobIndex ) );
void clone_mobile args ( ( CHAR_DATA * parent, CHAR_DATA * clone ) );
OD *create_object args ( ( OBJ_INDEX_DATA * pObjIndex, int level ) );
void clone_object args ( ( OBJ_DATA * parent, OBJ_DATA * clone ) );
void clear_char args ( ( CHAR_DATA * ch ) );
char *get_extra_descr args ( ( const char *name, EXTRA_DESCR_DATA * ed ) );
MID *get_mob_index args ( ( long vnum ) );
OID *get_obj_index args ( ( long vnum ) );
RID *get_room_index args ( ( long vnum ) );
MPC *get_mprog_index args ( ( long vnum ) );
char fread_letter args ( ( FILE * fp ) );
int fread_number args ( ( FILE * fp ) );
long fread_long args ( ( FILE * fp ) );
long fread_flag args ( ( FILE * fp ) );
char *fread_string args ( ( FILE * fp ) );
char *fread_string_eol args ( ( FILE * fp ) );
void fread_to_eol args ( ( FILE * fp ) );
char *fread_word args ( ( FILE * fp ) );
long flag_convert args ( ( char letter ) );
void *alloc_mem args ( ( int sMem ) );
void *alloc_perm args ( ( int sMem ) );
char *str_dup args ( ( const char *str ) );
void _free_mem args ( ( void *pMem, int sMem, char *file, int line ) );
void _free_string args ( ( char *pstr, char *file, int line ) );
int number_fuzzy args ( ( int number ) );
int number_range args ( ( int from, int to ) );
int number_percent args ( ( void ) );
int number_door args ( ( void ) );
int number_bits args ( ( int width ) );
long number_mm args ( ( void ) );
int dice args ( ( int number, int size ) );
int interpolate args ( ( int level, int value_00, int value_32 ) );
void smash_tilde args ( ( char *str ) );
bool str_cmp args ( ( const char *astr, const char *bstr ) );
bool str_prefix args ( ( const char *astr, const char *bstr ) );
bool str_prefix_c args ( ( const char *astr, const char *bstr ) );
bool str_infix args ( ( const char *astr, const char *bstr ) );
bool str_infix_c args ( ( const char *astr, const char *bstr ) );
char *str_replace args ( ( char *astr, char *bstr, char *cstr ) );
char *str_replace_c args ( ( char *astr, char *bstr, char *cstr ) );
bool str_suffix args ( ( const char *astr, const char *bstr ) );
char *capitalize args ( ( const char *str ) );
void append_file args ( ( CHAR_DATA * ch, char *file, char *str ) );
void debug_log args ( ( char *str ) );
void bug args ( ( const char *str, int param ) );
void log_string args ( ( const char *str ) );
void tail_chain args ( ( void ) );
// void randomize_entrances args ( ( long code ) );
int find_door args ( ( CHAR_DATA * ch, char *arg ) );
bool has_key args ( ( CHAR_DATA * ch, long key ) );
void enter_exit args ( ( CHAR_DATA * ch, char *arg ) );
bool remove_obj args ( ( CHAR_DATA * ch, int iWear, bool fReplace ) );
CD *find_keeper args ( ( CHAR_DATA * ch ) );
int get_cost args ( ( CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy ) );
void obj_to_keeper args ( ( OBJ_DATA * obj, CHAR_DATA * ch ) );
OD *get_obj_keeper
args ( ( CHAR_DATA * ch, CHAR_DATA * keeper, char *argument ) );
bool can_quest args ( ( CHAR_DATA * ch ) );
void bank_update args ( ( void ) );
bool obj_check args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void recursive_clone
args ( ( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * clone ) );
void save_bans args ( ( void ) );
void load_bans args ( ( void ) );
void ban_site args ( ( CHAR_DATA * ch, char *argument, bool fPerm ) );
bool is_stat args ( ( const struct flag_type * flag_table ) );
int flag_lookup2
args ( ( const char *name, const struct flag_type * flag_table ) );
AREA_DATA *get_vnum_area args ( ( int vnum ) );
AREA_DATA *get_area_data args ( ( int vnum ) );
int flag_value
args ( ( const struct flag_type * flag_table, char *argument ) );
char *flag_string args ( ( const struct flag_type * flag_table, int bits ) );
void add_reset
args ( ( ROOM_INDEX_DATA * room, RESET_DATA * pReset, int index ) );
void save_clanlist args ( ( int clannum ) );
void load_clanlist args ( ( void ) );
char *player_clanwho args ( ( CHAR_DATA * ch ) );
bool process_ansi_output args ( ( DESCRIPTOR_DATA * d ) );
void fix_sex args ( ( CHAR_DATA * ch ) );
void center_to_char args ( ( char *argument, CHAR_DATA * ch, int columns ) );
void do_font args ( ( CHAR_DATA * ch, char *argument ) );
void init_mm args ( ( void ) );
void load_area args ( ( FILE * fp ) );
void new_load_area args ( ( FILE * fp ) );  /* OLC */
//void load_helps args ( ( FILE * fp ) );
void load_helps args( ( FILE *fp, char *fname ) );
void load_old_mob args ( ( FILE * fp ) );
void load_mobiles args ( ( FILE * fp ) );
void load_old_obj args ( ( FILE * fp ) );
void load_objects args ( ( FILE * fp ) );
void load_resets args ( ( FILE * fp ) );
void load_rooms args ( ( FILE * fp ) );
void load_shops args ( ( FILE * fp ) );
void load_specials args ( ( FILE * fp ) );
void load_notes args ( ( void ) );
void load_mobprogs args ( ( FILE * fp ) );
void load_wizlist args ( ( void ) );
void load_anclist args ( ( void ) );
void fix_exits args ( ( void ) );
void fix_mobprogs args ( ( void ) );
void sort_areas_by_level args ( ( void ) );
void load_bank args ( ( void ) );
void load_bounties args ( ( void ) );


/* mem.c - memory prototypes. */
RESET_DATA *new_reset_data args ( ( void ) );
void free_reset_data args ( ( RESET_DATA * pReset ) );
AREA_DATA *new_area args ( ( void ) );
void free_area args ( ( AREA_DATA * pArea ) );
EXIT_DATA *new_exit args ( ( void ) );
void free_exit args ( ( EXIT_DATA * pExit ) );
ROOM_INDEX_DATA *new_room_index args ( ( void ) );
void free_room_index args ( ( ROOM_INDEX_DATA * pRoom ) );
SHOP_DATA *new_shop args ( ( void ) );
void free_shop args ( ( SHOP_DATA * pShop ) );
OBJ_INDEX_DATA *new_obj_index args ( ( void ) );
void free_obj_index args ( ( OBJ_INDEX_DATA * pObj ) );
MOB_INDEX_DATA *new_mob_index args ( ( void ) );
void free_mob_index args ( ( MOB_INDEX_DATA * pMob ) );
void show_liqlist args ( ( CHAR_DATA * ch ) );
void show_damlist args ( ( CHAR_DATA * ch ) );
char *mprog_type_to_name args ( ( int type ) );
MPROG_LIST *new_mprog args ( ( void ) );
void free_mprog args ( ( MPROG_LIST * mp ) );
MPROG_CODE *new_mpcode args ( ( void ) );
void free_mpcode args ( ( MPROG_CODE * pMcode ) );
void new_reset args ( ( ROOM_INDEX_DATA * pR, RESET_DATA * pReset ) );
long convert_level args ( ( char *arg ) );
long get_area_level args ( ( AREA_DATA * pArea ) );
bool check_critical_hit args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void check_assist args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_dodge args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_parry args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_shield_block args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void dam_message
args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, bool immune,
         int verbose, bool singular ) );
void process_shields args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void death_cry args ( ( CHAR_DATA * ch ) );
void group_gain args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
int xp_compute
args ( ( CHAR_DATA * gch, CHAR_DATA * victim, int total_levels ) );
void make_corpse args ( ( CHAR_DATA * ch, CHAR_DATA * killer ) );
void one_hit
args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary ) );
void one_hit_mock
args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary ) );
void mob_hit args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
void raw_kill args ( ( CHAR_DATA * victim, CHAR_DATA * killer ) );
void set_fighting args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void disarm args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_critical args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_counter
args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt ) );
void check_arena args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void affect_modify args ( ( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd ) );
bool is_friend args ( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void home_buy args ( ( CHAR_DATA * ch ) );
void home_sell args ( ( CHAR_DATA * ch, char *argument ) );
void home_describe args ( ( CHAR_DATA * ch ) );
int flag_lookup
args ( ( const char *name, const struct flag_type * flag_table ) );
void say_spell args ( ( CHAR_DATA * ch, int sn ) );
bool saves_dispel args ( ( int dis_level, int spell_level, int duration ) );
bool check_dispel args ( ( int dis_level, CHAR_DATA * victim, int sn ) );
int powerf args ( ( int value, int pow ) );
int keyword_lookup args ( ( const char **table, char *keyword ) );
int num_eval args ( ( int lval, int oper, int rval ) );
CD *get_random_char args ( ( CHAR_DATA * mob ) );
int count_people_room args ( ( CHAR_DATA * mob, int iFlag ) );
int get_order args ( ( CHAR_DATA * ch ) );
bool has_item
args ( ( CHAR_DATA * ch, long vnum, sh_int item_type, bool fWear ) );
bool get_mob_vnum_room args ( ( CHAR_DATA * ch, long vnum ) );
bool get_obj_vnum_room args ( ( CHAR_DATA * ch, long vnum ) );
int cmd_eval
args ( ( long vnum, char *line, int check, CHAR_DATA * mob, CHAR_DATA * ch,
         const void *arg1, const void *arg2, CHAR_DATA * rch ) );
void expand_arg
args ( ( char *buf, const char *format, CHAR_DATA * mob, CHAR_DATA * ch,
         const void *arg1, const void *arg2, CHAR_DATA * rch ) );
int count_spool args ( ( CHAR_DATA * ch, NOTE_DATA * spool ) );
// void save_notes args ( ( int type ) );
//void append_note args ( ( NOTE_DATA * pnote ) );
bool is_note_to args ( ( CHAR_DATA * ch, NOTE_DATA * pnote ) );
void note_attach args ( ( CHAR_DATA * ch, int type ) );
void note_remove args ( ( CHAR_DATA * ch, NOTE_DATA * pnote, bool delete ) );
void update_read args ( ( CHAR_DATA * ch, NOTE_DATA * pnote ) );
void note_remove_quiet args ( ( NOTE_DATA * pnote ) );
void display_resets args ( ( CHAR_DATA * ch ) );
char *fix_string args ( ( const char *str ) );
void save_area_list args ( ( void ) );
char *fwrite_flag args ( ( long flags, char buf[] ) );
void save_mobprogs args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_mobile args ( ( FILE * fp, MOB_INDEX_DATA * pMobIndex ) );
void save_mobiles args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_object args ( ( FILE * fp, OBJ_INDEX_DATA * pObjIndex ) );
void save_objects args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_rooms args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_specials args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_door_resets args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_resets args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_shops args ( ( FILE * fp, AREA_DATA * pArea ) );
void save_area args ( ( AREA_DATA * pArea ) );
int is_qobj args ( ( OBJ_DATA * obj ) );
void affect_join_obj args ( ( OBJ_DATA * obj, AFFECT_DATA * paf ) );
void add_apply
args ( ( OBJ_DATA * obj, int loc, int mod, int where, int type, int dur,
         long bit, int level ) );
void quest_usage args ( ( CHAR_DATA * ch ) );
void quest_where args ( ( CHAR_DATA * ch, char *what ) );
void scan_list
args ( ( ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch, sh_int depth,
         sh_int door ) );
void scan_char
args ( ( CHAR_DATA * victim, CHAR_DATA * ch, sh_int depth, sh_int door ) );
void sign_attach args ( ( CHAR_DATA * ch, int type ) );
void list_group_chosen args ( ( CHAR_DATA * ch ) );
void load_social args ( ( FILE * fp, struct social_type * social ) );
void save_social args ( ( const struct social_type * s, FILE * fp ) );
int social_lookup args ( ( const char *name ) );
bool dragon args ( ( CHAR_DATA * ch, char *spell_name ) );
char *del_last_line args ( ( char *string ) );
int hit_gain args ( ( CHAR_DATA * ch ) );
int mana_gain args ( ( CHAR_DATA * ch ) );
int move_gain args ( ( CHAR_DATA * ch ) );
bool check_social args ( ( CHAR_DATA * ch, char *command, char *argument ) );
/* local procedures */
void load_thread
args ( ( char *name, NOTE_DATA ** list, int type, time_t free_time ) );
void parse_note args ( ( CHAR_DATA * ch, char *argument, int type ) );
bool hide_note args ( ( CHAR_DATA * ch, NOTE_DATA * pnote ) );
void convert_mob args ( ( MOB_INDEX_DATA * mob ) );
void convert_obj args ( ( OBJ_INDEX_DATA * obj ) );
void assign_area_vnum args ( ( long vnum ) );   /* OLC */

void convert_mobile args ( ( MOB_INDEX_DATA * pMobIndex ) );    /* OLC ROM */
void convert_objects args ( ( void ) ); /* OLC ROM */
void convert_object args ( ( OBJ_INDEX_DATA * pObjIndex ) );    /* OLC ROM */
char *format_obj_to_char
args ( ( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort ) );
BUFFER2 *show_list_to_char
args ( ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing ) );
void show_char_to_char_0 args ( ( CHAR_DATA * victim, CHAR_DATA * ch ) );
void show_char_to_char_1 args ( ( CHAR_DATA * victim, CHAR_DATA * ch ) );
void show_char_to_char args ( ( CHAR_DATA * list, CHAR_DATA * ch ) );
bool check_blind args ( ( CHAR_DATA * ch ) );
void show_obj_stats args ( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool check_parse_name args ( ( char *name ) );
bool check_reconnect args ( ( DESCRIPTOR_DATA * d, char *name, bool fConn ) );
bool check_playing args ( ( DESCRIPTOR_DATA * d, char *name ) );
int main args ( ( int argc, char **argv ) );
void nanny args ( ( DESCRIPTOR_DATA * d, char *argument ) );
bool process_output args ( ( DESCRIPTOR_DATA * d, bool fPrompt ) );
void read_from_buffer args ( ( DESCRIPTOR_DATA * d ) );
void stop_idling args ( ( CHAR_DATA * ch ) );
void bust_a_prompt args ( ( CHAR_DATA * ch ) );
void fwrite_char args ( ( CHAR_DATA * ch, FILE * fp ) );
void fwrite_obj
args ( ( CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest ) );
void fwrite_pet args ( ( CHAR_DATA * pet, FILE * fp ) );
void fread_char args ( ( CHAR_DATA * ch, FILE * fp ) );
void fread_pet args ( ( CHAR_DATA * ch, FILE * fp ) );
void fread_obj args ( ( CHAR_DATA * ch, FILE * fp ) );
CHAR_DATA *find_forger args ( ( CHAR_DATA * ch ) );
void halt_mud args ( ( int sig ) );
void _quit args ( ( int iError, char *file, int line ) );
/*void bprintf args ( ( BUFFER2 * buffer, char *, ... ) )
    __attribute__ ( ( format ( printf, 2, 3 ) ) ); */


/* recycle.c */
void recycle_descriptors args( ( void ) );
void recycle_dnss args( ( void ) );
char *buffer_string args ( ( BUFFER2 * buffer ) );

/* cprintf.c */
int cprintf ( char *buf, char *ptr, ... ) __attribute__ ((format(printf, 2, 3)));
int cnprintf ( char *buf, int maxlen, char *ptr, ... ) __attribute__ ((format(printf, 3, 4)));

/* board.c */
NOTE_DATA *new_note ( void );
int board_number args ( ( const BOARD_DATA *board ) );
void save_notes ( void );
int unread_notes args ( ( CHAR_DATA *ch, BOARD_DATA *board ) );
int total_notes args ( ( CHAR_DATA *ch, BOARD_DATA *board ) );

/* math.c */
bool chance args ( ( int num ) );
bool bigchance args ( ( int num ) );

/* events.c */
void dailyevents args ( ( void ) );
void happymoment args ( ( void ) );
void merrychristmas args ( ( void ) );
void nowaitquesting args ( ( void ) );
void event_doubleqp args ( ( void ) );
void event_doublexp args ( ( void ) );
void event_doubledamage args ( ( void ) );
void event_doublestance args ( ( void ) );
void event_doublepracs args ( ( void ) );
void event_marketcrash args ( ( void ) );

/* hedit.c */
HELP_AREA *get_help_area ( HELP_DATA *help );

/* olc_save.c */
void save_helps args ( ( FILE *fp, HELP_AREA *ha ) );
void save_other_helps args ( ( CHAR_DATA *ch ) );

/* bounty.c */
bool is_bountied args ( ( CHAR_DATA *ch ) );
void save_bounties args ( ( void ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef AD

#endif
