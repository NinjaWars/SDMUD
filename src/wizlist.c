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

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sd.h"
#include "recycle.h"

WIZ_DATA *wiz_list;

/*char *const wiz_titles[] = {
    "Owners      ",
    "Enforcers   ",
    "Creators    ",
    "Supremacies ",
    "Deities     ",
    "Gods        ",
    "Divinities  ",
    "Myths       ",
    "Angels      ",
    "Legends     ",
    "Hi Questors ",
    "Questors    ",
    "Lo Questors ",
    "Inactive    "
};*/

/*
 * Local functions.
 */
void change_wizlist
args ( ( CHAR_DATA * ch, bool add, int level, char *argument, char *job ) );

void save_wizlist ( void )
{
    WIZ_DATA *pwiz;
    FILE *fp=NULL;
    bool found = FALSE;

    if ( ( fp = file_open ( WIZ_FILE, "w" ) ) == NULL )
    {
        perror ( WIZ_FILE );
    }

    for ( pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next )
    {
        found = TRUE;
        fprintf ( fp, "%s %d %s~\n", pwiz->name, pwiz->level, pwiz->job );
    }

    new_fclose ( fp );
    if ( !found )
        unlink ( WIZ_FILE );
}

void load_wizlist ( void )
{
    FILE *fp=NULL;
    WIZ_DATA *wiz_last;

    if ( ( fp = file_open ( WIZ_FILE, "r" ) ) == NULL )
    {
        return;
    }

    wiz_last = NULL;
    for ( ;; )
    {
        WIZ_DATA *pwiz;
        if ( feof ( fp ) )
        {
            new_fclose ( fp );
            return;
        }

        pwiz = new_wiz (  );

        pwiz->name = str_dup ( fread_word ( fp ) );
        pwiz->level = fread_number ( fp );
	pwiz->job = str_dup ( fread_string ( fp ) );
        fread_to_eol ( fp );

        if ( wiz_list == NULL )
            wiz_list = pwiz;
        else
            wiz_last->next = pwiz;
        wiz_last = pwiz;
    }
    new_fclose ( fp );
}

CH_CMD ( do_wizlist )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BUFFER2 *buffer;
    WIZ_DATA *pwiz;
    int level;
    
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, arg3 );
    argument = one_argument ( argument, arg4 );

    if ( ( arg1[0] != '\0' ) && ( ch->level == MAX_LEVEL ) )
    {
        if ( !str_prefix ( arg1, "add" ) )
        {
            if ( !is_number ( arg2 ) || ( arg3[0] == '\0' ) || ( arg4[0] == '\0' ) )
            {
                send_to_char ( "Syntax: wizlist add <level> <name> <job description>\n\r", ch );
                return;
            }
            level = atoi ( arg2 );
            change_wizlist ( ch, TRUE, level, arg3, arg4 );
            return;
        }
        if ( !str_prefix ( arg1, "delete" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char ( "Syntax: wizlist delete <name>\n\r", ch );
                return;
            }
            change_wizlist ( ch, FALSE, 0, arg2, "Fired." );
            return;
        }
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "       wizlist delete <name>\n\r", ch );
        send_to_char ( "       wizlist add <level> <name>\n\r", ch );
        return;
    }

    if ( wiz_list == NULL )
    {
        send_to_char ( "No immortals listed at this time.\n\r", ch );
        return;
    }
    
    buffer = buffer_new ( 1337 );
    buffer_strcat ( buffer, "   Below is a list of the staff of Shattered Dreams.\n\r"
                            "   They are to be treated with utmost respect at all\n\r"
                            "   times, and their statements may not be questioned.\n\r\n\r" );
    buffer_strcat ( buffer, "      {rName         Lvl     Job\n\r" );

    for ( level = HEADIMP; level > HERO; level-- )
    {
        for ( pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next )
        {
            if ( pwiz->level == level )
            {
	       sprintf ( buf, "      {x%-12s {D%d     {c%s{x\n\r", pwiz->name, pwiz->level, pwiz->job );
	       buffer_strcat ( buffer, buf );            
	    }
        }
    }
    page_to_char ( buffer_string ( buffer ), ch );
    buffer_free ( buffer );
    return;
}

void update_wizlist ( CHAR_DATA * ch, int level )
{
    WIZ_DATA *prev;
    WIZ_DATA *curr;

    if ( IS_NPC ( ch ) )
    {
        return;
    }
    prev = NULL;
    for ( curr = wiz_list; curr != NULL; prev = curr, curr = curr->next )
    {
        if ( !str_cmp ( ch->name, curr->name ) )
        {
            if ( prev == NULL )
                wiz_list = wiz_list->next;
            else
                prev->next = curr->next;

            free_wiz ( curr );
            save_wizlist (  );
        }
    }
    if ( level <= HERO )
    {
        return;
    }
    curr = new_wiz (  );
    curr->name = str_dup ( ch->name );
    curr->job = str_dup ( "Not yet assigned." );
    curr->level = level;
    curr->next = wiz_list;
    wiz_list = curr;
    save_wizlist (  );
    return;
}

void change_wizlist ( CHAR_DATA * ch, bool add, int level, char *argument, char *job )
{
    char arg1[MAX_INPUT_LENGTH];
//    char arg2[MIL];
    WIZ_DATA *prev;
    WIZ_DATA *curr;

    one_argument ( argument, arg1 );
    //one_argument ( job, arg2 );
    
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        if ( !add )
            send_to_char ( "    wizlist delete <name>\n\r", ch );
        else
            send_to_char ( "    wizlist add <level> <name> <job description>\n\r", ch );
        return;
    }
    if ( add )
    {
        if ( ( level <= HERO ) || ( level > MAX_LEVEL ) )
        {
            send_to_char ( "Syntax:\n\r", ch );
            send_to_char ( "    wizlist add <level> <name> <job description>\n\r", ch );
            return;
        }
    }
    if ( !add )
    {
        prev = NULL;
        for ( curr = wiz_list; curr != NULL; prev = curr, curr = curr->next )
        {
            if ( !str_cmp ( capitalize ( arg1 ), curr->name ) )
            {
                if ( prev == NULL )
                    wiz_list = wiz_list->next;
                else
                    prev->next = curr->next;

                free_wiz ( curr );
                save_wizlist (  );
            }
        }
    }
    else
    {
        curr = new_wiz (  );
        curr->name = str_dup ( capitalize ( arg1 ) );
	curr->job = str_dup ( capitalize ( job ) );
	curr->level = level;
        curr->next = wiz_list;
        wiz_list = curr;
        save_wizlist (  );
    }
    return;
}
