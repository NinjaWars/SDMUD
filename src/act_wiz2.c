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
#include <errno.h>		
#include <unistd.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include <stddef.h>
#include <sys/stat.h>
#include <dirent.h>

bool is_assigned_area(int vnum);
int days_since_last_file_mod(char *);

int days_since_last_file_mod( char *filename )
{
    int days;
    struct stat buf;
    /* declared in previously included globals.h, no need for extern declaration */
/*    extern time_t current_time; */

    if (!stat(filename, &buf))
    {
	days = (current_time - buf.st_mtime)/86400;
    }
    else
	days = 0;

    return days;
}

CH_CMD ( do_pickup )
{
    char search_name[MIL];
    char arg[MIL];    
    char buf[MSL];
    int vnum = 0, number = 0;
    OBJ_DATA *obj;
    CHAR_DATA *gch;
    CHAR_DATA *tch;
    bool is_aggressive = FALSE;
    bool is_vnum = FALSE;
    bool is_mobs = FALSE;
    
    /*Invalid usage?*/
    if (argument[0] == '\0')
    {
   		do_help (ch, "pickup");
		return;
    }

    argument = one_argument(argument, search_name);
    sprintf(buf, "Argument: %s\n\r", search_name);
    send_to_char(buf, ch);
    
    while (argument[0] != '\0')
    {
        argument = one_argument(argument, arg);
        if (!str_prefix(arg, "aggressive"))
        {
            send_to_char("Picking up aggressively.\n\r", ch);
            is_aggressive = TRUE;
        }
            
        if (!str_prefix(arg, "mobile") || !str_prefix(arg, "mobs") )
        {
            send_to_char("Picking up aggressively (only from mobs and not players).\n\r", ch);
            is_aggressive = TRUE;
            is_mobs = TRUE;
        }
    
        if (!str_prefix(arg, "vnum"))
        {
            send_to_char("Argument is a vnum.\n\r", ch);
            is_vnum = TRUE;
        }
    }
    
    if (is_vnum)
    {    
        if (!is_number(search_name))
        {
            send_to_char("If you search by vnum, the argument must be a number.\n\r", ch);
            return;
        }            
        vnum = atoi(search_name);
    }        

    /*Let's find some objects.*/
    for (obj = object_list; obj != NULL; obj = obj->next)
    {
        //obj_next = obj->next;
        /*Can see object?*/
        if (!can_see_obj (ch, obj))
            continue;
        
        /*Check vnum/name for match.*/
        if (is_vnum)
        {
            if (obj->pIndexData->vnum != vnum)
            continue;
        }            
        else
        {
            if (!is_name (search_name, obj->name))
                continue;
        }
                                
        /*Aggressive pickup style?*/
        if (!is_aggressive)
        {
            if (obj->in_obj != NULL || obj->in_room == NULL || obj->carried_by != NULL ||
            obj->wear_flags == 0)
                    continue;
            if (obj->in_room != NULL && !can_see_room (ch, obj->in_room))
                    continue;
        
            /*Check furniture.*/
            if (obj->in_room != NULL)
            {
                for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
                {
                    if (gch->on == obj)
                    /*Not picking it up, people are on it. (Use aggressive to pick it up)*/
                        continue;
                }
            }

            /*here we CAN check for money to just extract, but won't.*/
            /*if (obj->item_type == ITEM_MONEY)...*/
            obj_from_room (obj);            
            obj_to_char(obj, ch);
            number++;
            continue;            
        }
        else 
        {
            /*Aggressive is trickier.*/
            if (obj->wear_flags == 0)
                continue;
            
            /*Check furniture.*/
            if (obj->in_room != NULL)
            {
                for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
                {
                    /*Get off!*/
                    if (gch->on == obj)
                        {
                            gch->position = POS_STANDING;
                            gch->on = NULL;
                        }
                }
            }
            
            /*If the object is in a container, we have to extract it from the container.*/
            /*Note that this object "should" be picked up by the next IF, or dropped onto
            the floor.*/
            if (obj->in_obj != NULL)
            {                
                if (obj->in_obj->carried_by != NULL)
                {
                    tch = obj->in_obj->carried_by;
                    
                    if (!can_see (ch, tch) || tch->in_room == NULL || tch == ch)
                        continue;

                    /*Mobs only?*/
                    if (is_mobs)
                        if (!IS_NPC(tch)) continue;
                    
                    /*Can't take objects from higher-level immortals.*/
                    if (IS_IMMORTAL(tch) && tch->level >= ch->level)
                        continue;
                }                    
                
                obj_from_obj (obj);
                if (obj->carried_by == NULL && obj->in_room == NULL)
                    obj_to_room (obj, get_room_index(ROOM_VNUM_LIMBO));
            }            
                
            /*Is the object carried by someone?*/            
            if (obj->carried_by != NULL && can_see (ch, obj->carried_by)
                && obj->carried_by->in_room != NULL && obj->carried_by != ch)
            {
                tch = obj->carried_by;

                /*Mobs only?*/
                if (is_mobs)
                   if (!IS_NPC(tch)) continue;

                /*Can't take objects from higher-level immortals.*/
                if (IS_IMMORTAL(tch) && tch->level >= ch->level)
                    continue;
                /*Obj_from_char checks character usage (wielded, etc), yay.*/
                obj_from_char(obj);
                obj_to_char(obj, ch);
                number++;
                continue;
            }                
            
            /*here we CAN check for money to just extract, but won't.*/
            /*if (obj->item_type == ITEM_MONEY)...*/

            /*Item's in a room.*/
            if (obj->in_room)
            {            
                obj_from_room (obj);
                obj_to_char(obj, ch);            
                number++;
                continue;
            }
            /*We should have picked up all valid objects by now.*/
        }/*Ends else (pickup is aggressive)*/
    }/*Ends for loop through objects*/

    if (number == 0)
        send_to_char("No matching objects found.\n\r", ch);
    else
    {
        sprintf(buf, "%d matching objects were found and added to your inventory.\n\r", number);
        send_to_char(buf, ch);
    }
    
    return;
}

