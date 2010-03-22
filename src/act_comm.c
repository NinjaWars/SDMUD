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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"
#include "db.h"
#include "olc.h"

CH_CMD ( do_clear )
{
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
    send_to_char ( "\n\r\n\r\n\r\n\r\n\r\n\r", ch );
}

/* RT code to delete yourself */

CH_CMD ( do_delet )
{
    send_to_char ( "You must type the full command to delete yourself.\n\r",
                   ch );
}

CH_CMD ( do_delete )
{
    char strsave[MAX_INPUT_LENGTH];

    if ( IS_NPC ( ch ) )
        return;

    if ( IS_IMMORTAL ( ch ) )
    {
	send_to_char ( "You are immortal! You cannot delete!\n\rIf you wish to be relieved from duty, talk to Rhaelar.\n\r", ch );
	return;
    }

    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
	send_to_char ( "You cannot delete while you are still {Rviolent{x!\n\r", ch );
	return;
    }

    if ( ch->pcdata->confirm_delete )
    {
        if ( argument[0] != '\0' )
        {
            send_to_char ( "Delete status removed.\n\r", ch );
            ch->pcdata->confirm_delete = FALSE;
            return;
        }
        else
        {
/*            if ( is_clead ( ch ) )
                update_clanlist ( ch, ch->clead, FALSE, TRUE );
            if ( is_clan ( ch ) )
                update_clanlist ( ch, ch->clan, FALSE, FALSE ); */
/*	    backup_char_obj ( ch ); */
	    do_remove ( ch, "all" );
	    do_drop ( ch, "all" );
	    do_sacrifice ( ch, "all.newbie" );
	    do_announce ( ch, "{Dhas {Rdeleted{D.{x\n\r" ); 
	    send_to_char ( "{DTired of this thing called {RL{ri{Rf{re{D?{x\n\r", ch );
	    send_to_char ( "{DYou bow down for the last time to pray to {MS{mina{D,{x\n\r", ch );
	    send_to_char ( "{DUnsheath your {Cd{cagge{Cr{D, and {Yt{yh{Yr{yu{Ys{yt{D it into your {Rh{rear{Rt{D.{x\n\r", ch );
	    send_to_char ( "{DYou begin to hear the voice of {rR{Dhaela{rr{D calling...{x\n\r", ch );
	    send_to_char ( "{D'{rDon't let the door hit you on the ass, {YLOSER{D!'{x\n\r\n\r", ch );
	    send_to_char ( "\n\r\n\r", ch );
	    send_to_char ( "{wIf you deleted for a specific reason, e.g. a player harrassing you\n\r", ch );
	    send_to_char ( "{wor something along those lines, please file a complaint to rhaelar@sdmud.com,\n\r", ch);
	    send_to_char ( "{wdescribing what made you delete, and what we can do to avoid other players\n\r", ch);
	    send_to_char ( "{wdeleting for the same reason in the future.\n\r\n\r", ch);
            sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( ch->name ) );
            wiznet ( "$N turns $Mself into line noise.", ch, NULL, 0, 0, 0 );
            stop_fighting ( ch, TRUE );
            if ( ch->level > HERO )
            {
                update_anclist ( ch, 1 );
            }
            force_quit ( ch, "" );
            unlink ( strsave );
            return;
        }
    }

    if ( argument[0] != '\0' )
    {
        send_to_char ( "Just type delete. No argument.\n\r", ch );
        return;
    }

    send_to_char ( "Type delete again to confirm this command.\n\r", ch );
    send_to_char ( "WARNING: this command is irreversible.\n\r", ch );
    send_to_char
        ( "Typing delete with an argument will undo delete status.\n\r", ch );
    ch->pcdata->confirm_delete = TRUE;
    wiznet ( "$N is contemplating deletion.", ch, NULL, 0, 0,
             get_trust ( ch ) );
}

CH_CMD ( do_rerol )
{
    send_to_char ( "You must type the full command to reroll yourself.\n\r",
                   ch );
}

/*CH_CMD ( do_questforge )
{
    char arg1[MAX_INPUT_LENGTH];    
    char arg2[MAX_INPUT_LENGTH];    
    char flag[3][MAX_INPUT_LENGTH]; 
    int wc = 0, dt = 0, flags = 0, i;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char numstr[31];
    AFFECT_DATA af;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, flag[0] );
    argument = one_argument ( argument, flag[1] );
    argument = one_argument ( argument, flag[2] );

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "You are an NPC. Go away.\n\r", ch );
        return;
    }

    if ( ( ch->in_room == NULL ) || ( ch->in_room->vnum != 3398 ) )
    {
        send_to_char ( "Does this look like the journeymen guild?\n\r", ch );
        return;
    }
    if ( arg1[0] == '\0'         )
    {
        send_to_char
            ( "{cSyntax{D:{x qforge <weaponclass> <damagetype> <flag1> <flag2> <flag3>\n\r",
              ch );
        send_to_char
            ( "        qforge wclass {D- {clist available weapon classes{x\n\r",
              ch );
        send_to_char
            ( "	qforge dtype  {D- {clist available damage types{x\n\r", ch );
        send_to_char
            ( "	qforge flags  {D- {clist available weapon flags{x\n\r", ch );
        return;
    }
    if ( !str_cmp ( arg1, "wclass" ) )
    {
        send_to_char ( "{cValid weapon classes are{D:{x\n\r", ch );
        do_help ( ch, "wclass" );
        return;
    }
    else if ( !str_cmp ( arg1, "dtype" ) )
    {
        send_to_char ( "{cValid damage types are{D:{x\n\r", ch );
        do_help ( ch, "wdam" );;
        return;
    }
    else if ( !str_cmp ( arg1, "flags" ) )
    {
        send_to_char ( "{cValid weapon flags are{D:{x\n\r", ch );
        do_help ( ch, "wtype" );
        return;
    }
    else if ( arg2[0] == '\0' || flag[0][0] == '\0' || flag[1][0] == '\0' ||
              flag[2][0] == '\0' )
    {
        do_questforge ( ch, "" );
        return;
    }
    if ( ch->qps - 200 < 0 )
    {
        send_to_char
            ( "Forging a weapon costs 200 immquest points. You don't have enough.\n\r",
              ch );
        return;
    }
    if ( ( weapon_lookup ( arg1 ) == -1 ) && str_cmp ( arg1, "exotic" ) )
    {
        send_to_char
            ( "This weapon class is not available. Valid classes are:\n\r",
              ch );
        do_help ( ch, "wclass" );
        return;
    }
    wc = weapon_type ( arg1 );
    if ( ( ( dt = attack_lookup ( arg2 ) ) == 0 ) ||
         ( ( dt >= 40 ) && ( dt <= 60 ) ) )
    {
        send_to_char
            ( "This damage type is not available. Valid types are:\n\r", ch );
        do_help ( ch, "wdam" );
        return;
    }
    for ( i = 0; i < 3; i++ )
    {
        switch ( i )
        {
            case 0:
                sprintf ( numstr, "first" );
                break;
            case 1:
                sprintf ( numstr, "second" );
                break;
            case 2:
                sprintf ( numstr, "third" );
                break;
        }
        if ( flag_value ( weapon_type2, flag[i] ) == NO_FLAG )
        {
            sprintf ( buf,
                      "The %s flag you have specified is not available. Valid flags are:\n\r",
                      numstr );
            send_to_char ( buf, ch );
    af.bitvector = 0;

    af.location = APPLY_HITROLL;
    af.modifier = ch->level / 5;
    affect_to_obj ( obj, &af );

    af.location = APPLY_DAMROLL;
    af.modifier = ch->level / 5;
    affect_to_obj ( obj, &af );
    af.location = APPLY_HIT;
    af.modifier = ch->level;
    affect_to_obj ( obj, &af );
    af.location = APPLY_MANA;
    af.modifier = ch->level;
    affect_to_obj ( obj, &af );
    af.location = APPLY_MOVE;
    af.modifier = ch->level;
    affect_to_obj ( obj, &af );
    af.location = APPLY_SAVES;
    af.modifier = ch->level / 8;
    affect_to_obj ( obj, &af );

    obj_to_char ( obj, ch );
    act ( "$n forges $p.", ch, obj, NULL, TO_ROOM );
    act ( "You forge $p.", ch, obj, NULL, TO_CHAR );
    send_to_char ( "200 immquest points have been deducted.\n\r", ch );
    //printf_log("%s forges a %s (%s/%s) at %d.",
    //ch->name,weapon_class[wc].name,attack_table[dt].name,
    //flag_string(weapon_type2,flags),ch->in_room->vnum);   
    return;
}
*/
CH_CMD ( do_reform )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int amount;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "\n\rSyntax:\n\r", ch );
        send_to_char ( "reform <type> <amount>\n\r", ch );
        send_to_char ( "\n\rtypes - AQP IQP\n\r", ch );
        return;
    }

    amount = is_number ( arg2 ) ? atol ( arg2 ) : -1;

    if ( !str_prefix ( arg1, "AQP" ) )
    {
        if ( IS_NPC ( ch ) )
        {
            printf_to_char ( ch, "Mobs have no need to do that" );
            return;
        }

        if ( amount > ch->pcdata->questpoints )
        {
            printf_to_char ( ch,
                             "\n\rHow can you reform %d AQP when you only have %ld?",
                             amount, ch->pcdata->questpoints );
            return;
        }
        if ( amount < 0 )
        {
            printf_to_char ( ch, "\n\rOnly positive amounts allowed...\n\r" );
            return;
        }
        if ( amount % 20 == 0 )
        {
            ch->pcdata->questpoints -= amount;
            ch->qps += amount / 20;
            printf_to_char ( ch,
                             "\n\rYou reform %d AQP. You gained %d, and now have a total of %d IQP.\n\r",
                             amount, (amount / 20), ch->qps );
            return;
        }
        else
        {
            printf_to_char ( ch,
                             "\n\rAmount {WMUST{x be divisible by 20\n\r" );
            return;
        }
    }

    if ( !str_prefix ( arg1, "IQP" ) )
    {
        if ( IS_NPC ( ch ) )
        {
            printf_to_char ( ch, "Mobs have no need to do that" );
            return;
        }

        if ( amount < 0 || amount > 200 )
        {
            send_to_char
                ( "\n\rYou can only reform up to 200 iqp at a time.\n\r",
                  ch );
            return;
        }
        if ( amount > ch->qps )
        {
            printf_to_char ( ch,
                             "\n\rHow can you reform %d IQP when you only have %ld?\n\r",
                             amount, ch->pcdata->questpoints );
            return;
        }
        if ( amount < 0 )
        {
            printf_to_char ( ch, "\n\rOnly positive amounts allowed...\n\r" );
            return;
        }
        ch->qps -= amount;
        ch->pcdata->questpoints += amount * 20;
        printf_to_char ( ch,
                         "\n\rYou reform %d IQP.  Your now have %ld AQP.\n\r",
                         amount, ch->pcdata->questpoints );
        return;
    }

    do_reform ( ch, "" );

    return;
}

struct tier_type
{
    int num;
    long bit;
};

const struct tier_type tier_table[] = {
    {1, TIER_01},
    {2, TIER_02},
    {3, TIER_03},
    {4, TIER_04},
    {0, 0}
};

CH_CMD ( do_reroll )
{
    if ( reboot_countdown > 0 && reboot_countdown < 11 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a reboot.\n\rPlease try again later.\n\r",
              ch );
        return;
    }

    if ( ch->pcdata->rerolltimer > 0 )
    {
    	printf_to_char ( ch, "You cannot reroll so soon after your last reroll. There are %d minutes left until you can reroll.\n\r", ch->pcdata->rerolltimer );
	return;
    }
    DESCRIPTOR_DATA *d;
    char /*strsave[MAX_INPUT_LENGTH], */buf[MAX_STRING_LENGTH];
    char player_psswd[MAX_STRING_LENGTH], player_title[MAX_STRING_LENGTH];
    char player_name[MAX_INPUT_LENGTH], player_prmpt[MAX_INPUT_LENGTH];
    char player_cname[MAX_INPUT_LENGTH], player_spouse[MAX_INPUT_LENGTH];
    char player_namereroll[MAX_INPUT_LENGTH];
    //char *player_whostring;

    int player_plat, player_gold, player_slvr, player_tier, player_comm, player_played;
    int player_iqp, player_aqp, player_clan, player_clan_rank, player_clead, player_balance; 
    int	player_pkkills, player_pkdeaths, player_awins, player_alosses, iClass, iSecv, wNet;
    int	ctier, player_trust, player_twins, player_tkills, player_tdeaths, player_times_rerolled;
    int player_mkill, player_mdeath, player_shares, player_exiletime, player_noclantime;
    long player_stances[11];
    bool is_helper = FALSE;
    int i;
