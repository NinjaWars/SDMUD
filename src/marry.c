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

/********************************************************************************
 * Marriage code copyright 1999-2001                                            *
 * Markanth : dlmud@dlmud.com                                                   *
 * Devil's Lament : dlmud.com port 3778                                         *
 * Web Page : http://www.dlmud.com                                              *
 *                                                                              *
 * Provides ability for players to marry one another.                           *
 * Note: If your homophobic, you will have to add sex checks.                   *
 *                                                                              *
 * All I ask in return is that you give me credit on your mud somewhere         *
 * or email me if you use it, or if you make any changes.                       *
 ********************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sd.h"
// #include "interp.h"

void do_propose (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *spouse;

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( IS_MARRIED ( ch ) )
    {
        send_to_char
            ( "You are already married, are you planning on cheating?!\n\r",
              ch );
        return;
    }

    if ( argument[0] == '\0')
    {
        send_to_char("Propose to who?\n\r", ch);
        return;
    }

    if ( ( spouse = get_char_room ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( spouse ) || spouse == ch )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( IS_MARRIED ( spouse ) )
    {
        send_to_char
            ( "They are already married I don't think they're interested.\n\r",
              ch );
        return;
    }

    free_string(ch->pcdata->spouse);
    ch->pcdata->spouse = str_dup( spouse->name );
    act ( "You get down on one knee and propose marriage to $N.", ch, NULL,
          spouse, TO_CHAR );
    act ( "$n gets down on one knee and proposes marriage to you! Will you consent?", ch, NULL, spouse, TO_VICT );
    act ( "$n gets down on one knee and proposes marriage to $N!", ch, NULL,
          spouse, TO_NOTVICT );
    return;
}

void do_consent (CHAR_DATA *ch, char *argument)
{

    CHAR_DATA *spouse;

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( IS_MARRIED ( ch ) )
    {
        send_to_char
            ( "You are already married, are you planning on cheating?!\n\r",
              ch );
        return;
    }

    if ( argument[0] == '\0')
    {
        send_to_char("Consent to who?\n\r", ch);
        return;
    }

    if ( ( spouse = get_char_room ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( spouse ) || spouse == ch )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( IS_MARRIED ( spouse ) )
    {
        send_to_char
            ( "They are already married I don't think they're interested.\n\r",
              ch );
        return;
    }

    if ( spouse->pcdata->spouse == NULL || spouse->pcdata->spouse[0] == '\0' ||
         str_cmp ( spouse->pcdata->spouse, ch->name ) )
    {
        send_to_char ( "They haven't proposed marriage! How embarrasing!\n\r", ch );
        return;
    }

    free_string( ch->pcdata->spouse );
	ch->pcdata->spouse = str_dup( spouse->name );
    act ( "You consent to $N's proposal of marriage!", ch, NULL, spouse,
          TO_CHAR );
    act ( "$n consents to your proposal of marriage!", ch, NULL, spouse,
          TO_VICT );
    act ( "$n consents to $N's proposal of marriage!", ch, NULL, spouse,
          TO_NOTVICT );
    return;
}

void do_marry (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vict1, *vict2;
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax: marry <husband> <wife>\n\r", ch );
        return;
    }

    if ( ( vict1 = get_char_room ( ch, arg1 ) ) == NULL ||
         ( vict2 = get_char_room ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "They seemed to have gotten cold feet! Where'd they go?\n\r",
                 ch );
        return;
    }

    if ( IS_NPC ( vict1 ) || IS_NPC ( vict2 ) || vict1 == vict2 )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( vict1->pcdata->spouse[0] == '\0' )
    {
        act("$N hasn't proposed to anyone.", ch, NULL, vict1, TO_CHAR);
        return;
    }

    if ( vict2->pcdata->spouse[0] == '\0' )
    {
        act("$N hasn't proposed to anyone.", ch, NULL, vict2, TO_CHAR);
        return;
    }

    if(str_cmp(vict1->pcdata->spouse, vict2->name) ||
        str_cmp(vict2->pcdata->spouse, vict1->name))
    {
        send_to_char ("They aren't engaged.\n\r", ch);
        return;
    }

    if ( IS_SET ( vict1->act, PLR2_MARRIED ) )
    {
        act("$N is already married.. maybe they need a divorce.", ch, NULL, vict1, TO_CHAR);
        return;
    }

    if ( IS_SET ( vict2->act, PLR2_MARRIED ) )
    {
        act("$N is already married.. maybe they need a divorce.", ch, NULL, vict2, TO_CHAR);
        return;
    }

    SET_BIT ( vict1->act, PLR2_MARRIED );
    SET_BIT ( vict2->act, PLR2_MARRIED );
    act ( "$N pronounces you man and wife!", vict1, NULL, ch, TO_CHAR );
    act ( "$N pronounces you man and wife!", vict2, NULL, ch, TO_CHAR );
    send_to_char("You pronounce them man and wife!", ch);
    return;
}

void do_divorce (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vict1, *vict2;
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax: divorce <husband> <wife>\n\r", ch );
        return;
    }

    if ( ( vict1 = get_char_room ( ch, arg1 ) ) == NULL ||
         ( vict2 = get_char_room ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( vict1 ) || IS_NPC ( vict2 ) || vict1 == vict2 )
    {
        send_to_char ( "Don't think so.\n\r", ch );
        return;
    }

    if ( !IS_MARRIED ( vict1 ) )
    {
        act ( "$N is not married.", ch, NULL, vict1, TO_CHAR );
        return;
    }

    if ( !IS_MARRIED ( vict2 ) )
    {
        act ( "$N is not married.", ch, NULL, vict2, TO_CHAR );
        return;
    }

    if(str_cmp(vict1->pcdata->spouse, vict2->name) ||
        str_cmp(vict2->pcdata->spouse, vict1->name))
    {
        send_to_char ("They aren't engaged.\n\r", ch);
        return;
    }

    REMOVE_BIT ( vict1->act, PLR2_MARRIED );
    REMOVE_BIT ( vict2->act, PLR2_MARRIED );
    free_string( vict1->pcdata->spouse );
	vict1->pcdata->spouse = str_dup( "" );
    free_string( vict2->pcdata->spouse );
	vict2->pcdata->spouse = str_dup( "" );
    act ( "$N hands you your divorce papers.", vict1, NULL, ch, TO_CHAR );
    act ( "$N hands you your divorce papers.", vict2, NULL, ch, TO_CHAR );
    send_to_char("You hand them thier divorce papers.", ch);
    return;
}

void do_spousetalk (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *spouse;

    if ( IS_NPC ( ch ) )
        return;

    if ( !IS_MARRIED ( ch ) )
    {
        send_to_char ( "You aren't married!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Tell your spouse what?\n\r", ch );
        return;
    }

    if ( ( spouse = get_char_world ( ch, ch->pcdata->spouse ) ) == NULL || IS_NPC(spouse))
    {
        act ( "You can't find $t.", ch, ch->pcdata->spouse, NULL, TO_CHAR );
        return;
    }

    act_new ( "You confide in $N: {D$t{x", ch, argument, spouse, TO_CHAR, POS_SLEEPING );
    act_new ( "$N confides in you: {D$t{x", spouse, argument, ch, TO_CHAR, POS_SLEEPING );
    return;
}

void do_union (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *spouse;

    if ( IS_NPC ( ch ) )
        return;

    if ( !IS_MARRIED ( ch ) )
    {
        send_to_char ( "You aren't married!\n\r", ch );
        return;
    }

    if ( ( spouse = get_char_world ( ch, ch->pcdata->spouse ) ) == NULL ||
         ( !spouse->in_room ) || IS_NPC(spouse) )
    {
        act ( "You can't find $t.", ch, ch->pcdata->spouse, NULL, TO_CHAR );
        return;
    }

    if ( IS_SET ( ch->in_room->room_flags, ROOM_NO_RECALL ) ||
         IS_AFFECTED ( ch, AFF_CURSE ) )
    {
        send_to_char ( "The Gods have forsaken you.\n\r", ch );
        return;
    }

    
    if ( ch->fighting != NULL )
    {
        send_to_char ( "Your a little busy right now.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) ||
    	 IS_SET ( ch->in_room->room_flags, ROOM_NO_RECALL ) ||
	 IS_SET ( spouse->in_room->room_flags, ROOM_ARENA ) ||
	 IS_SET ( spouse->in_room->room_flags, ROOM_NO_RECALL ) ||
	 IS_SET ( ch->act2, PLR2_PKVIOLENT ) ||
	 IS_SET ( spouse->act2, PLR2_PKVIOLENT ) ||
	 IS_SET ( ch->in_room->room_flags, ROOM_PRIVATE ) ||
	 IS_SET ( spouse->in_room->room_flags, ROOM_PRIVATE ) )
	 {
	     send_to_char ( "Don't even think about it!\n\r", ch );
	     return;
	 }
	 
    act ( "$n leaves to find $s spouse.", ch, NULL, NULL, TO_ROOM );
    char_from_room ( ch );
    char_to_room ( ch, spouse->in_room );
    act ( "$n arrives for $s spouse.", ch, NULL, NULL, TO_ROOM );
    do_look ( ch, "auto" );
    return;
}