/*
 * Freevnums by Leifab of lttm3.com
 */ 

CH_CMD ( do_freevnums )
{
//    BUFFER	*buffer;
    BUFFER2	*buffer;
    char	buf[ MSL ];
    int		lvnum;
    int		uvnum;
    int		count;
    int		i;
    int		lows;
    bool	searching;

    lvnum	= 0;
    uvnum	= 0;
    count	= 0;
    lows	= 0;
    searching	= FALSE;
//    buffer = new_buf();
    buffer = buffer_new ( 1337 );

    send_to_char( "Searching for free vnums between 0 and 150k.\n\r", ch );

    for( i = 0 ; i < 100000 ; i++ )
    {
	if( !( get_room_index( i ) )
	 && !( get_mob_index( i ) )
	 && !( get_obj_index( i ) )
	 && !(is_assigned_area( i ) ) )/* Show blocks that are outside areas */
	{
	    if( !searching )
	    {
		searching	= TRUE;
		lvnum		= i;
		uvnum		= lvnum;
		count++;
	    }
	    else
	    {
		uvnum = i;
		count++;
	    }
	}
	else if( searching )
	{
	    searching 	= FALSE;
	    if( count < 50 )
	    {
		lows++;
	    }


		sprintf( buf, "{x %5d - %5d (%5d vnums)\n\r",
		    lvnum,
		    uvnum,
		    count );
		//add_buf( buffer, buf );
		buffer_strcat ( buffer, buf );

	    count	= 0;
	}
    }

    if( lows )
    {
	sprintf( buf, "%d blocks of less than 50 free vnums were found.\n\r", lows );
	//add_buf( buffer, buf );
	buffer_strcat ( buffer, buf );
    }

//    page_to_char( buf_string(buffer), ch );
    page_to_char( buffer_string(buffer), ch );
    //free_buf( buffer );
    buffer_free ( buffer );

    return;
}

bool is_assigned_area(int vnum)
{
   AREA_DATA *a;

   for ( a = area_first; a; a = a->next)
     {
	if (vnum >= a->min_vnum && vnum <= a->max_vnum)
	  return TRUE;
     }

   return FALSE;
}

