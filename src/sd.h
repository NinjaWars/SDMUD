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

#include <pthread.h>

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef short int sh_int;
typedef unsigned char bool;
#endif

//#define WWW_WHO		"/home/httpd/vhosts/sdmud.com/cgi-bin/sd.who"

/* Additional junk by Rhaelar */

#include <setjmp.h>
#define INSTASEGFAULT area_last->next = (AREA_DATA *)area_last->name;
#define NUL '\0'
#define false 0
#define true 1
#define MUD_NAME     "SD Version"
#define	RELEASE      "Stable"
#define	COMPILE_TIME "{DCompiled on      {x: {d" __DATE__ " at " __TIME__ "{x.\n\r"

const char *versionToString args ( ( int version ) );


/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define CLEAR           "[0m"  /* Resets Colour        */
#define BLINK           "[5m"  /* Blink                */
#define C_BLACK		"[0;30m"   /* Normal Colours       */
#define C_RED           "[0;31m"
#define C_GREEN         "[0;32m"
#define C_YELLOW        "[0;33m"
#define C_BLUE          "[0;34m"
#define C_MAGENTA       "[0;35m"
#define C_CYAN          "[0;36m"
#define C_WHITE         "[0;37m"
#define C_D_GREY        "[1;30m"   /* Light Colors         */
#define C_B_RED         "[1;31m"
#define C_B_GREEN       "[1;32m"
#define C_B_YELLOW      "[1;33m"
#define C_B_BLUE        "[1;34m"
#define C_B_MAGENTA     "[1;35m"
#define C_B_CYAN        "[1;36m"
#define C_B_WHITE       "[1;37m"
#define R_BLACK		"[0m[0;30m"   /* Reset Colours       */
#define R_RED           "[0m[0;31m"
#define R_GREEN         "[0m[0;32m"
#define R_YELLOW        "[0m[0;33m"
#define R_BLUE          "[0m[0;34m"
#define R_MAGENTA       "[0m[0;35m"
#define R_CYAN          "[0m[0;36m"
#define R_WHITE         "[0m[0;37m"
#define R_D_GREY        "[0m[1;30m"   /* Reset Light         */
#define R_B_RED         "[0m[1;31m"
#define R_B_GREEN       "[0m[1;32m"
#define R_B_YELLOW      "[0m[1;33m"
#define R_B_BLUE        "[0m[1;34m"
#define R_B_MAGENTA     "[0m[1;35m"
#define R_B_CYAN        "[0m[1;36m"
#define R_B_WHITE       "[0m[1;37m"
#define BACKSPACE       "[1D"
#define BACKSPACEB      "[2D"
#define BACKSPACEC      "[3D"
#define BACKSPACED      "[4D"
#define BACKSPACEE      "[5D"
#define BACKSPACEF      "[6D"
#define BACKSPACEG      "[7D"
#define BACKSPACEH      "[8D"
#define BACKSPACEI      "[9D"
#define BACKSPACEJ      "[10D"
#define FLASHING        "\x1B[5m"

/*
 * Structure types.
 */
typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct ban_data BAN_DATA;
typedef struct wiz_data WIZ_DATA;
typedef struct anc_data ANC_DATA;
typedef struct cln_data CLN_DATA;
typedef struct mbr_data MBR_DATA;
//typedef struct buf_type BUFFER;
typedef struct char_data CHAR_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct help_area_data HELP_AREA;
typedef struct kill_data KILL_DATA;
typedef struct mem_data MEM_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct gen_data GEN_DATA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;
typedef struct mprog_list MPROG_LIST;
typedef struct mprog_code MPROG_CODE;
typedef struct auction_data AUCTION_DATA;
//typedef struct gquest_data GQUEST;
typedef struct illegal_data ILLEGAL_DATA;
typedef struct tag_data TAG_DATA;
typedef struct bounty_data BOUNTY_DATA;
/*
 * Function types.
 */
typedef void DO_FUN args ( ( CHAR_DATA * ch, char *argument ) );
typedef bool SPEC_FUN args ( ( CHAR_DATA * ch ) );
typedef void SPELL_FUN
args ( ( int sn, int level, CHAR_DATA * ch, void *vo, int target ) );

#define CH_CMD(fun)  void fun (CHAR_DATA *ch, char *argument)
#define MAGIC(fun)   void fun (int sn, int level, CHAR_DATA *ch, void *vo, int target)
#define WOLLA(fun)   void fun (void)

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4000
#define MAX_INPUT_LENGTH	  600
#define PAGELEN			   22
#define MAX_EXPLORE              8192
// #define MAX_EXPLORE              3000

/* I am lazy :) */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH
#define stc send_to_char
#define ptc printf_to_char

#define STATUS_LOOKUP       0   // New Descriptor, in lookup by default.
#define STATUS_DONE         1   // The lookup is done.
#define STATUS_WAIT         2   // Closed while in thread.
#define STATUS_CLOSED       3   // Closed, ready to be recycled.

typedef struct  dns_lookup               DNS_LOOKUP;

struct dns_lookup
{
  DNS_LOOKUP       *next;
  DESCRIPTOR_DATA  *d;
  char             *buf;
  sh_int           status;
};

extern          DNS_LOOKUP      *       dns_free;
extern          DNS_LOOKUP      *       dns_list;
extern          pthread_mutex_t         memory_mutex;

/* 
 * Moved this here from db.c for the new unoverflowable
 * buffer by Erwin Andreasen. Rhaelar 12'2004
 */
// #define MAX_MEM_LIST 12
#define MAX_MEM_LIST 48
#include "buffer.h"

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define	SHARE_VALUE		  200   /* The base share value */
/*#define MAX_SOCIALS               255*/
#define MAX_SKILL		  272
#define MAX_GROUP		   100
#define MAX_IN_GROUP		   40
#define MAX_TRACK		   20
#define MAX_LIQUID		   36
#define MAX_DAMAGE_MESSAGE	   46
#define MAX_ALIAS		   25
#define MAX_HOST		   25
#define MAX_FORGET		   10
#define MAX_DUPES		    7
#define MAX_CLASS		   28
#define MAX_PC_RACE		   60
#define MAX_CLAN		   15
#define MAX_LEVEL		  216
#define MAX_QUOTES                 26
#define MAX_NEST		  200
#define MAX_DEBUG		    3
//#define	MAX_GQUEST_MOB             25

#define LEVEL_HERO		   (MAX_LEVEL - 15)
#define LEVEL_ANCIENT              (MAX_LEVEL - 14)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 13)

#define PULSE_PER_SECOND            4
/*
   Pulse_Violence was originally 3, slowing down combat pace a bit,
   too much spam so quickly.
   
   - Rhaelar 11'2004 
*/
#define PULSE_VIOLENCE            ( 4 * PULSE_PER_SECOND)
/* Pulse_Mobile was originally 4, saving some CPU - Rhaelar 11'2004 */
#define PULSE_MOBILE              ( 6 * PULSE_PER_SECOND) 
//#define PULSE_MUSIC               ( 6 * PULSE_PER_SECOND)
#define PULSE_TICK                (60 * PULSE_PER_SECOND)
// #define PULSE_QUEST               (60 * PULSE_PER_SECOND)
#define PULSE_AREA                (120 * PULSE_PER_SECOND)
#define PULSE_AUCTION             ( 10 * PULSE_PER_SECOND)
//#define PULSE_TELEPORT            (20 * PULSE_PER_SECOND)
#define MINIMUM_BID			10
#define AUCTION_LENGTH		  	5

