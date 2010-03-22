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

ANC_DATA *anc_list;
/*
char *const anc_titles[] = {
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    ",
    "Ancients    "
};
*/
/*
 * Local functions.
 */
void change_anclist
args ( ( CHAR_DATA * ch, bool add, int level, char *argument ) );

void save_anclist ( void )
{
    ANC_DATA *panc;
    FILE *fp=NULL;
    bool found = FALSE;

    if ( ( fp = file_open ( ANC_FILE, "w" ) ) == NULL )
    {
         perror ( ANC_FILE );
    }

    for ( panc = anc_list; panc != NULL; panc = panc->next )
    {
        found = TRUE;
        fprintf ( fp, "%s %d\n", panc->name, panc->level );
    }

    new_fclose ( fp );
    if ( !found )
        unlink ( ANC_FILE );
}

void load_anclist ( void )
{
    FILE *fp=NULL;
    ANC_DATA *anc_last;

    strcat ( boot_buf, "He certainly was.." );
    if ( ( fp = file_open ( ANC_FILE, "r" ) ) == NULL )
    {
        strcat ( boot_buf, "....\n\r\n\r                    " );
        return;
    }

    anc_last = NULL;
    strcat ( boot_buf, "....\n\r\n\r                    " );
    for ( ;; )
    {
        ANC_DATA *panc;
        if ( feof ( fp ) )
        {
            new_fclose ( fp );
            return;
        }

        panc = new_anc (  );

        panc->name = str_dup ( fread_word ( fp ) );
        panc->level = fread_number ( fp );
        fread_to_eol ( fp );

        if ( anc_list == NULL )
            anc_list = panc;
        else
            anc_last->next = panc;
        anc_last = panc;
    }
    new_fclose ( fp );
}

CH_CMD ( do_ancients )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char title[MAX_STRING_LENGTH];
    //BUFFER *buffer;
    BUFFER2 *buffer;
    int level;
    ANC_DATA *panc;
    int lngth;
    int amt;
    bool found;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, arg3 );

/*
 * Uncomment the following to use the old method of having
 * a fixed wizlist in the rot.are file.
 */

