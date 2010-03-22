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
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "sd.h"

void do_start_massarena ( void )
{
    int char_room;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;

    if ( !is_mass_arena_fight )
    {
        do_gmessage
            ( "{R[{WArena{R] {xThe tournament has started! Let the games begin!{x\n\r" );
	do_tclear ( NULL, NULL );
        is_mass_arena_fight = TRUE;
        sprintf ( lastwinner, " " );        
	for ( ch = char_list; ch != NULL; ch = ch_next )
        {
            ch_next = ch->next;
            if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
                 IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
            {
                char_room = number_range ( 50000, 50013 );
                char_from_room ( ch );
                char_to_room ( ch, get_room_index ( char_room ) );
/*                do_visible ( ch, NULL );

                affect_strip ( ch, gsn_plague );
                affect_strip ( ch, gsn_poison );
                affect_strip ( ch, gsn_blindness );
                affect_strip ( ch, gsn_sleep );
                affect_strip ( ch, gsn_curse ); */

                ch->hit = ch->max_hit;
                ch->mana = ch->max_mana;
                ch->move = ch->max_move;
                update_pos ( ch );
                do_look ( ch, "auto" );
            }
	    else if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
		      !IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
	    {
		send_to_char ( "You didn't join the tournament even though you voted for it. Your flag has been removed.\n\r", ch );
		REMOVE_BIT ( ch->act2, PLR2_MASS_ARENA );
	    }
        }
    }
}

CH_CMD ( do_tournament )
{
    if ( IS_NPC ( ch ) || ch->desc == NULL )
        return;

    if ( reboot_countdown > 0 && reboot_countdown <= 5 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a reboot.\n\rPlease try again after the reboot.\n\r",
              ch );
        return;
    }

    if ( !str_cmp ( argument, "start" ) )
    {
	if ( !IS_IMMORTAL ( ch ) )
	    return;
	do_itournament ( ch, NULL );
	return;
    }

    if ( !str_cmp ( argument, "stop" ) )
    {
	if ( !IS_IMMORTAL ( ch ) )
	    return;
	do_gmessage ( "The {RTOURNAMENT{x has been {RSTOPPED{x.\n\r" );
	do_aclear ( ch, NULL );
	return;
    }
/*
    if ( IS_IMMORTAL ( ch ) )
    {
	send_to_char ( "Log on your mortal if you want to participate!\n\r", ch);
	return;
    }
*/
    if ( ch->level < LEVEL_HERO )
    {
	send_to_char ( "You have to be a hero or an ancient to participate in tournaments.\n\r", ch);
	return;
    }

    if ( IS_SET ( ch->act2, PLR2_CHALLENGER || IS_SET ( ch->act2, PLR2_CHALLENGED ) ) )
    {
	send_to_char ( "Don't you think you should finish your current arena business first?\n\r", ch );
	return;
    }

    if (IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
    {
      send_to_char ("Don't you think you should finish your freeze tag business first?.\n\r", ch);
      return;
    }

    if ( arena == FIGHT_START )
    {
        send_to_char
            ( "Sorry, some one else has already started a challenge, please try later.\n\r",
              ch );
        return;
    }
/*
    if ( arena == FIGHT_BUSY )
    {
        send_to_char
            ( "Sorry, there is a fight in progress, please wait a few moments.\n\r",
              ch );
        return;
    }
*/
    if ( arena == FIGHT_LOCK )
    {
        send_to_char ( "Sorry, the arena is currently locked from use.\n\r",
                       ch );
        return;
    }
    if ( IS_SET ( ch->act2, PLR2_NOARENA ) )
    {
        send_to_char
            ( "You are not allowed to fight in the arena.\n\r",
              ch );
        return;
    }
    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "You are already in the arena.\n\r", ch );
        return;
    }
    if ( is_mass_arena_fight )
    {
        send_to_char ( "Sorry, its too late to join now.\n\r", ch );
        return;
    }

    if ( !str_cmp ( argument, "join" ) )
    {
        char buf[MSL];

        if ( IS_NPC ( ch ) )
            return;

        if ( !is_mass_arena )
        {
            send_to_char
                ( "There is not a tournament to join at this time.\n\r", ch );
            return;
        }
        if ( is_mass_arena_fight )
        {
            send_to_char ( "It is too late to join now.\n\r", ch );
            return;
        }
        if ( !IS_SET ( ch->act2, PLR2_MASS_ARENA ) )
        {
/*
            send_to_char
                ( "You must first turn on your tournament flag.\n\rPlease type tournament to do so.\n\r",
                  ch );
            return;*/
	   SET_BIT ( ch->act2, PLR2_MASS_ARENA );
/*	   send_to_char ( "If a tournament join process has already been started, you'll want to type 'tournament join' instead of just 'tournament', otherwise you won't participate.\n\r", ch ); */
        }
        if ( IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
        {
            send_to_char ( "You have already joined the tournament.\n\r", ch );
            return;
        }
        if ( !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && is_mass_arena &&
             !is_mass_arena_fight )
        {
	    if ( IS_IMMORTAL ( ch ) )
	    {
	       send_to_char ( "Sorry, there's a bug with immortals+tournament, until its\n\rbeen fixed you can't participate in tournaments.", ch );
	       return;
	    }
            sprintf ( buf,
                     "{R[{WArena{R] {b%s{x has joined the tournament!{x\n\r",
                      ch->name );
            do_gmessage ( buf );
            SET_BIT ( ch->act2, PLR2_MASS_JOINER );
            return;
        }
    }
    if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
         !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && !is_mass_arena_fight )
    {
        send_to_char ( "Your tournament starting vote flag is now {ROFF{x.\n\r", ch );
        REMOVE_BIT ( ch->act2, PLR2_MASS_ARENA );
        return;
    }
    if ( !IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
         !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && !is_mass_arena_fight )
    {
        send_to_char ( "Your tournament starting vote flag is now {GON{x.\n\r", ch );
        SET_BIT ( ch->act2, PLR2_MASS_ARENA );
        return;
    }
}