#define HEADIMP			MAX_LEVEL
#define	IMPLEMENTOR		(MAX_LEVEL - 1)
#define CREATOR			(MAX_LEVEL - 2)
#define SUPREME			(MAX_LEVEL - 3)
#define DEITY			(MAX_LEVEL - 4)
#define GOD			(MAX_LEVEL - 5)
#define FATE			(MAX_LEVEL - 6)
#define SPIRIT			(MAX_LEVEL - 7)
#define ANGEL			(MAX_LEVEL - 8)
#define LEGEND			(MAX_LEVEL - 9)
#define IMMORTAL		(MAX_LEVEL - 10)
#define DEMI			(MAX_LEVEL - 11)
#define KNIGHT			(MAX_LEVEL - 12)
#define SQUIRE                  (MAX_LEVEL - 13)
#define ANCIENT			LEVEL_ANCIENT
#define HERO			LEVEL_HERO

#define VALUE_SILVER		    0
#define VALUE_GOLD		    1
#define VALUE_PLATINUM		    2

/*
 * Site ban structure.
 */

#define BAN_SUFFIX		A
#define BAN_PREFIX		B
#define BAN_NEWBIES		C
#define BAN_ALL			D
#define BAN_PERMIT		E
#define BAN_PERMANENT		F

struct ban_data
{
    BAN_DATA *next;
    bool valid;
    sh_int ban_flags;
    sh_int level;
    char *name;
};

struct struckdrunk
{
    int min_drunk_level;
    int number_of_rep;
    char *replacement[11];
};

struct wiz_data
{
    WIZ_DATA *next;
    bool valid;
    sh_int level;
    char *name;
    char *job;
};

struct anc_data
{
    ANC_DATA *next;
    bool valid;
    sh_int level;
    char *name;
};

struct cln_data
{
    CLN_DATA *next;
    bool valid;
    sh_int clan;
    int members;
    char *name;
    MBR_DATA *list;
    long cbalance;
};

struct mbr_data
{
    MBR_DATA *next;
    bool valid;
    char *name;
};
/*
struct buf_type
{
    BUFFER2 *next;
    bool valid;
    sh_int state;               
    sh_int size;                
    char *string;               
};
*/

/*
 * Bounty stuff -- by Gomi no sensei
 * Heavily modified by Rhaelar
 */
struct bounty_data
{
    BOUNTY_DATA *next;
    bool        valid;
    char *      name;
    int         amount;
};

/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct time_info_data
{
    int hour;
    int day;
    int month;
    int year;
};

struct weather_data
{
    int mmhg;
    int change;
    int sky;
    int sunlight;
};

struct illegal_data
{
       ILLEGAL_DATA *next; /* pointer to next node */
       char mode;          /* mode                 */
       char *word;         /* word                 */
       char *who;          /* who illegalized      */
       char *why;          /* why done             */
};

/*
 * Connected state for a channel.
 */
#define CON_REBOOT_RECOVER            -1
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_RACE		 6
#define CON_GET_NEW_SEX			 7
#define CON_GET_NEW_CLASS		 8
#define CON_GET_ALIGNMENT		 9
#define CON_DEFAULT_CHOICE		10
#define CON_GEN_GROUPS			11
#define CON_READ_IMOTD			13
#define CON_READ_MOTD			14
#define CON_BREAK_CONNECT		15
#define CON_WIZ                         17
#define CON_COLOUR_LOGIN                18
#define CON_BEGIN_REROLL                19
#define CON_NOTE_TO                     20
#define CON_NOTE_SUBJECT                21
#define CON_NOTE_EXPIRE                 22
#define CON_NOTE_TEXT                   23
#define CON_NOTE_FINISH                 24
#define CON_NOT_PLAYING			25
#define CON_GET_HOMETOWN		26


struct auction_data
{
    AUCTION_DATA *next;
    OBJ_DATA *item;
    CHAR_DATA *owner;
    CHAR_DATA *high_bidder;
    sh_int status;
    long current_bid;
    long platinum_held;
    long min_bid;
    bool valid;
};

/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
    DESCRIPTOR_DATA *next;
    DESCRIPTOR_DATA *snoop_by;
    CHAR_DATA *character;
    CHAR_DATA *original;
    bool valid;
    char *host;
    sh_int descriptor;
    sh_int connected;
    bool fcommand;
    char inbuf[4 * MAX_INPUT_LENGTH];
    char incomm[MAX_INPUT_LENGTH];
    char inlast[MAX_INPUT_LENGTH];
    char *              run_buf;
    char *              run_head;
    int repeat;
    char *outbuf;
    int outsize;
    int outtop;
    char *showstr_head;
    char *showstr_point;
    void *pEdit;                /* OLC */
    char **pString;             /* OLC */
    int editor;                 /* OLC */
    bool ansi;
    sh_int lookup_status;
};

/*
 * Attribute bonus structures.
 */
struct str_app_type
{
    sh_int tohit;
    sh_int todam;
    sh_int carry;
    sh_int wield;
};

struct int_app_type
{
    sh_int learn;
};

struct wis_app_type
{
    sh_int practice;
};

struct dex_app_type
{
    sh_int defensive;
};

struct con_app_type
{
    sh_int hitp;
    sh_int shock;
};

/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4

/*
 * Help table types.
 */
struct help_data
{
    HELP_DATA *next;
    HELP_DATA *next_area;
    sh_int level;
    char *keyword;
    char *text;
};

struct help_area_data
{
        HELP_AREA *     next;
        HELP_DATA *     first;
        HELP_DATA *     last;
        AREA_DATA *     area;
        char *          filename;
        bool            changed;
};

/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct shop_data
{
    SHOP_DATA *next;            /* Next shop in list            */
    long keeper;                /* Vnum of shop keeper mob      */
    sh_int buy_type[MAX_TRADE]; /* Item types shop will buy     */
    sh_int profit_buy;          /* Cost multiplier for buying   */
    sh_int profit_sell;         /* Cost multiplier for selling  */
    sh_int open_hour;           /* First opening hour           */
    sh_int close_hour;          /* First closing hour           */
};

/*
 * Per-class stuff.
 */

#define MAX_GUILD 	3
#define MAX_STATS 	5
#define STAT_STR 	0
#define STAT_INT	1
#define STAT_WIS	2
#define STAT_DEX	3
#define STAT_CON	4

/*
* Tier definitions
*/
#define TIER_01		A
#define TIER_02		B
#define TIER_03         C
#define TIER_04         D
#define TIER_05		E

struct class_type
{
    char *name;                 /* the full name of the class */
    char who_name[4];           /* Three-letter name for 'who'  */
    sh_int attr_prime;          /* Prime attribute              */
    sh_int weapon;              /* First weapon                 */
    sh_int guild[MAX_GUILD];    /* Vnum of guild rooms          */
    sh_int skill_adept;         /* Maximum skill level          */
    sh_int thac0_00;            /* Thac0 for level  0           */
    sh_int thac0_32;            /* Thac0 for level 32           */
    sh_int hp_min;              /* Min hp gained on leveling    */
    sh_int hp_max;              /* Max hp gained on leveling    */
    bool fMana;                 /* Class gains mana on level    */
    char *base_group;           /* base skills gained           */
    char *default_group;        /* default skills gained        */
    sh_int tier;                /* tier this class belongs to                   */
    sh_int tier_next;           /* tier gained when you HERO and reroll */
};

struct item_type
{
    int type;
    char *name;
};

struct weapon_type
{
    char *name;
    long vnum;
    sh_int type;
    sh_int *gsn;
};

struct wiznet_type
{
    char *name;
    long flag;
    int level;
};

struct attack_type
{
    char *name;                 /* name */
    char *noun;                 /* message */
    int damage;                 /* damage class */
};

struct quote_type
{
    char *text;
    char *by;
};