/*
    do_help(ch,"wizlist");
    return;
*/

    if ( ( arg1[0] != '\0' ) && ( ch->level == MAX_LEVEL ) )
    {
        if ( !str_prefix ( arg1, "add" ) )
        {
            if ( !is_number ( arg2 ) || ( arg3[0] == '\0' ) )
            {
                send_to_char ( "Syntax: ancients add <level> <name>\n\r", ch );
                return;
            }
            level = atoi ( arg2 );
            change_anclist ( ch, TRUE, level, arg3 );
            return;
        }
        if ( !str_prefix ( arg1, "delete" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char ( "Syntax: ancients delete <name>\n\r", ch );
                return;
            }
            change_anclist ( ch, FALSE, 0, arg2 );
            return;
        }
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "       ancients delete <name>\n\r", ch );
        send_to_char ( "       ancients add <level> <name>\n\r", ch );
        return;
    }

    if ( anc_list == NULL )
    {
        send_to_char ( "No ancients listed at this time.\n\r", ch );
        return;
    }
    //buffer = new_buf (  );
    buffer = buffer_new ( 1337 );
    sprintf ( title, "The Ancients of Shattered Dreams{x" );
    sprintf ( buf,
              "{B  ___________________________________________________________________________{x\n\r" );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    sprintf ( buf, "{B /\\_\\%70s\\_\\\n\r", " " );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    lngth = ( 70 - strlen ( title ) ) / 2;
    for ( ; lngth >= 0; lngth-- )
    {
        strcat ( title, " " );
    }
    sprintf ( buf, "{B|/\\\\_\\{D%72s{B\\_\\\n\r", title );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    sprintf ( buf, "{B\\_/_|_|{x%69s{B|_|\n\r", " " );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    for ( level = HEADIMP; level > HERO; level-- )
    {
        found = FALSE;
        amt = 0;
        for ( panc = anc_list; panc != NULL; panc = panc->next )
        {
            if ( panc->level == level )
            {
                amt++;
                found = TRUE;
            }
        }
        if ( !found )
        {
            if ( level == HERO + 1 )
            {
                sprintf ( buf, "{x {B___|_|{x%69s{B|_|\n\r", " " );
                //ddd_buf ( buffer, buf );
		buffer_strcat ( buffer, buf );
            }
            continue;
        }
/*        sprintf ( buf, "{x    {B|_|{B%37s {b[{w%d{b]{x%26s{B|_|{x\n\r",
                  anc_titles[HEADIMP - level], level, " " );
        add_buf ( buffer, buf ); */
/*        sprintf ( buf, "{x    {B|_|{c%25s******************{x%26s{B|_|{x\n\r",
                  " ", " " );
        add_buf ( buffer, buf ); */         lngth = 0;
        for ( panc = anc_list; panc != NULL; panc = panc->next )
        {
            if ( panc->level == level )
            {
                if ( lngth == 0 )
                {
                    if ( amt > 2 )
                    {
                        sprintf ( buf, "{x    {B|_|{x{%s%12s%-17s ",
                                  level >= LEVEL_IMMORTAL ? "R" : "Y", " ",
                                  panc->name );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        lngth = 1;
                    }
                    else if ( amt > 1 )
                    {
                        sprintf ( buf, "{x    {B|_|{x{%s%21s%-17s ",
                                  level >= LEVEL_IMMORTAL ? "R" : "Y", " ",
                                  panc->name );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        lngth = 1;
                    }
                    else
                    {
                        sprintf ( buf, "{x    {B|_|{x{%s%30s%-39s{B|_|{x\n\r",
                                  level >= LEVEL_IMMORTAL ? "R" : "Y", " ",
                                  panc->name );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        lngth = 0;
                    }
                }
                else if ( lngth == 1 )
                {
                    if ( amt > 2 )
                    {
                        sprintf ( buf, "%-17s ", panc->name );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        lngth = 2;
                    }
                    else
                    {
                        sprintf ( buf, "%-30s{B|_|{x\n\r", panc->name );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        lngth = 0;
                    }
                }
                else
                {
                    sprintf ( buf, "%-21s{B|_|{x\n\r", panc->name );
                    //add_buf ( buffer, buf );
		    buffer_strcat ( buffer, buf );
                    lngth = 0;
                    amt -= 3;
                }
            }
        }
        if ( level == HERO + 1 )
        {
            sprintf ( buf, "{x {B___|_|{x%69s{B|_|{x\n\r", " " );
        }
        else
        {
            sprintf ( buf, "{x   {B |_|{x%69s{B|_|{x\n\r", " " );
        }
        //add_buf ( buffer, buf );
	buffer_strcat ( buffer, buf );
    }
    sprintf ( buf, "{B/ \\ {B|_|{x%69s{B|_|{x\n\r", " " );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    sprintf ( buf, "{B|\\//_/{x%70s{B/_/\n\r", " " );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    sprintf ( buf,
              "{B\\{B/_/_______________________________________________________________________/_/{x\n\r" );
    //add_buf ( buffer, buf );
    buffer_strcat ( buffer, buf );
    //page_to_char ( buf_string ( buffer ), ch );
    page_to_char ( buffer_string ( buffer ), ch );
    //free_buf ( buffer );
    buffer_free ( buffer );
    return;
}

void update_anclist ( CHAR_DATA * ch, int level )
{
    ANC_DATA *prev;
    ANC_DATA *curr;

    if ( IS_NPC ( ch ) )
    {
        return;
    }
    prev = NULL;
    for ( curr = anc_list; curr != NULL; prev = curr, curr = curr->next )
    {
        if ( !str_cmp ( ch->name, curr->name ) )
        {
            if ( prev == NULL )
                anc_list = anc_list->next;
            else
                prev->next = curr->next;

            free_anc ( curr );
            save_anclist (  );
        }
    }
    if ( level <= HERO )
    {
        return;
    }
    curr = new_anc (  );
    curr->name = str_dup ( ch->name );
    curr->level = level;
    curr->next = anc_list;
    anc_list = curr;
    save_anclist (  );
    return;
}

void change_anclist ( CHAR_DATA * ch, bool add, int level, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ANC_DATA *prev;
    ANC_DATA *curr;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        if ( !add )
            send_to_char ( "    ancients delete <name>\n\r", ch );
        else
            send_to_char ( "    ancients add <level> <name>\n\r", ch );
        return;
    }
    if ( add )
    {
        if ( ( level <= HERO ) || ( level > MAX_LEVEL ) )
        {
            send_to_char ( "Syntax:\n\r", ch );
            send_to_char ( "    ancients add <level> <name>\n\r", ch );
            return;
        }
    }
    if ( !add )
    {
        prev = NULL;
        for ( curr = anc_list; curr != NULL; prev = curr, curr = curr->next )
        {
            if ( !str_cmp ( capitalize ( arg ), curr->name ) )
            {
                if ( prev == NULL )
                    anc_list = anc_list->next;
                else
                    prev->next = curr->next;

                free_anc ( curr );
                save_anclist (  );
            }
        }
    }
    else
    {
        curr = new_anc (  );
        curr->name = str_dup ( capitalize ( arg ) );
        curr->level = level;
        curr->next = anc_list;
        anc_list = curr;
        save_anclist (  );
    }
    return;
}
