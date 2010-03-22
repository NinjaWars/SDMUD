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
#include <time.h>
#include <unistd.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "magic.h"
#include "olc.h"
#include "db.h"

CH_CMD ( do_wiznet )
{
    int flag;
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->wiznet, WIZ_ON ) )
        {
            send_to_char ( "Signing off of Wiznet.\n\r", ch );
            REMOVE_BIT ( ch->wiznet, WIZ_ON );
        }
        else
        {
            send_to_char ( "Welcome to Wiznet!\n\r", ch );
            SET_BIT ( ch->wiznet, WIZ_ON );
        }
        return;
    }

    if ( !str_prefix ( argument, "on" ) )
    {
        send_to_char ( "Welcome to Wiznet!\n\r", ch );
        SET_BIT ( ch->wiznet, WIZ_ON );
        return;
    }

    if ( !str_prefix ( argument, "off" ) )
    {
        send_to_char ( "Signing off of Wiznet.\n\r", ch );
        REMOVE_BIT ( ch->wiznet, WIZ_ON );
        return;
    }

    /* show wiznet status */
    if ( !str_prefix ( argument, "status" ) )
    {
        buf[0] = '\0';

        if ( !IS_SET ( ch->wiznet, WIZ_ON ) )
            strcat ( buf, "off " );

        for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
            if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) )
            {
                strcat ( buf, wiznet_table[flag].name );
                strcat ( buf, " " );
            }

        strcat ( buf, "\n\r" );

        send_to_char ( "Wiznet status:\n\r", ch );
        send_to_char ( buf, ch );
        return;
    }

    if ( !str_prefix ( argument, "show" ) )
        /* list of all wiznet options */
    {
        buf[0] = '\0';

        for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
        {
            if ( wiznet_table[flag].level <= get_trust ( ch ) )
            {
                strcat ( buf, wiznet_table[flag].name );
                strcat ( buf, " " );
            }
        }

        strcat ( buf, "\n\r" );

        send_to_char ( "Wiznet options available to you are:\n\r", ch );
        send_to_char ( buf, ch );
        return;
    }

    flag = wiznet_lookup ( argument );

    if ( flag == -1 || get_trust ( ch ) < wiznet_table[flag].level )
    {
        send_to_char ( "No such option.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) )
    {
        sprintf ( buf, "You will no longer see %s on wiznet.\n\r",
                  wiznet_table[flag].name );
        send_to_char ( buf, ch );
        REMOVE_BIT ( ch->wiznet, wiznet_table[flag].flag );
        return;
    }
    else
    {
        sprintf ( buf, "You will now see %s on wiznet.\n\r",
                  wiznet_table[flag].name );
        send_to_char ( buf, ch );
        SET_BIT ( ch->wiznet, wiznet_table[flag].flag );
        return;
    }

}

void wiznet ( char *string, CHAR_DATA * ch, OBJ_DATA * obj, long flag,
              long flag_skip, int min_level )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf ( buf, "{V%s{x", string );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             ( IS_HERO ( d->character ) ||
               ( d->character->class >= MAX_CLASS / 2 ) ) &&
             IS_SET ( d->character->wiznet, WIZ_ON ) && ( !flag ||
                                                          IS_SET ( d->
                                                                   character->
                                                                   wiznet,
                                                                   flag ) ) &&
             ( !flag_skip || !IS_SET ( d->character->wiznet, flag_skip ) ) &&
             get_trust ( d->character ) >= min_level && d->character != ch )
        {
            if ( IS_SET ( d->character->wiznet, WIZ_PREFIX ) )
                send_to_char ( "{y-{Y-{R>{r>{x ", d->character );
            act_new ( buf, d->character, obj, ch, TO_CHAR, POS_DEAD );
        }
    }

    return;
}

/*
void wiznet ( char *string, CHAR_DATA * ch, OBJ_DATA * obj, long flag,
              long flag_skip, int min_level )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf ( buf, "{V%s{x", string );
    if ( strlen ( string ) >= 4 )
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING && IS_IMMORTAL ( d->character ) &&
                 IS_SET ( d->character->wiznet, WIZ_ON ) && ( !flag ||
                                                              IS_SET ( d->
                                                                       character->
                                                                       wiznet,
                                                                       flag ) )
                 && ( !flag_skip ||
                      !IS_SET ( d->character->wiznet, flag_skip ) ) &&
                 get_trust ( d->character ) >= min_level && d->character != ch )
            {
                if ( IS_SET ( d->character->wiznet, WIZ_PREFIX ) )
                    send_to_char ( "{y-{Y-{R>{r>{x ", d->character );
                act_new ( buf, d->character, obj, ch, TO_CHAR, POS_DEAD );
            }
        }

    return;
}
*/
CH_CMD ( do_iquest )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    argument = one_argument ( argument, arg1 );

    if ( ( arg1[0] == '\0' ) || ( !IS_IMMORTAL ( ch ) ) )
    {
        if ( !global_quest )
        {
            send_to_char ( "There is no quest in progress.\n\r", ch );
            return;
        }
        if ( ch->on_quest )
        {
            send_to_char ( "You'll have to wait till the quest is over.\n\r",
                           ch );
            return;
        }
        ch->on_quest = TRUE;
        send_to_char ( "Your quest flag is now on.\n\r", ch );
        return;
    }
    if ( !str_cmp ( arg1, "on" ) )
    {
        if ( global_quest )
        {
            send_to_char ( "The global quest flag is already on.\n\r", ch );
            return;
        }
        global_quest = TRUE;
        send_to_char ( "The global quest flag is now on.\n\r", ch );
        return;
    }
    if ( !str_cmp ( arg1, "off" ) )
    {
        if ( !global_quest )
        {
            send_to_char ( "The global quest flag is not on.\n\r", ch );
            return;
        }
        global_quest = FALSE;
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING )
            {
                d->character->on_quest = FALSE;
            }
        }
        send_to_char ( "The global quest flag is now off.\n\r", ch );
        return;
    }
    do_iquest ( ch, "" );
    return;
}

/* equips a character */
CH_CMD ( do_outfit )
{
    OBJ_DATA *obj;
    int i, sn, vnum;

    if ( ch->level > 9 || IS_NPC ( ch ) )
    {
        send_to_char ( "Find it yourself!\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) == NULL )
    {
        if ( ch->carry_number + 1 > can_carry_n ( ch ) )
        {
            send_to_char ( "You can't carry any more items.\n\r", ch );
            return;
        }
        obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_BANNER ), 0 );
        obj->cost = 0;
        obj_to_char ( obj, ch );
        equip_char ( ch, obj, WEAR_LIGHT );
        act ( "$G gives you a light.", ch, NULL, NULL, TO_CHAR );
    }

    if ( ( obj = get_eq_char ( ch, WEAR_BODY ) ) == NULL )
    {
        if ( ch->carry_number + 1 > can_carry_n ( ch ) )
        {
            send_to_char ( "You can't carry any more items.\n\r", ch );
            return;
        }
        obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_VEST ), 0 );
        obj->cost = 0;
        obj_to_char ( obj, ch );
        equip_char ( ch, obj, WEAR_BODY );
        act ( "$G gives you a vest.", ch, NULL, NULL, TO_CHAR );
    }

    /* do the weapon thing */
    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        sn = 0;
        vnum = OBJ_VNUM_SCHOOL_SWORD;   /* just in case! */

        if ( ch->carry_number + 1 > can_carry_n ( ch ) )
        {
            send_to_char ( "You can't carry any more items.\n\r", ch );
            return;
        }

        for ( i = 0; weapon_table[i].name != NULL; i++ )
        {
            if ( ch->pcdata->learned[sn] <
                 ch->pcdata->learned[*weapon_table[i].gsn] )
            {
                sn = *weapon_table[i].gsn;
                vnum = weapon_table[i].vnum;
            }
        }

        obj = create_object ( get_obj_index ( vnum ), 0 );
        obj_to_char ( obj, ch );
        equip_char ( ch, obj, WEAR_WIELD );
        act ( "$G gives you a weapon.", ch, NULL, NULL, TO_CHAR );
    }

    if ( ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL ||
           !IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) ) &&
         ( obj = get_eq_char ( ch, WEAR_SHIELD ) ) == NULL )
    {
        if ( ch->carry_number + 1 > can_carry_n ( ch ) )
        {
            send_to_char ( "You can't carry any more items.\n\r", ch );
            return;
        }
        obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_SHIELD ), 0 );
        obj->cost = 0;
        obj_to_char ( obj, ch );
        equip_char ( ch, obj, WEAR_SHIELD );
        act ( "$G gives you a shield.", ch, NULL, NULL, TO_CHAR );
    }
}

/* RT nochannels command, for those spammers */
CH_CMD ( do_nochannels )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Nochannel whom?", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->level >= ch->level )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOCHANNELS ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOCHANNELS );
        send_to_char ( "The gods have restored your channel priviliges.\n\r",
                       victim );
        send_to_char ( "NOCHANNELS removed.\n\r", ch );
        sprintf ( buf, "$N restores channels to %s", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOCHANNELS );
        send_to_char ( "The gods have revoked your channel priviliges.\n\r",
                       victim );
        send_to_char ( "NOCHANNELS set.\n\r", ch );
        sprintf ( buf, "$N revokes %s's channels.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    return;
}

CH_CMD ( do_smote )
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) )
    {
        send_to_char ( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Emote what?\n\r", ch );
        return;
    }

    if ( strstr ( argument, ch->name ) == NULL )
    {
        send_to_char ( "You must include your name in an smote.\n\r", ch );
        return;
    }

    send_to_char ( argument, ch );
    send_to_char ( "\n\r", ch );

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( vch->desc == NULL || vch == ch )
            continue;

        if ( ( letter = strstr ( argument, vch->name ) ) == NULL )
        {
            send_to_char ( argument, vch );
            send_to_char ( "\n\r", vch );
            continue;
        }

        strcpy ( temp, argument );
        temp[strlen ( argument ) - strlen ( letter )] = '\0';
        last[0] = '\0';
        name = vch->name;

        for ( ; *letter != '\0'; letter++ )
        {
            if ( *letter == '\'' && matches == strlen ( vch->name ) )
            {
                strcat ( temp, "r" );
                continue;
            }

            if ( *letter == 's' && matches == strlen ( vch->name ) )
            {
                matches = 0;
                continue;
            }

            if ( matches == strlen ( vch->name ) )
            {
                matches = 0;
            }

            if ( *letter == *name )
            {
                matches++;
                name++;
                if ( matches == strlen ( vch->name ) )
                {
                    strcat ( temp, "you" );
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat ( last, letter, 1 );
                continue;
            }

            matches = 0;
            strcat ( temp, last );
            strncat ( temp, letter, 1 );
            last[0] = '\0';
            name = vch->name;
        }

        send_to_char ( temp, vch );
        send_to_char ( "\n\r", vch );
    }

    return;
}

CH_CMD ( do_bamfin )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC ( ch ) )
    {
        smash_tilde ( argument );

        if ( argument[0] == '\0' )
        {
            sprintf ( buf, "Your poofin is %s\n\r", ch->pcdata->bamfin );
            send_to_char ( buf, ch );
            return;
        }
/*
        if ( strstr ( argument, ch->name ) == NULL )
        {
            send_to_char ( "You must include your name.\n\r", ch );
            return;
        }*/

        free_string ( ch->pcdata->bamfin );
        ch->pcdata->bamfin = str_dup ( argument );

        sprintf ( buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin );
        send_to_char ( buf, ch );
    }
    return;
}

CH_CMD ( do_bamfout )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC ( ch ) )
    {
        smash_tilde ( argument );

        if ( argument[0] == '\0' )
        {
            sprintf ( buf, "Your poofout is %s\n\r", ch->pcdata->bamfout );
            send_to_char ( buf, ch );
            return;
        }
/*
        if ( strstr ( argument, ch->name ) == NULL )
        {
            send_to_char ( "You must include your name.\n\r", ch );
            return;
        }*/

        free_string ( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup ( argument );

        sprintf ( buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout );
        send_to_char ( buf, ch );
    }
    return;
}

CH_CMD ( do_deny )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Deny whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    SET_BIT ( victim->act, PLR_DENY );
    send_to_char ( "You are denied access!\n\r", victim );
    sprintf ( buf, "$N denies access to %s", victim->name );
    wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    send_to_char ( "OK.\n\r", ch );
    save_char_obj ( victim );
    stop_fighting ( victim, TRUE );
    do_quit ( victim, "" );

    return;
}

CH_CMD ( do_wipe )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Wipe whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    SET_BIT ( victim->act2, PLR2_WIPED );
    sprintf ( buf, "$N wipes access to %s", victim->name );
    wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    send_to_char ( "OK.\n\r", ch );
    save_char_obj ( victim );
    stop_fighting ( victim, TRUE );
    do_disconnect ( ch, victim->name );

    return;
}

CH_CMD ( do_disconnect )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Disconnect whom?\n\r", ch );
        return;
    }

    if ( is_number ( arg ) )
    {
        int desc;

        if ( ch->level < MAX_LEVEL )
        {
            return;
        }

        desc = atoi ( arg );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->descriptor == desc )
            {
                close_socket ( d );
                send_to_char ( "Ok.\n\r", ch );
                return;
            }
        }
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL )
    {
        act ( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( IS_SET ( victim->act, PLR_KEY ) && ( ch->level < MAX_LEVEL ) )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d == ch->desc )
            {
                close_socket ( d );
                return;
            }
        }
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d == victim->desc )
        {
            close_socket ( d );
            send_to_char ( "Ok.\n\r", ch );
            return;
        }
    }

    bug ( "Do_disconnect: desc not found.", 0 );
    send_to_char ( "Descriptor not found!\n\r", ch );
    return;
}

CH_CMD ( do_twit )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: twit <character>.\n\r", ch );
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
        send_to_char ( "Your command backfires!\n\r", ch );
        send_to_char ( "You are now considered a TWIT.\n\r", ch );
        SET_BIT ( ch->act, PLR_TWIT );
        return;
    }

    if ( IS_SET ( victim->act, PLR_TWIT ) )
    {
        send_to_char ( "Someone beat you to it.\n\r", ch );
    }
    else
    {
        SET_BIT ( victim->act, PLR_TWIT );
        send_to_char ( "Twit flag set.\n\r", ch );
        send_to_char ( "You are now considered a TWIT.\n\r", victim );
    }
    return;
}

CH_CMD ( do_permtwit )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: permtwit <character>.\n\r", ch );
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
        send_to_char ( "Your command backfires!\n\r", ch );
        send_to_char ( "You are now considered a TWIT.\n\r", ch );
        SET_BIT ( ch->act, PLR_TWIT );
	SET_BIT ( ch->act2, PLR2_PERM_TWIT );
        return;
    }

    if ( IS_SET ( victim->act, PLR_TWIT ) )
    {
        send_to_char ( "Someone beat you to it.\n\r", ch );
    }
    else
    {
        SET_BIT ( victim->act, PLR_TWIT );
	SET_BIT ( victim->act2, PLR2_PERM_TWIT );
        send_to_char ( "Permament twit flag set (Remove with pardon).\n\r", ch );
        send_to_char ( "You are now considered a TWIT.\n\r", victim );
    }
    return;
}

CH_CMD ( do_violent )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: violent <character>.\n\r", ch );
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
        send_to_char ( "No.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
    {
	REMOVE_BIT ( victim->act2, PLR2_PKVIOLENT );
	REMOVE_BIT ( victim->act, PLR_NORESTORE );
	send_to_char ( "{RViolent{x removed.\n\r", ch );
	send_to_char ( "You are no longer {Rviolent{x.\n\r", victim );
	victim->pcdata->pkviolenttime = 0;
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_PKVIOLENT );
	SET_BIT ( victim->act, PLR_NORESTORE );
        send_to_char ( "{RViolent{x flag set.\n\r", ch );
        send_to_char ( "You are now {Rviolent{x.\n\r", victim );
	victim->pcdata->pkviolenttime = 6;
    }
    return;
}

CH_CMD ( do_safe )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: safe <character>.\n\r", ch );
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
        send_to_char ( "No.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
    {
	REMOVE_BIT ( victim->act2, PLR2_PKSAFE );
	send_to_char ( "{wSafe{x removed.\n\r", ch );
	send_to_char ( "You are no longer {wsafe{x!\n\r", victim );
	victim->pcdata->pksafetime = 0;
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_PKSAFE );
        send_to_char ( "{wSafe{x flag set.\n\r", ch );
        send_to_char ( "You are now {wsafe{x!\n\r", victim );
	victim->pcdata->pksafetime = 8;
    }
    return;
}

CH_CMD ( do_pardon )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: pardon <character>.\n\r", ch );
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

    if ( IS_SET ( victim->act, PLR_TWIT ) )
    {
        REMOVE_BIT ( victim->act, PLR_TWIT );
	if ( IS_SET ( victim->act2, PLR2_PERM_TWIT ) )
	REMOVE_BIT ( victim->act2, PLR2_PERM_TWIT );
        send_to_char ( "Twit flag removed.\n\r", ch );
        send_to_char ( "You are no longer a TWIT.\n\r", victim );
    }
    return;
}

CH_CMD ( do_echo )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Global echo what?\n\r", ch );
        return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            if ( d->character->level >= ch->level )
                send_to_char ( "global> ", d->character );
            send_to_char ( argument, d->character );
            send_to_char ( "\n\r", d->character );
        }
    }

    return;
}

CH_CMD ( do_wecho )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Warn echo what?\n\r", ch );
        return;
    }

    sprintf ( buf, "{z{W***{x {Y%s{x {z{W***{x", argument );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_echo ( ch, buf );
    do_restore ( ch, "all" );
    do_allpeace ( ch, "" );
    return;
}

CH_CMD ( do_recho )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Local echo what?\n\r", ch );

        return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character->in_room == ch->in_room )
        {
            if ( d->character->level >= ch->level )
                send_to_char ( "local> ", d->character );
            send_to_char ( argument, d->character );
            send_to_char ( "\n\r", d->character );
        }
    }

    return;
}

CH_CMD ( do_zecho )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Zone echo what?\n\r", ch );
        return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character->in_room != NULL &&
             ch->in_room != NULL &&
             d->character->in_room->area == ch->in_room->area )
        {
            if ( d->character->level >= ch->level )
                send_to_char ( "zone> ", d->character );
            send_to_char ( argument, d->character );
            send_to_char ( "\n\r", d->character );
        }
    }
}

CH_CMD ( do_pecho )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg );

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char ( "Personal echo what?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "Target not found.\n\r", ch );
        return;
    }

    if ( victim->level >= ch->level )
        send_to_char ( "personal> ", victim );

    send_to_char ( argument, victim );
    send_to_char ( "\n\r", victim );
    send_to_char ( "personal> ", ch );
    send_to_char ( argument, ch );
    send_to_char ( "\n\r", ch );
}

ROOM_INDEX_DATA *find_location ( CHAR_DATA * ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( is_number ( arg ) )
        return get_room_index ( atol ( arg ) );

    if ( ( victim = get_char_world ( ch, arg ) ) != NULL )
        return victim->in_room;

    if ( ( obj = get_obj_world ( ch, arg ) ) != NULL )
        return obj->in_room;

    return NULL;
}

CH_CMD ( do_corner )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Corner whom?\n\r", ch );
        return;
    }

    sprintf ( buf, "%s %d", arg, ROOM_VNUM_CORNER );
    do_transfer ( ch, buf );
//    do_nochannels ( ch, buf );
    return;
}

CH_CMD ( do_transfer )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Transfer whom (and where)?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg1, "all" ) && ( ch->level >= CREATOR ) )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING && d->character != ch &&
                 d->character->in_room != NULL &&
                 ch->level >= d->character->ghost_level &&
                 can_see ( ch, d->character ) )
            {
                char buf[MAX_STRING_LENGTH];
                sprintf ( buf, "%s %s", d->character->name, arg2 );
                do_transfer ( ch, buf );
            }
        }
        return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
        location = ch->in_room;
    }
    else
    {
        if ( ( location = find_location ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "No such location.\n\r", ch );
            return;
        }

        if ( !is_room_owner ( ch, location ) &&
             room_is_private ( ch, location ) && ch->level < MAX_LEVEL )
        {
            send_to_char ( "That room is private right now.\n\r", ch );
            return;
        }
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( ( victim->level > ch->level && !IS_SET ( ch->act, PLR_KEY ) &&
           ( victim->level != MAX_LEVEL ) ) ||
         ( ( IS_SET ( victim->act, PLR_KEY ) ) &&
           ( ch->level != MAX_LEVEL ) ) )
    {
        send_to_char ( "You failed!\n\r", ch );
        return;
    }

    if ( victim->in_room == NULL )
    {
        send_to_char ( "They are in limbo.\n\r", ch );
        return;
    }

    if ( victim->fighting != NULL )
        stop_fighting ( victim, TRUE );
    act ( "A mighty hand of the gods reaches down and grabs $n.", victim, NULL, NULL, TO_ROOM );
    char_from_room ( victim );
    char_to_room ( victim, location );
    act ( "$n is thrown into the room by a hand of the gods.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
        act ( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look ( victim, "auto" );
    if ( victim->pet != NULL )
    {
        char_from_room ( victim->pet );
        char_to_room ( victim->pet, location );
        act ( "$n materializes!", victim->pet, NULL, NULL, TO_ROOM );
    }
    send_to_char ( "Ok.\n\r", ch );
}

CH_CMD ( do_allpeace )
{
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character != ch &&
             d->character->in_room != NULL &&
             ch->level >= d->character->ghost_level &&
             can_see ( ch, d->character ) )
        {
            char buf[MAX_STRING_LENGTH];
            sprintf ( buf, "%s peace", d->character->name );
            do_at ( ch, buf );
        }
    }
}

CH_CMD ( do_wedpost )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: wedpost <char>\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't playing.\n\r", ch );
        return;
    }

    if ( victim->wedpost )
    {
        send_to_char
            ( "They are no longer allowed to post wedding announcements.\n\r",
              ch );
        victim->wedpost = FALSE;
    }
    else
    {
        send_to_char
            ( "They are now allowed to post wedding announcements.\n\r", ch );
        victim->wedpost = TRUE;
    }
}