/*=======================================================================*
 * function: do_challenge                                                *
 * purpose: sends initial arena match query                              *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
CH_CMD ( do_challenge )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
/* == First make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;

    if ( reboot_countdown > 0 && reboot_countdown <= 5 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a reboot.\n\rPlease try again after the reboot.\n\r",
              ch );
        return;
    }

    if ( is_mass_arena )
    {
        send_to_char ( "There is currently a tournament taking place.\n\r",
                       ch );
        return;
    }
    if ( arena == FIGHT_START )
    {
        send_to_char
            ( "Sorry, some one else has already started a challenge, please try later.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_BUSY )
    {
        send_to_char
            ( "Sorry, there is a fight in progress, please wait a few moments.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_LOCK )
    {
        send_to_char ( "Sorry, the arena is currently locked from use.\n\r",
                       ch );
        return;
    }
    if (IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
    {
      send_to_char ("You can't challenge someone while you are playing freeze tag.\n\r", ch);
      return;
    }
    if ( IS_SET ( ch->act2, PLR2_NOARENA ) )
    {
        send_to_char
            ( "You are not allowed to fight in the arena.\n\r",
              ch );
        return;
    }
    if ( IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char
            ( "You have already been challenged, either ACCEPT or DECLINE first.\n\r",
              ch );
        return;
    }

    if ( ch->hit < ( ch->max_hit - 500 ) )
    {
        send_to_char ( "You must have maximum 500 less than your max hp to start a challenge.\n\r",
                       ch );
        return;
    }

    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
        send_to_char ( "But you are {Rviolent{x!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "You must specify whom you wish to challenge.\n\r",
                       ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They are not playing.\n\r", ch );
        return;
    }

 if( victim == ch )
  {
  send_to_char("You can't challenge yourself.\n\r",ch);
  return;
  }

    if ( IS_SET ( victim->act2, PLR2_CHALLENGER ) )
    {
        send_to_char ( "They have already challenged someone else.\n\r", ch );
        return;
    }
    if ( IS_SET ( victim->act2, PLR2_NOARENA ) )
    {
        send_to_char
            ( "They are not allowed to fight in the arena.\n\r",
              ch );
        return;
    }
    if ( victim->fighting != NULL )
    {
        send_to_char ( "That person is engaged in battle right now.\n\r",
                       ch );
        return;
    }

    if ( victim->hit < ( victim->max_hit - 500 ) )
    {
        send_to_char
            ( "That player is not healthy enough to fight right now.\n\r",
              ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
    {
        send_to_char ( "They are {Rviolent{x. You cannot fight them right now.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_AFK ) )
    {
        send_to_char ( "They are {GAFK{x. Wait till they return.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL )
    {
        send_to_char
            ( "That player is linkdead at the moment, try them later.\n\r",
              ch );
        return;
    }
/* == Now for the challenge == */
    ch->challenged = victim;
    SET_BIT ( ch->act2, PLR2_CHALLENGER );
    victim->challenger = ch;
    SET_BIT ( victim->act2, PLR2_CHALLENGED );
    arena = FIGHT_START;
    send_to_char ( "Challenge has been sent\n\r", ch );
    act ( "$n has challenged you to a death match.", ch, NULL, victim,
          TO_VICT );
    sprintf ( buf,
              "{R[{WArena{R]{b %s {xhas challenged {b%s{x to a duel.\n\r",
              ch->name, victim->name );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf, d->character );
        }
    }
    sprintf ( buf, "type: ACCEPT %s to meet the challenge.\n\r", ch->name );
    send_to_char ( buf, victim );
    sprintf ( buf, "type: DECLINE %s to chicken out.\n\r", ch->name );
    send_to_char ( buf, victim );
    arena_countdown = 3;
    return;
}