struct race_type
{
    char *name;                 /* call name of the race */
    bool pc_race;               /* can be chosen by pcs */
    long act;                   /* act bits for the race */
    long act2;                  /* act2 bits for the race */
    long aff;                   /* aff bits for the race */
    long off;                   /* off bits for the race */
    long imm;                   /* imm bits for the race */
    long res;                   /* res bits for the race */
    long vuln;                  /* vuln bits for the race */
    long shd;                   /* shd bits for the race */
    long form;                  /* default form flag for the race */
    long parts;                 /* default parts for the race */
};
/*
struct gquest_data
{
    int mobs[MAX_GQUEST_MOB];
    char *who;
    int mob_count;
    int timer;
    int involved;
    int qpoints;
    int gold;
    int minlevel;
    int maxlevel;
    int running;
    int next;
};

#define		GQUEST_OFF              0
#define		GQUEST_WAITING		1
#define		GQUEST_RUNNING		2

bool load_gquest_data args ( ( void ) );
bool save_gquest_data args ( ( void ) );
void auto_gquest args ( ( void ) );
bool start_gquest args ( ( CHAR_DATA * ch, char *argument ) );
void end_gquest args ( ( void ) );
void gquest_update args ( ( void ) );
void gquest_wait args ( ( void ) );
bool generate_gquest args ( ( CHAR_DATA * who ) );
int count_gqmobs args ( ( CHAR_DATA * ch ) );
int is_gqmob args ( ( CHAR_DATA * ch, long vnum ) );
void reset_gqmob args ( ( CHAR_DATA * ch, long value ) );
bool is_random_gqmob args ( ( long vnum ) );
*/
void     free_runbuf     args( ( DESCRIPTOR_DATA *d ) );

struct pc_race_type             /* additional data for pc races */
{
    char *name;                 /* MUST be in race_type */
    char who_name[6];
    sh_int points;              /* cost in points of the race */
    sh_int class_mult[MAX_CLASS];   /* exp multiplier for class, * 100 */
    char *skills[5];            /* bonus skills for the race */
    sh_int stats[MAX_STATS];    /* starting stats */
    sh_int max_stats[MAX_STATS];    /* maximum stats */
    sh_int size;                /* aff bits for the race */
    long recall;
    sh_int tier;
};

struct spec_type
{
    char *name;                 /* special function name */
    SPEC_FUN *function;         /* the function */
};

/*
 * Data structure for notes.
 */

/*
#define NOTE_NOTE	0
#define NOTE_IDEA	1
#define NOTE_PENALTY	2
#define NOTE_NEWS	3
#define NOTE_CHANGES	4
#define NOTE_SIGN	5
#define NOTE_WEDDINGS	6
#define NOTE_JOKES	7
*/
struct note_data
{
    NOTE_DATA *next;
    bool valid;
    sh_int type;
    char *sender;
    char *date;
    char *to_list;
    char *subject;
    char *text;
    time_t date_stamp;
    time_t expire;
};

/*
 * An affect.
 */
struct affect_data
{
    AFFECT_DATA *next;
    bool valid;
    sh_int where;
    sh_int type;
    sh_int level;
    sh_int duration;
    sh_int location;
    sh_int modifier;
    long bitvector;
};

/* where definitions */
#define TO_AFFECTS	0
#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
#define TO_WEAPON	5
#define TO_SHIELDS	6

struct tag_data
{
  int status;
  int timer;
  int next;
  int playing;
};

#define TAG_OFF 0
#define TAG_ISWAIT 1
#define TAG_ISPLAY 2

#define TAG_PLAYING		        (A)
#define TAG_FROZEN		        (B)
#define TAG_RED			        (C)
#define TAG_BLUE			    (D)
#define TAG_WAITING             (E)

#define FTAG_WAIT_ROOM      24000
#define FTAG_MIN_VNUM		24001
#define FTAG_MAX_VNUM		24036

/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
    sh_int number;
    sh_int killed;
    sh_int kills;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_FIDO		   3090
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404
#define MOB_VNUM_BANKER            3399
#define MOB_VNUM_PATROLMAN	   2106
#define MOB_VNUM_CORPSE		   3174
#define MOB_VNUM_ANIMATE	   3175
#define MOB_VNUM_DEMON		   3181
#define MOB_VNUM_CLAYTON	   10009
#define MOB_VNUM_FAMILIAR          230
#define GROUP_VNUM_TROLLS	   2100
#define GROUP_VNUM_OGRES	   2101
#define	MOB_VNUM_REGISTAR          3397
#define MOB_VNUM_AIR_ELEMENTAL     27115
#define MOB_VNUM_FIRE_ELEMENTAL    27116
#define MOB_VNUM_EARTH_ELEMENTAL   27117
#define MOB_VNUM_WATER_ELEMENTAL   27118


/* RT ASCII conversions -- used so we can have letters in this file */

#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864    /* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee 		       1073741824

#define STANCE_NONE            -1
#define	STANCE_NORMAL		0
#define	STANCE_VIPER		1
#define	STANCE_CRANE		2
#define	STANCE_CRAB	        3
#define	STANCE_MONGOOSE 	4
#define STANCE_BULL	        5
#define STANCE_MANTIS		6
#define STANCE_DRAGON		7
#define STANCE_TIGER		8
#define	STANCE_MONKEY		9
#define	STANCE_SWALLOW	   	10

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		(A)     /* Auto set for mobs    */
#define ACT_SENTINEL	    	(B) /* Stays in one room    */
#define ACT_SCAVENGER	      	(C) /* Picks up objects     */
#define ACT_KEY			(D)
#define ACT_RANGER		(E)
#define ACT_AGGRESSIVE		(F) /* Attacks PC's         */
#define ACT_STAY_AREA		(G) /* Won't leave area     */
#define ACT_WIMPY		(H)
#define ACT_PET			(I)     /* Auto set for pets    */
#define ACT_TRAIN		(J)     /* Can train PC's       */
#define ACT_PRACTICE		(K) /* Can practice PC's    */
#define ACT_DRUID		(L)
#define ACT_NO_BODY		(M)     /* Will not leave a corpse */
#define ACT_NB_DROP		(N)     /* Corpseless will drop all */
#define ACT_UNDEAD		(O)
#define ACT_VAMPIRE		(P)
#define ACT_CLERIC		(Q)
#define ACT_MAGE		(R)
#define ACT_THIEF		(S)
#define ACT_WARRIOR		(T)
#define ACT_NOALIGN		(U)
#define ACT_NOPURGE		(V)
#define ACT_OUTDOORS		(W)
#define ACT_IS_SATAN		(X)
#define ACT_INDOORS		(Y)
#define ACT_IS_PRIEST		(Z)
#define ACT_IS_HEALER		(aa)
#define ACT_GAIN		(bb)
#define ACT_UPDATE_ALWAYS	(cc)
#define ACT_IS_CHANGER		(dd)
#define ACT_QUESTMASTER         (ee)

/* ACT2 bits */
#define ACT2_FORGER              (A)
#define ACT2_DEALER              (B)
#define ACT2_BANKER              (C)
#define ACT2_BOUNTY		 (D)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		15
#define DAM_OTHER               16
#define DAM_HARM		17
#define DAM_CHARM		18
#define DAM_SOUND		19

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH		(O)
#define ASSIST_ALL       	(P)
#define ASSIST_ALIGN	        (Q)
#define ASSIST_RACE    	     	(R)
#define ASSIST_PLAYERS      	(S)
#define ASSIST_GUARD        	(T)
#define ASSIST_VNUM		(U)
#define OFF_FEED		(V)

/* return values for check_imm */
#define IS_NORMAL		0
#define IS_IMMUNE		1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		(S)
#define IMM_SOUND		(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)