CH_CMD ( do_recover )
{
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Recover whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You can't recover NPC's.\n\r", ch );
        return;
    }

    if ( ( get_trust ( victim ) > get_trust ( ch ) &&
           !IS_SET ( ch->act, PLR_KEY ) && ( victim->level != MAX_LEVEL ) ) ||
         ( ( IS_SET ( victim->act, PLR_KEY ) ) &&
           ( ch->level != MAX_LEVEL ) ) )
    {
        send_to_char ( "You failed!\n\r", ch );
        return;
    }

    if ( victim->in_room == NULL )
    {
        send_to_char ( "They are in limbo.\n\r", ch );
        return;
    }

    if ( victim->fighting != NULL )
    {
        send_to_char ( "They are fighting.\n\r", ch );
        return;
    }

    if ( victim->alignment < 0 )
    {
        if ( ( location = get_room_index ( ROOM_VNUM_TEMPLEB ) ) == NULL )
        {
            send_to_char ( "The recall point seems to be missing.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( ( location = get_room_index ( ROOM_VNUM_TEMPLE ) ) == NULL )
        {
            send_to_char ( "The recall point seems to be missing.\n\r", ch );
            return;
        }
    }

    if ( is_clan ( victim ) &&
         ( clan_table[victim->clan].hall != ROOM_VNUM_ALTAR ) &&
         !IS_SET ( victim->act, PLR_TWIT ) )
        location = get_room_index ( clan_table[victim->clan].hall );

    if ( IS_NPC ( victim ) && IS_SET ( ch->act, ACT_PET ) &&
         is_clan ( victim->master ) &&
         ( clan_table[victim->master->clan].hall != ROOM_VNUM_ALTAR ) &&
         !IS_SET ( victim->master->act, PLR_TWIT ) )
        location = get_room_index ( clan_table[victim->master->clan].hall );

    if ( victim->in_room == location )
    {
        act ( "$N does not need recovering.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !IS_SET ( victim->in_room->room_flags, ROOM_NO_RECALL ) &&
         !IS_AFFECTED ( victim, AFF_CURSE ) )
    {
        act ( "$N does not need recovering.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim->fighting != NULL )
        stop_fighting ( victim, TRUE );
    act ( "$n disappears in a flash.", victim, NULL, NULL, TO_ROOM );
    char_from_room ( victim );
    char_to_room ( victim, location );
    act ( "$n arrives from a flash of light.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
        act ( "$n has recovered you.", ch, NULL, victim, TO_VICT );
    do_look ( victim, "auto" );
    act ( "$N has been recovered.", ch, NULL, victim, TO_CHAR );
    if ( victim->pet != NULL )
    {
        char_from_room ( victim->pet );
        char_to_room ( victim->pet, location );
        act ( "$n materializes!", victim->pet, NULL, NULL, TO_ROOM );
    }
}

CH_CMD ( do_at )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "NPC's cannot use this command.\n\r", ch );
        return;
    }
    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "At where what?\n\r", ch );
        return;
    }

    if ( ( location = find_location ( ch, arg ) ) == NULL )
    {
        send_to_char ( "No such location.\n\r", ch );
        return;
    }

    if ( !is_room_owner ( ch, location ) && room_is_private ( ch, location )
         && ch->level < MAX_LEVEL )
    {
        send_to_char ( "That room is private right now.\n\r", ch );
        return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room ( ch );
    char_to_room ( ch, location );
    interpret ( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
        if ( wch == ch )
        {
            char_from_room ( ch );
            char_to_room ( ch, original );
            ch->on = on;
            break;
        }
    }

    return;
}

CH_CMD ( do_goto )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH];
    int count = 0;

    if ( ( argument[0] == '\0' ) && ( IS_NPC ( ch ) ) )
    {
        send_to_char ( "Goto where?\n\r", ch );
        return;
    }
    if ( ( argument[0] == '\0' ) && ( !ch->pcdata->recall ) )
    {
        send_to_char ( "Goto where?\n\r", ch );
        return;
    }
    if ( ( argument[0] == '\0' ) && ( ch->pcdata->recall ) )
    {
        sprintf ( arg, "%d", ch->pcdata->recall );
    }
    else
    {
        sprintf ( arg, "%s", argument );
    }
    if ( ( location = find_location ( ch, arg ) ) == NULL )
    {
        send_to_char ( "No such location.\n\r", ch );
        return;
    }

    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;

    if ( !is_room_owner ( ch, location ) && room_is_private ( ch, location )
         && ( count > 1 || ch->level < MAX_LEVEL ) )
    {
        send_to_char ( "That room is private right now.\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
        stop_fighting ( ch, TRUE );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( ( rch->level >= ch->invis_level ) &&
             ( rch->level >= ch->ghost_level ) )
        {
            if ( ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
                act ( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
            else
                act ( "$n leaves in a {Dd{Wr{cea{Wm{Dy {Bm{bi{Bs{bt{x.", ch, NULL, rch,
                      TO_VICT );
        }
    }

    char_from_room ( ch );
    char_to_room ( ch, location );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( ( rch->level >= ch->invis_level ) &&
             ( rch->level >= ch->ghost_level ) )
        {
            if ( ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0' )
                act ( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
            else
                act ( "$n appears in a {Dd{Wr{cea{Wm{Dy {Bm{bi{Bs{bt{x.", ch, NULL, rch,
                      TO_VICT );
        }
    }
//    if ( ( argument[0] == '\0' ) && ( ch->pet != NULL ) )
    do_look ( ch, "auto" );
    if ( ch->pet != NULL )
    {
        char_from_room ( ch->pet );
        char_to_room ( ch->pet, location );
	act ( "$n materializes!", ch->pet, NULL, NULL, TO_ROOM );
    }
    return;
}

CH_CMD ( do_violate )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Goto where?\n\r", ch );
        return;
    }

    if ( ( location = find_location ( ch, argument ) ) == NULL )
    {
        send_to_char ( "No such location.\n\r", ch );
        return;
    }

    if ( !room_is_private ( ch, location ) )
    {
        send_to_char ( "That room isn't private, use goto.\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
        stop_fighting ( ch, TRUE );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( ( rch->level >= ch->invis_level ) &&
             ( rch->level >= ch->ghost_level ) )
        {
            if ( ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
                act ( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
            else
                act ( "$n leaves in a swirling mist.", ch, NULL, rch,
                      TO_VICT );
        }
    }

    char_from_room ( ch );
    char_to_room ( ch, location );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( ( rch->level >= ch->invis_level ) &&
             ( rch->level >= ch->ghost_level ) )
        {
            if ( ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0' )
                act ( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
            else
                act ( "$n appears in a swirling mist.", ch, NULL, rch,
                      TO_VICT );
        }
    }

    do_look ( ch, "auto" );
    return;
}

/* RT to replace the 3 stat commands */

CH_CMD ( do_stat )
{
    char arg[MAX_INPUT_LENGTH];
    char *string;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;

    string = one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  stat <name>\n\r", ch );
        send_to_char ( "  stat obj <name>\n\r", ch );
        send_to_char ( "  stat mob <name>\n\r", ch );
        send_to_char ( "  stat room <number>\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "room" ) )
    {
        do_rstat ( ch, string );
        return;
    }

    if ( !str_cmp ( arg, "obj" ) )
    {
        do_ostat ( ch, string );
        return;
    }

    if ( !str_cmp ( arg, "char" ) || !str_cmp ( arg, "mob" ) )
    {
        do_mstat ( ch, string );
        return;
    }

    /* do it the old way */

    obj = get_obj_world ( ch, argument );
    if ( obj != NULL )
    {
        do_ostat ( ch, argument );
        return;
    }

    victim = get_char_world ( ch, argument );
    if ( victim != NULL )
    {
        do_mstat ( ch, argument );
        return;
    }

    location = find_location ( ch, argument );
    if ( location != NULL )
    {
        do_rstat ( ch, argument );
        return;
    }

    send_to_char ( "Nothing by that name found anywhere.\n\r", ch );
}

CH_CMD ( do_rstat )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg[MAX_INPUT_LENGTH * 100];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument ( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location ( ch, arg );
    if ( location == NULL )
    {
        send_to_char ( "No such location.\n\r", ch );
        return;
    }

    if ( !is_room_owner ( ch, location ) && ch->in_room != location &&
         room_is_private ( ch, location ) && !IS_TRUSTED ( ch, HEADIMP ) )
    {
        send_to_char ( "That room is private right now.\n\r", ch );
        return;
    }

    sprintf ( buf, "Name: '%s'\n\rArea: '%s'\n\r", location->name,
              location->area->name );
    send_to_char ( buf, ch );

    sprintf ( buf,
              "Vnum: %ld  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
              location->vnum, location->sector_type, location->light,
              location->heal_rate, location->mana_rate );
    send_to_char ( buf, ch );

    sprintf ( buf, "Room flags: %s.\n\rDescription:\n\r%s",
              room_bit_name ( location->room_flags ), location->description );
    send_to_char ( buf, ch );

    if ( location->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char ( "Extra description keywords: '", ch );
        for ( ed = location->extra_descr; ed; ed = ed->next )
        {
            send_to_char ( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char ( " ", ch );
        }
        send_to_char ( "'.\n\r", ch );
    }

    send_to_char ( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
        if ( ( get_trust ( ch ) >= rch->ghost_level ) &&
             ( can_see ( ch, rch ) ) )
        {
            send_to_char ( " ", ch );
            one_argument ( rch->name, buf );
            send_to_char ( buf, ch );
        }
    }

    send_to_char ( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
        send_to_char ( " ", ch );
        one_argument ( obj->name, buf );
        send_to_char ( buf, ch );
    }
    send_to_char ( ".\n\r", ch );

    for ( door = 0; door <= 5; door++ )
    {
        EXIT_DATA *pexit;

        if ( ( pexit = location->exit[door] ) != NULL )
        {
            sprintf ( buf,
                      "Door: %d.  To: %ld.  Key: %ld.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
                      door,
                      ( pexit->u1.to_room ==
                        NULL ? -1 : pexit->u1.to_room->vnum ), pexit->key,
                      pexit->exit_info, pexit->keyword,
                      pexit->description[0] !=
                      '\0' ? pexit->description : "(none).\n\r" );
            send_to_char ( buf, ch );
        }
    }

    return;
}

CH_CMD ( do_ostat )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg[MAX_INPUT_LENGTH * 100];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Stat what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "Nothing like that in hell, earth, or heaven.\n\r",
                       ch );
        return;
    }

    sprintf ( buf, "Name(s): %s\n\r", obj->name );
    send_to_char ( buf, ch );

    sprintf ( buf, "Vnum: %ld  Format: %s  Type: %s  Resets: %d\n\r",
              obj->pIndexData->vnum,
              obj->pIndexData->new_format ? "new" : "old",
              item_type_name ( obj ), obj->pIndexData->reset_num );
    send_to_char ( buf, ch );

    sprintf ( buf, "Short description: %s\n\rLong description: %s\n\r",
              obj->short_descr, obj->description );
    send_to_char ( buf, ch );

    sprintf ( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
              wear_bit_name ( obj->wear_flags ),
              extra_bit_name ( obj->extra_flags ) );
    send_to_char ( buf, ch );

    sprintf ( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r", 1,
              get_obj_number ( obj ), obj->weight, get_obj_weight ( obj ),
              get_true_weight ( obj ) );
    send_to_char ( buf, ch );

    sprintf ( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
              obj->level, obj->cost, obj->condition, obj->timer );
    send_to_char ( buf, ch );

    sprintf ( buf,
              "In room: %ld  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
              obj->in_room == NULL ? 0 : obj->in_room->vnum,
              obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,
              obj->carried_by == NULL ? "(none)" : can_see ( ch,
                                                             obj->
                                                             carried_by ) ?
              obj->carried_by->name : "someone", obj->wear_loc );
    send_to_char ( buf, ch );

    sprintf ( buf, "Values: %ld %ld %ld %ld %ld\n\r", obj->value[0],
              obj->value[1], obj->value[2], obj->value[3], obj->value[4] );
    send_to_char ( buf, ch );

    /* now give out vital statistics as per identify */

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf ( buf, "Level %ld spells of:", obj->value[0] );
            send_to_char ( buf, ch );

            if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
            {
                send_to_char ( " '", ch );
                send_to_char ( skill_table[obj->value[1]].name, ch );
                send_to_char ( "'", ch );
            }

            if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
            {
                send_to_char ( " '", ch );
                send_to_char ( skill_table[obj->value[2]].name, ch );
                send_to_char ( "'", ch );
            }

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char ( " '", ch );
                send_to_char ( skill_table[obj->value[3]].name, ch );
                send_to_char ( "'", ch );
            }

            if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
            {
                send_to_char ( " '", ch );
                send_to_char ( skill_table[obj->value[4]].name, ch );
                send_to_char ( "'", ch );
            }

            send_to_char ( ".\n\r", ch );
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf ( buf, "Has %ld(%ld) charges of level %ld", obj->value[1],
                      obj->value[2], obj->value[0] );
            send_to_char ( buf, ch );

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char ( " '", ch );
                send_to_char ( skill_table[obj->value[3]].name, ch );
                send_to_char ( "'", ch );
            }

            send_to_char ( ".\n\r", ch );
            break;

        case ITEM_DRINK_CON:
            sprintf ( buf, "It holds %s-colored %s.\n\r",
                      liq_table[obj->value[2]].liq_color,
                      liq_table[obj->value[2]].liq_name );
            send_to_char ( buf, ch );
            break;

        case ITEM_WEAPON:
            send_to_char ( "Weapon type is ", ch );
            switch ( obj->value[0] )
            {
                case ( WEAPON_EXOTIC ):
                    send_to_char ( "exotic\n\r", ch );
                    break;
                case ( WEAPON_SWORD ):
                    send_to_char ( "sword\n\r", ch );
                    break;
                case ( WEAPON_DAGGER ):
                    send_to_char ( "dagger\n\r", ch );
                    break;
                case ( WEAPON_SPEAR ):
                    send_to_char ( "spear/staff\n\r", ch );
                    break;
                case ( WEAPON_MACE ):
                    send_to_char ( "mace/club\n\r", ch );
                    break;
                case ( WEAPON_AXE ):
                    send_to_char ( "axe\n\r", ch );
                    break;
                case ( WEAPON_FLAIL ):
                    send_to_char ( "flail\n\r", ch );
                    break;
                case ( WEAPON_WHIP ):
                    send_to_char ( "whip\n\r", ch );
                    break;
                case ( WEAPON_POLEARM ):
                    send_to_char ( "polearm\n\r", ch );
                    break;
                default:
                    send_to_char ( "unknown\n\r", ch );
                    break;
            }
            if ( obj->clan )
            {
                sprintf ( buf, "Damage is variable.\n\r" );
            }
            else
            {
                if ( obj->pIndexData->new_format )
                    sprintf ( buf, "Damage is %ldd%ld (average %ld)\n\r",
                              obj->value[1], obj->value[2],
                              ( 1 + obj->value[2] ) * obj->value[1] / 2 );
                else
                    sprintf ( buf, "Damage is %ld to %ld (average %ld)\n\r",
                              obj->value[1], obj->value[2],
                              ( obj->value[1] + obj->value[2] ) / 2 );
            }
            send_to_char ( buf, ch );

            sprintf ( buf, "Damage noun is %s.\n\r",
                      attack_table[obj->value[3]].noun );
            send_to_char ( buf, ch );

            if ( obj->value[4] )    /* weapon flags */
            {
                sprintf ( buf, "Weapons flags: %s\n\r",
                          weapon_bit_name ( obj->value[4] ) );
                send_to_char ( buf, ch );
            }
            break;

        case ITEM_ARMOR:
            if ( obj->clan )
            {
                sprintf ( buf, "Armor class is variable.\n\r" );
            }
            else
            {
                sprintf ( buf,
                          "Armor class is %ld pierce, %ld bash, %ld slash, and %ld vs. magic\n\r",
                          obj->value[0], obj->value[1], obj->value[2],
                          obj->value[3] );
            }
            send_to_char ( buf, ch );
            break;

        case ITEM_CONTAINER:
        case ITEM_PIT:
            sprintf ( buf,
                      "Capacity: %ld#  Maximum weight: %ld#  flags: %s\n\r",
                      obj->value[0], obj->value[3],
                      cont_bit_name ( obj->value[1] ) );
            send_to_char ( buf, ch );
            if ( obj->value[4] != 100 )
            {
                sprintf ( buf, "Weight multiplier: %ld%%\n\r",
                          obj->value[4] );
                send_to_char ( buf, ch );
            }
            break;
    }

    if ( is_clan_obj ( obj ) )
    {
        sprintf ( buf, "This object is owned by the [{%s%s{x] clan.\n\r",
                  clan_table[obj->clan].pkill ? "B" : "M",
                  clan_table[obj->clan].who_name );
        send_to_char ( buf, ch );
    }

    if ( is_class_obj ( obj ) )
    {
        sprintf ( buf, "This object may only be used by a %s.\n\r",
                  class_table[obj->class].name );
        send_to_char ( buf, ch );
    }

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char ( "Extra description keywords: '", ch );

        for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char ( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char ( " ", ch );
        }

        for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char ( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char ( " ", ch );
        }

        send_to_char ( "'\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        sprintf ( buf, "Affects %s by %d, level %d",
                  affect_loc_name ( paf->location ), paf->modifier,
                  paf->level );
        send_to_char ( buf, ch );
        if ( paf->duration > -1 )
            sprintf ( buf, ", %d hours.\n\r", paf->duration );
        else
            sprintf ( buf, ".\n\r" );
        send_to_char ( buf, ch );
        if ( paf->bitvector )
        {
            switch ( paf->where )
            {
                case TO_AFFECTS:
                    sprintf ( buf, "Adds %s affect.\n",
                              affect_bit_name ( paf->bitvector ) );
                    break;
                case TO_WEAPON:
                    sprintf ( buf, "Adds %s weapon flags.\n",
                              weapon_bit_name ( paf->bitvector ) );
                    break;
                case TO_OBJECT:
                    sprintf ( buf, "Adds %s object flag.\n",
                              extra_bit_name ( paf->bitvector ) );
                    break;
                case TO_IMMUNE:
                    sprintf ( buf, "Adds immunity to %s.\n",
                              imm_bit_name ( paf->bitvector ) );
                    break;
                case TO_RESIST:
                    sprintf ( buf, "Adds resistance to %s.\n\r",
                              imm_bit_name ( paf->bitvector ) );
                    break;
                case TO_VULN:
                    sprintf ( buf, "Adds vulnerability to %s.\n\r",
                              imm_bit_name ( paf->bitvector ) );
                    break;
                case TO_SHIELDS:
                    sprintf ( buf, "Adds %s shield.\n",
                              shield_bit_name ( paf->bitvector ) );
                    break;
                default:
                    sprintf ( buf, "Unknown bit %d: %ld\n\r", paf->where,
                              paf->bitvector );
                    break;
            }
            send_to_char ( buf, ch );
        }
    }

    if ( !obj->enchanted )
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
            sprintf ( buf, "Affects %s by %d, level %d.\n\r",
                      affect_loc_name ( paf->location ), paf->modifier,
                      paf->level );
            send_to_char ( buf, ch );
            if ( paf->bitvector )
            {
                switch ( paf->where )
                {
                    case TO_AFFECTS:
                        sprintf ( buf, "Adds %s affect.\n",
                                  affect_bit_name ( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        sprintf ( buf, "Adds %s object flag.\n",
                                  extra_bit_name ( paf->bitvector ) );
                        break;
                    case TO_IMMUNE:
                        sprintf ( buf, "Adds immunity to %s.\n",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_RESIST:
                        sprintf ( buf, "Adds resistance to %s.\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_VULN:
                        sprintf ( buf, "Adds vulnerability to %s.\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_SHIELDS:
                        sprintf ( buf, "Adds %s shield.\n",
                                  shield_bit_name ( paf->bitvector ) );
                        break;
                    default:
                        sprintf ( buf, "Unknown bit %d: %ld\n\r", paf->where,
                                  paf->bitvector );
                        break;
                }
                send_to_char ( buf, ch );
            }
        }

    return;
}

CH_CMD ( do_mstat )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg[MAX_INPUT_LENGTH * 100];
//    BUFFER *output;
    BUFFER2 *output;
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Stat whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

//    output = new_buf (  );
    output = buffer_new ( 2048 );

    if ( !IS_NPC ( victim ) )
    {
/*        sprintf ( buf, "Name: %s\n\rSocket: %s\n\r", victim->name,
                  victim->pcdata->socket ); */
        sprintf ( buf, "Name: %s\n\r", victim->name ); 
    }
    else
    {
        sprintf ( buf, "Name: %s\n\r", victim->name );
    }
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    sprintf ( buf,
              "Vnum: %ld  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %ld\n\r",
              IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
              IS_NPC ( victim ) ? victim->pIndexData->
              new_format ? "new" : "old" : "pc",
              race_table[victim->race].name,
              IS_NPC ( victim ) ? victim->group : 0,
              sex_table[victim->sex].name,
              victim->in_room == NULL ? 0 : victim->in_room->vnum );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( IS_NPC ( victim ) )
    {
        sprintf ( buf, "Count: %d  Killed: %d  Kills: %d\n\r", victim->pIndexData->count,
                  victim->pIndexData->killed, victim->pIndexData->kills );
//        add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }

    sprintf ( buf,
              "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
              victim->perm_stat[STAT_STR], get_curr_stat ( victim, STAT_STR ),
              victim->perm_stat[STAT_INT], get_curr_stat ( victim, STAT_INT ),
              victim->perm_stat[STAT_WIS], get_curr_stat ( victim, STAT_WIS ),
              victim->perm_stat[STAT_DEX], get_curr_stat ( victim, STAT_DEX ),
              victim->perm_stat[STAT_CON], get_curr_stat ( victim,
                                                           STAT_CON ) );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    sprintf ( buf,
              "Hp: %ld/%ld  Mana: %ld/%ld  Move: %ld/%ld  Practices: %d\n\r",
              victim->hit, victim->max_hit, victim->mana, victim->max_mana,
              victim->move, victim->max_move,
              IS_NPC ( ch ) ? 0 : victim->practice );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    sprintf ( buf, "Lv: %d  Class: %s  Align: %d  Exp: %ld\n\r",
              victim->level,
              IS_NPC ( victim ) ? "mobile" : class_table[victim->class].name,
              victim->alignment, victim->exp );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );


    sprintf ( buf, "Platinum: %ld  Gold: %ld  Silver: %ld\n\r",
              victim->platinum, victim->gold, victim->silver );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );


    sprintf ( buf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
              GET_AC ( victim, AC_PIERCE ), GET_AC ( victim, AC_BASH ),
              GET_AC ( victim, AC_SLASH ), GET_AC ( victim, AC_EXOTIC ) );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );


    sprintf ( buf,
              "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
              GET_HITROLL ( victim ), GET_DAMROLL ( victim ),
              victim->saving_throw, size_table[victim->size].name,
              position_table[victim->position].name, victim->wimpy );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );


    if ( IS_NPC ( victim ) && victim->pIndexData->new_format )
    {
        sprintf ( buf, "Damage: %dd%d  Message:  %s\n\r",
                  victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE],
                  attack_table[victim->dam_type].noun );
//        add_buf ( output, buf );
          buffer_strcat ( output, buf );
    }
    sprintf ( buf, "Fighting: %s\n\r",
              victim->fighting ? victim->fighting->name : "(none)" );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( !IS_NPC ( victim ) )
    {
        sprintf ( buf,
                  "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d  Quest: %d\n\r",
                  victim->pcdata->condition[COND_THIRST],
                  victim->pcdata->condition[COND_HUNGER],
                  victim->pcdata->condition[COND_FULL],
                  victim->pcdata->condition[COND_DRUNK], victim->qps );
        //add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    sprintf ( buf, "Carry number: %d  Carry weight: %ld\n\r",
              victim->carry_number, get_carry_weight ( victim ) / 10 );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( !IS_NPC ( victim ) )
    {
        sprintf ( buf, "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
                  get_age ( victim ),
                  ( int ) ( victim->played + current_time -
                            victim->logon ) / 3600,
                  victim->pcdata->last_level, victim->timer );
//        add_buf ( output, buf );
          buffer_strcat ( output, buf );
    }

    sprintf ( buf, "Act: %s\n\r", act_bit_name ( victim->act ) );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( victim->act2 )
    {
        sprintf ( buf, "Act2: %s\n\r", act2_bit_name ( victim->act2 ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( victim->comm )
    {
        sprintf ( buf, "Comm: %s\n\r", comm_bit_name ( victim->comm ) );
    //    add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( IS_NPC ( victim ) && victim->off_flags )
    {
        sprintf ( buf, "Offense: %s\n\r",
                  off_bit_name ( victim->off_flags ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( victim->imm_flags )
    {
        sprintf ( buf, "Immune: %s\n\r", imm_bit_name ( victim->imm_flags ) );
    //    add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( victim->res_flags )
    {
        sprintf ( buf, "Resist: %s\n\r", imm_bit_name ( victim->res_flags ) );
        //add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( victim->vuln_flags )
    {
        sprintf ( buf, "Vulnerable: %s\n\r",
                  imm_bit_name ( victim->vuln_flags ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    sprintf ( buf, "Form: %s\n\rParts: %s\n\r",
              form_bit_name ( victim->form ),
              part_bit_name ( victim->parts ) );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( victim->affected_by )
    {
        sprintf ( buf, "Affected by %s\n\r",
                  affect_bit_name ( victim->affected_by ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    if ( victim->shielded_by )
    {
        sprintf ( buf, "Shielded by %s\n\r",
                  shield_bit_name ( victim->shielded_by ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    sprintf ( buf, "Master: %s  Leader: %s  Pet: %s\n\r",
              victim->master ? victim->master->name : "(none)",
              victim->leader ? victim->leader->name : "(none)",
              victim->pet ? victim->pet->name : "(none)" );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );


    if ( !IS_NPC ( victim ) )
    {
        sprintf ( buf, "Security: %d.\n\r", victim->pcdata->security ); /* OLC */
        send_to_char ( buf, ch );   /* OLC */
    }

    sprintf ( buf, "Short description: %s\n\rLong  description: %s",
              victim->short_descr,
              victim->long_descr[0] !=
              '\0' ? victim->long_descr : "(none)\n\r" );
//    add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( IS_NPC ( victim ) && victim->spec_fun != 0 )
    {
        sprintf ( buf, "Mobile has special procedure %s.\n\r",
                  spec_name ( victim->spec_fun ) );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
        sprintf ( buf,
                  "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
                  skill_table[( int ) paf->type].name,
                  affect_loc_name ( paf->location ), paf->modifier,
                  paf->duration,
                  paf->where ==
                  TO_SHIELDS ? shield_bit_name ( paf->
                                                 bitvector ) :
                  affect_bit_name ( paf->bitvector ), paf->level );
//        add_buf ( output, buf );
        buffer_strcat ( output, buf );
    }
    if ( IS_SET ( ch->act, PLR_QUESTOR ) )
    {
        if ( ch->pcdata->questmob == -1 )
            printf_to_char ( ch, "{g%s's Quest is {WALMOST{g complete.{x\n\r",
                             ch->name );
        else if ( ch->pcdata->questobj > 0 )
        {
            questinfoobj = get_obj_index ( ch->pcdata->questobj );
            if ( questinfoobj != NULL )
            {
                printf_to_char ( ch,
                                 "{gThey are on a quest to recover {W%s{g.{x\n\r",
                                 questinfoobj->name );
            }
            else
                send_to_char ( "{gThey aren't currently on a quest.{x\n\r",
                               ch );
        }
        else if ( ch->pcdata->questmob > 0 )
        {
            questinfo = get_mob_index ( ch->pcdata->questmob );
            if ( questinfo != NULL )
            {
                printf_to_char ( ch,
                                 "{gThey are on a quest to slay %s{g.{x\n\r",
                                 questinfo->short_descr );
            }
            else
                send_to_char ( "{gThey aren't currently on a quest.{x\n\r",
                               ch );
        }
    }

//    page_to_char ( buf_string ( output ), ch );
    page_to_char ( buffer_string ( output ), ch );
    //free_buf ( output );
    buffer_free ( output );
    return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */

CH_CMD ( do_vnum )
{
    char arg[MAX_INPUT_LENGTH * 100];
    char buf[MAX_STRING_LENGTH * 100];
//    BUFFER *output;
    BUFFER2 *output;
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;
    char *string;

    string = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  vnum obj <name>\n\r", ch );
        send_to_char ( "  vnum mob <name>\n\r", ch );
        send_to_char ( "  vnum skill <skill or spell>\n\r", ch );
        send_to_char ( "  vnum areas\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "obj" ) )
    {
        do_ofind ( ch, string );
        return;
    }

    if ( !str_cmp ( arg, "mob" ) || !str_cmp ( arg, "char" ) )
    {
        do_mfind ( ch, string );
        return;
    }

    if ( !str_cmp ( arg, "skill" ) || !str_cmp ( arg, "spell" ) )
    {
        do_slookup ( ch, string );
        return;
    }

    if ( !str_cmp ( arg, "areas" ) || !str_cmp ( arg, "area" ) )
    {
//        output = new_buf (  );
        output = buffer_new ( 1337 );
        iAreaHalf = ( top_area + 1 ) / 2;
        pArea1 = area_first;
        pArea2 = area_first;
        for ( iArea = 0; iArea < iAreaHalf; iArea++ )
            pArea2 = pArea2->next;

        for ( iArea = 0; iArea < iAreaHalf; iArea++ )
        {
            sprintf ( buf,
                      "{g%-20s {w[{R%-7ld{w-{R%-7ld{w]{g  %-20s {w[{R%-7ld{w-{R%-7ld{w]{x\n\r",
                      pArea1->name, pArea1->min_vnum, pArea1->max_vnum,
                      ( pArea2 != NULL ) ? pArea2->name : "",
                      ( pArea2 != NULL ) ? pArea2->min_vnum : 0,
                      ( pArea2 != NULL ) ? pArea2->max_vnum : 0 );
//            add_buf ( output, buf );
            buffer_strcat ( output, buf );
            pArea1 = pArea1->next;
            if ( pArea2 != NULL )
                pArea2 = pArea2->next;
        }
//        page_to_char ( buf_string ( output ), ch );
        page_to_char ( buffer_string ( output ), ch );
//        free_buf ( output );
        buffer_free ( output );
        return;
    }

    /* do both */
    do_mfind ( ch, argument );
    do_ofind ( ch, argument );
}

CH_CMD ( do_mfind )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg[MAX_INPUT_LENGTH * 100];
//    BUFFER *output;
    BUFFER2 *output;
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Find whom?\n\r", ch );
        return;
    }

    fAll = FALSE;               /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;
    //output = new_buf (  );
    output = buffer_new ( 1337 );

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
        if ( ( pMobIndex = get_mob_index ( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || is_name ( argument, pMobIndex->player_name ) )
            {
                found = TRUE;
                sprintf ( buf, "[%6ld] %s\n\r", pMobIndex->vnum,
                          pMobIndex->short_descr );
//                add_buf ( output, buf );
		buffer_strcat ( output, buf );
            }
        }
    }

    if ( !found )
    {
        send_to_char ( "No mobiles by that name.\n\r", ch );
    }
    else
    {
//        page_to_char ( buf_string ( output ), ch );
        page_to_char ( buffer_string ( output ), ch );
    }
//    free_buf ( output );
    buffer_free ( output );
    return;
}

CH_CMD ( do_ofind )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg[MAX_INPUT_LENGTH * 100];
//    BUFFER *output;
    BUFFER2 *output;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Find what?\n\r", ch );
        return;
    }

    fAll = FALSE;               /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;
//    output = new_buf (  );
    output = buffer_new ( 1337 );

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index ( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || is_name ( argument, pObjIndex->name ) )
            {
                found = TRUE;
                sprintf ( buf, "[%6ld] %s\n\r", pObjIndex->vnum,
                          pObjIndex->short_descr );
//                add_buf ( output, buf );
		buffer_strcat ( output, buf );
            }
        }
    }

    if ( !found )
    {
        send_to_char ( "No objects by that name.\n\r", ch );
    }
    else
    {
//        page_to_char ( buf_string ( output ), ch );
        page_to_char ( buffer_string ( output ), ch );
    }
//    free_buf ( output );
    buffer_free ( output );
    return;
}

CH_CMD ( do_owhere )
{
    char buf[MAX_INPUT_LENGTH * 100];
//    BUFFER *buffer;
    BUFFER2 *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0;

    found = FALSE;
    number = 0;
    /*max_found = 50; */

//    buffer = new_buf (  );
    buffer = buffer_new ( 1337 );

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Find what?\n\r", ch );
        return;
    }

    if ( strlen ( argument ) < 2 )
    {
        send_to_char ( "Two characters minimum.\n\r", ch );
        return;
    }
        
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( !can_see_obj ( ch, obj ) || !is_name ( argument, obj->name ) ||
             ch->level < obj->level || ( obj->carried_by != NULL &&
                                         !can_see ( ch, obj->carried_by ) ) )
            continue;

        found = TRUE;
        number++;

        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
            ;

        if ( in_obj->carried_by != NULL && can_see ( ch, in_obj->carried_by )
             && in_obj->carried_by->in_room != NULL )
            sprintf ( buf, "%3d) %s is carried by %s [Room %ld]\n\r", number,
                      obj->short_descr, PERS ( in_obj->carried_by, ch ),
                      in_obj->carried_by->in_room->vnum );
        else if ( in_obj->in_room != NULL &&
                  can_see_room ( ch, in_obj->in_room ) )
            sprintf ( buf, "%3d) %s is in %s [Room %ld]\n\r", number,
                      obj->short_descr, in_obj->in_room->name,
                      in_obj->in_room->vnum );
        else
/*            sprintf ( buf, "%3d) %s is somewhere\n\r", number,
                      obj->short_descr ); */

        buf[0] = UPPER ( buf[0] );
//        add_buf ( buffer, buf );
        buffer_strcat ( buffer, buf );

/*        if ( number >= max_found )
	{
	    sprintf ( buf, "\n\rMaximum amount of objects found. Try specifying your search more thoroughly.\n\r" );
            buffer_strcat ( buffer, buf );
            break;
	} */
    }

    if ( !found )
        send_to_char ( "Nothing like that in heaven or earth.\n\r", ch );
    else
//        page_to_char ( buf_string ( buffer ), ch );
	page_to_char ( buffer_string ( buffer ), ch );

//    free_buf ( buffer );
    buffer_free ( buffer );
}

CH_CMD ( do_mwhere )
{
    char buf[MAX_STRING_LENGTH * 100];
//    BUFFER *buffer;
    BUFFER2 *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;
        
    if ( argument[0] == '\0' )
    {
        DESCRIPTOR_DATA *d;

        /* show characters logged */

//        buffer = new_buf (  );
        buffer = buffer_new ( 1337 );
	for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->character != NULL && d->connected == CON_PLAYING &&
                 d->character->in_room != NULL && can_see ( ch, d->character )
                 && can_see_room ( ch, d->character->in_room ) )
            {
                victim = d->character;
                if ( ( victim->level <= CREATOR && ch->level <= CREATOR ) ||
                     ch->level > CREATOR )
                {
                    count++;
                    if ( d->original != NULL )
                        sprintf ( buf,
                                  "%3d) %s (in the body of %s) is in %s [%ld]\n\r",
                                  count, d->original->name,
                                  victim->short_descr, victim->in_room->name,
                                  victim->in_room->vnum );
                    else
                        sprintf ( buf, "%3d) %s is in %s [%ld]\n\r", count,
                                  victim->name, victim->in_room->name,
                                  victim->in_room->vnum );
//                    add_buf ( buffer, buf );
		    buffer_strcat ( buffer, buf );
                }
/*		if ( count >= 50 )
		{
		   sprintf ( buf, "That is all you can locate at once. Try defining your search more thoroughly.\n\r" );
		   add_buf ( buffer, buf );
		   break;
		} */
            }
        }

//        page_to_char ( buf_string ( buffer ), ch );
        page_to_char ( buffer_string ( buffer ), ch );
//        free_buf ( buffer );
        buffer_free ( buffer );
        return;
    }

    if ( strlen ( argument ) < 2 )
    {
        send_to_char ( "Two characters minimum.\n\r", ch );
        return;
    }
    
    found = FALSE;
//    buffer = new_buf (  );
    buffer = buffer_new ( 1337 );
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( victim->in_room != NULL && is_name ( argument, victim->name ) )
        {
            if ( ( victim->level <= CREATOR && ch->level <= CREATOR ) ||
                 ch->level > CREATOR )
            {
                found = TRUE;
                count++;
                sprintf ( buf, "%3d) [%ld] %-28s [%ld] %s\n\r", count,
                          IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
                          IS_NPC ( victim ) ? victim->short_descr : victim->
                          name, victim->in_room->vnum,
                          victim->in_room->name );
//                add_buf ( buffer, buf );
		buffer_strcat ( buffer, buf );
            }
        }
    }

    if ( !found )
        act ( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
    else
//        page_to_char ( buf_string ( buffer ), ch );
        page_to_char ( buffer_string ( buffer ), ch );

//    free_buf ( buffer );
    buffer_free ( buffer );

    return;
}

CH_CMD ( do_reboo )
{
    send_to_char ( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}

/*void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d,*d_next;

    if (ch->invis_level < LEVEL_ANCIENT)
    {
    	sprintf( buf, "Reboot by %s.", ch->name );
    	do_echo( ch, buf );
    }
    do_force ( ch, "all save");
    do_save (ch, "");
    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
    	close_socket(d);
    }
    
    return;
}*/

/*CH_CMD ( do_reboot )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Usage: reboot now\n\r", ch );
        send_to_char ( "Usage: reboot <ticks to reboot>\n\r", ch );
        send_to_char ( "Usage: reboot cancel\n\r", ch );
        send_to_char ( "Usage: reboot status\n\r", ch );
        return;
    }

    if ( is_name ( arg, "cancel" ) )
    {
        reboot_counter = -1;
        send_to_char ( "Reboot canceled.\n\r", ch );
        return;
    }

    if ( is_name ( arg, "now" ) )
    {
        reboot_rot (  );
        return;
    }

    if ( is_name ( arg, "status" ) )
    {
        if ( reboot_counter == -1 )
            sprintf ( buf, "Automatic rebooting is inactive.\n\r" );
        else
            sprintf ( buf, "Reboot in %i minutes.\n\r", reboot_counter );
        send_to_char ( buf, ch );
        return;
    }

    if ( is_number ( arg ) )
    {
        reboot_counter = atoi ( arg );
        sprintf ( buf, "Shattered Dreams will reboot in %i ticks.\n\r", reboot_counter );
        send_to_char ( buf, ch );
        return;
    }

    do_reboot ( ch, "" );
}
*/
void reboot_rot ( void )
{
    DESCRIPTOR_DATA *d, *d_next;

    // save_gquest_data();
    sprintf ( log_buf, "Rebooting SD." );
    log_string ( log_buf );
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next = d->next;
        write_to_buffer ( d, "Shattered Dreams is now going down for a reboot.",
                          0 );
        if ( d->character != NULL )
            save_char_obj ( d->character );
        close_socket ( d );
    }
    merc_down = TRUE;
    return;
}

CH_CMD ( do_shutdow )
{
    send_to_char ( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}

CH_CMD ( do_shutdown )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d, *d_next;

    if ( ch->invis_level < LEVEL_ANCIENT )
        sprintf ( buf, "Shutdown by %s.", ch->name );
    append_file ( ch, SHUTDOWN_FILE, buf );
    strcat ( buf, "\n\r" );
    if ( ch->invis_level < LEVEL_ANCIENT )
        do_echo ( ch, buf );
    do_force ( ch, "all save" );
    do_save ( ch, "" );
    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next = d->next;
        close_socket ( d );
    }
    return;
}
/*
CH_CMD ( do_protect )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Protect whom from snooping?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "You can't find them.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_SNOOP_PROOF ) )
    {
        act_new ( "$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR,
                  POS_DEAD );
        send_to_char ( "Your snoop-proofing was just removed.\n\r", victim );
        REMOVE_BIT ( victim->comm, COMM_SNOOP_PROOF );
    }
    else
    {
        act_new ( "$N is now snoop-proof.", ch, NULL, victim, TO_CHAR,
                  POS_DEAD );
        send_to_char ( "You are now immune to snooping.\n\r", victim );
        SET_BIT ( victim->comm, COMM_SNOOP_PROOF );
    }
}
*/
CH_CMD ( do_snoop )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Snoop whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL )
    {
        send_to_char ( "No descriptor to snoop.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "Cancelling all snoops.\n\r", ch );
        if ( !IS_TRUSTED ( ch, HEADIMP ) )
        {
            wiznet ( "$N stops being such a snoop.", ch, NULL, WIZ_SNOOPS,
                     WIZ_SECURE, get_trust ( ch ) );
        }
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->snoop_by == ch->desc )
                d->snoop_by = NULL;
        }
        return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
        send_to_char ( "Busy already.\n\r", ch );
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

    if ( get_trust ( victim ) >= get_trust ( ch ) &&
           !IS_TRUSTED ( ch, HEADIMP ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( ch->desc != NULL )
    {
        for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
        {
            if ( d->character == victim || d->original == victim )
            {
                send_to_char ( "No snoop loops.\n\r", ch );
                return;
            }
        }
    }

    victim->desc->snoop_by = ch->desc;
    if ( !IS_TRUSTED ( ch, HEADIMP ) )
    {
        sprintf ( buf, "$N starts snooping on %s",
                  ( IS_NPC ( ch ) ? victim->short_descr : victim->name ) );
        wiznet ( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust ( ch ) );
    }
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_switch )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Switch into whom?\n\r", ch );
        return;
    }

    if ( ch->desc == NULL )
        return;

    if ( ch->desc->original != NULL )
    {
        send_to_char ( "You are already switched.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "Ok.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) )
    {
        send_to_char ( "You can only switch into mobiles.\n\r", ch );
        return;
    }

    if ( victim->level > ch->level )
    {
        send_to_char
            ( "That character is too powerful for you to handle.\n\r", ch );
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

    if ( victim->desc != NULL )
    {
        send_to_char ( "Character in use.\n\r", ch );
        return;
    }

    sprintf ( buf, "$N switches into %s", victim->short_descr );
    wiznet ( buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust ( ch ) );

    ch->desc->character = victim;
    ch->desc->original = ch;
    victim->desc = ch->desc;
    ch->desc = NULL;
    /* change communications to match */
    if ( ch->prompt != NULL )
        victim->prompt = str_dup ( ch->prompt );
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    send_to_char ( "Ok.\n\r", victim );
    return;
}

CH_CMD ( do_return )
{
    char buf[MAX_STRING_LENGTH];

    if ( ch->desc == NULL )
        return;

    if ( ch->desc->original == NULL )
    {
        send_to_char ( "You aren't switched.\n\r", ch );
        return;
    }

    send_to_char
        ( "You return to your original body. Type replay to see any missed tells.\n\r",
          ch );
    if ( ch->prompt != NULL )
    {
        free_string ( ch->prompt );
        ch->prompt = NULL;
    }

    sprintf ( buf, "$N returns from %s.", ch->short_descr );
    wiznet ( buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE,
             get_trust ( ch ) );
    ch->desc->character = ch->desc->original;
    ch->desc->original = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc = NULL;
    return;
}

/* trust levels for load and clone */
bool obj_check ( CHAR_DATA * ch, OBJ_DATA * obj )
{
    if ( IS_TRUSTED ( ch, GOD ) ||
         ( IS_TRUSTED ( ch, IMMORTAL ) && obj->level <= 105 ) ||
         ( IS_TRUSTED ( ch, DEMI ) && obj->level <= 100 ) ||
         ( IS_TRUSTED ( ch, KNIGHT ) && obj->level <= 20 ) ||
         ( IS_TRUSTED ( ch, SQUIRE ) && obj->level == 5 ) )
        return TRUE;
    else
        return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone ( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * clone )
{
    OBJ_DATA *c_obj, *t_obj;

    for ( c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content )
    {
        if ( obj_check ( ch, c_obj ) )
        {
            t_obj = create_object ( c_obj->pIndexData, 0 );
            clone_object ( c_obj, t_obj );
            obj_to_obj ( t_obj, clone );
            recursive_clone ( ch, c_obj, t_obj );
        }
    }
}

/* command that is similar to load */
CH_CMD ( do_clone )
{
    char arg[MAX_INPUT_LENGTH];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA *obj;

    rest = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Clone what?\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg, "object" ) )
    {
        mob = NULL;
        obj = get_obj_here ( ch, rest );
        if ( obj == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
    }
    else if ( !str_prefix ( arg, "mobile" ) ||
              !str_prefix ( arg, "character" ) )
    {
        obj = NULL;
        mob = get_char_room ( ch, rest );
        if ( mob == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
    }
    else                        /* find both */
    {
        mob = get_char_room ( ch, argument );
        obj = get_obj_here ( ch, argument );
        if ( mob == NULL && obj == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
    }

    /* clone an object */
    if ( obj != NULL )
    {
        OBJ_DATA *clone;
/* 
        if ( !obj_check ( ch, obj ) )
        {
            send_to_char
                ( "Your powers are not great enough for such a task.\n\r",
                  ch );
            return;
        }
*/
        if ( obj->item_type == ITEM_EXIT )
        {
            send_to_char ( "You cannot clone an exit object.\n\r", ch );
            return;
        }
        clone = create_object ( obj->pIndexData, 0 );
        clone_object ( obj, clone );
        if ( obj->carried_by != NULL )
            obj_to_char ( clone, ch );
        else
            obj_to_room ( clone, ch->in_room );
        recursive_clone ( ch, obj, clone );

        act ( "$n has created $p.", ch, clone, NULL, TO_ROOM );
        act ( "You clone $p.", ch, clone, NULL, TO_CHAR );
        wiznet ( "$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE,
                 get_trust ( ch ) );
        return;
    }
    else if ( mob != NULL )
    {
        CHAR_DATA *clone;
        OBJ_DATA *new_obj;
        char buf[MAX_STRING_LENGTH];

        if ( !IS_NPC ( mob ) )
        {
            send_to_char ( "You can only clone mobiles.\n\r", ch );
            return;
        }

        if ( ( mob->level > 100 && !IS_TRUSTED ( ch, GOD ) ) ||
             ( mob->level > 90 && !IS_TRUSTED ( ch, IMMORTAL ) ) ||
             ( mob->level > 85 && !IS_TRUSTED ( ch, DEMI ) ) ||
             ( mob->level > 0 && !IS_TRUSTED ( ch, KNIGHT ) ) ||
             !IS_TRUSTED ( ch, SQUIRE ) )
        {
            send_to_char
                ( "Your powers are not great enough for such a task.\n\r",
                  ch );
            return;
        }

        clone = create_mobile ( mob->pIndexData );
        clone_mobile ( mob, clone );

        for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
        {
            if ( obj_check ( ch, obj ) )
            {
                new_obj = create_object ( obj->pIndexData, 0 );
                clone_object ( obj, new_obj );
                recursive_clone ( ch, obj, new_obj );
                obj_to_char ( new_obj, clone );
                new_obj->wear_loc = obj->wear_loc;
            }
        }
        char_to_room ( clone, ch->in_room );
        act ( "$n has created $N.", ch, NULL, clone, TO_ROOM );
        act ( "You clone $N.", ch, NULL, clone, TO_CHAR );
        sprintf ( buf, "$N clones %s.", clone->short_descr );
        wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
        return;
    }
}


/* RT to replace the two load commands */

CH_CMD ( do_load )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  load mob <vnum>\n\r", ch );
        send_to_char ( "  load obj <vnum> <level>\n\r", ch );
        if ( ch->level >= CREATOR )
            send_to_char ( "  load voodoo <player>\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "mob" ) || !str_cmp ( arg, "char" ) )
    {
        do_mload ( ch, argument );
        return;
    }

    if ( !str_cmp ( arg, "obj" ) )
    {
        do_oload ( ch, argument );
        return;
    }

    if ( !str_cmp ( arg, "voodoo" ) && ( ch->level >= CREATOR ) )
    {
        do_vload ( ch, argument );
        return;
    }
    /* echo syntax */
    do_load ( ch, "" );
}

CH_CMD ( do_mload )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument ( argument, arg );

    if ( arg[0] == '\0' || !is_number ( arg ) )
    {
        send_to_char ( "Syntax: load mob <vnum>.\n\r", ch );
        return;
    }

    if ( ( pMobIndex = get_mob_index ( atol ( arg ) ) ) == NULL )
    {
        send_to_char ( "No mob has that vnum.\n\r", ch );
        return;
    }

    victim = create_mobile ( pMobIndex );
    char_to_room ( victim, ch->in_room );
    act ( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    sprintf ( buf, "$N loads %s.", victim->short_descr );
    wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_oload )
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;

    argument = one_argument ( argument, arg1 );
    one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number ( arg1 ) )
    {
        send_to_char ( "Syntax: load obj <vnum> <level>.\n\r", ch );
        return;
    }

    level = get_trust ( ch );   /* default */

    if ( arg2[0] != '\0' )      /* load with a level */
    {
        if ( !is_number ( arg2 ) )
        {
            send_to_char ( "Syntax: oload <vnum> <level>.\n\r", ch );
            return;
        }
        level = atoi ( arg2 );
        if ( level < 0 || level > get_trust ( ch ) )
        {
            send_to_char ( "Level must be be between 0 and your level.\n\r",
                           ch );
            return;
        }
    }

    if ( ( pObjIndex = get_obj_index ( atol ( arg1 ) ) ) == NULL )
    {
        send_to_char ( "No object has that vnum.\n\r", ch );
        return;
    }
    if ( pObjIndex->item_type == ITEM_EXIT )
    {
        send_to_char ( "You cannot load an exit object.\n\r", ch );
        return;
    }

    obj = create_object ( pObjIndex, level );
    if ( CAN_WEAR ( obj, ITEM_TAKE ) )
        obj_to_char ( obj, ch );
    else
        obj_to_room ( obj, ch->in_room );
    act ( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    wiznet ( "$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE,
             get_trust ( ch ) );
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_vload )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    char *name;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: load voodoo <player>\n\r", ch );
        return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( !str_prefix ( arg1, wch->name ) && !found )
        {
            if ( IS_NPC ( wch ) )
                continue;

            if ( wch->level > ch->level )
                continue;

            found = TRUE;

            if ( ( pObjIndex = get_obj_index ( OBJ_VNUM_VOODOO ) ) == NULL )
            {
                send_to_char ( "Cannot find the voodoo doll vnum.\n\r", ch );
                return;
            }
            obj = create_object ( pObjIndex, 0 );
            name = wch->name;
            sprintf ( buf, obj->short_descr, name );
            free_string ( obj->short_descr );
            obj->short_descr = str_dup ( buf );
            sprintf ( buf, obj->description, name );
            free_string ( obj->description );
            obj->description = str_dup ( buf );
            sprintf ( buf, obj->name, name );
            free_string ( obj->name );
            obj->name = str_dup ( buf );
            if ( CAN_WEAR ( obj, ITEM_TAKE ) )
                obj_to_char ( obj, ch );
            else
                obj_to_room ( obj, ch->in_room );
            act ( "$n has created $p!", ch, obj, NULL, TO_ROOM );
            wiznet ( "$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE,
                     get_trust ( ch ) );
            send_to_char ( "Ok.\n\r", ch );
            return;
        }
    }
    send_to_char ( "No one of that name is playing.\n\r", ch );
    return;
}
/*
CH_CMD ( do_randclan )
{
    randomize_entrances ( ROOM_VNUM_CLANS );
    send_to_char ( "Clan entrances have been moved.\n\r", ch );
    return;
}
*/
CH_CMD ( do_purge )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA *obj_next;

        for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
        {
            vnext = victim->next_in_room;
            if ( IS_NPC ( victim ) && !IS_SET ( victim->act, ACT_NOPURGE ) &&
                 victim != ch /* safety precaution */  )
                extract_char ( victim, TRUE );
        }

        for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( !IS_OBJ_STAT ( obj, ITEM_NOPURGE ) )
                extract_obj ( obj );
        }

        act ( "$n purges the room!", ch, NULL, NULL, TO_ROOM );
        send_to_char ( "Ok.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) )
    {

        if ( ch == victim )
        {
            send_to_char ( "Ho ho ho.\n\r", ch );
            return;
        }

        if ( get_trust ( ch ) <= get_trust ( victim ) )
        {
            send_to_char ( "Maybe that wasn't a good idea...\n\r", ch );
            sprintf ( buf, "%s tried to purge you!\n\r", ch->name );
            send_to_char ( buf, victim );
            return;
        }

        if ( get_trust ( ch ) <= DEITY )
        {
            send_to_char ( "Not against PC's!\n\r", ch );
            return;
        }

        act ( "$n disintegrates $N.", ch, 0, victim, TO_NOTVICT );

        if ( victim->level > 1 )
            save_char_obj ( victim );
        d = victim->desc;
        extract_char ( victim, TRUE );
        if ( d != NULL )
            close_socket ( d );

        return;
    }

    act ( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char ( victim, TRUE );
    return;
}

CH_CMD ( do_advance )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) )
    {
        send_to_char ( "Syntax: advance <char> <level>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "That player is not here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ( level = atoi ( arg2 ) ) < 1 || level > 216 )
    {
        send_to_char ( "Level must be 1 to 216.\n\r", ch );
        return;
    }

    if ( level > get_trust ( ch ) )
    {
        send_to_char ( "Limited to your trust level.\n\r", ch );
        return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
        int temp_prac;

        send_to_char ( "Lowering a player's level!\n\r", ch );
        send_to_char
            ( "{R******** {GOOOOHHHHHHHHHH  NNNNOOOO {R*******{x\n\r",
              victim );
        sprintf ( buf,
                  "{R**** {WYou've been demoted to level %d {R****{x\n\r",
                  level );
        send_to_char ( buf, victim );
        if ( ( victim->level > HERO ) || ( level > HERO ) )
        {
            update_wizlist ( victim, level );
            //update_anclist ( victim, level );
        }
        temp_prac = victim->practice;
        victim->level = 1;
        victim->exp = exp_per_level ( victim, victim->pcdata->points );
        victim->max_hit = 100;
        victim->max_mana = 100;
        victim->max_move = 100;
        victim->practice = 0;
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;
        advance_level_quiet ( victim );
        victim->practice = temp_prac;
    }
    else
    {
        send_to_char ( "Raising a player's level!\n\r", ch );
        send_to_char
            ( "{B******* {GOOOOHHHHHHHHHH  YYYYEEEESSS {B******{x\n\r",
              victim );
        sprintf ( buf,
                  "{B**** {WYou've been advanced to level %d {B****{x\n\r",
                  level );
        send_to_char ( buf, victim );
        if ( ( victim->level > HERO ) || ( level > HERO ) )
        {
            update_wizlist ( victim, level );
      //      update_anclist ( victim, level );
        }
    }

    for ( iLevel = victim->level; iLevel < level; iLevel++ )
    {
        victim->level += 1;
        advance_level_quiet ( victim );
    }
    victim->exp =
        exp_per_level ( victim, victim->pcdata->points ) * UMAX ( 1,
                                                                  victim->
                                                                  level );
    victim->trust = 0;
    save_char_obj ( victim );
    return;
}

CH_CMD ( do_knight )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument ( argument, arg1 );

    /* if (!IS_SET(ch->act, PLR_KEY))
       {
       send_to_char( "This function is not currently implemented.\n\r", ch );
       return;
       } */

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: knight <char>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "That player is not here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    level = 204;

    if ( level <= victim->level )
    {
        return;
    }
    else
    {
        act ( "You touch $Ns shoulder with a sword called {GKnight's Faith{x.", ch, NULL, victim, TO_CHAR );
        act ( "$n touches your shoulder with a sword called {GKnight's Faith{x.", ch, NULL, victim, TO_VICT );
        act ( "$n touches $Ns shoulder with a sword called {GKnight's Faith{x.", ch, NULL, victim, TO_NOTVICT );
        act ( "$N glows with an unearthly light as $S mortality slips away.",
              ch, NULL, victim, TO_NOTVICT );
    }
    update_wizlist ( victim, level );
    for ( iLevel = victim->level; iLevel < level; iLevel++ )
    {
        send_to_char ( "You raise a level!!  ", victim );
        victim->level += 1;
        advance_level ( victim );
    }
    victim->exp =
        exp_per_level ( victim, victim->pcdata->points ) * UMAX ( 1,
                                                                  victim->
                                                                  level );
    victim->trust = 0;
    save_char_obj ( victim );
    return;
}

CH_CMD ( do_squire )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument ( argument, arg1 );

    /* if (!IS_SET(ch->act, PLR_KEY))
       {
       send_to_char( "This function is not currently implemented.\n\r", ch );
       return;
       } */

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: squire <char>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "That player is not here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    level = 203;

    if ( level <= victim->level )
    {
        return;
    }
    else
    {
        act ( "You touch $Ns shoulder with a sword called {BSquire's Faith{x.", ch, NULL, victim, TO_CHAR );
        act ( "$n touches your shoulder with a sword called {BSquire's Faith{x.", ch, NULL, victim, TO_VICT );
        act ( "$n touches $Ns shoulder with a sword called {BSquire's Faith{x.", ch, NULL, victim, TO_NOTVICT );
        act ( "$N glows with an unearthly light as $S mortality slips away.",
              ch, NULL, victim, TO_NOTVICT );

    }
    update_wizlist ( victim, level );
    for ( iLevel = victim->level; iLevel < level; iLevel++ )
    {
        send_to_char ( "You raise a level!!  ", victim );
        victim->level += 1;
        advance_level ( victim );
    }
    victim->exp =
        exp_per_level ( victim, victim->pcdata->points ) * UMAX ( 1,
                                                                  victim->
                                                                  level );
    victim->trust = 0;
    save_char_obj ( victim );
    return;
}

CH_CMD ( do_trust )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) )
    {
        send_to_char ( "Syntax: trust <char> <level>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "That player is not here.\n\r", ch );
        return;
    }

    if ( ( level = atoi ( arg2 ) ) < 0 || level > 210 )
    {
        send_to_char ( "Level must be 0 (reset) or 1 to 210.\n\r", ch );
        return;
    }

    if ( level > get_trust ( ch ) )
    {
        send_to_char ( "Limited to your trust.\n\r", ch );
        return;
    }

    victim->trust = level;
    return;
}

