/***************************************************************************
*   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*   Michael Seifert, Hans Henrik St�rfeldt, Tom Madsen, and Katja Nyboe.   *
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

/* stuff for providing a crash-proof buffer */

/*#define MAX_BUF		65536
#define MAX_BUF_LIST 	12
#define BASE_BUF 	1024 */

/* valid states */
/*#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2 */

/* ban data recycling */
#define BD BAN_DATA
BD *new_ban args ( ( void ) );
void free_ban args ( ( BAN_DATA * ban ) );
#undef BD

/* wizlist data recycling */
#define WD WIZ_DATA
WD *new_wiz args ( ( void ) );
void free_wiz args ( ( WIZ_DATA * ban ) );
#undef WD

/* wizlist data recycling */
#define AD ANC_DATA
AD *new_anc args ( ( void ) );
void free_anc args ( ( ANC_DATA * ban ) );
#undef AD

/* clan data recycling */
#define CL CLN_DATA
CL *new_cln args ( ( void ) );
#undef CL

/* clan data recycling */
#define MB MBR_DATA
MB *new_mbr args ( ( void ) );
void free_mbr args ( ( MBR_DATA * mbr ) );
#undef MB

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD *new_descriptor args ( ( void ) );
void free_descriptor args ( ( DESCRIPTOR_DATA * d ) );
#undef DD

/* char gen data recycling */
#define GD GEN_DATA
GD *new_gen_data args ( ( void ) );
void free_gen_data args ( ( GEN_DATA * gen ) );
#undef GD

/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED *new_extra_descr args ( ( void ) );
void free_extra_descr args ( ( EXTRA_DESCR_DATA * ed ) );
#undef ED

/* affect recycling */
#define AD AFFECT_DATA
AD *new_affect args ( ( void ) );
void free_affect args ( ( AFFECT_DATA * af ) );
#undef AD

/* object recycling */
#define OD OBJ_DATA
OD *new_obj args ( ( void ) );
void free_obj args ( ( OBJ_DATA * obj ) );
#undef OD

/* character recyling */
#define CD CHAR_DATA
#define PD PC_DATA
CD *new_char args ( ( void ) );
void free_char args ( ( CHAR_DATA * ch ) );
PD *new_pcdata args ( ( void ) );
void free_pcdata args ( ( PC_DATA * pcdata ) );
#undef PD
#undef CD

/* mob id and memory procedures */
#define MD MEM_DATA
long get_pc_id args ( ( void ) );
long get_mob_id args ( ( void ) );
MD *new_mem_data args ( ( void ) );
void free_mem_data args ( ( MEM_DATA * memory ) );
MD *find_memory args ( ( MEM_DATA * memory, long id ) );
#undef MD

/* buffer procedures */

/*BUFFER *new_buf args ( ( void ) );
BUFFER *new_buf_size args ( ( int size ) );
void free_buf args ( ( BUFFER * buffer ) );
bool add_buf args ( ( BUFFER * buffer, char *string ) );
void clear_buf args ( ( BUFFER * buffer ) );
char *buf_string args ( ( BUFFER * buffer ) ); 
int get_size args ( ( int val ) );*/

AUCTION_DATA *new_auction ( void );
void free_auction ( AUCTION_DATA * auction );

HELP_AREA *     new_had         args( ( void ) );
HELP_DATA *     new_help        args( ( void ) );
void            free_help       args( ( HELP_DATA * ) );


/* bounty recycling */
#define BD BOUNTY_DATA
BD      *new_bounty args( (void) );
void    free_bounty args( (BOUNTY_DATA *b) );
#undef BD