/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SOUND		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SOUND		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E) /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB		(S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD		(cc)

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE		(K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS		(Y)

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		(A)
#define AFF_DETECT_SCRY		(B)
#define AFF_DETECT_EVIL		(C)
#define AFF_DETECT_INVIS	(D)
#define AFF_DETECT_MAGIC	(E)
#define AFF_DETECT_HIDDEN	(F)
#define AFF_DETECT_GOOD		(G)
#define AFF_FAERIE_FIRE		(I)
#define AFF_INFRARED		(J)
#define AFF_CURSE		(K)
#define AFF_FARSIGHT		(L)
#define AFF_POISON		(M)
#define AFF_SNEAK		(P)
#define AFF_HIDE		(Q)
#define AFF_SLEEP		(R)
#define AFF_CHARM		(S)
#define AFF_FLYING		(T)
#define AFF_PASS_DOOR		(U)
#define AFF_HASTE		(V)
#define AFF_CALM		(W)
#define AFF_PLAGUE		(X)
#define AFF_WEAKEN		(Y)
#define AFF_DARK_VISION		(Z)
#define AFF_BERSERK		(aa)
#define AFF_SWIM		(bb)
#define AFF_REGENERATION        (cc)
#define AFF_SLOW		(dd)
/* #define AFF_BANISH              (ee) */
#define AFF_STRANGLE		(ee)

/*
 * More Bits for 'shielded_by'.
 * Used in #MOBILES.
 */
#define SHD_PROTECT_VOODOO	(A)
#define SHD_INVISIBLE		(B)
#define SHD_ICE			(C)
#define SHD_FIRE		(D)
#define SHD_SHOCK		(E)
#define SHD_ACID                (F)
#define SHD_POISON              (G)
#define SHD_BRIAR               (I)
#define SHD_SANCTUARY		(H)
#define SHD_PROTECT_EVIL	(N)
#define SHD_PROTECT_GOOD	(O)
#define SHD_DIVINE_PROTECTION   (P)
#define SHD_MANA                (Q)
/* #define SHD_SWALK               (R)  */
#define SHD_WARD                (R)
#define SHD_MISTBLEND           (S)

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2

/* AC types */
#define AC_PIERCE			0
#define AC_BASH				1
#define AC_SLASH			2
#define AC_EXOTIC			3

/* dice */
#define DICE_NUMBER			0
#define DICE_TYPE			1
#define DICE_BONUS			2

/* size */
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE	      1
#define OBJ_VNUM_GOLD_ONE	      2
#define OBJ_VNUM_GOLD_SOME	      3
#define OBJ_VNUM_SILVER_SOME	      4
#define OBJ_VNUM_COINS		      5
#define OBJ_VNUM_PLATINUM_ONE	      6
#define OBJ_VNUM_PLATINUM_SOME	      7

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17
#define OBJ_VNUM_BLOOD		     18

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_PORTAL		     25
#define OBJ_VNUM_VOODOO		     51
#define OBJ_VNUM_EXIT		     52
#define OBJ_VNUM_CHAIN		     53
#define OBJ_VNUM_BAG		     54

#define OBJ_VNUM_ROSE		   3331

#define OBJ_VNUM_PIT		   3010

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_SPEAR	   3717
#define OBJ_VNUM_SCHOOL_STAFF	   3718
#define OBJ_VNUM_SCHOOL_AXE	   3719
#define OBJ_VNUM_SCHOOL_FLAIL	   3720
#define OBJ_VNUM_SCHOOL_WHIP	   3721
#define OBJ_VNUM_SCHOOL_POLEARM    3722

#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_MAP		   3162
#define OBJ_VNUM_WMAP		   3163
#define OBJ_VNUM_EMAP		   3169

#define OBJ_VNUM_WHISTLE	   2116

#define OBJ_VNUM_QUEST_SIGN	   19
#define	OBJ_VNUM_POTION		   3384
#define	OBJ_VNUM_SCROLL		   3385

#define	OBJ_VNUM_QDIAMOND	   3388
#define	OBJ_VNUM_CUBIC		   3386
#define	OBJ_VNUM_DPOUCH		   3383
#define	OBJ_VNUM_CPOUCH		   3387
#define OBJ_VNUM_DIAMOND	   3377

#define OBJ_VNUM_SURVIVAL_PACK	   3032
#define OBJ_VNUM_SURVIVAL_A	   2140
#define OBJ_VNUM_SURVIVAL_B	   2127
#define OBJ_VNUM_SURVIVAL_C	   2122
#define OBJ_VNUM_SURVIVAL_D	   2124
#define OBJ_VNUM_SURVIVAL_E	   2135
#define OBJ_VNUM_SURVIVAL_F	   2136
#define OBJ_VNUM_SURVIVAL_G	   2131
#define OBJ_VNUM_SURVIVAL_H	   2128
#define OBJ_VNUM_SURVIVAL_I	   2125
#define OBJ_VNUM_SURVIVAL_J	   2126
#define OBJ_VNUM_SURVIVAL_K	   3162
#define OBJ_VNUM_SURVIVAL_L	   2120
#define OBJ_VNUM_SURVIVAL_M	   2130
#define OBJ_VNUM_SURVIVAL_N	   2134
#define OBJ_VNUM_SURVIVAL_O	   2132
#define OBJ_VNUM_SURVIVAL_P	   2137
#define OBJ_VNUM_SURVIVAL_Q	   2123
#define OBJ_VNUM_SURVIVAL_R	   2129
#define OBJ_VNUM_SURVIVAL_S	   2138
#define OBJ_VNUM_SURVIVAL_T	   2138
#define OBJ_VNUM_SURVIVAL_U	   2121
#define OBJ_VNUM_SURVIVAL_V	   2139
#define OBJ_VNUM_SURVIVAL_W	   9222
#define OBJ_VNUM_SURVIVAL_X	   9224
#define OBJ_VNUM_PROTOPLASM	  20050

#define OBJ_VNUM_BED                 55
#define OBJ_VNUM_MACE               230

#define OBJ_VNUM_QUEST_PILL	     50
#define OBJ_VNUM_IQP_PILL	     34
#define OBJ_VNUM_FIDO_POO	     2905
#define OBJ_ITEM_SOUL		     24
#define OBJ_VNUM_DEMON_TOKEN_FOUND   26
#define OBJ_VNUM_ANGEL_TOKEN_FOUND   27
#define OBJ_VNUM_DEMON_TOKEN_BOUGHT  32
#define OBJ_VNUM_ANGEL_TOKEN_BOUGHT  33
#define OBJ_VNUM_STEAK		     28
#define OBJ_VNUM_CHRISTMAS_COOKIE    35
#define MG_WEAPON	             31
#define OBJ_VNUM_SEVERED_BOUNTY_HEAD 36



/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_CLOTHING		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PROTECT		     27
#define ITEM_MAP		     28
#define ITEM_PORTAL		     29
#define ITEM_WARP_STONE		     30
#define ITEM_ROOM_KEY		     31
#define ITEM_GEM		     32
#define ITEM_JEWELRY		     33
#define ITEM_JUKEBOX		     34
#define ITEM_DEMON_STONE	     35
#define ITEM_EXIT		     36
#define ITEM_PIT		     37
#define ITEM_SLOT_MACHINE            38

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		(A)
#define ITEM_HUM		(B)
#define ITEM_DARK		(C)
#define ITEM_LOCK		(D)
#define ITEM_EVIL		(E)
#define ITEM_INVIS		(F)
#define ITEM_MAGIC		(G)
#define ITEM_NODROP		(H)
#define ITEM_BLESS		(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)
#define ITEM_NOPURGE		(O)
#define ITEM_ROT_DEATH		(P)
#define ITEM_VIS_DEATH		(Q)
#define ITEM_NOSAC		(R)
#define ITEM_NONMETAL		(S)
#define ITEM_NOLOCATE		(T)
#define ITEM_MELT_DROP		(U)
#define ITEM_HAD_TIMER		(V)
#define ITEM_SELL_EXTRACT	(W)
#define ITEM_BURN_PROOF		(Y)
#define ITEM_NOUNCURSE		(Z)
#define ITEM_QUEST		(aa)
#define ITEM_FORCED		(bb)
#define ITEM_QUESTPOINT		(cc)
#define ITEM_NOSTEAL		(dd)

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD		(N)
#define ITEM_HOLD		(O)
#define ITEM_NO_SAC		(P)
#define ITEM_WEAR_FLOAT		(Q)
#define ITEM_WEAR_FACE		(R)
#define ITEM_WEAR_ANKLE         (S)
#define ITEM_WEAR_TATTOO        (T)
#define ITEM_WEAR_AURA          (U)
#define ITEM_WEAR_EAR           (V)
#define ITEM_WEAR_CTATTOO       (W)

/* weapon class */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_FLAIL		6
#define WEAPON_WHIP		7
#define WEAPON_POLEARM		8

#define MAX_WEAPON              8

/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)
#define WEAPON_MANADRAIN        (I)

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH		(C)
#define GATE_BUGGY		(D)
#define GATE_RANDOM		(E)

/* furniture flags */
#define STAND_AT		(A)
#define STAND_ON		(B)
#define STAND_IN		(C)
#define SIT_AT			(D)
#define SIT_ON			(E)
#define SIT_IN			(F)
#define REST_AT			(G)
#define REST_ON			(H)
#define REST_IN			(I)
#define SLEEP_AT		(J)
#define SLEEP_ON		(K)
#define SLEEP_IN		(L)
#define PUT_AT			(M)
#define PUT_ON			(N)
#define PUT_IN			(O)
#define PUT_INSIDE		(P)

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVES		     20
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_SPELL_AFFECT	     25

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8
#define CONT_PUT_ON		     16

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CORNER	      3
#define ROOM_VNUM_CHAT		   3001
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_TEMPLEB	   3365
#define ROOM_VNUM_PIT		   3367
/*#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_ALTARB	   3366*/
#define ROOM_VNUM_ALTAR		   3014
#define ROOM_VNUM_ALTARB	   3014
#define ROOM_VNUM_MS		   3014
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_BALANCE	   4500
#define ROOM_VNUM_CIRCLE	   4400
#define ROOM_VNUM_DEMISE	   4201
#define ROOM_VNUM_HONOR		   4300
#define ROOM_VNUM_CHAIN		   21408
#define ROOM_VNUM_CLANS           70707
#define ROOM_VNUM_ARENA           50000
#define ROOM_VNUM_ALOSER          50015
#define ROOM_VNUM_AWINNER         50014
#define ROOM_VNUM_MORGUE           3001
#define ROOM_VNUM_DONATION         3367
#define ROOM_VNUM_ALTAR_MUTANT      242
#define ROOM_VNUM_SCATTER	  21416

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		(A)
#define ROOM_NO_MOB		(C)
#define ROOM_INDOORS		(D)

#define ROOM_PRIVATE		(J)
#define ROOM_SAFE		(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_RECALL		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_HEROES_ONLY	(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_LAW		(S)
#define ROOM_NOWHERE		(T)
#define ROOM_LOCKED		(X)
#define ROOM_ARENA              (Y)
#define ROOM_HOME               (Z)
#define ROOM_TELEPORT           (B)

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5

/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      (A)
#define EX_CLOSED		      (B)
#define EX_LOCKED		      (C)
#define EX_PICKPROOF		      (F)
#define EX_NOPASS		      (G)
#define EX_EASY			      (H)
#define EX_HARD			      (I)
#define EX_INFURIATING		      (J)
#define EX_NOCLOSE		      (K)
#define EX_NOLOCK		      (L)

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_FLOAT		     18
#define WEAR_SECONDARY		     19
#define WEAR_FACE		     20
#define WEAR_ANKLE_L                 21
#define WEAR_ANKLE_R                 22
#define WEAR_TATTOO                  23
#define WEAR_AURA                    24
#define WEAR_EAR_L                   25
#define WEAR_EAR_R                   26
#define WEAR_CTATTOO             27
#define MAX_WEAR		     28

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2
#define COND_HUNGER		      3

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_SITTING		      6
#define POS_FIGHTING		      7
#define POS_STANDING		      8

/* Status of Arena */

#define FIGHT_OPEN                    0
#define FIGHT_START                   1
#define FIGHT_BUSY                    2
#define FIGHT_LOCK                    3

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		(A)     /* Don't EVER set.      */

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define PLR_NOCOLOUR		(B)

/* RT auto flags */
#define PLR_AUTOASSIST		(C)
#define PLR_AUTOEXIT		(D)
#define PLR_AUTOLOOT		(E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD		(G)
#define PLR_AUTOSPLIT		(H)

/* RT personal flags */
#define PLR_NOTRAN		(I)
#define PLR_AUTOPEEK		(J)
#define PLR_SETTITLE	(K)
#define PLR_NORESTORE		(L)
#define PLR_NOTITLE		(M)
#define PLR_HOLYLIGHT		(N)
#define PLR_CANLOOT		(P)
#define PLR_NOSUMMON		(Q)
#define PLR_NOFOLLOW		(R)
#define PLR_QUESTOR             (S)
#define PLR_LONER		(T)

/* 2 bits reserved, S-T */

/* penalty flags */
#define PLR_PERMIT		(U)
#define PLR_LOG			(W)
#define PLR_DENY		(X)
#define PLR_FREEZE		(Y)
#define PLR_TWIT		(Z)
#define PLR_KEY			(bb)
#define PLR_NOCLAN		(cc)
#define PLR_REROLL		(dd)
#define PLR_WASQUESTING         (ee) /* (ee) is unused */

/*arena*/
#define PLR2_CHALLENGER         (A)
#define PLR2_CHALLENGED         (B)
#define PLR2_CONSENT            (C)
#define PLR2_WIPED              (D)
#define PLR2_TRUE_TRUST         (E)
#define PLR2_NO_EXP		(F)
#define PLR2_PKVIOLENT		(G)
#define PLR2_PKSAFE		(H)
#define PLR2_NOARENA		(I)
#define PLR2_PERM_TWIT		(J)
#define PLR2_MASS_ARENA		(K)
#define PLR2_MASS_JOINER	(L)
#define PLR2_MARRIED		(M)
#define PLR2_HELPER		(N)
#define PLR2_INACTIVE		(O)
//#define PLR2_MULTIPLAY		(P)

/* Verbose options */
#define VERBOSE_FLAGS		(A)
#define	VERBOSE_FLAG_EFFECTS 	(B)
#define	VERBOSE_DODGE		(C)
#define	VERBOSE_STD		(D)
#define VERBOSE_BURN		(E)
#define	VERBOSE_SHIELD		(F)
#define VERBOSE_SHIELD_COMP	(G)


/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF            	(B)
#define COMM_NOWIZ              (C)
#define COMM_NOOOC              (D)
#define COMM_NOGOSSIP           (E)
#define COMM_NOASK              (F)
#define COMM_NOMUSIC            (G)
#define COMM_NOCLAN		(H)
#define COMM_NOQUOTE		(I)
#define COMM_NORACE             (K)
#define COMM_NOANNOUNCE         (J) 
#define COMM_NOPRAY		(P)
#define COMM_NOAUCTION          (K)
/* #define COMM_NOARENA		(J) */
/*
#define COMM_TRUE_TRUST		(K)*/
#define COMM_COMPACT		(L)
#define COMM_BRIEF		(M)
#define COMM_PROMPT		(N)
#define COMM_COMBINE		(O)
/* #define COMM_TELNET_GA		(P) */
#define COMM_SHOW_AFFECTS	(Q)
#define COMM_NOGRATS		(R)
/*#define COMM_WIPED		(S)*/
#define COMM_CURSE              (S)
#define COMM_NOHINT		(Y)
/* #define COMM_NOCHEER		(P) */ /* P bliver ikke brugt */
#define COMM_AFK		(Z)
#define COMM_LONG		(aa)
#define COMM_STORE		(bb)

/* penalties */
#define COMM_NOEMOTE		(T)
#define COMM_NOSHOUT		(U)
#define COMM_NOTELL		(V)
#define COMM_NOCHANNELS		(W)
#define COMM_NOCGOSSIP          (X)
#define COMM_NOQGOSSIP		(cc)
#define COMM_NOSOCIAL           (dd)
#define COMM_NOGSOCIAL          (ee)
#define COMM_NOSAY		(J)

/* WIZnet flags */
#define WIZ_ON			(A)
#define WIZ_TICKS		(B)
#define WIZ_LOGINS		(C)
#define WIZ_SITES		(D)
#define WIZ_LINKS		(E)
#define WIZ_DEATHS		(F)
#define WIZ_RESETS		(G)
#define WIZ_MOBDEATHS		(H)
#define WIZ_FLAGS		(I)
#define WIZ_PENALTIES		(J)
#define WIZ_SACCING		(K)
#define WIZ_LEVELS		(L)
#define WIZ_SECURE		(M)
#define WIZ_SWITCHES		(N)
#define WIZ_SNOOPS		(O)
#define WIZ_RESTORE		(P)
#define WIZ_LOAD		(Q)
#define WIZ_NEWBIE		(R)
#define WIZ_PREFIX		(S)
#define WIZ_SPAM		(T)
#define WIZ_MEM         (U)
#define WIZ_BUGS        (V)
#define WIZ_DROPS	(X)

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
    MOB_INDEX_DATA *next;
    SPEC_FUN *spec_fun;
    SHOP_DATA *pShop;
    MPROG_LIST *mprogs;
    AREA_DATA *area;            /* OLC */
    long vnum;
    long group;
    bool new_format;
    sh_int count;
    sh_int killed;
    char *player_name;
    char *short_descr;
    char *long_descr;
    char *description;
    long act;
    long act2;
    long affected_by;
    long shielded_by;
    sh_int alignment;
    sh_int level;
    int hitroll;
    long hit[3];
    long mana[3];
    long damage[3];
    int ac[4];
    sh_int dam_type;
    long off_flags;
    long imm_flags;
    long res_flags;
    long vuln_flags;
    sh_int start_pos;
    sh_int default_pos;
    sh_int sex;
    sh_int race;
    long wealth;
    long form;
    long parts;
    sh_int size;
    char *material;
    long mprog_flags;
    char *die_descr;
    char *say_descr;
    sh_int kills;
};