CH_CMD ( do_restore )
{
    char arg[MAX_INPUT_LENGTH * 100], buf[MAX_STRING_LENGTH * 100];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' || !str_cmp ( arg, "room" ) )
    {
        /* cure room */

        for ( vch = ch->in_room->people; vch != NULL;
              vch = vch->next_in_room )
        {
            if ( IS_SET ( vch->act, PLR_NORESTORE ) )
            {
                act ( "$n attempts to restore you, but fails.", ch, NULL, vch,
                      TO_VICT );
            }
            else
            {
                affect_strip ( vch, gsn_plague );
                affect_strip ( vch, gsn_poison );
                affect_strip ( vch, gsn_blindness );
                affect_strip ( vch, gsn_sleep );
                affect_strip ( vch, gsn_curse );

                vch->hit = vch->max_hit;
                vch->mana = vch->max_mana;
                vch->move = vch->max_move;
                update_pos ( vch );
                act ( "$n has restored you.", ch, NULL, vch, TO_VICT );
            }
        }

        sprintf ( buf, "$N restored room %ld.", ch->in_room->vnum );
        wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );

        send_to_char ( "Room restored.\n\r", ch );
        return;

    }

    if ( get_trust ( ch ) >= MAX_LEVEL - 6 && !str_cmp ( arg, "all" ) )
    {
        /* cure all */

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            victim = d->character;

            if ( victim == NULL || IS_NPC ( victim ) )
                continue;

            if ( IS_SET ( victim->act, PLR_NORESTORE ) )
            {
                act ( "$n attempts to restore you, but fails.", ch, NULL,
                      victim, TO_VICT );
            }
            else
            {
                affect_strip ( victim, gsn_plague );
                affect_strip ( victim, gsn_poison );
                affect_strip ( victim, gsn_blindness );
                affect_strip ( victim, gsn_sleep );
                affect_strip ( victim, gsn_curse );

                victim->hit = victim->max_hit;
                victim->mana = victim->max_mana;
                victim->move = victim->max_move;
                update_pos ( victim );
                if ( victim->in_room != NULL )
/*		if ( ch->pcdata->rmessage != NUL )
		    do_gmessage ( ch->pcdata->rmessage );  */
		    act ( "$n has restored you.", ch, NULL, victim, TO_VICT );
	    }
        }
        send_to_char ( "All active players restored.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_NORESTORE ) )
    {
        act ( "$n attempts to restore you, but fails.", ch, NULL, victim,
              TO_VICT );
        send_to_char ( "You failed.\n\r", ch );
        return;
    }
    affect_strip ( victim, gsn_plague );
    affect_strip ( victim, gsn_poison );
    affect_strip ( victim, gsn_blindness );
    affect_strip ( victim, gsn_sleep );
    affect_strip ( victim, gsn_curse );
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos ( victim );
    act ( "$n has restored you.", ch, NULL, victim, TO_VICT );
    sprintf ( buf, "$N restored %s",
              IS_NPC ( victim ) ? victim->short_descr : victim->name );
    wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_immkiss )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Who do you want to kiss?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    if ( ch->in_room != victim->in_room )
    {
        send_to_char ( "Your lips aren't that long!\n\r", ch );
        return;
    }
    affect_strip ( victim, gsn_plague );
    affect_strip ( victim, gsn_poison );
    affect_strip ( victim, gsn_blindness );
    affect_strip ( victim, gsn_sleep );
    affect_strip ( victim, gsn_curse );
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos ( victim );
    act ( "$n kisses you, and you feel a sudden rush of adrenaline.", ch,
          NULL, victim, TO_VICT );
    send_to_char ( "You feel MUCH better now!\n\r", victim );
    send_to_char ( "They feel MUCH better now!\n\r", ch );
    sprintf ( buf, "$N immkissed %s",
              IS_NPC ( victim ) ? victim->short_descr : victim->name );
    wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );
    return;
}

