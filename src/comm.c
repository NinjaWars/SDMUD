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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern int malloc_debug args ( ( int ) );
extern int malloc_verify args ( ( void ) );
#endif

/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif

/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const char echo_off_str[] = { '\0' };
const char echo_on_str[] = { '\0' };
const char go_ahead_str[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };
#endif

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen
) );*/
/*int	bind		args( ( int s, struct sockaddr *name, int namelen
) );*/
void bzero args ( ( char *b, int length ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void bzero args ( ( char *b, int length ) );
#endif

#if	defined(__hpux)
/*int	accept		args( ( int s, void *addr, int *addrlen ) );*/
/*int	bind		args( ( int s, const void *addr, int addrlen )
);*/
void bzero args ( ( char *b, int length ) );
int getpeername args ( ( int s, void *addr, int *addrlen ) );
int getsockname args ( ( int s, void *name, int *addrlen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int setsockopt
args ( ( int s, int level, int optname, const void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif
/*
#if	defined(linux)
//int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
//int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif
*/
#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct timeval
{
    time_t tv_sec;
    time_t tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static long theKeys[4];

int gettimeofday args ( ( struct timeval * tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern int errno;
#endif

#if	defined(MSDOS)
int gettimeofday args ( ( struct timeval * tp, void *tzp ) );
int kbhit args ( ( void ) );
#endif

#if	defined(NeXT)
int close args ( ( int fd ) );
int fcntl args ( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short htons args ( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long ntohl args ( ( u_long hostlong ) );
#endif
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen
) );*/
/*int	bind		args( ( int s, struct sockaddr *name, int namelen
) );*/
int close args ( ( int fd ) );
int fcntl args ( ( int fd, int cmd, int arg ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
#if	!defined(htons)
u_short htons args ( ( u_short hostshort ) );
#endif
int listen args ( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long ntohl args ( ( u_long hostlong ) );
#endif
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int setsockopt
args ( ( int s, int level, int optname, caddr_t optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int accept args ( ( int s, struct sockaddr * addr, int *addrlen ) );
int bind args ( ( int s, struct sockaddr * name, int namelen ) );
void bzero args ( ( char *b, int length ) );
int close args ( ( int fd ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
#if defined(SYSV)
int setsockopt
args ( ( int s, int level, int optname, const char *optval, int optlen ) );
#else
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
#endif
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int accept args ( ( int s, struct sockaddr * addr, int *addrlen ) );
int bind args ( ( int s, struct sockaddr * name, int namelen ) );
void bzero args ( ( char *b, int length ) );
int close args ( ( int fd ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * Global variables.
 */
DESCRIPTOR_DATA *d_next;        /* Next descriptor in loop      */
bool god;                       /* All new chars are gods!      */
char clcode[MAX_INPUT_LENGTH];

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos args ( ( void ) );
bool read_from_descriptor args ( ( DESCRIPTOR_DATA * d ) );
bool write_to_descriptor args ( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void game_loop_unix args ( ( int control ) );
int init_socket args ( ( int port ) );
void init_descriptor args ( ( int control ) );
/* void init_descriptor_www args ( ( int wwwcontrol ) ); */
bool read_from_descriptor args ( ( DESCRIPTOR_DATA * d ) );
bool write_to_descriptor args ( ( int desc, char *txt, int length ) );
#endif

bool check_parse_name2 ( char *name );

/* Needs to be global because of do_reboot */
int port, control;
volatile sig_atomic_t crashed = 0;

static void sigalrm ( sig )
int sig;
{
    time_t tm;
    time ( &tm );
    fprintf ( stderr, "TOCK!\n\r" );
    
    if ( ( tm - current_time ) > 32 )
    {
	if ( debug_mode )
	{
	      int i;
	      for ( i=0; i < MAX_DEBUG; i++ )
              {
                 fprintf ( stderr, "Debug message: %s\r", last_debug[i] );
	      }
	}    
        char buf[MSL];
	sprintf ( buf, "The MUD just looped. Last command was:\n\r%s", last_command );
	bug_message ( "The MUD", "Rhaelar", "Loop", 30, buf );
        fprintf ( stderr, "Looping - Last Command: %s\r", last_command );
        // this requires you to add an "if (ch)" before the send_to_char
        // statements in do_reboot.
        do_reboot ( NULL, "wolla" );
        exit ( 1 );
    }
    alarm ( 300 );
}

DNS_LOOKUP        *     dns_free;
DNS_LOOKUP        *     dns_list;
pthread_mutex_t        memory_mutex = PTHREAD_MUTEX_INITIALIZER;

int main ( int argc, char **argv )
{
    struct timeval now_time;
    bool freboot = FALSE;
    struct sigaction halt_action, ignore_action, alarm_action;

    halt_action.sa_handler = halt_mud;
    sigemptyset ( &halt_action.sa_mask );
    halt_action.sa_flags = SA_NOMASK;

    ignore_action.sa_handler = SIG_IGN;
    sigemptyset ( &ignore_action.sa_mask );
    ignore_action.sa_flags = 0;

    alarm_action.sa_handler = sigalrm;
    sigemptyset ( &alarm_action.sa_mask );
    alarm_action.sa_flags = SA_NOMASK;

    sigaction ( SIGPIPE, &ignore_action, NULL );    /* who cares about pipes? */
    sigaction ( SIGHUP, &ignore_action, NULL ); /* stay alive if user quits */
    sigaction ( SIGINT, &halt_action, NULL );   /* interrupted at keyboard */
    sigaction ( SIGQUIT, &halt_action, NULL );  /* quit at keyboard */
    sigaction ( SIGILL, &halt_action, NULL );   /* illegal instruction */ 
    sigaction ( SIGFPE, &halt_action, NULL );   /* floating point error */
    sigaction ( SIGSEGV, &halt_action, NULL );  /* invalid memory reference */
    sigaction ( SIGTERM, &halt_action, NULL );  /* terminate */
    sigaction ( SIGBUS, &halt_action, NULL );   /* out of memory?? */
    sigaction ( SIGALRM, &alarm_action, NULL ); /* endless loop check */ 

    alarm ( 300 );

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug ( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday ( &now_time, NULL );
    current_time = ( time_t ) now_time.tv_sec;
    strcpy ( str_boot_time, ctime ( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow ( stdout );
    csetmode ( C_RAW, stdin );
    cecho2file ( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
/*    if ( ( fpReserve = fopen ( NULL_FILE, "r" ) ) == NULL )
    {
        perror ( NULL_FILE );
        quit ( 1 );
    } */

    /*
       * Get the port number.
     */
    port = 7000;
    if ( argc > 1 )
    {
        if ( !is_number ( argv[1] ) )
        {
            fprintf ( stderr, "Usage: %s [port ##]\n", argv[0] );
            quit ( 1 );
        }
        else if ( ( port = atoi ( argv[1] ) ) <= 1024 )
        {
            fprintf ( stderr, "Port numbers must be above 1024.\n" );
            quit ( 1 );
        }
    }

    if ( argc > 2 )
    {
        if ( !str_cmp ( argv[2], "state" ) && is_number ( argv[3] ) )
        {
            freboot = TRUE;
            control = atoi ( argv[3] );
/*           wwwcontrol = atoi ( argv[4] ); */
        }
        else
            freboot = FALSE;
    }

    /*
     * Run the game.
     */

    log_string ( "Initializing Telnet Port." );
    if ( freboot == FALSE )
        control = init_socket ( port );

    /*log_string ( "Initializing WWW Port." );
    if ( freboot == FALSE )
        wwwcontrol = init_socket ( wwwport );*/
    boot_db (  );
    arena = FIGHT_OPEN;
    /* sprintf ( log_buf, "MUD Booted: Telnet: %d, WWW: %d", port, wwwport ); */
    sprintf ( log_buf, "SD is ready to give you wild dreams on port %d", port);
    log_string ( log_buf );

    if ( freboot == TRUE )
    {
        log_string ( "Initiating reboot Recovery" );
        reboot_recover (  );
        log_string ( "Reboot Recovery Completed." );
    }

    game_loop_unix ( control );
 /*   close ( wwwcontrol );*/
    close ( control );

    /*
     * That's all, folks.
     */
    log_string ( "Normal termination of game." );
    quit ( 0 );
    return 0;
}

#if defined(unix)
int init_socket ( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror ( "Init_socket: socket" );
        quit ( 1 );
    }

    if ( setsockopt
         ( fd, SOL_SOCKET, SO_REUSEADDR, ( char * ) &x, sizeof ( x ) ) < 0 )
    {
        perror ( "Init_socket: SO_REUSEADDR" );
        close ( fd );
        quit ( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
        struct linger ld;

        ld.l_onoff = 1;
        ld.l_linger = 1000;

        if ( setsockopt
             ( fd, SOL_SOCKET, SO_DONTLINGER, ( char * ) &ld,
               sizeof ( ld ) ) < 0 )
        {
            perror ( "Init_socket: SO_DONTLINGER" );
            close ( fd );
            quit ( 1 );
        }
    }
#endif

    sa = sa_zero;
    sa.sin_family = AF_INET;
    sa.sin_port = htons ( port );

    if ( bind ( fd, ( struct sockaddr * ) &sa, sizeof ( sa ) ) < 0 )
    {
        perror ( "Init socket: bind" );
        close ( fd );
        quit ( 1 );
    }

    if ( listen ( fd, 3 ) < 0 )
    {
        perror ( "Init socket: listen" );
        close ( fd );
        quit ( 1 );
    }

    return fd;
}
#endif

#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos ( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday ( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor = 0;
    dcon.connected = CON_GET_NAME;
    dcon.host = str_dup ( "localhost" );
    dcon.outsize = 2000;
    dcon.outbuf = alloc_mem ( dcon.outsize );
    dcon.next = descriptor_list;
    dcon.showstr_head = NULL;
    dcon.showstr_point = NULL;
    dcon.pEdit = NULL;          /* OLC */
    dcon.pString = NULL;        /* OLC */
    dcon.editor = 0;            /* OLC */
    dcon.ansi = TRUE;
    descriptor_list = &dcon;

    /*
     * Send the greeting. DET ER MACINTOSH DET HER ARRRRGGGGH
     */
    {
        extern char *help_greetinga;
        extern char *help_greetingb;
        extern char *help_greetingc;
        extern char *help_greetingd;
        extern char *help_greetinge;
	extern char *help_greetingf;
	extern char *help_greetingg;
	extern char *help_greetingh;
	extern char *help_greetingi;
	extern char *help_greetingj;
	extern char *help_greetingk;
	extern char *help_greetingl;
	extern char *help_greetingm;
	extern char *help_greetingn;
	extern char *help_greetingo;
	extern char *help_greetingp;
	extern char *help_greetingq;
	extern char *help_greetingr;
	extern char *help_greetings;
	extern char *help_greetingt;
	extern char *help_authors;
        extern char *help_login;
        switch ( number_range ( 0, 19 ) )
        {
            default:
                if ( help_greetinga[0] == '.' )
                    write_to_buffer ( &dcon, help_greetinga + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetinga, 0 );
                break;
            case 0:
                if ( help_greetinga[0] == '.' )
                    write_to_buffer ( &dcon, help_greetinga + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetinga, 0 );
                break;
            case 1:
                if ( help_greetingb[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingb + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingb, 0 );
                break;
            case 2:
                if ( help_greetingc[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingc + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingc, 0 );
                break;
            case 3:
                if ( help_greetingd[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingd + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingd, 0 );
                break;
            case 4:
                if ( help_greetinge[0] == '.' )
                    write_to_buffer ( &dcon, help_greetinge + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetinge, 0 );
                break;
            case 5:
                if ( help_greetingf[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingf + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingf, 0 );
                break;
            case 6:
                if ( help_greetingg[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingg + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingg, 0 );
                break;
	    case 7:
                if ( help_greetingh[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingh + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingh, 0 );
                break;
            case 8:
                if ( help_greetingi[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingi + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingi, 0 );
                break;
            case 9:
                if ( help_greetingj[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingj + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingj, 0 );
                break;
            case 10:
                if ( help_greetingk[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingk + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingk, 0 );
                break;
            case 11:
                if ( help_greetingl[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingl + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingl, 0 );
                break;
            case 12:
                if ( help_greetingm[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingm + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingm, 0 );
                break;
            case 13:
                if ( help_greetingn[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingn + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingn, 0 );
                break;
            case 14:
                if ( help_greetingo[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingo + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingo, 0 );
                break;
            case 15:
                if ( help_greetingp[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingp + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingp, 0 );
                break;
            case 16:
                if ( help_greetingq[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingq + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingq, 0 );
                break;
            case 17:
                if ( help_greetingr[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingr + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingr, 0 );
                break;
            case 18:
                if ( help_greetings[0] == '.' )
                    write_to_buffer ( &dcon, help_greetings + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetings, 0 );
                break;
            case 19:
                if ( help_greetingt[0] == '.' )
                    write_to_buffer ( &dcon, help_greetingt + 1, 0 );
                else
                    write_to_buffer ( &dcon, help_greetingt, 0 );
                break;
        }

        if ( help_authors[0] == '.' )
            write_to_buffer ( &dcon, help_authors + 1, 0 );
        else
            write_to_buffer ( &dcon, help_authors, 0 );
        if ( help_login[0] == '.' )
            write_to_buffer ( &dcon, help_login + 1, 0 );
        else
            write_to_buffer ( &dcon, help_login, 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
        DESCRIPTOR_DATA *d;

        /*
         * Process input.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            d->fcommand = FALSE;

#if defined(MSDOS)
            if ( kbhit (  ) )
#endif
            {
                if ( d->character != NULL )
                    d->character->timer = 0;
                if ( !read_from_descriptor ( d ) )
                {
                    if ( d->character != NULL )
                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                    continue;
                }
            }

            if ( d->character != NULL && d->character->daze > 0 )
                --d->character->daze;

            if ( d->character != NULL && d->character->wait > 0 )
            {
                --d->character->wait;
                continue;
            }

            read_from_buffer ( d );
            if ( d->incomm[0] != '\0' )
            {
                d->fcommand = TRUE;
                stop_idling ( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string ( d, d->incomm );
                else if ( d->pString )
                    string_add ( d->character, d->incomm );
                else
                    switch ( d->connected )
                    {
                        case CON_PLAYING:
                            if ( !run_olc_editor ( d ) )
                                substitute_alias ( d, d->incomm );
                            break;
                        default:
                            nanny ( d, d->incomm );
                            break;
                    }

                d->incomm[0] = '\0';
            }
        }

        /*
         * Autonomous game motion.
         */
        update_handler ( );

        /*
         * Output.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( ( d->fcommand || d->outtop > 0 ) )
            {
                if ( !process_output ( d, TRUE ) )
                {
                    if ( d->character != NULL && d->character->level > 5 )
                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                }
            }
        }

        /*
         * Synchronize to a clock.
         * Busy wait (blargh).
         */
        now_time = last_time;
        for ( ;; )
        {
            int delta;

#if defined(MSDOS)
            if ( kbhit (  ) )
#endif
            {
                if ( dcon.character != NULL )
                    dcon.character->timer = 0;
                if ( !read_from_descriptor ( &dcon ) )
                {
                    if ( dcon.character != NULL && d->character->level > 1 )
                        save_char_obj ( d->character );
                    dcon.outtop = 0;
                    close_socket ( &dcon );
                }
#if defined(MSDOS)
                break;
#endif
            }

            gettimeofday ( &now_time, NULL );
            delta =
                ( now_time.tv_sec - last_time.tv_sec ) * 1000 * 1000 +
                ( now_time.tv_usec - last_time.tv_usec );
            if ( delta >= 1000000 / PULSE_PER_SECOND )
                break;
        }
        last_time = now_time;
        current_time = ( time_t ) last_time.tv_sec;
    }

    return;
}
#endif

#if defined(unix)
void game_loop_unix ( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal ( SIGPIPE, SIG_IGN );
    gettimeofday ( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
        fd_set in_set;
        fd_set out_set;
        fd_set exc_set;
        DESCRIPTOR_DATA *d;
        int maxdesc;

#if defined(MALLOC_DEBUG)
        if ( malloc_verify (  ) != 1 )
            abort (  );
#endif

        /*
         * Poll all active descriptors.
         */
        FD_ZERO ( &in_set );
        FD_ZERO ( &out_set );
        FD_ZERO ( &exc_set );
        FD_SET ( control, &in_set );
        maxdesc = control;
        for ( d = descriptor_list; d; d = d->next )
        {
            maxdesc = UMAX ( maxdesc, d->descriptor );
            FD_SET ( d->descriptor, &in_set );
            FD_SET ( d->descriptor, &out_set );
            FD_SET ( d->descriptor, &exc_set );
        }

        if ( select ( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) <
             0 )
        {
            perror ( "Game_loop: select: poll" );
            quit ( 1 );
        }

        /*
         * New connection?
         */
        if ( FD_ISSET ( control, &in_set ) )
            init_descriptor ( control );

        FD_ZERO ( &in_set );
        FD_ZERO ( &out_set );
        FD_ZERO ( &exc_set );
/*        FD_SET ( wwwcontrol, &in_set );
        maxdesc = wwwcontrol; */
        for ( d = descriptor_list; d; d = d->next )
        {
            maxdesc = UMAX ( maxdesc, d->descriptor );
            FD_SET ( d->descriptor, &in_set );
            FD_SET ( d->descriptor, &out_set );
            FD_SET ( d->descriptor, &exc_set );
        }

        if ( select ( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) <
             0 )
        {
            perror ( "Game_loop: select: poll" );
            quit ( 1 );
        }

        /* if ( FD_ISSET ( wwwcontrol, &in_set ) )
            init_descriptor_www ( wwwcontrol ); */

        /*
         * Kick out the freaky folks.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            if ( FD_ISSET ( d->descriptor, &exc_set ) )
            {
                FD_CLR ( d->descriptor, &in_set );
                FD_CLR ( d->descriptor, &out_set );
                if ( d->character && d->character->level > 5 )
                    save_char_obj ( d->character );
                d->outtop = 0;
                close_socket ( d );
            }
        }

        /*
         * Process input.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            d->fcommand = FALSE;

            if ( FD_ISSET ( d->descriptor, &in_set ) )
            {
                if ( d->character != NULL )
                    d->character->timer = 0;
                if ( !read_from_descriptor ( d ) )
                {
                    FD_CLR ( d->descriptor, &out_set );
                    if ( d->character != NULL && d->character->level > 5 )
                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                    continue;
                }
            }

            if ( d->character != NULL && d->character->daze > 0 )
                --d->character->daze;

            if ( d->character != NULL && d->character->wait > 0 )
            {
                --d->character->wait;
                continue;
            }

            read_from_buffer ( d );
            if ( d->incomm[0] != '\0' )
            {
                d->fcommand = TRUE;
                stop_idling ( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string ( d, d->incomm );
                else if ( d->pString )
                    string_add ( d->character, d->incomm );
                else
                    switch ( d->connected )
                    {
                        case CON_PLAYING:
                            if ( !run_olc_editor ( d ) )
                                substitute_alias ( d, d->incomm );
                            break;
                        default:

                            nanny ( d, d->incomm );
                            break;
                    }

                d->incomm[0] = '\0';
            }
        }

        /*
         * Autonomous game motion.
         */
        update_handler (  );

        /*
         * Output.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( ( d->fcommand || d->outtop > 0 ) &&
                 FD_ISSET ( d->descriptor, &out_set ) )
            {
                if ( !process_output ( d, TRUE ) )
                {
                    if ( d->character != NULL && d->character->level > 1 )
                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                }
            }
        }

        /*
         * Synchronize to a clock.
         * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
         * Careful here of signed versus unsigned arithmetic.
         */
        {
            struct timeval now_time;
            long secDelta;
            long usecDelta;

            gettimeofday ( &now_time, NULL );
            usecDelta =
                ( ( int ) last_time.tv_usec ) - ( ( int ) now_time.tv_usec ) +
                1000000 / PULSE_PER_SECOND;
            secDelta =
                ( ( int ) last_time.tv_sec ) - ( ( int ) now_time.tv_sec );
            while ( usecDelta < 0 )
            {
                usecDelta += 1000000;
                secDelta -= 1;
            }

            while ( usecDelta >= 1000000 )
            {
                usecDelta -= 1000000;
                secDelta += 1;
            }

            if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
            {
                struct timeval stall_time;

                stall_time.tv_usec = usecDelta;
                stall_time.tv_sec = secDelta;
                if ( select ( 0, NULL, NULL, NULL, &stall_time ) < 0 )
                {
                    if ( errno != EINTR )
                    {
                        perror ( "Game_loop: select: stall" );
                        quit ( 1 );
                    }
                }
            }
        }

        gettimeofday ( &last_time, NULL );
        current_time = ( time_t ) last_time.tv_sec;
    }

    return;
}
#endif

/*void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

   *//* Main loop *//*
   while ( !merc_down )
   {
   fd_set in_set;
   fd_set out_set;
   fd_set exc_set;
   DESCRIPTOR_DATA *d;
   int maxdesc;

   #if defined(MALLOC_DEBUG)
   if ( malloc_verify( ) != 1 )
   abort( );
   #endif

 *//*
   * Poll all active descriptors.
 *//*
   FD_ZERO( &in_set  );
   FD_ZERO( &out_set );
   FD_ZERO( &exc_set );
   FD_SET( control, &in_set );
   maxdesc   = control;
   for ( d = descriptor_list; d; d = d->next )
   {
   maxdesc = UMAX( maxdesc, d->descriptor );
   FD_SET( d->descriptor, &in_set  );
   FD_SET( d->descriptor, &out_set );
   FD_SET( d->descriptor, &exc_set );
   }

   if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
   {
   perror( "Game_loop: select: poll" );
   exit( 1 );
   }
 */
    /*
     * New connection?
 *//*
   if ( FD_ISSET( control, &in_set ) )
   init_descriptor( control );

   FD_ZERO( &in_set  );
   FD_ZERO( &out_set );
   FD_ZERO( &exc_set );
   FD_SET( wwwcontrol, &in_set ); 
   maxdesc   = wwwcontrol; 
   for ( d = descriptor_list; d; d = d->next )
   {
   maxdesc = UMAX( maxdesc, d->descriptor );
   FD_SET( d->descriptor, &in_set  );
   FD_SET( d->descriptor, &out_set );
   FD_SET( d->descriptor, &exc_set );
   }

   if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
   {
   perror( "Game_loop: select: poll" );
   exit( 1 );
   }

   if ( FD_ISSET( wwwcontrol, &in_set ) ) 
   init_descriptor_www( wwwcontrol );
*/
    /*
     * Kick out the freaky folks.
     */
  /*
   for ( d = descriptor_list; d != NULL; d = d_next )
   {
   d_next = d->next;   
   if ( FD_ISSET( d->descriptor, &exc_set ) )
   {
   FD_CLR( d->descriptor, &in_set  );
   FD_CLR( d->descriptor, &out_set );
   if ( d->character && d->character->level > 1)
   save_char_obj( d->character );
   d->outtop = 0;
   close_socket( d );
   }
   }
 */
    /*
     * Process input.
    */ 

/*
   for ( d = descriptor_list; d != NULL; d = d_next )
   {
   d_next    = d->next;
   d->fcommand       = FALSE;

   if ( FD_ISSET( d->descriptor, &in_set ) )
   {
   if ( d->character != NULL )
   d->character->timer = 0;
   if ( !read_from_descriptor( d ) )
   {
   FD_CLR( d->descriptor, &out_set );
   if ( d->character != NULL && d->character->level > 1)
   save_char_obj( d->character );
   d->outtop = 0;
   close_socket( d );
   continue;
   }
   }

   if (d->character != NULL && d->character->daze > 0)
   --d->character->daze;

   if ( d->character != NULL && d->character->wait > 0 )
   {
   --d->character->wait;
   continue;
   }

   read_from_buffer( d );
   if ( d->incomm[0] != '\0' )
   {
   d->fcommand       = TRUE;
   stop_idling( d->character );

 *//* OLC *//*
   if ( d->showstr_point )
   show_string( d, d->incomm );
   else
   if ( d->pString )
   string_add( d->character, d->incomm );
   else
   switch ( d->connected )
   {
   case CON_PLAYING:
   if ( !run_olc_editor( d ) )
   substitute_alias( d, d->incomm );
   break;
   default:

   nanny( d, d->incomm );
   break;
   }

   d->incomm[0]        = '\0';
   }
   }

 */
    /*
     * Autonomous game motion.
 *//*
   update_handler( );

 */
    /*
     * Output.
 *//*
   for ( d = descriptor_list; d != NULL; d = d_next )
   {
   d_next = d->next;

   if ( ( d->fcommand || d->outtop > 0 )
   &&   FD_ISSET(d->descriptor, &out_set) )
   {
   if ( !process_output( d, TRUE ) )
   {
   if ( d->character != NULL && d->character->level > 1)
   save_char_obj( d->character );
   d->outtop = 0;
   close_socket( d );
   }
   }
   }

 *//*
   * Synchronize to a clock.
   * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
   * Careful here of signed versus unsigned arithmetic.
 *//*
   {
   struct timeval now_time;
   long secDelta;
   long usecDelta;

   gettimeofday( &now_time, NULL );
   usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
   + 1000000 / PULSE_PER_SECOND;
   secDelta  = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
   while ( usecDelta < 0 )
   {
   usecDelta += 1000000;
   secDelta  -= 1;
   }

   while ( usecDelta >= 1000000 )
   {
   usecDelta -= 1000000;
   secDelta  += 1;
   }

   if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
   {
   struct timeval stall_time;

   stall_time.tv_usec = usecDelta;
   stall_time.tv_sec  = secDelta;
   if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
   {
   perror( "Game_loop: select: stall" );
   exit( 1 );
   }
   }
   }

   gettimeofday( &last_time, NULL );
   current_time = (time_t) last_time.tv_sec;
   }

   return;
   }
   #endif */

#if defined(unix)
void init_descriptor ( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
//    struct hostent *from;
    int desc;
    unsigned int size;
    pthread_attr_t attr;
    pthread_t thread_lookup;
    DNS_LOOKUP *dnslookup;

  /* initialize threads */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


  /* New Dummy Argument */
  if (dns_free == NULL)
  {
    dnslookup = alloc_perm(sizeof(*dnslookup));
  }
  else
  {
    dnslookup = dns_free;
    dns_free = dns_free->next;
  }
  dnslookup->status = 1;   
  dnslookup->next = dns_list;
  dns_list = dnslookup;    
      
    size = sizeof ( sock );
    getsockname ( control, ( struct sockaddr * ) &sock, &size );
    if ( ( desc =
           accept ( control, ( struct sockaddr * ) &sock, &size ) ) < 0 )
    {
        perror ( "New_descriptor: accept" );
        return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl ( desc, F_SETFL, FNDELAY ) == -1 )
    {
        perror ( "New_descriptor: fcntl: FNDELAY" );
        return;
    }

    /*
     * Cons a new descriptor.
     */
/*   
    dnew = new_descriptor();
    dnew->descriptor	= desc;
    dnew->connected	= CON_GET_NAME;
    dnew->showstr_head	= NULL;
    dnew->showstr_point = NULL;
    dnew->outsize	= 2000;
    dnew->outbuf	= alloc_mem( dnew->outsize );
*/
    dnew = new_descriptor (  ); /* new_descriptor now also allocates things */
    dnew->descriptor = desc;
    dnew->pEdit = NULL;         /* OLC */
    dnew->pString = NULL;       /* OLC */
    dnew->editor = 0;           /* OLC */
    dnew->ansi = TRUE;
    size = sizeof ( sock );
    if ( getpeername ( desc, ( struct sockaddr * ) &sock, &size ) < 0 )
    {
        perror ( "New_descriptor: getpeername" );
        dnew->host = str_dup ( "(unknown)" );
    }
    else
    {
        /*
         * Would be nice to use inet_ntoa here but it takes a struct arg,
         * which ain't very compatible between gcc and system libraries.
         */
        int addr;

        addr = ntohl ( sock.sin_addr.s_addr );
        sprintf ( buf, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF,
                  ( addr >> 16 ) & 0xFF, ( addr >> 8 ) & 0xFF,
                  ( addr ) & 0xFF );
/* Removed internal resolver - Rhaelar 11'2004 */
	dnew->host = str_dup ( buf );
	  
        sprintf ( log_buf, "Sock.sinaddr:  %s", buf );
	if ( str_cmp ( buf, "24.106.156.212" ) ) // Darren - This should to be changed to your server's IP address.
	{
          wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, 0 );
	  connections_today++;
	}
        log_string ( log_buf );
/*        from =
            gethostbyaddr ( ( char * ) &sock.sin_addr,
                            sizeof ( sock.sin_addr ), AF_INET );
        dnew->host = str_dup ( from ? from->h_name : buf ); */
	 /* Set up the dummyarg for use in lookup_address */
        dnslookup->buf      = str_dup((char *) &sock.sin_addr);
        dnslookup->d        = dnew;

        /* create a thread to do the lookup */
        pthread_create( &thread_lookup, &attr, (void*)&lookup_address, (void*) dnslookup);
    }


    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban ( dnew->host, BAN_ALL ) )
    {
        write_to_descriptor( desc, "You are not allowed to play on Shattered Dreams.\n\rIf you think this is a mistake, file a complaint to rhaelar@sdmud.com\n\rGoodbye.\n\r", 0 );
        close ( desc );
        free_descriptor ( dnew );
        return;
    }
        
    /*
     * Init descriptor data.
     */
    dnew->next = descriptor_list;
    descriptor_list = dnew;

    /* New colour selection greeting screen - Rhaelar 12'2004 */
    write_to_buffer ( dnew, "\n\r\n\r\n\r\n\r                    Shattered Dreams MUD\n\r", 0 );


    /* I put in quotes here. Fancy baby! ;) -Rhaelar */
/*    switch ( number_range ( 1, 2 ) )
    {    
      default:
        write_to_buffer ( dnew, "        The realm of dreams awaits you, adventurer.\n\r", 0 );
    	break;
      case 1:
        write_to_buffer ( dnew, "        Cowards die many times before their deaths.\n\r", 0 );
        write_to_buffer ( dnew, "        The valiant never taste of death, but once.\n\r", 0 );
	break;
      case 2:
        write_to_buffer ( dnew, "        Cowards die many times before their deaths.\n\r", 0 );
        write_to_buffer ( dnew, "        The valiant never taste of death, but once.\n\r", 0 );
    	break; */
	/* Gotta get some more added, but I'm too lazy. Later. -Rhaelar 12'2004 */
//    }
    write_to_buffer ( dnew, "\n\r           [0m[0;33m-[0m[0;32m-[0m[0;31m-[0m Enable ANSI colouring? (Y/N) [0m[0;31m-[0m[0;32m-[0m[0;33m-[0m ", 0 );
    return;
}
#endif

void close_socket ( DESCRIPTOR_DATA * dclose )
{
    CHAR_DATA *ch;

    if (dclose->lookup_status > STATUS_DONE) return;
        dclose->lookup_status += 2;
	
    if ( dclose->outtop > 0 )
        process_output ( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
        write_to_buffer ( dclose->snoop_by,
                          "Your victim has left the game.\n\r", 0 );
    }

    {
        DESCRIPTOR_DATA *d;

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->snoop_by == dclose )
                d->snoop_by = NULL;
        }
    }

    if ( ( ch = dclose->character ) != NULL )
    {
        sprintf ( log_buf, "Closing link to %s.", ch->name );
        log_string ( log_buf );

	if ( ch->pet && ch->pet->in_room == NULL )
	{
		char_to_room( ch->pet, get_room_index(ROOM_VNUM_LIMBO) );
		extract_char( ch->pet, TRUE );
	}

//        if ( dclose->connected == CON_PLAYING )
	/* If ch is writing note or playing, just lose link otherwise clear char */
	if ((dclose->connected == CON_PLAYING)
	    ||((dclose->connected >= CON_NOTE_TO)
	    && (dclose->connected <= CON_NOTE_FINISH)))
        {
            act ( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
            wiznet ( "Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0 );
/*            {
                char buf[MAX_STRING_LENGTH];
                act ( buf, ch, NULL, NULL, TO_ALL );
            } */
            ch->desc = NULL;
        }
        else
        {
            free_char ( dclose->original ? dclose->original : dclose->character );
        }
    }

    free_runbuf(dclose);
     if ( d_next == dclose )
        d_next = d_next->next;
/*
    if ( dclose == descriptor_list )
    {
        descriptor_list = descriptor_list->next;
    }
    else
    {
        DESCRIPTOR_DATA *d;

        for ( d = descriptor_list; d && d->next != dclose; d = d->next )
            ;
        if ( d != NULL )
            d->next = dclose->next;
        else
            bug ( "Close_socket: dclose not found.", 0 );
    } */

  dclose->connected = CON_NOT_PLAYING;  
/*  close ( dclose->descriptor );
  free_descriptor ( dclose );
#if defined(MSDOS) || defined(macintosh)
    quit ( 1 );
#endif */
    return;
}

bool read_from_descriptor ( DESCRIPTOR_DATA * d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
        return TRUE;

    if (d->connected == CON_NOT_PLAYING)
        return TRUE;

    /* Check for overflow. */
    iStart = strlen ( d->inbuf );
    if ( iStart >= sizeof ( d->inbuf ) - 10 )
    {
        if ( d->lookup_status == STATUS_DONE )
	{
	   sprintf ( log_buf, "%s input overflow!", d->host );
           log_string ( log_buf );
           write_to_descriptor ( d->descriptor,
                              "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	}
        return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ;; )
    {
        int c;
        c = getc ( stdin );
        if ( c == '\0' || c == EOF )
            break;
        putc ( c, stdout );
        if ( c == '\r' )
            putc ( '\n', stdout );
        d->inbuf[iStart++] = c;
        if ( iStart > sizeof ( d->inbuf ) - 10 )
            break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ;; )
    {
        int nRead;

        nRead =
            read ( d->descriptor, d->inbuf + iStart,
                   sizeof ( d->inbuf ) - 10 - iStart );
        if ( nRead > 0 )
        {
            iStart += nRead;
            if ( d->inbuf[iStart - 1] == '\n' ||
                 d->inbuf[iStart - 1] == '\r' )
                break;
        }
        else if ( nRead == 0 )
        {
            log_string ( "EOF encountered on read." );
            return FALSE;
        }
        else if ( errno == EWOULDBLOCK )
            break;
        else
        {
            perror ( "Read_from_descriptor" );
            return FALSE;
        }
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}

/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer ( DESCRIPTOR_DATA * d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
        return;

   if ( d->run_buf )
   {
     while (isdigit(*d->run_head) && *d->run_head != '\0')
     {
       char *s,*e;

       s = d->run_head;
       while( isdigit( *s ) )
         s++;
       e = s;
       while( *(--s) == '0' && s != d->run_head );
       if ( isdigit( *s ) && *s != '0' && *e != 'o')
       {
         d->incomm[0] = *e;
         d->incomm[1] = '\0';
         s[0]--;
         while (isdigit(*(++s)))
           *s = '9';
         return;
       }
       if (*e == 'o')
         d->run_head = e;
       else
         d->run_head = ++e;
     }
     if (*d->run_head != '\0')
     {
       if (*d->run_head != 'o')
       {
         d->incomm[0] = *d->run_head++;
         d->incomm[1] = '\0';
         return;
       }
       else
       {
         char buf[MAX_INPUT_LENGTH];

         d->run_head++;

         sprintf( buf, "open " );
         switch( *d->run_head )
         {
           case 'n' : sprintf( buf+strlen(buf), "north" ); break;
           case 's' : sprintf( buf+strlen(buf), "south" ); break;
           case 'e' : sprintf( buf+strlen(buf), "east" ); break;
           case 'w' : sprintf( buf+strlen(buf), "west" ); break;
           case 'u' : sprintf( buf+strlen(buf), "up" ); break;
           case 'd' : sprintf( buf+strlen(buf), "down" ); break;
           default: return;
         }

         strcpy( d->incomm, buf );
         d->run_head++;
         return;
       }
     }
     free_runbuf(d);
   }

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
        if ( d->inbuf[i] == '\0' )
            return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
        if ( k >= MAX_INPUT_LENGTH - 2 )
        {
            write_to_descriptor ( d->descriptor, "Line too long.\n\r", 0 );

            /* skip the rest of the line */
            for ( ; d->inbuf[i] != '\0'; i++ )
            {
                if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
                    break;
            }
            d->inbuf[i] = '\n';
            d->inbuf[i + 1] = '\0';
            break;
        }

        if ( d->inbuf[i] == '\b' && k > 0 )
            --k;
        else if ( isascii ( d->inbuf[i] ) && isprint ( d->inbuf[i] ) )
            d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
        d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
        if ( d->incomm[0] != '!' && str_cmp ( d->incomm, d->inlast ) )
        {
            d->repeat = 0;
        }
        else
        {
            d->repeat++;
            if ( d->repeat == 25 )
            {
	        if ( d->lookup_status == STATUS_DONE )
		{
                   sprintf ( log_buf, "%s input spamming!", d->host );
                   log_string ( log_buf );
                   wiznet ( "Spam spam spam $N spam spam spam spam spam!",
                         d->character, NULL, WIZ_SPAM, 0,
                         get_trust ( d->character ) );
                   if ( d->incomm[0] == '!' )
                    wiznet ( d->inlast, d->character, NULL, WIZ_SPAM, 0,
                             get_trust ( d->character ) );
                   else
                    wiznet ( d->incomm, d->character, NULL, WIZ_SPAM, 0,
                             get_trust ( d->character ) );
		}
            }
            else if ( d->repeat == 35 )
            {
	        if ( d->lookup_status == STATUS_DONE )
		{
                   sprintf ( log_buf, "%s still input spamming!", d->host );
                   log_string ( log_buf );
                   wiznet ( "Spam spam spam $N spam spam spam spam spam!",
                         d->character, NULL, WIZ_SPAM, 0,
                         get_trust ( d->character ) );
                   if ( d->incomm[0] == '!' )
                    wiznet ( d->inlast, d->character, NULL, WIZ_SPAM, 0,
                             get_trust ( d->character ) );
                   else
                    wiznet ( d->incomm, d->character, NULL, WIZ_SPAM, 0,
                             get_trust ( d->character ) );
                   write_to_descriptor ( d->descriptor,
                                      "\n\r*** PUT A LID ON IT!!! ***\n\r",
                                      0 );
		}
            }
            else if ( d->repeat >= 300 )
            {
	        if ( d->lookup_status == STATUS_DONE )
		{
                   d->repeat = 0;
                   write_to_descriptor ( d->descriptor,
                                      "\n\r*** YOU SHOULD REALLY STOP NOW!!! ***\n\r", 0 );
		}
               /* strcpy ( d->incomm, "quit" ); */
            }
        }
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
        strcpy ( d->incomm, d->inlast );
    else
        strcpy ( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
        i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i + j] ) != '\0'; j++ )
        ;
    return;
}

/** Function: process_ansi_output
* Descr   : Sends buffered output to descriptor, converting any ANSI codes
*         : along the way if character has selected ANSI.
* Returns : TRUE/FALSE based on success of output to descriptor
* Syntax  : (N/A)
* Written : v1.0 5/98
* Author  : Lope, updated by Gary McNickle <gary@tarmongaidon.org>
*/
bool process_ansi_output ( DESCRIPTOR_DATA * d )
{
    CHAR_DATA *ch;
    char *counter;
    char output[MSL];
    char clcode[MSL];
    char *work;
    bool success = TRUE;
    char *i;

    /*
     * No sense in dealing with a null descriptor 
     */
    if ( d == NULL )
        return FALSE;

    /*
     * Initialize output buffer, counter and working string 
     */
    memset ( output, 0, MSL );
    counter = output;
    work = d->outbuf;

    while ( *work != '\0' && ( work - d->outbuf ) < d->outtop )
    {
        if ( ( int ) ( counter - output ) >= MSL - 32 )
        {                       /*
                                 * have a full buffer 
                                 */
            *counter++ = '\0';

            if ( !
                 ( success =
                   write_to_descriptor ( d->descriptor, output,
                                         strlen ( output ) ) ) )
                break;          /* problems...  */

            memset ( output, 0, MSL );
            counter = output;   /* increment counter  */
        }

        if ( *work != '{' )
        {
            *counter++ = *work++;
            continue;
        }

        /*
         * Ok, we have an ansi request... process it 
         */
        /*
         * Pass the '{' code, next should be actual color code itself 
         */
        work++;

        clcode[0] = '\0';       /* Initialize buffer  */

        ch = d->character;

        if ( d->ansi == TRUE )
            switch ( *work )
            {
                default:
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'x':
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case '0':
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'z':
                    strcpy ( clcode, BLINK );
                    break;
                case 'b':
                    strcpy ( clcode, C_BLUE );
                    break;
                case '4':
                    strcpy ( clcode, C_BLUE );
                    break;
                case 'c':
                    strcpy ( clcode, C_CYAN );
                    break;
                case '6':
                    strcpy ( clcode, C_CYAN );
                    break;
                case 'g':
                    strcpy ( clcode, C_GREEN );
                    break;
                case '2':
                    strcpy ( clcode, C_GREEN );
                    break;
                case 'm':
                    strcpy ( clcode, C_MAGENTA );
                    break;
                case '5':
                    strcpy ( clcode, C_MAGENTA );
                    break;
                case 'r':
                    strcpy ( clcode, C_RED );
                    break;
                case '1':
                    strcpy ( clcode, C_RED );
                    break;
                case 'w':
                    strcpy ( clcode, C_WHITE );
                    break;
                case '7':
                    strcpy ( clcode, C_WHITE );
                    break;
                case 'y':
                    strcpy ( clcode, C_YELLOW );
                    break;
                case '3':
                    strcpy ( clcode, C_YELLOW );
                    break;
                case 'B':
                    strcpy ( clcode, C_B_BLUE );
                    break;
                case '$':
                    strcpy ( clcode, FLASHING );
                    break;
                case 'C':
                    strcpy ( clcode, C_B_CYAN );
                    break;
                case '^':
                    strcpy ( clcode, C_B_CYAN );
                    break;
                case 'G':
                    strcpy ( clcode, C_B_GREEN );
                    break;
                case '@':
                    strcpy ( clcode, C_B_GREEN );
                    break;
                case 'M':
                    strcpy ( clcode, C_B_MAGENTA );
                    break;
                case '%':
                    strcpy ( clcode, C_B_MAGENTA );
                    break;
                case 'R':
                    strcpy ( clcode, C_B_RED );
                    break;
                case '!':
                    strcpy ( clcode, C_B_RED );
                    break;
                case 'W':
                    strcpy ( clcode, C_B_WHITE );
                    break;
                case '&':
                    strcpy ( clcode, C_B_WHITE );
                    break;
                case 'Y':
                    strcpy ( clcode, C_B_YELLOW );
                    break;
                case '#':
                    strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'D':
                    strcpy ( clcode, C_D_GREY );
                    break;
                case '8':
                    strcpy ( clcode, C_D_GREY );
                    break;
                case '*':
                    switch ( number_range ( 1, 14 ) )
                    {
                        default:
                            strcpy ( clcode, C_D_GREY );
                            break;
                        case 1:
                            strcpy ( clcode, C_RED );
                            break;
                        case 2:
                            strcpy ( clcode, C_B_RED );
                            break;
                        case 3:
                            strcpy ( clcode, C_GREEN );
                            break;
                        case 4:
                            strcpy ( clcode, C_B_GREEN );
                            break;
                        case 5:
                            strcpy ( clcode, C_YELLOW );
                            break;
                        case 6:
                            strcpy ( clcode, C_B_YELLOW );
                            break;
                        case 7:
                            strcpy ( clcode, C_BLUE );
                            break;
                        case 8:
                            strcpy ( clcode, C_B_BLUE );
                            break;
                        case 9:
                            strcpy ( clcode, C_MAGENTA );
                            break;
                        case 10:
                            strcpy ( clcode, C_B_MAGENTA );
                            break;
                        case 11:
                            strcpy ( clcode, C_CYAN );
                            break;
                        case 12:
                            strcpy ( clcode, C_B_CYAN );
                            break;
                        case 13:
                            strcpy ( clcode, C_WHITE );
                            break;
                        case 14:
                            strcpy ( clcode, C_B_WHITE );
                            break;
                        case 15:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case 'A':      /* Auction Channel */
                    if ( ch && ch->color_auc )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_auc, ch ) );
                    else
                        strcpy ( clcode, C_B_GREEN );
                    break;
                case 'E':      /* Clan Gossip Channel */
                    if ( ch && ch->color_cgo )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_cgo, ch ) );
                    else
                        strcpy ( clcode, C_B_RED );
                    break;
                case 'F':      /* Clan Talk Channel */
                    if ( ch && ch->color_cla )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_cla, ch ) );
                    else
                        strcpy ( clcode, C_B_MAGENTA );
                    break;
                case 'H':      /* Gossip Channel */
                    if ( ch && ch->color_gos )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_gos, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'J':      /* Grats Channel */
                    if ( ch && ch->color_gra )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_gra, ch ) );
                    else
                        strcpy ( clcode, C_YELLOW );
                    break;
                case 'K':      /* Group Tell Channel */
                    if ( ch && ch->color_gte )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_gte, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'L':      /* Immortal Talk Channel */
                    if ( ch && ch->color_imm )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_imm, ch ) );
                    else
                        strcpy ( clcode, C_B_WHITE );
                    break;
                case 'N':      /* Music Channel */
                    if ( ch && ch->color_mus )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_mus, ch ) );
                    else
                        strcpy ( clcode, C_B_CYAN );
                    break;
                case 'P':      /* Question+Answer Channel */
                    if ( ch && ch->color_que )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_que, ch ) );
                    else
                        strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'Q':      /* Quote Channel */
                    if ( ch && ch->color_quo )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_quo, ch ) );
                    else
                        strcpy ( clcode, C_GREEN );
                    break;
                case 'S':      /* Say Channel */
                    if ( ch && ch->color_say )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_say, ch ) );
                    else
                        strcpy ( clcode, C_MAGENTA );
                    break;
                case 'T':      /* Shout+Yell Channel */
                    if ( ch && ch->color_sho )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_sho, ch ) );
                    else
                        strcpy ( clcode, C_RED );
                    break;
                case 'U':      /* Tell+Reply Channel */
                    if ( ch && ch->color_tel )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_tel, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'V':      /* Wiznet Messages */
                    if ( ch && ch->color_wiz )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_wiz, ch ) );
                    else
                        strcpy ( clcode, C_WHITE );
                    break;
                case 'a':      /* Mobile Talk */
                    if ( ch && ch->color_mob )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_mob, ch ) );
                    else
                        strcpy ( clcode, C_MAGENTA );
                    break;
                case 'e':      /* Room Title */
                    if ( ch && ch->color_roo )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_roo, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'f':      /* Opponent Condition */
                    if ( ch && ch->color_con )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_con, ch ) );
                    else
                        strcpy ( clcode, C_B_RED );
                    break;
                case 'h':      /* Fight Actions */
                    if ( ch && ch->color_fig )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_fig, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'i':      /* Opponents Fight Actions */
                    if ( ch && ch->color_opp )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_opp, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'j':      /* Disarm Messages */
                    if ( ch && ch->color_dis )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_dis, ch ) );
                    else
                        strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'k':      /* Witness Messages */
                    if ( ch && ch->color_wit )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_wit, ch ) );
                    else
                        strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'l':      /* Quest Gossip */
                    if ( ch && ch->color_qgo )
                        strcpy ( clcode,
                                 colour_channel ( ch->color_qgo, ch ) );
                    else
                        strcpy ( clcode, C_B_CYAN );
                    break;

                case '{':
                    sprintf ( clcode, "%c", '{' );
                    break;
            }
        /*
         * end: switch 
         */
        work++;                 /* pass the ansi request code  */

        /*
         * increment counters 
         */
        i = clcode;
        while ( ( *counter = *i ) != '\0' )
            ++counter, ++i;

    }

    success = success &&
        ( write_to_descriptor ( d->descriptor, output, strlen ( output ) ) );

    d->outtop = 0;
    return success;

}

/*
 * Low level output function.
 */
bool process_output ( DESCRIPTOR_DATA * d, bool fPrompt )
{
    /*
     * Bust a prompt.
     */
    if ( !merc_down )
    {
        if ( d->showstr_point )
            write_to_buffer ( d, "[Hit Return to continue, or type Q to abort]\n\r", 0 );
        else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
            write_to_buffer ( d, "> ", 2 );
        else if ( fPrompt && d->connected == CON_PLAYING )
        {
            CHAR_DATA *ch;

            CHAR_DATA *victim;

            ch = d->character;

            /* battle prompt */
/*            if ( ( victim = ch->fighting ) != NULL && can_see ( ch, victim ) )
            {
                float percent = 0.00;
                char meter[100];
                char buf[MAX_STRING_LENGTH];

                if ( victim->max_hit > 0 )
                    percent = victim->hit * 100 / victim->max_hit;
                else
                    percent = -1;

                if ( percent >= 100 )
                    sprintf ( meter, "[{C**********{x]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "[{M********* {x]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "[{Y********  {x]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "[{G*******   {x]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "[{B******    {x]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "[{m*****     {x]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "[{y****      {x]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "[{g***       {x]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "[{b**        {x]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "[{R*         {x]" );
                else if ( percent >= 0 )
                    sprintf ( meter, "[{x          {x]" );
                sprintf ( buf, "%s{x: %s {D[{W%c%.2f{D]{x\n\r",
                          IS_NPC ( victim ) ? victim->short_descr : victim->
                          name, meter, '%', percent );
                buf[0] = UPPER ( buf[0] );
                send_to_char ( buf, ch );
                if ( victim->stunned )
                {
                    sprintf ( buf, "{f%s is stunned.{x\n\r",
                              IS_NPC ( victim ) ? victim->
                              short_descr : victim->name );
                    send_to_char ( buf, ch );
                }
                if ( victim->max_hit > 0 )
                    percent = ch->hit * 100 / ch->max_hit;
                else
                    percent = -1;
                if ( percent >= 100 )
                    sprintf ( meter, "[{C**********{x]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "[{M********* {x]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "[{Y********  {x]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "[{G*******   {x]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "[{B******    {x]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "[{m*****     {x]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "[{y****      {x]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "[{g***       {x]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "[{b**        {x]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "[{R*         {x]" );
                else if ( percent >= 0 )
                    sprintf ( meter, "[{x          {x]" );
                sprintf ( buf, "You: %s {D[{d%c%.2f{D]\n\r", meter, '%', percent );
                buf[0] = UPPER ( buf[0] );
                send_to_char ( buf, ch );
            } */

           if ( ( victim = ch->fighting ) != NULL && can_see ( ch, victim ) )
            {
                float percent = 0.00;
                char meter[100];
                char buf[MAX_STRING_LENGTH];
                float victhit = 0.00;
                float victhitmax = 0.00;
                float chhit = 0.00;
                float chhitmax = 0.00;

                victhit = victim->hit;
                victhitmax = victim->max_hit;
                if ( victhit > 0 && victhitmax > 0 )
                    percent = ( ( victhit * 100 ) / victhitmax );
                else
                    percent = 0;

                if ( percent == 100 )
                    sprintf ( meter, "[{C**********{x]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "[{M********* {x]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "[{Y********  {x]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "[{G*******   {x]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "[{B******    {x]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "[{m*****     {x]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "[{y****      {x]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "[{g***       {x]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "[{b**        {x]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "[{R*         {x]" );
                else if ( percent >= 1 )
                    sprintf ( meter, "[{D*         {x]" );
                else if ( percent < 1 )
                    sprintf ( meter, "[            {x]" );
                sprintf ( buf, "%s{x: %s {D[{d%c%.2f{D]{x\n\r",
                          IS_NPC ( victim ) ? victim->short_descr : victim->
                          name, meter, '%', percent );
                buf[0] = UPPER ( buf[0] );
                send_to_char ( buf, ch );
                if ( victim->stunned )
                {
                    sprintf ( buf, "{f%s is stunned.{x\n\r",
                              IS_NPC ( victim ) ? victim->short_descr : victim->
                              name );
                    send_to_char ( buf, ch );
                }
                chhit = ch->hit;
                chhitmax = ch->max_hit;
                if ( chhit > 0 && chhitmax > 0 )
                    percent = ( ( chhit * 100 ) / chhitmax );
                else
                    percent = 0;

                if ( percent == 100 )
                    sprintf ( meter, "[{C**********{x]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "[{M********* {x]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "[{Y********  {x]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "[{G*******   {x]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "[{B******    {x]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "[{m*****     {x]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "[{y****      {x]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "[{g***       {x]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "[{b**        {x]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "[{R*         {x]" );
                else if ( percent >= 0 )
                    sprintf ( meter, "[{D*         {x]" );
                if ( percent < 0 )
                    sprintf ( meter, "[            {x]" );
                sprintf ( buf, "You: %s {D[{d%c%.2f{D]\n\r", meter, '%', percent );
                buf[0] = UPPER ( buf[0] );
                send_to_char ( buf, ch );
/*            write_to_buffer( d, buf, 0); */
            }

            ch = d->original ? d->original : d->character;
            if ( !IS_SET ( ch->comm, COMM_COMPACT ) )
                write_to_buffer ( d, "\n\r", 2 );

            if ( IS_SET ( ch->comm, COMM_PROMPT ) )
                bust_a_prompt ( d->character );

/*            if ( IS_SET ( ch->comm, COMM_TELNET_GA ) )
                write_to_buffer ( d, go_ahead_str, 0 ); */
        }
    }
    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
        return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
        if ( d->character != NULL )
            write_to_buffer ( d->snoop_by, d->character->name, 0 );
        write_to_buffer ( d->snoop_by, "> ", 2 );
        write_to_buffer ( d->snoop_by, d->outbuf, d->outtop );
    }

    return process_ansi_output ( d );
}

extern char* stance_name[];
/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 * Completely overhauled for color by RW
 */
void bust_a_prompt ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH * 100];
    char buf2[MAX_STRING_LENGTH * 100];
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool round;
    const char *dir_name[] = { "N", "E", "S", "W", "U", "D" };
    char *door_name[] = { "north", "east", "south", "west", "up", "down" };
    int door, outlet;

    sprintf ( buf2, "%s", ch->prompt );
    if ( buf2 == NULL || buf2[0] == '\0' )
    {
        sprintf ( buf, "<%ldhp %ldm %ldmv> %s", ch->hit, ch->mana, ch->move,
                  ch->prefix );
        send_to_char ( buf, ch );
        return;
    }

    if ( !IS_NPC ( ch ) && ch->pcdata->jailtime > 0 )
    {
    	if ( ch->tells )
	do_replay ( ch, "" );
	printf_to_char ( ch, "You have %d minutes of jail time remaining.\n\r", ch->pcdata->jailtime );
	return;
    }
    
    if ( IS_SET ( ch->comm, COMM_AFK ) )
    {
	if ( ch->tells )
	printf_to_char ( ch, "{R<{YAFK{R>{x {WY{wo{Wu {wh{Wa{wv{We {wt{We{wl{Wl{ws {Ww{wa{Wi{wt{Wi{wn{Wg{w!{x " );
	else 
	printf_to_char ( ch, "{R<{WAFK{R>{x " );
        return;
    }

    found = FALSE;
    doors[0] = '\0';
    if ( ch->position > POS_SLEEPING )
    {
        if ( IS_AFFECTED ( ch, AFF_BLIND ) &&
             !IS_SET ( ch->act, PLR_HOLYLIGHT ) )
        {
            found = TRUE;
            strcpy ( doors, "blinded" );
        }
        else
        {
            for ( door = 0; door < 6; door++ )
            {
                round = FALSE;
                outlet = door;
                if ( ( ch->alignment < 0 ) &&
                     ( pexit = ch->in_room->exit[door + 6] ) != NULL )
                    outlet += 6;
                if ( ( pexit = ch->in_room->exit[outlet] ) != NULL &&
                     pexit->u1.to_room != NULL &&
                     ( can_see_room ( ch, pexit->u1.to_room ) ||
                       ( IS_AFFECTED ( ch, AFF_INFRARED ) &&
                         !IS_AFFECTED ( ch, AFF_BLIND ) ) ) &&
                     !IS_SET ( pexit->exit_info, EX_CLOSED ) )
                {
                    found = TRUE;
                    round = TRUE;
                    strcat ( doors, dir_name[door] );
                }
                if ( !round )
                {
                    OBJ_DATA *portal;

                    portal =
                        get_obj_exit ( door_name[door],
                                       ch->in_room->contents );
                    if ( ( portal != NULL ) &&
                         !IS_AFFECTED ( ch, AFF_BLIND ) )
                    {
                        found = TRUE;
                        round = TRUE;
                        strcat ( doors, dir_name[door] );
                    }
                }
            }
        }
    }
    else
    {
        strcpy ( doors, "sleeping" );
        found = TRUE;
    }	
    if ( !found )
    {
        sprintf ( doors, "none" );
    }
/*    else
    { */
        sprintf ( buf, "%s", doors );
    //}                        
    str_replace_c ( buf2, "%e", buf );
    str_replace_c ( buf2, "%c", "\n\r" );
    sprintf ( buf, "%ld", ch->hit );
    str_replace_c ( buf2, "%h", buf );
    sprintf ( buf, "%ld", ch->max_hit );
    str_replace_c ( buf2, "%H", buf );
    sprintf ( buf, "%ld", ch->mana );
    str_replace_c ( buf2, "%m", buf );
    sprintf ( buf, "%ld", ch->max_mana );
    str_replace_c ( buf2, "%M", buf );
    sprintf ( buf, "%ld", ch->move );
    str_replace_c ( buf2, "%v", buf );
    sprintf ( buf, "%ld", ch->max_move );
    str_replace_c ( buf2, "%V", buf );
    
/*    	if (ch->stance[0] >= 1)
		sprintf(buf, "%d %s", ch->stance[ch->stance[0]],
						stance_name[ch->stance[0]]);
	else sprintf(buf, "No Stance");
    str_replace_c(buf2, "%S", buf);
	if ( ch->pet != NULL )
		if (ch->pet->stance[0] >= 1)
			sprintf(buf, "%d %s", ch->pet->stance[ch->pet->stance[0]],
						stance_name[ch->pet->stance[0]]);
		else sprintf(buf, "No Stance");
  */  
    
    /* Countdown and Nextquest timers - Rhaelar 9'2004 */
    if ( ch->pcdata->nextquest > 0 )
    sprintf ( buf, "%d until next quest", ch->pcdata->nextquest );
    else if ( ch->pcdata->countdown > 0 )
    sprintf ( buf, "%d left for quest", ch->pcdata->countdown );
    else
    sprintf ( buf, "Not on quest" );
    str_replace_c ( buf2, "%q", buf );
/*    sprintf ( buf, "%d", ch->pcdata->countdown );
    str_replace_c ( buf2, "%Q", buf ); */
    
    sprintf ( buf, "%ld", ch->exp );
    str_replace_c ( buf2, "%x", buf );
    if ( !IS_NPC ( ch ) && ch->desc->original == NULL && !IS_SET ( ch->act2, PLR2_NO_EXP ) && ch->level < LEVEL_HERO )
        sprintf ( buf, "%ld", ( ch->level + 1 ) * exp_per_level ( ch, ch->pcdata->points ) - ch->exp );
    else if ( ch->level == LEVEL_HERO )
        sprintf ( buf, "Hero " );
    else if ( ch->level == LEVEL_ANCIENT )
        sprintf ( buf, "Ancient " );
    else if ( ch->level >= LEVEL_IMMORTAL )
        sprintf ( buf, "Immortal " );	
    else if ( IS_SET ( ch->act2, PLR2_NO_EXP ) )
        sprintf ( buf, "Noexp On " );
    else
    sprintf ( buf, "none" );
    str_replace_c ( buf2, "%X", buf );
    sprintf ( buf, "%ld", ch->platinum );
    str_replace_c ( buf2, "%p", buf );
    sprintf ( buf, "%ld", ch->gold );
    str_replace_c ( buf2, "%g", buf );
    sprintf ( buf, "%ld", ch->silver );
    str_replace_c ( buf2, "%s", buf );
    if ( ch->level > 9 )
        sprintf ( buf, "%d", ch->alignment );
    else
        sprintf ( buf, "%s",
                  IS_GOOD ( ch ) ? "good" : IS_EVIL ( ch ) ? "evil" :
                  "neutral" );
    str_replace_c ( buf2, "%a", buf );
    if ( ch->in_room != NULL )
        sprintf ( buf, "%s",
                  ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) ) ||
                    ( !IS_AFFECTED ( ch, AFF_BLIND ) &&
                      !room_is_dark ( ch->in_room ) ) ) ? ch->in_room->
                  name : "darkness" );
    else
        sprintf ( buf, " " );
    str_replace_c ( buf2, "%r", buf );
    if ( IS_IMMORTAL ( ch ) )   // && ch->in_room != NULL) )
        sprintf ( buf, "%ld", ch->in_room->vnum );
    else
        sprintf ( buf, " " );
    str_replace_c ( buf2, "%o", buf );
    if ( IS_IMMORTAL ( ch ) )
        sprintf ( buf, "%s", olc_ed_name ( ch ) );
    str_replace_c ( buf2, "%O", buf );
    if ( IS_IMMORTAL ( ch ) )
        sprintf ( buf, "%s", olc_ed_vnum ( ch ) );
    str_replace_c ( buf2, "%R", buf );
    if ( IS_IMMORTAL ( ch ) )   // && ch->in_room != NULL ))
        sprintf ( buf, "%s", ch->in_room->area->name );
    else
        sprintf ( buf, " " );
    str_replace_c ( buf2, "%z", buf );

    send_to_char ( buf2, ch );

    if ( ch->prefix[0] != '\0' )
        write_to_buffer ( ch->desc, ch->prefix, 0 );

    return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer ( DESCRIPTOR_DATA * d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
        length = strlen ( txt );

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
        d->outbuf[0] = '\n';
        d->outbuf[1] = '\r';
        d->outtop = 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
        char *outbuf;

        if ( d->outsize >= 64000 )
        {
            bug ( "Buffer overflow. Closing.\n\r", 0 );
            bug("Write_to_buffer:  outsize too large:  %d", d->outsize);
            close_socket ( d ); 
            return;
        }

/*        if ( d->outsize >= 31500 )
        {
            close_socket ( d );
            return;
        } */

	  outbuf = alloc_mem ( 2 * d->outsize );
        strncpy ( outbuf, d->outbuf, d->outtop );
        free_mem ( d->outbuf, d->outsize );
        d->outbuf = outbuf;
        d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strncpy ( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor ( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
        desc = 1;
#endif

    if ( length <= 0 )
        length = strlen ( txt );

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
        /* nBlock = UMIN ( length - iStart, 2048 ); *//* Trying to fix bug - Rhaelar */
        nBlock = UMIN ( length - iStart, 9216 ); 
	
        if ( ( nWrite = write ( desc, txt + iStart, nBlock ) ) < 0 )
        {
            perror ( "Write_to_descriptor" );
            return TRUE;
        }
    }

    txt = NULL;
    return TRUE;
}

/* 
 * Function to send the login greeting.
 * Took this out of dnew allocation to implement colour choosing
 * before login. -Rhaelar 12'2004
 */
void send_greeting ( DESCRIPTOR_DATA * d )
{
    /*
     * Send the greeting.
     */
    {
  switch ( number_range ( 0, 1 ) ) // 0, 35 before, this is temporary until greets are restored
        {
            default:
                if ( help_greetinga[0] == '.' )
                    write_to_buffer ( d, help_greetinga + 1, 0 );
                else
                    write_to_buffer ( d, help_greetinga, 0 );
                break;
            case 0:
                if ( help_greetinga[0] == '.' )
                    write_to_buffer ( d, help_greetinga + 1, 0 );
                else
                    write_to_buffer ( d, help_greetinga, 0 );
                break;
            case 1:
                if ( help_greetingb[0] == '.' )
                    write_to_buffer ( d, help_greetingb + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingb, 0 );
                break;
            case 2:
                if ( help_greetingc[0] == '.' )
                    write_to_buffer ( d, help_greetingc + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingc, 0 );
                break;
            case 3:
                if ( help_greetingd[0] == '.' )
                    write_to_buffer ( d, help_greetingd + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingd, 0 );
                break;
            case 4:
                if ( help_greetinge[0] == '.' )
                    write_to_buffer ( d, help_greetinge + 1, 0 );
                else
                    write_to_buffer ( d, help_greetinge, 0 );
                break;
            case 5:
                if ( help_greetingf[0] == '.' )
                    write_to_buffer ( d, help_greetingf + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingf, 0 );
                break;
            case 6:
                if ( help_greetingg[0] == '.' )
                    write_to_buffer ( d, help_greetingg + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingg, 0 );
                break;
	    case 7:
                if ( help_greetingh[0] == '.' )
                    write_to_buffer ( d, help_greetingh + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingh, 0 );
                break;
            case 8:
                if ( help_greetingi[0] == '.' )
                    write_to_buffer ( d, help_greetingi + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingi, 0 );
                break;
            case 9:
                if ( help_greetingj[0] == '.' )
                    write_to_buffer ( d, help_greetingj + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingj, 0 );
                break;
            case 10:
                if ( help_greetingk[0] == '.' )
                    write_to_buffer ( d, help_greetingk + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingk, 0 );
                break;
            case 11:
                if ( help_greetingl[0] == '.' )
                    write_to_buffer ( d, help_greetingl + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingl, 0 );
                break;
            case 12:
                if ( help_greetingm[0] == '.' )
                    write_to_buffer ( d, help_greetingm + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingm, 0 );
                break;
            case 13:
                if ( help_greetingn[0] == '.' )
                    write_to_buffer ( d, help_greetingn + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingn, 0 );
                break;
            case 14:
                if ( help_greetingo[0] == '.' )
                    write_to_buffer ( d, help_greetingo + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingo, 0 );
                break;
            case 15:
                if ( help_greetingp[0] == '.' )
                    write_to_buffer ( d, help_greetingp + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingp, 0 );
                break;
            case 16:
                if ( help_greetingq[0] == '.' )
                    write_to_buffer ( d, help_greetingq + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingq, 0 );
                break;
            case 17:
                if ( help_greetingr[0] == '.' )
                    write_to_buffer ( d, help_greetingr + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingr, 0 );
                break;
            case 18:
                if ( help_greetings[0] == '.' )
                    write_to_buffer ( d, help_greetings + 1, 0 );
                else
                    write_to_buffer ( d, help_greetings, 0 );
                break;
            case 19:
                if ( help_greetingt[0] == '.' )
                    write_to_buffer ( d, help_greetingt + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingt, 0 );
                break;
            case 20:
                if ( help_greetingu[0] == '.' )
                    write_to_buffer ( d, help_greetingu + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingu, 0 );
                break;
            case 21:
                if ( help_greetingv[0] == '.' )
                    write_to_buffer ( d, help_greetingv + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingv, 0 );
                break;
            case 22:
                if ( help_greetingx[0] == '.' )
                    write_to_buffer ( d, help_greetingx + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingx, 0 );
                break;
            case 23:
                if ( help_greetingy[0] == '.' )
                    write_to_buffer ( d, help_greetingy + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingy, 0 );
                break;
            case 24:
                if ( help_greetingz[0] == '.' )
                    write_to_buffer ( d, help_greetingz + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingz, 0 );
                break;
            case 25:
                if ( help_greetingaa[0] == '.' )
                    write_to_buffer ( d, help_greetingaa + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingaa, 0 );
                break;
            case 26:
                if ( help_greetingbb[0] == '.' )
                    write_to_buffer ( d, help_greetingbb + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingbb, 0 );
                break;
            case 27:
                if ( help_greetingcc[0] == '.' )
                    write_to_buffer ( d, help_greetingcc + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingcc, 0 );
                break;
            case 28:
                if ( help_greetingdd[0] == '.' )
                    write_to_buffer ( d, help_greetingdd + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingdd, 0 );
                break;
            case 29:
                if ( help_greetingee[0] == '.' )
                    write_to_buffer ( d, help_greetingee + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingee, 0 );
                break;
            case 30:
                if ( help_greetingff[0] == '.' )
                    write_to_buffer ( d, help_greetingff + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingff, 0 );
                break;
            case 31:
                if ( help_greetinggg[0] == '.' )
                    write_to_buffer ( d, help_greetinggg + 1, 0 );
                else
                    write_to_buffer ( d, help_greetinggg, 0 );
                break;
            case 32:
                if ( help_greetinghh[0] == '.' )
                    write_to_buffer ( d, help_greetinghh + 1, 0 );
                else
                    write_to_buffer ( d, help_greetinghh, 0 );
                break;
            case 33:
                if ( help_greetingii[0] == '.' )
                    write_to_buffer ( d, help_greetingii + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingii, 0 );
                break;
            case 34:
                if ( help_greetingjj[0] == '.' )
                    write_to_buffer ( d, help_greetingjj + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingjj, 0 );
                break;
            case 35:
                if ( help_greetingkk[0] == '.' )
                    write_to_buffer ( d, help_greetingkk + 1, 0 );
                else
                    write_to_buffer ( d, help_greetingkk, 0 );
                break;
	}

        if ( help_authors[0] == '.' )
            write_to_buffer ( d, help_authors + 1, 0 );
        else
            write_to_buffer ( d, help_authors, 0 );
        if ( help_login[0] == '.' )
            write_to_buffer ( d, help_login + 1, 0 );
        else
            write_to_buffer ( d, help_login, 0 );
    }
    return;
}


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny ( DESCRIPTOR_DATA * d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char buf[MAX_STRING_LENGTH];
    char newbuf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    char *pwdnew;
    char *p;
//    int iClass, race, i, weapon;
    int iClass, race, i;
    int pos = 0;
    bool fOld;
/*
    while ( isspace ( *argument ) )
        argument++;
*/

    /* Delete leading spaces UNLESS character is writing a note */
    if (d->connected != CON_NOTE_TEXT)
    {
        while ( isspace(*argument) )
            argument++;
    }

    ch = d->character;

    switch ( d->connected )
    {

        default:
            bug ( "Nanny: bad d->connected %d.", d->connected );
            close_socket ( d );
            return;

	case CON_NOT_PLAYING:
	    break;
	
        case CON_GET_NAME:
            if ( argument[0] == '\0' )
            {
		write_to_buffer ( d, "You need to type something. Reconnect and try again.\n\r", 0 );
                close_socket ( d );
                return;
            }

            argument[0] = UPPER ( argument[0] );
            if ( !check_parse_name ( argument ) )
            {
                write_to_buffer ( d, "Illegal name, try another.\n\r", 0 );
                write_to_buffer ( d,
                                  "(If you've used this name here before, and are no\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " longer able to, it may be because we've added a\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " new mobile that uses the same name. Log in with\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " a new name, and let Rhaelar know, and we will fix it.)\n\r",
                                  0 );

/*                write_to_buffer ( d, "\n\rName: ", 0 ); */
		close_socket ( d );
                return;
            }

            fOld = load_char_obj ( d, argument );
            ch = d->character;

            if ( IS_SET ( ch->act, PLR_DENY ) )
            {
                sprintf ( log_buf, "Denying access to %s@%s.", argument,
                          d->host );
                log_string ( log_buf );
                write_to_buffer ( d, "You are denied access.\n\r", 0 );
                close_socket ( d );
                return;
            }

            if ( check_ban ( d->host, BAN_PERMIT ) &&
                 !IS_SET ( ch->act, PLR_PERMIT ) )
            {
                write_to_buffer ( d,
                                  "You have been banned from SD.\n\rIf you think this is a mistake, email rhaelar@sdmud.com.\n\r",
                                  0 );
                close_socket ( d );
                return;
            }

/*
	if (check_adr(d->host,BAN_PERMIT) && (ch->level > 101) )
	{
	    write_to_buffer(d,"Immortals are not allowed to connect from your site.\n\r",0);
	    close_socket(d);
	    return;
	}
*/
            if ( IS_SET ( ch->act2, PLR2_WIPED ) )
            {
	        d->outtop = 0;
                close_socket ( d );                 return;
            }

            if ( check_reconnect ( d, argument, FALSE ) )
            {
                fOld = TRUE;
            }
            else
            {
                if ( wizlock && ( !IS_IMMORTAL ( ch ) ) )
                {
                    write_to_buffer ( d,
                                      "The game is currently wizlocked. Please come back later.\n\r",
                                      0 );
                    if ( !IS_IMMORTAL ( ch ) )
                    {
                        close_socket ( d );
                        return;
                    }
                }
            }

            if ( fOld )
            {
/*                if ( newlock && IS_IMMORTAL ( ch ) )
                    write_to_buffer ( d,
                                      "The game is currently newlocked!\n\r", 
                                      0 ); */
                /* Old player */
                write_to_buffer ( d, "{xPassword: ", 0 );
                write_to_buffer ( d, echo_off_str, 0 );
                d->connected = CON_GET_OLD_PASSWORD;
                return;
            }
            else
            {
                /* New player */
                if ( newlock )
                {
                    write_to_buffer ( d, "The game is newlocked.\n\r Please log an already existing character or email rhaelar@sdmud.com if you don't have any.\n\r", 0 );
                    close_socket ( d );
                    return;
                }

                if ( check_ban ( d->host, BAN_NEWBIES ) )
                {
                    write_to_buffer ( d,
                                      "The Gods of SD have revoked your character creation abilities.\n\r",
                                      0 );
                    write_to_buffer ( d,
                                      "Please log an already existing character.\n\r",
                                      0 );
                    write_to_buffer ( d,                                       "If you think this is a mistake, email rhaelar@sdmud.com.\n\r",
                                      0 );
                    close_socket ( d );
                    return;
                }

                sprintf ( buf, "So, {R%s{x, you call yourself a {Cd{creame{Cr{x? (Y/N)? ", argument );
                write_to_buffer ( d, buf, 0 );
                d->connected = CON_CONFIRM_NEW_NAME;
                return;
            }
            break;    

case CON_COLOUR_LOGIN:
    if ( argument[0] == '\0' )
	{
	    write_to_buffer ( d, "You need to type something. Reconnect and try again.\n\r", 0 );
	    close_socket( d );
	    return;
	}

           if ( reboot_countdown == 1 )
            {
                write_to_buffer ( d,
                                  "\n\r\n\rShattered Dreams is getting ready for the daily maintenance reboot in less than 1 minute.\n\rPlease reconnect in one minute.\n\r\n\r",
                                  0 );
                close_socket ( d );
                return;
            }
	
    if (d->lookup_status != STATUS_DONE)
    {
       write_to_buffer( d, "\n\r     SD is still looking up your host. Please be patient.\n\r", 0 );
       write_to_buffer( d, "           [0m[0;33m-[0m[0;32m-[0m[0;31m-[0m Enable ANSI colouring? (Y/N) [0m[0;31m-[0m[0;32m-[0m[0;33m-[0m ", 0 );

       return;
    }
    
/*    if ( is_banned ( d ) )
    {
       write_to_descriptor( desc, "You are not allowed to play on Shattered Dreams.\n\rIf you think this is a mistake, file a complaint to rhaelar@sdmud.com\n\rGoodbye.\n\r", 0 );
       close_socket( d );
       return;
    }     */
    if ( check_ban ( d->host, BAN_ALL ) )
    {
        write_to_buffer ( d, "You are not allowed to play on Shattered Dreams.\n\rIf you think this is a mistake, file a complaint to rhaelar@sdmud.com\n\rGoodbye.\n\r", 0 );
        close_socket ( d );
	return;
    }    

    switch( *argument )
	{
	case 'y' :
	case 'Y' :
		d->ansi = TRUE;
		write_to_buffer( d, "\n\rWoo! ANSI Colouring enabled!\n\r", 0 );
		send_greeting ( d );
		d->connected = CON_GET_NAME;
		return;
	case 'n' :
	case 'N' :
	  	d->ansi = FALSE;
	  	write_to_buffer( d, "\n\rColor disabled. Type 'color' in-game to enable it.\n\r", 0 );
		send_greeting ( d );
		d->connected = CON_GET_NAME;
		return;
	default:
		write_to_buffer(d,"                     (Y)es or (N)o? ", 0);
		break;
	}
    break;


        case CON_GET_OLD_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( str_cmp
                 ( md5crypt ( argument ), ch->pcdata->pwd ) )
            {
                write_to_buffer ( d, "The password you entered was wrong.\n\r", 0 );
                write_to_buffer ( d, "Please reconnect and try again.\n\rIf you are completely sure you entered the right password,\n\r", 0 );
                write_to_buffer ( d, "and you're still getting this message,\n\rcreate a new character and notify Rhaelar via tell or note.\n\r", 0 );
		sprintf ( log_buf, "%s@%s: Wrong password", ch->name, d->host );
		wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );
		log_string ( log_buf );
		append_file ( ch, WRONGPW_FILE, log_buf );
                close_socket ( d );
                return;
            }
/*	    DESCRIPTOR_DATA *dm;
	    DESCRIPTOR_DATA *dm_next;
	    for ( dm = descriptor_list; dm != NULL; dm = dm_next )
            {
	        dm_next = dm->next;
		if ( !str_cmp ( dm->host, d->host ) && !IS_SET ( d->character->act2, PLR2_MULTIPLAY ) )
		{
		    write_to_buffer ( d, "You are not allowed to log on more than one character from your host.\n\rSpeak to Rhaelar if you need multiplaying permission (e.g. on a LAN with more people, etc.)\n\r", 0 );
		    close_socket ( d );
		}
	    }  */

	    if ( IS_INACTIVE ( ch ) )
	    {
	        write_to_buffer ( d, "Sorry. Due to your long abscence, your immortal character has been temporarily suspended.\n\rLog on a mortal and send a note to me, Rhaelar, or leave an email to rhaelar@gmail.com.\n\r", 0 );
		close_socket ( d );
		return;
	    }
	    	    
            write_to_buffer ( d, echo_on_str, 0 );

            if ( check_playing ( d, ch->name ) )
                return;
            
//	    load_char_obj ( d, ch->name );

            free_string ( ch->pcdata->socket );
            ch->pcdata->socket = str_dup ( d->host );
            if ( check_reconnect ( d, ch->name, TRUE ) )
                return;
            sprintf ( log_buf, "%s@%s has connected.", ch->name, d->host );
            log_string ( log_buf );
            wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );
            free_string ( ch->pcdata->socket );

            ch->pcdata->socket = str_dup ( d->host );
/*	    if ( IS_SET ( ch->act, PLR_NOCOLOUR ) )
	    d->ansi = FALSE; */
	    if ( d->ansi )
	    REMOVE_BIT ( ch->act, PLR_NOCOLOUR );
	    else
	    SET_BIT ( ch->act, PLR_NOCOLOUR );
/*	if (IS_SET(ch->act, PLR_REROLL ) )
	{
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    sprintf(newbuf, "%s", str_dup( ch->pcdata->pwd ));
	    sprintf( argument, "%s", capitalize( ch->name ) );
	    free_char( d->character );
	    d->character = NULL;
	    
	    ch   = d->character;
	    free_string( ch->pcdata->pwd );
	    ch->pcdata->pwd	= str_dup( newbuf );
	    newbuf[0] = '\0';
	    ch->pcdata->tier = 1;
        free_string( ch->pcdata->socket );
	    ch->pcdata->socket = str_dup( d->host );
	    write_to_buffer( d, echo_on_str, 0 );
	    write_to_buffer(d, "The following races are available:\n\r\n\r",0);
	    pos = 0;
	    for ( race = 1; race_table[race].name != NULL; race++ )
	    {
		if (!race_table[race].pc_race || !IS_SET(ch->pcdata->ctier, pc_race_table[race].tier))
		    continue;
			sprintf(newbuf, "{g%6s{g%-24s{x", " ", race_table[race].name);
			write_to_buffer(d,newbuf,0);
			pos++;
			if (pos >= 2)
			{ 
				write_to_buffer(d,"\n\r",1);
				pos = 0;
			}
		}
	    newbuf[0] = '\0';
	    write_to_buffer(d,"\n\r\n\r",0);
	    write_to_buffer(d, "What is your race (help for more information)?.\n\r",0);
	    d->connected = CON_GET_NEW_RACE;
	    break;
	}
OLD REROLL DUDE! */

            /*if ( IS_IMMORTAL(ch) )
               {
               do_help( ch, "imotd" );
               d->connected = CON_READ_IMOTD;
               } */
	    if ( IS_IMMORTAL ( ch ) )
            {
                write_to_buffer ( d,
                                  "{cWould you like to login {W({BW{W){cizinvis, {W({BI{W){cncognito, or {W({BN{W){cormal?{x",
                                  0 );
                d->connected = CON_WIZ;
            }

            else
            {
                do_help ( ch, "motd" );
                d->connected = CON_READ_MOTD;
            }

            for ( pos = 0; pos < MAX_DUPES; pos++ )
            {
                if ( ch->pcdata->dupes[pos] == NULL )
                    break;

                if ( ( victim =
                       get_char_mortal ( ch,
                                         ch->pcdata->dupes[pos] ) ) != NULL )
                    force_quit ( victim, "" );
            }

            break;
        case CON_WIZ:
            write_to_buffer ( d, "\n\r", 2 );
            switch ( *argument )
            {
                case 'w':
                case 'W':
                    ( ch->invis_level = ch->level );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                case 'i':
                case 'I':
                    ( ch->incog_level = ch->level );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                case 'n':
                case 'N':
                    ( ch->incog_level = 0 );
                    ( ch->invis_level = 0 );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WI{wnvalid {Ychoice{w, entering in {cnormal{w mode.{x\n\r\n\r",
                                      0 );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
            }
            break;

/* RT code for breaking link */

        case CON_BREAK_CONNECT:
            switch ( *argument )
            {
                case 'y':
                case 'Y':
                    for ( d_old = descriptor_list; d_old != NULL;
                          d_old = d_next )
                    {
                        d_next = d_old->next;
                        if ( d_old == d || d_old->character == NULL )
                            continue;

                        if ( str_cmp
                             ( ch->name,
                               d_old->original ? d_old->original->
                               name : d_old->character->name ) )
                            continue;
		        act ( "{W$n bends in agony as $s body is taken over by another {z{Dsoul{x{W!{x", d_old->character, NULL, NULL, TO_ROOM );
                        close_socket ( d_old );
                    }
                    free_string ( ch->pcdata->socket );
                    ch->pcdata->socket = str_dup ( d->host );
                    if ( check_reconnect ( d, ch->name, TRUE ) )
                        return;
                    write_to_buffer ( d,
                                      "Reconnect attempt failed.\n\rName: ",
                                      0 );
                    if ( d->character != NULL )
                    {
                        free_char ( d->character );
                        d->character = NULL;
                    }
                    d->connected = CON_GET_NAME;
                    break;

                case 'n':
                case 'N':
                    write_to_buffer ( d, "Name: ", 0 );
                    if ( d->character != NULL )
                    {
                        free_char ( d->character );
                        d->character = NULL;
                    }
                    d->connected = CON_GET_NAME;
                    break;

                default:
                    write_to_buffer ( d, "Please type Y or N? ", 0 );
                    break;
            }
            break;

            if ( reboot_countdown == 1 )
            {
                write_to_buffer ( d,
                                  " The mud is getting ready for a reboot in less than 1 minute.\n\rPlease try back then.\n\r",
                                  0 );
                close_socket ( d );
                return;
            }

        case CON_CONFIRM_NEW_NAME:
            switch ( *argument )
            {
                case 'y':
                case 'Y':
                    sprintf ( buf,
                              "New character.\n\rGive me a password for %s: %s",
                              ch->name, echo_off_str );
                    free_string ( ch->pcdata->socket );
                    ch->pcdata->socket = str_dup ( d->host );
                    write_to_buffer ( d, buf, 0 );
                    d->connected = CON_GET_NEW_PASSWORD;
                    break;

                case 'n':
                case 'N':
                    write_to_buffer ( d, "Ok, who is a dreamer then? ", 0 );
                    free_char ( d->character );
                    d->character = NULL;
                    d->connected = CON_GET_NAME;
                    break;

                default:
                    write_to_buffer ( d, "Please type {RYes {xor {RNo{x. ", 0 );
                    break;
            }
            break;

        case CON_BEGIN_REROLL:
            write_to_buffer ( d,
                              "{RNow beginning the rerolling process.{x\n\r\n\r",
                              0 );
            write_to_buffer ( d,
                              "{RThe following races are available:{x\n\r  ",
                              0 );
            write_to_buffer ( d,
                              "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{DShattered Dreams {gRaces{r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                              0 );
            pos = 0;
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
                if ( !race_table[race].pc_race ||
                     !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
                    continue;

                sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                          race_table[race].name );
                write_to_buffer ( d, newbuf, 0 );
                pos++;
                if ( pos >= 2 )
                {
                    write_to_buffer ( d, "\n\r", 1 );
                    pos = 0;
                }
            }
            write_to_buffer ( d, "\n\r", 0 );
            write_to_buffer ( d,
                              "What is your race (help for more information)? ",
                              0 );
            d->connected = CON_GET_NEW_RACE;
            break;

        case CON_GET_NEW_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( strlen ( argument ) < 5 )
            {
                write_to_buffer ( d,
                                  "Password must be at least five characters long.\n\rPassword: ",
                                  0 );
                return;
            }

            pwdnew = md5crypt ( argument );
            for ( p = pwdnew; *p != '\0'; p++ )
            {
                if ( *p == '~' )
                {
                    write_to_buffer ( d,
                                      "New password not acceptable, try again.\n\rPassword: ",
                                      0 );
                    return;
                }
            }

            free_string ( ch->pcdata->pwd );
            ch->pcdata->pwd = str_dup ( pwdnew );
            write_to_buffer ( d, "Please retype password: ", 0 );
            d->connected = CON_CONFIRM_NEW_PASSWORD;
            break;

        case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( str_cmp
                 ( md5crypt ( argument ), ch->pcdata->pwd ) )
            {
                write_to_buffer ( d,
                                  "Passwords don't match.\n\rRetype password: ",
                                  0 );
                d->connected = CON_GET_NEW_PASSWORD;
                return;
            }

            free_string ( ch->pcdata->socket );
            ch->pcdata->socket = str_dup ( d->host );
            ch->pcdata->cname = str_dup ( ch->name );
            write_to_buffer ( d, echo_on_str, 0 );

/*
        case CON_GET_ANSI:
            switch ( argument[0] )
            {
                case 'y':
                case 'Y':
                    d->ansi = FALSE;
                    SET_BIT ( ch->act, PLR_NOCOLOUR );
                    break;
                case 'n':
                case 'N':
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WPlease answer {YYes{W or {RNo{W.{x\n\r",
                                      0 );
                    return;
            } */

            write_to_buffer ( d,
                              "\n\r",
                              0 );

            write_to_buffer ( d,
                              "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{DShattered Dreams {gRaces{r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                              0 );
            pos = 0;
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
                if ( !race_table[race].pc_race ||
                     !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
                    continue;

                sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                          race_table[race].name );
                write_to_buffer ( d, newbuf, 0 );
                pos++;
                if ( pos >= 2 )
                {
                    write_to_buffer ( d, "\n\r", 1 );
                    pos = 0;
                }
            }
            newbuf[0] = '\0';
            write_to_buffer ( d, "\n\r\n\r", 0 );
            write_to_buffer ( d,
                              "{WIf you are new here, a good choice would be {Yhuman{W.{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{RNOTICE: {WDragons are {YHARD{W to level!{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{WWhat is your race (help for more information)?{x ",
                              0 );
            d->connected = CON_GET_NEW_RACE;
            break;

        case CON_GET_NEW_RACE:
            one_argument ( argument, arg );

            if ( !str_cmp ( arg, "help" ) )
            {
                argument = one_argument ( argument, arg );
                if ( argument[0] == '\0' )
                    do_help ( ch, "race help" );
                else
                    do_help ( ch, argument );
            write_to_buffer ( d,
                              "{YHuman{W are easiest to level.{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{RNOTICE: {WDragons are {YHARD{W to level!{x\n\r",
                              0 );
                write_to_buffer ( d,
                                  "{WWhat is your race (help for more information)?{x ",
                                  0 );
                break;
            }

            race = race_lookup ( argument );

            if ( race == 0 || !race_table[race].pc_race ||
                 !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
            {
                write_to_buffer ( d, "{RThat is not a valid race.{x\n\r", 0 );
                write_to_buffer ( d,
                                  "{RThe following races are available:\n\r{x  ",
                                  0 );
                write_to_buffer ( d,
                                  "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{DShattered Dreams {gRaces{r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                                  0 );
                for ( race = 1; race_table[race].name != NULL; race++ )
                {
                    if ( !race_table[race].pc_race ||
                         !IS_SET ( ch->pcdata->ctier,
                                   pc_race_table[race].tier ) )
                        continue;

                    sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                              race_table[race].name );
                    write_to_buffer ( d, newbuf, 0 );
                    pos++;
                    if ( pos >= 2 )
                    {
                        write_to_buffer ( d, "\n\r", 2 );
                        pos = 0;
                    }
                }
                write_to_buffer ( d, "\n\r", 0 );
                write_to_buffer ( d,
                                  "{WWhat is your race? (help for more information){x ",
                                  0 );
                break;
            }

            ch->race = race;
            /* initialize stats */
            for ( i = 0; i < MAX_STATS; i++ )
                ch->perm_stat[i] = pc_race_table[race].stats[i];
            ch->act = ch->act | race_table[race].act;
            ch->act2 = ch->act2 | race_table[race].act2;
            ch->affected_by = ch->affected_by | race_table[race].aff;
            ch->shielded_by = ch->shielded_by | race_table[race].shd;
            ch->imm_flags = ch->imm_flags | race_table[race].imm;
            ch->res_flags = ch->res_flags | race_table[race].res;
            ch->vuln_flags = ch->vuln_flags | race_table[race].vuln;
            ch->form = race_table[race].form;
            ch->parts = race_table[race].parts;

            /* add skills */
            for ( i = 0; i < 5; i++ )
            {
                if ( pc_race_table[race].skills[i] == NULL )
                    break;
                group_add ( ch, pc_race_table[race].skills[i], FALSE );
            }
            /* add cost */
            ch->pcdata->points = pc_race_table[race].points;
            ch->size = pc_race_table[race].size;
            write_to_buffer ( d,
                              "{WThe following classes are available:{x\n\r\n\r",
                              0 );
/*
            for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            {
                if ( !IS_SET ( ch->pcdata->ctier, class_table[iClass].tier ) )
                    continue;
                sprintf ( buf, "      {%c%s{x\n\r",
                          IS_SET ( class_table[iClass].tier,
                                   ch->pcdata->ctier ) ? 'R' : 'B',
                          class_table[iClass].name );
                write_to_buffer ( d, buf, 0 );
            }
            write_to_buffer ( d, "\n\r\n\r", 0 ); */
	do_help ( ch, "NEWCHARTIER" );
/*
        write_to_buffer(d,  "{WWhat is your sex {w({BM{x/{MF{x/{RN{w){W?{x ", 0 );
        d->connected = CON_GET_NEW_SEX;*/
            write_to_buffer ( d, "{WWhat is your class ? {x", 0 ); 
            d->connected = CON_GET_NEW_CLASS;
            break;

        case CON_GET_NEW_SEX:
            switch ( argument[0] )
            {
                case 'm':
                case 'M':
                    ch->sex = SEX_MALE;
                    ch->pcdata->true_sex = SEX_MALE;
                    break;
                case 'f':
                case 'F':
                    ch->sex = SEX_FEMALE;
                    ch->pcdata->true_sex = SEX_FEMALE;
                    break;
                case 'n':
                case 'N':
                    ch->sex = SEX_NEUTRAL;
                    ch->pcdata->true_sex = SEX_NEUTRAL;
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WThat's not a sex.\n\rWhat IS your sex? {x",
                                      0 );
                    return;
            }

            write_to_buffer ( d, echo_on_str, 0 );  /*
                                                       write_to_buffer(d, "{WThe following classes are available:{x\n\r\n\r",0);
                                                       for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
                                                       {
                                                       if ( !IS_SET(ch->pcdata->ctier, class_table[iClass].tier) )
                                                       continue;
                                                       sprintf(buf, "      {%c%s{x\n\r", IS_SET(class_table[iClass].tier, ch->pcdata->ctier) ? 'R' : 'B',
                                                       class_table[iClass].name);
                                                       write_to_buffer(d,buf,0);
                                                       }
                                                       write_to_buffer(d,"\n\r\n\r",0);
                                                       write_to_buffer(d, "{WWhat is your class ? {x",0);
                                                       d->connected = CON_GET_NEW_CLASS; */
            write_to_buffer ( d,
                              "\n\r{WYou may be {Bgood{W, {Gneutral{W, or {Devil{W.{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{WWhich alignment {w({BG{w/{GN{w/{DE{x){W?{x ",
                              0 );
            d->connected = CON_GET_ALIGNMENT;
            break;
	    
    case CON_GET_HOMETOWN:
    	if (get_hometown(argument) == -1)
    	{
    		write_to_buffer( d, "That's not a valid selection.\n\r",0);
    		write_to_buffer( d, "\n\rValid selections are:\n\r",0);
		for (i=0;hometown_table[i].name != NULL; ++i)
		{
			sprintf(buf,"{D[{W%-15s{D]{x\n\r", hometown_table[i].name );
			write_to_buffer( d, buf, 0 );
		}
		write_to_buffer( d, "\n\rWe recommend SilverXire for new players.\n\r", 0 );    		
		write_to_buffer( d, "\n\rWhat's your {yhometown{x? ", 0);
    		return;
    	}
    			
	ch->hometown = get_hometown(argument);
	
        //d->connected = CON_GET_ALIGNMENT;
	do_help ( ch, "motd" );
	d->connected = CON_READ_MOTD;
	break;
	    
        case CON_GET_NEW_CLASS:
            iClass = class_lookup ( argument );

            if ( iClass == -1 )
            {
                write_to_buffer ( d,
                                  "That's not a class.\n\rWhat IS your class? ",
                                  0 );
                return;
            }
            if ( !IS_SET ( ch->pcdata->ctier, class_table[iClass].tier ) )
            {
                write_to_buffer ( d,
                                  "That class is not allowed to your tier.\n\rWhat IS your class? ",
                                  0 );
                return;
            }
            ch->class = iClass;

            sprintf ( log_buf, "%s@%s new player.", ch->name, d->host );
            log_string ( log_buf );
            wiznet ( "Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0,
                     0 );
            wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );

            write_to_buffer ( d, "\n\r", 2 );   /*
                                                   write_to_buffer(d,  "{WYou may be {Bgood{W, {Gneutral{W, or {Devil{W.{x\n\r",0);
                                                   write_to_buffer(d,  "{WWhich alignment {w({BG{w/{GN{w/{DE{x){W?{x ",0);
                                                   d->connected = CON_GET_ALIGNMENT; */
            write_to_buffer ( d,
                              "{WWhat is your sex {w({BM{x/{MF{x/{RN{w){W?{x ",
                              0 );
            d->connected = CON_GET_NEW_SEX;
            break;

        case CON_GET_ALIGNMENT:
            switch ( argument[0] )
            {
                case 'g':
                case 'G':
                    ch->alignment = 1000;
                    break;
                case 'n':
                case 'N':
                    ch->alignment = 0;
                    break;
                case 'e':
                case 'E':
                    ch->alignment = -1000;
                    break;
                default:
                    write_to_buffer ( d, "That's not a valid alignment.\n\r",
                                      0 );
                    write_to_buffer ( d, "{WWhich alignment {w({BG{w/{GN{w/{DE{x){W?{x ", 0 );
                    return;
            }

            write_to_buffer ( d, "\n\r", 0 );

            group_add ( ch, "rom basics", FALSE );
            group_add ( ch, class_table[ch->class].base_group, FALSE );
	    
            ch->pcdata->learned[gsn_recall] = 30;
            ch->pcdata->learned[gsn_flee] = 30;
	    ch->pcdata->learned[gsn_dagger] = 30;
	    ch->pcdata->learned[gsn_sword] = 30;
            write_to_buffer ( d,
                              "{WDo you wish to customize this character?{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{WHere at {RS{rhattered {RD{rreams{W we advise only veteran mudders to customize.{x\n\r",
                              0 );
            write_to_buffer ( d, "{WCustomize {w({YY{w/{RN{w){W?{x ", 0 );
            d->connected = CON_DEFAULT_CHOICE;
            break;

        case CON_DEFAULT_CHOICE:
            write_to_buffer ( d, "\n\r", 2 );
            switch ( argument[0] )
            {
                case 'y':
                case 'Y':
                    ch->gen_data = new_gen_data (  );
                    ch->gen_data->points_chosen = ch->pcdata->points;
                    do_help ( ch, "group header" );
                    list_group_costs ( ch );
                    write_to_buffer ( d,
                                      "{WYou already have the following skills:{x\n\r",
                                      0 );
                    do_skills ( ch, "{g" );
                    do_help ( ch, "menu choice" );
                    d->connected = CON_GEN_GROUPS;
                    break;
                case 'n':
                case 'N':
                    group_add ( ch, class_table[ch->class].default_group,
                                TRUE );
                    write_to_buffer ( d, "\n\r", 2 );
/*                    write_to_buffer ( d,
                                      "{WPlease pick a weapon from the following choices:\n\r",
                                      0 );
                    buf[0] = '\0';
                    for ( i = 0; weapon_table[i].name != NULL; i++ )
                        if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                        {
                            strcat ( buf, weapon_table[i].name );
                            strcat ( buf, " " );
                        }
                    strcat ( buf, "\n\r{WYour choice? {x" );
                    write_to_buffer ( d, buf, 0 );*/
		    //do_help ( ch, "motd" );
		                write_to_buffer( d, "What will your {yhometown{x be?\n\r", 0 );
	for (i=0;hometown_table[i].name != NULL; ++i)
	{
		sprintf(buf,"{D[{W%-15s{D]{x\n\r", hometown_table[i].name );
		write_to_buffer( d, buf, 0 );
	}
	write_to_buffer( d, "\n\rWe recommend SilverXire for new players.\n\r", 0 );
	write_to_buffer( d, "\n\rWhat's your {yhometown{x? ", 0);
	d->connected = CON_GET_HOMETOWN;
//                    d->connected = CON_READ_MOTD; 
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WPlease answer {w({YY{w/{RN{w){W?{x ",
                                      0 );
                    return;
            }
            break;

/*        case CON_PICK_WEAPON:
            write_to_buffer ( d, "{W \n\r", 2 );
            weapon = weapon_lookup ( argument );
            if ( weapon == -1 ||
                 ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0 )
            {
                write_to_buffer ( d,
                                  "That's not a valid selection. Choices are:\n\r",
                                  0 );
                buf[0] = '\0';
                for ( i = 0; weapon_table[i].name != NULL; i++ )
                    if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                    {
                        strcat ( buf, weapon_table[i].name );
                        strcat ( buf, " " );
                    }
                strcat ( buf, "\n\r{WYour choice?{x " );
                write_to_buffer ( d, buf, 0 );
                return;
            }

            ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
            write_to_buffer ( d, "\n\r", 2 ); 
            do_help ( ch, "motd" ); 
            d->connected = CON_READ_MOTD;
            break;*/

        case CON_GEN_GROUPS:
            send_to_char ( "\n\r", ch );
            if ( !str_cmp ( argument, "done" ) )
            {
                sprintf ( buf, "{WYou have spent a total of {b%d {Wcreation points, which make you need {Y%ld{W experience per level.{x\n\r",
                          ch->pcdata->points, ( long ) exp_per_level ( ch, ch->gen_data->points_chosen ) );
                send_to_char ( buf, ch );
/*                sprintf ( buf, "{WExperience per level: {Y%ld{x\n\r",
                          ( long ) exp_per_level ( ch,
                                                   ch->gen_data->
                                                   points_chosen ) ); */
/*                if ( ch->pcdata->points < 40 )
                    ch->train = ( 40 - ch->pcdata->points + 1 ) / 2; */
                free_gen_data ( ch->gen_data );
                ch->gen_data = NULL;
//                send_to_char ( buf, ch );
                write_to_buffer ( d, " \n\r", 2 );
/*                write_to_buffer ( d,
                                  "{WPlease pick a weapon from the following choices:{x\n\r",
                                  0 );
                buf[0] = '\0';
                for ( i = 0; weapon_table[i].name != NULL; i++ )
                    if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                    {
                        strcat ( buf, weapon_table[i].name );
                        strcat ( buf, " " );
                    }
                strcat ( buf, "\n\r{WYour choice?{x " );
                write_to_buffer ( d, buf, 0 ); */
/*	        do_help ( ch, "motd" );
                d->connected = CON_READ_MOTD; */
		                write_to_buffer( d, "What will your {yhometown{x be?\n\r", 0 );
	for (i=0;hometown_table[i].name != NULL; ++i)
	{
		sprintf(buf,"{D[{W%-15s{D]{x\n\r", hometown_table[i].name );
		write_to_buffer( d, buf, 0 );
	}
	write_to_buffer( d, "\n\rWe recommend SilverXire for new players.\n\r", 0 );
	write_to_buffer( d, "\n\rWhat's your {yhometown{x? ", 0);
	d->connected = CON_GET_HOMETOWN;		
                break;
            }

            if ( !parse_gen_groups ( ch, argument ) )
                send_to_char
                    ( "{WChoices are: list,learned,premise,add,drop,info,class,help, and done.{x\n\r",
                      ch );

            do_help ( ch, "menu choice" );
            break;

        case CON_READ_IMOTD:
            write_to_buffer ( d, "\n\r", 2 );
            do_help ( ch, "motd" );
            d->connected = CON_READ_MOTD;
            break;

        /* states for new note system, (c)1995-96 erwin@andreasen.org */
        /* ch MUST be PC here; have nwrite check for PC status! */

    case CON_NOTE_TO:
        handle_con_note_to (d, argument);
        break;

    case CON_NOTE_SUBJECT:
        handle_con_note_subject (d, argument);
        break; /* subject */

    case CON_NOTE_EXPIRE:
        handle_con_note_expire (d, argument);
        break;

    case CON_NOTE_TEXT:
        handle_con_note_text (d, argument);
        break;

    case CON_NOTE_FINISH:
        handle_con_note_finish (d, argument);
        break;



        case CON_READ_MOTD:

            if ( IS_QUESTOR ( ch ) )
            {
                do_quest ( ch, "info" );
                if ( ch->pcdata->questobj > 0 )
                {
                    OBJ_INDEX_DATA *pObj = NULL;
                    OBJ_DATA *obj = NULL;
                    ROOM_INDEX_DATA *pRoom = NULL;

                    if ( ( pObj =
                           get_obj_index ( ch->pcdata->questobj ) ) == NULL )
                        end_quest ( ch, 0 );
                    else
                        obj = create_object ( pObj, ch->level );

                    if ( ( pRoom =
                           get_room_index ( ch->pcdata->questloc ) ) == NULL )
                        pRoom = get_random_room ( ch );

                    obj_to_room ( obj, pRoom );
                }
            }

            if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0' )
            {
                write_to_buffer ( d, "\n\r\n\r\n\rWarning! Null password!\n\r", 0 );
                write_to_buffer ( d,
                                  "Please report old password with bug.\n\r",
                                  0 );
                write_to_buffer ( d,
                                  "Type 'password null <new password>' to fix.\n\r\n\r\n\r\n\r",
                                  0 );
            }

            write_to_buffer ( d,
                              "\n\rWelcome to Shattered Dreams. We addict players for their own enjoyment.\n\r",
                              0 );
            ch->next = char_list;
            char_list = ch;
            d->connected = CON_PLAYING;
            reset_char ( ch );
            sprintf ( buf,
                      "\n\r{WYou are currently logged on as {c%s.{x\n\r\n\r\n\r",
                      ch->name );
            send_to_char ( buf, ch );

            if ( ch->level == 0 )
            {

                ch->perm_stat[class_table[ch->class].attr_prime] += 4;
/* add this while initializing all the racial stuff for new characters */
                ch->level = 1;
                ch->exp = exp_per_level ( ch, ch->pcdata->points );
/*                ch->hit = ch->max_hit;
                ch->mana = ch->max_mana;
                ch->move = ch->max_move; */
		if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) )
		{
		   ch->train = 40;
                   ch->practice = 40;
		}
		else if ( IS_SET ( class_table[ch->class].tier, TIER_02 ) )
		{
		   ch->train = 30;
		   ch->practice = 30;
		}
		else if ( IS_SET ( class_table[ch->class].tier, TIER_03 ) )
		{
		   ch->train = 15;
		   ch->practice = 15;
		}
		else
		{
		   ch->train = 5;
		   ch->practice = 5;
		}
                ch->pcdata->plr_wager = 0;  /* arena betting amount */
/*                ch->pcdata->awins = 0;  */ /* arena wins           */
/*                ch->pcdata->alosses = 0;   */ /* arena losses         */
/*		ch->pcdata->pkviolenttime = 0;
		ch->pcdata->pksafetime = 0; */
		ch->lines = 35;
/*                sprintf ( buf, "the %s",
                          title_table[ch->class][ch->level][ch->sex ==
                                                            SEX_FEMALE ? 1 :
                                                            0] );
*/
	
	ch->max_hit = 100;
	ch->max_mana = 100;
	ch->max_move = 100;

/*                sprintf ( buf, "{x" );
                set_title ( ch, buf ); */
                do_pack ( ch, "self" );
                do_autoall ( ch, "self" );
/*                do_qspell ( ch, "self" ); */
		do_prompt ( ch, "questor" ); /* People like autoquesting *nods* */
/*                obj_to_char ( create_object
                              ( get_obj_index ( OBJ_VNUM_MAP ), 0 ), ch );
                obj_to_char ( create_object
                              ( get_obj_index ( OBJ_VNUM_WMAP ), 0 ), ch );
                obj_to_char ( create_object
                              ( get_obj_index ( OBJ_VNUM_EMAP ), 0 ), ch ); */
                //char_to_room ( ch, get_room_index ( ROOM_VNUM_SCHOOL ) );
	        char_to_room( ch, get_room_index( hometown_table[ch->hometown].school ) );

 
 /*
 * Small additions to make character creation way easier, by Rhaelar 08'2004
 */
        creations_today++;
 
	do_get ( ch, "all bag" );
	do_wear ( ch, "sword" );
	do_second ( ch, "sword" );
	do_wear ( ch, "all" ); 
	do_eat ( ch, "spellup" );
	do_drink ( ch, "jug" );
	do_eat ( ch, "mre" );
	do_put ( ch, "all bag" );
	do_stance(ch,"");
/*
	if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) )
	{
	   do_note ( ch, "catchup" );
	   do_idea ( ch, "catchup" );
	   do_weddings ( ch, "catchup" );
	   do_jokes ( ch, "catchup" );
	   do_news ( ch, "catchup" );
	   do_changes ( ch, "catchup" );
	}
*/
	ch->hit = ch->max_hit;
	ch->mana = ch->max_mana;
	ch->move = ch->max_move;
	
	if ( !IS_SET ( class_table[ch->class].tier, TIER_01 ) )
	{
	   save_char_obj ( ch );
	   backup_char_obj ( ch );
	   do_announce ( ch, "{Dhas just finished {Grerolling{D.{x" );
	}
	else
	{
	   char buf[MSL];
	   save_char_obj ( ch );
	   //do_announce ( ch, "{Dhas just started!{x" );
	   sprintf ( buf, "{m-{M-{R>{r>{x {DEverybody welcome {R%s{D, our newest player!{x\n\r", ch->name );
	   do_gmessage ( buf );
//           ch->pcdata->firsthost = str_dup (ch->pcdata->socket);
	}

                send_to_char ( "\n\r\n\r\n\r\n\r", ch ); /* Clear screen to make people less confused */
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\n\r\n\r\n\r\n\r", ch );
                send_to_char ( "\x01B[2J", ch );

/*
 * Back to the standard jizzle.
 */

/*	    do_help(ch,"NEWBIE INFO");*/
                send_to_char ( "{RIf{Y you are {Rnew{Y here we suggest reading 'help newbie'{x\n\r", ch );
                send_to_char ( "\n\r", ch );
            }

            else if ( ch->in_room != NULL )
            {
                char_to_room ( ch, ch->in_room );
            }
            else if ( IS_IMMORTAL ( ch ) )
            {
                char_to_room ( ch, get_room_index ( ROOM_VNUM_CHAT ) );
            }
            else
            {
/*                if ( ch->alignment < 0 )
                    char_to_room ( ch, get_room_index ( ROOM_VNUM_TEMPLEB ) );
                else
                    char_to_room ( ch, get_room_index ( ROOM_VNUM_TEMPLE ) ); */
    	            char_to_room( ch, get_room_index( hometown_table[ch->hometown].school ) );
            }

   if ( ch->pcdata->cname == '\0' )
	ch->pcdata->cname = str_dup ( ch->name );

   if ( ( IS_SET ( ch->act2, PLR2_CHALLENGER ) || IS_SET ( ch->act2, PLR2_CHALLENGED )
	|| IS_SET ( ch->act2, PLR2_MASS_ARENA )|| IS_SET ( ch->act2, PLR2_MASS_JOINER ) ) )
   {
    ch->challenged = NULL;
    REMOVE_BIT(ch->act2,PLR2_CHALLENGER);
    ch->challenger = NULL;
    REMOVE_BIT(ch->act2,PLR2_CHALLENGED);
    REMOVE_BIT(ch->act2,PLR2_MASS_ARENA);
    REMOVE_BIT(ch->act2,PLR2_MASS_JOINER);
    arena = FIGHT_OPEN;
   }
/*
   if ( IS_SET ( ch->act, PLR_NORESTORE ) )
	REMOVE_BIT ( ch->act, PLR_NORESTORE );
*/

	if (ch->stance[0]==0)
	{
/*		send_to_char("{RWarning{x: Stock stance removed.  No cheating!",ch);
		wiznet("$n had stock stance. Removed.",ch,NULL,WIZ_LOGINS,WIZ_SITES,get_trust(ch)); */
		ch->stance[0] = -1;
	}

   if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
   {
    send_to_char ( "Oops, you logged in the arena. Let me just move you...\n\r", ch );
    char_from_room ( ch );
    char_to_room ( ch, get_room_index ( ROOM_VNUM_MS ) );
   }
/*
   if ( !IS_IMMORTAL ( ch ) 
   {
     if ( IS_SET ( ch->in_room->room_flags, ROOM_IMP_ONLY ||
		 ch->in_room->room_flags, ROOM_GODS_ONLY ) )
     {
       send_to_char ( "You logged in an IMM room, moving you...\n\r", ch );
       char_from_room ( ch );
       char_to_room ( ch, get_room_index ( ROOM_VNUM_MS ) );
     }
   }
*/
/*
   if ( IS_SET ( ch->act, PLR_WASQUESTING ) )
   {
      ch->pcdata->nextquest = 10;
      REMOVE_BIT ( ch->act, PLR_WASQUESTING );
   }
*/
   if ( IS_SET ( ch->act, PLR_WASQUESTING ) )
      REMOVE_BIT ( ch->act, PLR_WASQUESTING );
   
            //do_look ( ch, "auto" );
            if ( IS_IMMORTAL ( ch ) )
            {
		/*if ( ch->incog_level == 0 && ch->invis_level == 0 ) 
		do_announce ( ch, "{Dis now watching over you..{x\n\r" );*/ 
		wiznet ( "$N has entered Shattered Dreams.", ch, NULL, WIZ_LOGINS,
                         WIZ_SITES, get_trust ( ch ) );
            } 

/*
            else
            {
                do_announce ( ch, "has entered Shattered Dreams." );
                wiznet ( "$N has entered Shattered Dreams.", ch, NULL, WIZ_LOGINS,
                         WIZ_SITES, get_trust ( ch ) );
            }
*/
   else if( ch->level < MAX_LEVEL - 8)
   {
      if (is_clan(ch))
/*		if (is_clead(ch))
			sprintf( buf, "{Da {Rleader{D of{x%s{Dhas entered {WS{Dhattered {WD{Dreams {r[{R%d{r]{x",clan_table[ch->clan].who_name,ch->level);
		else */
			sprintf( buf, "a %s{Dof{b%s{Dhas entered {WS{Dhattered {WD{Dreams {r[{R%d{r]{x",clan_rank_table[ch->clan_rank].title_of_rank[ch->sex],clan_table[ch->clan].who_name,ch->level);
	  else
		sprintf( buf, "{Dhas entered {WS{Dhattered {WD{Dreams {r[{R%d{r]{x",ch->level);
	  do_announce(ch,buf);
	  wiznet ( "$N has entered Shattered Dreams.", ch, NULL, WIZ_LOGINS, WIZ_SITES, get_trust ( ch ) );
   }

                act ( "$n has entered Shattered Dreams.", ch, NULL, NULL,
                      TO_ROOM );

            if ( ch->pet != NULL )
            {
                char_to_room ( ch->pet, ch->in_room );
                act ( "$n has entered Shattered Dreams.", ch->pet, NULL, NULL,
                      TO_ROOM );
            }

//            do_unread ( ch, "" );
	    ++ch->pcdata->timesplayed;
	    ++logins_today;
	    update_lasthost ( ch );
	    send_to_char ( "\n\r", ch );
	    do_board ( ch, "" ); /* Show board status */
            break;
    }
    return;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name2 ( char *name )
{
    int e;
    /*
     * Reserved words.
     
    if ( is_name
         ( name,
           "all auto immortal immortals self someone something the"
           "you demise balance circle loner honor unlinked {" ) )
        return FALSE;

    if ( str_cmp ( capitalize ( name ), "Rhaelar" ) &&
         ( !str_prefix ( "Rhae", name ) || !str_suffix ( "Rhaelar", name ) ) )
        return FALSE;
    */
    /*
     * Cursing
     */
    for ( e = 1; e < MAX_CLAN; e++ )
    {
        if ( !str_prefix ( clan_table[e].name, name ) )
            return FALSE;
    }
    if ( !str_infix ( "immortal", name ) )
        return FALSE;
    if ( !str_infix ( " ", name ) )
        return FALSE;
    if ( !str_infix ( "fuck", name ) )
        return FALSE;
    if ( !str_infix ( "shit", name ) )
        return FALSE;
    if ( !str_infix ( "asshole", name ) )
        return FALSE;
    if ( !str_infix ( "pussy", name ) )
        return FALSE;
    /*
     * Length restrictions.
     */
    if ( strlen ( name ) < 4 )
        return FALSE;
#if defined(MSDOS)
    if ( strlen ( name ) > 8 )
        return FALSE;
#endif
#if defined(macintosh) || defined(unix)
    if ( strlen ( name ) > 12 )
        return FALSE;
#endif
    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
        char *pc;
        bool fIll, adjcaps = FALSE, cleancaps = FALSE;
        int total_caps = 0;
        fIll = TRUE;
        for ( pc = name; *pc != '\0'; pc++ )
        {
            if ( !isalpha ( *pc ) )
                return FALSE;
            if ( isupper ( *pc ) )  /* ugly anti-caps hack */
            {
                if ( adjcaps )
                    cleancaps = TRUE;
                total_caps++;
                adjcaps = TRUE;
            }
            else
                adjcaps = FALSE;
            if ( LOWER ( *pc ) != 'i' && LOWER ( *pc ) != 'l' )
                fIll = FALSE;
        }

        if ( fIll )
            return FALSE;
        if ( cleancaps ||
             ( total_caps > ( strlen ( name ) ) / 2 && strlen ( name ) < 3 ) )
            return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
        MOB_INDEX_DATA *pMobIndex;
        int iHash;
        for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        {
            for ( pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL;
                  pMobIndex = pMobIndex->next )
            {
                if ( is_name ( name, pMobIndex->player_name ) )
                    return FALSE;
            }
        }
    }

    /*
     * Check names of people already playing, yes this is necessary for multiple newbies
     * with the same name (thanks Saro).
     */
    if ( descriptor_list )
    {
        int count = 0;
        DESCRIPTOR_DATA *d, *dnext;
        for ( d = descriptor_list; d != NULL; d = dnext )
        {
            dnext = d->next;
            if ( d->connected != CON_PLAYING && d->character &&
                 d->character->name && d->character->name[0] &&
                 !str_cmp ( d->character->name, name ) )
            {
                count++;
                close_socket ( d );
            }
        }
        if ( count )
        {
            sprintf ( log_buf, "Double Newbie alert (%s)", name );
            wiznet ( log_buf, NULL, NULL, WIZ_LOGINS, 0, 0 );
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect ( DESCRIPTOR_DATA * d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];
    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
        if ( !IS_NPC ( ch ) && ( !fConn || ch->desc == NULL ) &&
             !str_cmp ( d->character->name, ch->name ) )
        {
            if ( fConn == FALSE )
            {
                free_string ( d->character->pcdata->pwd );
                d->character->pcdata->pwd = str_dup ( ch->pcdata->pwd );
            }
            else
            {
                OBJ_DATA *obj;
                free_char ( d->character );
                d->character = ch;
                ch->desc = d;
                ch->timer = 0;

	    if ( is_name ( ch->name, "Areri" ) )
	    d->host = str_dup ( "dialup-2.173.126.82.Dial2.Philadelphia1.Level3.net" );

                if ( ch->tells )
                {
                    sprintf ( buf,
                              "Reconnecting.  You have {R%d{x tells waiting.\n\r",
                              ch->tells );
                    send_to_char ( buf, ch );
                    send_to_char ( "Type 'replay' to see tells.\n\r", ch );
                }
                else
                {
                    send_to_char
                        ( "Reconnecting.  You have no tells waiting.\n\r",
                          ch );
                }
                act ( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
                if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL &&
                     obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
                    --ch->in_room->light;
                sprintf ( log_buf, "%s@%s reconnected.", ch->name, d->host );
                log_string ( log_buf );
                wiznet ( "$N groks the fullness of $S link.", ch, NULL,
                         WIZ_LINKS, 0, 0 );
                d->connected = CON_PLAYING;
		/* Inform the character of a note in progress and the possbility
		 * of continuation!
		 */
		if (ch->pcdata->in_progress)
		    send_to_char ("You have a note in progress. Type 'note write' to continue it.\n\r", ch);
            }
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * Check if already playing.
 */
bool check_playing ( DESCRIPTOR_DATA * d, char *name )
{
    DESCRIPTOR_DATA *dold;
    for ( dold = descriptor_list; dold; dold = dold->next )
    {
        if ( dold != d && dold->character != NULL &&
             dold->connected != CON_GET_NAME &&
             dold->connected != CON_GET_OLD_PASSWORD &&
             !str_cmp ( name,
                        dold->original ? dold->original->name : dold->
                        character->name ) )
        {
            write_to_buffer ( d, "That character is already playing.\n\r",
                              0 );
            write_to_buffer ( d, "Do you wish to connect anyway (Y/N)?", 0 );
            d->connected = CON_BREAK_CONNECT;
            return TRUE;
        }
    }

    return FALSE;
}

void stop_idling ( CHAR_DATA * ch )
{
    if ( ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING
         || ch->was_in_room == NULL ||
         ch->in_room != get_room_index ( ROOM_VNUM_LIMBO ) )
        return;
    ch->timer = 0;
    char_from_room ( ch );
    char_to_room ( ch, ch->was_in_room );
    ch->was_in_room = NULL;
    act ( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

void send_to_char ( const char *txt, CHAR_DATA * ch )
{
    if ( txt != NULL && ch != NULL && ch->desc != NULL )
        write_to_buffer ( ch->desc, txt, strlen ( txt ) );
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char ( const char *txt, CHAR_DATA * ch )
{
    if ( txt == NULL || ch == NULL || ch->desc == NULL )
        return;
    if ( ch->lines == 0 )
    {
        send_to_char ( txt, ch );
        return;
    }

#if defined(macintosh)
    send_to_char ( txt, ch );
#else
/*    ch->desc->showstr_head = alloc_mem ( strlen ( txt ) + 1 );
    strcpy ( ch->desc->showstr_head, txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string ( ch->desc, "" ); */
    if (ch->desc->showstr_head &&
	(strlen(txt)+strlen(ch->desc->showstr_head)+1) < 64000)
    {
      char *temp=alloc_mem(strlen(txt) + strlen(ch->desc->showstr_head) + 1);
      strcpy(temp, ch->desc->showstr_head);
      strcat(temp, txt);
      ch->desc->showstr_point = temp + 
	(ch->desc->showstr_point - ch->desc->showstr_head);
      free_mem(ch->desc->showstr_head, strlen(ch->desc->showstr_head) + 1);
      ch->desc->showstr_head=temp;
    }
    else
    {
      if (ch->desc->showstr_head)
	free_mem(ch->desc->showstr_head, strlen(ch->desc->showstr_head)+1);
      ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
      strcpy(ch->desc->showstr_head,txt);
      ch->desc->showstr_point = ch->desc->showstr_head;
      show_string(ch->desc,"");
    }
#endif
}

/* string pager */
void show_string ( struct descriptor_data *d, char *input )
{
    char buffer[4 * MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;
    one_argument ( input, buf );
    if ( buf[0] != '\0' )
    {
        if ( d->showstr_head )
        {
            free_string ( d->showstr_head );
            d->showstr_head = 0;
        }
        d->showstr_point = 0;
        return;
    }

    if ( d->character )
        show_lines = d->character->lines;
    else
        show_lines = 0;
    for ( scan = buffer;; scan++, d->showstr_point++ )
    {
        if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' ) &&
             ( toggle = -toggle ) < 0 )
            lines++;
        else if ( !*scan || ( show_lines > 0 && lines >= show_lines ) )
        {
            *scan = '\0';
            write_to_buffer ( d, buffer, strlen ( buffer ) );
            for ( chk = d->showstr_point; isspace ( *chk ); chk++ );
            {
                if ( !*chk )
                {
                    if ( d->showstr_head )
                    {
                        free_string ( d->showstr_head );
                        d->showstr_head = 0;
                    }
                    d->showstr_point = 0;
                }
            }
            return;
        }
    }
    return;
}

/* quick sex fixer */
void fix_sex ( CHAR_DATA * ch )
{
    if ( ch->sex < 0 || ch->sex > 2 )
        ch->sex = IS_NPC ( ch ) ? 0 : ch->pcdata->true_sex;
}

/*
 * Extended act with optional suppression - Gregor Stipicic, 2001
 */
void xact_new ( const char *format, CHAR_DATA * ch, const void *arg1,
                const void *arg2, int type, int min_pos, int verbose )
{
    static char *const he_she[] = {
        "it", "he", "she"
    };
    static char *const him_her[] = {
        "it", "him", "her"
    };
    static char *const his_her[] = {
        "its", "his", "her"
    };
    CHAR_DATA *to;
    CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
    OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;
    const char *str;
    char *i;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
//    int collen = -1;
    if ( !format || !*format )
        return;
    if ( !ch || !ch->in_room )
        return;
    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( !vch )
        {
            bug ( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if ( !vch->in_room )
            return;
        to = vch->in_room->people;
    }

    for ( ; to; to = to->next_in_room )
    {
        if ( ( !IS_NPC ( to ) && to->desc == NULL ) ||
             ( IS_NPC ( to ) && !HAS_TRIGGER ( to, TRIG_ACT ) ) ||
             to->position < min_pos )
            continue;
        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
        point = buf;
        str = format;
        while ( *str )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            i = NULL;
            switch ( *str )
            {
                case '$':
                    ++str;
                    i = " <@@@> ";
                    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
                    {
                        bug ( "Act: missing arg2 for code %d.", *str );
                        i = " <@@@> ";
                    }
                    else
                    {
                        switch ( *str )
                        {
                            default:
                                bug ( "Act: bad code %d.", *str );
                                i = " <@@@> ";
                                break;
                            case 't':
                                i = ( char * ) arg1;
                                break;
                            case 'T':
                                i = ( char * ) arg2;
                                break;
                            case 'n':
                                i = PERS ( ch, to );
                                break;
                            case 'N':
                                i = PERS ( vch, to );
                                break;
                            case 'e':
                                i = he_she[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'E':
                                i = he_she[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'm':
                                i = him_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'M':
                                i = him_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 's':
                                i = his_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'S':
                                i = his_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'p':
                                i = can_see_obj ( to,
                                                  obj1 ) ? obj1->
                                    short_descr : "something";
                                break;
                            case 'P':
                                i = can_see_obj ( to,
                                                  obj2 ) ? obj2->
                                    short_descr : "something";
                                break;
                            case 'd':
                                if ( !arg2 || ( ( char * ) arg2 )[0] == '\0' )
                                {
                                    i = "door";
                                }
                                else
                                {
                                    one_argument ( ( char * ) arg2, fname );
                                    i = fname;
                                }
                                break;
                            case 'G':
                                if ( ch->alignment < 0 )
                                {
                                    i = "The god of evil"; // Previously Rhaelar
                                }
                                else
                                {
                                    i = "The god of good"; // Previously Sina
                                }
                                break;
                        }
                    }
                    break;
                default:
                    *point++ = *str++;
                    break;
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
/*        if ( collen )
            buf[collen] = UPPER ( buf[collen] ); */
        buf[0] = UPPER ( buf[0] );
        if ( to->desc != NULL && (to->desc->connected == CON_PLAYING) )
        {
            if ( IS_NPC ( to ) || !IS_SET ( to->pcdata->verbose, verbose ) )
                write_to_buffer ( to->desc, buf, point - buf );
            continue;
        }
    }
    return;
}

char *colour ( char type, CHAR_DATA * ch )
{

    if ( IS_NPC ( ch ) )
        return ( "" );
    switch ( type )
    {
        default:
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case 'x':
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case '0':
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case 'z':
            sprintf ( clcode, BLINK );
            break;
        case 'b':
            sprintf ( clcode, C_BLUE );
            break;
        case '4':
            sprintf ( clcode, C_BLUE );
            break;
        case 'c':
            sprintf ( clcode, C_CYAN );
            break;
        case '6':
            sprintf ( clcode, C_CYAN );
            break;
        case 'g':
            sprintf ( clcode, C_GREEN );
            break;
        case '2':
            sprintf ( clcode, C_GREEN );
            break;
        case 'm':
            sprintf ( clcode, C_MAGENTA );
            break;
        case '5':
            sprintf ( clcode, C_MAGENTA );
            break;
        case 'r':
            sprintf ( clcode, C_RED );
            break;
        case '1':
            sprintf ( clcode, C_RED );
            break;
        case 'w':
            sprintf ( clcode, C_WHITE );
            break;
        case '7':
            sprintf ( clcode, C_WHITE );
            break;
        case 'y':
            sprintf ( clcode, C_YELLOW );
            break;
        case '3':
            sprintf ( clcode, C_YELLOW );
            break;
        case 'B':
            sprintf ( clcode, C_B_BLUE );
            break;
        case '$':
            sprintf ( clcode, C_B_BLUE );
            break;
        case 'C':
            sprintf ( clcode, C_B_CYAN );
            break;
        case '^':
            sprintf ( clcode, C_B_CYAN );
            break;
        case 'G':
            sprintf ( clcode, C_B_GREEN );
            break;
        case '@':
            sprintf ( clcode, C_B_GREEN );
            break;
        case 'M':
            sprintf ( clcode, C_B_MAGENTA );
            break;
        case '%':
            sprintf ( clcode, C_B_MAGENTA );
            break;
        case 'R':
            sprintf ( clcode, C_B_RED );
            break;
        case '!':
            sprintf ( clcode, C_B_RED );
            break;
        case 'W':
            sprintf ( clcode, C_B_WHITE );
            break;
        case '&':
            sprintf ( clcode, C_B_WHITE );
            break;
        case 'Y':
            sprintf ( clcode, C_B_YELLOW );
            break;
        case '#':
            sprintf ( clcode, C_B_YELLOW );
            break;
        case 'D':
            sprintf ( clcode, C_D_GREY );
            break;
        case '8':
            sprintf ( clcode, C_D_GREY );
            break;
        case '*':
            switch ( number_range ( 2, 16 ) )
            {
                case 2:
                    sprintf ( clcode, C_RED );
                    break;
                case 3:
                    sprintf ( clcode, C_GREEN );
                    break;
                case 4:
                    sprintf ( clcode, C_YELLOW );
                    break;
                case 5:
                    sprintf ( clcode, C_BLUE );
                    break;
                case 6:
                    sprintf ( clcode, C_MAGENTA );
                    break;
                case 7:
                    sprintf ( clcode, C_CYAN );
                    break;
                case 8:
                    sprintf ( clcode, C_WHITE );
                    break;
                case 9:
                    sprintf ( clcode, C_D_GREY );
                    break;
                case 10:
                    sprintf ( clcode, C_B_RED );
                    break;
                case 11:
                    sprintf ( clcode, C_B_GREEN );
                    break;
                case 12:
                    sprintf ( clcode, C_B_YELLOW );
                    break;
                case 13:
                    sprintf ( clcode, C_B_BLUE );
                    break;
                case 14:
                    sprintf ( clcode, C_B_MAGENTA );
                    break;
                case 15:
                    sprintf ( clcode, C_B_CYAN );
                    break;
                case 16:
                    sprintf ( clcode, C_B_WHITE );
                    break;
            }
            break;
        case 'A':              /* Auction Channel */
            if ( ch->color_auc )
            {
                sprintf ( clcode, colour_channel ( ch->color_auc, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_GREEN );
            }
            break;
        case 'E':              /* Clan Gossip Channel */
            if ( ch->color_cgo )
            {
                sprintf ( clcode, colour_channel ( ch->color_cgo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'F':              /* Clan Talk Channel */
            if ( ch->color_cla )
            {
                sprintf ( clcode, colour_channel ( ch->color_cla, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_MAGENTA );
            }
            break;
        case 'H':              /* Gossip Channel */
            if ( ch->color_gos )
            {
                sprintf ( clcode, colour_channel ( ch->color_gos, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'J':              /* Grats Channel */
            if ( ch->color_gra )
            {
                sprintf ( clcode, colour_channel ( ch->color_gra, ch ) );
            }
            else
            {
                sprintf ( clcode, C_YELLOW );
            }
            break;
        case 'K':              /* Group Tell Channel */
            if ( ch->color_gte )
            {
                sprintf ( clcode, colour_channel ( ch->color_gte, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'L':              /* Immortal Talk Channel */
            if ( ch->color_imm )
            {
                sprintf ( clcode, colour_channel ( ch->color_imm, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_WHITE );
            }
            break;
        case 'N':              /* Music Channel */
            if ( ch->color_mus )
            {
                sprintf ( clcode, colour_channel ( ch->color_mus, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'P':              /* Question+Answer Channel */
            if ( ch->color_que )
            {
                sprintf ( clcode, colour_channel ( ch->color_que, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_YELLOW );
            }
            break;
        case 'Q':              /* Quote Channel */
            if ( ch->color_quo )
            {
                sprintf ( clcode, colour_channel ( ch->color_quo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_GREEN );
            }
            break;
        case 'S':              /* Say Channel */
            if ( ch->color_say )
            {
                sprintf ( clcode, colour_channel ( ch->color_say, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'T':              /* Shout+Yell Channel */
            if ( ch->color_sho )
            {
                sprintf ( clcode, colour_channel ( ch->color_sho, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'U':              /* Tell+Reply Channel */
            if ( ch->color_tel )
            {
                sprintf ( clcode, colour_channel ( ch->color_tel, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'V':              /* Wiznet Messages */
            if ( ch->color_wiz )
            {
                sprintf ( clcode, colour_channel ( ch->color_wiz, ch ) );
            }
            else
            {
                sprintf ( clcode, C_WHITE );
            }
            break;
        case 'a':              /* Mobile Talk */
            if ( ch->color_mob )
            {
                sprintf ( clcode, colour_channel ( ch->color_mob, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'e':              /* Room Title */
            if ( ch->color_roo )
            {
                sprintf ( clcode, colour_channel ( ch->color_roo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'f':              /* Opponent Condition */
            if ( ch->color_con )
            {
                sprintf ( clcode, colour_channel ( ch->color_con, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'h':              /* Fight Actions */
            if ( ch->color_fig )
            {
                sprintf ( clcode, colour_channel ( ch->color_fig, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'i':              /* Opponents Fight Actions */
            if ( ch->color_opp )
            {
                sprintf ( clcode, colour_channel ( ch->color_opp, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'j':              /* Disarm Messages */
            if ( ch->color_dis )
            {
                sprintf ( clcode, colour_channel ( ch->color_dis, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_YELLOW );
            }
            break;
        case 'k':              /* Witness Messages */
            if ( ch->color_wit )
            {
                sprintf ( clcode, colour_channel ( ch->color_wit, ch ) );
            }
            else
            {
                sprintf ( clcode, colour_clear ( ch ) );
            }
            break;
        case 'l':              /* Quest Gossip */
            if ( ch->color_qgo )
            {
                sprintf ( clcode, colour_channel ( ch->color_qgo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_CYAN );
            }
            break;
        case '{':
            sprintf ( clcode, "%c", '{' );
            break;
    }
    return clcode;
}

/*
 * The colour version of the act( ) function, -Lope
 */
/*void act_new ( const char *format, CHAR_DATA * ch, const void *arg1,
               const void *arg2, int type, int min_pos )
{
    static char *const he_she[] = {
        "it", "he", "she"
    };
    static char *const him_her[] = {
        "it", "him", "her"
    };
    static char *const his_her[] = {
        "its", "his", "her"
    };
    CHAR_DATA *to;
    CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
    OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;
    const char *str;
    char *i;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    if ( !format || !*format )
        return;
    if ( !ch || !ch->in_room )
        return;
    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( !vch )
        {
            bug ( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if ( !vch->in_room )
            return;
        to = vch->in_room->people;
    }

    for ( ; to; to = to->next_in_room )
    {
        if ( ( !IS_NPC ( to ) && to->desc == NULL ) ||
             ( IS_NPC ( to ) && !HAS_TRIGGER ( to, TRIG_ACT ) ) ||
             to->position < min_pos )
            continue;
        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
        point = buf;
        str = format;
        while ( *str )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            i = NULL;
            switch ( *str )
            {
                case '$':
                    ++str;
                    i = " <@@@> ";
                    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
                    {
                        bug ( "Act: missing arg2 for code %d.", *str );
                        i = " <@@@> ";
                    }
                    else
                    {
                        switch ( *str )
                        {
                            default:
                                bug ( "Act: bad code %d.", *str );
                                i = " <@@@> ";
                                break;
                            case 't':
                                i = ( char * ) arg1;
                                break;
                            case 'T':
                                i = ( char * ) arg2;
                                break;
                            case 'n':
                                i = PERS ( ch, to );
                                break;
                            case 'N':
                                i = PERS ( vch, to );
                                break;
                            case 'e':
                                i = he_she[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'E':
                                i = he_she[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'm':
                                i = him_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'M':
                                i = him_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 's':
                                i = his_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'S':
                                i = his_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'p':
                                i = can_see_obj ( to,
                                                  obj1 ) ? obj1->
                                    short_descr : "something";
                                break;
                            case 'P':
                                i = can_see_obj ( to,
                                                  obj2 ) ? obj2->
                                    short_descr : "something";
                                break;
                            case 'd':
                                if ( !arg2 || ( ( char * ) arg2 )[0] == '\0' )
                                {
                                    i = "door";
                                }
                                else
                                {
                                    one_argument ( ( char * ) arg2, fname );
                                    i = fname;
                                }
                                break;
                            case 'G':
                                if ( ch->alignment < 0 )
                                {
                                    i = "The god of evil"; // Previously Rhaelar
                                }
                                else
                                {
                                    i = "The god of good"; // Previously Sina
                                }
                                break;
                        }
                    }
                    break;
                default:
                    *point++ = *str++;
                    break;
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
        buf[0] = UPPER ( buf[0] );
        if ( to->desc != NULL )
            write_to_buffer ( to->desc, buf, point - buf );
    }
    return;
}
*/
void act ( const char *format, CHAR_DATA * ch, const void *arg1,
           const void *arg2, int type )
{
    /* to be compatible with older code */
    xact_new ( format, ch, arg1, arg2, type, POS_RESTING, VERBOSE_STD );
}


void act_new (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
	  int type, int min_pos )
{    
    xact_new(format,ch,arg1,arg2,type,min_pos,VERBOSE_STD);
}

char *colour_clear ( CHAR_DATA * ch )
{

    if ( ch && ch->color )
    {
        if ( ch->color == 1 )
            sprintf ( clcode, R_RED );
        else if ( ch->color == 2 )
            sprintf ( clcode, R_GREEN );
        else if ( ch->color == 3 )
            sprintf ( clcode, R_YELLOW );
        else if ( ch->color == 4 )
            sprintf ( clcode, R_BLUE );
        else if ( ch->color == 5 )
            sprintf ( clcode, R_MAGENTA );
        else if ( ch->color == 6 )
            sprintf ( clcode, R_CYAN );
        else if ( ch->color == 7 )
            sprintf ( clcode, R_WHITE );
        else if ( ch->color == 8 )
            sprintf ( clcode, R_D_GREY );
        else if ( ch->color == 9 )
            sprintf ( clcode, R_B_RED );
        else if ( ch->color == 10 )
            sprintf ( clcode, R_B_GREEN );
        else if ( ch->color == 11 )
            sprintf ( clcode, R_B_YELLOW );
        else if ( ch->color == 12 )
            sprintf ( clcode, R_B_BLUE );
        else if ( ch->color == 13 )
            sprintf ( clcode, R_B_MAGENTA );
        else if ( ch->color == 14 )
            sprintf ( clcode, R_B_CYAN );
        else if ( ch->color == 15 )
            sprintf ( clcode, R_B_WHITE );
        else if ( ch->color == 16 )
            sprintf ( clcode, R_BLACK );
        else
            sprintf ( clcode, CLEAR );
    }
    else
    {
        sprintf ( clcode, CLEAR );
    }
    return clcode;
}

char *colour_channel ( int colornum, CHAR_DATA * ch )
{

    if ( colornum == 1 )
        sprintf ( clcode, C_RED );
    else if ( colornum == 2 )
        sprintf ( clcode, C_GREEN );
    else if ( colornum == 3 )
        sprintf ( clcode, C_YELLOW );
    else if ( colornum == 4 )
        sprintf ( clcode, C_BLUE );
    else if ( colornum == 5 )
        sprintf ( clcode, C_MAGENTA );
    else if ( colornum == 6 )
        sprintf ( clcode, C_CYAN );
    else if ( colornum == 7 )
        sprintf ( clcode, C_WHITE );
    else if ( colornum == 8 )
        sprintf ( clcode, C_D_GREY );
    else if ( colornum == 9 )
        sprintf ( clcode, C_B_RED );
    else if ( colornum == 10 )
        sprintf ( clcode, C_B_GREEN );
    else if ( colornum == 11 )
        sprintf ( clcode, C_B_YELLOW );
    else if ( colornum == 12 )
        sprintf ( clcode, C_B_BLUE );
    else if ( colornum == 13 )
        sprintf ( clcode, C_B_MAGENTA );
    else if ( colornum == 14 )
        sprintf ( clcode, C_B_CYAN );
    else if ( colornum == 15 )
        sprintf ( clcode, C_B_WHITE );
    else if ( colornum == 16 )
        sprintf ( clcode, C_BLACK );
    else
        sprintf ( clcode, colour_clear ( ch ) );
    return clcode;
}

/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday ( struct timeval *tp, void *tzp )
{
    tp->tv_sec = time ( NULL );
    tp->tv_usec = 0;
}
#endif

void sdlogf ( char *fmt, ... )
{
    char buf[2 * MSL];
    va_list args;
    va_start ( args, fmt );
    vsprintf ( buf, fmt, args );
    va_end ( args );
    log_string ( buf );
}

/* source: EOD, by John Booth <???> */
void printf_to_char ( CHAR_DATA * ch, char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;
    va_start ( args, fmt );
    vsnprintf ( buf, MSL, fmt, args );
    va_end ( args );
    send_to_char ( buf, ch );
}

void center_to_char ( char *argument, CHAR_DATA * ch, int columns )
{
    char centered[MAX_INPUT_LENGTH];
    int spaces;
    columns = ( columns < 2 ) ? 80 : columns;
    spaces = ( columns - strlen ( argument ) ) / 2;
    sprintf ( centered, "%*c%s", spaces, ' ', argument );
    send_to_char ( centered, ch );
    return;
}

CH_CMD ( do_font )
{
    int place, size;
    char buf[10];
    size = strlen ( argument );
    /* top border */
    center_to_char ( "{b+{D-", ch, 72 - ( 2 * size ) );
    for ( place = 2; place < size; place++ )
        send_to_char ( "{b+{D-", ch );
    send_to_char ( "{b+\n\r", ch );
    /* middle */
    sprintf ( buf, "{B%c", UPPER ( argument[0] ) );
    center_to_char ( buf, ch, 72 - ( 2 * size ) );
    for ( place = 1; place < size; place++ )
    {
        sprintf ( buf, " %c", UPPER ( argument[place] ) );
        send_to_char ( buf, ch );
    }
    send_to_char ( "{x\n\r", ch );
    /* bottom border */
    center_to_char ( "{b+{D-", ch, 72 - ( 2 * size ) );
    for ( place = 2; place < size; place++ )
        send_to_char ( "{b+{D-", ch );
    send_to_char ( "{b+\n\r", ch );
    return;
}

void bugf ( char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;
    va_start ( args, fmt );
    vsnprintf ( buf, MSL, fmt, args );
    va_end ( args );
    bug ( buf, 0 );
}

/*
void logf (char * fmt, ...)
{
	char buf [2*MSL];
	va_list args;
	va_start (args, fmt);
	vsnprintf (buf, 2*MSL, fmt, args);
	va_end (args);
	
	log_string (buf);
}
*/

#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/*
Palrich -- mdb99284@kestrel.tamucc.edu
telnet://areth.com:4000
*/

/*  Reboot (Copyover) - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
 *  http://pip.dknet.dk/~pip1773
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
CH_CMD ( do_copyove )
{
	send_to_char ( "You have to type the command fully to do a reboot.\n\r", ch );
	return;
}

CH_CMD ( do_reboot )
{
  char wollaarg[MIL];
  argument = one_argument (argument, wollaarg);
  if (str_cmp (wollaarg, "wolla"))
  {
     send_to_char ( "Use the 'autoreboot <minutes till reboot>' command!\n\r", ch );
     return;
  }
    FILE *fp = NULL;
    DESCRIPTOR_DATA *d, *d_next;
    char arg0[10], arg1[10], arg2[10], arg3[10];
    char buf[100];
    //int number; 
/*    bool has_reset = FALSE;
    OBJ_DATA *obj;
    RESET_DATA *pReset; */
    fp = file_open ( REBOOT_FILE, "w" );
    if ( !fp )
    {
        if ( ch )
            send_to_char ( "Reboot file not writeable, aborted.\r\n", ch );
        sprintf ( buf, "Could not write to reboot file: %s",
                  REBOOT_FILE );
        log_string ( buf );
        perror ( "do_reboot:fopen" );
        return;
    }

    /* Consider changing all saved areas here, if you use OLC */
//    do_asave ( NULL, "changed" );   /* - autosave changed areas */
    //save_gquest_data();
//    number = number_range ( 0, MAX_QUOTES );

/*    fpObj = file_open("../data/text/corpses.txt", "w" );
    for ( obj = object_list; obj != NULL; obj = obj->next )
	{   has_reset = FALSE;
		if(obj->carried_by )
			continue;
		if(obj->item_type != ITEM_CORPSE_PC )
			continue;
		for ( pReset = obj->in_room->reset_first; pReset; pReset = pReset->next )
		{   
			if(( (pReset->command == 'O' || pReset->command == 'P') && pReset->arg3 == obj->pIndexData->vnum ) ||
				( pReset->command == 'E' && pReset->arg1 == obj->pIndexData->vnum ) )
			{
				has_reset = TRUE;
				break;
			}
		}
		if(has_reset)
			continue;
		fwrite_obj(NULL, obj, fpObj, 0 );
	}
	fprintf(fpObj, "#END\n");
	file_close(fpObj);
*/
//    sprintf ( buf, "\r\nThe world is being rebuilt." );
    sprintf ( buf, "\r\n[1;30mReboot           [0m: " );

    /* For each playing descriptor, save its state */
    for ( d = descriptor_list; d; d = d_next )
    {
        CHAR_DATA *och = CH ( d );
        d_next = d->next;       /* We delete from the list , so need to save this */
        if ( !d->character || d->connected > CON_PLAYING )  /* drop those logging on */
        {
            write_to_descriptor ( d->descriptor,
                                  "\r\nSorry, the mud is being restarted.\r\n",
                                  0 );
            write_to_descriptor ( d->descriptor,
                                  "If you were rerolling, just relogon and attempt to reroll again..\r\n",
                                  0 );
            write_to_descriptor ( d->descriptor,
                                  "\r\nCome back in a few seconds.\r\n",
                                  0 );
	    close_socket ( d ); /* throw'em out */
        }
        else
        {
            fprintf ( fp, "%d %s %s\n", d->descriptor, och->name, d->host );
//            save_char_obj ( och );
            write_to_descriptor ( d->descriptor, buf, 0 );
        }
    }
    fprintf ( fp, "-1\n" );
    new_fclose ( fp );
    //close_db(); /* Close mySQL database connections */

    /* Close reserve and other always-open files and release other resources */
/*    if ( fpReserve )
        fclose ( fpReserve ); */
    /* exec - descriptors are inherited */
    sprintf ( arg0, "%s", "sdmud" );
    sprintf ( arg1, "%d", port );
 /*   sprintf ( arg2, "%d", wwwport ); */
    sprintf ( arg2, "%s", "state" );
    sprintf ( arg3, "%d", control );
/*    sprintf ( arg5, "%d", wwwcontrol ); */
    execl ( EXE_FILE, arg0, arg1, arg2, arg3, ( char * ) NULL );
    /* Failed - sucessful exec will not return */
    perror ( "do_reboot: execl" );
    if ( ch )
        send_to_char ( "The attempt to re-create the world failed.\r\n", ch );
        send_to_char ( "FAIL\n\r", ch ); 
    /* Here you might want to reopen fpReserve */
/*    if ( !fpReserve )
        fpReserve = fopen ( NULL_FILE, "r" ); */
}

/* Recover from a reboot - load players */
void reboot_recover (  )
{
    DESCRIPTOR_DATA *d;
    FILE *fp = NULL;
    char name[100];
    char host[MSL];
    int desc;
    bool fOld;
    sdlogf ( "Reboot recovery initiated" );
    fp = file_open ( REBOOT_FILE, "r" );
    if ( !fp )                  /* there are some descriptors open which will hang forever then ? */
    {
        perror ( "reboot_recover:fopen" );
        sdlogf ( "reboot file not found. Exitting.\n\r" );
        quit ( 1 );
    }

    unlink ( REBOOT_FILE );   /* In case something crashes - doesn't prevent reading  */
    for ( ;; )
    {
        fscanf ( fp, "%d %s %s\n", &desc, name, host );
        if ( desc == -1 )
            break;
        /* Write something, and check if it goes error-free */
        if ( !write_to_descriptor
             ( desc, "D", 0 ) )
        { 
            close ( desc );     /* nope */
            continue;
        }

        d = new_descriptor (  );
	d->lookup_status = 1;
        d->descriptor = desc;
        d->host = str_dup ( host );
        d->next = descriptor_list;
        descriptor_list = d;
        d->connected = CON_REBOOT_RECOVER;    /* -15, so close_socket frees the char */
        /* Now, find the pfile */
        fOld = load_char_obj ( d, name );
        if ( !fOld )            /* Player file not found?! */
        {
            write_to_descriptor ( desc,
                                  "\n\rSomehow, your character was lost in the reboot. Sorry.\n\rYou should talk to Rhaelar.\n\r",
                                  0 );
            close_socket ( d );
        }
        else                    /* ok! */
        {
            write_to_descriptor ( desc, "one.", 
                                  0 );

            /* Just In Case */
            if ( !d->character->in_room )
                d->character->in_room = get_room_index ( ROOM_VNUM_TEMPLE );
            /* Insert in the char_list */
            d->character->next = char_list;

/* Remove people from arena, and remove (or re-add) safe/violent flags cause the timer isn't saved */
/*
if ( ( IS_SET ( d->character->act2, PLR2_CHALLENGER ) || IS_SET ( d->character->act2, PLR2_CHALLENGED ) ) )
{
   d->character->challenged = NULL;
   REMOVE_BIT(d->character->act2,PLR2_CHALLENGER);
   d->character->challenger = NULL;
   REMOVE_BIT(d->character->act2,PLR2_CHALLENGED);
   if ( IS_SET ( d->character->in_room->room_flags, ROOM_ARENA ) )
   {
    char_from_room ( d->character );
    char_to_room ( d->character, get_room_index ( ROOM_VNUM_MS ) );
   }
   arena = FIGHT_OPEN;
   send_to_char ( "Your arena fight has been cancelled because of the happy moment.\n\r", d->character );
}
*/

/* Update the bank once a day, updating every tick just got people millions of platinum in no time. -Rhaelar 11'2004 */
     if ( chance ( 20 ) )	
	bank_update (  );

if ( IS_SET ( d->character->act, PLR_NOCOLOUR ) )
 d->ansi = FALSE;

if ( IS_SET ( d->character->act2, PLR2_PKVIOLENT ) )
 d->character->pcdata->pkviolenttime = 6;

if ( IS_SET ( d->character->act2, PLR2_PKSAFE ) )
 d->character->pcdata->pksafetime = 3;

     if ( IS_QUESTOR ( d->character ) )
     {
        send_to_char ( "{*Your quest has been cleared.\n\r{x", d->character );
        end_quest ( d->character, 0 );
     }

/* Remove end */


            char_list = d->character;
            char_to_room ( d->character, d->character->in_room );
//            do_look ( d->character, "auto" ); 
            act ( "$n materializes!", d->character, NULL, NULL, TO_ROOM );
            d->connected = CON_PLAYING;

            if ( d->character->pet != NULL )
            {
                char_to_room ( d->character->pet, d->character->in_room );
                act ( "$n materializes!.", d->character->pet, NULL, NULL,
                      TO_ROOM );
            }

	    do_version ( d->character, "" );

        }
     }
    new_fclose ( fp );
//    wiznet ( "Shattered Dreams just rebooted successfully.", 0, NULL, 0, 0, 0 );  
// 	load_reboot_obj();
}

void halt_mud ( int sig )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    struct sigaction default_action;
    int i;
    pid_t forkpid;
    
    wait ( NULL );
    if ( !crashed )
    {
        crashed++;
	if ( debug_mode )
	{
	      int i;
	      for ( i=0; i < MAX_DEBUG; i++ )
              {
                 fprintf ( stderr, "Debug message: %s\n\r", last_debug[i] );
	      }
	}
        
	char buf[MSL];
	sprintf ( buf, "The MUD just crashed. Last command was:\n\r%s", last_command );
	bug_message ( "The MUD", "Rhaelar", "Crash", 30, buf );
	
	fprintf ( stderr, "GAME CRASHED (SIGNAL %d).\rLast command: %s\n\r",
                  sig, last_command );
        // Inform last command typer that he caused the crash

        if ( strlen ( last_command2 ) )
        {
            write_to_descriptor ( last_descriptor,
                                  "\n\rThe last command you typed, '", 0 );
            write_to_descriptor ( last_descriptor, last_command2, 0 );
            write_to_descriptor ( last_descriptor,
                                  "', might have caused a crash.\n\r"
                                  "Tell Rhaelar about this, and avoid using that command.\n\r",
                                  0 );
        }

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = CH ( d );
            if ( !ch )
            {
                close_socket ( d );
                continue;
            }
            if ( IS_NPC ( ch ) )
                continue;
            write_to_descriptor ( d->descriptor,
                                  "\n\rShattered Dreams just found an unhandled exception.\007\n\r", 0 );
	    write_to_descriptor ( d->descriptor,
	    			  "Attempting to avoid total shutdown by executing the\n\rmaintenance reboot before schedule.\n\r", 0 );
	    write_to_descriptor ( d->descriptor, "Sorry for the hassle.\n\r", 0 ); 
        }

        // try to save all characters - save_char_obj has sanity checking
/*        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = CH ( d );
            if ( !ch )
            {
                close_socket ( d );
                continue;
            }
            save_char_obj ( ch );
        } */

        // success - proceed with fork/reboot plan.  Otherwise will go to
        // next section and crash with a full reboot to recover
        if ( ( forkpid = fork (  ) ) > 0 )
        {
            // Parent process reboot and exit 
            waitpid ( forkpid, NULL, WNOHANG | WUNTRACED );
            // this requires you to add an "if (ch)" before the send_to_char
            // statements in do_reboot.
            do_reboot ( NULL, "wolla" );
            exit ( 0 );
        }
        else if ( forkpid < 0 )
        {
            exit ( 1 );
        }
        // Child process proceed to dump
        // Close all files!
        for ( i = 255; i >= 0; i-- )
            close ( i );

        // Dup /dev/null to STD{IN,OUT,ERR}
        open ( "/dev/null", O_RDWR );
        dup ( 0 );
        dup ( 0 );

        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        // I run different scripts depending on my port
        if ( !fork (  ) )
        {
/*	    execl ( MUDCHECK_SCRIPT, ( char * ) NULL );
	    execl ( DEVCHECK_SCRIPT, ( char * ) NULL ); */
            exit ( 0 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 1 )
    {
        crashed++;

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = d->original ? d->original : d->character;
            if ( ch == NULL )
            {
                close_socket ( d );
                continue;
            }
            if ( IS_NPC ( ch ) )
                continue;
            write_to_descriptor ( d->descriptor,
                                  "*** Error stabilizing the mud. Conducting full reboot. **\007\n\r",
                                  0 );
            close_socket ( d );
            continue;
        }
        fprintf ( stderr, "CHARACTERS NOT SAVED.\r" );
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 2 )
    {
        crashed++;
        fprintf ( stderr, "TOTAL GAME CRASH." );
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 3 )
    {
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );     }
}

void load_reboot_obj(void)
{	FILE *fpObj=NULL;
	char letter;
        char *word;
	bool bugged = FALSE;
	
//    fpObj = file_open ( "../data/text/corpses.txt", "r" );

    if ( ( fpObj = file_open ( "../data/text/corpses.txt", "r" ) ) != NULL )
    {
        AttemptJump = true;

                if (setjmp(jump_env) == 1)
                {
                        size_t iLine, iChar;
                        char c;

                        iChar = ftell(fpObj);
                        fseek(fpObj, 0, SEEK_SET);
                        for (iLine = 0; (size_t) ftell(fpObj) < iChar; iLine++)
                        {
                                while ((c = fgetc(fpObj)) != '\n' && c != EOF)
                                        ;
                        }
                        fseek(fpObj, iChar, SEEK_SET);

                        bugf("[*SAVED-CORPSES*] Error in file '../data/text/corpses.txt' at line %d", iLine);
                        wiznet ( "Please report this to Rhaelar, if he's online, tell him to get off his lazy ass and fix it!", NULL, NULL, WIZ_BUGS, 0, 0 );
                        new_fclose(fpObj);
		/* Shit dude the file is bugged :'( */
                        bugged = TRUE;
                }

     if ( !bugged )
     {
	unlink("../data/text/corpses.txt"); //To prevent from char's doubling corpses.
	sdlogf("Loading old items.");
	for(;;)
	{	

        letter = fread_letter( fpObj );
        if ( letter == '*' )
        {	fread_to_eol( fpObj );
            continue;
        }

	    if ( letter != '#' )
        {	bug( "Load_char_obj: # in corpses.txt not found.", 0 );
            break;
        }

//		sdlogf("Freading word.");
        word = fread_word( fpObj );
			 if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( NULL, fpObj );
        else if ( !str_cmp( word, "O"      ) ) fread_obj  ( NULL, fpObj );
        else if ( !str_cmp( word, "END"    ) ) break;
        else
        {	bug( "Load_reboot_obj: bad section.", 0 );
            break;
        }
	}
	new_fclose(fpObj);
	return;
    }
  }
}

void update_lasthost (CHAR_DATA *ch)
{
  int i;
  char *newhost;
  newhost = str_dup (ch->pcdata->socket);
  
 for (i=0; i < MAX_HOST; i++)
 {
    if (((ch->pcdata->lasthost[i]) == 0 ) || ((ch->pcdata->lasthost[i]) == newhost ))
    break;
  }
  if (i <= MAX_HOST) /* open slot */
  {
        ch->pcdata->lasthost[i] = newhost;
  }
  if (i == MAX_HOST -1)
  {
  fwrite_host(ch);
  return;
  }
}

void fwrite_host(CHAR_DATA *ch)
{
  FILE *fp=NULL;
  int i;
  char strsave[MIL];
  {
        sprintf( strsave, "%s%s", HOST_DIR, capitalize( ch->name ) );
        if ((fp = fopen(strsave,"w")) == NULL)
        {
            bug("fwrite_host: fopen",0);
            perror(strsave);
        }

  for (i=0; i < MAX_HOST; i++)
        fprintf(fp,"[%d]\t%s\n", i , ch->pcdata->lasthost[i]);
        new_fclose( fp );
	clear_host( ch );
	save_char_obj (ch);
    }
}

void clear_host(CHAR_DATA *ch)
{
  int i;
  for (i=0;i<MAX_HOST; i++)
  ch->pcdata->lasthost[i] = NULL;
  return;
}