/* memory settings */
#define MEM_CUSTOMER	A
#define MEM_SELLER	B
#define MEM_HOSTILE	C
#define MEM_AFRAID	D

/* memory for mobs */
struct mem_data
{
    MEM_DATA *next;
    bool valid;
    int id;
    int reaction;
    time_t when;
};

#include "board.h"

/*
 * One character (PC or NPC).
 */
struct char_data
{
    CHAR_DATA *next;
    CHAR_DATA *next_in_room;
    CHAR_DATA *master;
    CHAR_DATA *leader;
    CHAR_DATA *fighting;
    CHAR_DATA *reply;
    CHAR_DATA *pet;
    CHAR_DATA *mprog_target;
    MEM_DATA *memory;
    SPEC_FUN *spec_fun;
    MOB_INDEX_DATA *pIndexData;
    DESCRIPTOR_DATA *desc;
    AFFECT_DATA *affected;
//    NOTE_DATA *pnote;
    OBJ_DATA *carrying;
    OBJ_DATA *on;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *was_in_room;
    AREA_DATA *zone;
    PC_DATA *pcdata;
    GEN_DATA *gen_data;
    bool valid;
    bool hastimer;
    bool attacker;
    bool on_quest;
    bool wedpost;
    char *name;
    long id;
    sh_int version;
    char *short_descr;
    char *long_descr;
    char *die_descr;
    char *say_descr;
    char *description;
    char *prompt;
    char *prefix;
    sh_int group;
    sh_int clan;
    sh_int clead;
    sh_int clan_rank;
    sh_int invited;
    sh_int sex;
    sh_int class;
    sh_int race;
    sh_int level;
    sh_int trust;
    int color;
    int color_auc;
    int color_cgo;
    int color_cla;
    int color_con;
    int color_dis;
    int color_fig;
    int color_gos;
    int color_gra;
    int color_gte;
    int color_imm;
    int color_mob;
    int color_mus;
    int color_opp;
    int color_qgo;
    int color_que;
    int color_quo;
    int color_roo;
    int color_say;
    int color_sho;
    int color_tel;
    int color_wit;
    int color_wiz;
    int played;
    int lines;                  /* for the pager */
    int stunned;
    time_t llogoff;
    time_t logon;
    sh_int timer;
    sh_int wait;
    sh_int daze;
    long hit;
    long max_hit;
    long mana;
    long max_mana;
    long move;
    long max_move;
    long platinum;
    long gold;
    long silver;
    long exp;
    int qps;
    int tells;
    long act;
    long act2;
    long comm;                  /* RT added to pad the vector */
    long wiznet;                /* wiz stuff */
    long imm_flags;
    long res_flags;
    long vuln_flags;
    sh_int invis_level;
    sh_int incog_level;
    sh_int ghost_level;
    long affected_by;
    long shielded_by;
    sh_int position;
    sh_int practice;
    sh_int train;
    int carry_weight;
    sh_int carry_number;
    sh_int llines;
//    sh_int song_ed_number;
    sh_int saving_throw;
    sh_int alignment;
    int hitroll;
    int damroll;
    int armor[4];
    int wimpy;
    /* tracking */
    sh_int track_to[MAX_TRACK];
    sh_int track_from[MAX_TRACK];
    /* stats */
    sh_int perm_stat[MAX_STATS];
    sh_int mod_stat[MAX_STATS];
    /* parts stuff */
    long form;
    long parts;
    sh_int size;
    char *material;
    /* mobile stuff */
    long off_flags;
    int damage[3];
    sh_int dam_type;
    sh_int start_pos;
    sh_int default_pos;