CH_CMD ( do_freeze )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Freeze whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_FREEZE ) )
    {
        REMOVE_BIT ( victim->act, PLR_FREEZE );
        send_to_char ( "You can play again.\n\r", victim );
        send_to_char ( "FREEZE removed.\n\r", ch );
        sprintf ( buf, "$N thaws %s.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->act, PLR_FREEZE );
        send_to_char ( "You can't do ANYthing!\n\r", victim );
        send_to_char ( "FREEZE set.\n\r", ch );
        sprintf ( buf, "$N puts %s in the deep freeze.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_norestore )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Norestore whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_NORESTORE ) )
    {
        REMOVE_BIT ( victim->act, PLR_NORESTORE );
        send_to_char ( "NORESTORE removed.\n\r", ch );
        sprintf ( buf, "$N allows %s restores.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->act, PLR_NORESTORE );
        send_to_char ( "NORESTORE set.\n\r", ch );
        sprintf ( buf, "$N denys %s restores.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_noarena )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Noarena whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_NOARENA ) )
    {
        REMOVE_BIT ( victim->act2, PLR2_NOARENA );
        send_to_char ( "NOARENA removed.\n\r", ch );
        sprintf ( buf, "$N allows %s to fight in the arena.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->act2, PLR2_NOARENA );
        send_to_char ( "NOARENA set.\n\r", ch );
        sprintf ( buf, "$N denies %s to fight in the arena.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_nogsocial )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "No globalsocial whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOGSOCIAL ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOGSOCIAL );
        send_to_char ( "NOGSOCIAL removed.\n\r", ch );
        sprintf ( buf, "$N allows %s to do global socials.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOGSOCIAL );
        send_to_char ( "NOGSOCIAL set.\n\r", ch );
        sprintf ( buf, "$N denies %s to use global socials.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_nosay )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Nosay whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOSAY ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOSAY );
        send_to_char ( "NOSAY removed.\n\r", ch );
        sprintf ( buf, "$N allows %s to say things.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOSAY );
        send_to_char ( "NOSAY set.\n\r", ch );
        sprintf ( buf, "$N denies %s to say things.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_notitle )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Notitle whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_NOTITLE ) )
    {
        REMOVE_BIT ( victim->act, PLR_NOTITLE );
        send_to_char ( "NOTITLE removed.\n\r", ch );
        sprintf ( buf, "$N allows %s title.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->act, PLR_NOTITLE );
        send_to_char ( "NOTITLE set.\n\r", ch );
        sprintf ( buf, "$N denys %s title.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

CH_CMD ( do_log )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Log whom?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        if ( fLogAll )
        {
            fLogAll = FALSE;
            send_to_char ( "Log ALL off.\n\r", ch );
        }
        else
        {
            fLogAll = TRUE;
            send_to_char ( "Log ALL on.\n\r", ch );
        }
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET ( victim->act, PLR_LOG ) )
    {
        REMOVE_BIT ( victim->act, PLR_LOG );
        send_to_char ( "LOG removed.\n\r", ch );
    }
    else
    {
        SET_BIT ( victim->act, PLR_LOG );
        send_to_char ( "LOG set.\n\r", ch );
    }

    return;
}

CH_CMD ( do_noemote )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Noemote whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOEMOTE ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOEMOTE );
        send_to_char ( "You can emote again.\n\r", victim );
        send_to_char ( "NOEMOTE removed.\n\r", ch );
        sprintf ( buf, "$N restores emotes to %s.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOEMOTE );
        send_to_char ( "You can't emote!\n\r", victim );
        send_to_char ( "NOEMOTE set.\n\r", ch );
        sprintf ( buf, "$N revokes %s's emotes.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    return;
}

CH_CMD ( do_noshout )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Noshout whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOSHOUT ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOSHOUT );
        send_to_char ( "You can shout again.\n\r", victim );
        send_to_char ( "NOSHOUT removed.\n\r", ch );
        sprintf ( buf, "$N restores shouts to %s.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOSHOUT );
        send_to_char ( "You can't shout!\n\r", victim );
        send_to_char ( "NOSHOUT set.\n\r", ch );
        sprintf ( buf, "$N revokes %s's shouts.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    return;
}

CH_CMD ( do_notell )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Notell whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_NOTELL ) )
    {
        REMOVE_BIT ( victim->comm, COMM_NOTELL );
        send_to_char ( "You can tell again.\n\r", victim );
        send_to_char ( "NOTELL removed.\n\r", ch );
        sprintf ( buf, "$N restores tells to %s.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->comm, COMM_NOTELL );
        send_to_char ( "You can't tell!\n\r", victim );
        send_to_char ( "NOTELL set.\n\r", ch );
        sprintf ( buf, "$N revokes %s's tells.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    return;
}

CH_CMD ( do_peace )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( rch->fighting != NULL )
        {
            stop_fighting ( rch, TRUE );
            if ( !IS_NPC ( rch ) )
            {
                send_to_char ( "Ok.\n\r", ch );
            }
        }
        if ( IS_NPC ( rch ) && IS_SET ( rch->act, ACT_AGGRESSIVE ) )
            REMOVE_BIT ( rch->act, ACT_AGGRESSIVE );
    }
    return;
}

CH_CMD ( do_wizlock )
{
    wizlock = !wizlock;

    if ( wizlock )
    {
        char buf[MIL];
        wiznet ( "$N has wizlocked the game.", ch, NULL, 0, 0, 0 );
        send_to_char ( "Game wizlocked.\n\r", ch );
        sprintf ( buf, "%s has wizlocked the game.", ch->name );
        append_file ( ch, WIZLOCK_FILE, buf );
    }
    else
    {
        wiznet ( "$N removes wizlock.", ch, NULL, 0, 0, 0 );
        send_to_char ( "Game un-wizlocked.\n\r", ch );
        unlink ( WIZLOCK_FILE );
    }

    return;
}

/* RT anti-newbie code */

CH_CMD ( do_newlock )
{
    newlock = !newlock;

    if ( newlock )
    {
        char buf[MIL];
        wiznet ( "$N locks out new characters.", ch, NULL, 0, 0, 0 );
        send_to_char ( "New characters have been locked out.\n\r", ch );
        sprintf ( buf, "%s has newlocked the game.", ch->name );
        append_file ( ch, NEWLOCK_FILE, buf );
    }
    else
    {
        wiznet ( "$N allows new characters back in.", ch, NULL, 0, 0, 0 );
        send_to_char ( "Newlock removed.\n\r", ch );
        unlink ( NEWLOCK_FILE );
    }

    return;
}

CH_CMD ( do_slookup )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Lookup which skill or spell?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].name == NULL )
                break;
            sprintf ( buf,
                      "{cSn{x: {Y%3d  {cSlot{x:{Y %3d{c  Skill{x/{cSpell{x: '{Y%s{x'{x\n\r",
                      sn, skill_table[sn].slot, skill_table[sn].name );
            send_to_char ( buf, ch );
        }
    }
    else
    {
        if ( ( sn = skill_lookup ( arg ) ) < 0 )
        {
            send_to_char ( "No such skill or spell.\n\r", ch );
            return;
        }

        sprintf ( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r", sn,
                  skill_table[sn].slot, skill_table[sn].name );
        send_to_char ( buf, ch );
    }

    return;
}

/* RT set replaces sset, mset, oset, and rset */

CH_CMD ( do_set )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  set mob   <name> <field> <value>\n\r", ch );
        send_to_char ( "  set obj   <name> <field> <value>\n\r", ch );
        send_to_char ( "  set room  <room> <field> <value>\n\r", ch );
        send_to_char ( "  set skill <name> <spell or skill> <value>\n\r", ch );
	send_to_char("  set stance  <name> <stance> <value>\n\r",ch);
        send_to_char ( "  set char  <name> <field> <value>\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg, "mobile" ) || !str_prefix ( arg, "character" ) )
    {
        do_mset ( ch, argument );
        return;
    }

    if ( !str_prefix ( arg, "skill" ) || !str_prefix ( arg, "spell" ) )
    {
        do_sset ( ch, argument );
        return;
    }

    if (!str_prefix(arg,"stance"))
    {
	do_stance_set(ch,argument);
	return;
    }
    
    
    if ( !str_prefix ( arg, "object" ) )
    {
        do_oset ( ch, argument );
        return;
    }

    if ( !str_prefix ( arg, "room" ) )
    {
        do_rset ( ch, argument );
        return;
    }
    /* echo syntax */
    do_set ( ch, "" );
}

CH_CMD ( do_sset )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  set skill <name> <spell or skill> <value>\n\r",
                       ch );
        send_to_char ( "  set skill <name> all <value>\n\r", ch );
        send_to_char ( "   (use the name of the skill, not the number)\n\r",
                       ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    fAll = !str_cmp ( arg2, "all" );
    sn = 0;
    if ( !fAll && ( sn = skill_lookup ( arg2 ) ) < 0 )
    {
        send_to_char ( "No such skill or spell.\n\r", ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number ( arg3 ) )
    {
        send_to_char ( "Value must be numeric.\n\r", ch );
        return;
    }

    value = atoi ( arg3 );
    if ( value < 0 || value > 100 )
    {
        send_to_char ( "Value range is 0 to 100.\n\r", ch );
        return;
    }

    if ( fAll )
    {
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].name != NULL )
                victim->pcdata->learned[sn] = value;
        }
    }
    else
    {
        victim->pcdata->learned[sn] = value;
    }

    return;
}

CH_CMD ( do_mset )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    long value;

    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    strcpy ( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  set char <name> <field> <value>\n\r", ch );
        send_to_char ( "  Field being one of:\n\r", ch );
        send_to_char ( "    str int wis dex con sex class level\n\r", ch );
        send_to_char ( "    race group platinum gold silver hp\n\r", ch );
        send_to_char ( "    mana move prac align train thirst\n\r", ch );
        send_to_char ( "    hunger drunk full quest aqp\n\r", ch );
        send_to_char ( "    security pkkills pkdeaths awins\n\r", ch );
        send_to_char ( "    alosses twins tkills tdeaths exiletime\n\r", ch );
	send_to_char ( "    mdeaths mkills jailtime\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number ( arg3 ) ? atol ( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp ( arg2, "str" ) )
    {
        if ( value < 3 || value > get_max_train ( victim, STAT_STR ) )
        {
            sprintf ( buf, "Strength range is 3 to %d\n\r.",
                      get_max_train ( victim, STAT_STR ) );
            send_to_char ( buf, ch );
            return;
        }

        victim->perm_stat[STAT_STR] = value;
        return;
    }
    if ( !str_cmp ( arg2, "security" ) )    /* OLC */
    {
	if ( ch->level < MAX_LEVEL )
	{
	    send_to_char ( "Only Rhaelar can hand out security.\n\r", ch );
	    return;
	}

        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value > ch->pcdata->security || value < 0 )
        {
            if ( ch->pcdata->security != 0 )
            {
                sprintf ( buf, "Valid security is 0-%d.\n\r",
                          ch->pcdata->security );
                send_to_char ( buf, ch );
            }
            else
            {
                send_to_char ( "Valid security is 0 only.\n\r", ch );
            }
            return;
        }
        victim->pcdata->security = value;
        return;
    }

    if ( !str_cmp ( arg2, "int" ) )
    {
        if ( value < 3 || value > get_max_train ( victim, STAT_INT ) )
        {
            sprintf ( buf, "Intelligence range is 3 to %d.\n\r",
                      get_max_train ( victim, STAT_INT ) );
            send_to_char ( buf, ch );
            return;
        }

        victim->perm_stat[STAT_INT] = value;
        return;
    }

    if ( !str_cmp ( arg2, "wis" ) )
    {
        if ( value < 3 || value > get_max_train ( victim, STAT_WIS ) )
        {
            sprintf ( buf, "Wisdom range is 3 to %d.\n\r",
                      get_max_train ( victim, STAT_WIS ) );
            send_to_char ( buf, ch );
            return;
        }

        victim->perm_stat[STAT_WIS] = value;
        return;
    }

    if ( !str_cmp ( arg2, "dex" ) )
    {
        if ( value < 3 || value > get_max_train ( victim, STAT_DEX ) )
        {
            sprintf ( buf, "Dexterity ranges is 3 to %d.\n\r",
                      get_max_train ( victim, STAT_DEX ) );
            send_to_char ( buf, ch );
            return;
        }

        victim->perm_stat[STAT_DEX] = value;
        return;
    }

    if ( !str_cmp ( arg2, "con" ) )
    {
        if ( value < 3 || value > get_max_train ( victim, STAT_CON ) )
        {
            sprintf ( buf, "Constitution range is 3 to %d.\n\r",
                      get_max_train ( victim, STAT_CON ) );
            send_to_char ( buf, ch );
            return;
        }

        victim->perm_stat[STAT_CON] = value;
        return;
    }

    if ( !str_prefix ( arg2, "sex" ) )
    {
        if ( value < 0 || value > 2 )
        {
            send_to_char ( "Sex range is 0 to 2.\n\r", ch );
            return;
        }
        victim->sex = value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->true_sex = value;
        return;
    }

    if ( !str_prefix ( arg2, "class" ) )
    {
        int class;

        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Mobiles have no class.\n\r", ch );
            return;
        }

        class = class_lookup ( arg3 );
        if ( class == -1 )
        {
            char buf[MAX_STRING_LENGTH];

            strcpy ( buf, "Possible classes are: " );
            for ( class = 0; class < MAX_CLASS; class++ )
            {
                if ( class > 0 )
                    strcat ( buf, " " );
                strcat ( buf, class_table[class].name );
            }
            strcat ( buf, ".\n\r" );

            send_to_char ( buf, ch );
            return;
        }

        victim->class = class;
        return;
    }

    if ( !str_prefix ( arg2, "level" ) )
    {
        if ( !IS_NPC ( victim ) )
        {
            send_to_char ( "Not on PC's.\n\r", ch );
            return;
        }

        if ( value < 0 || value > ch->level )
        {
            sprintf ( buf, "Level range is 0 to %d.\n\r", ch->level );
            send_to_char ( buf, ch );
            return;
        }
        victim->level = value;
        return;
    }

    if ( !str_prefix ( arg2, "platinum" ) )
    {
        victim->platinum = value;
        return;
    }

    if ( !str_prefix ( arg2, "gold" ) )
    {
        victim->gold = value;
        return;
    }

    if ( !str_prefix ( arg2, "silver" ) )
    {
        victim->silver = value;
        return;
    }

    if ( !str_prefix ( arg2, "pkkills" ) )
    {
        victim->pcdata->pkkills = value;
        return;
    }

    if ( !str_prefix ( arg2, "pkdeaths" ) )
    {
        victim->pcdata->pkdeaths = value;
        return;
    }

    if ( !str_prefix ( arg2, "twins" ) )
    {
        victim->pcdata->twins = value;
        return;
    }

    if ( !str_prefix ( arg2, "tkills" ) )
    {
        victim->pcdata->tkills = value;
        return;
    }

    if ( !str_prefix ( arg2, "tdeaths" ) )
    {
        victim->pcdata->tdeaths = value;
        return;
    }

    if ( !str_prefix ( arg2, "mdeaths" ) )
    {
        victim->pcdata->mdeath = value;
        return;
    }

    if ( !str_prefix ( arg2, "mkills" ) )
    {
        victim->pcdata->mkill = value;
        return;
    }
    
    if ( !str_prefix ( arg2, "exiletime" ) )
    {
        victim->pcdata->exiletime = value;
        return;
    }

    if ( !str_prefix ( arg2, "jailtime" ) )
    {
        victim->pcdata->jailtime = value;
        return;
    }
                
    if ( !str_prefix ( arg2, "awins" ) )
    {
        victim->pcdata->awins = value;
        return;
    }

    if ( !str_prefix ( arg2, "alosses" ) )
    {
        victim->pcdata->alosses = value;
        return;
    }

    if ( !str_prefix ( arg2, "hp" ) )
    {
        if ( value < -10 || value > 100000000 )
        {
            send_to_char ( "Hp range is -10 to 100,000,000 hit points.\n\r",
                           ch );
            return;
        }
        victim->max_hit = value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_hit = value;
        return;
    }

    if ( !str_prefix ( arg2, "mana" ) )
    {
        if ( value < 0 || value > 100000000 )
        {
            send_to_char ( "Mana range is 0 to 100,000,000 mana points.\n\r",
                           ch );
            return;
        }
        victim->max_mana = value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_mana = value;
        return;
    }

    if ( !str_prefix ( arg2, "move" ) )
    {
        if ( value < 0 || value > 100000 )
        {
            send_to_char ( "Move range is 0 to 100,000,000 move points.\n\r",
                           ch );
            return;
        }
        victim->max_move = value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_move = value;
        return;
    }

    if ( !str_prefix ( arg2, "practice" ) )
    {
        if ( value < 0 || value > 500 )
        {
            send_to_char ( "Practice range is 0 to 500 sessions.\n\r", ch );
            return;
        }
        victim->practice = value;
        return;
    }

    if ( !str_prefix ( arg2, "train" ) )
    {
        if ( value < 0 || value > 500 )
        {
            send_to_char ( "Training session range is 0 to 500 sessions.\n\r",
                           ch );
            return;
        }
        victim->train = value;
        return;
    }

    if ( !str_prefix ( arg2, "align" ) )
    {
        if ( value < -1000 || value > 1000 )
        {
            send_to_char ( "Alignment range is -1000 to 1000.\n\r", ch );
            return;
        }
        victim->alignment = value;
        if ( victim->pet != NULL )
            victim->pet->alignment = victim->alignment;
        return;
    }

    if ( !str_prefix ( arg2, "thirst" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char ( "Thirst range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_THIRST] = value;
        return;
    }

    if ( !str_prefix ( arg2, "drunk" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char ( "Drunk range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_DRUNK] = value;
        return;
    }

    if ( !str_prefix ( arg2, "full" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char ( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_FULL] = value;
        return;
    }

    if ( !str_prefix ( arg2, "hunger" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char ( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_HUNGER] = value;
        return;
    }

    if ( !str_prefix ( arg2, "quest" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "NPC's don't need quest points.\n\r", ch );
            return;
        }

        victim->qps = value;
        return;
    }
    if ( !str_prefix ( arg2, "aqp" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "NPC's don't need quest points.\n\r", ch );
            return;
        }

        victim->pcdata->questpoints = value;
        return;
    }

    if ( !str_prefix ( arg2, "race" ) )
    {
        int race;

        race = race_lookup ( arg3 );

        if ( race == 0 )
        {
            send_to_char ( "That is not a valid race.\n\r", ch );
            return;
        }

        if ( !IS_NPC ( victim ) && !race_table[race].pc_race )
        {
            send_to_char ( "That is not a valid player race.\n\r", ch );
            return;
        }

        victim->race = race;
        return;
    }

    if ( !str_prefix ( arg2, "group" ) )
    {
        if ( !IS_NPC ( victim ) )
        {
            send_to_char ( "Only on NPCs.\n\r", ch );
            return;
        }
        victim->group = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_mset ( ch, "" );
    return;
}

CH_CMD ( do_string )
{
    char type[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int cnt, plc;

    smash_tilde ( argument );
    argument = one_argument ( argument, type );
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    strcpy ( arg3, argument );

    if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  string char <name> <field> <string>\n\r", ch );
        send_to_char ( "    fields: name short long title who spec cname\n\r", ch );
        send_to_char ( "  string obj  <name> <field> <string>\n\r", ch );
        send_to_char ( "    fields: name short long extended\n\r", ch );
        return;
    }

    if ( !str_prefix ( type, "character" ) || !str_prefix ( type, "mobile" ) )
    {
        if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
        {
            send_to_char ( "They aren't here.\n\r", ch );
            return;
        }

        /* clear zone for mobs */
        victim->zone = NULL;

        /* string something */

        if ( ( victim->level >= ch->level ) && ( ch != victim ) )
        {
            send_to_char ( "That will not be done.\n\r", ch );
            return;
        }

        if ( !str_prefix ( arg2, "who" ) )
        {
            if ( IS_NPC ( victim ) )
            {
                send_to_char ( "Not on NPC's.\n\r", ch );
                return;
            }
            if ( ( ch->level < CREATOR ) && ( victim->level < HERO ) &&
                 ( victim->class < MAX_CLASS / 2 ) )
            {
                send_to_char ( "Not on 1st tier mortals.\n\r", ch );
                return;
            }
            buf[0] = '\0';
            buf2[0] = '\0';
            victim->pcdata->who_descr = str_dup ( "" );
            if ( arg3[0] == '\0' )
            {
                return;
            }
            cnt = 0;
            for ( plc = 0; plc < strlen ( arg3 ); plc++ )
            {
                if ( arg3[plc] != '{' )
                {
                    if ( buf[0] == '\0' )
                    {
                        sprintf ( buf2, "%c", arg3[plc] );
                    }
                    else
                    {
                        sprintf ( buf2, "%s%c", buf, arg3[plc] );
                    }
                    sprintf ( buf, "%s", buf2 );
                    cnt++;
                }
                else if ( arg3[plc + 1] == '{' )
                {
                    if ( buf[0] == '\0' )
                    {
                        sprintf ( buf2, "{{" );
                    }
                    else
                    {
                        sprintf ( buf2, "%s{{", buf );
                    }
                    sprintf ( buf, "%s", buf2 );
                    cnt++;
                    plc++;
                }
                else
                {
                    if ( buf[0] == '\0' )
                    {
                        sprintf ( buf2, "{%c", arg3[plc + 1] );
                    }
                    else
                    {
                        sprintf ( buf2, "%s{%c", buf, arg3[plc + 1] );
                    }
                    sprintf ( buf, "%s", buf2 );
                    plc++;
                }
                if ( cnt >= 10 )
                {
                    plc = strlen ( arg3 );
                }
            }
            sprintf ( buf2, "%s{0", buf );
            sprintf ( buf, "%s", buf2 );
            while ( cnt < 10 )
            {
                sprintf ( buf2, "%s ", buf );
                sprintf ( buf, "%s", buf2 );
                cnt++;
            }
            victim->pcdata->who_descr = str_dup ( buf );
            buf[0] = '\0';
            buf2[0] = '\0';
            return;
        }

        if ( arg3[0] == '\0' )
        {
            do_string ( ch, "" );
            return;
        }

        if ( !str_prefix ( arg2, "name" ) )
        {
            if ( !IS_NPC ( victim ) )
            {
                send_to_char ( "Not on PC's.\n\r", ch );
                return;
            }
            free_string ( victim->name );
            victim->name = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "short" ) )
        {
            free_string ( victim->short_descr );
            victim->short_descr = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "long" ) )
        {
            free_string ( victim->long_descr );
            strcat ( arg3, "\n\r" );
            victim->long_descr = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "title" ) )
        {
            if ( IS_NPC ( victim ) )
            {
                send_to_char ( "Not on NPC's.\n\r", ch );
                return;
            }

            set_title ( victim, arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "cname" ) )
        {
            if ( IS_NPC ( victim ) )
            {
                send_to_char ( "Not on NPC's.\n\r", ch );
                return;
            }
            free_string ( victim->pcdata->cname );
            victim->pcdata->cname = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "spec" ) )
        {
            if ( !IS_NPC ( victim ) )
            {
                send_to_char ( "Not on PC's.\n\r", ch );
                return;
            }

            if ( ( victim->spec_fun = spec_lookup ( arg3 ) ) == 0 )
            {
                send_to_char ( "No such spec fun.\n\r", ch );
                return;
            }

            return;
        }
    }

    if ( arg3[0] == '\0' )
    {
        do_string ( ch, "" );
        return;
    }
    if ( !str_prefix ( type, "object" ) )
    {
        /* string an obj */

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
        if ( !str_prefix ( arg2, "name" ) )
        {
            free_string ( obj->name );
            obj->name = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "short" ) )
        {
            free_string ( obj->short_descr );
            obj->short_descr = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "long" ) )
        {
            free_string ( obj->description );
            obj->description = str_dup ( arg3 );
            return;
        }

        if ( !str_prefix ( arg2, "ed" ) || !str_prefix ( arg2, "extended" ) )
        {
            EXTRA_DESCR_DATA *ed;

            argument = one_argument ( argument, arg3 );
            if ( argument == NULL )
            {
                send_to_char
                    ( "Syntax: oset <object> ed <keyword> <string>\n\r", ch );
                return;
            }

            strcat ( argument, "\n\r" );

            ed = new_extra_descr (  );

            ed->keyword = str_dup ( arg3 );
            ed->description = str_dup ( argument );
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            return;
        }
    }

    /* echo bad use message */
    do_string ( ch, "" );
}

CH_CMD ( do_oset )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    long value;
    int clan;
    int class;

    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    strcpy ( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  set obj <object> <field> <value>\n\r", ch );
        send_to_char ( "  Field being one of:\n\r", ch );
        send_to_char ( "    value0 value1 value2 value3 value4 (v1-v4)\n\r",
                       ch );
        send_to_char ( "    level weight cost timer clan guild\n\r", ch );
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

    if ( !str_prefix ( arg2, "extra" ) )
    {
        if ( ( value = flag_value ( extra_flags, arg3 ) ) != NO_FLAG )
        {
            TOGGLE_BIT ( obj->extra_flags, value );
        }
        return;
    }

    if ( !str_prefix ( arg2, "clan" ) )
    {
        if ( !str_prefix ( arg3, "none" ) )
        {
            obj->clan = 0;
            return;
        }
        if ( ( clan = clan_lookup ( arg3 ) ) == 0 )
        {
            send_to_char ( "No such clan exists.\n\r", ch );
            return;
        }
        obj->clan = clan;
        return;
    }
    if ( !str_prefix ( arg2, "guild" ) )
    {
        if ( !str_prefix ( arg3, "none" ) )
        {
            obj->class = 0;
            return;
        }
        if ( ( class = class_lookup ( arg3 ) ) == 0 )
        {
            send_to_char ( "No such guild exists.\n\r", ch );
            return;
        }
        obj->class = class;
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atol ( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp ( arg2, "value0" ) || !str_cmp ( arg2, "v0" ) )
    {
        if ( obj->item_type == ITEM_WEAPON )
        {
            obj->value[0] = UMIN ( MAX_WEAPON, value );
            obj->value[0] = UMAX ( 0, obj->value[0] );
            return;
        }
        if ( ( obj->item_type == ITEM_WAND ) ||
             ( obj->item_type == ITEM_STAFF ) ||
             ( obj->item_type == ITEM_POTION ) ||
             ( obj->item_type == ITEM_SCROLL ) ||
             ( obj->item_type == ITEM_PILL ) )
        {
            obj->value[0] = UMIN ( MAX_LEVEL, value );
            obj->value[0] = UMAX ( 0, obj->value[0] );
            return;
        }
        obj->value[0] = value;
        return;
    }

    if ( !str_cmp ( arg2, "value1" ) || !str_cmp ( arg2, "v1" ) )
    {
        obj->value[1] = value;
        return;
    }

    if ( !str_cmp ( arg2, "value2" ) || !str_cmp ( arg2, "v2" ) )
    {
        if ( ( obj->item_type == ITEM_FOUNTAIN ) ||
             ( obj->item_type == ITEM_DRINK_CON ) )
        {
            obj->value[2] = UMIN ( MAX_LIQUID, value );
            obj->value[2] = UMAX ( 0, obj->value[2] );
            return;
        }
        obj->value[2] = value;
        return;
    }
    
    if ( !str_cmp ( arg2, "value3" ) || !str_cmp ( arg2, "v3" ) )
    {
        if ( obj->item_type == ITEM_WEAPON )
        {
/*            obj->value[3] = UMIN ( MAX_DAMAGE_MESSAGE, value );
            obj->value[3] = UMAX ( 0, obj->value[3] ); */ /* Awww wtf is this -Rhaelar 12'2004 */
/*	    obj->value[3] = attack_lookup ( value ); */
	    send_to_char ( "Use the damnoun command.\n\r", ch );
            return;
        }
        obj->value[3] = value;
        return;
    }

    if ( !str_cmp ( arg2, "value4" ) || !str_cmp ( arg2, "v4" ) )
    {
        obj->value[4] = value;
        return;
    }

    if ( !str_prefix ( arg2, "extra" ) )
    {
        send_to_char ( "Use the flag command instead.\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg2, "wear" ) )
    {
        send_to_char ( "Use the flag command instead.\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg2, "level" ) )
    {
        if ( ( get_trust ( ch ) < CREATOR &&
               ( obj->pIndexData->level - 5 ) > value ) &&
             !IS_SET ( ch->act, PLR_KEY ) )
        {
            send_to_char
                ( "You may not lower an item more than 5 levels!\n\r", ch );
            return;
        }
        if ( ( get_trust ( ch ) == CREATOR &&
               ( obj->pIndexData->level - 10 ) > value ) &&
             !IS_SET ( ch->act, PLR_KEY ) )
        {
            send_to_char
                ( "You may not lower an item more than 10 levels!\n\r", ch );
            return;
        }
        obj->level = UMIN ( MAX_LEVEL, value );
        obj->level = UMIN ( 0, obj->level );
        return;
    }

    if ( !str_prefix ( arg2, "weight" ) )
    {
        obj->weight = value;
        return;
    }

    if ( !str_prefix ( arg2, "cost" ) )
    {
        obj->cost = value;
        return;
    }

    if ( !str_prefix ( arg2, "timer" ) )
    {
        obj->timer = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_oset ( ch, "" );
    return;
}

CH_CMD ( do_rset )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    long value;

    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    strcpy ( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  set room <location> <field> <value>\n\r", ch );
        send_to_char ( "  Field being one of:\n\r", ch );
        send_to_char ( "    sector\n\r", ch );
        return;
    }

    if ( ( location = find_location ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "No such location.\n\r", ch );
        return;
    }

    if ( !is_room_owner ( ch, location ) && ch->in_room != location &&
         room_is_private ( ch, location ) && !IS_TRUSTED ( ch, HEADIMP ) )
    {
        send_to_char ( "That room is private right now.\n\r", ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number ( arg3 ) )
    {
        send_to_char ( "Value must be numeric.\n\r", ch );
        return;
    }
    value = atol ( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix ( arg2, "flags" ) )
    {
        send_to_char ( "Use the flag command instead.\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg2, "sector" ) )
    {
        location->sector_type = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_rset ( ch, "" );
    return;
}
/*
 * Commented out by Rhaelar due to new sockets command below - 5'2004
 *
CH_CMD ( do_sockets )
{
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int count;
    char *st;
    char s[100];
    char idle[10];

    count = 0;
    buf[0] = '\0';
    buf2[0] = '\0';

    strcat ( buf2,
             "\n\r[Num Connected_State Login@ Idl] Player Name Host\n\r" );
    strcat ( buf2,
             "--------------------------------------------------------------------------\n\r" );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->character && can_see ( ch, d->character ) )
        {
  
            switch ( d->connected )
            {
                case CON_PLAYING:
                    st = "    PLAYING    ";
                    break;
                case CON_GET_NAME:
                    st = "   Get Name    ";
                    break;
                case CON_GET_OLD_PASSWORD:
                    st = "Get Old Passwd ";
                    break;
                case CON_CONFIRM_NEW_NAME:
                    st = " Confirm Name  ";
                    break;
                case CON_GET_NEW_PASSWORD:
                    st = "Get New Passwd ";
                    break;
                case CON_CONFIRM_NEW_PASSWORD:
                    st = "Confirm Passwd ";
                    break;
                case CON_GET_NEW_RACE:
                    st = "  Get New Race ";
                    break;
                case CON_GET_NEW_SEX:
                    st = "  Get New Sex  ";
                    break;
                case CON_GET_NEW_CLASS:
                    st = " Get New Class ";
                    break;
                case CON_GET_ALIGNMENT:
                    st = " Get New Align ";
                    break;
                case CON_DEFAULT_CHOICE:
                    st = " Choosing Cust ";
                    break;
                case CON_GEN_GROUPS:
                    st = " Customization ";
                    break;
                case CON_PICK_WEAPON:
                    st = " Picking Weapon";
                    break;
                case CON_READ_IMOTD:
                    st = " Reading IMOTD ";
                    break;
                case CON_BREAK_CONNECT:
                    st = "   LINKDEAD    ";
                    break;
                case CON_READ_MOTD:
                    st = "  Reading MOTD ";
                    break;
                case CON_WIZ:
                    st = "  IMM CHOICE   ";
                    break;
                case CON_GET_ANSI:
                    st = "   Get ANSI    ";
                    break;
                default:
                    st = "   !UNKNOWN!   ";
                    break;
            }
            count++;

  
            vch = d->original ? d->original : d->character;
            strftime ( s, 100, "%I:%M%p", localtime ( &vch->logon ) );

            if ( vch->timer > 0 )
                sprintf ( idle, "%-2d", vch->timer );
            else
                sprintf ( idle, "  " );

            sprintf ( buf, "[%3d %s %7s %2s] %-12s %-32.32s\n\r",
                      d->descriptor, st, s, idle,
                      ( d->original ) ? d->original->name : ( d->
                                                              character ) ?
                      d->character->name : "(None!)", d->host );

            strcat ( buf2, buf );

        }
    }

    sprintf ( buf, "\n\r%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat ( buf2, buf );
    send_to_char ( buf2, ch );
    return;
}
*/

CH_CMD ( do_sockets )
{
    CHAR_DATA       *vch;
    DESCRIPTOR_DATA *d;
    char            buf  [ MSL ];
    char            buf2 [ MSL ];
    char            arg  [MIL];
    int             count;
    char *          st;
    char            s[100];
    char            idle[10];


    count       = 0;
    buf[0]      = '\0';
    buf2[0]     = '\0';

    strcat( buf2,"\n\r{C:{B==========================================================================={C:{x\n\r" );
    strcat( buf2,"{B|<><><><><><><><><><><><><><>     {Y Sockets {B     <><><><><><><><><><><><><><>|{x\n\r");
    strcat( buf2,"{C:{B==========================================================================={C:{x\n\r");
    strcat( buf2,"{B|  [{CNum State    Login{B@{C  Idl{B] [ {CPlayer Name{B ] [  {CHosts{B  ]       |{x\n\r");
    strcat( buf2,"{C:{B==========================================================================={C:{x\n\r");

  one_argument(argument,arg);
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->character && can_see( ch, d->character )
		    && (arg[0] == '\0' || is_name(arg,d->character->name)
		 	|| (d->original && is_name(arg,d->original->name)))
			&& !IS_NPC(d->character) && (d->character->level <= ch->level))

        {
           switch( d->connected )
           {
              case CON_PLAYING:              st = "PLAYING ";    break;
              case CON_GET_NAME:             st = "Get Name";    break;
              case CON_GET_OLD_PASSWORD:     st = "Passwd  ";    break;
              case CON_CONFIRM_NEW_NAME:     st = "New Name";    break;
              case CON_GET_NEW_PASSWORD:     st = "New Pwd ";    break;
              case CON_CONFIRM_NEW_PASSWORD: st = "Con Pwd ";    break;
              case CON_GET_NEW_RACE:         st = "New Race";    break;
              case CON_GET_NEW_SEX:          st = "New Sex ";    break;
              case CON_GET_NEW_CLASS:        st = "New Cls ";    break;
              case CON_GET_ALIGNMENT:        st = "New Aln ";	 break;
//              case CON_DEFAULT_CHOICE        st = "Default ";	 break;
              case CON_GEN_GROUPS:           st = " Custom ";	 break;
//              case CON_PICK_WEAPON:          st = " Weapon ";	 break; 
	      case CON_READ_IMOTD:	     st = " IMOTD  "; 	 break;
	      case CON_BREAK_CONNECT:	     st = "LINKDEAD";	 break;
              case CON_READ_MOTD:            st = "  MOTD  ";    break;
	      case CON_NOTE_TO:		     st = "Note-To ";    break;
	      case CON_NOTE_TEXT:	     st = " NWrite ";    break;
	      case CON_WIZ:		     st = "WizLogin";    break;
	      case CON_COLOUR_LOGIN:	     st = " Colour ";    break;
	      case CON_BEGIN_REROLL:	     st = "BginRrol";    break;
	      case CON_NOTE_SUBJECT:	     st = "Note-Sub";    break;
	      case CON_NOTE_EXPIRE:	     st = "Note-Exp";    break;
	      case CON_NOTE_FINISH:	     st = "Note-Fin";    break;
	      case CON_NOT_PLAYING:	     st = "Recyclin";    break;
              default:                       st = "UNKNOWN!";    break;
           }
           count++;

           /* Format "login" value... */
           vch = d->original ? d->original : d->character;
           strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );

           if ( vch->timer > 0 )
              sprintf( idle, "%-2d", vch->timer );
           else
              sprintf( idle, "  " );

       sprintf( buf, "{B|  [{C%-3d %-8s %7s  %2s{B]  {C%-12s  %-30s {B|{x\n\r",
              d->descriptor,
              st,
              s,
              idle,
              ( d->original ) ? d->original->name
                              : ( d->character )  ? d->character->name
                                                  : "(None!)",
              d->host );

           strcat( buf2, buf );

        }
    }

    strcat( buf2,"{B|                                                                           |{x");
    sprintf( buf,"\n\r{B|  Users:{C%-3d                                                                 {B|{x\n\r", count );
    strcat( buf2, buf );
    strcat( buf2,"{C:{B==========================================================================={C:{x\n\r");
    send_to_char( buf2, ch );
    return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
CH_CMD ( do_force )
{
    char buf[MAX_STRING_LENGTH];
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

    sprintf ( buf, "$n forces you to '%s'.", argument );

    if ( !str_cmp ( arg, "all" ) )
    {
        DESCRIPTOR_DATA *d, *d_next;

        if ( get_trust ( ch ) < MAX_LEVEL - 5 )
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
                act ( buf, ch, NULL, d->character, TO_VICT );
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
                act ( buf, ch, NULL, d->character, TO_VICT );
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
                act ( buf, ch, NULL, d->character, TO_VICT );
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

        act ( buf, ch, NULL, victim, TO_VICT );
        interpret ( victim, argument );
    }

    send_to_char ( "Ok.\n\r", ch );
    return;
}

/*
 * New routines by Dionysos.
 */
CH_CMD ( do_invis )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
        /* take the default path */

        if ( ch->invis_level )
        {
            ch->invis_level = 0;
            if ( !IS_TRUSTED ( ch, HEADIMP ) )
            {
                act ( "$n slowly fades into existence.", ch, NULL, NULL,
                      TO_ROOM );
            }
            else
            {
                act ( "$n appears in a blinding {z{Wflash{x!", ch, NULL, NULL,
                      TO_ROOM );
            }
            send_to_char ( "You slowly fade back into existence.\n\r", ch );
        }
        else
        {
            if ( !IS_TRUSTED ( ch, HEADIMP ) )
            {
                act ( "$n slowly fades into thin air.", ch, NULL, NULL,
                      TO_ROOM );
            }
            else
            {
                act ( "A {Wblinding white light{x envelops $n, then {z{Dvanishes{x.", ch, NULL, NULL, TO_ROOM );
            }
            send_to_char ( "You slowly vanish into thin air.\n\r", ch );
            ch->invis_level = get_trust ( ch );
        }
    else
        /* do the level thing */
    {
        level = atoi ( arg );
        if ( level < 2 || level > get_trust ( ch ) )
        {
            send_to_char
               ( "Invis level must be between 2 and your level.\n\r", ch );
            return;
        }
        else
        {
            if ( !IS_TRUSTED ( ch, HEADIMP ) )
            {
                act ( "$n slowly fades into thin air.", ch, NULL, NULL,
                      TO_ROOM );
            }
            else
            {
                act ( "A {Wblinding white light{x envelops $n, then {z{Dvanishes{x.", ch, NULL, NULL, TO_ROOM );
            }
            send_to_char ( "You slowly vanish into thin air.\n\r", ch );
            ch->reply = NULL;
            ch->invis_level = level;
        }
    }

    return;
}

CH_CMD ( do_incognito )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
        /* take the default path */

        if ( ch->incog_level )
        {
            ch->incog_level = 0;
            act ( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You are no longer cloaked.\n\r", ch );
        }
        else
        {
            ch->incog_level = get_trust ( ch );
            ch->ghost_level = 0;
            act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You cloak your presence.\n\r", ch );
        }
    else
        /* do the level thing */
    {
        level = atoi ( arg );
        if ( level < 2 || level > get_trust ( ch ) )
        {
            send_to_char
                ( "Incog level must be between 2 and your level.\n\r", ch );
            return;
        }
        else
        {
            ch->reply = NULL;
            ch->incog_level = level;
            ch->ghost_level = 0;
            act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You cloak your presence.\n\r", ch );
        }
    }

    return;
}

CH_CMD ( do_ghost )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
        /* take the default path */

        if ( ch->ghost_level )
        {
            ch->ghost_level = 0;
            act ( "$n steps out from the mist.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You step out from the mist.\n\r", ch );
        }
        else
        {
            ch->ghost_level = get_trust ( ch );
            ch->incog_level = 0;
            act ( "$n vanishes into a mist.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You vanish into a mist.\n\r", ch );
        }
    else
        /* do the level thing */
    {
        level = atoi ( arg );
        if ( level < 2 || level > get_trust ( ch ) )
        {
            send_to_char
                ( "Ghost level must be between 2 and your level.\n\r", ch );
            return;
        }
        else
        {
            ch->reply = NULL;
            ch->ghost_level = level;
            ch->incog_level = 0;
            act ( "$n vanishes into a mist.", ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You vanish into a mist.\n\r", ch );
        }
    }

    return;
}

CH_CMD ( do_holylight )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_HOLYLIGHT ) )
    {
        REMOVE_BIT ( ch->act, PLR_HOLYLIGHT );
        send_to_char ( "Holy light mode off.\n\r", ch );
    }
    else
    {
        SET_BIT ( ch->act, PLR_HOLYLIGHT );
        send_to_char ( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* prefix command: it will put the string typed on each line typed */

CH_CMD ( do_prefi )
{
    send_to_char ( "You cannot abbreviate the prefix command.\r\n", ch );
    return;
}

CH_CMD ( do_prefix )
{
    char buf[MAX_INPUT_LENGTH];

    if ( argument[0] == '\0' )
    {
        if ( ch->prefix[0] == '\0' )
        {
            send_to_char ( "You have no prefix to clear.\r\n", ch );
            return;
        }

        send_to_char ( "Prefix removed.\r\n", ch );
        free_string ( ch->prefix );
        ch->prefix = str_dup ( "" );
        return;
    }

    if ( ch->prefix[0] != '\0' )
    {
        sprintf ( buf, "Prefix changed to %s.\r\n", argument );
        free_string ( ch->prefix );
    }
    else
    {
        sprintf ( buf, "Prefix set to %s.\r\n", argument );
    }

    ch->prefix = str_dup ( argument );
}

CH_CMD ( do_mquest )
{
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Make a quest item of what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry ( ch, argument ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
    {
        REMOVE_BIT ( obj->extra_flags, ITEM_QUEST );
        act ( "$p is no longer a quest item.", ch, obj, NULL, TO_CHAR );
    }
    else
    {
        SET_BIT ( obj->extra_flags, ITEM_QUEST );
        act ( "$p is now a quest item.", ch, obj, NULL, TO_CHAR );
    }

    return;
}

CH_CMD ( do_mpoint )
{
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Make a questpoint item of what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry ( ch, argument ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_QUESTPOINT ) )
    {
        REMOVE_BIT ( obj->extra_flags, ITEM_QUESTPOINT );
        act ( "$p is no longer a questpoint item.", ch, obj, NULL, TO_CHAR );
    }
    else
    {
        SET_BIT ( obj->extra_flags, ITEM_QUESTPOINT );
        act ( "$p is now a questpoint item.", ch, obj, NULL, TO_CHAR );
    }

    return;
}

CH_CMD ( do_gset )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( ( argument[0] == '\0' ) || !is_number ( argument ) )
    {
        send_to_char ( "Goto point cleared.\n\r", ch );
        ch->pcdata->recall = 0;
        return;
    }

    ch->pcdata->recall = atol ( argument );

    send_to_char ( "Ok.\n\r", ch );

    return;
}

CH_CMD ( do_wizslap )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *pRoomIndex;
    AFFECT_DATA af;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "WizSlap whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( victim->level >= ch->level )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }
    pRoomIndex = get_random_room ( victim );

    OBJ_DATA *footwear;
    if ( ( footwear = get_eq_char ( victim, WEAR_FEET ) ) != NULL )
    {
        obj_from_char ( footwear );
	obj_to_room ( footwear, ch->in_room );
    }
    act ( "$n slaps you, sending you reeling through time and space!\n\rYou are slapped out of your footwear!\n\r", ch,
          NULL, victim, TO_VICT );
    act ( "$n slaps $N, sending $M reeling through time and space! $N leaves their footwear behind.", ch, NULL,
          victim, TO_NOTVICT );
    act ( "You send $N reeling through time and space! They are slapped out of their shoes!", ch, NULL, victim,
          TO_CHAR );
    char_from_room ( victim );
    char_to_room ( victim, pRoomIndex );
    act ( "$n crashes to the ground!", victim, NULL, NULL, TO_ROOM );
    af.where = TO_AFFECTS;
    af.type = skill_lookup ( "weaken" );
    af.level = 203;
    af.duration = 120;
    af.location = APPLY_STR;
    af.modifier = -15;
    af.bitvector = AFF_WEAKEN;
    affect_to_char ( victim, &af );
    af.where = TO_AFFECTS;
    af.type = skill_lookup ( "curse" );
    af.level = 203;
    af.duration = 120;
    af.location = APPLY_STR;
    af.modifier = -15;
    af.bitvector = AFF_CURSE;
    affect_to_char ( victim, &af );
    send_to_char ( "You feel the gods turn against you.\n\r", victim );
    do_look ( victim, "auto" );
    return;
}

CH_CMD ( do_pack )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *pack;
    OBJ_DATA *obj;
    int i;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Send a survival pack to whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( ( victim->level >= 10 ) && ( ch->level < DEMI ) )
    {
        send_to_char ( "They don't need one at thier level.\n\r", ch );
        return;
    }

    if ( !can_pack ( victim ) )
    {
        send_to_char ( "They already have a survival pack.\n\r", ch );
        return;
    }

    pack = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_PACK ), 0 );
    pack->level = 5;

    for ( i = 0; i < 7; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_A ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_B ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_C ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_D ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_E ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_F ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_G ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_H ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_I ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_J ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
/*    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_K ), 0 );
    obj->level = 5; 
    obj_to_obj ( obj, pack ); */
    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_L ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_M ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_N ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_O ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_P ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_Q ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_R ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_S ), 0 );
    obj->level = 1;
    obj_to_obj ( obj, pack );
/*    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_T ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack ); */
    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_U ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    }
/*    for ( i = 0; i < 2; i++ )
    {
        obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_V ), 0 );
        obj->level = 5;
        obj_to_obj ( obj, pack );
    } */
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_W ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );
    obj = create_object ( get_obj_index ( OBJ_VNUM_SURVIVAL_X ), 0 );
    obj->level = 5;
    obj_to_obj ( obj, pack );

    obj_to_char ( pack, victim );

    send_to_char ( "Ok.\n\r", ch );
    act ( "$p suddenly appears in your inventory.", ch, pack, victim,
          TO_VICT );
    return;
}

