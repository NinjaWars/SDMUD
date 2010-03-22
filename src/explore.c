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

#include <stdio.h>
#include <stdlib.h>
#include "sd.h"

int bitcount ( char ch )
{
    int bit, count = 0;

    for ( bit = 1 << 7; bit > 0; bit >>= 1 )
    {
        if ( ch & bit )
            count++;
    }

    return count;
}

int roomcount ( CHAR_DATA * ch )
{
    int index = 0, count = 0;

    if ( IS_NPC ( ch ) )
        return 0;

    for ( index = 0; index < MAX_EXPLORE; index++ )
    {
        count += bitcount ( ch->pcdata->explored[index] );
    }

    return count;
}

int areacount ( CHAR_DATA * ch )
{
    long index = 0;
    int count = 0;

    if ( IS_NPC ( ch ) )
        return 0;

    if ( ( ch->in_room == NULL ) || ( ch->in_room->area == NULL ) )
        return 0;

    for ( index = ch->in_room->area->min_vnum;
          index <= ch->in_room->area->max_vnum; index++ )
    {
        count += getbit ( ch->pcdata->explored, index );
    }

    return count;
}

int arearooms ( CHAR_DATA * ch )
{
    int count = 0;
    int index = 0;

    if ( IS_NPC ( ch ) )
        return 0;

    if ( ( ch->in_room == NULL ) || ( ch->in_room->area == NULL ) )
        return 0;

    for ( index = ch->in_room->area->min_vnum;
          index <= ch->in_room->area->max_vnum; index++ )
    {
        if ( get_room_index ( index ) != NULL )
            count++;
    }

    return count;
}

void setbit ( char *explored, int index )
{
    explored[index / 8] |= 1 << ( index % 8 );
    return;
}

int getbit ( char *explored, int index )
{
    return ( IS_SET ( explored[index / 8], 1 << ( index % 8 ) ) != 0 );
}

void fwrite_rle ( char *explored, FILE * fp )
{
    int index;
    int bit = 0;
    int count = 0;

    fprintf ( fp, "RoomRLE      %d", bit );

    for ( index = 0; index < ( MAX_EXPLORE * 8 ) - 1; index++ )
    {
        if ( getbit ( explored, index ) == bit )
            count++;
        else
        {
            fprintf ( fp, " %d", count );
            count = 1;
            bit = getbit ( explored, index );
        }
    }
    fprintf ( fp, " %d -1\n", count );
    return;
}

void fread_rle ( char *explored, FILE * fp )
{
    int index;
    int bit = 0;
    int count = 0;
    int pos = 0;

    index = 0;

    bit = fread_number ( fp );

    for ( ;; )
    {
        count = fread_number ( fp );

        if ( count < 0 )
            break;
        if ( count == 0 )
            continue;

        do
        {
            if ( bit == 1 )
                explored[index / 8] |= 1 << ( index % 8 );
            index++;
        }
        while ( index < pos + count );
        pos = index;
        bit = ( bit == 1 ) ? 0 : 1;
    }
    return;
}

CH_CMD ( do_explored )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "{wUsage:\n\r"
                       "{Yexplored world - {WShow total amount of the explored\n\r"
                       "{Yexplored area  - {WShow total amount of the current area explored.{x\n\r",
                       ch );

        return;
    }

    if ( !str_cmp ( argument, "world" ) )
    {
        int rcnt = roomcount ( ch );
        double rooms = top_room, percent = ( double ) rcnt / ( rooms / 100 );

        printf_to_char ( ch,
                         "You have explored {W%d {Wof {W%d rooms "
                         "({W%.2f%% of the mud)\n\r{x", rcnt, top_room,
                         percent );

        return;
    }

    else if ( !str_cmp ( argument, "area" ) )
    {
        int rcnt = areacount ( ch );
        double rooms = ( double ) ( arearooms ( ch ) );
        double percent = ( double ) rcnt / ( rooms / 100 );

        printf_to_char ( ch,
                         "You have explored {W%d of {W%d {wrooms in "
                         "this area. ({W%.2f%%)\n\r{x", rcnt,
                         arearooms ( ch ), percent );

        return;
    }
    else
    {
        do_explored ( ch, "" );
        return;
    }
}