    sh_int mprog_delay;

    CHAR_DATA *challenger;      /*  person who challenged you */
    CHAR_DATA *challenged;      /*  person who you challenged */
    CHAR_DATA *gladiator;       /*  ARENA player wagered on */

    int isquestmob;

    int stance_mob_vnum;
    int stance [11];
    sh_int hometown;
    long extra;
};

/*
 * Data which only PC's have.
 */
struct pc_data
{
    PC_DATA *next;
    //BUFFER *buffer;
    BUFFER2 *buffer;
    bool valid;
    char *pwd;
    char *bamfin;
    char *bamfout;
    char *title;
    char *who_descr;
    char *socket;

/*    time_t last_note;
    time_t last_idea;
    time_t last_penalty;
    time_t last_news;
    time_t last_changes;
    time_t last_weddings;
    time_t last_jokes; */
    long perm_hit;
    long perm_mana;
    long perm_move;
    sh_int true_sex;
    int last_level;
    int recall;
    int tier;
    int ctier;
    int pkset;
    sh_int condition[4];
    sh_int learned[MAX_SKILL];
    bool group_known[MAX_GROUP];
    sh_int points;
    bool confirm_delete;
    bool confirm_reroll;
    char *forget[MAX_FORGET];
    char *dupes[MAX_DUPES];
    char *alias[MAX_ALIAS];
    char *alias_sub[MAX_ALIAS];
    int security;               /* OLC *//* Builder security */
    long pkkills;
    long pkdeaths;
    long balance;
    long shares;
    char explored[MAX_EXPLORE];
    int plr_wager;              /*  ARENA amount wagered     */
    int awins;                  /*  ARENA number of wins     */
    int alosses;                /*  ARENA number of losses  */
/* sh_int            clan_rank; *//* 0 for unguilded and loner 1-10 otherwise */
    sh_int nextquest;
    sh_int countdown;
    long questobj;
    long questmob;
    long questgiver;
    long questloc;
    long questpoints;
    char *spouse;
//    long gq_mobs[MAX_GQUEST_MOB];
    int verbose;
    int pkvalue;
    CLN_DATA *clan_data;
    CHAR_DATA *         tracking;
    sh_int              track_count;
    char *cname; 
    sh_int pkviolenttime;
    sh_int pksafetime;
    int twins; /* Tournament wins */
    int tkills; /* Tournament kills */
    int tdeaths; /* Tournament deaths */
    long tag_flags;
    int times_rerolled;
    BOARD_DATA * board;			/* The current board 	    */
    time_t last_note[MAX_BOARD];	/* last note for the boards */
    NOTE_DATA * in_progress;
    int timesplayed;
    int mkill;
    int mdeath;
    /*char *rmessage;*/
    int fake_stance;
    int exiletime;
    int noclantime;
    int rerolltimer;
    int assassinatetimer;
    int jailtime;
    char *lasthost[MAX_HOST];
    //char *firsthost;
};