/*=======================================================================*
 * function: do_accept                                                   *
 * purpose: to accept the arena match, and move the players to the arena *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
CH_CMD ( do_agree )
{
    float odd1, odd2;
    float lvl1, lvl2;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    int char_room;
    int vict_room;
 /*== the room VNUM's for our arena.are ==*/
    /* we use 1051 thru 1066 for a 4x4 arena */
    char_room = number_range ( 50000, 50013 );
    vict_room = number_range ( 50000, 50013 );
/* == first make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;
    if ( !IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You have not been challenged.\n\r", ch );
        return;
    }
    if ( arena == FIGHT_BUSY )
    {
        send_to_char
            ( "Sorry, there is a fight in progress, please wait a few moments.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_LOCK )
    {
        send_to_char ( "Sorry, the arena is currently locked from use.\n\r",
                       ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char
            ( "You must specify whose challenge you wish to accept.\n\r",
              ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't logged in!\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char ( "You haven't challenged yourself!\n\r", ch );
        return;
    }
    if ( !IS_SET ( victim->act2, PLR2_CHALLENGER ) ||
         victim != ch->challenger )
    {
        send_to_char ( "That player hasn't challenged you!\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
        send_to_char ( "But you are {Rviolent{x!\n\r", ch );
        return;
    }    
/* == now get to business == */
    send_to_char ( "You have accepted the challenge!\n\r", ch );
    act ( "$n accepts your challenge!", ch, NULL, victim, TO_VICT );
    arenas_today++;
/* == announce the upcoming event == */
    lvl1 = ch->hit;
    lvl2 = victim->hit;
    odd1 = ( lvl1 / lvl2 );
    odd2 = ( lvl2 / lvl1 );
    sprintf ( buf1,
              "{R[{WArena{R]{w %s {W({D%d {wwins{W) ({D%d {wlosses{W) {wPayoff odds %f\n\r",
              victim->name, victim->pcdata->awins, victim->pcdata->alosses,
              odd1 );
    sprintf ( buf2,
              "{R[{WArena{R]{w %s {W({D%d {wwins{W) ({D%d {wlosses{W) {wPayoff odds %f\n\r",
              ch->name, ch->pcdata->awins, ch->pcdata->alosses, odd2 );
    strcpy ( buf3,
             "{R[{WArena{R]{x To wager on the fight, type: {bbet{x ({bamount{x) ({bplayer name{x)\n\r" );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf1, d->character );
            send_to_char ( buf2, d->character );
            send_to_char ( buf3, d->character );
            d->character->gladiator = NULL;
        }
    }

    arena_countdown = 0;    
/* == now move them both to an arena for the fun == */

    if ( IS_SET ( ch->act2, PLR2_PKSAFE ) )
    REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
    if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
    REMOVE_BIT ( victim->act2, PLR2_PKSAFE );

    send_to_char ( "You make your way into the arena.\n\r", ch );
    char_from_room ( ch );
    char_to_room ( ch, get_room_index ( char_room ) );
    do_look ( ch, "auto" );
// SET_BIT(ch->act,PLR_SILENCE);
// SET_BIT(ch->act,PLR_NO_TELL);
    send_to_char ( "You make your way to the arena.\n\r", victim );
    char_from_room ( victim );
    char_to_room ( victim, get_room_index ( vict_room ) );
    do_look ( victim, "auto" );
// SET_BIT(victim->act,PLR_SILENCE);
// SET_BIT(victim->act,PLR_NO_TELL);
    arena = FIGHT_BUSY;
    return;
}