bool can_pack ( CHAR_DATA * ch )
{
    OBJ_DATA *object;
    bool found;

    if ( ch->desc == NULL )
        return TRUE;

    if ( ch->level > HERO )
        return TRUE;

    /*
     * search the list of objects.
     */
    found = TRUE;
    for ( object = ch->carrying; object != NULL;
          object = object->next_content )
    {
        if ( object->pIndexData->vnum == OBJ_VNUM_SURVIVAL_PACK )
            found = FALSE;
    }
    if ( found )
        return TRUE;

    return FALSE;
}

CH_CMD ( do_dupe )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH], arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;

    if ( IS_NPC ( ch ) )
        return;

    smash_tilde ( argument );

    argument = one_argument ( argument, arg );
    one_argument ( argument, arg2 );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Dupe whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        if ( victim->pcdata->dupes[0] == NULL )
        {
            send_to_char ( "They have no dupes set.\n\r", ch );
            return;
        }
        send_to_char ( "They currently have the following dupes:\n\r", ch );

        for ( pos = 0; pos < MAX_DUPES; pos++ )
        {
            if ( victim->pcdata->dupes[pos] == NULL )
                break;

            sprintf ( buf, "    %s\n\r", victim->pcdata->dupes[pos] );
            send_to_char ( buf, ch );
        }
        return;
    }

    for ( pos = 0; pos < MAX_DUPES; pos++ )
    {
        if ( victim->pcdata->dupes[pos] == NULL )
            break;

        if ( !str_cmp ( arg2, victim->pcdata->dupes[pos] ) )
        {
            found = TRUE;
        }
    }

    if ( found )
    {
        found = FALSE;
        for ( pos = 0; pos < MAX_DUPES; pos++ )
        {
            if ( victim->pcdata->dupes[pos] == NULL )
                break;

            if ( found )
            {
                victim->pcdata->dupes[pos - 1] = victim->pcdata->dupes[pos];
                victim->pcdata->dupes[pos] = NULL;
                continue;
            }

            if ( !str_cmp ( arg2, victim->pcdata->dupes[pos] ) )
            {
                send_to_char ( "Dupe removed.\n\r", ch );
                free_string ( victim->pcdata->dupes[pos] );
                victim->pcdata->dupes[pos] = NULL;
                found = TRUE;
            }
        }
        return;
    }

    for ( pos = 0; pos < MAX_DUPES; pos++ )
    {
        if ( victim->pcdata->dupes[pos] == NULL )
            break;
    }

    if ( pos >= MAX_DUPES )
    {
        send_to_char ( "Sorry, they've reached the limit for dupes.\n\r",
                       ch );
        return;
    }

    /* make a new dupe */
    victim->pcdata->dupes[pos] = str_dup ( arg2 );
    sprintf ( buf, "%s now has the dupe %s set.\n\r", victim->name, arg2 );
    send_to_char ( buf, ch );
}
    