/*
player_last_note;
    int player_last_idea, player_last_penalty, player_last_news, player_last_changes;
    int player_last_weddings, player_last_jokes, player_times_rerolled; */
    bool wasHero = FALSE;

    if ( IS_NPC ( ch ) || ( d = ch->desc ) == NULL )
        return;

    if ( IS_IMMORTAL ( ch ) )
    {
	send_to_char ( "You are immortal! You cannot reroll!\n\rIf you wish to be relieved from duty, talk to Rhaelar.\n\r", ch );
	return;
    }

    if ( ch->level < LEVEL_HERO && !IS_SET ( ch->act, PLR_REROLL ) )
    {
        sprintf ( buf,
                  "You must be level %d or already have rerolled to reroll.\n\r",
                  LEVEL_HERO );
        send_to_char ( buf, ch );
        return;
    }

    if ( ch->level >= LEVEL_HERO )
        wasHero = TRUE;
    iClass = ch->class;

    if ( ch->pcdata->confirm_reroll )
    {
        if ( argument[0] == '\0' )
        {
            send_to_char ( "Reroll status removed.\n\r", ch );
            ch->pcdata->confirm_reroll = FALSE;
            return;
        }
        else
        {
            if ( !is_number ( argument ) )
            {
                send_to_char ( "Reroll status removed.\n\r", ch );
                ch->pcdata->confirm_reroll = FALSE;
                return;
            }
            /* Make sure ctier is not greater or less than number tiers in tier_table */
            else if ( ( ctier = atoi ( argument ) ) <= 0 || ctier > 4 )
            {
                send_to_char
                    ( "That is not a valid tier.  Please type REROLL <tier>.\n\r",
                      ch );
                return;
            }

            if ( !IS_SET ( ch->pcdata->tier, tier_table[ctier - 1].bit ) )
            {
                send_to_char
                    ( "That tier is not available to you.\n\rReroll status removed.\n\r",
                      ch );
                ch->pcdata->confirm_reroll = FALSE;
                return;
            }
/*	    do_backup ( ch, NULL ); */
	    rerolls_today++;
	    backup_char_obj ( ch );
	    save_char_obj ( ch );

/*            if ( is_clead ( ch ) )
                update_clanlist ( ch, ch->clead, FALSE, TRUE );
            if ( is_clan ( ch ) )
                update_clanlist ( ch, ch->clan, FALSE, FALSE ); */
            if ( ch->level > HERO )
                update_anclist ( ch, 1 );

            /*
             * Send a nice informational message.
             */
            //sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( ch->name ) );
            stop_fighting ( ch, TRUE );
            send_to_char
                ( "{RYou have chosen to reroll.  You will now be dropped in at the\n\r",
                  ch );
            send_to_char
                ( "race selection section of character creation, and will be allowed\n\r",
                  ch );
            send_to_char
                ( "to choose from a wider selection of races and classes.\n\r\n\r",
                  ch );
            send_to_char ( "\n\r{R[{WHit Enter to Continue{R]{x\n\r", ch );

            sprintf ( player_name, "%s", capitalize ( ch->name ) );
            sprintf ( player_namereroll, "%scock", capitalize ( ch->name ) );
            sprintf ( player_cname, "%s", ch->pcdata->cname );
            sprintf ( player_spouse, "%s", ch->pcdata->spouse );
            sprintf ( player_psswd, "%s", ch->pcdata->pwd );
            sprintf ( player_title, "%s", ch->pcdata->title );
            sprintf ( player_prmpt, "%s", ch->prompt );
/*        if ( ch->pcdata->who_descr[0] != '\0' )	    
            sprintf ( player_whostring, "%s", ch->pcdata->who_descr ); */
            player_iqp = ch->qps;
            player_aqp = ch->pcdata->questpoints;
	    player_clan = ch->clan;
	    player_clan_rank = ch->clan_rank;
	    player_clead = ch->clead;
	    player_balance = ch->pcdata->balance;
            wNet = ch->wiznet;
            iSecv = ch->pcdata->security;
            player_tier = ch->pcdata->tier;
            player_comm = ch->comm;
            player_trust = get_trust ( ch );
            player_plat = ch->platinum != 0 ? 1 : 0;
            player_gold = ch->gold <= 25 ? ch->gold : 25;
            player_slvr = ch->silver != 0 ? ch->silver : 0;
	    player_played = ( ch->played + ( int ) ( current_time - ch->logon ) );
	    player_pkkills = ch->pcdata->pkkills;
	    player_pkdeaths = ch->pcdata->pkdeaths;
	    player_awins = ch->pcdata->awins;
	    player_alosses = ch->pcdata->alosses;
	    player_twins = ch->pcdata->twins;
	    player_tkills = ch->pcdata->tkills;
	    player_tdeaths = ch->pcdata->tdeaths;
/*
	    player_last_note = ch->pcdata->last_note;
	    player_last_idea = ch->pcdata->last_idea;
	    player_last_penalty = ch->pcdata->last_penalty;
	    player_last_news = ch->pcdata->last_news;
	    player_last_changes = ch->pcdata->last_changes;
	    player_last_weddings = ch->pcdata->last_weddings;
	    player_last_jokes = ch->pcdata->last_jokes;
*/
	    player_times_rerolled = ch->pcdata->times_rerolled;
	    player_mkill = ch->pcdata->mkill;
	    player_mdeath = ch->pcdata->mdeath;
	    player_shares = ch->pcdata->shares;
            player_exiletime = ch->pcdata->exiletime;
            player_noclantime = ch->pcdata->noclantime;
	 
	    	    	    
	    if ( IS_SET ( ch->act2, PLR2_HELPER ) )
	    is_helper = TRUE;
/*	    
		for (i=0; i < 11; i++)
			ch->stance[i] = player_stances[i];
*/
		for (i=0; i < 11; i++)
			player_stances[i] = ch->stance[i];

	    do_announce ( ch, "{Dhas just begun {Grerolling{D.{x" );
	    
            /*
               * "After extract_char the ch is no longer valid!"
             */

    /* Free note that might be there somehow */
    if (ch->pcdata->in_progress)
	free_note (ch->pcdata->in_progress);
	
            extract_char ( ch, TRUE );
            //unlink ( strsave );

            /*
               * Delete the pfile, but don't boot the character.
               * Instead, do a load_char_obj to get a new ch,
               * saving the password, and some variables.  Then,
               * set the PLR_REROLL bit and drop the player in at
               * CON_BEGIN_REROLL.
	       *
	       * This fine lil' piece of code no longer deletes
	       * the pfile. It was never necessary. Rhaelar 01'2005
             */
            
	    load_char_obj ( d, player_namereroll );
	    
	    /*
	     * Lets reset some critical things - Rhaelar 01'2005
	     * Don't need this anymore cause load_char_obj loads a
	     * completely clean new pfile. No memory remains.
	     */
	    /*d->character->level = 0;
	    d->character->pet = NULL;
	    d->character->affected = NULL;
	    d->character->carrying = NULL;
	    d->character->carry_weight = 0;
	    d->character->carry_number = 0;
	    d->character->gen_data = NULL;
	    bzero ( d->character->pcdata->learned, MAX_SKILL );
	    bzero ( d->character->pcdata->group_known, MAX_GROUP );
            d->character->pcdata->points = 0; */
	    	        
	    /* Reset over - Now for transitioning... */
            d->character->gold = player_gold;
            d->character->silver = player_slvr;
            d->character->platinum = player_plat;
            d->character->prompt = str_dup ( player_prmpt );
            d->character->pcdata->pwd = str_dup ( player_psswd );
            d->character->pcdata->title = str_dup ( player_title );
            d->character->pcdata->socket = str_dup ( d->host );
            d->character->name = str_dup ( player_name );            
	    d->character->pcdata->cname = str_dup ( player_cname );
            d->character->pcdata->spouse = str_dup ( player_spouse );
//            d->character->pcdata->who_descr = str_dup ( player_whostring );
            d->character->pcdata->tier = player_tier;
            d->character->comm = player_comm;
            d->character->trust = player_trust;
            d->character->pcdata->security = iSecv;
            d->character->wiznet = wNet;
            d->character->qps = player_iqp;
            d->character->pcdata->questpoints = player_aqp;
	    d->character->clan = player_clan;
	    d->character->clan_rank = player_clan_rank;
	    d->character->clead = player_clead;
	    d->character->pcdata->balance = player_balance;
	    d->character->pcdata->pkkills = player_pkkills;
	    d->character->pcdata->pkdeaths = player_pkdeaths;
	    d->character->pcdata->awins = player_awins;
	    d->character->pcdata->alosses = player_alosses;
	    d->character->pcdata->twins = player_twins;
	    d->character->pcdata->tkills = player_tkills;
	    d->character->pcdata->tdeaths = player_tdeaths;
	    d->character->played = player_played;
/*
	    d->character->pcdata->last_note = player_last_note;
	    d->character->pcdata->last_idea = player_last_idea;
	    d->character->pcdata->last_penalty = player_last_penalty;
	    d->character->pcdata->last_news = player_last_news;
	    d->character->pcdata->last_changes = player_last_changes;
	    d->character->pcdata->last_weddings = player_last_weddings;
	    d->character->pcdata->last_jokes = player_last_jokes;
*/
	    d->character->pcdata->times_rerolled = player_times_rerolled + 1;
	    d->character->pcdata->mkill = player_mkill;
	    d->character->pcdata->mdeath = player_mdeath;
	    d->character->pcdata->shares = player_shares;
	    d->character->pcdata->rerolltimer = 10;
            d->character->pcdata->exiletime = player_exiletime;
            d->character->pcdata->noclantime = player_noclantime;
	    if ( is_helper )
	    SET_BIT ( ch->act2, PLR2_HELPER );

//            wiznet ( "$N has rerolled.", d->character, NULL, 0, 0, 0 );
		for (i=0; i < 11; i++)
			ch->stance[i] = player_stances[i];


            SET_BIT ( d->character->pcdata->ctier,
                      tier_table[ctier - 1].bit );
            SET_BIT ( d->character->act, PLR_REROLL );
            ch->pcdata->confirm_reroll = FALSE;
	    
            d->connected = CON_BEGIN_REROLL;
            return;
        }
    }

    if ( argument[0] != '\0' )
    {
        send_to_char
            ( "To set reroll status type REROLL with no arguments.\n\r", ch );
        send_to_char ( "Proper syntax will be show at that time.\n\r", ch );
        return;
    }

    /* NOTE: these next two statements needed as a catch-all *
       * to prevent the need of a pwipe,     -- Seronis         *
       ******************************************************* */
    SET_BIT ( d->character->pcdata->tier, TIER_01 );
    SET_BIT ( d->character->pcdata->tier, class_table[iClass].tier );
    if ( wasHero )
        SET_BIT ( d->character->pcdata->tier, class_table[iClass].tier_next );

    send_to_char ( "Reroll status confirmed.  This command is NOT \n\r", ch );
    send_to_char ( "reversable.  Type REROLL with no arguments to \n\r", ch );
    send_to_char ( "remove reroll status. \n\r\n\r", ch );
    send_to_char ( "Syntax: REROLL <tier> \n\r", ch );
    send_to_char ( "You are allowed to select from the following\n\r", ch );
    send_to_char ( "Tiers: ", ch );

    if ( IS_SET ( ch->pcdata->tier, TIER_01 ) )
        send_to_char ( "1 ", ch );
    if ( IS_SET ( ch->pcdata->tier, TIER_02 ) )
        send_to_char ( "2 ", ch );
    if ( IS_SET ( ch->pcdata->tier, TIER_03 ) )
        send_to_char ( "3 ", ch );
    if ( IS_SET ( ch->pcdata->tier, TIER_04 ) )
        send_to_char ( "4 ", ch );

    send_to_char ( "\n\r", ch );

    ch->pcdata->confirm_reroll = TRUE;
    wiznet ( "$N is contemplating rerolling.", ch, NULL, 0, 0,
             get_trust ( ch ) );
}

CH_CMD ( do_ancient )
{

    if ( ch->level < LEVEL_HERO )
    {
        printf_to_char ( ch,
                         "{xYou must be level %d to become an Ancient.\n\r",
                         LEVEL_HERO );
        return;
    }

    if ( ch->level == 202 )
    {
        send_to_char ( "You are already an Ancient.\n\r", ch );
        return;
    }

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "Mobs can't become Ancients!\n\r", ch );
        return;
    }

    if ( IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Immortals have no need to become Ancients!\n\r", ch );
        return;
    }

/*    if ( !IS_SET ( class_table[ch->class].tier, TIER_04 ) )
    {

        send_to_char ( "{xYou must be tier 4 to become an Ancient.\n\r", ch );
        return;
    } */

    if ( ch->pcdata->questpoints < 10000 )
    {

        send_to_char
            ( "You must have 10,000 autoquest points to become an Ancient.\n\r",
              ch );
        return;
    }

    if ( ch->qps < 5 )
    {
        send_to_char
            ( "You must have 5 immquest points to become an Ancient.\n\r",
              ch );
        return;
    }

    if ( ch->platinum < 1000 )
    {
        send_to_char
            ( "You must be carrying 1000 platinum to become an Ancient.\n\r",
              ch );
        return;
    }

    send_to_char
        ( "{RCongratulations! You have become an {YA{yn{Yc{yi{Ye{yn{Yt{R!!!{x\n\r",
          ch );

    do_announce ( ch, "has just become an {YA{yn{Yc{yi{Ye{yn{Yt{x!!!\n\r" );
    ch->level += 1;
    ch->pcdata->questpoints -= 10000;
    ch->qps -= 5;
    ch->platinum -= 1000;
    ch->max_hit += 10000;
    ch->max_mana += 10000;
    ch->max_move += 10000;
    ch->pcdata->perm_hit += 10000;
    ch->pcdata->perm_mana += 10000;
    ch->pcdata->perm_move += 10000;
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;
/*    do_backup ( ch, NULL ); */
    update_anclist ( ch, ch->level );
    backup_char_obj ( ch );
    save_char_obj ( ch );
    return;
}

/* show suppression settings - Gregor Stipicic aka Fade, 2001 */
CH_CMD ( do_verbose )
{
#define VER_OO(ch,flag) (!IS_SET((ch)->pcdata->verbose,(flag))?" {y({YON{y){x":"{b({BOFF{b){x")
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg );

    if ( IS_NPC ( ch ) )
        return;

    if ( arg[0] == '\0' )
    {
        send_to_char ( "{CV{cerbose{x settings{D:\n\r", ch );
        printf_to_char ( ch,
                         "{CW{ceapon flags{D  :   %s\n\r{CD{codge/etc.  {D  :   %s{x\n\r",
                         VER_OO ( ch, VERBOSE_FLAGS ), VER_OO ( ch,
                                                                VERBOSE_DODGE ) );
        printf_to_char ( ch, "{CO{cbject effects{D:   %s{x\n\r",
                         VER_OO ( ch, VERBOSE_BURN ) );
        printf_to_char ( ch,
                         "{cShields are %scompressed into one line.{x\n\r",
                         IS_SET ( ch->pcdata->verbose,
                                  VERBOSE_SHIELD ) ? "" : "not " );
        return;
    }
    if ( !str_prefix ( arg, "weapon" ) )
    {
        if ( IS_SET ( ch->pcdata->verbose, VERBOSE_FLAGS ) )
        {
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_FLAGS );
            send_to_char ( "{cWeapon flags are no longer suppressed.{x\n\r",
                           ch );
        }
        else
        {
            SET_BIT ( ch->pcdata->verbose, VERBOSE_FLAGS );
            send_to_char ( "{cWeapon flags are suppressed now.{x\n\r", ch );
        }
        return;
    }
    if ( !str_prefix ( arg, "dodge" ) )
    {
        if ( IS_SET ( ch->pcdata->verbose, VERBOSE_DODGE ) )
        {
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_DODGE );
            send_to_char
                ( "{cDodges, parries, and shield blocks are no longer suppressed.{x\n\r",
                  ch );
        }
        else
        {
            SET_BIT ( ch->pcdata->verbose, VERBOSE_DODGE );
            send_to_char
                ( "{cDodges, parries, and shield blocks are suppressed now.{x\n\r",
                  ch );
        }
        return;
    }
    if ( !str_prefix ( arg, "object" ) )
    {
        if ( IS_SET ( ch->pcdata->verbose, VERBOSE_BURN ) )
        {
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_BURN );
            send_to_char
                ( "{cObject effects (burning potions, etc.) are no longer suppressed.{x\n\r",
                  ch );
        }
        else
        {
            SET_BIT ( ch->pcdata->verbose, VERBOSE_BURN );
            send_to_char
                ( "{cObject effects (burning potions, etc.) are suppressed now.{x\n\r",
                  ch );
        }
        return;
    }
    if ( !str_prefix ( arg, "shields" ) )
    {
        if ( IS_SET ( ch->pcdata->verbose, VERBOSE_SHIELD ) )
        {
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_SHIELD );
            SET_BIT ( ch->pcdata->verbose, VERBOSE_SHIELD_COMP );
            send_to_char
                ( "{cShields are no longer compressed into one line.{x\n\r",
                  ch );
        }
        else
        {
            SET_BIT ( ch->pcdata->verbose, VERBOSE_SHIELD );
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_SHIELD_COMP );
            send_to_char
                ( "{cShields are compressed into one line now.{x\n\r", ch );
        }
        return;
    }
    send_to_char
        ( "{cSyntax{D: {CVerbose {cweapon{D/{cflags{D/{cdodge{D/{cobject{D/{cshields{x\n\r",
          ch );
    return;
}