/*=======================================================================*
 * function: do_decline                                                  *
 * purpose: to chicken out from a sent arena challenge                   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
CH_CMD ( do_decline )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
/*== make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;
    if ( !IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You have not been challenged.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char
            ( "You must specify whose challenge you wish to decline.\n\r",
              ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't logged in!\n\r", ch );
        return;
    }
    if ( !IS_SET ( victim->act2, PLR2_CHALLENGER ) ||
         victim != ch->challenger )
    {
        send_to_char ( "That player hasn't challenged you.\n\r", ch );
        return;
    }
    if ( victim == ch )
        return;
/*== now actually decline == */
    victim->challenged = NULL;
    REMOVE_BIT ( victim->act2, PLR2_CHALLENGER );
    ch->challenger = NULL;
    REMOVE_BIT ( ch->act2, PLR2_CHALLENGED );
    arena = FIGHT_OPEN;
    send_to_char ( "Challenge declined!\n\r", ch );
    act ( "$n has declined your challenge.", ch, NULL, victim, TO_VICT );
    sprintf ( buf,
              "{R[{WArena{R]{b %s {xhas declined {b%s's {xchallenge.\n\r",
              ch->name, victim->name );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf, d->character );
        }
    }
    arena_countdown = 0;    
    return;
}

/*======================================================================*
 * function: do_bet                                                     *
 * purpose: to allow players to wager on the outcome of arena battles   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96               *
 *======================================================================*/
CH_CMD ( do_bet )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *fighter;
    int wager;
    argument = one_argument ( argument, arg );
    if ( argument[0] == '\0' || !is_number ( arg ) )
    {
        send_to_char ( "Syntax: BET [amount] [player]\n\r", ch );
        return;
    }
    if ( ch->gladiator != NULL )
    {
        send_to_char ( "You have already placed a bet on this fight.\n\r",
                       ch );
        return;
    }
/*== disable the actual fighters from betting ==*/
    if ( IS_SET ( ch->act2, PLR2_CHALLENGER ) ||
         IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You can't bet on this battle.\n\r", ch );
        return;
    }
    fighter = get_char_world ( ch, argument );
/*== make sure the choice is valid ==*/
    if ( fighter == NULL )
    {
        send_to_char ( "That player is not logged in.\n\r", ch );
        return;
    }
    if ( IS_NPC ( fighter ) )
    {
        send_to_char ( "Why bet on a mob? They aren't fighting...\n\r", ch );
        return;
    }
    if ( !IS_SET ( fighter->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "That player is not in the arena.\n\r", ch );
        return;
    }
    if ( IS_IMMORTAL ( fighter ) )
    {
	send_to_char ( "Sorry, you can't bet on an immortal.\n\r", ch );
	return;
    }
/*== do away with the negative number trickery ==*/
    if ( !str_prefix ( "-", arg ) )
    {
        send_to_char ( "Error: Invalid argument!\n\r", ch );
        return;
    }
    wager = atoi ( arg );
    if ( wager > 50 || wager < 1 )
    {
        send_to_char ( "Wager range is between 1 and 50 platinum\n\r", ch );
        return;
    }
/*== make sure they have the cash ==*/
    if ( wager > ch->platinum )
    {
        send_to_char ( "You don't have that much platinum to wager!\n\r",
                       ch );
        return;
    }
/*== now set the info ==*/
    ch->gladiator = fighter;
    ch->pcdata->plr_wager = wager;
    sprintf ( buf, "You have placed a %d platinum wager on %s\n\r", wager,
              fighter->name );
    send_to_char ( buf, ch );
    return;
}

/*CH_CMD ( do_dismiss )
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if( IS_NPC ( ch ) || IS_NPC( victim ) )
   return;

 if( !IS_SET(ch->act2,PLR2_CHALLENGER) )
  {
  send_to_char("You have not sent a challenge.\n\r",ch);
  return;
  }
 victim = ch->challenged;
 if(( get_char_world(ch,victim->name)) == NULL)
  {
  send_to_char("You opponent is not logged in!\n\r",ch);
  return;
  }

 if( arena == FIGHT_BUSY )
  {
  send_to_char("Sorry, you can't chicken out now!.\n\r",ch);
  return;
  }

ch->challenged = NULL;
REMOVE_BIT(ch->act2,PLR2_CHALLENGER);
victim->challenger = NULL;
REMOVE_BIT(victim->act2,PLR2_CHALLENGED);
arena = FIGHT_OPEN;
send_to_char("Challenge removed!\n\r",ch);
// act("$n has removed their challenge.",ch,NULL,victim,TO_VICT);
sprintf(buf,"{R[{WArena{R]{x {b%s{x has {Gremoved{x their challenge to{x {b%s{x.\n\r",
        ch->name, victim->name);

do_gmessage ( buf );
return;
}
*/