CH_CMD ( do_cscore )
{
    int i;
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Stat whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, argument ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    
    if ( IS_NPC ( victim ) )
    {
       send_to_char ( "Why would you want to cscore mobs?\n\r", ch );
       return;
    }

    printf_to_char ( ch,
                     "\n\r{R=============================================================================={x\n\r" );
    printf_to_char ( ch, "{R|                   {x%s%s{x\n\r", IS_NPC ( victim ) ? victim->name : victim->pcdata->cname,
                     IS_NPC ( victim ) ? ", the mobile." : victim->pcdata->title );
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    printf_to_char ( ch,
                     "{R| {rRace {x:{D %-15s{R| {rAQP{x :{D %-8ld{R| {rHP{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rAge  {x:{D %-15d{R| {rIQP{x :{D %-8d{R| {rMN{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rSex  {x:{D %-15s{R| {rDamr{x:{D %-8d{R| {rMV{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rLevel{x:{D %-15d{R| {rHitr{x:{D %-8d{R| {rItems{x  :{D %-6d{x/{D%d\n\r"
                     "{R| {rClass{x:{D %-15s{R| {rPlat{x:{D %-8ld{R| {rWeight{x :{D %-6ld{x/{D%d\n\r"
                     "{R| {rAlign{x:{D %-15d{R| {rGold{x:{D %-8ld{R| {rSaves{x  :{D %-6d{x\n\r"
                     "{R| {rWimpy{x:{D %-15d{R| {rSilv{x:{D %-8ld{R| {rBank{x   :{D %ld{x\n\r",
                     IS_NPC ( victim ) ? "mobile" : race_table[victim->race].name, 
		     victim->pcdata->questpoints, victim->hit, victim->max_hit, get_age ( victim ), 
		     victim->qps, victim->mana, victim->max_mana, 
		     victim->sex == 0 ? "Sexless" : victim->sex == 1 ? "Male" : "Female", 
		     GET_DAMROLL ( victim ), victim->move, victim->max_move, victim->level, 
		     GET_HITROLL ( victim ), victim->carry_number, can_carry_n ( victim ), 
		     class_table[victim->class].name, victim->platinum, get_carry_weight ( victim ) / 10, can_carry_w ( victim ) / 10, victim->alignment, victim->gold, victim->saving_throw, victim->wimpy, 
		     victim->silver, victim->pcdata->balance );
   printf_to_char ( ch,
		     "{R| {rTrain{x:{D %-15d{R| {rPrac{x:{D %-8d{R| {rHours  {x:{D %d{x\n\r",
		     victim->train, victim->practice, ( victim->played + ( int ) ( current_time - victim->logon ) ) / 3600 );
/*    printf_to_char ( ch,                
"{R+-----------------------------------------------------------------------------{x\n\r" ); */
/* Doesn't work...hmm */ /*
if ( ch->pcdata->condition[COND_FULL] >= 0 )
{
   printf_to_char ( ch,
		     "{R| {rYour hunger is at {D%d{r, thirst {D%d{r, and you're {D%d{d% {rfull.{x\n\r",
	ch->pcdata->condition[COND_HUNGER],
	ch->pcdata->condition[COND_THIRST],
	ch->pcdata->condition[COND_FULL] );
}
if ( ch->pcdata->condition[COND_DRUNK] > 0 )
   printf_to_char ( ch,
		     "{R| {rYou are {Gd{gr{Gu{gn{Gk{g! {r({D%d{d%{r){x\n\r", ch->pcdata->condition[COND_DRUNK] );
*/

/*stats*/
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    printf_to_char ( ch,
                     "{R| {rStr{x:{D %d{x({d%d{x) {rInt{x: {D%d{x({d%d{x) {rWis{x: {D%d{x({d%d{x) {rDex{x: {D%d{x({d%d{x) {rCon{x: {D%d{x({d%d{x)\n\r",
                     victim->perm_stat[STAT_STR], get_curr_stat ( victim, STAT_STR ),
                     victim->perm_stat[STAT_INT], get_curr_stat ( victim, STAT_INT ),
                     victim->perm_stat[STAT_WIS], get_curr_stat ( victim, STAT_WIS ),
                     victim->perm_stat[STAT_DEX], get_curr_stat ( victim, STAT_DEX ),
                     victim->perm_stat[STAT_CON], get_curr_stat ( victim, STAT_CON ) );
     /*AC*/ if ( victim->level >= 1 )
    {
        printf_to_char ( ch,
                         "{R| {rPierce{x:{D %d{r   Bash{x:{D %d{x   {rSlash {x:{D %d{r   Magic{x:{D %d{x\n\r",
                         GET_AC ( victim, AC_PIERCE ), GET_AC ( victim, AC_BASH ),
                         GET_AC ( victim, AC_SLASH ), GET_AC ( victim, AC_EXOTIC ) );
    }
    for ( i = 0; i < 4; i++ )
    {
        char *temp;
        switch ( i )
        {
            case ( AC_PIERCE ):
                temp = "piercing";
                break;
            case ( AC_BASH ):
                temp = "bashing";
                break;
            case ( AC_SLASH ):
                temp = "slashing";
                break;
            case ( AC_EXOTIC ):
                temp = "magic";
                break;
            default:
                temp = "error";
                break;
        }
    }
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL ( victim ) )
    {
        printf_to_char ( ch, "{R| {rHoly Light{x: " );
        if ( IS_SET ( victim->act, PLR_HOLYLIGHT ) )
            printf_to_char ( ch, "{Don{x" );
        else
            printf_to_char ( ch, "{Doff{x" );
    }
    if ( IS_IMMORTAL ( victim ) )
    {
        printf_to_char ( ch,
                         "{r   Invis{x:{D %d   {rIncognito{x:{D %d   {rGhost{x:{D %d\n\r",
                         victim->invis_level, victim->incog_level, victim->ghost_level );
        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );
    }
    if ( !IS_NPC ( victim ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rVersusMobs{d]{r Kills{x: {D%d  {rDeaths{x:{D %d{x\n\r",
                         victim->pcdata->mkill, victim->pcdata->mdeath );
    }
    if ( !IS_NPC ( victim ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rTournament{d]{r Wins{x: {D%d  {rKills{x:{D %d  {rDeaths{x: {D%d{x\n\r",
                         victim->pcdata->twins, victim->pcdata->tkills, victim->pcdata->tdeaths );
    }
    if ( !IS_NPC ( victim ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{r1vs1 Arena{d]{r Wins{x:{D %d  {rLosses{x:{D %d{x\n\r",
                         victim->pcdata->awins, victim->pcdata->alosses );
        if ( victim->challenger != NULL )
        {
            printf_to_char ( ch,
                             "{R| {d[{r1vs1 Arena{d]{r You have been challenged by{r %s{c.{x\n\r",
                             victim->challenger->name );
        }
        if ( victim->gladiator != NULL )
        {
            printf_to_char ( ch,
                             "{R| {d[{rArena bet{d]{r You have a {Y%d platinum{r bet on %s.{x\n\r",
                             victim->pcdata->plr_wager, victim->gladiator->name );
        }
    }

    if ( !IS_NPC ( victim ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rPlayerkill{d]{r Kills{x: {D%ld  {rDeaths{x: {D%ld{x\n\r",
                         victim->pcdata->pkkills, victim->pcdata->pkdeaths );
    }
        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );
    
    if ( is_clan ( victim ) )
    {
        printf_to_char ( ch, "{R| {rClan{x: %s {rRank{x: %s{x, %s {x\n\r",
                         clan_table[victim->clan].who_name,
                         clan_rank_table[victim->clan_rank].title_of_rank[victim->
                                                                      sex],
                         is_pkill ( victim ) ? "{RPkill{c.{x" :
                         "{mNon-Pkill{c.{x" );
    }
    if ( ch->invited )
    {
        printf_to_char ( ch,
                         "{R| {RYou have been invited to join clan {x[{%s%s{x]\n\r",
                         clan_table[victim->invited].pkill ? "B" : "M",
                         clan_table[victim->invited].who_name );
    }
    if ( !IS_NPC ( ch ) )
    {
        int rcnt = roomcount ( victim );
        int arcnt = areacount ( victim );
        double rooms = top_room, percent = ( double ) rcnt / ( rooms / 100 );
        double arooms = ( double ) ( arearooms ( victim ) ), apercent =
            ( double ) arcnt / ( arooms / 100 );
        printf_to_char ( ch,
                         "{R| {rExplored{x: {D%4d{c/{d%4d{r rooms. {r({d%5.2f%%{r of the world{r) ({d%5.2f%%{r of current area)\n\r{x",
                         rcnt, top_room, percent, apercent );
    }

        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );

if ( victim->level < LEVEL_HERO && !IS_SET ( victim->act2, PLR2_NO_EXP ) )
   printf_to_char ( ch,
		     "{R| {rYou have gained a total of {D%ld{r experience, and need {D%ld{r to level{x\n\r", 
victim->exp, ( victim->level + 1 ) * exp_per_level ( victim, victim->pcdata->points ) - victim->exp );

	if ( victim->pcdata->timesplayed == 1 )
	    printf_to_char ( ch, "{R|{r This is the first time ever you've played on Shattered Dreams!{x\n\r" );
	else
	    printf_to_char ( ch, "{R|{r You have played on Shattered Dreams a total of {D%d {rtimes.{x\n\r", victim->pcdata->timesplayed );



	if ( victim->pcdata->times_rerolled <= 0 )
	{
	   printf_to_char ( ch, "{R| {rYou haven't rerolled yet. Type 'help reroll' for more info.{x\n\r" );
	}
	else if ( victim->pcdata->times_rerolled == 1 )
	{
	   printf_to_char ( ch, "{R| {rYou have rerolled once so far.{x\n\r" );
	}
	else
	{
	   printf_to_char ( ch, "{R| {rYou have rerolled a total of {D%d{r times so far.{x\n\r", victim->pcdata->times_rerolled );
	}

    if ( IS_MARRIED ( victim ) )
        printf_to_char ( ch, "{R| {rYou are happily married to {D%s{r.{x\n\r", victim->pcdata->spouse );

    if ( victim->pcdata->exiletime > 0 )
    {
    	printf_to_char ( ch, "{R| {rYou have {D%d {rminutes of {YEXILE {rremaining. {d({D%d hours{d){r.{x\n\r",
	victim->pcdata->exiletime, ( victim->pcdata->exiletime / 60 ) );
    }	
        if ( victim->pcdata->questmob == -1 || has_questobj ( victim ) )    /* killed target mob */
            send_to_char ( 
                      "{R| {rYour quest is {fALMOST{x{r complete! {rGet back to your questmaster before your time runs out!{x\n\r", ch );

        else if ( victim->pcdata->questobj > 0 )    /* questing for an object */
        {
	    OBJ_INDEX_DATA *questinfoobj;
            questinfoobj = get_obj_index ( victim->pcdata->questobj );
            if ( questinfoobj != NULL )
                printf_to_char ( ch,
                          "{R| {rYou are on a quest to recover the fabled %s{r!{x\n\r",
                          questinfoobj->short_descr );
	}
        else if ( victim->pcdata->questmob > 0 )    /* questing for a mob */
        {
	    MOB_INDEX_DATA *questinfo;
            questinfo = get_mob_index ( victim->pcdata->questmob );
            if ( questinfo != NULL )
                printf_to_char ( ch,
                          "{R| {rYou are on a quest to slay the dreaded {D%s{r!{x\n\r",
                          questinfo->short_descr );

	}

        if ( !IS_SET ( victim->act, PLR_QUESTOR ) )
        {
//            send_to_char ( "{R| {rYou aren't currently on a quest.{x\n\r", ch );
            if ( victim->pcdata->nextquest > 1 )
                printf_to_char ( ch,
                          "{R| {rThere are {D%d {rminutes remaining until you can go on another quest.{x\n\r",
                          victim->pcdata->nextquest );

            else if ( victim->pcdata->nextquest == 1 )
                printf_to_char ( ch,
                          "{R| {rThere is less than a minute remaining until you can go on another quest.{x\n\r" );
	}
        else if ( victim->pcdata->countdown > 0 )
            printf_to_char ( ch, "{R| {rTime left for current quest: {D%d{x\n\r",
                      victim->pcdata->countdown );

    printf_to_char ( ch,
                     "{R=============================================================================={x\n\r" );
}


CH_CMD ( do_award )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    long value;

    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    strcpy ( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "  award <name> <field> <value>\n\r", ch );
        send_to_char ( "\n\rField being one of the following:\n\r", ch );
        send_to_char ( "platinum hp mana move practice train iqp aqp\n\r",
                       ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number ( arg3 ) ? atol ( arg3 ) : -1;

    if ( !str_prefix ( arg2, "platinum" ) )
    {
        victim->platinum += value;
        printf_to_char ( ch, "You have awarded %s %ld platinum!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld platinum!",
                         value );
        return;
    }

    if ( !str_prefix ( arg2, "hp" ) )
    {
        if ( value < -100 || value > 100 )
        {
            send_to_char ( "Hp range is -100 to 100 hit points.\n\r", ch );
            return;
        }
        victim->max_hit += value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_hit += value;
        printf_to_char ( ch, "You have awarded %s %ld hitpoints!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld hitpoints!",
                         value );
        return;
    }

    if ( !str_prefix ( arg2, "mana" ) )
    {
        if ( value < -100 || value > 100 )
        {
            send_to_char ( "Mana range is -100 to 100 mana points.\n\r", ch );
            return;
        }
        victim->max_mana += value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_mana += value;
        printf_to_char ( ch, "You have awarded %s %ld mana!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld mana!", value );
        return;
    }

    if ( !str_prefix ( arg2, "move" ) )
    {
        if ( value < -100 || value > 100 )
        {
            send_to_char ( "Move range is -100 to 100 move points.\n\r", ch );
            return;
        }
        victim->max_move += value;
        if ( !IS_NPC ( victim ) )
            victim->pcdata->perm_move += value;
        printf_to_char ( ch, "You have awarded %s %ld move!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld move!", value );
        return;
    }

    if ( !str_prefix ( arg2, "practice" ) )
    {
        if ( value < -50 || value > 50 )
        {
            send_to_char ( "Practice range is -50 to 50 sessions.\n\r", ch );
            return;
        }
        victim->practice += value;
        printf_to_char ( ch, "You have awarded %s %ld practices!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld practices!",
                         value );
        return;
    }

    if ( !str_prefix ( arg2, "train" ) )
    {
        if ( value < -50 || value > 50 )
        {
            send_to_char
                ( "Training session range is -50 to 50 sessions.\n\r", ch );
            return;
        }
        victim->train += value;
        printf_to_char ( ch, "You have awarded %s %ld trains!\n\r",
                         victim->name, value );
        printf_to_char ( victim, "You have been awarded %ld trains!", value );
        return;
    }

    if ( !str_prefix ( arg2, "iqp" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "NPC's don't need quest points.\n\r", ch );
            return;
        }

        victim->qps += value;
        printf_to_char ( ch, "You have awarded %s %ld IQP!\n\r", victim->name,
                         value );
        printf_to_char ( victim, "You have been awarded %ld IQP!", value );
        return;
    }
    if ( !str_prefix ( arg2, "aqp" ) )
    {
        if ( IS_NPC ( victim ) )
        {
            send_to_char ( "NPC's don't need quest points.\n\r", ch );
            return;
        }

        victim->pcdata->questpoints += value;
        printf_to_char ( ch, "You have awarded %s %ld AQP!\n\r", victim->name,
                         value );
        printf_to_char ( victim, "You have been awarded %ld AQP!", value );
        return;
    }

    /*
     * Generate usage message.
     */
    do_award ( ch, "" );
    return;
}

CH_CMD ( do_bonus )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) )
    {
        send_to_char ( "Syntax: bonus <char> <Exp>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "That Player is not here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char ( "You may not bonus yourself.\n\r", ch );
        return;
    }

    if ( IS_IMMORTAL ( victim ) || victim->level >= LEVEL_IMMORTAL )
    {
        send_to_char ( "You can't bonus immortals silly!\n\r", ch );
        return;
    }

    value = atoi ( arg2 );

    if ( value < -5000 || value > 5000 )
    {
        send_to_char ( "Value range is -5000 to 5000.\n\r", ch );
        return;
    }

    if ( value == 0 )
    {
        send_to_char ( "The value must not be equal to 0.\n\r", ch );
        return;
    }

    gain_exp ( victim, value );

    sprintf ( buf, "You have bonused %s a whopping %d experience points.\n\r",
              victim->name, value );
    send_to_char ( buf, ch );

    if ( value > 0 )
    {
        sprintf ( buf, "You have been bonused %d experience points.\n\r",
                  value );
        send_to_char ( buf, victim );
    }
    else
    {
        sprintf ( buf, "You have been penalized %d experience points.\n\r",
                  value );
        send_to_char ( buf, victim );
    }

    return;
}

/* Vape command by Lucas Clav of Ash Planet (206.28.93.8:4000), you may use these commands
as long as this header remains in tact. */
CH_CMD ( do_vape )
{
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
/*    char buf[MAX_STRING_LENGTH]; */

    if ( is_name ( ch->name, "Suedoe" ) )
        return;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax: Vape <name> \n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You cannot VAPE mobiles!\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
	send_to_char ( "Haha, funny!\n\r", ch );
        return;
    }

    sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( victim->name ) );
    stop_fighting ( victim, TRUE );
/*    sprintf ( buf, "{w%s {Dflies down from the heavens.{x", ch->name );
    do_echo ( ch, buf );
    sprintf ( buf,
              "{w%s {Dlooks at {w%s{D and smiles as they shove"
              " their dagger into {w%s's {Dback.{x", ch->name, victim->name,
              victim->name );
    do_echo ( ch, buf );
    sprintf ( buf,
              "{DYou watch as {w%s's{D heart flies out their "
              "chest....{x",
              victim->name );
    do_echo ( ch, buf ); */
    force_quit ( victim, "" );
    if ( victim->level >= ANCIENT )
    {
    update_wizlist ( victim, 1 );
    update_anclist ( victim, 1 );
    }
    unlink ( strsave );
    return;
}

CH_CMD ( do_addlag )
{

    CHAR_DATA *victim;
    char arg1[MAX_STRING_LENGTH];
    int x;
    argument = one_argument ( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "addlag to who?", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They're not here.", ch );
        return;
    }

    if ( ( x = atoi ( argument ) ) <= 0 )
    {
        send_to_char ( "That makes a LOT of sense.", ch );
        return;
    }

    if ( x > 100 )
    {
        send_to_char ( "There's a limit to cruel and unusual punishment",
                       ch );
        return;
    }

/*    send_to_char
        ( "{WOh {RSHIT{Y!{x The lag monster is attacking you{Y!{R!{Y!{x\n\r",
          victim ); */
    WAIT_STATE ( victim, x );
    send_to_char ( "Adding lag now...\n\r", ch );
    return;
}

CH_CMD ( do_qspell )
{
    CHAR_DATA *vch;
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    argument = one_argument ( argument, arg );
    if ( IS_NPC ( ch ) )
        return;
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Spellup whom?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) && ( ch->level >= SQUIRE ) )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING && d->character != ch &&
                 d->character->in_room != NULL &&
                 ch->level >= d->character->ghost_level &&
                 can_see ( ch, d->character ) )
            {
                char buf[MAX_STRING_LENGTH];
                sprintf ( buf, "%s %s", d->character->name, arg );
                do_qspell ( ch, buf );
            }
        }
        return;
    }
    if ( str_cmp ( "room", arg ) )
    {
        if ( ( vch = get_char_world ( ch, arg ) ) == NULL )
        {
            send_to_char ( "They aren't here.\n\r", ch );
            return;
        }
    }
    else
        vch = ch;
    if ( !str_cmp ( "room", arg ) )
        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        {
            if ( vch == ch )
                continue;
            if ( IS_NPC ( vch ) )
                continue;
            spell_shockshield ( skill_lookup ( "shockshield" ), ch->level, ch,
                                vch, TARGET_CHAR );
            spell_fireshield ( skill_lookup ( "fireshield" ), ch->level, ch,
                               vch, TARGET_CHAR );
            spell_iceshield ( skill_lookup ( "iceshield" ), ch->level, ch,
                              vch, TARGET_CHAR );
            spell_acidshield ( skill_lookup ( "acidshield" ), ch->level, ch,
                               vch, TARGET_CHAR );
            spell_poisonshield ( skill_lookup ( "poisonshield" ), ch->level,
                                 ch, vch, TARGET_CHAR );
            spell_briarshield ( skill_lookup ( "briarshield" ), ch->level, ch,
                                vch, TARGET_CHAR );
            spell_shield ( skill_lookup ( "shield" ), ch->level, ch, vch,
                           TARGET_CHAR );
            spell_armor ( skill_lookup ( "armor" ), ch->level, ch, vch,
                          TARGET_CHAR );
            spell_sanctuary ( skill_lookup ( "sanctuary" ), ch->level, ch,
                              vch, TARGET_CHAR );
            spell_fly ( skill_lookup ( "fly" ), ch->level, ch, vch,
                        TARGET_CHAR );
            spell_frenzy ( skill_lookup ( "frenzy" ), ch->level, ch, vch,
                           TARGET_CHAR );
            spell_giant_strength ( skill_lookup ( "giant strength" ),
                                   ch->level, ch, vch, TARGET_CHAR );
            spell_bless ( skill_lookup ( "bless" ), ch->level, ch, vch,
                          TARGET_CHAR );
            spell_haste ( skill_lookup ( "haste" ), ch->level, ch, vch,
                          TARGET_CHAR );
            spell_dragon_skin ( skill_lookup ( "dragon skin" ), ch->level, ch,
                                vch, TARGET_CHAR );
            spell_dragon_wisdom ( skill_lookup ( "dragon wisdom" ), ch->level,
                                  ch, vch, TARGET_CHAR );
        }
    else
    {
        spell_shockshield ( skill_lookup ( "shockshield" ), ch->level, ch,
                            vch, TARGET_CHAR );
        spell_fireshield ( skill_lookup ( "fireshield" ), ch->level, ch, vch,
                           TARGET_CHAR );
        spell_iceshield ( skill_lookup ( "iceshield" ), ch->level, ch, vch,
                          TARGET_CHAR );
        spell_acidshield ( skill_lookup ( "acidshield" ), ch->level, ch, vch,
                           TARGET_CHAR );
        spell_poisonshield ( skill_lookup ( "poisonshield" ), ch->level, ch,
                             vch, TARGET_CHAR );
        spell_briarshield ( skill_lookup ( "briarshield" ), ch->level, ch,
                            vch, TARGET_CHAR );
        spell_shield ( skill_lookup ( "shield" ), ch->level, ch, vch,
                       TARGET_CHAR );
        spell_armor ( skill_lookup ( "armor" ), ch->level, ch, vch,
                      TARGET_CHAR );
        spell_sanctuary ( skill_lookup ( "sanctuary" ), ch->level, ch, vch,
                          TARGET_CHAR );
        spell_fly ( skill_lookup ( "fly" ), ch->level, ch, vch, TARGET_CHAR );
        spell_frenzy ( skill_lookup ( "frenzy" ), ch->level, ch, vch,
                       TARGET_CHAR );
        spell_giant_strength ( skill_lookup ( "giant strength" ), ch->level,
                               ch, vch, TARGET_CHAR );
        spell_bless ( skill_lookup ( "bless" ), ch->level, ch, vch,
                      TARGET_CHAR );
        spell_haste ( skill_lookup ( "haste" ), ch->level, ch, vch,
                      TARGET_CHAR );
        spell_dragon_skin ( skill_lookup ( "dragon skin" ), ch->level, ch,
                            vch, TARGET_CHAR );
        spell_dragon_wisdom ( skill_lookup ( "dragon wisdom" ), ch->level, ch,
                              vch, TARGET_CHAR );
    }
    return;
}

/** Function: do_pload
  * Descr   : Loads a player object into the mud, bringing them (and their
  *           pet) to you for easy modification.  Player must not be connected.
  *           Note: be sure to send them back when your done with them.
  * Returns : (void)
  * Syntax  : pload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
CH_CMD ( do_pload )
{
    DESCRIPTOR_DATA d;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];
    if ( argument[0] == '\0' )
    {
        send_to_char ( "Load who?\n\r", ch );
        return;
    }

    argument[0] = UPPER ( argument[0] );
    argument = one_argument ( argument, name );
    /* Dont want to load a second copy of a player who's allready online! */
    if ( get_char_world ( ch, name ) != NULL )
    {
        send_to_char ( "That person is allready connected!\n\r", ch );
        return;
    }

    isChar = load_char_obj ( &d, name );    /* char pfile exists? */
    if ( !isChar )
    {
        send_to_char
            ( "Load Who? Are you sure? I cant seem to find them.\n\r", ch );
        return;
    }

    d.character->desc = NULL;
    d.character->next = char_list;
    char_list = d.character;
    d.connected = CON_PLAYING;
    reset_char ( d.character );
    /* bring player to imm */
    if ( d.character->in_room != NULL )
    {
        char_to_room ( d.character, ch->in_room );  /* put in room imm is in */
    }

    act ( "$n has pulled $N from the pattern!", ch, NULL, d.character,
          TO_ROOM );
    if ( d.character->pet != NULL )
    {
        char_to_room ( d.character->pet, d.character->in_room );
        act ( "$n has entered the game.", d.character->pet, NULL, NULL,
              TO_ROOM );
    }

}

/** Function: do_punload
  * Descr   : Returns a player, previously 'ploaded' back to the void from
  *           whence they came.  This does not work if the player is actually 
  *           connected.
  * Returns : (void)
  * Syntax  : punload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
CH_CMD ( do_punload )
{
    CHAR_DATA *victim;
    char who[MAX_INPUT_LENGTH];
    argument = one_argument ( argument, who );
    if ( ( victim = get_char_world ( ch, who ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

  /** Person is legitametly logged on... was not ploaded.
   */
    if ( victim->desc != NULL )
    {
        send_to_char ( "I dont think that would be a good idea...\n\r", ch );
        return;
    }

    if ( victim->was_in_room != NULL )  /* return player and pet to orig room */
    {
        char_to_room ( victim, victim->was_in_room );
        if ( victim->pet != NULL )
            char_to_room ( victim->pet, victim->was_in_room );
    }

    save_char_obj ( victim );
    do_quit ( victim, "" );
    act ( "$n has released $N back to the Pattern.", ch, NULL, victim,
          TO_ROOM );
}

CH_CMD ( do_addapply )
{
    OBJ_DATA *obj;
    AFFECT_DATA paf;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    int affect_modify = 0, bit = 0, enchant_type, pos, i;
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, arg3 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char
            ( "Syntax for applies: addapply <object> <apply type> <value>\n\r",
              ch );
        send_to_char ( "Apply Types: hp str dex int wis con mana\n\r", ch );
        send_to_char ( "             ac move hitroll damroll saves\n\r\n\r",
                       ch );
        send_to_char
            ( "Syntax for affects: addapply <object> affect <affect name>\n\r",
              ch );
        send_to_char
            ( "Affect Names: blind invisible detect_evil detect_invis detect_magic\n\r",
              ch );
        send_to_char
            ( "              detect_hidden detect_good sanctuary faerie_fire infrared\n\r",
              ch );
        send_to_char
            ( "              curse poison protect_evil protect_good sneak hide sleep charm\n\r",
              ch );
        send_to_char
            ( "              flying pass_door haste calm plague weaken dark_vision berserk\n\r",
              ch );
        send_to_char
            ( "              shockshield fireshield iceshield slow\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "No such object exists!\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg2, "hp" ) )
        enchant_type = APPLY_HIT;
    else if ( !str_prefix ( arg2, "str" ) )
        enchant_type = APPLY_STR;
    else if ( !str_prefix ( arg2, "dex" ) )
        enchant_type = APPLY_DEX;
    else if ( !str_prefix ( arg2, "int" ) )
        enchant_type = APPLY_INT;
    else if ( !str_prefix ( arg2, "wis" ) )
        enchant_type = APPLY_WIS;
    else if ( !str_prefix ( arg2, "con" ) )
        enchant_type = APPLY_CON;
    else if ( !str_prefix ( arg2, "mana" ) )
        enchant_type = APPLY_MANA;
    else if ( !str_prefix ( arg2, "move" ) )
        enchant_type = APPLY_MOVE;
    else if ( !str_prefix ( arg2, "ac" ) )
        enchant_type = APPLY_AC;
    else if ( !str_prefix ( arg2, "hitroll" ) )
        enchant_type = APPLY_HITROLL;
    else if ( !str_prefix ( arg2, "damroll" ) )
        enchant_type = APPLY_DAMROLL;
    else if ( !str_prefix ( arg2, "saves" ) )
        enchant_type = APPLY_SAVING_SPELL;
    else if ( !str_prefix ( arg2, "affect" ) )
        enchant_type = APPLY_SPELL_AFFECT;
    else
    {
        send_to_char ( "That apply is not possible!\n\r", ch );
        return;
    }

    if ( enchant_type == APPLY_SPELL_AFFECT )
    {
        for ( pos = 0; affect_flags[pos].name != NULL; pos++ )
            if ( !str_cmp ( affect_flags[pos].name, arg3 ) )
                bit = affect_flags[pos].bit;
    }
    else
    {
        if ( is_number ( arg3 ) )
            affect_modify = atoi ( arg3 );
        else
        {
            send_to_char ( "Applies require a value.\n\r", ch );
            return;
        }
    }

    affect_enchant ( obj );
    /* create the affect */
    paf.where = TO_AFFECTS;
    paf.type = 0;
    paf.level = ch->level;
    paf.duration = -1;
    paf.location = enchant_type;
    paf.modifier = affect_modify;
    paf.bitvector = bit;
    if ( enchant_type == APPLY_SPELL_AFFECT )
    {
        /* make table work with skill_lookup */
        for ( i = 0; arg3[i] != '\0'; i++ )
        {
            if ( arg3[i] == '_' )
                arg3[i] = ' ';
        }

        paf.type = skill_lookup ( arg3 );
    }

    affect_to_obj ( obj, &paf );
    send_to_char ( "Ok.\n\r", ch );
}

CH_CMD ( do_resetxp )
{
    CHAR_DATA *victim;
    char arg[MAX_STRING_LENGTH];
    argument = one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Reset who's xp?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_world ( ch, arg ) ) )
    {
        send_to_char ( "They are not currently on.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on  a NPC.\n\r", ch );
        return;
    }

    ch->exp = exp_per_level ( ch, ch->pcdata->points ) * ch->level;
    send_to_char ( "Xp to level has been reseted.\n\r", ch );
    send_to_char ( "Your Xp to level has been reset.\n\r", victim );
    save_char_obj ( ch );
    return;
}

/*
 * Coded by: Thale (Andrew Maslin)
 * Syntax: Rename <victim> <new_name>
 * Limitations: This header must be kept with this function.  In addition,
 * this file is subject to the ROM license.  The code in this file is
 * copywritten by Andrew Maslin, 1998.  If you have a "credits" help in your
 * mud, please add the name Thale to that as credit for this function.
 */
CH_CMD ( do_rename )
{
    CHAR_DATA *victim;
    FILE *fp=NULL;
    char strsave[MAX_INPUT_LENGTH];
    char *name;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    char playerfile[MAX_INPUT_LENGTH];
    if ( !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "You don't have the power to do that.\n\r", ch );
        return;
    }

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Rename who?\n\r", ch );
        return;
    }
    if ( arg2[0] == '\0' )
    {
        send_to_char ( "What should their new name be?\n\r", ch );
        return;
    }

    arg2[0] = UPPER ( arg2[0] );
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't connected.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Use string for NPC's.\n\r", ch );
        return;
    }

    if ( !check_parse_name ( arg2 ) )
    {
        sprintf ( buf, "The name {c%s{x is {Rnot allowed{x.\n\r", arg2 );
        send_to_char ( buf, ch );
        return;
    }

    sprintf ( playerfile, "%s%s", PLAYER_DIR, capitalize ( arg2 ) );
    if ( ( fp = file_open ( playerfile, "r" ) ) != NULL )
    {
        sprintf ( buf, "There is already someone named %s.\n\r",
                  capitalize ( arg2 ) );
        send_to_char ( buf, ch );
        new_fclose ( fp );
        return;
    }

    if ( ( victim->level >= ch->level ) && ( victim->level >= ch->trust ) &&
         ( ( ch->level != HEADIMP ) || ( ch->trust != HEADIMP ) ) &&
         ( ch != victim ) )
    {
        send_to_char ( "I don't think that's a good idea.\n\r", ch );
        return;
    }

    if ( victim->position == POS_FIGHTING )
    {
        send_to_char ( "They are fighting right now.\n\r", ch );
        return;
    }

    name = str_dup ( victim->name );
    sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( victim->name ) );
    free_string ( name );
    arg2[0] = UPPER ( arg2[0] );
    free_string ( victim->name );
    victim->name = str_dup ( arg2 );
    save_char_obj ( victim );
    unlink ( strsave );
/*#if defined(unix)
    if ( IS_IMMORTAL ( victim ) )
    {
        sprintf ( strsave, "%s%s", GOD_DIR, capitalize ( name ) );
        unlink ( strsave );
    }
#endif*/
    if ( victim != ch )
    {
        sprintf ( buf, "{YNOTICE: {xYou have been renamed to {c%s{x.\n\r",
                  arg2 );
        send_to_char ( buf, victim );
    }
    send_to_char ( "Done.\n\r", ch );
    return;
}

CH_CMD ( do_doubleqp )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !double_qp )
        {
            send_to_char ( "Syntax: double <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( double_qp )
        {
            send_to_char ( "Double QP is already in affect!\n\r", ch );
            return;
        }
        double_qp = TRUE;
        send_to_char ( "Double QP is now in affect!\n\r", ch );
        do_announce ( ch, "has activated doubleqp" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !double_qp )
        {
            send_to_char ( "The global QP flag isn't on!.\n\r", ch );
            return;
        }
        double_qp = FALSE;
        send_to_char ( "The double QP flag is now off!\n\r", ch );
        do_announce ( ch, "has turned off doubleqp" );
        return;
    }
}

CH_CMD ( do_doublexp )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !double_exp )
        {
            send_to_char ( "Syntax: double <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( double_exp )
        {
            send_to_char ( "Double exp is already in affect!\n\r", ch );
            return;
        }
        double_exp = TRUE;
        send_to_char ( "Double exp is now in affect!\n\r", ch );
        do_announce ( ch, "has activated doublexp" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !double_exp )
        {
            send_to_char ( "The global exp flag isn't on!.\n\r", ch );
            return;
        }
        double_exp = FALSE;
        send_to_char ( "The double exp flag is now off!\n\r", ch );
        do_announce ( ch, "has turned off doublexp" );
        return;
    }
}

CH_CMD ( do_doubledamage )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !double_damage )
        {
            send_to_char ( "Syntax: doubledamage <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( double_damage )
        {
            send_to_char ( "Double damage is already in affect!\n\r", ch );
            return;
        }
        double_damage = TRUE;
        send_to_char ( "Double damage is now in affect!\n\r", ch );
        do_announce ( ch, "has activated double damage" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !double_damage )
        {
            send_to_char ( "Double damage is already off!.\n\r", ch );
            return;
        }
        double_damage = FALSE;
        send_to_char ( "Double damage has been turned off!\n\r", ch );
        do_announce ( ch, "has turned off double damage" );
        return;
    }
}

CH_CMD ( do_doublestance )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !double_stance )
        {
            send_to_char ( "Syntax: doublestance <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( double_stance )
        {
            send_to_char ( "Double stance gain is already in affect!\n\r", ch );
            return;
        }
        double_stance = TRUE;
        send_to_char ( "Double stance gain is now in affect!\n\r", ch );
        do_announce ( ch, "has activated double stance gain" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !double_stance )
        {
            send_to_char ( "Double stance gain is already off!.\n\r", ch );
            return;
        }
        double_stance = FALSE;
        send_to_char ( "Double stance gain has been turned off!\n\r", ch );
        do_announce ( ch, "has turned off double stance gain" );
        return;
    }
}

CH_CMD ( do_doublepracs )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !double_pracs )
        {
            send_to_char ( "Syntax: doublepracs <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( double_pracs )
        {
            send_to_char ( "Double quest practice gain is already in affect!\n\r", ch );
            return;
        }
        double_pracs = TRUE;
        send_to_char ( "Double quest practice gain is now in affect!\n\r", ch );
        do_announce ( ch, "has activated double quest practice gain" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !double_pracs )
        {
            send_to_char ( "Double quest practice gain is already off!.\n\r", ch );
            return;
        }
        double_pracs = FALSE;
        send_to_char ( "Double quest practice gain has been turned off!\n\r", ch );
        do_announce ( ch, "has turned off double quest practice gain" );
        return;
    }
}


CH_CMD ( do_nowaitquesting )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        if ( !no_wait_questing )
        {
            send_to_char ( "Syntax: nowaitquesting <on|off>.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp ( arg, "on" ) )
    {
        if ( no_wait_questing )
        {
            send_to_char ( "No wait questing is already in affect!\n\r", ch );
            return;
        }
        no_wait_questing = TRUE;
        send_to_char ( "No wait questing is now in affect!\n\r", ch );
        do_announce ( ch, "has activated no wait questing" );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
        if ( !no_wait_questing )
        {
            send_to_char ( "No wait questing is already off!.\n\r", ch );
            return;
        }
        no_wait_questing = FALSE;
        send_to_char ( "No wait questing has been turned off!\n\r", ch );
        do_announce ( ch, "has turned off no wait questing" );
        return;
    }
}

CH_CMD ( do_alldoubles )
{
    char arg[MIL];
    argument = one_argument ( argument, arg );
    if ( ( arg[0] == '\0' ) )
    {
        send_to_char ( "Syntax: alldoubles <on|off>.\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "on" ) )
    {
    	do_doublestance ( ch, "on" );
	do_doubledamage ( ch, "on" );
	do_doublepracs ( ch, "on" );
	do_doublexp ( ch, "on" );
	do_doubleqp ( ch, "on" );
	/*do_nowaitquesting ( ch, "on" ); *//* This is just overkill */
	send_to_char ( "Done.", ch );
        return;
    }
    if ( !str_cmp ( arg, "off" ) )
    {
    	do_doublestance ( ch, "off" );
	do_doubledamage ( ch, "off" );
	do_doublepracs ( ch, "off" );
	do_doublexp ( ch, "off" );
	do_doubleqp ( ch, "off" );
	//do_nowaitquesting ( ch, "off" );
	send_to_char ( "Done.", ch );
	return;
    }
}

CH_CMD ( do_questreset )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    argument = one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax: questreset <player>\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Mobs dont quest.\n\r", ch );
        return;
    }

/*  if (IS_SET(victim->act,PLR_QUESTOR))
  REMOVE_BIT(victim->act,PLR_QUESTOR);
  victim->pcdata->questgiver    = NULL;
  victim->pcdata->countdown     = 0;
  victim->pcdata->questmob      = 0;
  victim->pcdata->questobj      = 0;
  victim->pcdata->nextquest     = 0;                                         
*/
    end_quest ( victim, 0 );
    printf_to_char ( victim, "%s has cleared your quest.\n\r",
                     PERS ( ch, victim ) );
    printf_to_char ( ch, "You clear %s's quest.\n\r", victim->name );
    return;
}

CH_CMD ( do_astat )
{

    if ( arena == FIGHT_OPEN )
        send_to_char ( "Arena is [CLEAR]\n\r", ch );
    if ( arena == FIGHT_START )
        send_to_char ( "A challenge has been started.\n\r", ch );
    if ( arena == FIGHT_BUSY )
        send_to_char ( "Arena is [BUSY]\n\r", ch );
    if ( arena == FIGHT_LOCK )
        send_to_char ( "Arena is [LOCKED]\n\r", ch );
    return;
}

CH_CMD ( do_tclear )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    arena = FIGHT_OPEN;
//    send_to_char ( "Arena now set [CLEARED]\n\r", ch );
    sprintf ( buf, "{W[{RArena{W]{x The arena has been opened.\n\r" );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character )
        {
            send_to_char ( buf, d->character );
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGER ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGER );
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGED ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGED );
            if ( IS_SET ( d->character->in_room->room_flags, ROOM_ARENA ) )
            {
		stop_fighting ( d->character, TRUE );
                char_from_room ( d->character );
                char_to_room ( d->character,
                               get_room_index ( ROOM_VNUM_TEMPLE ) );
		send_to_char ( "Your arena fight has been aborted, a tournament is starting!\n\r", d->character );
		do_look ( d->character, "auto" );		
            }
        }
    }
}

CH_CMD ( do_aclear )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    arena = FIGHT_OPEN;
    send_to_char ( "Arena now set [CLEARED]\n\r", ch );
    sprintf ( buf, "{W[{RArena{W]{x The arena has been opened.\n\r" );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character )
        {
            send_to_char ( buf, d->character );
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGER ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGER );
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGED ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGED );
            if ( IS_SET ( d->character->act2, PLR2_MASS_ARENA ) )
                REMOVE_BIT ( d->character->act2, PLR2_MASS_ARENA );
            if ( IS_SET ( d->character->act2, PLR2_MASS_JOINER ) )
                REMOVE_BIT ( d->character->act2, PLR2_MASS_JOINER );
            if ( IS_SET ( d->character->in_room->room_flags, ROOM_ARENA ) )
            {
                char_from_room ( d->character );
                char_to_room ( d->character,
                               get_room_index ( ROOM_VNUM_TEMPLE ) );
		do_look ( d->character, "auto" );		
            }
        }
    }
    is_mass_arena = FALSE;
    is_mass_arena_fight = FALSE;
    mass_arena_ticks = 0;
    mass_arena_valid_ticks = 0;
    mass_arena_players_joined = 0;
    arena_countdown = 0;
}


