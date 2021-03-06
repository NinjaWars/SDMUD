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
#include "sd.h"

/* random room generation procedure */
ROOM_INDEX_DATA *get_random_room ( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *room;

    for ( ;; )
    {
        room = get_room_index ( number_range ( 0, 65535 ) );
        if ( room != NULL )
            if ( can_see_room ( ch, room ) &&
                 ( !( strstr ( room->area->builders, "Unlinked" ) ) ) &&
                 !room_is_private ( ch, room ) &&
                 !IS_SET ( room->room_flags, ROOM_PRIVATE ) &&
                 !IS_SET ( room->room_flags, ROOM_SOLITARY ) &&
                 !IS_SET ( room->room_flags, ROOM_LOCKED ) &&
                 !IS_SET ( room->room_flags, ROOM_SAFE ) && ( IS_NPC ( ch ) ||
                                                              IS_SET ( ch->
                                                                       act,
                                                                       ACT_AGGRESSIVE )
                                                              ||
                                                              !IS_SET ( room->
                                                                        room_flags,
                                                                        ROOM_LAW ) ) )
                break;
    }

    return room;
}

ROOM_INDEX_DATA  *pull_random_room(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *room;
	int test = 0;
	do
	{
		room = get_room_index( number_range( 0, 81099 ) );
		if ( room != NULL )
		{
			if ( can_see_room(ch,room)
			&& !room_is_private(ch,room)
			&& !IS_SET(room->room_flags, ROOM_LOCKED)
			&& (IS_NPC(ch) || IS_SET(ch->act,ACT_AGGRESSIVE) || !IS_SET(room->room_flags,ROOM_LAW))
			&& !(room->vnum >=29000  && room->vnum <= 29499)
			&& !(room->vnum >= 40000 && room->vnum <= 40499)
			&& !(room->vnum >= 27000 && room->vnum <= 27999)
			&& !(room->vnum >= 13500 && room->vnum <= 13699))
			{
				test = 1;
			}
			else
			{
				test = 0;
			}
		}
	}while (test != 1);
	return room;
}


/* RT Enter portals */
CH_CMD ( do_enter )
{
    ROOM_INDEX_DATA *location;

    if ( ch->fighting != NULL )
        return;

    /* nifty portal stuff */
    if ( argument[0] != '\0' )
    {
        ROOM_INDEX_DATA *old_room;
        OBJ_DATA *portal;
        CHAR_DATA *fch, *fch_next;

        old_room = ch->in_room;

        portal = get_obj_list ( ch, argument, ch->in_room->contents );

        if ( portal == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }

        if ( portal->item_type != ITEM_PORTAL ||
             ( IS_SET ( portal->value[1], EX_CLOSED ) &&
               !IS_TRUSTED ( ch, KNIGHT ) ) )
        {
            send_to_char ( "You can't seem to find a way in.\n\r", ch );
            return;
        }

        if ( !IS_TRUSTED ( ch, KNIGHT ) &&
             !IS_SET ( portal->value[2], GATE_NOCURSE ) &&
             ( IS_AFFECTED ( ch, AFF_CURSE ) ) )
        {
            send_to_char ( "Something prevents you from leaving...\n\r", ch );
            return;
        }

        if ( IS_SET ( portal->value[2], GATE_RANDOM ) ||
             portal->value[3] == -1 )
        {
            location = get_random_room ( ch );
            portal->value[3] = location->vnum;  /* for record keeping :) */
        }
        else if ( IS_SET ( portal->value[2], GATE_BUGGY ) &&
                  ( number_percent (  ) < 5 ) )
            location = get_random_room ( ch );
        else
            location = get_room_index ( portal->value[3] );

        if ( location == NULL || location == old_room ||
             !can_see_room ( ch, location ) ||
             ( room_is_private ( ch, location ) &&
               !IS_TRUSTED ( ch, HEADIMP ) ) )
        {
            act ( "$p doesn't seem to go anywhere.", ch, portal, NULL,
                  TO_CHAR );
            return;
        }

        if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_AGGRESSIVE ) &&
             IS_SET ( location->room_flags, ROOM_LAW ) )
        {
            send_to_char ( "Something prevents you from leaving...\n\r", ch );
            return;
        }

        act ( "$n steps into $p.", ch, portal, NULL, TO_ROOM );

        if ( IS_SET ( portal->value[2], GATE_NORMAL_EXIT ) )
            act ( "You enter $p.", ch, portal, NULL, TO_CHAR );
        else
            act ( "You walk through $p and find yourself somewhere else...",
                  ch, portal, NULL, TO_CHAR );

        char_from_room ( ch );
        char_to_room ( ch, location );

        if ( IS_SET ( portal->value[2], GATE_GOWITH ) ) /* take the gate along */
        {
            obj_from_room ( portal );
            obj_to_room ( portal, location );
        }

        if ( IS_SET ( portal->value[2], GATE_NORMAL_EXIT ) )
            act ( "$n has arrived.", ch, portal, NULL, TO_ROOM );
        else
            act ( "$n has arrived through $p.", ch, portal, NULL, TO_ROOM );

        do_look ( ch, "auto" );

        /* charges */
        if ( portal->value[0] > 0 )
        {
            portal->value[0]--;
            if ( portal->value[0] == 0 )
                portal->value[0] = -1;
        }

        /* protect against circular follows */
        if ( old_room == location )
            return;

        for ( fch = old_room->people; fch != NULL; fch = fch_next )
        {
            fch_next = fch->next_in_room;

            if ( portal == NULL || portal->value[0] == -1 )
                /* no following through dead portals */
                continue;

            if ( fch->master == ch && IS_AFFECTED ( fch, AFF_CHARM ) &&
                 fch->position < POS_STANDING )
                do_stand ( fch, "" );

            if ( fch->master == ch && fch->position == POS_STANDING )
            {

                if ( IS_SET ( ch->in_room->room_flags, ROOM_LAW ) &&
                     ( IS_NPC ( fch ) &&
                       IS_SET ( fch->act, ACT_AGGRESSIVE ) ) )
                {
                    act ( "You can't bring $N into the city.", ch, NULL, fch,
                          TO_CHAR );
                    act ( "You aren't allowed in the city.", fch, NULL, NULL,
                          TO_CHAR );
                    continue;
                }

                act ( "You follow $N.", fch, NULL, ch, TO_CHAR );
                do_enter ( fch, argument );
            }
        }

        if ( portal != NULL && portal->value[0] == -1 )
        {
            act ( "$p fades out of existence.", ch, portal, NULL, TO_CHAR );
            if ( ch->in_room == old_room )
                act ( "$p fades out of existence.", ch, portal, NULL,
                      TO_ROOM );
            else if ( old_room->people != NULL )
            {
                act ( "$p fades out of existence.", old_room->people, portal,
                      NULL, TO_CHAR );
                act ( "$p fades out of existence.", old_room->people, portal,
                      NULL, TO_ROOM );
            }
            extract_obj ( portal );
        }

        /* 
         * If someone is following the char, these triggers get activated
         * for the followers before the char, but it's safer this way...
         */
        if ( IS_NPC ( ch ) && HAS_TRIGGER ( ch, TRIG_ENTRY ) )
            mp_percent_trigger ( ch, NULL, NULL, NULL, TRIG_ENTRY );
        if ( !IS_NPC ( ch ) )
            mp_greet_trigger ( ch );

        return;
    }

    send_to_char ( "Nope, can't do it.\n\r", ch );
    return;
}