CH_CMD ( do_idle )
{

    CHAR_DATA *vch;
    char	buf[MAX_INPUT_LENGTH];
    char	status[MAX_INPUT_LENGTH];

    send_to_char("Players     Idle  Hours  HpL Position  Status Host\n\r-----------------------------------------------------------\n\r",ch);
    for(vch=char_list; vch != NULL; vch = vch->next)
    {
        if (( IS_NPC( vch ) || !can_see( ch, vch ) ) ||  (ch->level < vch->level))
	    continue;
	sprintf (status, "%s",
	vch->pcdata->countdown > 0 ? "Quest" :
	vch->on_quest  ? "Iquest" :
	vch->desc && vch->desc->editor ? olc_ed_name( vch ) :
	"    " );



        sprintf(buf,"%-12s%4d%7d%5.1f %-10s%-8s%-30.30s\n\r",vch->name, vch->timer,
            ( vch->played + (int) (current_time - vch->logon) ) / 3600,
            (float)( vch->played + (int) (current_time - vch->logon) )/(3600*vch->level),
                    position_flags[vch->position].name, status,
                    vch->desc ? vch->desc->host : "No descriptor." );
        send_to_char(buf,ch);
    }
    if (number_percent() == 1 )
       send_to_char( "You have become better at idleness!\n\r", ch );
    send_to_char( "\n\r", ch );
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 * Modified command by Rhaelar 07'2004
 */
CH_CMD ( do_forcesilent )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "Force whom to do what?\n\r", ch );
        return;
    }

    one_argument ( argument, arg2 );

    if ( !str_cmp ( arg2, "delete" ) || !str_prefix ( arg2, "mob" ) )
    {
        send_to_char ( "That will NOT be done.\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg2, "reroll" ) )
    {
        send_to_char ( "That will NOT be done.\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        DESCRIPTOR_DATA *d, *d_next;

        if ( get_trust ( ch ) < MAX_LEVEL - 3 )
        {
            send_to_char ( "Not at your level!\n\r", ch );
            return;
        }

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( d->connected == CON_PLAYING &&
                 get_trust ( d->character ) < get_trust ( ch ) )
            {
/*                act ( buf, ch, NULL, d->character, TO_VICT ); */
                interpret ( d->character, argument );
            }
        }
    }
    else if ( !str_cmp ( arg, "players" ) )
    {
        DESCRIPTOR_DATA *d, *d_next;

        if ( get_trust ( ch ) < MAX_LEVEL - 2 )
        {
            send_to_char ( "Not at your level!\n\r", ch );
            return;
        }

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( d->connected == CON_PLAYING &&
                 get_trust ( d->character ) < get_trust ( ch ) &&
                 d->character->level < LEVEL_ANCIENT )
            {
/*               act ( buf, ch, NULL, d->character, TO_VICT ); */
                interpret ( d->character, argument );
            }
        }
    }
    else if ( !str_cmp ( arg, "gods" ) )
    {
        DESCRIPTOR_DATA *d, *d_next;

        if ( get_trust ( ch ) < MAX_LEVEL - 2 )
        {
            send_to_char ( "Not at your level!\n\r", ch );
            return;
        }

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( d->connected == CON_PLAYING &&
                 get_trust ( d->character ) < get_trust ( ch ) &&
                 d->character->level >= LEVEL_ANCIENT )
            {
/*                act ( buf, ch, NULL, d->character, TO_VICT ); */
                interpret ( d->character, argument );
            }
        }
    }
    else
    {
        CHAR_DATA *victim;

        if ( ( victim = get_char_world ( ch, arg ) ) == NULL ||
             ( victim->level >= ch->level && victim->level == MAX_LEVEL ) )
        {
            send_to_char ( "They aren't here.\n\r", ch );
            return;
        }

        if ( victim == ch )
        {
            send_to_char ( "Aye aye, right away!\n\r", ch );
            return;
        }

        if ( !is_room_owner ( ch, victim->in_room ) &&
             ch->in_room != victim->in_room &&
             room_is_private ( ch, victim->in_room ) &&
             !IS_TRUSTED ( ch, HEADIMP ) )
        {
            send_to_char ( "That character is in a private room.\n\r", ch );
            return;
        }

        if ( ( get_trust ( victim ) >= get_trust ( ch ) &&
               !IS_SET ( ch->act, PLR_KEY ) &&
               ( victim->level != MAX_LEVEL ) ) || ( !IS_NPC ( victim ) &&
                                                     ( IS_SET
                                                       ( victim->act,
                                                         PLR_KEY ) ) &&
                                                     ( ch->level !=
                                                       MAX_LEVEL ) ) )
        {
            send_to_char ( "Do it yourself!\n\r", ch );
            return;
        }

        if ( !IS_NPC ( victim ) && get_trust ( ch ) < MAX_LEVEL - 3 )
        {
            send_to_char ( "Not at your level!\n\r", ch );
            return;
        }

/*        act ( buf, ch, NULL, victim, TO_VICT ); */
        interpret ( victim, argument );
    }

    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_slay )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: [Char] [Type]\n\r", ch );
	send_to_char( "Types: Bodyslam, Skin, Slit, Immolate, Demon, Shatter, Slit, Deheart, Pounce.\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "skin" ) )
    {
    act( "You rip the flesh from $N and send his soul to the fiery depths of hell.", ch, NULL, victim, TO_CHAR );
    act( "Your flesh has been torn from your bones and your bodyless soul now watches your bones incenerate in the fires of hell.", ch, NULL, victim, TO_VICT );
    act( "$n rips the flesh off of $N, releasing his soul into the fiery depths of hell.", ch, NULL, victim, TO_NOTVICT );
	}
/* This one is dedicated to my head slave */

	else if ( !str_cmp( arg2, "xwife" ) && get_trust(ch) == MAX_LEVEL )
	{
	act( "You whip out the rotting leg of Touron's x-wife and beat $N to death with it!", ch, NULL, victim, TO_CHAR );
	act( "$n pulls out the rotting leg of Touron's x-wife and beats you to death with it!", ch, NULL, victim, TO_VICT );
	act( "$n pulls out the rotting leg of Touron's x-wife and beats $N to death with it!", ch, NULL, victim, TO_NOTVICT );
	}

	else if ( !str_cmp( arg2, "deheart" ) )
	{
	act( "You rip through $N's chest and pull out $M beating heart in your hand.", ch, NULL, victim, TO_CHAR );
	act( "You feel a sharp pain as $n rips into your chest and pulls our your beating heart in $M hand.", ch, NULL, victim, TO_VICT );
	act( "Specks of blood hit your face as $n rips through $N's chest pulling out $M's beating heart.", ch, NULL, victim, TO_NOTVICT );
	}

    else if ( !str_cmp( arg2, "immolate" ) )
    {
      act( "Your fireball turns $N into a blazing inferno.",  ch, NULL, victim, TO_CHAR    );
      act( "$n releases a searing fireball in your direction.", ch, NULL, victim, TO_VICT    );
      act( "$n points at $N, who bursts into a flaming inferno.",  ch, NULL, victim, TO_NOTVICT );
    }

    /* Rhaelar-style ! Haiiiiiahhhhhhh! */
    else if ( !str_cmp( arg2, "bodyslam" ) )
    {
      act( "You leap high into the air and slam onto $N, rendering them lifeless.",  ch, NULL, victim, TO_CHAR );
      act( "$n leaps high into the air and smashes down hard on you. You didn't survive that.", ch, NULL, victim, TO_VICT    );
      act( "$n leaps high into the air and smashes onto $N, who turns into a pancake. Anybody got syrup?",  ch, NULL, victim, TO_NOTVICT );
    }
    
    else if ( !str_cmp( arg2, "shatter" ) )
    {
      act( "You freeze $N with a glance and shatter the frozen corpse into tiny shards.",  ch, NULL, victim, TO_CHAR    );
      act( "$n freezes you with a glance and shatters your frozen body into tiny shards.", ch, NULL, victim, TO_VICT    );
      act( "$n freezes $N with a glance and shatters the frozen body into tiny shards.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "demon" ) )
    {
      act( "You gesture, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams in panic before being eaten alive.",  ch, NULL, victim, TO_CHAR );
      act( "$n gestures, and a slavering demon appears.  The foul creature turns on you with a horrible grin.   You scream in panic before being eaten alive.",  ch, NULL, victim, TO_VICT );
      act( "$n gestures, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams in panic before being eaten alive.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "pounce" ) )
    {
      act( "Leaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...",  ch, NULL, victim, TO_CHAR );
      act( "In a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your life ends...", ch, NULL, victim, TO_VICT );
      act( "Leaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N's dying body away.",  ch, NULL, victim, TO_NOTVICT );
    }
 
    else if ( !str_cmp( arg2, "slit" ))
    {
      act( "You calmly slit $N's throat.", ch, NULL, victim, TO_CHAR );
      act( "$n reaches out with a clawed finger and calmly slits your throat.", ch, NULL, victim, TO_VICT );
      act( "A claw extends from $n's hand as $M calmly slits $N's throat.", ch, NULL, victim, TO_NOTVICT );
    }

    else
    {
      act( "You slay $N in cold blood!",  ch, NULL, victim, TO_CHAR    );
      act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
      act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    }

    raw_kill(victim,ch);
    return;
}
/*
CH_CMD ( do_moston )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: moston <amount>.\n\r", ch );
        return;
    }

*/

CH_CMD ( do_itournament )
{
	if ( IS_NPC ( ch ) )
	   return;

        if ( !is_mass_arena && !is_mass_arena_fight )
        {
            do_gmessage
                ( "An Immortal forms a {Wb{dr{Wi{dg{Wh{dt {Bb{bl{Bu{be {Mm{ma{Mg{mi{Mc{x ball inside their hands.{x\n\r" );
            do_gmessage
                ( "The ball levitates slowly in the air before {Ye{yx{Yp{yl{Yo{yd{Yi{yn{Yg {xin a blinding {Wwhite {zflash{x{W!{x{x\n\r" );
            do_gmessage
                ( "{R[{WARENA{R] {xThe tournament is now {GOPEN{x.\n\r{R[{WARENA{R] {xType: {RTOURNAMENT JOIN{x. You have 2 minutes to do so.{x\n\r" );
            mass_arena_ticks = 0;
            mass_arena_players_joined = 0;
            is_mass_arena = TRUE;
	    return;
	}
	send_to_char ( "Something is already happening in there\n\r", ch );
	return;
}

CH_CMD ( do_autoreboot )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( IS_IMMORTAL ( ch ) && !IS_NPC ( ch ) )
    {
        if ( is_reboot_countdown == TRUE )
        {
            send_to_char
                ( "There is already a reboot countdown in progress.\n\rType ABORT to abort.\n\r",
                  ch );
            sprintf ( buf, "Current countdown has %d minute(s) remaining.\n\r",
                      ( reboot_countdown - 1 ) );
            send_to_char ( buf, ch );
            return;
        }
        if ( arg1[0] == '\0' )
        {
            send_to_char
                ( "Reboot in 5 minutes.\n\r( default if no time specified )\n\r\n\rType ABORT to abort.\n\r",
                  ch );
            reboot_countdown = 6;
            is_reboot_countdown = TRUE;
        }
        else
        {
            sprintf ( buf,
                      "\n\rReboot in %d minutes\n\r\n\rType abort to abort.\n\r",
                      atoi ( arg1 ) );
            send_to_char ( buf, ch );
            reboot_countdown = ( atoi ( arg1 ) + 1 );
            is_reboot_countdown = TRUE;
        }
        if ( !str_cmp ( arg2, "silent" ) )
        {
            send_to_char ( "Reboot countdown started silently.\n\r", ch );
        }
        else
        {
            if ( reboot_countdown > 6 )
            {
                sprintf ( buf,
                          "\n\r{R[{Wreboot{R]{C Reboot countdown started, {R%d{C minutes remaining.{x\n\r{R[{WReboot{R] {CPlease see '{chelp reboot{C' for more information.{x\n\r",
                          ( reboot_countdown - 1 ) );
                do_gmessage ( buf );
            }
        }

    }
}

CH_CMD ( do_abort )
{
    if ( IS_IMMORTAL ( ch ) )
    {

        if ( is_reboot_countdown == FALSE )
        {
            send_to_char ( "There is no reboot countdown to abort.\n\r", ch );
            return;
        }
        if ( str_cmp ( argument, "silent" ) )
        {
            do_announce ( ch, "has aborted the reboot.\n\r" );
        }
        else
        {
            send_to_char ( "The reboot was aborted silently.\n\r", ch );
        }
        reboot_countdown = -1;
        is_reboot_countdown = FALSE;
    }

}

/* Scatters everything in the room to random rooms all over your mud */
/* Written by Dither of Planes of Dominion webmaster@podmud.com */
CH_CMD ( do_scatter )
{
 if ( ( ch->in_room->vnum != ROOM_VNUM_SCATTER ) )
 {
	send_to_char ( "You have to be in the scatter room to do this. The vnum for that room is 21416 (goto 21416).\n\r", ch );
	return;
 }
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  ROOM_INDEX_DATA *pRoomIndex;

  for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next ) {
    obj_next = obj->next_content;
    for ( ; ; )
    {
        pRoomIndex = get_room_index( number_range( 0, 41059 ) );
        if ( pRoomIndex != NULL )
        if ( CAN_ACCESS(pRoomIndex) )
            break;
    }
//    act("$p vanishes in a puff of smoke!",ch,obj,NULL,TO_ROOM);
    obj_from_room(obj);
    obj_to_room(obj, pRoomIndex);
  }
  send_to_char ( "Done. Type 'owhere <obj keyword> to see where it/they landed.\n\r", ch );
  return;
}
/*
CH_CMD ( do_rmessage )
{
    if ( IS_NPC ( ch ) )
        return;
    
    if ( argument[0] == '\0' )
    {
	send_to_char ( "Restore message cleared.\n\r", ch );
	free_string ( ch->pcdata->rmessage )
	ch->pcdata->rmessage = NULL;
	return;
    }
    free_string ( ch->pcdata->rmessage );
    ch->pcdata->rmessage = str_dup ( argument );
    send_to_char ( "Ok.\n\r", ch );
}
*/

static OBJ_DATA *rgObjNest[MAX_NEST];
CH_CMD ( do_last )
{
    char arg[MIL];
    char buf[MSL];
    char buf2[MSL];
    CHAR_DATA *victim;
    FILE *fp=NULL;
    bool fOld;
    int i;
    //BUFFER *buffer;
    BUFFER2 *buffer;
    //buffer = new_buf();
    buffer = buffer_new ( 1337 );
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "View the last hosts for whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) != NULL )
    {
    if ( (victim->level > LEVEL_HERO) && (victim->level > ch->level) )
    {
	send_to_char("The gods wouldn't like that.\n\r",ch);
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    sprintf(buf,"%s is on now!\n\r",
	victim->name);
    send_to_char(buf,ch);
if(ch->level >= 213)
{
send_to_char("\n\rLast 25 hosts:\n\r",ch);
  for (i=0; i < MAX_HOST; i++)
  {
    if (victim->pcdata->lasthost[i]!= NULL)
    {
      sprintf(buf2,"{r[{R%2d{r]{x\t{D %-30s{x\n\r",
                  i,
                 victim->pcdata->lasthost[i]);
      //add_buf(buffer, buf2);
      buffer_strcat ( buffer, buf2 );
    }
  }
//  page_to_char( buf_string(buffer), ch );
  page_to_char( buffer_string(buffer), ch );
//  free_buf(buffer);
  buffer_free(buffer);
  return;
 }
}
    
    victim = new_char();
    victim->pcdata = new_pcdata();
    fOld = FALSE;

         sprintf( buf, "%s%s", PLAYER_DIR, capitalize( arg ) );

    if ( ( fp = fopen( buf, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	fOld = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char( victim, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) break;
	    else if ( !str_cmp( word, "O"      ) ) break;
	    else if ( !str_cmp( word, "PET"    ) ) break;
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	new_fclose( fp );
    }
    if ( !fOld )
    {
	send_to_char("No player by that name exists.\n\r",ch);
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    if ( (victim->level > LEVEL_HERO) && (victim->level > ch->level) )
    {
	send_to_char("The gods wouldn't like that.\n\r",ch);
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    sprintf(buf,"%s last logged off on %s",
	victim->name, (char *) ctime(&victim->llogoff));
    send_to_char(buf,ch);
if(ch->level >= 213)
{
send_to_char("\n\rLast 25 hosts:\n\r",ch);
  for (i=0; i < MAX_HOST; i++)
  {
    if (victim->pcdata->lasthost[i]!= NULL)
    {
      sprintf(buf2,"{r[{R%2d{r]{x\t{D %-30s{x\n\r",
                  i,
                 victim->pcdata->lasthost[i]);
      //add_buf(buffer, buf2);
      buffer_strcat ( buffer, buf2 );
    }
  }
  //page_to_char( buf_string(buffer), ch );
  page_to_char( buffer_string(buffer), ch );
//  free_buf(buffer);
  buffer_free ( buffer );
  return;
}
    free_pcdata(victim->pcdata);
    free_char(victim);
    return;
}

CH_CMD ( do_damnoun )
{
    if ( IS_NPC ( ch ) ) return;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;    
    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    strcpy ( arg2, argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  damnoun <object> <damtype>\n\r", ch );
	send_to_char ( "Don't use this command unless you know what you're doing!!!\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "Nothing like that in heaven or earth.\n\r", ch );
        return;
    }
    if ( obj->item_type == ITEM_EXIT )
    {
        send_to_char ( "You cannot modify exit objects.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "Dammit silly! That's not a weapon!\n\r", ch );
	send_to_char ( "I'm telling you, don't use this command unless you know what you're doing!\n\r", ch );
	return;
    }
    if ( attack_lookup ( argument ) == 0 )
    {
       send_to_char ( "That's not a valid damnoun.\n\r", ch );
       return;
    }
    obj->value[3] = attack_lookup ( argument );
    send_to_char ( "Done.\n\r", ch );
    return;
}

CH_CMD ( do_mudset )
{
    if ( IS_NPC ( ch ) ) return;
    
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    long value = 0;
    
    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    strcpy ( arg2, argument );
    
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  mudset maxtoday <value>\n\r", ch );
        send_to_char ( "  mudset loginstoday <value>\n\r", ch );
        send_to_char ( "  mudset maxever <value>\n\r", ch );
        return;
    }

    value = is_number ( arg2 ) ? atol ( arg2 ) : -1;

    if ( !str_prefix ( arg1, "maxtoday" ) )
    {
        max_on = value;
	send_to_char ( "Done.\n\r", ch );
        return;
    }
    if ( !str_prefix ( arg1, "loginstoday" ) )
    {
        logins_today = value;
	send_to_char ( "Done.\n\r", ch );
        return;
    }
    if ( !str_prefix ( arg1, "maxever" ) )
    {
        max_ever = value;
	send_to_char ( "Done.\n\r", ch );
        return;
    }
    send_to_char ( "Invalid syntax.\n\r", ch );
    return;
}

CH_CMD ( do_helper )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: helper <character>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ( victim->level >= ch->level ) && ( victim != ch ) )
    {
        send_to_char ( "You don't really have the power to decide such a thing.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_HELPER ) )
    {
        REMOVE_BIT ( victim->act2, PLR2_HELPER );
        send_to_char ( "They are no longer a mortal helper.\n\r", ch );
	send_to_char ( "You are no longer a mortal helper.\n\r", victim );
	return;
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_HELPER );
	send_to_char ( "They are now a mortal helper.\n\r", ch );
        send_to_char ( "You are now a mortal helper!\n\r", victim );
	return;
    }
    return;
}

CH_CMD ( do_inactive )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: inactive <character>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }
    
    if ( !IS_IMMORTAL ( victim ) )
    {
        send_to_char ( "You're trying to mark a mortal as inactive?\n\rSorry stupid, no can be done.\n\r", ch );
        return;
    }

    if ( ( victim->level >= ch->level ) && ( victim != ch ) )
    {
        send_to_char ( "You don't really have the power to decide such a thing.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_INACTIVE ) )
    {
        REMOVE_BIT ( victim->act2, PLR2_INACTIVE );
        send_to_char ( "They are no longer marked as an inactive immortal.\n\r", ch );
	return;
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_INACTIVE );
        send_to_char ( "They have now been marked as an inactive immortal.\n\r", ch );
	return;
    }
    return;
}

/*CH_CMD ( do_multiplay )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: multiplay <character>.\n\r", ch );
	send_to_char ( "Only set this for ONE of the characters allowed to play!\n\r", ch );
	send_to_char ( "Example:\n\r Anna and John are on from the same IP, they want to be able to play at the same time cause they are brother and sister.\n\rOnly allowed John to multiplay, and both of them will be able to log on.\n\rIf you mark both as allowed, they will be able to have three characters on at once.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }
    
    if ( ( victim->level >= ch->level ) && ( victim != ch ) )
    {
        send_to_char ( "You don't really have the power to decide such a thing.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR2_MULTIPLAY ) )
    {
        REMOVE_BIT ( victim->act2, PLR2_MULTIPLAY );
        send_to_char ( "They are no longer allowed to have more than one character on from the same address.\n\r", ch );
	return;
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_MULTIPLAY );
        send_to_char ( "They are now allowed to have one more character on from the same address.\n\rRemember to post a note about this on the permissions board.\n\r", ch );
	return;
    }
    return;
}*/

CH_CMD ( do_toggle )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
            send_to_char ( "{WToggle-stuff:\n\r\n\r  {DChristmas\n\r  Stuffchanged\n\r  Debugmode{x\n\r", ch );
            return;
    }

    if ( !str_cmp ( arg, "christmas" ) )
    {
        if ( its_christmas )
        {
           its_christmas = FALSE;
	   send_to_char ( "Christmas mode OFF\n\r", ch );
	   return;
	}
	else
	{
	   its_christmas = TRUE;
	   send_to_char ( "Christmas mode ON! Merry christmas!\n\r", ch );
	   return;
	}
    }
    if ( !str_cmp ( arg, "special_day" ) )
    {
        if ( special_day )
        {
           special_day = FALSE;
	   send_to_char ( "Ok, no longer a special day :(\n\r", ch );
	   return;
	}
	else
	{
	   special_day = TRUE;
	   send_to_char ( "Woo! Happy special day!\n\r", ch );
	   return;
	}
    }
	if ( !str_cmp ( arg, "stuffchanged" ) )
    {
        if ( stuff_was_changed )
        {
           stuff_was_changed = FALSE;
	   send_to_char ( "Nothing was changed, no autoreboot in the morning.\n\r", ch );
	   return;
	}
	else
	{
	   stuff_was_changed = TRUE;
	   send_to_char ( "Woo! Changes! Will reboot in the morning, sir!\n\r", ch );
	   return;
	}
    }
    if ( !str_cmp ( arg, "debugmode" ) )
    {
        if ( debug_mode )
        {
           debug_mode = FALSE;
	   send_to_char ( "Debugmode OFF\n\r", ch );
	   return;
	}
	else
	{
	   debug_mode = TRUE;
	   send_to_char ( "Debugmode ON\n\r", ch );
	   return;
	}
    }
    return;
}

CH_CMD ( do_ps )
{
   
   if ( argument[0] == '\0' )
   {
      send_to_char ( "ps what?\n\r", ch );
      return;
   }
/*   if ( !str_cmp ( arg, "ux" ) )
   { */
        FILE *fp=NULL;
        char proc[MSL];
	char line[80];
   	char buf[MAX_STRING_LENGTH];
   	BUFFER2 *output;
   	output = buffer_new( 1337 );

        sprintf( proc, "ps %s > ../data/text/ps.txt", argument );
        system( proc );

        /* open file */
	    fp = fopen( "../data/text/ps.txt","r" );

        while( fgets( line, 80, fp ) != NULL )
		{
			sprintf(buf, "%s", line);
	    		buffer_strcat(output,buf);
		}

		new_fclose(fp);
		page_to_char( buffer_string(output), ch );
		buffer_free(output);
		return;
    //}
/*    if ( !str_cmp ( arg, "ax" ) )
    {
        FILE *fp=NULL;
        char proc[24];
	char line[80];
   	char buf[MAX_STRING_LENGTH];
   	BUFFER2 *output;
   	output = buffer_new( 1337 );

        sprintf( proc, "ps ax > ../data/text/ps.txt" );
        system( proc );

  	    fp = fopen( "../data/text/ps.txt","r" );

        while( fgets( line, 80, fp ) != NULL )
		{
			sprintf(buf, "%s", line);
	    		buffer_strcat(output,buf);
		}

		new_fclose(fp);
		page_to_char( buffer_string(output), ch );
		buffer_free(output);
		return;
    }  */
    return;
}