CH_CMD ( do_abusy )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    arena = FIGHT_BUSY;
    send_to_char ( "Arena now set [BUSY]\n\r", ch );
    sprintf ( buf, "{W[{RArena{W]{x The arena is now busy.\n\r" );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character )
        {
            send_to_char ( buf, d->character );
        }
    }
    arena_countdown = 0;
}

CH_CMD ( do_alock )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    arena = FIGHT_LOCK;
    send_to_char ( "Arena now set [LOCKED]\n\r", ch );
    sprintf ( buf, "{W[{RArena{W]{x The arena has been locked.\n\r" );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character )
        {
            send_to_char ( buf, d->character );
        }
    }
    arena_countdown = 0;
}

CH_CMD ( do_update )            /* by Maniac */
{
    if ( argument[0] == '\0' )  /* No options ??? */
    {
        send_to_char ( "The following options are available:\n\r", ch );
        send_to_char ( "   bank: Update the share_value.\n\r", ch );
	send_to_char ( "   clans: Update the clan list.\n\r", ch );
        return;
    }

    if ( !str_prefix ( argument, "bank" ) )
    {
/*     if ( time_info.hour < 9 || time_info.hour > 17 )
           {
           send_to_char( "Sorry, the market's closed.\n\r", ch );
           return;
           } */
        bank_update (  );
        send_to_char ( "Ok...bank updated.\n\r", ch );
        return;
    }

    if ( !str_prefix ( argument, "clans" ) )
    {
        load_clanlist (  );
        send_to_char ( "Ok...clans/clanlist updated.\n\r", ch );
        return;
    }

    return;
}

/* Written by takeda (takeda@mathlab.sunysb.edu) *//*
CH_CMD ( do_avator ) 
   {  
   char buf[MAX_STRING_LENGTH];      
   char arg1[MAX_INPUT_LENGTH];      
   OBJ_DATA *obj_next;                       
   OBJ_DATA *obj;                    
   int level;                                
   int iLevel;                               

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' || !is_number( arg1 ) )
   {
   send_to_char( "Syntax: avatar <level>.\n\r", ch );
   return;
   }

   if ( IS_NPC(ch) )
   {
   send_to_char( "Not on NPC's.\n\r", ch );
   return;
   }

   if ( ( level = atoi( arg1 ) ) < 1 || level > MAX_LEVEL )
   {
   sprintf(buf, "Level must be 1 to %d.\n\r", MAX_LEVEL );
   send_to_char( buf, ch );
   return;
   }

   if ( level > get_trust( ch ) )
   {
   send_to_char( "Limited to your trust level.\n\r", ch );
   sprintf(buf, "Your Trust is %d.\n\r",ch->trust);
   send_to_char(buf,ch);
   return;
   }
   if(ch->trust == 0)
   {
   ch->trust = ch->level;
   }

   if ( level <= ch->level )
   {
   int temp_prac;

   send_to_char( "Lowering a player's level!\n\r", ch );
   send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r",ch );
   temp_prac = ch->practice;
   ch->level    = 1;
   ch->exp      = exp_per_level(ch,ch->pcdata->points);
   ch->max_hit  = 20;
   ch->max_mana = 100;
   ch->max_move = 100;
   ch->practice = 0;
   ch->hit      = ch->max_hit;
   ch->mana     = ch->max_mana;
   ch->move     = ch->max_move;
   advance_level( ch );
   ch->practice = temp_prac;
   }
   else
   {
   send_to_char( "Raising a player's level!\n\r", ch );
   send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", ch);
   }

   for ( iLevel = ch->level ; iLevel < level; iLevel++ )
   {
   ch->level += 1;
   advance_level( ch);
   }
   sprintf(buf,"You are now level %d.\n\r",ch->level);
   send_to_char(buf,ch);
   ch->exp   = exp_per_level(ch,ch->pcdata->points)
   * UMAX( 1, ch->level );

   if(ch->level < 103)
   {
   for (obj = ch->carrying; obj; obj = obj_next);
   }
   obj_next = obj->next_content;

   if (obj->wear_loc != WEAR_NONE && can_see_obj (ch, obj))
   remove_obj (ch, obj->wear_loc, TRUE);

   }
   }
   save_char_obj(ch);  
   return; 
   }
*/

/*void do_ifollow( CHAR_DATA *ch, char *argument )*/
CH_CMD ( do_ifollow )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch))
    {
      send_to_char("Sorry, NPC's can't do this.\n\r",ch);
      return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Follow whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "NPC's cannot be tracked\n\r", ch);
        return;
    }

    if (ch->pcdata->tracking != NULL)
    {
      if( ch->pcdata->tracking == victim)
      {
	send_to_char("You are already tracking them.\n\r",ch);
	return;
      }
      sprintf(buf,"You stop tracking %s.\n\r", ch->pcdata->tracking->name );
      send_to_char(buf,ch);
      ch->pcdata->tracking->pcdata->track_count--;
      ch->pcdata->tracking = NULL;
      if(victim == ch) return;

    }
    else if(victim == ch)
    {
      send_to_char("You can't follow yourself.\n\r",ch);
      return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    victim->pcdata->track_count++;
    ch->pcdata->tracking = victim;
    sprintf(buf,"You start tracking %s\n\r", victim->name );
    send_to_char( buf, ch );
    return;
}

CH_CMD ( do_arealinks )
{
	FILE *fp=NULL;
//	BUFFER *buffer;
	BUFFER2 *buffer;
	AREA_DATA *parea;
	EXIT_DATA *pexit;
	ROOM_INDEX_DATA *to_room;
	ROOM_INDEX_DATA *from_room;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int vnum = 0;
	int iHash, door;
	bool found = FALSE;

	/* To provide a convenient way to translate door numbers to words */
	static char * const dir_name[] = {"north","east","south","west","up","down"
};

   if (ch->pcdata->security < 8)
		{
		stc("You don't have the security to do this.\n\r",ch);
		return;
		}

	//buffer = new_buf();
	buffer = buffer_new ( 1337 );
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	/* First, the 'all' option */
	if (!str_cmp(arg1,"all"))
	{
		/*
		 * If a filename was provided, try to open it for writing
		 * If that fails, just spit output to the screen.
		 */
		if (arg2[0] != '\0')
		{
//			fclose(fpReserve);
			if( (fp = fopen(arg2, "w")) == NULL)
			{
				stc("Error opening file, printing to screen.\n\r",ch);
				new_fclose(fp);
//				fpReserve = fopen(NULL_FILE, "r");
				fp = NULL;
			}
		}
		else
			fp = NULL;

		/* Open a buffer if it's to be output to the screen */
		if (!fp)
			//buffer = new_buf();
			buffer = buffer_new ( 1337 );

		/* Loop through all the areas */
		for (parea = area_first; parea != NULL; parea = parea->next)
		{
			/* First things, add area name  and vnums to the buffer */
			sprintf(buf, "*** %s (%ld to %ld) ***\n\r", parea->name, parea->min_vnum, parea->max_vnum);
			fp ? fprintf(fp, buf) : buffer_strcat(buffer, buf);

			/* Now let's start looping through all the rooms. */
			found = FALSE;
			for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
			{
				for( from_room = room_index_hash[iHash];
					 from_room != NULL;
					 from_room = from_room->next )
				{
					/*
					 * If the room isn't in the current area,
					 * then skip it, not interested.
					 */
					if ( from_room->vnum < parea->min_vnum
					||   from_room->vnum > parea->max_vnum )
						continue;

					/* Aha, room is in the area, lets check all directions */
					for (door = 0; door < 6 ; door++)
					{
						/* Does an exit exist in this direction? */
						if( (pexit = from_room->exit[door]) != NULL )
						{
							to_room = pexit->u1.to_room;

							/*
							 * If the exit links to a different area
							 * then add it to the buffer/file
							 */
							if( to_room != NULL
							&&  (to_room->vnum < parea->min_vnum
							||   to_room->vnum > parea->max_vnum) )
							{
								found = TRUE;
								sprintf(buf, "    (%ld) links %s to %s (%ld)\n\r"
,
									from_room->vnum, dir_name[door],
									to_room->area->name, to_room->vnum);

								/* Add to either buffer or file */
								if(fp == NULL)
									buffer_strcat(buffer, buf);
								else
									fprintf(fp, buf);
							}
						}
					}
				}
			}

			/* Informative message for areas with no external links */
			if (!found)
				buffer_strcat(buffer, "    No links to other areas found.\n\r");
		}

		/* Send the buffer to the player */
		if (!fp)
		{
			//page_to_char(buf_string(buffer), ch);
			page_to_char ( buffer_string ( buffer ), ch );
			//free_buf(buffer);
			buffer_free ( buffer );
		}
		/* Or just clean up file stuff */
		else
		{
			new_fclose(fp);
//			fpReserve = fopen(NULL_FILE, "r");
		}

		return;
	}

	/* No argument, let's grab the char's current area */
	if(arg1[0] == '\0')
	{
		parea = ch->in_room ? ch->in_room->area : NULL;

		/* In case something wierd is going on, bail */
		if (parea == NULL)
		{
			stc("You aren't in an area right now, funky.\n\r",ch);
			return;
		}
	}
	/* Room vnum provided, so lets go find the area it belongs to */
	else if(is_number(arg1))
	{
		vnum = atoi(arg1);

		/* Hah! No funny vnums! I saw you trying to break it... */
		if (vnum <= 0 || vnum > 65536)
		{
			stc("The vnum must be between 1 and 65536.\n\r",ch);
			return;
		}

		/* Search the areas for the appropriate vnum range */
		for (parea = area_first; parea != NULL; parea = parea->next)
		{
			if(vnum >= parea->min_vnum && vnum <= parea->max_vnum)
				break;
		}

		/* Whoops, vnum not contained in any area */
		if (parea == NULL)
		{
			stc("There is no area containing that vnum.\n\r",ch);
			return;
		}
	}
	/* Non-number argument, must be trying for an area name */
	else
	{
		/* Loop the areas, compare the name to argument */
		for(parea = area_first; parea != NULL; parea = parea->next)
		{
			if(!str_prefix(arg1, parea->name))
				break;
		}

		/* Sorry chum, you picked a goofy name */
		if (parea == NULL)
		{
			stc("There is no such area.\n\r",ch);
			return;
		}
	}

	/* Just like in all, trying to fix up the file if provided */
	if (arg2[0] != '\0')
	{
//		fclose(fpReserve);
		if( (fp = fopen(arg2, "w")) == NULL)
		{
			stc("Error opening file, printing to screen.\n\r",ch);
			new_fclose(fp);
//			fpReserve = fopen(NULL_FILE, "r");
			fp = NULL;
		}
	}
	else
		fp = NULL;

	/* And we loop the rooms */
	for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for( from_room = room_index_hash[iHash];
			 from_room != NULL;
			 from_room = from_room->next )
		{
			/* Gotta make sure the room belongs to the desired area */
			if ( from_room->vnum < parea->min_vnum
			||   from_room->vnum > parea->max_vnum )
				continue;

			/* Room's good, let's check all the directions for exits */
			for (door = 0; door < 6; door++)
			{
				if( (pexit = from_room->exit[door]) != NULL )
				{
					to_room = pexit->u1.to_room;

					/* Found an exit, does it lead to a different area? */
					if( to_room != NULL
					&&  (to_room->vnum < parea->min_vnum
					||   to_room->vnum > parea->max_vnum) )
					{
						found = TRUE;
						sprintf(buf, "%s (%ld) links %s to %s (%ld)\n\r", parea->name, 
						from_room->vnum, dir_name[door], to_room->area->name, to_room->vnum);

						/* File or buffer output? */
						if(fp == NULL)
							stc(buf, ch);
						else
							fprintf(fp, buf);
					}
				}
			}
		}
	}

	/* Informative message telling you it's not externally linked */
	if(!found)
	{
		stc("No links to other areas found.\n\r",ch);
		/* Let's just delete the file if no links found */
		if (fp)
			unlink(arg2);
		return;
	}

	/* Close up and clean up file stuff */
	if(fp)
	{
		new_fclose(fp);
//		fpReserve = fopen(NULL_FILE, "r");
	}

}