/* RT code to display channel status */

CH_CMD ( do_channels )
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char ( "\n\r{WChannel{x       {RStatus{x\n\r", ch );
    send_to_char ( "{x-------{D---------{W=========={D-------{x---------\n\r",
                   ch );

    send_to_char ( "{WIC{x            ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOGOSSIP ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WOOC{x           ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOOOC ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WCURSE         {x", ch );
    if ( !IS_SET ( ch->comm, COMM_CURSE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WRace Talk     {x", ch );
    if ( !IS_SET ( ch->comm, COMM_NORACE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WSocial{x        ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOSOCIAL ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );
    
    send_to_char ( "{WAnnounce     {x ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOANNOUNCE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WClan Gossip{x   ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOCGOSSIP ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WQuest Gossip{x  ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOCGOSSIP ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );
/*
    send_to_char("ooc            ",ch);
    if (!IS_SET(ch->comm,COMM_NOOOC))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);
*/
    send_to_char ( "{WMusic{x         ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOMUSIC ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );
    
    send_to_char ( "{WAuction{x       ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOAUCTION ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WQ/A           {x", ch );
    if ( !IS_SET ( ch->comm, COMM_NOASK ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WQuote {x        ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOQUOTE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WGrats{x         ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOGRATS ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );
/*
    send_to_char ( "{WCheers{x        ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOCHEER ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );
*/

    send_to_char ( "{WHints{x         ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOHINT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    if ( IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "{WImmchat{x       ", ch );
        if ( !IS_SET ( ch->comm, COMM_NOWIZ ) )
            send_to_char ( "{YON{x\n\r", ch );
        else
            send_to_char ( "{yOFF{x\n\r", ch );
    }

    send_to_char ( "{WShouts{x        ", ch );
    if ( !IS_SET ( ch->comm, COMM_NOSHOUT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WTells{x         ", ch );
    if ( !IS_SET ( ch->comm, COMM_DEAF ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r", ch );

    send_to_char ( "{WQuiet Mode{x    ", ch );
    if ( IS_SET ( ch->comm, COMM_QUIET ) )
        send_to_char ( "{YON{x\n\r\n\r", ch );
    else
        send_to_char ( "{yOFF{x\n\r\n\r", ch );

    send_to_char ( "{x-------{D---------{W=========={D-------{x---------\n\r",
                   ch );

    if ( IS_SET ( ch->comm, COMM_AFK ) )
        send_to_char ( "{WYou are {RAFK{W.{x\n\r", ch );

    if ( IS_SET ( ch->comm, COMM_STORE ) )
        send_to_char ( "{WYou store tells during fights.{x\n\r", ch );
/*
    if ( IS_SET ( ch->comm, COMM_SNOOP_PROOF ) )
        send_to_char ( "{WYou are immune to snooping.{x\n\r", ch );
*/
    if ( ch->lines != PAGELEN )
    {
        if ( ch->lines )
        {
            sprintf ( buf, "{WYou display{Y %d{W lines of scroll.{x\n\r",
                      ch->lines + 2 );
            send_to_char ( buf, ch );
        }
        else
            send_to_char ( "{WScroll buffering is off.{x\n\r", ch );
    }

    if ( ch->prompt != NULL )
    {
        sprintf ( buf, "{WYour current prompt is:{x %s{x\n\r", ch->prompt );
        send_to_char ( buf, ch );
    }
    
    if ( IS_SET ( ch->comm, COMM_NOSHOUT ) )
        send_to_char ( "{WYou cannot shout.{x\n\r", ch );

    if ( IS_SET ( ch->comm, COMM_NOTELL ) )
        send_to_char ( "{WYou cannot use tell.{x\n\r", ch );

    if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        send_to_char ( "{WYou cannot use channels.{x\n\r", ch );

    if ( IS_SET ( ch->comm, COMM_NOEMOTE ) )
        send_to_char ( "{WYou cannot show emotions.{x\n\r", ch );

}

/* RT deaf blocks out all shouts */

CH_CMD ( do_deaf )
{

    if ( IS_SET ( ch->comm, COMM_DEAF ) )
    {
        send_to_char ( "You can now hear tells again.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_DEAF );
    }
    else
    {
        send_to_char ( "From now on, you won't hear tells.\n\r", ch );
        SET_BIT ( ch->comm, COMM_DEAF );
    }
}
/*
CH_CMD ( do_cheers )
{

    if ( IS_SET ( ch->comm, COMM_NOCHEER ) )
    {
        send_to_char ( "You will now see announcements when people level again.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_NOCHEER );
    }
    else
    {
        send_to_char ( "From now on, you won't get announcements when people level.\n\r", ch );
        SET_BIT ( ch->comm, COMM_NOCHEER );
    }
}
*/
/* RT quiet blocks out all communication */

CH_CMD ( do_quiet )
{
    if ( IS_SET ( ch->comm, COMM_QUIET ) )
    {
        send_to_char ( "Quiet mode removed.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_QUIET );
    }
    else
    {
        send_to_char ( "From now on, you will only hear says and emotes.\n\r",
                       ch );
        SET_BIT ( ch->comm, COMM_QUIET );
    }
}

/*
void do_afw ( CHAR_DATA *ch, char * argument)  
{
    char buf[MAX_STRING_LENGTH];
     
    if (IS_SET(ch->comm,COMM_AFW))
    {
      if(ch->tells)
      {
         sprintf( buf, "AFW mode removed.  You have {R%d{x tells waiting.\n\r", ch->tells );
         send_to_char( buf, ch );
         send_to_char("Type 'replay' to see tells.\n\r",ch);
      }
      else
      {
         send_to_char("AFW mode removed.  You have no tells waiting.\n\r",ch);
      }
      REMOVE_BIT(ch->comm,COMM_AFW);
    }
   else
   {
     send_to_char("You are now in AFW mode.\n\r",ch);
     SET_BIT(ch->comm,COMM_AFW);
   }
}*/

/* afk command */
CH_CMD ( do_afk )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET ( ch->comm, COMM_AFK ) )
    {
        if ( ch->tells )
        {
            sprintf ( buf,
                      "AFK mode removed.  You have {R%d{x tells waiting.\n\r",
                      ch->tells );
            send_to_char ( buf, ch );
            send_to_char ( "Type 'replay' to see tells.\n\r", ch );
        }
        else
        {
            send_to_char
                ( "AFK mode removed.  You have no tells waiting.\n\r", ch );
        }
        REMOVE_BIT ( ch->comm, COMM_AFK );
    }
    else
    {
        send_to_char ( "You are now in AFK mode.\n\r", ch );
        SET_BIT ( ch->comm, COMM_AFK );
    }
}

CH_CMD ( do_autostore )
{
    if ( IS_SET ( ch->comm, COMM_STORE ) )
    {
        send_to_char ( "You will no longer store tells during fights.\n\r",
                       ch );
        REMOVE_BIT ( ch->comm, COMM_STORE );
    }
    else
    {
        send_to_char ( "You will now store tells during fights.\n\r", ch );
        SET_BIT ( ch->comm, COMM_STORE );
    }
}

CH_CMD ( do_replay )
{
    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "You can't replay.\n\r", ch );
        return;
    }

    if ( buffer_string ( ch->pcdata->buffer )[0] == '\0' )
    {
        send_to_char ( "You have no tells to replay.\n\r", ch );
        return;
    }

    page_to_char ( buffer_string ( ch->pcdata->buffer ), ch );
    //clear_buf ( ch->pcdata->buffer );
    buffer_clear ( ch->pcdata->buffer );
    ch->tells = 0;
}

CH_CMD ( do_racetalk )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( IS_NPC ( ch ) )
        return;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NORACE ) )
        {
            send_to_char ( "Race channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NORACE );
        }
        else
        {
            send_to_char ( "Race channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NORACE );
        }
    }
    else
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channels priviliges.\n\r", ch );
            return;
        }
        
/*	if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"
                          "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	} */

        REMOVE_BIT ( ch->comm, COMM_NORACE );
/*
        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
          argument = makedrunk ( ch, argument );
*/

        sprintf ( buf, "{W$n {c-{C={D%s{C={c-{x '{r$t{x'",
                  pc_race_table[ch->race].name );
        act_new ( buf, ch, argument, NULL, TO_CHAR, POS_DEAD );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && victim != ch &&
                 victim->race == ch->race && !IS_NPC ( victim ) &&
                 !IS_SET ( victim->comm, COMM_NORACE ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                act_new ( buf, ch, argument, d->character, TO_VICT,
                          POS_SLEEPING );
            }
        }
    }
}

CH_CMD ( do_curse )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_CURSE ) )
        {
            send_to_char ( "{DC{RU{DRSE{x channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_CURSE );
        }
        else
        {
            send_to_char ( "{DC{RU{DRSE{x channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_CURSE );
        }
    }
    else                        /* flame message sent, turn gossip on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges{x\n\r", ch );
            return;
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }
        
/*	if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	} */

        REMOVE_BIT ( ch->comm, COMM_CURSE );
/*
        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
          argument = makedrunk ( ch, argument );
*/
        sprintf ( buf, "{cYou {r-{R={DC{cu{DRSE{R={r- {x'{R%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_CURSE ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{x$n {r-{R={DC{cu{DRSE{R={r- {x'{R$t{x'", ch,
                              argument, d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

/* RT ooc rewritten in ROM style */
CH_CMD ( do_ooc )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOOOC ) )
        {
            send_to_char ( "Ooc and Chat channels are now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOOOC );
        }
        else
        {
            send_to_char ( "Ooc and Chat channels are now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOOOC );
        }
    }
    else                        /* ooc message sent, turn ooc on if it is off */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;

            REMOVE_BIT ( ch->comm, COMM_NOOOC );
        }

/*        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	} */
	
        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }
/*
        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
          argument = makedrunk ( ch, argument );
*/
        /* I want to use smash_colour here, but it overflows the buffer :( - Rhaelar 12'2004 */
	sprintf ( buf, "{g-{G={xYou{G={g- {D({yOOC{D){x '{c%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOOOC ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{g-{G={x$n{G={g-{D ({yOOC{D){x '{c$t{x'", ch,
                              argument, d->character, TO_VICT, POS_DEAD );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_chat )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOOOC ) )
        {
            send_to_char ( "Ooc and Chat channels are now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOOOC );
        }
        else
        {
            send_to_char ( "Ooc and Chat channels are now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOOOC );
        }
    }
    else                        /* ooc message sent, turn ooc on if it is off */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;

            REMOVE_BIT ( ch->comm, COMM_NOOOC );
        }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/	
        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        sprintf ( buf, "{g-{G={wYou{G={g- {D({WChat{D){x '{c%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOOOC ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{g-{G={x$n{G={g-{D ({WChat{D){x '{c$t{x'", ch,
                              argument, d->character, TO_VICT, POS_DEAD );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_pray )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOPRAY ) )
        {
            send_to_char ( "Pray channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOPRAY );
        }
        else
        {
            send_to_char ( "Pray channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOPRAY );
        }
    }
    else                        /* pray message sent, turn pray on if it is off */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;

            REMOVE_BIT ( ch->comm, COMM_NOPRAY );
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        sprintf ( buf, "{g-{G={wYou{G={g- {D({MPray{D){x '{W%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
	    IS_IMMORTAL ( d->character ) &&
                 !IS_SET ( victim->comm, COMM_NOPRAY ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{g-{G={x$n{G={g-{D ({MPray{D){x '{W$t{x'", ch,
                              argument, d->character, TO_VICT, POS_DEAD );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

/* RT chat replaced with ROM gossip */
CH_CMD ( do_gossip )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOGOSSIP ) )
        {
            send_to_char ( "IC channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOGOSSIP );
        }
        else
        {
            send_to_char ( "IC channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOGOSSIP );
        }
    }
    else                        /* gossip message sent, turn gossip on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/
        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOGOSSIP );
/*
        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
            argument = makedrunk ( argument, ch ); 
*/
//	    argument = makedrunk ( argument, ch );

        sprintf ( buf,
                  "{y-{Y={WYOU{Y={y- {w[{RI{rn {RC{rharacter{w] {x'{D%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOGOSSIP ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
//MORE DRUNK
/*                    if ( !IS_NPC ( ch ) &&
                         ch->pcdata->condition[COND_DRUNK] > 10 )
                        argument = makedrunk ( argument, ch ); */
//END
                    act_new
                        ( "{w[{RI{rn {RC{rharacter{w] {y-{Y={B$n{Y={y-{x '{D$t{x'",
                          ch, argument, d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_qgossip )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( !ch->on_quest && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "You are not on a quest!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOQGOSSIP ) )
        {
            send_to_char ( "Quest gossip channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOQGOSSIP );
        }
        else
        {
            send_to_char ( "Quest gossip channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOQGOSSIP );
        }
    }
    else                        /* qgossip message sent, turn qgossip on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/	
        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOQGOSSIP );

        sprintf ( buf, "You qgossip '{C%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOQGOSSIP ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) &&
                 ( ( victim->on_quest ) || IS_IMMORTAL ( victim ) ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "$n qgossips '{C$t{x'", ch, argument,
                              d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_grats )
{
    char buf[MAX_STRING_LENGTH];
    char bufy[MAX_STRING_LENGTH];
    char bufz[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    buf[0] = '\0';
    bufy[0] = '\0';
    bufz[0] = '\0';
    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOGRATS ) )
        {
            send_to_char ( "Grats channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOGRATS );
        }
        else
        {
            send_to_char ( "Grats channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOGRATS );
        }
    }
    else                        /* grats message sent, turn grats on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/	
        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOGRATS );

        sprintf ( buf, "You {RG{Yr{Ra{Yt{Rs{x '%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOGRATS ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "$n {RG{Yr{Ra{Yt{Rs{x '{x$t{x'", ch, argument,
                              d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_quote )
{
    char buf[MAX_STRING_LENGTH * 3];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOQUOTE ) )
        {
            send_to_char ( "Quote channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOQUOTE );
        }
        else
        {
            send_to_char ( "Quote channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOQUOTE );
        }
    }
    else                        /* quote message sent, turn quote on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }

        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }
/*        
	if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/
        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOQUOTE );

        sprintf ( buf, "{g-{G={bYou{G={g- {Rquote{x '{c%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOQUOTE ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{g-{G={b$n{G={g- {Rquotes{x '$t{x'", ch,
                              argument, d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

void social_channel ( const char *format, CHAR_DATA * ch, const void *arg2,
                      int type )
{
    static char *const he_she[] = { "it", "he", "she" };
    static char *const him_her[] = { "it", "him", "her" };
    static char *const his_her[] = { "its", "his", "her" };

    CHAR_DATA *to;
    CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
    const char *str;
    char *i;
    char *point;
    char buf[MAX_STRING_LENGTH];

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_QUIET ) )
    {
        send_to_char ( "You must turn off quiet mode first.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
    {
        send_to_char ( "The gods have revoked your channel priviliges.\n\r",
                       ch );
        return;
    }

    if ( !format || !*format )
        return;

    if ( !ch || !ch->desc || !ch->in_room )
        return;

    to = char_list;
    if ( type == TO_VICT )
    {
        if ( !vch )
        {
            bug ( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if ( !vch->in_room )
            return;

        to = vch;
    }
    if ( type == TO_CHAR )
    {
        if ( ch == NULL )
        {
            return;
        }
        to = ch;
    }

    for ( ; to; to = to->next )
    {
        if ( !to->desc || to->position < POS_RESTING || !to->in_room )
            continue;

        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
        if ( IS_SET ( to->comm, COMM_NOSOCIAL ) ||
             IS_SET ( to->comm, COMM_QUIET ) )
            continue;
        point = buf;
        sprintf ( buf, "[Gmote] " ); 
        point += 8;
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
        send_to_char ( "{W", to );
        if ( to->desc )
            write_to_buffer ( to->desc, buf, point - buf );
        send_to_char ( "{x", to );
    }
    return;
}

/* RT ask channel */
CH_CMD ( do_ask )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOASK ) )
        {
            send_to_char ( "Q/A channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOASK );
        }
        else
        {
            send_to_char ( "Q/A channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOASK );
        }
    }
    else                        /* ask sent, turn Q/A on if it isn't already */
    {
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
  */  
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }

        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOASK );

        sprintf ( buf, "You ask '{y%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOASK ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "$n asks '{y$t{x'", ch, argument, d->character,
                              TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

/* RT answer channel - uses same line as asks */
CH_CMD ( do_answer )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOASK ) )
        {
            send_to_char ( "Q/A channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOASK );
        }
        else
        {
            send_to_char ( "Q/A channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOASK );
        }
    }
    else                        /* answer sent, turn Q/A on if it isn't already */
    {
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
  */  
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }

        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOASK );

        sprintf ( buf, "You answer '{y%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOASK ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "$n answers '{y$t{x'", ch, argument,
                              d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

/* RT music channel */
CH_CMD ( do_music )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOMUSIC ) )
        {
            send_to_char ( "Music channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOMUSIC );
        }
        else
        {
            send_to_char ( "Music channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOMUSIC );
        }
    }
    else                        /* music sent, turn music on if it isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
        }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/	
        if ( ( !str_infix ( " fuck ", argument ) ) ||
             ( !str_prefix ( "fuck ", argument ) ) ||
             ( !str_suffix ( " fuck", argument ) ) ||
             ( !str_suffix ( " fuck.", argument ) ) ||
             ( !str_suffix ( " fuck!", argument ) ) ||
             ( !str_cmp ( "fuck", argument ) ) ||
             ( !str_cmp ( "fuck.", argument ) ) ||
             ( !str_cmp ( "fuck!", argument ) ) ||
             ( !str_cmp ( "shit", argument ) ) ||
             ( !str_cmp ( "shit.", argument ) ) ||
             ( !str_cmp ( "shit!", argument ) ) ||
             ( !str_infix ( " shit ", argument ) ) ||
             ( !str_prefix ( "shit ", argument ) ) ||
             ( !str_suffix ( " shit", argument ) ) ||
             ( !str_suffix ( " shit.", argument ) ) ||
             ( !str_suffix ( " shit!", argument ) ) )
        {
            if ( !IS_IMMORTAL ( ch ) )
            {
                send_to_char
                    ( "Don't let the IMM's catch you talking like that!\n\r",
                      ch );
                return;
            }
        }

        if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
        {
            send_to_char
                ( "Just keep your nose in the corner like a good little player.\n\r",
                  ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOMUSIC );

        sprintf ( buf, "You {CM{cU{CS{cI{CC{x: '{w%s{x'\n\r", argument );
        send_to_char ( buf, ch );
        sprintf ( buf, "$n {CM{cU{CS{cI{CC{x: '{w%s{x'", argument );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOMUSIC ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new ( "{W$n {cM{CU{cS{CI{cC{x: '{w$t{x'", ch,
                              argument, d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_hints )
{
if(IS_NPC(ch))
  return;

if(IS_SET(ch->comm,COMM_NOHINT))
  {
  REMOVE_BIT(ch->comm, COMM_NOHINT);
  send_to_char("Hint channel is now ON.\n\r",ch);
  return;
  }
else
  {
  SET_BIT(ch->comm, COMM_NOHINT);
  send_to_char("Hint channel is now OFF.\n\r",ch);
  return;
  }
}

CH_CMD ( do_announce )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] != '\0' )
    {
    sprintf ( buf, "{m-{M-{R>{r> {D$n {D%s{x", argument );
    act_new ( "{m-{M-{R>{r> {D$n {W$t{x", ch,
              argument, NULL, TO_CHAR, POS_DEAD );
          for ( d = descriptor_list; d != NULL; d = d->next )
          {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOANNOUNCE ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) )
            {
                act_new ( "{m-{M-{R>{r> {D$n {D$t{x", ch,
                       argument, d->character, TO_VICT, POS_DEAD );
            }
         }
    }
    return;
}

CH_CMD ( do_noannounce )
{
        if ( IS_SET ( ch->comm, COMM_NOANNOUNCE ) )
        {
            REMOVE_BIT ( ch->comm, COMM_NOANNOUNCE );            
	    send_to_char ( "You will now see announcements again.\n\r", ch );
        }
        else
        {
            SET_BIT ( ch->comm, COMM_NOANNOUNCE );            
	    send_to_char ( "You will no longer see announcements.\n\r", ch );
        }
return;
}
void do_gmessage ( char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf ( buf, "%s", argument );

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf, d->character );
        }
    }

    return;
}


CH_CMD ( do_immtalk )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOWIZ ) )
        {
            send_to_char ( "Immortal channel is now ON\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOWIZ );
        }
        else
        {
            send_to_char ( "Immortal channel is now OFF\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOWIZ );
        }
        return;
    }

    REMOVE_BIT ( ch->comm, COMM_NOWIZ );

    sprintf ( buf, "{W[{BI{bmm{BC{bhat{w] {r-{R={G$n{R={r- {x'{C%s{x'",
              argument );
    act_new ( "{W[{BI{bmm{BC{bhat{W] {r-{R={G$n{R={r- {x'{C$t{x'", ch,
              argument, NULL, TO_CHAR, POS_DEAD );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && IS_IMMORTAL ( d->character ) &&
             !IS_SET ( d->character->comm, COMM_NOWIZ ) )
        {
            act_new ( "{W[{BI{bmm{BC{bhat{W] {r-{R={G$n{R={r-{x'{C$t{x'", ch,
                      argument, d->character, TO_VICT, POS_DEAD );
        }
    }

    return;
}

/*
 * Different statements for punctuation at the end of a say now. 
 * -Marquoz (marquoz@gost.net)
 */
CH_CMD ( do_say )
{
    int length;

    if ( IS_SET ( ch->comm, COMM_NOSAY ) )
    {
	send_to_char ( "You are not allowed to say anything.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Say what?\n\r", ch );
        return;
    }

    /* Make the words drunk if needed */
/*	if (IS_DRUNK(ch))
*/

/*        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
	  argument = makedrunk ( argument, ch ); */

    length = strlen ( argument );

    /*
     * trailing {7 is to assure the last ' is colored right
     * trailing {x is to assure no colour bleed from the green -M
     * Added in functionality for different ways to say
     */

    if ( argument[length - 1] == '!' )
    {
        /* 'something!' = exclaim */
        if ( argument[length - 2] != '!' )
        {
            act ( "{W$n {Yexclaims{x '{W$T{x'{x", ch, NULL, argument,
                  TO_ROOM );
            act ( "{WYou {Yexclaim{x '{W$T'{x", ch, NULL, argument, TO_CHAR );
        }
        /* 'something!!!' = screams */
        else
        {
            act ( "{W$n {Rscreams {x'{W$T{x'{x", ch, NULL, argument,
                  TO_ROOM );
            act ( "{WYou {Rscream {x'{W$T{x'{x", ch, NULL, argument,
                  TO_CHAR );
        }
    }
    else if ( argument[length - 1] == '?' )
    {
        /* 'something?!' = boggles */
        if ( argument[length - 2] == '!' )
        {
            act ( "{W$n {gboggles {x'{W$T{x'{x", ch, NULL, argument,
                  TO_ROOM );
            act ( "{WYou {gboggle {x'{W$T{x'{x", ch, NULL, argument,
                  TO_CHAR );
        }
        /* 'something?' = asks */
        else if ( argument[length - 2] != '?' )
        {
            act ( "{W$n {Casks {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
            act ( "{WYou{C ask {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
        }
        /* 'something??' = confusedly asks */
        else if ( argument[length - 20] != '?' )
        {
            act ( "{W$n {Gdemands {x'{W$T{x'{x", ch, NULL, argument,
                  TO_ROOM );
            act ( "{WYou {Gdemand {x'{W$T{x'{x", ch, NULL, argument,
                  TO_CHAR );
        }
    }
    /* 'something...' = mutter */
    else if ( argument[length - 1] == '.' && argument[length - 2] == '.' &&
              argument[length - 3] == '.' )
    {
        act ( "{W$n{D mutters {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou{D mutter {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* '=), :) = chuckles */
    else if ( argument[length - 1] == ')' &&
              ( argument[length - 2] == '=' || argument[length - 2] == ':' ) )
    {
        act ( "{W$n {cchuckles {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou {cchuckle {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* '=(, :( = sulks */
    else if ( argument[length - 1] == '(' &&
              ( argument[length - 2] == '=' || argument[length - 2] == ':' ) )
    {
        act ( "{W$n {bsulks {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou {bsulk {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* '=P, :P = smirks */
    else if ( argument[length - 1] == 'P' &&
              ( argument[length - 2] == '=' || argument[length - 2] == ':' ) )
    {
        act ( "{W$n {rsmirks {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou {rsmirk {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* ';) = leers */
    else if ( argument[length - 1] == ')' && ( argument[length - 2] == ';' ) )
    {
        act ( "{W$n {yleers {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou {yleer {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* '=O, :O = sings */
    else if ( argument[length - 1] == 'O' &&
              ( argument[length - 2] == '=' || argument[length - 2] == ':' ) )
    {
        act ( "{W$n {Msings {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou {Msing {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }
    /* Add more punctuations with another else if statement here */
    else                        /* Any other punctuation */
    {
        act ( "{W$n says {x'{W$T{x'{x", ch, NULL, argument, TO_ROOM );
        act ( "{WYou say {x'{W$T{x'{x", ch, NULL, argument, TO_CHAR );
    }

    if ( !IS_NPC ( ch ) )
    {
        CHAR_DATA *mob, *mob_next;
        for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
        {
            mob_next = mob->next_in_room;
            if ( IS_NPC ( mob ) && HAS_TRIGGER ( mob, TRIG_SPEECH ) &&
                 mob->position == mob->pIndexData->default_pos )
                mp_act_trigger ( argument, mob, ch, NULL, NULL, TRIG_SPEECH );
        }
    }
    return;
}

/*void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    act( "{W$n says '$T{x'", ch, NULL, argument, TO_ROOM );
    act( "{WYou say '$T{x'", ch, NULL, argument, TO_CHAR );

    if ( !IS_NPC(ch) )
    {
	CHAR_DATA *mob, *mob_next;
	for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
	{
	    mob_next = mob->next_in_room;
	    if ( IS_NPC(mob) && HAS_TRIGGER( mob, TRIG_SPEECH )
	    &&   mob->position == mob->pIndexData->default_pos )
		mp_act_trigger( argument, mob, ch, NULL, NULL, TRIG_SPEECH );
	}
    }
    return;
}*/

CH_CMD ( do_shout )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOSHOUT ) )
        {
            send_to_char ( "You can hear shouts again.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOSHOUT );
        }
        else
        {
            send_to_char ( "You will no longer hear shouts.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOSHOUT );
        }
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOSHOUT ) )
    {
        send_to_char ( "You can't shout.\n\r", ch );
        return;
    }

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
  */  
    REMOVE_BIT ( ch->comm, COMM_NOSHOUT );

    WAIT_STATE ( ch, 12 );

    act ( "{WYou {B**{RSHOUT{B**{x '{W$T{x'", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *victim;
        int pos;
        bool found = FALSE;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING && d->character != ch &&
             !IS_SET ( victim->comm, COMM_NOSHOUT ) &&
             !IS_SET ( victim->comm, COMM_QUIET ) )
        {
            for ( pos = 0; pos < MAX_FORGET; pos++ )
            {
                if ( victim->pcdata->forget[pos] == NULL )
                    break;
                if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                    found = TRUE;
            }
            if ( !found )
            {
                act ( "{W$n {B**{RSHOUTS{B** {x'{W$t{x'", ch, argument,
                      d->character, TO_VICT );
            }
        }
    }

    return;
}

CH_CMD ( do_tell )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int pos;
    bool found = FALSE;
    int wtime;

    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOTELL ) || IS_SET ( ch->comm, COMM_DEAF ) )
    {
        send_to_char ( "Your message didn't get through.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_QUIET ) )
    {
        send_to_char ( "You must turn off quiet mode first.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_DEAF ) )
    {
        send_to_char ( "You must turn off deaf mode first.\n\r", ch );
        return;
    }

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "Tell whom what?\n\r", ch );
        return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world ( ch, arg ) ) == NULL ||
         ( IS_NPC ( victim ) && victim->in_room != ch->in_room ) )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 10 && !IS_NPC ( victim ) && !IS_IMMORTAL ( victim ) )
        {
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on tells. Until level 10, you will only be able to\n\r"
	   		  "send tells to immortals.\n\r", ch );
	   return;
	}
*/
    if ( IS_SET ( victim->comm, COMM_NOTELL ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "They can't send or receive tells.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL && !IS_NPC ( victim ) )
    {
        act ( "{W$N seems to have misplaced $S link. Your tell will go through when they regain their link.{x.", ch,
              NULL, victim, TO_CHAR );
        sprintf ( buf, "{D[{B%s {Btells you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        victim->reply = ch;
        return;
    }

    if ( !IS_NPC ( victim ) )
    {
        for ( pos = 0; pos < MAX_FORGET; pos++ )
        {
            if ( victim->pcdata->forget[pos] == NULL )
                break;
            if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                found = TRUE;
        }
    }
    if ( found )
    {
        act ( "$N doesn't seem to be listening to you.", ch, NULL, victim,
              TO_CHAR );
        return;
    }

/* Removed..Vengeance */
/*
    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/

    if ( ( IS_SET ( victim->comm, COMM_QUIET ) ||
           IS_SET ( victim->comm, COMM_DEAF ) ) && !IS_IMMORTAL ( ch ) )
    {
        act ( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
        return;
    }

    if ( IS_SET ( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC ( victim ) )
        {
            act ( "$E is AFK, and not receiving tells.", ch, NULL, victim,
                  TO_CHAR );
            return;
        }

        act ( "$E is AFK, $E will see your tell when they get back.", ch,
              NULL, victim, TO_CHAR );
        sprintf ( buf, "{D[{B%s {Btells you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        victim->reply = ch;	
        return;
    }

    if ( IS_SET ( victim->comm, COMM_STORE ) && ( victim->fighting != NULL ) )
    {
        if ( IS_NPC ( victim ) )
        {
            act ( "$E is fighting, and not receiving tells.", ch, NULL,
                  victim, TO_CHAR );
            return;
        }

        act ( "$E is fighting, but your tell will go through when $E finishes.", ch, NULL, victim, TO_CHAR );
	send_to_char ( "{wYou just received a tell! Type {C'{creplay{C' {wto see it.{x\n\r", victim );
        sprintf ( buf, "{D[{B%s {Btells you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        victim->reply = ch;	
        return;
    }

    act ( "{D[{BYou tell $N{D]{x '{c$t{x'", ch, argument, victim, TO_CHAR );
    act_new ( "{D[{B$n {Btells you{D] '{c$t{x'", ch, argument, victim, TO_VICT,
              POS_DEAD );
    victim->reply = ch;

    if ( !IS_NPC ( ch ) && IS_NPC ( victim ) &&
         HAS_TRIGGER ( victim, TRIG_SPEECH ) )
        mp_act_trigger ( argument, victim, ch, NULL, NULL, TRIG_SPEECH );

    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );

    return;
}

CH_CMD ( do_reply )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;
    int wtime;

    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOTELL ) )
    {
        send_to_char ( "Your message didn't get through.\n\r", ch );
        return;
    }

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }
    
    if ( ( victim = ch->reply ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 10 && !IS_NPC ( victim ) && !IS_IMMORTAL ( victim ) )
        {
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on tells. Until level 10, you will only be able to\n\r"
	   		  "send tells to immortals.\n\r", ch );
	   return;
	}
*/
    
    if ( victim->desc == NULL && !IS_NPC ( victim ) )
    {
        act ( "$N seems to have misplaced $S link...try again later.", ch,
              NULL, victim, TO_CHAR );
	sprintf ( buf, "{D[{B%s{B replies to you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        return;
    }

    if ( !IS_NPC ( victim ) )
    {
        for ( pos = 0; pos < MAX_FORGET; pos++ )
        {
            if ( victim->pcdata->forget[pos] == NULL )
                break;
            if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                found = TRUE;
        }
    }
    if ( found )
    {
        act ( "$N doesn't seem to be listening to you.", ch, NULL, victim,
              TO_CHAR );
        return;
    }

/* Removed...Vengeance */
/*
    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/

    if ( ( IS_SET ( victim->comm, COMM_QUIET ) ||
           IS_SET ( victim->comm, COMM_DEAF ) ) && !IS_IMMORTAL ( ch ) &&
         !IS_IMMORTAL ( victim ) )
    {
        act_new ( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,
                  POS_DEAD );
        return;
    }

/* Removed....Vengeance */
/*
    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }
*/

    if ( IS_SET ( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC ( victim ) )
        {
            act_new ( "$E is AFK, and not receiving tells.", ch, NULL, victim,
                      TO_CHAR, POS_DEAD );
            return;
        }

        act_new ( "$E is AFK, but your tell will go through when $E returns.",
                  ch, NULL, victim, TO_CHAR, POS_DEAD );
	sprintf ( buf, "{D[{B%s{B replies to you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        return;
    }

    if ( IS_SET ( victim->comm, COMM_STORE ) && ( victim->fighting != NULL ) )
    {
        if ( IS_NPC ( victim ) )
        {
            act ( "$E is fighting, and not receiving tells.", ch, NULL,
                  victim, TO_CHAR );
            return;
        }

        act ( "$E is fighting, but your tell will go through when $E finishes.", ch, NULL, victim, TO_CHAR );
	send_to_char ( "{wYou just received a tell! Type {C'{creplay{C' {wto see it.{x\n\r", victim );
	sprintf ( buf, "{D[{B%s{B replies to you{D] '{c%s{x'\n\r", PERS ( ch, victim ),
                  argument );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( victim->pcdata->buffer, buf );
	buffer_strcat ( victim->pcdata->buffer, buf );
        victim->tells++;
        return;
    }    
    
    act_new ( "{D[{BYou reply to {r$N{D] '{c$t{x'", ch, argument, victim,
              TO_CHAR, POS_DEAD );
    act_new ( "{D[{B$n {Breplies to you{D] '{c$t{x'", ch, argument, victim,
              TO_VICT, POS_DEAD );
    victim->reply = ch;

    wtime = UMAX ( 2, 9 - ( ch->level ) );
    WAIT_STATE ( ch, wtime );

    return;
}

CH_CMD ( do_yell )
{
    DESCRIPTOR_DATA *d;

    if ( IS_SET ( ch->comm, COMM_NOSHOUT ) )
    {
        send_to_char ( "You can't yell.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Yell what?\n\r", ch );
        return;
    }

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }

    act ( "{YYou {b!!{WYELL{b!!{x '{R$t{x'", ch, argument, NULL, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        int pos;
        bool found = FALSE;

        if ( d->connected == CON_PLAYING && d->character != ch &&
             d->character->in_room != NULL &&
             d->character->in_room->area == ch->in_room->area &&
             !IS_SET ( d->character->comm, COMM_QUIET ) )
        {
            for ( pos = 0; pos < MAX_FORGET; pos++ )
            {
                if ( d->character->pcdata->forget[pos] == NULL )
                    break;
                if ( !str_cmp
                     ( ch->name, d->character->pcdata->forget[pos] ) )
                    found = TRUE;
            }
            if ( !found )
            {
                act ( "{Y$n {b!!{WYELLS{b!!{x '{R$t{x'", ch, argument,
                      d->character, TO_VICT );
            }
        }
    }

    return;
}

CH_CMD ( do_emote )
{
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

/*        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
          argument = makedrunk ( ch, argument ); */

    MOBtrigger = FALSE;
    act ( "$n $T{x", ch, NULL, argument, TO_ROOM );
    act ( "$n $T{x", ch, NULL, argument, TO_CHAR );
    MOBtrigger = TRUE;
    return;
}

CH_CMD ( do_pmote )
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

    act ( "{c$n $t{x", ch, argument, NULL, TO_CHAR );

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( vch->desc == NULL || vch == ch )
            continue;

        if ( ( letter = strstr ( argument, vch->name ) ) == NULL )
        {
            MOBtrigger = FALSE;
            act ( "{c$N $t{x", vch, argument, ch, TO_CHAR );
            MOBtrigger = TRUE;
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

        MOBtrigger = FALSE;
        act ( "{c$N $t{x", vch, temp, ch, TO_CHAR );
        MOBtrigger = TRUE;
    }

    return;
}

/*
 * All the posing stuff.
 */
struct pose_table_type
{
    char *message[2 * MAX_CLASS];
};

const struct pose_table_type pose_table[] = {
    {
     {
      "You sizzle with {Be{bn{Be{br{Bg{by{x.",
      "$n sizzles with {Be{bn{Be{br{Bg{by{x.",
      "You feel very {Wholy{x.",
      "$n looks very {Wholy{x.",
      "You perform a small card trick.",
      "$n performs a small card trick.",
      "You show your bulging muscles.",
      "$n shows $s pitiful muscles.",
      "You nonchalantly turn wine into water.",
      "$n nonchalantly turns wine into water.",
      "... 98, 99, 100 ... you do pushups.",
      "..1....mmph!.. 2 .. $n does pushups.",
      "You deftly steal everyone's weapon.",
      "$n deftly steals your weapon."}
     },

    {
     {
      "You turn into a butterfly, then return to your normal shape.",
      "$n turns into a butterfly, then returns to $s normal shape.",
      "You nonchalantly turn wine into water.",
      "$n nonchalantly turns wine into water.",
      "You wiggle your ears alternately.",
      "$n wiggles $s ears alternately.",
      "You crack nuts between your fingers.",
      "$n cracks nuts between $s fingers.",
      "A {Whalo{x appears over your head.",
      "A {Whalo{x appears over $n's head.",
      "Deep in prayer, you levitate.",
      "Deep in prayer, $n levitates.",
      "You steal the underwear off every person in the room.",
      "Your underwear is gone!  $n stole it!"}
     },

    {
     {
      "{BBlue sparks{x fly from your fingers.",
      "{BBlue sparks{x fly from $n's fingers.",
      "A {Whalo{x appears over your head.",
      "A {Whalo{x appears over $n's head.",
      "You nimbly tie yourself into a knot.",
      "$n nimbly ties $mself into a knot.",
      "You grizzle your teeth and look mean.",
      "$n grizzles $s teeth and looks mean.",
      "Crunch, crunch -- you munch a bottle.",
      "Crunch, crunch -- $n munches a bottle.",
      "You juggle with daggers, apples, and eyeballs.",
      "$n juggles with daggers, apples, and eyeballs.",
      "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
      "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs."}
     },

    {
     {
      "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
      "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs.",
      "You recite words of wisdom.",
      "$n recites words of wisdom.",
      "You juggle with daggers, apples, and eyeballs.",
      "$n juggles with daggers, apples, and eyeballs.",
      "You hit your head, and your eyes roll.",
      "$n hits $s head, and $s eyes roll.",
      "You nimbly tie yourself into a knot.",
      "$n nimbly ties $mself into a knot.",
      "A spot light hits you.",
      "A spot light hits $n.",
      "You count the money in everyone's pockets.",
      "Check your money, $n is counting it."}
     },

    {
     {
      "A slimy {ggreen monster{x appears before you and bows.",
      "A slimy {ggreen monster{x appears before $n and bows.",
      "Deep in prayer, you levitate.",
      "Deep in prayer, $n levitates.",
      "You steal the underwear off every person in the room.",
      "Your underwear is gone!  $n stole it!",
      "Crunch, crunch -- you munch a bottle.",
      "Crunch, crunch -- $n munches a bottle.",
      "A fire elemental singes your hair.",
      "A fire elemental singes $n's hair.",
      "The sun pierces through the clouds to illuminate you.",
      "The sun pierces through the clouds to illuminate $n.",
      "Your eyes dance with greed.",
      "$n's eyes dance with greed."}
     },

    {
     {
      "You turn everybody into a little pink elephant.",
      "You are turned into a little pink elephant by $n.",
      "An {Wangel{x consults you.",
      "An {Wangel{x consults $n.",
      "The dice roll ... and you win again.",
      "The dice roll ... and $n craps out.",
      "... 98, 99, 100 ... you do pushups.",
      "..1....mmph!.. 2 .. $n does pushups.",
      "Smoke and fumes leak from your nostrils.",
      "Smoke and fumes leak from $n's nostrils.",
      "You balance a pocket knife on your tongue.",
      "$n balances a pocket knife on your tongue.",
      "Watch your feet, you are juggling granite boulders.",
      "Watch your feet, $n is juggling granite boulders."}
     },

    {
     {
      "A small ball of light dances on your fingertips.",
      "A small ball of light dances on $n's fingertips.",
      "Your body {Yglows with an unearthly light{x.",
      "$n's body {Yglows with an unearthly light{x.",
      "You count the money in everyone's pockets.",
      "Check your money, $n is counting it.",
      "Arnold Schwarzenegger admires your physique.",
      "PeeWee Herman laughs at $n's physique.",
      "Everyone levitates as you pray.",
      "You levitate as $n prays.",
      "You produce a coin from everyone's ear.",
      "$n produces a coin from your ear.",
      "Oomph!  You squeeze water out of a granite boulder.",
      "Oomph!  $n squeezes water out of a granite boulder."}
     },

    {
     {
      "Smoke and fumes leak from your nostrils.",
      "Smoke and fumes leak from $n's nostrils.",
      "A spot light hits you.",
      "A spot light hits $n.",
      "You balance a pocket knife on your tongue.",
      "$n balances a pocket knife on your tongue.",
      "Watch your feet, you are juggling granite boulders.",
      "Watch your feet, $n is juggling granite boulders.",
      "A {Ccool breeze{x refreshes you.",
      "A {Ccool breeze{x refreshes $n.",
      "You step behind your {Dshadow{x.",
      "$n steps behind $s {Dshadow{x.",
      "Arnold Schwarzenegger admires your physique.",
      "PeeWee Herman laughs at $n's physique."}
     },

    {
     {
      "The light flickers as you rap in magical languages.",
      "The light flickers as $n raps in magical languages.",
      "Everyone levitates as you pray.",
      "You levitate as $n prays.",
      "You produce a coin from everyone's ear.",
      "$n produces a coin from your ear.",
      "Oomph!  You squeeze water out of a granite boulder.",
      "Oomph!  $n squeezes water out of a granite boulder.",
      "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
      "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
      "The ocean parts before you.",
      "The ocean parts before $n.",
      "Your head disappears.",
      "$n's head disappears. {D(I was tired of looking at it anyway){x"}
     },

    {
     {
      "Your head disappears.",
      "$n's head disappears. {D(I was tired of looking at it anyway){x",
      "A {Ccool breeze{x refreshes you.",
      "A {Ccool breeze{x refreshes $n.",
      "You step behind your {Dshadow{x.",
      "$n steps behind $s {Dshadow{x.",
      "You pick your teeth with a spear.",
      "$n picks $s teeth with a spear.",
      "The stones dance to your command.",
      "The stones dance to $n's command.",
      "A thunder cloud kneels to you.",
      "A thunder cloud kneels to $n.",
      "The Grey Mouser buys you a beer.",
      "The Grey Mouser buys $n a beer."}
     },

    {
     {
      "A fire elemental singes your hair.",
      "A fire elemental singes $n's hair.",
      "The sun pierces through the clouds to illuminate you.",
      "The sun pierces through the clouds to illuminate $n.",
      "Your eyes dance with greed.",
      "$n's eyes dance with greed.",
      "Everyone is swept off their foot by your hug.",
      "You are swept off your feet by $n's hug.",
      "Your head disappears.",
      "$n's head disappears. {D(I was tired of looking at it anyway){x",
      "A {Ccool breeze{x refreshes you.",
      "A {Ccool breeze{x refreshes $n.",
      "You step behind your {Dshadow{x.",
      "$n steps behind $s {Dshadow{x."}
     },

    {
     {
      "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
      "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
      "The ocean parts before you.",
      "The ocean parts before $n.",
      "You deftly steal everyone's weapon.",
      "$n deftly steals your weapon.",
      "Your karate chop splits a tree.",
      "$n's karate chop breaks $s hand.",
      "A thunder cloud kneels to you.",
      "A thunder cloud kneels to $n.",
      "The Grey Mouser buys you a beer.",
      "The Grey Mouser buys $n a beer.",
      "A strap of your armor breaks over your mighty thews.",
      "A strap of $n's armor breaks over $s rolls of fat."}
     },

    {
     {
      "The stones dance to your command.",
      "The stones dance to $n's command.",
      "A thunder cloud kneels to you.",
      "A thunder cloud kneels to $n.",
      "The Grey Mouser buys you a beer.",
      "The Grey Mouser buys $n a beer.",
      "A strap of your armor breaks over your mighty thews.",
      "A strap of $n's armor breaks over $s rolls of fat.",
      "The heavens and grass change colour as you smile.",
      "The heavens and grass change colour as $n smiles.",
      "The Burning Man speaks to you.",
      "The Burning Man speaks to $n.",
      "Everyone's pocket explodes with your fireworks.",
      "Your pocket explodes with $n's fireworks."}
     },

    {
     {
      "The heavens and grass change colour as you smile.",
      "The heavens and grass change colour as $n smiles.",
      "The Burning Man speaks to you.",
      "The Burning Man speaks to $n.",
      "Everyone's pocket explodes with your fireworks.",
      "Your pocket explodes with $n's fireworks.",
      "A boulder cracks at your frown.",
      "A boulder cracks at $n's frown.",
      "Everyone's clothes are transparent, and you are laughing.",
      "Your clothes are transparent, and $n is laughing.",
      "An eye in a pyramid w{zi{xnks at you.",
      "An eye in a pyramid w{zi{xnks at $n.",
      "Everyone discovers your dagger a centimeter from their eye.",
      "You discover $n's dagger a centimeter from your eye."}
     },

    {
     {
      "Everyone's clothes are transparent, and you are laughing.",
      "Your clothes are transparent, and $n is laughing.",
      "An eye in a pyramid w{zi{xnks at you.",
      "An eye in a pyramid w{zi{xnks at $n.",
      "Everyone discovers your dagger a centimeter from their eye.",
      "You discover $n's dagger a centimeter from your eye.",
      "Mercenaries arrive to do your bidding.",
      "Mercenaries arrive to do $n's bidding.",
      "A black hole swallows you.",
      "A black hole swallows $n.",
      "Valentine Michael Smith offers you a glass of water.",
      "Valentine Michael Smith offers $n a glass of water.",
      "Where did you go?",
      "Where did $n go?"}
     },

    {
     {
      "A black hole swallows you.",
      "A black hole swallows $n.",
      "Valentine Michael Smith offers you a glass of water.",
      "Valentine Michael Smith offers $n a glass of water.",
      "Where did you go?",
      "Where did $n go?",
      "Four matched Percherons bring in your chariot.",
      "Four matched Percherons bring in $n's chariot.",
      "The world shimmers in time with your whistling.",
      "The world shimmers in time with $n's whistling.",
      "The great god $G gives you a staff.",
      "The great god $G gives $n a staff.",
      "Everyone's clothes are transparent, and you are laughing.",
      "Your clothes are transparent, and $n is laughing."}
     },

    {
     {
      "The world shimmers in time with your whistling.",
      "The world shimmers in time with $n's whistling.",
      "The great god $G gives you a staff.",
      "The great god $G gives $n a staff.",
      "Click.",
      "Click.",
      "Atlas asks you to relieve him.",
      "Atlas asks $n to relieve him.",
      "Four matched Percherons bring in your chariot.",
      "Four matched Percherons bring in $n's chariot.",
      "Mercenaries arrive to do your bidding.",
      "Mercenaries arrive to do $n's bidding.",
      "A boulder cracks at your frown.",
      "A boulder cracks at $n's frown."}
     }
};
/*
CH_CMD ( do_pose )
{
    int level;
    int pose;

    if ( IS_NPC ( ch ) )
        return;

    level =
        UMIN ( ch->level,
               sizeof ( pose_table ) / sizeof ( pose_table[0] ) - 1 );
    pose = number_range ( 0, level );

    if ( ch->class < MAX_CLASS / 2 )
    {
        act ( pose_table[pose].message[2 * ch->class + 0], ch, NULL, NULL,
              TO_CHAR );
        act ( pose_table[pose].message[2 * ch->class + 1], ch, NULL, NULL,
              TO_ROOM );
    }
    else
    {
        act ( pose_table[pose].
              message[2 * ( ch->class - ( MAX_CLASS / 2 ) ) + 0], ch, NULL,
              NULL, TO_CHAR );
        act ( pose_table[pose].
              message[2 * ( ch->class - ( MAX_CLASS / 2 ) ) + 1], ch, NULL,
              NULL, TO_ROOM );
    }
    return;
}
*/
CH_CMD ( do_bug )
{
//    append_file ( ch, BUG_FILE, argument );
    send_to_char ( "This command has been disabled. Use the bugs and typos board to inform about bugs.\n\r", ch );
    return;
}

CH_CMD ( do_typo )
{
//    append_file ( ch, TYPO_FILE, argument );
    send_to_char ( "This command has been disabled. Use the bugs and typos board to inform about typos.\n\r", ch );
    return;
}

CH_CMD ( do_rent )
{
    send_to_char ( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}

CH_CMD ( do_qui )
{
    send_to_char ( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}

/* 
 * The Routine
 * Quote Code is by elfren@aros.net
 */

void do_qquote ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    int number;

    number = number_range ( 0, MAX_QUOTES );

    sprintf ( buf, "\n\r{*%s\n\r\n\r {x- {D%s{x\n\r", quote_table[number].text,
              quote_table[number].by );
    send_to_char ( buf, ch );
    return;
}


CH_CMD ( do_quit )
{
//    char strsave[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d, *d_next;
    int id;
    bool relog = FALSE;

    if ( IS_NPC ( ch ) )
        return;

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "No way! You are fighting.\n\r", ch );
        return;
    }
    if ( auction_list != NULL &&
         ( auction_list->high_bidder == ch || auction_list->owner == ch ) )
    {
        send_to_char ( "You still have a stake in the auction!\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->act, PLR_QUESTOR ) )
    {
        printf_to_char ( ch,
                         "{WHow do you expect to {Yleave{W while on a quest?{x\n\r" );
        return;
    }

    if ( IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
    {
	send_to_char ( "Not until the tournament is over.\n\r", ch );
	return;
    }

    if ( IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
	send_to_char ( "You have been challenged. You have to decline or fight the match before you leave.\n\r", ch );
	return;
    }

    if ( IS_SET ( ch->act2, PLR2_CHALLENGER ) )
    {
	send_to_char ( "You have challenged someone. You have fight the match or let the challenge time out before you leave.\n\r", ch);
	return;
    }

    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
	send_to_char ( "You are {Rviolent{x! You cannot quit!\n\r", ch );
	return;
    }


    if (IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
    {
      send_to_char ("You can't quit while you're still playing freeze tag.\n\r", ch);
      return;
    }


    if ( ch->position < POS_STUNNED )
    {
        send_to_char ( "You're not DEAD yet.\n\r", ch );
        return;
    }
    
    if ( !IS_VALID ( ch ) )
    {
        send_to_char ( "Your character is not valid! Please consult a level 216 immediately!\n\rIf you really need to leave now, drop a quick note to Rhaelar saying something's wrong with your character, then disconnect your client.\n\r", ch );
        bug ( "char_update: trying to work with invalid character", 0 );
        return;
    }
    
/*
    if ( ch->level < 2 && IS_SET ( class_table[ch->class].tier, TIER_01 ) )
    {
        send_to_char
            ( "\n\r\n\r{RYou are below level 5!\n\r {YYOUR CHARACTER {C{zWILL NOT BE SAVED{x BEFORE LEVEL 2!{Y!!\n\r If you want to leave anyway, just type 'delete' twice.\n\r\n\r", ch );
        return;
    }
*/

/*    do_qquote (ch);  */
/*
   if ( ch->pcdata->nextquest > 0 )
   SET_BIT ( ch->act, PLR_WASQUESTING );
*/
   if ( IS_SET ( ch->comm, COMM_AFK ) )
	REMOVE_BIT ( ch->comm, COMM_AFK );

/*       printf_to_char(ch,"{DBut alas you must.{x\n\r");
       printf_to_char(ch,"{WFor now we say good-bye, but we hope you shall return.{x\n\r\n\r");
       send_to_char( "{WCome back soon to Shattered Dreams.{x\n\r\n\r",ch); */
//      WAIT_STATE ( ch, 25 * PULSE_VIOLENCE );
    act ( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf ( log_buf, "%s has quit.", ch->name );
    log_string ( log_buf );

    wiznet ( "$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
             get_trust ( ch ) );

   /* do_announce ( ch, "has left the realms." ); */

   if ( !IS_IMMORTAL ( ch ) )
      do_announce ( ch, "{Dhas left to chase a {Md{mr{De{ma{Mm{D.{x\n\r" ); 
/*
    send_to_char ( "\n\r\n\r {R@@@@@@   @@@  @@@   @@@@@@   @@@@@@@  @@@@@@@  @@@@@@@@  @@@@@@@   @@@@@@@@  @@@@@@@\n\r", ch );
    send_to_char ( "@@@@@@@   @@@  @@@  @@@@@@@@  @@@@@@@  @@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@\n\r", ch );
    send_to_char ( "{r!{R@@       @@{r!{R  @@@  @@{r! {R @@@    @@{r!      {R@@{r!    {R@@{r!      {R @@{r!  {R@@@  @@{r!       @@{r! {R @@@\n\r", ch );
    send_to_char ( "{r!{R@{r!       !{R@{r!  {R@{r!{R@  {r!{R@{r!  {R@{r!{R@    {r!{R@{r!      !{R@{r!    !{R@{r!       !{R@{r!  {R@{r!{R@  {r!{R@{r!       !{R@{r!  {R@{r!{R@\n\r", ch );
    send_to_char ( "{r!!{R@@{r!!    {R@{r!{R@{r!{R@{r!{R@{r!  {R@{r!{R@{r!{R@{r!{R@{r!    {R@{r!!      {R@{r!!    {R@{r!!!{D:{r!    {R@{r!{R@{r!!{R@{r!   {R@{r!!!{D:{r!    {R@{r!{R@  {r!{R@{r!\n\r", ch );
    send_to_char ( " {r!!{R@{r!!!   !!!{R@{r!!!!  !!!{R@{r!!!!    !!!      !!!    !!!!!{D:    {r!!{R@{r!{R@{r!    !!!!!{D:    {r!{R@{r!  !!!\n\r", ch );
    send_to_char ( "     {r!{D:{r!  {r!!{D: {r !!!  !!{D:  {r!!!    !!{D:      {r!!{D:    {r!!{D:       {r!!{D: :{r!!   !!{D:       {r!!{D:  {r!!!\n\r", ch );
    send_to_char ( "    {r!{D:{r!   {D:{r!{D:  {r!{D:{r!  {D:{r!{D:  {r!{D:{r!    {D:{r!{D:      :{r!{D:    :{r!{D:       :{r!{D:  {r!{D:{r!  {D:{r!{D:       :{r!{D:  {r!{D:{r!\n\r", ch );
    send_to_char ( "{D:::: ::   ::   :::  ::   :::     ::       ::     :: ::::  ::   :::   :: ::::   :::: ::\n\r", ch );
    send_to_char ( "{D:: : :     :   : :   :   : :     :        :     : :: ::    :   : :  : :: ::   :: :  :{x\n\r\n\r", ch );
    send_to_char ( "             {R@@@@@@@   @@@@@@@   @@@@@@@@   @@@@@@   @@@@@@@@@@    @@@@@@\n\r", ch );
    send_to_char ( "             @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@@@@  @@@@@@@\n\r", ch );
    send_to_char ( "             @@{r!  {R@@@  @@{r!  @@@  @@{r!      {R @@{r! {R @@@  @@{r! {R@@{r! {R@@{r!  !{R@@    \n\r", ch );
    send_to_char ( "             {r!{R@{r!  {R@{r!{R@  {r!{R@{r! {R @{r!{R@  {r!{R@{r!       {r!{R@{r!  {R@{r!{R@  {r!{R@{r! !{R@{r! !{R@{r!  !{R@{r!    \n\r", ch );
    send_to_char ( "             {R@{r!{R@  {r!{R@{r!  {R@{r!{R@{r!!{R@{r!   {R@{r!!!{D:{r!    {R@{r!{R@{r!{R@{r!{R@{r!  {R@{r!! !!{R@ @{r!{R@  {r!!{R@@{r!! \n\r", ch );
    send_to_char ( "             {r!{R@{r!  !!!  !!{R@{r!{R@{r!    !!!!!{D:    {r!!!{R@{r!!!!  !{R@{r!   ! !{R@{r!   !!{R@{r!!!\n\r", ch );
    send_to_char ( "             {r!!{D:  {r!!!  !!{D: :{r!!   !!{D:       {r!!{D:  {r!!!  !!{D:     {r!!{D:      {r !{D:{r!\n\r", ch );
    send_to_char ( "             {D:{r!{D:  {r!{D:{r! {D :{r!{D:  {r!{D:{r!  {D:{r!{D:       :{r!{D:  {r!{D:{r!  {D:{r!{D:     :{r!{D:      {r!{D:{r!\n\r", ch );
    send_to_char ( "             {D:::: ::  ::   :::   :: ::::  ::   :::  :::     ::   :::: :: \n\r", ch );
    send_to_char ( "             {D:: :  :    :   : :  : :: ::    :   : :   :      :    :: : :{x \n\r\n\r", ch );
*/
    /*
     * After extract_char the ch is no longer valid!
     */
/*    if ( ch-> level < 5 && IS_SET ( class_table[ch->class].tier, TIER_01 ) )
    {
       send_to_char ( "\n\r{CYou didn't reach level 5 before you quit.\n\rYour character was deleted.{x\n\r\n\r", ch );
       sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( ch->name ) );
       extract_char ( ch, TRUE );
       unlink ( strsave );
       return;
    } */

    send_to_char ( "-> {xHandling quit request.\n\r", ch );
    /* Free note that might be there somehow */
    if ( IS_SET ( ch->act2, PLR2_PKSAFE ) )
    {
	 REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
	 ch->pcdata->pksafetime = 0;
	 send_to_char ( "-> {xRemoved safe flag.\n\r", ch );
    }    
    if (ch->pcdata->in_progress)
    {
	free_note (ch->pcdata->in_progress);
    	send_to_char ( "-> {xCleared the note you had in progress.\n\r", ch );    
    }
    save_char_obj ( ch );
    send_to_char ( "-> {xSaved your character.\n\r", ch );    
    
    if ( !strcmp ( "relog", argument ) )
    {
    	send_to_char ( "-> {xCommencing character relog.\n\r", ch );
        relog = TRUE;
    }        
        
    id = ch->id;        
    d = ch->desc;
    send_to_char ( "-> {xExtracting your character.\n\r", ch );
    extract_char ( ch, TRUE );    
        
  if ( !relog )
  {    
    if ( d != NULL )
    { 
       write_to_buffer ( d, "\n\rSee you soon on Shattered Dreams!\n\rDon't forget to visit our forums at www.sdmud.com!\n\r", 0 );
       close_socket ( d );
    }
  }
  else
  {
    write_to_buffer ( d, "-> Character relog ready.\n\r-> Jumping back to greeting screen.\n\r\n\r", 0 );
    send_greeting ( d );
    d->connected = CON_GET_NAME;
  }
      
    /* toast evil cheating bastards */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        CHAR_DATA *tch;
        d_next = d->next;
        tch = d->original ? d->original : d->character;
        if ( tch && tch->id == id )
        {
            extract_char ( tch, TRUE );
	    send_to_char ( "Don't even try, stupid. Play the game like you're supposed to.\n\r", tch );
            close_socket ( d );
        }
    } 
    return;
}

void force_quit ( CHAR_DATA * ch, char *argument )
{
    DESCRIPTOR_DATA *d, *d_next;
    int id;
    int lose;
    if ( IS_NPC ( ch ) )
        return;
    if ( ch->level >= LEVEL_IMMORTAL )
        return;
    if ( ch->position == POS_FIGHTING )
    {
        lose = ( ch->desc != NULL ) ? 25 : 50;
        gain_exp ( ch, 0 - lose );
        stop_fighting ( ch, TRUE );
    }

    if ( ch->position < POS_STUNNED )
    {
        lose = ( ch->desc != NULL ) ? 50 : 100;
        gain_exp ( ch, 0 - lose );
    }
/*    send_to_char ( "{rTo bad your leaving we will miss you.{x\n\r", ch );
    send_to_char ( "{wYour slash hits Reality for {G*{W666{G* damage!{x\n\r",
                   ch );
    send_to_char
        ( "{BReality's truth {mdoes {RUNSPEAKABLE {mthings{x to you for {W*{R32700{W* {xdamage!{x\n\r\n\r",
          ch ); */
    WAIT_STATE ( ch, 25 * PULSE_VIOLENCE );
    act ( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf ( log_buf, "%s has quit.", ch->name );
    log_string ( log_buf );
    wiznet ( "$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
             get_trust ( ch ) );
    /*
     * After extract_char the ch is no longer valid!
     */

    /* Free note that might be there somehow */
    if (ch->pcdata->in_progress)
	free_note (ch->pcdata->in_progress);

    save_char_obj ( ch );
    id = ch->id;
    d = ch->desc;
    extract_char ( ch, TRUE );
    if ( d != NULL )
        close_socket ( d );
    /* toast evil cheating bastards */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        CHAR_DATA *tch;
        d_next = d->next;
        tch = d->original ? d->original : d->character;
        if ( tch && tch->id == id )
        {
            extract_char ( tch, TRUE );
            close_socket ( d );
        }
    }

    return;
}

CH_CMD ( do_save )
{
    if ( IS_NPC ( ch ) )
        return;
/*
    if ( ch-> level < 5 && IS_SET ( class_table[ch->class].tier, TIER_01 ) )
    {
       send_to_char ( "{CYou need to be at least level 5 tier 1 before you can save your character.{x\n\r", ch );
       return;
    }
*/
    if ( !IS_VALID ( ch ) )
    {
        send_to_char ( "Your character is not valid! Please consult a level 216 immediately!\n\r", ch );
        bug ( "char_update: trying to work with invalid character", 0 );
        return;
    }

    save_char_obj ( ch );
    send_to_char ( "Your Shattered Dreams character has been saved.\n\r", ch );
    send_to_char ( "If you wish to backup your pfile, type 'backup'.\n\r", ch );
/*    do_backup ( ch, NULL ); */
    //WAIT_STATE(ch, PULSE_VIOLENCE);
    return;
}

CH_CMD ( do_follow )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Follow whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL )
    {
        act ( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
        return;
    }

    if ( victim == ch )
    {
        if ( ch->master == NULL )
        {
            send_to_char ( "You already follow yourself.\n\r", ch );
            return;
        }
        stop_follower ( ch );
        return;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_ARENA) )
    {
        send_to_char ( "You can't group with people in the tournament/arena.\n\rIt is a matter of proving who is the best, not what groupers are the best.\n\r",
                       ch );
        return;
    }

    if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_NOFOLLOW ) &&
         !IS_IMMORTAL ( ch ) )
    {
        act ( "$N doesn't seem to want any followers.\n\r", ch, NULL, victim,
              TO_CHAR );
        return;
    }
/*
    if ( ( !IS_NPC ( ch ) && !IS_NPC ( victim ) ) && ( !IS_IMMORTAL ( ch ) )
         && ( !IS_IMMORTAL ( victim ) ) && ( ch != victim ) &&
         ( !str_cmp ( ch->pcdata->socket, victim->pcdata->socket ) ) )
    {
        act ( "You can't seem to follow $N.\n\r", ch, NULL, victim, TO_CHAR );
        return;
    }
*/
    REMOVE_BIT ( ch->act, PLR_NOFOLLOW );
    if ( ch->master != NULL )
        stop_follower ( ch );
    add_follower ( ch, victim );
    return;
}

void add_follower ( CHAR_DATA * ch, CHAR_DATA * master )
{
    if ( ch->master != NULL )
    {
        bug ( "Add_follower: non-null master.", 0 );
        return;
    }

    ch->master = master;
    ch->leader = NULL;
    if ( can_see ( master, ch ) )
        act ( "$n now follows you.", ch, NULL, master, TO_VICT );
    act ( "You now follow $N.", ch, NULL, master, TO_CHAR );
    return;
}

void stop_follower ( CHAR_DATA * ch )
{
    if ( ch->master == NULL )
    {
        bug ( "Stop_follower: null master.", 0 );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) )
    {
        REMOVE_BIT ( ch->affected_by, AFF_CHARM );
        affect_strip ( ch, gsn_charm_person );
    }
    if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_PET ) )
    {
        REMOVE_BIT ( ch->act, ACT_PET );
        ch->timer = 0;
        ch->hastimer = TRUE;
    }

    if ( can_see ( ch->master, ch ) && ch->in_room != NULL )
    {
        act ( "$n stops following you.", ch, NULL, ch->master, TO_VICT );
        act ( "You stop following $N.", ch, NULL, ch->master, TO_CHAR );
    }
    if ( ch->master->pet == ch )
        ch->master->pet = NULL;
    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets ( CHAR_DATA * ch )
{
    CHAR_DATA *pet;
    if ( ( pet = ch->pet ) != NULL )
    {
        stop_follower ( pet );
        if ( pet->in_room != NULL )
            act ( "$N slowly fades away.", ch, NULL, pet, TO_NOTVICT );
        extract_char ( pet, TRUE );
    }
    ch->pet = NULL;
    return;
}

void die_follower ( CHAR_DATA * ch )
{
    CHAR_DATA *fch;
    if ( ch->master != NULL )
    {
        if ( ch->master->pet == ch )
            ch->master->pet = NULL;
        stop_follower ( ch );
    }

    ch->leader = NULL;
    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
        if ( fch->master == ch )
            stop_follower ( fch );
        if ( fch->leader == ch )
            fch->leader = fch;
    }

    return;
}

CH_CMD ( do_order )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;
    argument = one_argument ( argument, arg );
    one_argument ( argument, arg2 );
    if ( !str_cmp ( arg2, "delete" ) || !str_cmp ( arg2, "mob" ) )
    {
        send_to_char ( "That will NOT be done.\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg2, "reroll" ) )
    {
        send_to_char ( "That will NOT be done.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "Order whom to do what?\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) )
    {
        send_to_char ( "You feel like taking, not giving, orders.\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        fAll = TRUE;
        victim = NULL;
    }
    else
    {
        fAll = FALSE;
        if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
        {
            send_to_char ( "They aren't here.\n\r", ch );
            return;
        }

        if ( victim == ch )
        {
            send_to_char ( "Aye aye, right away!\n\r", ch );
            return;
        }

        if ( !IS_AFFECTED ( victim, AFF_CHARM ) || victim->master != ch ||
             ( IS_IMMORTAL ( victim ) && victim->trust >= ch->trust ) )
        {
            send_to_char ( "Do it yourself!\n\r", ch );
            return;
        }
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
        och_next = och->next_in_room;
        if ( IS_AFFECTED ( och, AFF_CHARM ) && och->master == ch &&
             ( fAll || och == victim ) )
        {
            found = TRUE;
            sprintf ( buf, "$n orders you to '%s'.", argument );
            act ( buf, ch, NULL, och, TO_VICT );
            interpret ( och, argument );
        }
    }

    if ( found )
    {
        WAIT_STATE ( ch, PULSE_VIOLENCE );
        send_to_char ( "Ok.\n\r", ch );
    }
    else
        send_to_char ( "You have no followers here.\n\r", ch );
    return;
}

CH_CMD ( do_group )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        CHAR_DATA *gch;
        CHAR_DATA *leader;
        leader = ( ch->leader != NULL ) ? ch->leader : ch;
        sprintf ( buf, "%s's group:\n\r", PERS ( leader, ch ) );
        send_to_char ( buf, ch );
        for ( gch = char_list; gch != NULL; gch = gch->next )
        {
            if ( is_same_group ( gch, ch ) )
            {
                sprintf ( buf,
                          "[%2d %s] %-16s %ld/%ld hp %ld/%ld mana %ld/%ld mv %5ld xp\n\r",
                          gch->level,
                          IS_NPC ( gch ) ? "Mob" : class_table[gch->class].
                          who_name, capitalize ( PERS ( gch, ch ) ), gch->hit,
                          gch->max_hit, gch->mana, gch->max_mana, gch->move,
                          gch->max_move, gch->exp );
                send_to_char ( buf, ch );
            }
        }
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
        send_to_char ( "But you are following someone else!\n\r", ch );
        return;
    }

    if ( victim->master != ch && ch != victim )
    {
        act_new ( "$N isn't following you.", ch, NULL, victim, TO_CHAR,
                  POS_SLEEPING );
        return;
    }

    if ( IS_AFFECTED ( victim, AFF_CHARM ) )
    {
        send_to_char ( "You can't remove charmed mobs from your group.\n\r",
                       ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) )
    {
        act ( "You like your master too much to leave $m!", ch, NULL, victim,
              TO_VICT );
        return;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_ARENA) )
    {
        send_to_char ( "You can't group with people in the tournament/arena.\n\rIt is a matter of proving who is the best, not what groupers are the best.\n\r",
                       ch );
        return;
    }

    if ( victim->level - ch->level > 25 )
    {
        send_to_char ( "They are to high of a level for your group.\n\r",
                       ch );
        return;
    }

    if ( victim->level - ch->level < -14 )
    {
        send_to_char ( "They are to low of a level for your group.\n\r", ch );
        return;
    }

    if ( is_same_group ( victim, ch ) && ch != victim )
    {
        victim->leader = NULL;
        act ( "$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT );
        act ( "$n removes you from $s group.", ch, NULL, victim, TO_VICT );
        act ( "You remove $N from your group.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ( !IS_NPC ( ch ) && !IS_NPC ( victim ) ) && ( !IS_IMMORTAL ( ch ) )
         && ( !IS_IMMORTAL ( victim ) ) && ( ch != victim ) &&
         ( !str_cmp ( ch->pcdata->socket, victim->pcdata->socket ) ) )
    {
        send_to_char ( "They are unable to join your group.\n\r", ch );
        return;
    }

    victim->leader = ch;
    act ( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act ( "You join $n's group.", ch, NULL, victim, TO_VICT );
    act ( "$N joins your group.", ch, NULL, victim, TO_CHAR );
    return;
}

/*
 * 'Split' originally by Gnort, God of Chaos.
 */
CH_CMD ( do_split )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_platinum = 0, amount_gold = 0, amount_silver = 0;
    int share_platinum, share_gold, share_silver;
    int extra_platinum, extra_gold, extra_silver;
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    one_argument ( argument, arg3 );
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Split how much?\n\r", ch );
        return;
    }

    amount_silver = atoi ( arg1 );
    if ( arg2[0] != '\0' )
        amount_gold = atoi ( arg2 );
    if ( arg3[0] != '\0' )
        amount_platinum = atoi ( arg3 );
    if ( amount_platinum < 0 || amount_gold < 0 || amount_silver < 0 )
    {
        send_to_char ( "Your group wouldn't like that.\n\r", ch );
        return;
    }

    if ( amount_platinum == 0 && amount_gold == 0 && amount_silver == 0 )
    {
        send_to_char ( "You hand out zero coins, but no one notices.\n\r",
                       ch );
        return;
    }

    if ( ( ch->silver + ( ch->gold * 100 ) + ( ch->platinum * 10000 ) ) <
         ( amount_silver + ( amount_gold * 100 ) +
           ( amount_platinum * 10000 ) ) )
    {
        send_to_char ( "You don't have that much to split.\n\r", ch );
        return;
    }

    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( is_same_group ( gch, ch ) && !IS_AFFECTED ( gch, AFF_CHARM ) )
            members++;
    }

    if ( members < 2 )
    {
        send_to_char ( "Just keep it all.\n\r", ch );
        return;
    }

    share_platinum = amount_platinum / members;
    extra_platinum = amount_platinum % members;
    amount_gold += ( extra_platinum * 100 );
    share_gold = amount_gold / members;
    extra_gold = amount_gold % members;
    amount_silver += ( extra_gold * 100 );
    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;
    if ( share_platinum == 0 && share_gold == 0 && share_silver == 0 )
    {
        send_to_char ( "Don't even bother, cheapskate.\n\r", ch );
        return;
    }

    deduct_cost ( ch, amount_platinum - extra_platinum, VALUE_PLATINUM );
    add_cost ( ch, share_platinum, VALUE_PLATINUM );
    deduct_cost ( ch, amount_gold - extra_gold, VALUE_GOLD );
    add_cost ( ch, share_gold, VALUE_GOLD );
    deduct_cost ( ch, amount_silver, VALUE_SILVER );
    add_cost ( ch, share_silver + extra_silver, VALUE_SILVER );
    if ( share_platinum > 0 )
    {
        sprintf ( buf,
                  "You split %d platinum coins. Your share is %d platinum.\n\r",
                  amount_platinum - extra_platinum, share_platinum );
        send_to_char ( buf, ch );
    }
    if ( share_gold > 0 )
    {
        sprintf ( buf, "You split %d gold coins. Your share is %d gold.\n\r",
                  amount_gold - extra_gold, share_gold );
        send_to_char ( buf, ch );
    }
    if ( share_silver > 0 )
    {
        sprintf ( buf,
                  "You split %d silver coins. Your share is %d silver.\n\r",
                  amount_silver, share_silver + extra_silver );
        send_to_char ( buf, ch );
    }

    if ( share_gold == 0 && share_silver == 0 )
    {
        sprintf ( buf,
                  "$n splits %d platinum coins. Your share is %d platinum.",
                  amount_platinum - extra_platinum, share_platinum );
    }
    else if ( share_platinum == 0 && share_silver == 0 )
    {
        sprintf ( buf, "$n splits %d gold coins. Your share is %d gold.",
                  amount_gold - extra_gold, share_gold );
    }
    else if ( share_platinum == 0 && share_gold == 0 )
    {
        sprintf ( buf, "$n splits %d silver coins. Your share is %d silver.",
                  amount_silver, share_silver );
    }
    else if ( share_silver == 0 )
    {
        sprintf ( buf,
                  "$n splits %d platinum and %d gold coins. giving you %d platinum and %d gold.\n\r",
                  amount_platinum - extra_platinum, amount_gold - extra_gold,
                  share_platinum, share_gold );
    }
    else if ( share_gold == 0 )
    {
        sprintf ( buf,
                  "$n splits %d platinum and %d silver coins. giving you %d platinum and %d silver.\n\r",
                  amount_platinum - extra_platinum, amount_silver,
                  share_platinum, share_silver );
    }
    else if ( share_platinum == 0 )
    {
        sprintf ( buf,
                  "$n splits %d gold and %d silver coins. giving you %d gold and %d silver.\n\r",
                  amount_gold - extra_gold, amount_silver, share_gold,
                  share_silver );
    }
    else
    {
        sprintf ( buf,
                  "$n splits %d platinum, %d gold and %d silver coins. giving you %d platinum, %d gold and %d silver.\n\r",
                  amount_platinum - extra_platinum, amount_gold - extra_gold,
                  amount_silver, share_platinum, share_gold, share_silver );
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( gch != ch && is_same_group ( gch, ch ) &&
             !IS_AFFECTED ( gch, AFF_CHARM ) )
        {
            act ( buf, ch, NULL, gch, TO_VICT );
            add_cost ( gch, share_platinum, VALUE_PLATINUM );
            add_cost ( gch, share_gold, VALUE_GOLD );
            add_cost ( gch, share_silver, VALUE_SILVER );
        }
    }

    return;
}

CH_CMD ( do_gtell )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    if ( argument[0] == '\0' )
    {
        send_to_char ( "Tell your group what?\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOTELL ) )
    {
        send_to_char ( "Your message didn't get through!\n\r", ch );
        return;
    }

    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf ( buf, "%s tells the group '{K%s{x'\n\r", ch->name, argument );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
        if ( is_same_group ( gch, ch ) )
            send_to_char ( buf, gch );
    }

    return;
}

/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group ( CHAR_DATA * ach, CHAR_DATA * bch )
{
    if ( ach == NULL || bch == NULL )
        return FALSE;
    if ( ach->leader != NULL )
        ach = ach->leader;
    if ( bch->leader != NULL )
        bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
CH_CMD ( do_colour )
{
    char arg[MAX_STRING_LENGTH];
    int ccolor;
    argument = one_argument ( argument, arg );
    if ( !*arg )
    {
        if ( IS_SET ( ch->act, PLR_NOCOLOUR ) )
        {
            REMOVE_BIT ( ch->act, PLR_NOCOLOUR );
            if ( ch->desc )
                ch->desc->ansi = TRUE;
            send_to_char
                ( "{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r", ch );
        }
        else
        {
            send_to_char ( "Colour is now OFF, <sigh>\n\r", ch );
            SET_BIT ( ch->act, PLR_NOCOLOUR );
            if ( ch->desc )
                ch->desc->ansi = FALSE;
        }
    }
    else if ( !str_prefix ( arg, "list" ) )
    {
        send_to_char ( "\n\rColors:\n\r", ch );
        send_to_char ( "     0 - Reset           9 - Bright Red\n\r", ch );
        send_to_char ( "     1 - Red            10 - Bright Green\n\r", ch );
        send_to_char ( "     2 - Green          11 - Yellow\n\r", ch );
        send_to_char ( "     3 - Brown          12 - Bright Blue\n\r", ch );
        send_to_char ( "     4 - Blue           13 - Bright Magenta\n\r",
                       ch );
        send_to_char ( "     5 - Magenta        14 - Bright Cyan\n\r", ch );
        send_to_char ( "     6 - Cyan           15 - Bright White\n\r", ch );
        send_to_char ( "     7 - White          16 - Black\n\r", ch );
        send_to_char ( "     8 - Grey           17 - None\n\r", ch );
/*        send_to_char ( "Channels:\n\r", ch );
        send_to_char ( "     ooc        cgossip    clan\n\r", ch );
        send_to_char ( "     gossip     grats      gtell\n\r", ch );
        send_to_char ( "     immtalk    music      ask\n\r", ch );
        send_to_char ( "     quote      say        shout\n\r", ch );
        send_to_char ( "     tell       wiznet     mobsay\n\r", ch );
        send_to_char ( "     room       condition  fight\n\r", ch );
        send_to_char ( "     opponent   witness    disarm\n\r", ch );
        send_to_char ( "     qgossip\n\r", ch ); */
        send_to_char ( "For a more detailed list, see HELP COLORS\n\r", ch );
        send_to_char
            ( "For a list of current settings, see HELP SETTINGS\n\r", ch );
    }
    else if ( !str_cmp ( arg, "0" ) )
    {
        ch->color = 0;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "1" ) )
    {
        ch->color = 1;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "2" ) )
    {
        ch->color = 2;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "3" ) )
    {
        ch->color = 3;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "4" ) )
    {
        ch->color = 4;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "5" ) )
    {
        ch->color = 5;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "6" ) )
    {
        ch->color = 6;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "7" ) )
    {
        ch->color = 7;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "8" ) )
    {
        ch->color = 8;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "9" ) )
    {
        ch->color = 9;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "10" ) )
    {
        ch->color = 10;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "11" ) )
    {
        ch->color = 11;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "12" ) )
    {
        ch->color = 12;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "13" ) )
    {
        ch->color = 13;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "14" ) )
    {
        ch->color = 14;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "15" ) )
    {
        ch->color = 15;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "16" ) )
    {
        ch->color = 16;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( !str_cmp ( arg, "17" ) )
    {
        ch->color = 0;
        send_to_char ( "{xOK\n\r", ch );
    }
    else if ( argument[0] == '\0' )
    {
        send_to_char ( "Syntax: color {{list|#|<channel> #}\n\r", ch );
    }
    else if ( !is_number ( argument ) )
    {
        send_to_char ( "Syntax: color {{list|#|<channel> #}\n\r", ch );
    }
    else
    {
        ccolor = atoi ( argument );
        if ( ccolor >= 18 )
        {
            send_to_char ( "Color number must be 0-17\n\r", ch );
            return;
        }
        if ( !str_prefix ( arg, "ooc" ) )
        {
            ch->color_auc = ccolor;
            send_to_char ( "ooc channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "cgossip" ) )
        {
            ch->color_cgo = ccolor;
            send_to_char ( "clan gossip channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "clan" ) )
        {
            ch->color_cla = ccolor;
            send_to_char ( "clan talk channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "gossip" ) )
        {
            ch->color_gos = ccolor;
            send_to_char ( "gossip channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "grats" ) )
        {
            ch->color_gra = ccolor;
            send_to_char ( "grats channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "gtell" ) )
        {
            ch->color_gte = ccolor;
            send_to_char ( "group tell channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "immtalk" ) )
        {
            ch->color_imm = ccolor;
            send_to_char ( "immortal talk channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "music" ) )
        {
            ch->color_mus = ccolor;
            send_to_char ( "music channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "ask" ) )
        {
            ch->color_que = ccolor;
            send_to_char ( "question/answer channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "quote" ) )
        {
            ch->color_quo = ccolor;
            send_to_char ( "quote channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "say" ) )
        {
            ch->color_say = ccolor;
            send_to_char ( "say channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "shout" ) )
        {
            ch->color_sho = ccolor;
            send_to_char ( "shout/yell channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "tell" ) )
        {
            ch->color_tel = ccolor;
            send_to_char ( "tell/reply channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "wiznet" ) )
        {
            ch->color_wiz = ccolor;
            send_to_char ( "wiznet channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "mobsay" ) )
        {
            ch->color_mob = ccolor;
            send_to_char ( "mobile talk channel set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "room" ) )
        {
            ch->color_roo = ccolor;
            send_to_char ( "room name display set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "condition" ) )
        {
            ch->color_con = ccolor;
            send_to_char ( "character condition display set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "fight" ) )
        {
            ch->color_fig = ccolor;
            send_to_char ( "your fight actions set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "opponent" ) )
        {
            ch->color_opp = ccolor;
            send_to_char ( "opponents fight actions set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "disarm" ) )
        {
            ch->color_dis = ccolor;
            send_to_char ( "disarm display set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "witness" ) )
        {
            ch->color_wit = ccolor;
            send_to_char ( "witness fight actions set.\n\r", ch );
        }
        else if ( !str_prefix ( arg, "qgossip" ) )
        {
            ch->color_qgo = ccolor;
            send_to_char ( "quest gossip channel set.\n\r", ch );
        }
        else
        {
            send_to_char ( "Syntax: color {{list|#|<channel> #}\n\r", ch );
        }

    }
    return;
}


CH_CMD ( do_gocial )
{
    char command[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    int counter;
    int count;
    char buf2[MAX_STRING_LENGTH];
    argument = one_argument ( argument, command );
    if ( command[0] == '\0' )
    {
        send_to_char ( "What do you wish to gocial?\n\r", ch );
        return;
    }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char
                ( "The gods have revoked your channel priviliges.\n\r", ch );
            return;
	}

        if ( IS_SET ( ch->comm, COMM_NOGSOCIAL ) )
        {
            send_to_char
                ( "You are not allowed to do global socials.\n\r", ch );
            return;
	}
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
	{
	   send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"                     "restriction on all channels. If you have any questions, use the\n\r"
	                  "pray command and hopefully you'll get an answer. For basic help\n\r"
			  "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
			  "you've reached level 20, you'll be able to use all channels.\n\r", ch );
	   return;
	}
*/	
    found = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == social_table[cmd].name[0] &&
             !str_prefix ( command, social_table[cmd].name ) )
        {
            found = TRUE;
            break;
        }
    }

    if ( !found )
    {
        send_to_char ( "What kind of social is that?!?!\n\r", ch );
        return;
    }

    if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_QUIET ) )
    {
        send_to_char ( "You must turn off quiet mode first.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOGOSSIP ) )
    {
        send_to_char ( "But you have the gossip channel turned off!\n\r",
                       ch );
        return;
    }

    if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOCHANNELS ) )
    {
        send_to_char ( "The gods have revoked your channel priviliges.\n\r",
                       ch );
        return;
    }

    switch ( ch->position )
    {
        case POS_DEAD:
            send_to_char ( "Lie still; you are DEAD!\n\r", ch );
            return;
        case POS_INCAP:
        case POS_MORTAL:
            send_to_char ( "You are hurt far too bad for that.\n\r", ch );
            return;
        case POS_STUNNED:
            send_to_char ( "You are too stunned for that.\n\r", ch );
            return;
    }

    socials_today++;
    
    one_argument ( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].char_no_arg );
        act_new ( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].others_no_arg );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *vch;
            vch = d->original ? d->original : d->character;
            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( vch->comm, COMM_NOGOSSIP ) &&
                 !IS_SET ( vch->comm, COMM_QUIET ) )
            {
                act_new ( buf, ch, NULL, vch, TO_VICT, POS_DEAD );
            }
        }
    }
    else if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    else if ( victim == ch )
    {
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].char_auto );
        act_new ( buf, ch, NULL, NULL, TO_CHAR, POS_DEAD );
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].others_auto );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *vch;
            vch = d->original ? d->original : d->character;
            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( vch->comm, COMM_NOGOSSIP ) &&
                 !IS_SET ( vch->comm, COMM_QUIET ) )
            {
                act_new ( buf, ch, NULL, vch, TO_VICT, POS_DEAD );
            }
        }
    }
    else
    {
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].char_found );
        act_new ( buf, ch, NULL, victim, TO_CHAR, POS_DEAD );
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].vict_found );
        act_new ( buf, ch, NULL, victim, TO_VICT, POS_DEAD );
        sprintf ( buf, "{Y({GG{global {MS{mocial{Y) {x'%s'",
                  social_table[cmd].others_found );
        for ( counter = 0; buf[counter + 1] != '\0'; counter++ )
        {
            if ( buf[counter] == '$' && buf[counter + 1] == 'N' )
            {
                strcpy ( buf2, buf );
                buf2[counter] = '\0';
		if ( IS_NPC ( victim ) )
                strcat ( buf2, victim->short_descr );
		else
		strcat ( buf2, victim->pcdata->cname );
                for ( count = 0; buf[count] != '\0'; count++ )
                {
                    buf[count] = buf[count + counter + 2];
                }
                strcat ( buf2, buf );
                strcpy ( buf, buf2 );
            }
            else if ( buf[counter] == '$' && buf[counter + 1] == 'E' )
            {
                switch ( victim->sex )
                {
                    default:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "it" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 1:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "it" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 2:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "it" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                }
            }
            else if ( buf[counter] == '$' && buf[counter + 1] == 'M' )
            {
                buf[counter] = '%';
                buf[counter + 1] = 's';
                switch ( victim->sex )
                {
                    default:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "it" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 1:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "him" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 2:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "her" );
			for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                }
            }
            else if ( buf[counter] == '$' && buf[counter + 1] == 'S' )
            {
                switch ( victim->sex )
                {
                    default:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "its" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 1:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "his" );
                        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                    case 2:
                        strcpy ( buf2, buf );
                        buf2[counter] = '\0';
                        strcat ( buf2, "her" );
		        for ( count = 0; buf[count] != '\0'; count++ )
                        {
                            buf[count] = buf[count + counter + 2];
                        }
                        strcat ( buf2, buf );
                        strcpy ( buf, buf2 );
                        break;
                }
            }

        }
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *vch;
            vch = d->original ? d->original : d->character;
            if ( d->connected == CON_PLAYING && d->character != ch &&
                 d->character != victim &&
                 !IS_SET ( vch->comm, COMM_NOGOSSIP ) &&
                 !IS_SET ( vch->comm, COMM_QUIET ) )
            {
                act_new ( buf, ch, NULL, vch, TO_VICT, POS_DEAD );
            }
        }
    }
    return;
}

CH_CMD ( do_backup )
{
    if ( !IS_VALID ( ch ) )
    {
        send_to_char ( "Your character is not valid! Please consult a level 216 immediately!\n\r", ch );
        bug ( "char_update: trying to work with invalid character", 0 );
        return;
    }
    if ( ch->level < 35 && IS_SET ( class_table[ch->class].tier, TIER_01 ) )
    {
        send_to_char ( "Your pfile will be backed up from level 35.\n\r", ch );
        return;
    }
    else
    {
        backup_char_obj ( ch );
	save_char_obj ( ch );
        send_to_char ( "Your P-File has been backed up.{x\n\r", ch );
        WAIT_STATE(ch,3 * PULSE_VIOLENCE); 
        return;
    }
}

CH_CMD ( do_gmote )
{
   if(IS_SET(ch->comm, COMM_QUIET))
   {
         send_to_char("Turn on the social channel first.\n\r",ch);
         return;
   }
   if(IS_SET(ch->comm, COMM_QUIET))
   {
         send_to_char("Turn off quiet first.\n\r",ch);
         return;
   }
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
         send_to_char("You can't show your emotions.\n\r",ch );
         return;
    }

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
       send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
       return;
    }
/*
        if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) && ch->level < 20 )
        {
           send_to_char ( "Sorry, due to abusive players, we've been forced to put a level\n\r"
                          "restriction on all channels. If you have any questions, use the\n\r"
                          "pray command and hopefully you'll get an answer. For basic help\n\r"
                          "try 'help' and 'help summary' or send an immortal a tell. When\n\r"
                          "you've reached level 20, you'll be able to use all channels.\n\r", ch );
           return;
        }
*/

    if ( argument[0] == '\0' )
    {
         send_to_char("Gmote what?\n\r", ch );
         return;
    }

/*        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
	  argument = makedrunk ( ch, argument ); */

/*    if ( ch->pcdata->condition[COND_DRUNK] > 0 )
       { argument = makedrunk (ch, argument); } */
    social_channel( "{B$n $T{x", ch, argument, TO_ROOM );
    social_channel( "{B$n $T{x", ch, argument, TO_CHAR );
    return;

}