/* Data for generating characters -- only used during generation */
struct gen_data
{
    GEN_DATA *next;
    bool valid;
    bool skill_chosen[MAX_SKILL];
    bool group_chosen[MAX_GROUP];
    int points_chosen;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0

struct liq_type
{
    char *liq_name;
    char *liq_color;
    sh_int liq_affect[5];
};

/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
    EXTRA_DESCR_DATA *next;     /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};

/*
 * Prototype for an object.
 */
struct obj_index_data
{
    OBJ_INDEX_DATA *next;
    EXTRA_DESCR_DATA *extra_descr;
    AFFECT_DATA *affected;
    AREA_DATA *area;            /* OLC */
    bool new_format;
    char *name;
    char *short_descr;
    char *description;
    long vnum;
    sh_int reset_num;
    char *material;
    sh_int item_type;
    long extra_flags;
    long wear_flags;
    int level;
    sh_int condition;
    sh_int count;
    sh_int weight;
    int cost;
    long value[5];
    sh_int clan;
    sh_int class;
};

/*
 * One object.
 */
struct obj_data
{
    OBJ_DATA *next;
    OBJ_DATA *next_content;
    OBJ_DATA *contains;
    OBJ_DATA *in_obj;
    OBJ_DATA *on;
    CHAR_DATA *carried_by;
    EXTRA_DESCR_DATA *extra_descr;
    AFFECT_DATA *affected;
    OBJ_INDEX_DATA *pIndexData;
    ROOM_INDEX_DATA *in_room;
    bool valid;
    bool enchanted;
    char *owner;
    char *killer;
    char *name;
    char *short_descr;
    char *description;
    sh_int item_type;
    long extra_flags;
    int wear_flags;
    sh_int wear_loc;
    sh_int weight;
    int cost;
    sh_int level;
    sh_int condition;
    char *material;
    sh_int timer;
    long value[5];
    sh_int clan;
    sh_int class;
};

/*
 * Exit data.
 */
struct exit_data
{
    union
    {
        ROOM_INDEX_DATA *to_room;
        long vnum;
    }
    u1;
    sh_int exit_info;
    long key;
    char *keyword;
    char *description;
    EXIT_DATA *next;            /* OLC */
    int rs_flags;               /* OLC */
    int orig_door;              /* OLC */
};

/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
    RESET_DATA *next;
    char command;
    long arg1;
    sh_int arg2;
    long arg3;
    sh_int arg4;
};

/*
 * Area definition.
 */
struct area_data
{
    AREA_DATA *next;
    HELP_AREA *helps;
    RESET_DATA *reset_first;
    RESET_DATA *reset_last;
    char *file_name;
    char *name;
    char *credits;
    sh_int age;
    sh_int nplayer;
    long low_range;
    long high_range;
    long min_vnum;
    long max_vnum;
    bool empty;
    char *builders;             /* OLC *//* Listing of */
    int vnum;                   /* OLC *//* Area vnum  */
    int area_flags;             /* OLC */
    int security;               /* OLC *//* Value 1-9  */
    AREA_DATA *next_sort;
    char *repop_msg;
};

/*
 * Room type.
 */
struct room_index_data
{
    ROOM_INDEX_DATA *next;
    CHAR_DATA *people;
    OBJ_DATA *contents;
    EXTRA_DESCR_DATA *extra_descr;
    AREA_DATA *area;
    EXIT_DATA *exit[12];
    EXIT_DATA *old_exit[12];
    RESET_DATA *reset_first;    /* OLC */
    RESET_DATA *reset_last;     /* OLC */
    char *name;
    char *description;
    char *owner;
    long vnum;
    int room_flags;
    sh_int light;
    sh_int sector_type;
    sh_int heal_rate;
    sh_int mana_rate;
    sh_int clan;
    int                 distance_from_source;
    ROOM_INDEX_DATA *   shortest_from_room;
    ROOM_INDEX_DATA *   shortest_next_room;
    int tele_dest;
};

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000

/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OBJ_CHAR_DEF	    5
#define TAR_OBJ_CHAR_OFF	    6
#define TAR_OBJ_TRAN		    7

#define TARGET_CHAR		    0
#define TARGET_OBJ		    1
#define TARGET_ROOM		    2
#define TARGET_NONE		    3

/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
    char *name;                 /* Name of skill                */
    sh_int skill_level[MAX_CLASS];  /* Level needed by class        */
    sh_int rating[MAX_CLASS];   /* How hard it is to learn      */
    SPELL_FUN *spell_fun;       /* Spell pointer (for spells)   */
    sh_int target;              /* Legal targets                */
    sh_int minimum_position;    /* Position for caster / user   */
    sh_int *pgsn;               /* Pointer to associated gsn    */
    bool socket;                /* Allow same socket use?       */
    sh_int slot;                /* Slot for #OBJECT loading     */
    sh_int min_mana;            /* Minimum mana used            */
    sh_int beats;               /* Waiting time after use       */
    char *noun_damage;          /* Damage message               */
    char *msg_off;              /* Wear off message             */
    char *msg_obj;              /* Wear off message for obects  */
};

struct group_type
{
    char *name;
    sh_int rating[MAX_CLASS];
    char *spells[MAX_IN_GROUP];
};

/*
 * MOBprog definitions
 */
#define TRIG_ACT	(A)
#define TRIG_BRIBE	(B)
#define TRIG_DEATH	(C)
#define TRIG_ENTRY	(D)
#define TRIG_FIGHT	(E)
#define TRIG_GIVE	(F)
#define TRIG_GREET	(G)
#define TRIG_GRALL	(H)
#define TRIG_KILL	(I)
#define TRIG_HPCNT	(J)
#define TRIG_RANDOM	(K)
#define TRIG_SPEECH	(L)
#define TRIG_EXIT	(M)
#define TRIG_EXALL	(N)
#define TRIG_DELAY	(O)
#define TRIG_SURR	(P)

struct mprog_list
{
    int trig_type;
    char *trig_phrase;
    long vnum;
    char *code;
    MPROG_LIST *next;
    bool valid;
};

struct mprog_code
{
    long vnum;
    char *code;
    MPROG_CODE *next;
};

/*
 * Utility macros.
 */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))
#define IS_NULLSTR(str)         ((str) == NULL || (str)[0] == '\0')
#define IS_STANCE(ch, sn)	(ch->stance[0] == sn)

 #define new_fclose(fp)                                          \
 do {                                                            \
 if ( (fp) == NULL )                                             \
 { bug("Halting new_fclose call with NULL fp (not previously set to NULL maybe)", 0 ); } \
 if ( (fp) != NULL ) {                                           \
 fclose((fp)); }                                                 \
 } while(0)

/* use for free'ing and str_dup'ing in 1 shot */
#define replace_string( pstr, nstr )            \
{                                               \
  _free_string( (pstr), __FILE__, __LINE__ );   \
  pstr = str_dup( (nstr) );                     \
}

#define free_mem( pMem, sMem )  _free_mem(pMem, sMem, __FILE__, __LINE__)

#define free_string( pstr )  _free_string(pstr, __FILE__, __LINE__)

/*
 * Gold Transformation Macros --Jouster
 */
#define TRANSFORM_TO_GOLD(ch,oldbal,oldgold)            \
{						\
    oldgold = ch->gold;				\
    if ( !IS_NPC( ch ) )			\
    {						\
	oldbal = ch->pcdata->balance;		\
	ch->gold += ( 100 * oldbal );		\
	ch->pcdata->balance = 0;		\
    }						\
    ch->gold += ( 100 * ch->platinum );		\
    ch->platinum = 0;				\
}