/*void do_jail( CHAR_DATA *ch, char *argument )*/
CH_CMD ( do_jail )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Jail whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    sprintf(buf,"$N jails %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    send_to_char( "Ok. Remember to set the jail time with the 'set char <name> jailtime <time in minutes>' command.\n\r", ch );
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_corner( ch, victim->name);
    do_freeze( ch, victim->name);
    send_to_char ( "You have been jailed and frozen for breaking the rules. Press return to see when your jailtime is over.", victim );
//    do_disconnect( ch, victim->name);
    return;
}


 /* void do_shortestpath(CHAR_DATA *ch, char *argument) */
CH_CMD ( do_shortestpath )
{
/* AREA_DATA *get_area_data        args( ( int vnum ) ); */

		ROOM_INDEX_DATA *pRoomIndex, *queueIn, *queueOut, *source, *destination;
/* extern  ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH]; */
		int iHash;
		AREA_DATA *pArea;
		EXIT_DATA *pexit, *pexit2;
		int door, door2;
	const char *shortdir_name[] = {"n","e","s","w","u","d"};
		int fArea = 0; //fArea is 0 for no area, nonzero for area vnum
		char buf[MAX_STRING_LENGTH];
		char buf2[MAX_STRING_LENGTH];
	destination = find_location( ch, argument );
	pArea = get_area_data( fArea ) ;
#ifdef DEBUG
		Debug ("do_shortestpath");
#endif

	//Use a breadth-first search to find shortest path.
	if ( argument[0] == '\0' ) {
		stc( "Syntax: map <destination player, mob, or vnum>\n\r", ch );
		stc( "        map area <area vnum>\n\r", ch );
		return;
	}
	//First, find source and destination rooms:
	if ( ( source = ch->in_room) == NULL ) {
		stc( "You must be somewhere to go anywhere.\n\r", ch );
		return;
	}
	//Check if we're looking for area vnum
	if (!str_prefix("area ",argument ))  {
		argument = one_argument(argument,buf); // strip "area" and discard buf
		if ( is_number( argument ) ) {
			fArea = atoi( argument );
			if ( !pArea ) {
				stc( "That {Garea vnum{x does not exist.\n\r", ch );
				return;
			}
			fArea++; // since vnums start at 0 this makes it nonzero.
		}
	}
	 else
if  ( destination == NULL )
{
		stc( "No such destination.\n\r", ch );
		return;
	}
	if ( (fArea && source->area == pArea) || source == destination ) {
		stc( "No need to walk to get there!\n\r", ch );
		return;
	}

	//Initialize: set distance of every room to a very large number, null links and queue
	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoomIndex  = room_index_hash[iHash];
			  pRoomIndex != NULL;
			  pRoomIndex  = pRoomIndex->next ) {
			pRoomIndex->distance_from_source = 32765;
			pRoomIndex->shortest_from_room = NULL;
			pRoomIndex->shortest_next_room = NULL;
		}
	}
	//Now set source distance to 0 and put it on the "search" queue
	source->distance_from_source = 0;
	queueIn = source;
	//Now, set distance for all adjacent rooms to search room + 1 until destination.
	//If destination not found, put each unsearched adjacent room on queue and repeat
	for ( queueOut = source; queueOut; queueOut = queueOut->shortest_next_room)
 {
		//for each exit to search room:
		for ( door = 0; door < MAX_DIR ; door++ ) {
			if ( ( pexit = queueOut->exit[door] ) != NULL
				&& pexit->u1.to_room != NULL ) {
				//if we haven't looked here, set distance and add to search list
				if (pexit->u1.to_room->distance_from_source >
					queueOut->distance_from_source + 1) {
					pexit->u1.to_room->distance_from_source =
						queueOut->distance_from_source + 1;
					//if we've found destination, we're done!
					if ((fArea && pexit->u1.to_room->area == pArea) ||
						pexit->u1.to_room == destination) {
						int count = 1;
						char buf3[3];
						// print the directions in reverse order as we walk back
						sprintf(buf3,"%s",shortdir_name[door]);
						sprintf(buf2,"%s",buf3);
						for (pRoomIndex = queueOut ; pRoomIndex->shortest_from_room;
								pRoomIndex = pRoomIndex->shortest_from_room) {
							for ( door2 = 0; door2 < MAX_DIR ; door2++ ) {
								if ( ( pexit2 = 
pRoomIndex->shortest_from_room->exit[door2] ) != NULL
										&& pexit2->u1.to_room == 
pRoomIndex ) {
									if (!str_cmp(shortdir_name[door2],buf3)) 
{
										count++;
									} else {
										sprintf(buf,"%s",buf2);
										if (count > 1) {
											
sprintf(buf2,"%s%d%s",shortdir_name[door2],count,buf);
										} else {
				sprintf(buf2,"%s,%s",shortdir_name[door2],buf);
										}
										count = 1;
										
sprintf(buf3,"%s",shortdir_name[door2]);
									}
								}
							}
						}
						if (count > 1) {
							sprintf(buf,"%s",buf2);
							sprintf(buf2,"%d%s",count,buf);
						}
						if (fArea) {
							sprintf(buf,"Shortest path to %s from %s is %d steps: %s.\n\r",
										pArea->name, source->name,
										pexit->u1.to_room->distance_from_source,buf2);
						} else {
							sprintf(buf,"Shortest path is %d steps: %s.\n\r",		
pexit->u1.to_room->distance_from_source,buf2);
						}
						stc( buf, ch );
						return;
					}
					//Didn't find destination, add to queue
					pexit->u1.to_room->shortest_from_room = queueOut;
					queueIn->shortest_next_room = pexit->u1.to_room;
					queueIn = pexit->u1.to_room;
				}
			}
		}
	}
	stc( "No path to destination.\n\r", ch );
	return;
}

/* void do_otype(CHAR_DATA *ch, char *argument) */
CH_CMD ( do_otype )
{
	int type;
	int type2;
	int vnum=1;
	char buf[MAX_STRING_LENGTH];
	//char buffer[36 * MAX_STRING_LENGTH];
	BUFFER2 *buffer;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char *item;
	OBJ_INDEX_DATA *obj;
	bool found;

	item = one_argument(argument, arg1);
	one_argument ( item , arg2);
	found = FALSE;
	//buffer [0] = '\0';
	buffer = buffer_new ( 1337 );

	if (ch->pcdata->security < 9)
		{
		stc("You don't have the security to be here, I'm sorry\n\r",ch);
		return;
		}

	if (arg1[0] == '\0')
		{
		stc("Type 'Help Otype' for usage\n\r",ch);
		return;
		}
	type2 = 0;
	if ((!str_cmp(arg1,"armor") || !str_cmp(arg1,"weapon"))
		&& (!str_cmp(arg2,"\0")))

		{
		stc("Type 'Help Otype' for proper usage.\n\r",ch);
		return;
		}
	else if (!str_cmp(arg1,"armor"))
		{
		type = flag_value(type_flags,arg1);
		if ((type2 = flag_value(wear_flags,arg2)) == NO_FLAG)
		  {
				stc("No such armor type.\n\r",ch);
				return;
		  }
		}

	else if (!str_cmp(arg1,"weapon"))

		{

		type = flag_value(type_flags,arg1);

		if ((type2 = flag_value(weapon_class,arg2)) == NO_FLAG)

		  {

				stc("No such weapon type.\n\r",ch);

				return;

		  }

		}

	else

		{

			if((type = flag_value(type_flags,arg1)) == NO_FLAG)

				{

				stc("Unknown Type.\n\r", ch);

				return;

				}

		}
	for(;vnum <= top_vnum_obj; vnum++)

		{

		if((obj=get_obj_index(vnum)) != NULL)

			{

			if((obj->item_type == type && type2 == 0
				&& str_cmp(arg1,"weapon") && str_cmp(arg1,"armor"))
			|| (obj->item_type == type && obj->value[0] == type2
				&& str_cmp(arg1,"armor"))
			|| (obj->item_type == type && IS_SET(obj->wear_flags,type2)
				&& str_cmp(arg1,"weapon")))
				{

					sprintf(buf, "%35s - %5d - Area [%d]\n\r", obj->short_descr, vnum, 
obj->area->vnum);
					found = TRUE;
					//strcat(buffer,buf);
					buffer_strcat ( buffer, buf );
				}
			}
		}

	if (!found)
		stc("No objects of that type exist\n\r",ch);

	else

			page_to_char ( buffer_string ( buffer ), ch );
}

CH_CMD ( do_skillstat )
{
	//BUFFER *buffer;
	BUFFER2 *buffer;
/*	char arg[MAX_INPUT_LENGTH]; */
	char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
	char skill_columns[LEVEL_HERO + 1];
	int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
	bool found = FALSE;
/*	char buf[MAX_STRING_LENGTH]; */
	CHAR_DATA *victim;

	if (IS_NPC(ch))
	return;
/*
	if ( arg[0] == '\0' )
	{
		stc( "List skills for whom?\n\r", ch );
		return;
	}
*/

	if ( argument[0] == '\0' )
	{
		stc( "List skills for whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, argument ) ) == NULL )
	{
		stc( "They aren't here.\n\r", ch );
		return;
	}

	if (IS_NPC(victim))
	{
		stc( "Use this for skills on players.\n\r", ch );
		return;
	}

	/* initialize data */
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		skill_columns[level] = 0;
		skill_list[level][0] = '\0';
	}

	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if (skill_table[sn].name == NULL )
		break;

		if ((level = skill_table[sn].skill_level[victim->class]) < LEVEL_HERO + 1
		&&  level >= min_lev && level <= max_lev
		&&  (skill_table[sn].spell_fun == spell_null)
		&&  victim->pcdata->learned[sn] > 0)
		{
			char buf[MAX_STRING_LENGTH]; 
			found = TRUE;
			level = skill_table[sn].skill_level[victim->class];
			if (victim->level < level)
				sprintf(buf,"%-18s n/a      ", skill_table[sn].name);
			else
				sprintf(buf,"%-18s %3d%%      ",skill_table[sn].name,
				victim->pcdata->learned[sn]);

			if (skill_list[level][0] == '\0')
				sprintf(skill_list[level],"\n\rLevel %3d: %s",level,buf);
			else /* append */
			{
				if ( ++skill_columns[level] % 2 == 0)
					strcat(skill_list[level],"\n\r           ");
					strcat(skill_list[level],buf);
			}
		}
	}

	/* return results */

	if (!found)
	{
		stc("No skills found.\n\r",ch);
		return;
	}

	//buffer = new_buf();
	buffer = buffer_new ( 1337 );
	for (level = 0; level < LEVEL_HERO + 1; level++)
		if (skill_list[level][0] != '\0')
			//add_buf(buffer,skill_list[level]);
			buffer_strcat ( buffer, skill_list[level] );
			//add_buf(buffer,"\n\r");
			buffer_strcat ( buffer, "\n\r" );
			//page_to_char(buf_string(buffer),ch);
			page_to_char ( buffer_string ( buffer ), ch );
			//free_buf(buffer);
			buffer_free ( buffer );
}

CH_CMD ( do_spellstat )
{
	//BUFFER *buffer;
	BUFFER2 *buffer;
	char buff[100];
	char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
	char spell_columns[LEVEL_HERO + 1];
	int sn, gn, col, level, min_lev = 1, max_lev = LEVEL_HERO, mana;
	bool found = FALSE;
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;


	if (IS_NPC(ch))
	return;

	if ( argument[0] == '\0' )
	{
		stc( "List spells for whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, argument ) ) == NULL )
	{
		stc( "They aren't here.\n\r", ch );
		return;
	}

	if (IS_NPC(victim))
	{
		stc( "Use this for skills on players.\n\r", ch );
		return;
	}

	/* groups */

	col = 0;

	for (gn = 0; gn < MAX_GROUP; gn++)
	{
		if (group_table[gn].name == NULL)
		break;
		if (victim->pcdata->group_known[gn])
		{
			sprintf(buff,"%-20s ",group_table[gn].name);
			stc(buff,ch);
			if (++col % 3 == 0)
				stc("\n\r",ch);
		}
	}
	if ( col % 3 != 0 )
	{
		stc( "\n\r", ch );
		sprintf(buff,"Creation points: %d\n\r",victim->pcdata->points);
		stc(buff,ch);
	}

	/* initialize data */
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		spell_columns[level] = 0;
		spell_list[level][0] = '\0';
	}

	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if (skill_table[sn].name == NULL )
		break;

		if ((level = skill_table[sn].skill_level[victim->class]) < LEVEL_HERO + 1
		&&  level >= min_lev && level <= max_lev
		&&  skill_table[sn].spell_fun != spell_null
		&&  victim->pcdata->learned[sn] > 0)
		{
			found = TRUE;
			level = skill_table[sn].skill_level[victim->class];

			if (victim->level < level)
				sprintf(buf,"%-17s   n/a        ", skill_table[sn].name);
			else
			{
				mana = UMAX(skill_table[sn].min_mana,
				100/(2 + victim->level - level));
				sprintf(buf,"%-17s  %3dm (%3d%%) ",skill_table[sn].name,mana,
					 victim->pcdata->learned[sn]);
			}

			if (spell_list[level][0] == '\0')
				sprintf(spell_list[level],"\n\rLevel %3d: %s",level,buf);
			else /* append */
			{
				if ( ++spell_columns[level] % 2 == 0)
				strcat(spell_list[level],"\n\r           ");
				strcat(spell_list[level],buf);
			}
		}
	}

	/* return results */

	if (!found)
	{
		stc("No spells found.\n\r",ch);
		return;
	}

	//buffer = new_buf();
	buffer = buffer_new ( 1337 );
	for (level = 0; level < LEVEL_HERO + 1; level++)
		if (spell_list[level][0] != '\0')
			//add_buf(buffer,spell_list[level]);
			buffer_strcat ( buffer, spell_list[level] );
			//add_buf(buffer,"\n\r");
			buffer_strcat ( buffer, "\n\r" );
			//page_to_char(buf_string(buffer),ch);
			page_to_char ( buffer_string ( buffer ), ch );
			//free_buf(buffer);
			buffer_free ( buffer );
}

CH_CMD ( do_vlist )
{
	char 		arg1[MAX_INPUT_LENGTH];
	//BUFFER		*buffer;
	BUFFER2		*buffer;
	AREA_DATA	*pArea;
	int		vnum;
	int		count;
	int		count2;
	int		num_array[327];
	int		col;
	char		buf[24 * MAX_STRING_LENGTH];

	col = 0;

	for (count = 0; count <328; count++)
	{
		num_array[count] = 0;
	}

	argument = one_argument(argument, arg1);

	if (ch->pcdata->security < 9)
	{
		stc("You don't have the security to be here, I'm sorry\n\r",ch);
		return;
	}

	if (!str_cmp(arg1, "all") )
	{
		//buffer = new_buf();
		buffer = buffer_new ( 1337 );

//		add_buf(buffer, "Available Vnums throughout the MUD:\n\r");
		buffer_strcat(buffer, "Available Vnums throughout the MUD:\n\r");

		for (pArea = area_first; pArea != NULL; pArea = pArea->next)
		{
			count = pArea->min_vnum;
			count2 = pArea->max_vnum;
			count /= 100;
			count2 /= 100;
			num_array[count] = 1;
			count = count + 1;
			while (count < count2)
			{
				num_array[count] = 1;
				count = count + 1;
			}


		}
		for (count = 0; count <327; count++)
		{
			if (num_array[count] != 1)
			{
				sprintf(buf, "%-5d - %-5d -    ",
					count * 100,
					(count * 100) + 99);
				//add_buf(buffer, buf);
				buffer_strcat ( buffer, buf );
				col++;
				if (col % 4 == 0)
					//add_buf(buffer, "\n\r");
					buffer_strcat ( buffer, "\n\r" );
			}
		}
		if (col % 4 != 0)
			//add_buf(buffer, "\n\r");
			buffer_strcat ( buffer, "\n\r" );
		//page_to_char(buf_string(buffer), ch);
		page_to_char ( buffer_string ( buffer ), ch );

		//free_buf(buffer);
		buffer_free ( buffer );
		return;
	}
	else
	{
		//buffer = new_buf();
		buffer = buffer_new ( 1337 );

		sprintf(buf,"Available Room Vnums:\n\r");
		//add_buf(buffer, buf);
		buffer_strcat ( buffer, buf );

		for (vnum = ch->in_room->area->min_vnum;
			vnum < ch->in_room->area->max_vnum; vnum++)
		{
			if ( !get_room_index(vnum) )
			{
				count = vnum;
				count2 = count + 1;

				if ( !get_room_index(count2) )
				{
					while ( (!get_room_index(count2) )
							 && (count2 != ch->in_room->area->max_vnum + 1) )
					{
						count2 = count2 + 1;
					}

					sprintf(buf, "%5d - %-5d ", count, count2 - 1 );
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );
					vnum = count2;
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, "\n\r" );
				}
				else
				{
					sprintf(buf, "   %5d      ", vnum);
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, buf );
				}
			}
		}
		if (col % 6 != 0)
			//add_buf(buffer, "\n\r");
			buffer_strcat ( buffer, "\n\r" );
		col = 0;

		//add_buf(buffer, "\n\rAvailable Object Vnums:\n\r");
		buffer_strcat ( buffer, "\n\r Available Object vnums:\n\r" );

		for (vnum = ch->in_room->area->min_vnum;
			vnum < ch->in_room->area->max_vnum; vnum++)
		{
			if ( !get_obj_index(vnum) )
			{
				count = vnum;
				count2 = count + 1;

				if ( !get_obj_index(count2) )
				{
					while ( !get_obj_index(count2)
							 && (count2 != ch->in_room->area->max_vnum + 1) )
					{
						count2 = count2 + 1;
					}

					sprintf(buf, "%5d - %-5d ", count, count2 - 1 );
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );
					vnum = count2;
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, "\n\r" );
				}
				else
				{
					sprintf(buf, "   %5d      ", vnum);
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );	
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, "\n\r" );
				}
			}
		}
		if (col % 6 != 0)
			//add_buf(buffer, "\n\r");
			buffer_strcat ( buffer, "\n\r" );
		col = 0;

		//add_buf(buffer, "\n\rAvailable Mob Vnums:\n\r");
		buffer_strcat ( buffer, "\n\rAvailable Mob Vnums:\n\r" );

		for (vnum = ch->in_room->area->min_vnum;
			vnum < ch->in_room->area->max_vnum; vnum++)
		{
			if ( !get_mob_index(vnum) )
			{
				count = vnum;
				count2 = count + 1;

				if ( !get_mob_index(count2) )
				{
					while ( !get_mob_index(count2)
							 && (count2 != ch->in_room->area->max_vnum + 1) )
					{
						count2 = count2 + 1;
					}

					sprintf(buf, "%5d - %-5d ", count, count2 - 1);
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );
					vnum = count2;
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, "\n\r" );
				}
				else
				{
					sprintf(buf, "   %5d      ", vnum);
					//add_buf(buffer, buf);
					buffer_strcat ( buffer, buf );
					col++;
					if (col % 6 == 0)
						//add_buf(buffer, "\n\r");
						buffer_strcat ( buffer, "\n\r" );
				}
			}
		}
		if (col % 6 != 0)
			//add_buf(buffer, "\n\r");
			buffer_strcat ( buffer, "\n\r" );
		col = 0;

		//page_to_char(buf_string(buffer), ch);
		page_to_char ( buffer_string ( buffer ), ch );
		//free_buf(buffer);
		buffer_free ( buffer );

		return;
	}
}

CH_CMD ( do_uptime )
{
    char buf[MAX_STRING_LENGTH];

    sprintf ( buf, "Shattered Dreams booted at %s\n\r", str_boot_time );
    send_to_char ( buf, ch );

}

CH_CMD ( do_repeat )
{
    char arg1[MSL];
//    char buf[MSL];
    int loop;

    if ( !IS_IMMORTAL ( ch ) || IS_NPC ( ch ) )
        return;

    loop = 0;
    argument = one_argument ( argument, arg1 );
        
    loop = ( atoi ( arg1 ) );

    if ( loop < 1 || loop > 50 )
    {
        send_to_char
            ( "I think that would be a bad idea. You are allowed to do repeat 1 - 50.\n\r",
              ch );
        return;
    }

    if ( !str_cmp ( argument, "repeat" ) )
    {
    	send_to_char ( "Fool! Are you trying to crash the mud?\n\r", ch );
/*	sprintf ( buf, "$N the moron just tried to do a repeat within a repeat, which would crash the mud!", ch );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 ); */
	return;
    }

    for ( ; loop > 0; loop-- )
    {
        interpret ( ch, argument );
    }
}

CH_CMD ( do_passwd )
{
    char arg1[MIL];
    char arg2[MIL];
    char buf[MSL];
    CHAR_DATA *victim;
    char *pwdnew;
	char *p;

    if ( IS_NPC(ch) )
    return;

    argument=one_argument( argument, arg1 );
    argument=one_argument( argument, arg2 );

    victim = get_char_world(ch, arg1);

    if (  ( ch->pcdata->pwd != '\0' )
    && ( arg1[0] == '\0' || arg2[0] == '\0')  )
    {
        send_to_char( "Syntax: password <char> <new>.\n\r", ch );
        return;
    }
    if( victim == '\0' )
    {
                send_to_char( "That person isn't here, they have to be here to reset pwd's.\n\r", ch);
                return;
    }
    if ( IS_NPC( victim ) )
    {
                send_to_char( "You cannot change the password of NPCs!\n\r",ch);
                return;
    }

    if (( victim->level > LEVEL_IMMORTAL ) || ( get_trust(victim) > LEVEL_IMMORTAL ))

        {
                send_to_char( "You can't change the password of that player.\n\r",ch);
                return;
    }

    if ( strlen(arg2) < 5 )
    {
        send_to_char( "New password must be at least five characters long.\n\r", ch );
        return;
    }

    pwdnew = md5crypt( arg2 );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    pwdnew = md5crypt( arg2 );
    free_string( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "Your password has been changed to %s.", arg2 );
    send_to_char( buf, victim);
    return;
}

CH_CMD ( do_iwelcome )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Iwelcome whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }
/*
    if ( victim == ch )
    {
	send_to_char ( "You welcome yourself to the real life. Nice!.... Moron.\n\r", ch );
	return;
    }
*/
    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    send_to_char ( "Okay, sending them a welcoming message...\n\r", ch );
    send_to_char ( "{C========================================================================================{x\n\r", victim );
  if ( is_name ( ch->name, "Rhaelar" ) )
    printf_to_char ( victim, "{cHello {R%s{c, I am {R%s{c, the creator and owner of Shattered Dreams.\n\r", victim->name, ch->name ); 
  else if ( is_name ( ch->name, "Sina" ) )
    printf_to_char ( victim, "{cHello {R%s{c, I am {R%s{c, the co-owner of Shattered Dreams.\n\r", victim->name, ch->name ); 
  else if ( is_name ( ch->name, "Ashlana" ) )
    printf_to_char ( victim, "{cHello {R%s{c, I am {R%s{c, the head questor of Shattered Dreams.\n\r", victim->name, ch->name ); 
  else
    printf_to_char ( victim, "{cHello {R%s{c, I am {R%s{c, an immortal of Shattered Dreams.\n\r", victim->name, ch->name ); 
    send_to_char ( "If you need any help, just ask me or any other immortal here at SD,\n\rand I/we will help you as much as we can.\n\r\n\r", victim );
    send_to_char ( "A few commands you should know about before you venture out in the realm of dreams are:\n\r", victim );
    send_to_char ( "\n\r{b* {BMap       {r- {DTo find speedwalks for areas. - See 'help map'", victim );
    send_to_char ( "\n\r{b* {BJog       {r- {DTo walk the speedwalks you find with 'map'. - See 'help jog'", victim );
    send_to_char ( "\n\r{b* {BConsider  {r- {DChecks a mobs/players stats before you fight them. - See 'help consider'", victim );
    send_to_char ( "\n\r{b* {BBackup    {r- {DShould be used regularly to prevent pfile problems. - See 'help backup'\n\r\n\r", victim );
    send_to_char ( "{cOnce again, I hope you will find Shattered Dreams enjoyable.\n\rIf you have any suggestions for improvement,", victim );
    send_to_char ( " please do not hesitate writing an\n\ridea (see 'help idea'), or posting an idea in the ideas forum on our website, sdmud.com.\n\r\n\r", victim );
    send_to_char ( "We do everything we can to make your stay here as enjoying as possible.\n\r", victim );
    send_to_char ( "{C========================================================================================{x\n\r", victim );
    return;
}

//void do_stance_set( CHAR_DATA *ch, char *argument )
CH_CMD ( do_stance_set )
{

    char arg1 [MIL];

    char arg2 [MIL];

    char arg3 [MIL];

    CHAR_DATA *victim;

    int i;

    int value;

    argument = one_argument( argument, arg1 );

    argument = one_argument( argument, arg2 );

    argument = one_argument( argument, arg3 );



    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )

    {

	send_to_char( "Syntax:\n\r",ch);

	send_to_char( "  set stance <name> <stance> <value>\n\r", ch);

	send_to_char( "  set stance <name> all <value>\n\r",ch);

	send_to_char("   (use the name of the stance, not the number)\n\r",ch);

	return;

    }



    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )

    {

	send_to_char( "They aren't here.\n\r", ch );

	return;

    }



    if ( !is_number( arg3 ) )

    {

	send_to_char( "Value must be numeric.\n\r", ch );

	return;

    }



	value = is_number( arg3 ) ? atoi( arg3 ) : -1;



    if ( value < 0 || value > 100 )

    {

	send_to_char( "Value range is 0 to 100.\n\r", ch );

	return;

    }



    if ( !str_cmp(arg2, "viper") ) { victim->stance[1] = value; return; }

    else if ( !str_cmp(arg2, "crane") ) { victim->stance[2] = value; return; }

    else if ( !str_cmp(arg2, "crab") ) { victim->stance[3] = value; return; }

    else if ( !str_cmp(arg2, "mongoose") ) { victim->stance[4] = value; return; }

    else if ( !str_cmp(arg2, "bull") ) { victim->stance[5] = value; return; }

    else if ( !str_cmp(arg2, "mantis") ) { victim->stance[6] = value; return; }

    else if ( !str_cmp(arg2, "dragon") ) { victim->stance[7] = value; return; }

    else if ( !str_cmp(arg2, "tiger") ) { victim->stance[8] = value; return; }

    else if ( !str_cmp(arg2, "monkey") ) { victim->stance[9] = value; return; }

    else if ( !str_cmp(arg2, "swallow") ) { victim->stance[10] = value; return; }

    else if ( !str_cmp(arg2, "all") )

{

for (i = 1; i < 11 ; i ++)

victim->stance[i] = value;

return;

}
}