#define TRANSFORM_FROM_GOLD(ch,oldbal,oldgold)                  \
{							\
    int transform;					\
    transform = ch->gold;				\
    if ( !IS_NPC( ch ) )				\
    {							\
	if ( transform >= ( oldbal * 100 ) )		\
	{						\
	    ch->pcdata->balance = oldbal;		\
	    transform -= ( oldbal * 100 );		\
	}						\
	else						\
	{						\
	    ch->pcdata->balance = ( transform / 100 );	\
	    transform = ( transform % 100 );		\
	}						\
    }							\
    if ( transform / 100 >= 1 )				\
    {							\
	ch->gold = ( transform % 100 );			\
	transform -= ch->gold;				\
	ch->platinum = ( transform / 100 );		\
    }							\
}

/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_INACTIVE(ch)		(get_trust(ch) >= LEVEL_IMMORTAL && IS_SET((ch)->act2, PLR2_INACTIVE))
#define IS_ANCIENT(ch)          (get_trust(ch) >= LEVEL_ANCIENT
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_SHIELDED(ch, sn)	(IS_SET((ch)->shielded_by, (sn)))

#define GET_AGE(ch)		((int) (17 + ((ch)->played \
				    + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)		((ch)->armor[type]			    \
		        + ( IS_AWAKE(ch)			    \
			? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)	\
		((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
		((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight(ch)	((ch)->carry_weight + ((ch)->silver/10) +  \
				 ((ch)->gold * 2 / 5) + ((ch)->platinum / 100))

#define IS_MARRIED(ch)      (!IS_NPC(ch) && IS_SET((ch)->act, PLR2_MARRIED) && \
                                (ch)->pcdata->spouse[0] != '\0' && (ch)->pcdata->spouse != NULL)

#define HAS_TRIGGER(ch,trig)	(IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define IS_QUESTOR(ch)     (!IS_NPC(ch) && IS_SET((ch)->act, PLR_QUESTOR) && \
                            ((ch)->pcdata->questmob != 0 || (ch)->pcdata->questobj != 0))

#define quit(iErr)      _quit( (iErr), __FILE__, __LINE__)

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)

#define CAN_ACCESS(room)        ( !IS_SET(room->room_flags, ROOM_PRIVATE) \
                                &&   !IS_SET(room->room_flags, ROOM_SOLITARY) \
                                &&   !IS_SET(room->room_flags, ROOM_TELEPORT) \
                                &&   !IS_SET(room->room_flags, ROOM_NEWBIES_ONLY) \
                                &&   !IS_SET(room->room_flags, ROOM_GODS_ONLY) \
                                &&   !IS_SET(room->room_flags, ROOM_IMP_ONLY) \
                                &&   !IS_SET(room->room_flags, ROOM_HOME) \
                                &&   !IS_SET(room->room_flags, ROOM_ARENA) )

/*
 * Description macros.
 */
#define PERS(ch, looker) ( can_see( looker,(ch) )?             \
			 ( IS_NPC(ch) ? (ch)->short_descr        \
			 : (ch)->pcdata->cname ) : IS_IMMORTAL(ch) ?      \
			 "{WAn {BIm{bm{Wor{bt{Bal{x" :         \
			 "someone")
/*
 * Structure for a social in the socials table.
 */
struct social_type
{
    char name[20];
    char *char_no_arg;
    char *others_no_arg;
    char *char_found;
    char *others_found;
    char *vict_found;
    char *char_not_found;
    char *char_auto;
    char *others_auto;
};

/*
 * Global constants.
 */
extern const struct str_app_type str_app[26];
extern const struct int_app_type int_app[26];
extern const struct wis_app_type wis_app[26];
extern const struct dex_app_type dex_app[26];
extern const struct con_app_type con_app[26];

extern const struct class_type class_table[MAX_CLASS];
extern const struct weapon_type weapon_table[];
extern const struct item_type item_table[];
extern const struct wiznet_type wiznet_table[];
extern const struct attack_type attack_table[];
extern const struct quote_type quote_table[MAX_QUOTES];
extern const struct race_type race_table[];
extern const struct pc_race_type pc_race_table[];
extern const struct spec_type spec_table[];
extern const struct liq_type liq_table[];
extern const struct skill_type skill_table[MAX_SKILL];
extern const struct group_type group_table[MAX_GROUP];
/*extern          struct social_type      social_table    [MAX_SOCIALS];*/

extern struct social_type *social_table;
extern char *const title_table[MAX_CLASS][MAX_LEVEL + 1][2];
extern const sh_int movement_loss[SECT_MAX];

char *md5crypt(const char *pwd );

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(apollo)
int atoi args ( ( const char *string ) );
void *calloc args ( ( unsigned nelem, size_t size ) );
#endif

#if	defined(sequent)
int fclose args ( ( FILE * stream ) );
int fprintf args ( ( FILE * stream, const char *format, ... ) );
int fread args ( ( void *ptr, int size, int n, FILE * stream ) );
int fseek args ( ( FILE * stream, long offset, int ptrname ) );
void perror args ( ( const char *s ) );
int ungetc args ( ( int c, FILE * stream ) );
#endif

#if	defined(sun)
int fclose args ( ( FILE * stream ) );
int fprintf args ( ( FILE * stream, const char *format, ... ) );
#if	defined(SYSV)
siz_t fread args ( ( void *ptr, size_t size, size_t n, FILE * stream ) );
#else
int fread args ( ( void *ptr, int size, int n, FILE * stream ) );
#endif
int fseek args ( ( FILE * stream, long offset, int ptrname ) );
void perror args ( ( const char *s ) );
int ungetc args ( ( int c, FILE * stream ) );
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif

/* economy.c*/
#define	BANK_INVEST
#define BANK_INVEST_SAVE
#define	BANK_TRANSFER

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY	30

/*
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1   /* Area has been modified. */
#define         AREA_ADDED      2   /* Area has been added to. */
#define         AREA_LOADING    4   /* Used for counting in db.c */

#define MAX_DIR	6
#define NO_FLAG -99             /* Must not be used in flags or stats. */

/*
 * Global Constants
 */
extern char *const dir_name[];
extern const sh_int rev_dir[];  /* sh_int - ROM OLC */

struct bit_type
{
    const struct flag_type *table;
    char *help;
};

extern const struct bit_type bitvector_type[];

 typedef struct  disabled_data			DISABLED_DATA;
 
 /* one disabled command */
 struct disabled_data
 {
 	DISABLED_DATA *next; /* pointer to next node */
 	struct cmd_type const *command; /* pointer to the command struct*/
 	char *disabled_by; /* name of disabler */
 	sh_int level; /* level of disabler */
 };
 
 extern			DISABLED_DATA	  *		disabled_first; /* interp.c */

 DECLARE_DO_FUN( do_disable      ); /* dec95 EA */

 #define DISABLED_FILE	"../data/text/disabled.txt"  /* disabled commands */

void load_disabled args ( ( void ) );
void save_disabled args ( ( void ) );

typedef struct change_data CHANGE_DATA;

struct change_data
{
  CHANGE_DATA *next;
  CHANGE_DATA *prev;
  char	      *imm;
  char	      *text;
  char	      *date;
};

/*illegal.c*/

void do_illegalize args( (CHAR_DATA *ch, char *argument) );
bool check_illegal args( (char *word) );
void load_illegal args( (void) );
void save_illegal args( (void) );

struct	hometown_type
{
	char	*name;
	int	recall;
	int	school;
	int	morgue;
	int	death;
};

extern const struct hometown_type hometown_table [];
int get_hometown args( ( char *argument ) );

char *last_debug[MAX_DEBUG];

#include "files.h"
#include "interp.h"
#include "proto.h"
#include "gsn.h"
#include "globals.h"

