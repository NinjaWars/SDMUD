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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "sd.h"
//#include "music.h"
#include "lookup.h"

/* used for saving */
int save_number = 0;

/*
 * Advancement stuff.
 */
void advance_level ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    int loc, i;
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    OBJ_DATA *obj;

    ch->pcdata->last_level =
        ( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600;

    sprintf ( buf, "the %s",
              title_table[ch->class][ch->level][ch->sex ==
                                                SEX_FEMALE ? 1 : 0] );
    /* set_title( ch, buf ); */

    add_hp =
        con_app[get_curr_stat ( ch, STAT_CON )].hitp +
        number_range ( class_table[ch->class].hp_min,
                       class_table[ch->class].hp_max );
    add_mana =
        number_range ( 4,
                       ( 4 * get_curr_stat ( ch, STAT_INT ) +
                         get_curr_stat ( ch, STAT_WIS ) ) );
    if ( !class_table[ch->class].fMana )
        add_mana /= 2;
    add_move =
        number_range ( 1,
                       ( get_curr_stat ( ch, STAT_CON ) +
                         get_curr_stat ( ch, STAT_DEX ) ) / 5 );
    add_prac = wis_app[get_curr_stat ( ch, STAT_WIS )].practice;

    add_hp = add_hp * 9 / 10;
    add_hp = add_hp * 7 / 4;
    add_mana = add_mana * 9 / 10;
    add_mana = add_mana * 7 / 4;
    add_move = add_move * 9 / 10;
    add_move = add_move * 2;

    add_hp = UMAX ( 4, add_hp );
    add_mana = UMAX ( 4, add_mana );
    add_move = UMAX ( 12, add_move );

    ch->max_hit += add_hp;
    ch->max_mana += add_mana;
    ch->max_move += add_move;
    ch->practice += add_prac;
    ch->train += number_range ( 1, 2 );

    ch->pcdata->perm_hit += add_hp;
    ch->pcdata->perm_mana += add_mana;
    ch->pcdata->perm_move += add_move;

    sprintf ( buf,
              "\nYour gain is: {r%d{W/{R%ld{Bhp{x {m%d{W/{M%ld{Bm{x {y%d{W/{Y%ld{Bmv {c%d{M/{C%d{Bprac{x.\n\r",
              add_hp, ch->max_hit, add_mana, ch->max_mana, add_move,
              ch->max_move, add_prac, ch->practice );
    send_to_char ( buf, ch );
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
            update_questobjs ( ch, obj );
    for ( loc = 0; loc < MAX_WEAR; loc++ )
    {
        obj = get_eq_char ( ch, loc );
        if ( obj == NULL )
            continue;
        if ( obj->clan )
        {
            for ( i = 0; i < 4; i++ )
            {
                ch->armor[i] += apply_ac ( ch->level - 1, obj, loc, i );
                ch->armor[i] -= apply_ac ( ch->level, obj, loc, i );
            }
        }
    }
    return;
}

void advance_level_quiet ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    int loc, i;
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    OBJ_DATA *obj;

    ch->pcdata->last_level =
        ( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600;

    sprintf ( buf, "the %s",
              title_table[ch->class][ch->level][ch->sex ==
                                                SEX_FEMALE ? 1 : 0] );
    /* set_title( ch, buf ); */

    add_hp =
        con_app[get_curr_stat ( ch, STAT_CON )].hitp +
        number_range ( class_table[ch->class].hp_min,
                       class_table[ch->class].hp_max );
    add_mana =
        number_range ( 4,
                       ( 4 * get_curr_stat ( ch, STAT_INT ) +
                         get_curr_stat ( ch, STAT_WIS ) ) / 2 );
    if ( !class_table[ch->class].fMana )
        add_mana /= 2;
    add_move =
        number_range ( 1,
                       ( get_curr_stat ( ch, STAT_CON ) +
                         get_curr_stat ( ch, STAT_DEX ) ) / 5 );
    add_prac = wis_app[get_curr_stat ( ch, STAT_WIS )].practice;

    add_hp = add_hp * 9 / 10;
    add_hp = add_hp * 7 / 4;
    add_mana = add_mana * 9 / 10;
    add_mana = add_mana * 7 / 4;
    add_move = add_move * 9 / 10;
    add_move = add_move * 2;

    add_hp = UMAX ( 4, add_hp );
    add_mana = UMAX ( 4, add_mana );
    add_move = UMAX ( 12, add_move );

    ch->max_hit += add_hp;
    ch->max_mana += add_mana;
    ch->max_move += add_move;
    ch->practice += add_prac;
    ch->train += 1;

    ch->pcdata->perm_hit += add_hp;
    ch->pcdata->perm_mana += add_mana;
    ch->pcdata->perm_move += add_move;

    for ( loc = 0; loc < MAX_WEAR; loc++ )
    {
        obj = get_eq_char ( ch, loc );
        if ( obj == NULL )
            continue;
        if ( obj->clan )
        {
            for ( i = 0; i < 4; i++ )
            {
                ch->armor[i] += apply_ac ( ch->level - 1, obj, loc, i );
                ch->armor[i] -= apply_ac ( ch->level, obj, loc, i );
            }
        }
    }
    return;
}

void gain_exp ( CHAR_DATA * ch, int gain )
{
    char buf[MAX_STRING_LENGTH];
/*    DESCRIPTOR_DATA *d; */
    if ( IS_NPC ( ch ) || ch->level >= LEVEL_HERO )
        return;

    ch->exp =
        UMAX ( exp_per_level ( ch, ch->pcdata->points ), ch->exp + gain );
    while ( ch->level < LEVEL_HERO &&
            ch->exp >= exp_per_level ( ch,
                                       ch->pcdata->points ) * ( ch->level +
                                                                1 ) )
    {
        send_to_char ( "{YYou raise a level{R!!{x  ", ch );
        ch->level += 1;
	if ( ch->level == LEVEL_HERO )
	{
	   do_announce ( ch, "has reached {RH{rER{RO{Y!{B!{C!{x" );
	   backup_char_obj ( ch );
	   save_char_obj ( ch );
	}
        sprintf ( buf, "{R%s {Bhas {Rattained {Blevel{Y %d{G!{g!{G!{g!{x",
                  ch->name, ch->level );
        wiznet ( buf, ch, NULL, WIZ_LEVELS, 0, 0 );
        advance_level ( ch );
        affect_strip ( ch, gsn_plague );
        affect_strip ( ch, gsn_poison );
        affect_strip ( ch, gsn_blindness );
        affect_strip ( ch, gsn_sleep );
        affect_strip ( ch, gsn_curse );

        ch->hit += ( ch->max_hit / 10 );
	if ( ch->hit > ch->max_hit )
	{
	ch->hit = ch->max_hit;
	}
        ch->mana = ch->max_mana;
        ch->move = ch->max_move;
        update_pos ( ch );
        send_to_char
            ( "{WYour body and soul have been healed, you have been given the {Rpower{W to continue.{x\n\r",
              ch );
/*        if ( ch->level > 49 && !is_clan ( ch ) )
        {
            send_to_char
                ( "{YYou have now been made a member of {RFr{re{Wel{ran{Rce{Y a PK loner clan, you "
                  "may still join any clan you wish.{x\n\r", ch );
            ch->clan = clan_lookup ( "loner" );
        } */

	 if ( ch->level == 20 && !is_clan ( ch ) )
	 {
	    send_to_char
		( "{YYou can now join a clan.{x\n\r{YIf you wish to join {RLoner{Y, the independent PK clan, type 'Loner'.{x\n\r{YYou can also join the Pknewbies clan which is a clan for people new to PK. For more info, type 'pknewbies'.{x\n\r", ch );
	 } 
/*
for ( d = descriptor_list; d != NULL; d = d->next )
{
         if(d->connected == CON_PLAYING 
             && !IS_SET(d->character->comm, COMM_QUIET)
	     && !IS_SET(d->character->comm, COMM_NOCHEER))
           printf_to_char(d->character, "{r!{R*{r!{w-{W>{YC{GH{YE{GE{YR{W<{w-{r!{R*{r! {*%s {Dhas attained level {*%d{Y!{G!{Y!{x\n\r",ch->name,ch->level);
}
*/

    }

    return;
}

/*
 * Regeneration stuff.
 */
int hit_gain ( CHAR_DATA * ch )
{
    int gain;
    int number;
    if ( ch->in_room == NULL )
        return 0;
    if ( IS_NPC ( ch ) )
    {
        gain = 5 + ch->level;
        if ( IS_AFFECTED ( ch, AFF_REGENERATION ) )
            gain *= 2;
        switch ( ch->position )
        {
            default:
                gain /= 2;
                break;
            case POS_SLEEPING:
                gain = 3 * gain / 2;
                break;
            case POS_RESTING:
                break;
            case POS_FIGHTING:
                gain /= 3;
                break;
        }

    }
    else
    {
        gain = UMAX ( 3, get_curr_stat ( ch, STAT_CON ) - 3 + ch->level / 2 );
        gain += class_table[ch->class].hp_max - 10;
        number = number_percent (  );
        if ( number < get_skill ( ch, gsn_fast_healing ) )
        {
            gain += number * gain / 100;
            if ( ch->hit < ch->max_hit )
                check_improve ( ch, gsn_fast_healing, TRUE, 8 );
        }

        switch ( ch->position )
        {
            default:
                gain /= 4;
                break;
            case POS_SLEEPING:
                break;
            case POS_RESTING:
                gain /= 2;
                break;
            case POS_FIGHTING:
                gain /= 6;
                break;
        }

        if ( ch->pcdata->condition[COND_HUNGER] == 0 )
            gain /= 2;
        if ( ch->pcdata->condition[COND_THIRST] == 0 )
            gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate / 90;
    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
        gain = gain * ch->on->value[3] / 90;
    if ( IS_AFFECTED ( ch, AFF_POISON ) )
        gain /= 4;
    if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
        gain /= 8;
    if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
        gain /= 2;
    return UMIN ( gain, ch->max_hit - ch->hit );
}

int mana_gain ( CHAR_DATA * ch )
{
    int gain;
    int number;
    if ( ch->in_room == NULL )
        return 0;
    if ( IS_NPC ( ch ) )
    {
        gain = 5 + ch->level;
        switch ( ch->position )
        {
            default:
                gain /= 2;
                break;
            case POS_SLEEPING:
                gain = 3 * gain / 2;
                break;
            case POS_RESTING:
                break;
            case POS_FIGHTING:
                gain /= 3;
                break;
        }
    }
    else
    {
        gain =
            ( ( get_curr_stat ( ch, STAT_WIS ) +
                get_curr_stat ( ch, STAT_INT ) + ch->level ) / 3 ) * 2;
        number = number_percent (  );
        if ( number < get_skill ( ch, gsn_meditation ) )
        {
            gain += number * gain / 100;
            if ( ch->mana < ch->max_mana )
                check_improve ( ch, gsn_meditation, TRUE, 8 );
        }
        if ( !class_table[ch->class].fMana )
            gain /= 2;
        switch ( ch->position )
        {
            default:
                gain /= 4;
                break;
            case POS_SLEEPING:
                break;
            case POS_RESTING:
                gain /= 2;
                break;
            case POS_FIGHTING:
                gain /= 6;
                break;
        }

        if ( ch->pcdata->condition[COND_HUNGER] == 0 )
            gain /= 2;
        if ( ch->pcdata->condition[COND_THIRST] == 0 )
            gain /= 2;
    }

    gain = gain * ch->in_room->mana_rate / 90;
    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
        gain = gain * ch->on->value[4] / 90;
    if ( IS_AFFECTED ( ch, AFF_POISON ) )
        gain /= 4;
    if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
        gain /= 8;
    if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
        gain /= 2;
    return UMIN ( gain, ch->max_mana - ch->mana );
}

int move_gain ( CHAR_DATA * ch )
{
    int gain;
    if ( ch->in_room == NULL )
        return 0;
    if ( IS_NPC ( ch ) )
    {
        gain = ch->level;
    }
    else
    {
        gain = UMAX ( 15, ch->level );
        switch ( ch->position )
        {
            case POS_SLEEPING:
                gain += get_curr_stat ( ch, STAT_DEX );
                break;
            case POS_RESTING:
                gain += get_curr_stat ( ch, STAT_DEX ) / 2;
                break;
        }

        if ( ch->pcdata->condition[COND_HUNGER] == 0 )
            gain /= 2;
        if ( ch->pcdata->condition[COND_THIRST] == 0 )
            gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate / 90;
    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
        gain = gain * ch->on->value[3] / 90;
    if ( IS_AFFECTED ( ch, AFF_POISON ) )
        gain /= 4;
    if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
        gain /= 8;
    if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
        gain /= 2;
    return UMIN ( gain, ch->max_move - ch->move );
}

void gain_condition ( CHAR_DATA * ch, int iCond, int value )
{
    int condition;
    if ( value == 0 || IS_NPC ( ch ) || ch->level >= LEVEL_IMMORTAL )
        return;
    condition = ch->pcdata->condition[iCond];
    if ( condition == -1 )
        return;
    ch->pcdata->condition[iCond] = URANGE ( 0, condition + value, 48 );
    if ( ch->pcdata->condition[iCond] == 0 )
    {
        switch ( iCond )
        {
            case COND_HUNGER:
		if ( !IS_SET ( ch->comm, COMM_AFK ) )
                send_to_char ( "You are hungry.\n\r", ch );
                break;
            case COND_THIRST:
		if ( !IS_SET ( ch->comm, COMM_AFK ) )
                send_to_char ( "You are thirsty.\n\r", ch );
                break;
            case COND_DRUNK:
                if ( condition != 0 )
		if ( !IS_SET ( ch->comm, COMM_AFK ) )
                    send_to_char ( "You are sober.\n\r", ch );
                break;
        }
    }

    return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
WOLLA ( mobile_update )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;
    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
        if ( !IS_NPC ( ch ) || ch->in_room == NULL ||
             IS_AFFECTED ( ch, AFF_CHARM ) )
            continue;
        if ( ch->in_room->area->empty &&
             !IS_SET ( ch->act, ACT_UPDATE_ALWAYS ) )
            continue;
        /* Examine call for special procedure */
        if ( ch->spec_fun != 0 )
        {
            if ( ( *ch->spec_fun ) ( ch ) )
                continue;
        }

        if ( ch->pIndexData->pShop != NULL )    /* give him some platinum */
            if ( ( ch->platinum * 100 + ch->gold ) < ch->pIndexData->wealth )
            {
                ch->platinum +=
                    ch->pIndexData->wealth * number_range ( 1, 20 ) / 5000000;
                ch->gold +=
                    ch->pIndexData->wealth * number_range ( 1, 20 ) / 50000;
            }

        /* That's all for sleeping / busy monster, and empty zones */
        if ( ch->position != POS_STANDING )
            continue;
        /* Scavenge */
        if ( IS_SET ( ch->act, ACT_SCAVENGER ) &&
             ch->in_room->contents != NULL && number_bits ( 6 ) == 0 )
        {
            OBJ_DATA *obj;
            OBJ_DATA *obj_best;
            int max;
            max = 1;
            obj_best = 0;
            for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
            {
                if ( CAN_WEAR ( obj, ITEM_TAKE ) && can_loot ( ch, obj ) &&
                     obj->cost > max && obj->cost > 0 )
                {
                    obj_best = obj;
                    max = obj->cost;
                }
            }

            if ( obj_best )
            {
                obj_from_room ( obj_best );
                obj_to_char ( obj_best, ch );
                act ( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
            }
        }

        /* Wander */
        if ( !IS_SET ( ch->act, ACT_SENTINEL ) && number_bits ( 3 ) == 0 &&
             ( door = number_bits ( 5 ) ) <= 5 &&
             ( pexit = ch->in_room->exit[door] ) != NULL &&
             pexit->u1.to_room != NULL &&
             !IS_SET ( pexit->exit_info, EX_CLOSED ) &&
             !IS_SET ( pexit->u1.to_room->room_flags, ROOM_NO_MOB ) &&
             ( !IS_SET ( ch->act, ACT_STAY_AREA ) ||
               pexit->u1.to_room->area == ch->in_room->area ) &&
             ( !IS_SET ( ch->act, ACT_OUTDOORS ) ||
               !IS_SET ( pexit->u1.to_room->room_flags, ROOM_INDOORS ) ) &&
             ( !IS_SET ( ch->act, ACT_INDOORS ) ||
               IS_SET ( pexit->u1.to_room->room_flags, ROOM_INDOORS ) ) )
        {
            move_char ( ch, door, FALSE, FALSE );
        }
	
	if ( IS_NPC ( ch ) && ch->isquestmob > 0 && ++ch->isquestmob >= 450 )
   		ch->isquestmob = 0;	

    }

    return;
}

/*
 * Update the weather.
 */
WOLLA ( weather_update )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;
    buf[0] = '\0';
    switch ( ++time_info.hour )
    {
        case 5:
            weather_info.sunlight = SUN_LIGHT;
            strcat ( buf, "The {Yd{ya{Yy {xhas {Cb{ce{Cg{cu{Cn{x.\n\r" );
            break;
        case 6:
            weather_info.sunlight = SUN_RISE;
            strcat ( buf, "The {Ysun {xrises in the {Ge{gast{x.\n\r" );
            break;
        case 19:
            weather_info.sunlight = SUN_SET;
            strcat ( buf, "The {Ysun{x slowly {Dd{disappear{Ds{x in the {Cw{cest{x.\n\r" );
            break;
        case 20:
            weather_info.sunlight = SUN_DARK;
            strcat ( buf, "The {Dnight{x has begun.\n\r" );
            break;
        case 24:
            time_info.hour = 0;
            time_info.day++;
            break;
    }

    if ( time_info.day >= 35 )
    {
        time_info.day = 0;
        time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
        time_info.month = 0;
        time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
        diff = weather_info.mmhg > 985 ? -2 : 2;
    else
        diff = weather_info.mmhg > 1015 ? -2 : 2;
    weather_info.change +=
        diff * dice ( 1, 4 ) + dice ( 2, 6 ) - dice ( 2, 6 );
    weather_info.change = UMAX ( weather_info.change, -12 );
    weather_info.change = UMIN ( weather_info.change, 12 );
    weather_info.mmhg += weather_info.change;
    weather_info.mmhg = UMAX ( weather_info.mmhg, 960 );
    weather_info.mmhg = UMIN ( weather_info.mmhg, 1040 );
    switch ( weather_info.sky )
    {
        default:
            bug ( "Weather_update: bad sky %d.", weather_info.sky );
            weather_info.sky = SKY_CLOUDLESS;
            break;
        case SKY_CLOUDLESS:
            if ( weather_info.mmhg < 990 ||
                 ( weather_info.mmhg < 1010 && number_bits ( 2 ) == 0 ) )
            {
                strcat ( buf, "The {Wsky{x is getting {Wc{wl{Wo{wu{Wd{wy{x.\n\r" );
                weather_info.sky = SKY_CLOUDY;
            }
            break;
        case SKY_CLOUDY:
            if ( weather_info.mmhg < 970 ||
                 ( weather_info.mmhg < 990 && number_bits ( 2 ) == 0 ) )
            {
                strcat ( buf, "It starts to {Br{ba{Bi{bn{x.\n\r" );
                weather_info.sky = SKY_RAINING;
            }

            if ( weather_info.mmhg > 1030 && number_bits ( 2 ) == 0 )
            {
                strcat ( buf, "The {Wc{wl{Wo{wu{Wd{ws{x disappear.\n\r" );
                weather_info.sky = SKY_CLOUDLESS;
            }
            break;
        case SKY_RAINING:
            if ( weather_info.mmhg < 970 && number_bits ( 2 ) == 0 )
            {
                strcat ( buf, "{YL{Di{Yg{Dh{Yt{Dn{Yi{Dn{Yg{x{z flashes{x in the {Ws{wk{Wy{x.\n\r" );
                weather_info.sky = SKY_LIGHTNING;
            }

            if ( weather_info.mmhg > 1030 ||
                 ( weather_info.mmhg > 1010 && number_bits ( 2 ) == 0 ) )
            {
                strcat ( buf, "The {Br{ba{Bi{bn{x stopped.\n\r" );
                weather_info.sky = SKY_CLOUDY;
            }
            break;
        case SKY_LIGHTNING:
            if ( weather_info.mmhg > 1010 ||
                 ( weather_info.mmhg > 990 && number_bits ( 2 ) == 0 ) )
            {
                strcat ( buf, "The {Yl{Di{Yg{Dh{Yt{Dn{Yi{Dn{Yg{x has stopped.\n\r" );
                weather_info.sky = SKY_RAINING;
                break;
            }
            break;
    }

    if ( buf[0] != '\0' )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING && IS_OUTSIDE ( d->character ) &&
                 IS_AWAKE ( d->character ) )
                send_to_char ( buf, d->character );
        }
    }
/*
        if ( time_info.hour < 9 || time_info.hour > 17 )
           {
	    int damage;
            if (IS_SET(class_table[d->character->class].name, "Revenant"))
            || (IS_SET(class_table[d->character->class].name, "Vampire"))
            || (IS_SET(class_table[d->character->class].name, "Lich"))
            || (IS_SET(class_table[d->character->class].name, "Cainite"))
            {
            printf_to_char(d->character,"The damaging rays of the sun {Rb{Yu{Rr{Yn{x your skin!{X\n\r");
            damage = ( d->character->level * 3 );
	    damage( d->character, victim, dam, gsn_knee,DAM_BASH,TRUE);
            }
          }
*/
    return;
}

/*
 * Update the bank system
 * (C) 1996 The Maniac from Mythran Mud
 *
 * This updates the shares value
 */
WOLLA ( bank_update )
{
    FILE *fp=NULL;
    if ( time_info.hour < 9 || time_info.hour > 17 )
        return;
    share_value += number_range ( 1, 30 );
    if ( share_value > 500 )
        share_value -= number_range ( 15, 150 );       /* Price stabilization */
    else
        share_value -= number_range ( 1, 15 );
    if ( ( fp = file_open ( BANK_FILE, "w" ) ) == NULL )
    {
        bug ( "bank_update:  fopen of BANK_FILE failed", 0 );
        new_fclose ( fp );
        return;
    }
    fprintf ( fp, "SHARE_VALUE %d\n", share_value );
    new_fclose ( fp );
}

/*
 * Update all chars, including mobs.
*/
WOLLA ( char_update )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;
    ch_quit = NULL;
    /* update save counter */
    save_number++;
    if ( save_number > 29 )
        save_number = 0;

/*   if ( arena != FIGHT_OPEN )
        arenacount++; */

/*	    
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
        if ( ch->timer > 30 )
            ch_quit = ch; 
        if ( IS_NPC ( ch ) && ch->hastimer )
        {
            if ( ++ch->timer > 5 )
            {
                act ( "$n decays into dust.", ch, NULL, NULL, TO_ROOM );
                extract_char ( ch, TRUE );
            }
        }
*/
	
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        AFFECT_DATA *paf;
        AFFECT_DATA *paf_next;

        ch_next = ch->next;

        if ( IS_NPC ( ch ) && ch->hastimer )
        {
            if ( ++ch->timer > 5 )
            {
                act ( "$n decays into dust.", ch, NULL, NULL, TO_ROOM );
                extract_char ( ch, TRUE );
            }
        }
		
        if ( !IS_NPC ( ch ) )
        {
	
	if ( ch->stunned && ch->position != POS_FIGHTING && ch->position != POS_STUNNED )
	ch->stunned = 0;

        if ( ch->position >= POS_STUNNED )
        {
            /* check to see if we need to go home */
            if ( IS_NPC ( ch ) && ch->zone != NULL &&
                 ch->zone != ch->in_room->area && ch->desc == NULL &&
                 ch->fighting == NULL && !IS_AFFECTED ( ch, AFF_CHARM ) &&
                 number_percent (  ) < 5 )
            {
                act ( "$n wanders on home.", ch, NULL, NULL, TO_ROOM );
                extract_char ( ch, TRUE );
                continue;
            }

            if ( ch->hit < ch->max_hit )
                ch->hit += hit_gain ( ch );
            else
                ch->hit = ch->max_hit;
            if ( ch->mana < ch->max_mana )
                ch->mana += mana_gain ( ch );
            else
                ch->mana = ch->max_mana;
            if ( ch->move < ch->max_move )
                ch->move += move_gain ( ch );
            else
                ch->move = ch->max_move;
        }

        if ( ch->position == POS_STUNNED )
            update_pos ( ch );
	    
	    	    
        if ( !IS_NPC ( ch ) && ch->level < LEVEL_IMMORTAL )
        {
            OBJ_DATA *obj;
            if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL &&
                 obj->item_type == ITEM_LIGHT && obj->value[2] > 0 )
            {
                if ( --obj->value[2] == 0 && ch->in_room != NULL )
                {
                    --ch->in_room->light;
                    act ( "$p goes out.", ch, obj, NULL, TO_ROOM );
                    act ( "$p flickers and goes out.", ch, obj, NULL,
                          TO_CHAR );
                    extract_obj ( obj );
                }
                else if ( obj->value[2] <= 5 && ch->in_room != NULL )
                    act ( "$p flickers.", ch, obj, NULL, TO_CHAR );
            }

            if ( !IS_NPC ( ch ) && IS_IMMORTAL ( ch ) && ( ch->desc != NULL ) )
               ch->timer = 0;

            if ( !IS_NPC ( ch ) && ch->position != POS_FIGHTING && ++ch->timer >= 10 && !IS_SET ( ch->comm, COMM_AFK ) )
		SET_BIT ( ch->comm, COMM_AFK );
/*
            if ( !IS_NPC ( ch ) && !IS_IMMORTAL ( ch ) && ++ch->timer >= 60 && !IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
            {
              if ( ch->was_in_room == NULL && ch->in_room != NULL )
              {
                           ch->was_in_room = ch->in_room;
                           if ( ch->fighting != NULL )
                             stop_fighting ( ch, TRUE );
                           act ( "$n disappears into the AFK void.", ch, NULL, NULL, TO_ROOM );
			   if ( !IS_SET ( ch->comm, COMM_AFK ) )
                           SET_BIT ( ch->comm, COMM_AFK );
                           send_to_char ( "You disappear into the AFK void.\n\r", ch );
                           char_from_room ( ch );
                           char_to_room ( ch, get_room_index ( ROOM_VNUM_LIMBO ) );
			   if ( IS_QUESTOR ( ch ) )
			   {
			      send_to_char ( "Your quest has been cancelled because you are AFK.\n\r", ch );
			      end_quest ( ch, 10 );
			   }
              }
*/
/*	      if ( !IS_NPC ( ch ) && ch->timer >= 30 && ch->position != POS_FIGHTING && !IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
	      {
	       if ( ch->was_in_room == NULL && ch->in_room != NULL )
	       {
                 ch->was_in_room = ch->in_room;

		 send_to_char ( "Exceeded inactivity limit... Logging off..\n\r", ch );
		 if ( IS_SET ( ch->comm, COMM_AFK ) ) REMOVE_BIT ( ch->comm, COMM_AFK );
		 do_replay ( ch, "" );
		 end_quest ( ch, 10 );
		 do_quit ( ch, "" );
	       }
	      }
*/	      
	      if ( !IS_NPC ( ch ) && ch->timer >= 10 && !ch->desc )
	      {
	       if ( ch->was_in_room == NULL && ch->in_room != NULL )
	       {
                 ch->was_in_room = ch->in_room;
		 if ( IS_SET ( ch->comm, COMM_AFK ) ) REMOVE_BIT ( ch->comm, COMM_AFK );
		 end_quest ( ch, 10 );
		 do_quit ( ch, "" );
	       }
	      }
	      
            gain_condition ( ch, COND_DRUNK, -1 );
            gain_condition ( ch, COND_FULL,
                             ch->size > SIZE_MEDIUM ? -4 : -2 );
            gain_condition ( ch, COND_THIRST, -1 );
            gain_condition ( ch, COND_HUNGER,
                             ch->size > SIZE_MEDIUM ? -2 : -1 );
        }

        for ( paf = ch->affected; paf != NULL; paf = paf_next )
        {
            paf_next = paf->next;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if ( number_range ( 0, 4 ) == 0 && paf->level > 0 )
                    paf->level--;   /* spell strength fades with time */
            }
            else if ( paf->duration < 0 );
            else
            {
                if ( paf_next == NULL || paf_next->type != paf->type ||
                     paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[paf->type].msg_off )
                    {
                        send_to_char ( skill_table[paf->type].msg_off, ch );
                        send_to_char ( "\n\r", ch );
                    }
                }
                affect_remove ( ch, paf );
            }
        }

        /*
         * Careful with the damages here,
         *   MUST NOT refer to ch after damage taken,
         *   as it may be lethal damage (on NPC).
         */

        if ( is_affected ( ch, gsn_plague ) && ch != NULL )
        {
            AFFECT_DATA *af, plague;
            CHAR_DATA *vch;
            int dam;
            if ( ch->in_room == NULL )
                return;
            act ( "$n writhes in agony as plague sores erupt from $s skin.",
                  ch, NULL, NULL, TO_ROOM );
            send_to_char ( "You writhe in agony from the plague.\n\r", ch );
            for ( af = ch->affected; af != NULL; af = af->next )
            {
                if ( af->type == gsn_plague )
                    break;
            }

            if ( af == NULL )
            {
                REMOVE_BIT ( ch->affected_by, AFF_PLAGUE );
                return;
            }

            if ( af->level == 1 )
                return;
            plague.where = TO_AFFECTS;
            plague.type = gsn_plague;
            plague.level = af->level - 1;
            plague.duration = number_range ( 1, 2 * plague.level );
            plague.location = APPLY_STR;
            plague.modifier = -5;
            plague.bitvector = AFF_PLAGUE;
            for ( vch = ch->in_room->people; vch != NULL;
                  vch = vch->next_in_room )
            {
                if ( !saves_spell ( plague.level - 2, vch, DAM_DISEASE ) &&
                     !IS_IMMORTAL ( vch ) && !IS_AFFECTED ( vch, AFF_PLAGUE )
                     && number_bits ( 4 ) == 0 )
                {
                    send_to_char ( "You feel hot and feverish.\n\r", vch );
                    act ( "$n shivers and looks very ill.", vch, NULL, NULL,
                          TO_ROOM );
                    affect_join ( vch, &plague );
                }
            }

            dam = UMIN ( ch->level, af->level / 5 + 1 );
            ch->mana -= dam;
            ch->move -= dam;
            damage_old ( ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE, 0 );
        }
        else if ( IS_AFFECTED ( ch, AFF_POISON ) && ch != NULL &&
                  !IS_AFFECTED ( ch, AFF_SLOW ) )
        {
            AFFECT_DATA *poison;
            poison = affect_find ( ch->affected, gsn_poison );
            if ( poison != NULL )
            {
                act ( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
                send_to_char ( "You shiver and suffer.\n\r", ch );
                damage_old ( ch, ch, poison->level / 10 + 1, gsn_poison,
                             DAM_POISON, FALSE, 0 );
            }
        }

        else if ( ch->position == POS_INCAP && number_range ( 0, 1 ) == 0 )
        {
            damage ( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE, 0 );
        }
        else if ( ch->position == POS_MORTAL )
        {
            damage ( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE, 0 );
        }
     }
    } 
    
    /*
     * 
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
/*    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        if ( !IS_VALID ( ch ) && !IS_VALID ( ch->desc ) )
        {
            bug ( "char_update: trying to work with invalid character", 0 );
            break;
        }

        ch_next = ch->next;
        if ( ch->desc != NULL && ch->desc->descriptor % 30 == save_number && ch->desc->connected == CON_PLAYING )
            save_char_obj ( ch );
        if ( ch == ch_quit )
	{
	    do_replay ( ch, "" );
            do_quit ( ch, "" );
	}
    } */
  
    tail_chain (  );
    return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
WOLLA ( obj_update )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;
    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
        CHAR_DATA *rch;
        char *message;
        obj_next = obj->next;

        for ( paf = obj->affected; paf != NULL; paf = paf_next )
        {
            paf_next = paf->next;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if ( number_range ( 0, 4 ) == 0 && paf->level > 0 )
                    paf->level--;
            }
            else if ( paf->duration < 0 );
            else
            {
                if ( paf_next == NULL || paf_next->type != paf->type ||
                     paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[paf->type].msg_obj )
                    {
                        if ( obj->carried_by != NULL )
                        {
                            rch = obj->carried_by;
                            act_new ( skill_table[paf->type].msg_obj, rch, obj,
                                  NULL, TO_CHAR, POS_RESTING );
                        }
                        if ( obj->in_room != NULL &&
                             obj->in_room->people != NULL )
                        {
                            rch = obj->in_room->people;
                            act_new ( skill_table[paf->type].msg_obj, rch, obj,
                                  NULL, TO_ALL, POS_RESTING );
                        }
                    }
                }

                affect_remove_obj ( obj, paf );
            }
        }

        if ( obj->timer <= 0 || --obj->timer > 0 )
            continue;
        switch ( obj->item_type )
        {
            default:                message = "$p crumbles into dust."; 	break;
            case ITEM_FOUNTAIN:     message = "$p dries up.";           	break;
            case ITEM_CORPSE_NPC:   message = "$p decays into dust.";   	break;
            case ITEM_CORPSE_PC:    message = "$p decays into dust.";   	break;
            case ITEM_FOOD:         message = "$p decomposes.";         	break;
            case ITEM_POTION:       message = "$p has evaporated from disuse."; break;
            case ITEM_PORTAL:       message = "$p fades out of existence.";     break;
            case ITEM_CONTAINER:
	                case ITEM_PIT:
                if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) )
                    if ( obj->contains )
                        message =
                            "$p flickers and vanishes, spilling its contents on the floor.";
                    else
                        message = "$p flickers and vanishes.";
                else
                    message = "$p crumbles into dust.";
                break;
        }

        if ( obj->carried_by != NULL )
        {
            if ( IS_NPC ( obj->carried_by ) &&
                 obj->carried_by->pIndexData->pShop != NULL )
                obj->carried_by->silver += obj->cost / 5;
            else
            {
                act_new ( message, obj->carried_by, obj, NULL, TO_CHAR, POS_RESTING );
                if ( obj->wear_loc == WEAR_FLOAT )
                    act_new ( message, obj->carried_by, obj, NULL, TO_ROOM, POS_RESTING );
            }
        }
        else if ( obj->in_room != NULL &&
                  ( rch = obj->in_room->people ) != NULL )
        {
            if ( !
                 ( obj->in_obj &&
                   obj->in_obj->pIndexData->item_type == ITEM_PIT &&
                   !CAN_WEAR ( obj->in_obj, ITEM_TAKE ) ) )
            {
                act_new ( message, rch, obj, NULL, TO_ROOM, POS_RESTING );
                act_new ( message, rch, obj, NULL, TO_CHAR, POS_RESTING );
            }
        }
        
	if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
	&&  obj->contains)
	{
     	    OBJ_DATA *t_obj, *next_obj;

	    for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
	    {
		next_obj = t_obj->next_content;
		obj_from_obj(t_obj);

		if (obj->in_obj)
		    obj_to_obj(t_obj,obj->in_obj);

		else if (obj->carried_by)
		    if (obj->wear_loc == WEAR_FLOAT)
			if (obj->carried_by->in_room == NULL)
			    extract_obj(t_obj);
			else
			    obj_to_room(t_obj,obj->carried_by->in_room);
		    else
		    	obj_to_char(t_obj,obj->carried_by);

		else if (obj->in_room == NULL)
		    extract_obj(t_obj);

		else
		    obj_to_room(t_obj,obj->in_room);
	    }
	}

        extract_obj ( obj );
    }
    return;
}

/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 *
 * Made this function only launch every 3 pulses, we don't need
 * to launch it every pulse. This saves us some CPU - Rhaelar 11'2004.
 */
WOLLA ( aggr_update )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;
    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
        wch_next = wch->next;
        if ( IS_NPC ( wch ) || wch->level >= LEVEL_IMMORTAL ||
             wch->in_room == NULL || wch->in_room->area->empty )
            continue;
        for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
        {
            int count;
            ch_next = ch->next_in_room;
            if ( !IS_NPC ( ch ) || !IS_SET ( ch->act, ACT_AGGRESSIVE ) ||
                 IS_SET ( ch->in_room->room_flags, ROOM_SAFE ) ||
                 IS_AFFECTED ( ch, AFF_CALM ) || ch->fighting != NULL ||
                 IS_AFFECTED ( ch, AFF_CHARM ) || !IS_AWAKE ( ch ) ||
                 ( IS_SET ( ch->act, ACT_WIMPY ) && IS_AWAKE ( wch ) ) ||
                 !can_see ( ch, wch ) || number_bits ( 1 ) == 0 )
                continue;
            /*
             * Ok we have a 'wch' player character and a 'ch' npc aggressor.
             * Now make the aggressor fight a RANDOM pc victim in the room,
             *   giving each 'vch' an equal chance of selection.
             */
            count = 0;
            victim = NULL;
            for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
            {
                vch_next = vch->next_in_room;
                if ( !IS_NPC ( vch ) && vch->level < LEVEL_IMMORTAL &&
                     ch->level >= vch->level - 5 &&
                     ( !IS_SET ( ch->act, ACT_WIMPY ) || !IS_AWAKE ( vch ) )
                     && can_see ( ch, vch ) )
                {
                    if ( number_range ( 0, count ) == 0 )
                        victim = vch;
                    count++;
                }
            }

            if ( victim == NULL )
                continue;
            multi_hit ( ch, victim, TYPE_UNDEFINED );
        }
    }
    tail_chain (  );
    return;
}

WOLLA ( olcautosave )
{
    AREA_DATA *pArea;
    DESCRIPTOR_DATA *d;
    char buf[MAX_INPUT_LENGTH];
/*  if you're still adding areas manually, you may need to remove the next line - Chance */
/*    save_area_list (  ); */
/*    HELP_AREA *ha;
    FILE *fp=NULL;
    save_helps( fp, ha ); */
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->editor )
            send_to_char ( "OLC Autosaving:\n\r", d->character );
    }
    sprintf ( buf, "None.\n\r" );
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        /* Save changed areas. */
        if ( IS_SET ( pArea->area_flags, AREA_CHANGED ) )
        {
            save_area ( pArea );
            sprintf ( buf, "%24s - '%s'", pArea->name, pArea->file_name );
            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( d->editor )
                {
                    send_to_char ( buf, d->character );
                    send_to_char ( "\n\r", d->character );
                }
            }
            REMOVE_BIT ( pArea->area_flags, AREA_CHANGED );
        }
    }
    if ( !str_cmp ( buf, "None.\n\r" ) )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->editor )
                send_to_char ( buf, d->character );
        }
    }
    tail_chain (  );
    return;
}

void pkviolent ( CHAR_DATA * ch, int pkviolenttime )
{
    SET_BIT ( ch->act, PLR_NORESTORE );
//    SET_BIT ( ch->act, PLR_LOG );    
    SET_BIT ( ch->act2, PLR2_PKVIOLENT );
    ch->pcdata->pkviolenttime = pkviolenttime;
}

void pksafe ( CHAR_DATA * ch, int pksafetime )
{
    SET_BIT ( ch->act2, PLR2_PKSAFE );
//    SET_BIT ( ch->act, PLR_LOG );
    ch->pcdata->pksafetime = pksafetime;
}

WOLLA ( pk_update )
{
//    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
     if ( !IS_NPC ( ch ) )
     {
      if ( ch->pcdata->pkviolenttime > 0 )
      {
        if ( --ch->pcdata->pkviolenttime == 0 )
	{
	 REMOVE_BIT ( ch->act, PLR_NORESTORE );
//	 REMOVE_BIT ( ch->act, PLR_LOG );
	 REMOVE_BIT ( ch->act2, PLR2_PKVIOLENT );
         send_to_char ( "You are no longer {Rviolent{x.\n\r", ch );
//        return;
	}
      }
/*      if ( ch->pcdata->pksafetime >= 0 )
	ch->pcdata->pksafetime--;
      if ( ch->pcdata->pksafetime == 0 ) */
      if ( ch->pcdata->pksafetime > 0 )
      {
        if ( --ch->pcdata->pksafetime == 0 )
	{
//	 REMOVE_BIT ( ch->act, PLR_LOG );
	 REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
         send_to_char ( "You are no longer {wsafe{x.\n\r", ch );
//         return;
	}
      }
    }
  }
  tail_chain (  );
  return;
}


WOLLA ( kinky_update )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    char buf[MAX_STRING_LENGTH];
    
/*    for ( d = descriptor_list; d != NULL; d = d->next )
    { */
    if ( arena_countdown > 0 )
    {
     if ( --arena_countdown == 0 )
     {
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character )
        {
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGER ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGER );
            if ( IS_SET ( d->character->act2, PLR2_CHALLENGED ) )
                REMOVE_BIT ( d->character->act2, PLR2_CHALLENGED );
            if ( IS_SET ( d->character->in_room->room_flags, ROOM_ARENA ) )
            {
                char_from_room ( d->character );
                char_to_room ( d->character,
                               get_room_index ( ROOM_VNUM_TEMPLE ) );
		do_look ( d->character, "auto" );		
            }
        
       }
      }
	do_gmessage ( "{R[{WArena{R] {WToo much time taken. Arena challenge removed.{x\n\r" );
	arena = FIGHT_OPEN;           
     }
    
    else if ( arena_countdown == 1 )
    {
	do_gmessage ( "{R[{WArena{R] {WBetter hurry up, challenge waiting time is running out!{x\n\r" );
    } 
   }   
      for ( ch = char_list; ch != NULL; ch = ch_next )
    { 
/*      if ( ch->pcdata->exiletime >= 0 )
	ch->pcdata->exiletime--; */
	ch_next = ch->next;
    if ( !IS_NPC ( ch ) )
    {
     if ( ch->pcdata->exiletime > 0 && ch->desc && !IS_SET ( ch->comm, COMM_AFK ) )
     {
      if ( --ch->pcdata->exiletime == 0 )
      {
         send_to_char ( "{YYour exile is {zover{x{Y! You are now a member of {Rno {Yclan!{x\n\r", ch );
 	 ch->clan = 0;
	 ch->clan_rank = 0;
         sprintf ( buf, "%s has finished his exile time. Taking him out of the clan...", ch->name );
         wiznet ( buf, NULL, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
      }
     } 
     if ( ch->pcdata->noclantime > 0 && ch->desc && !IS_SET ( ch->comm, COMM_AFK ) )
     {
      if ( --ch->pcdata->noclantime == 0 )
      {
         send_to_char ( "{YYour noclan period is {zover{x{Y! You can now {Rno a {Yclan{x again!\n\r", ch );
         sprintf ( buf, "%s has finished his noclan time. They can now join a clan again...", ch->name );
         wiznet ( buf, NULL, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
      }
     } 
/*     if ( ch->pcdata->arenatimer > 0 && --ch->pcdata->arenatimer == 0 )
     {
     	send_to_char ( "You can now perform an arena challenge again.\n\r", ch );
     }
*/
     if ( ch->pcdata->rerolltimer > 0 && ch->desc )
     {
       if ( --ch->pcdata->rerolltimer == 0 )
       {
         ;
       	 //send_to_char ( "You are now allowed to reroll again.\n\r", ch );
       }
     }
     if ( ch->pcdata->assassinatetimer > 0 && ch->desc )
     {
      if ( --ch->pcdata->assassinatetimer == 0 )
      {
      	send_to_char ( "You are now allowed to do another assassination.\n\r", ch );
      }
     }
     if ( ch->pcdata->jailtime > 0 && ch->desc )
     {
      if ( --ch->pcdata->jailtime == 0 )
      {
	send_to_char ( "Your time in jail is over. You are free!\n\r", ch );
	REMOVE_BIT ( ch->act, PLR_FREEZE );
        char_from_room ( ch );
	char_to_room ( ch, get_room_index ( ROOM_VNUM_MS ) );
	act ( "$n flies into the room, having been kicked in the butt by the jail inspector.", ch, NULL, NULL, TO_ROOM );
        sprintf ( buf, "%s has finished his jailtime. Releasing...", ch->name );
	do_look ( ch, "auto" );
        wiznet ( buf, NULL, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
      }     
     }    
    }
  }
  tail_chain (  );
  return;
}



/*
void pksafe_update ( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING && ( ch = ( d->original ? d->original : d->character ) ) != NULL )
     {
      if ( ch->pcdata->pksafetime >= 0 )
	ch->pcdata->pksafetime--;
      if ( ch->pcdata->pksafetime == 0 )
        {
	REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
        send_to_char ( "You are no longer {wsafe{x.\n\r", ch );
        return;
        }
  }
 }
}
*/

WOLLA ( autosave )
{
    DESCRIPTOR_DATA *d;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( !IS_VALID ( d->character ) || !IS_VALID ( d ) )
        {
//            bug ( "autosave: Not saving not yet connected or bugged character", 0 );
            break;
        }
        
        if ( d->connected == CON_PLAYING )
	{
	   if ( !IS_NPC ( d->character ) && !IS_SET ( d->character->act2, PLR2_CHALLENGED ) && !IS_SET ( d->character->act2, PLR2_CHALLENGER ) && !IS_SET ( d->character->pcdata->tag_flags, TAG_PLAYING ) && !IS_SET ( d->character->pcdata->tag_flags, TAG_WAITING ) && d->character->timer < 5 && !IS_SET ( d->character->in_room->room_flags, ROOM_ARENA && chance ( 30 ) ) )
	   {
                save_char_obj ( d->character );
	   }
	}
    }
   tail_chain (  );    
   return;
}

WOLLA ( hint_update )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;

  for(ch=char_list;ch!=NULL;ch=ch_next)
    {
    ch_next = ch->next;

    if(!IS_NPC(ch) && !IS_SET(ch->comm, COMM_NOHINT))
      {
      send_to_char("{R*{Y*{B*{WHINT{B*{Y*{R*{c ",ch);
      switch(number_range(0,27))
        {
        default: send_to_char("Type 'wizlist' to see a list of immortals on Shattered Dreams.", ch); break;
        case 0: send_to_char("Type 'areas' to see an organized list of areas you can travel to.",ch); break;
        case 1: send_to_char("Type 'channels' to see which of your channels are on and which are off.",ch); break;
        case 2: send_to_char("Type 'who' to see what players are online.",ch); break;
        case 3: send_to_char("Type 'help newbie' if you need starting help.",ch); break;
        case 4: send_to_char("Use 'ask' for asking questions. E.g: 'ask How can I get to Maerkai?'",ch); break;
        case 5: send_to_char("Use 'jog' to walk your speedwalks. See 'help jog' for help.",ch); break;
        case 6: send_to_char("Use 'map' for finding the speedwalk to an area on the mud. See 'help map' for help.",ch); break;
	case 7: send_to_char("Type 'hints' to turn this channel on/off.",ch); break;
	case 8: send_to_char("Type 'skills' to see a list of your current skills.",ch); break;
	case 9: send_to_char("Type 'spells' to see a list of your current spells.",ch); break;
	case 10: send_to_char("Type 'help note' to get help on writing and reading notes.",ch); break;
	case 11: send_to_char("There is a bunch of useful information on our website, www.sdmud.com, check it out.",ch); break;
	case 12: send_to_char("If you think you are getting too much spam in combat, try toggling some of the 'verbose' options. See 'help verbose'",ch); break;
	case 13: send_to_char("If you can't find the helpfile for something, just use the ask channel! There are almost always people willing to help.",ch); break;
	case 14: send_to_char("Type 'score' to see your current statistics.",ch); break;
	case 15: send_to_char("Type 'affects' to see what spells and skills you are affected by.",ch); break;
	case 16: send_to_char("Wanna know what becoming an Ancient is about? See 'help ancient'.",ch); break;
	case 17: send_to_char("Type 'clan list' for a list of clans on SD.",ch); break;
	case 18: send_to_char("If you want to know more about the clan system on SD, type 'help clans'.",ch); break;
	case 19: send_to_char("Type 'motd' to see some important and useful notices.",ch); break;
	case 20: send_to_char("Be sure to read the rules, so you don't get in trouble. Type 'help rules'.",ch); break;
	case 21: send_to_char("If you wish to go PK without being in a dependent clan, just type 'loner' twice!",ch); break;
	case 22: send_to_char("Use 'consider' to see how tough someone is, hp, vulnerabilites, immunities, and such.",ch); break;
	case 23: send_to_char("Type 'help stances' to know more about SD's unique fighting styles.",ch); break;			case 24: send_to_char("To quit and reach the logon screen without having to reconnect, simply type 'quit relog'",ch); break;
	case 25: send_to_char("Type 'help pkill' to see more info on player-killing on SD, the rules and what not.",ch); break;
	case 26: send_to_char("If you wish to re-create your character, but you're still in 1st tier, type 'delete' twice and re-make the character.",ch); break;
	case 27: send_to_char("At any point in the tiers 2-4, at any level, you can reroll to another race and class if you please.",ch); break;
        }
      send_to_char("{x\n\r",ch);
      }
    }
    tail_chain (  );    
return;
}

WOLLA ( maintenancecheck )
{
  if ( ++aftermaintenance > 70 )
  {
    char buf[MAX_STRING_LENGTH];
    int hour;
    hour = ( int ) ( struct tm * ) localtime ( &current_time )->tm_hour;
    if ( hour == 7 && !is_reboot_countdown )
    {
/*     if ( !stuff_was_changed )
     {
      sprintf ( buf,
      "{R[{WNotice{R]{C No changes done to the source. Maintenance reboot will not execute today.{x\n\r" );
                         do_gmessage ( buf );
      return;
     } */
//     else
     if ( stuff_was_changed )
     {
        reboot_countdown = 30;
        is_reboot_countdown = TRUE;

        sprintf ( buf,
         "{R[{WNotice{R]{C The daily maintenance reboot will execute soon. reboot timer set.{x" );
                           do_gmessage ( buf );
        sprintf ( buf,
          "\n\r{R[{WReboot{R]{C Daily maintenance reboot is being prepared, {R%d{C minutes till reboot.\n\r{x",
                          reboot_countdown );
                do_gmessage ( buf );

	return;
     }
     aftermaintenance = 30;
    }
   }
  tail_chain (  );
  return;
}

WOLLA ( sync_stats )
{
    FILE *fp=NULL;
    int number;
    int count;
    DESCRIPTOR_DATA *d;
/* Most on ever */
    count = 0;
    for ( d = descriptor_list; d; d = d->next )
    {
        count++;
    }
    cur_on = count;

    if ( cur_on > max_on )
        max_on = cur_on;

    if ( max_on > max_ever && max_ever > 1 )
    {
        max_ever = max_on;
        if ( ( fp = file_open ( MAX_EVER_FILE, "w" ) ) == NULL )
        {
            bug ( "max_ever_update:  fopen of MAX_EVER_FILE failed", 0 );
            new_fclose ( fp );
            return;
        }
        fprintf ( fp, "MAX_EVER %d\n", max_ever );
        new_fclose ( fp );
    }
    else
    {
        if ( !( fp = file_open ( MAX_EVER_FILE, "r" ) ) )
        {
            new_fclose ( fp );
            return;
        }

        for ( ;; )
        {
            char *word;
            char letter;

            do
            {
                letter = getc ( fp );
                if ( feof ( fp ) )
                {
                    new_fclose ( fp );
                    return;

                }
            }
            while ( isspace ( letter ) );
            ungetc ( letter, fp );

            word = fread_word ( fp );
            if ( !str_cmp ( word, "MAX_EVER" ) )
            {
                number = fread_number ( fp );
                if ( number > 0 )
                    max_ever = number;
                new_fclose ( fp );
                return;

            }
        }
    }
    /* Most on ever - end */
    tail_chain (  );    
    return;
}

WOLLA ( tournament_update )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    if ( !is_mass_arena )
        is_mass_arena_fight = FALSE;

    if ( arena == FIGHT_OPEN )
    {
        //arenacount = 0;

        if ( is_mass_arena )
            mass_arena_ticks++;
        else
        {
            mass_arena_ticks = 0;
            mass_arena_players_joined = 0;
//            sprintf ( lastwinner, " " );
        }

        if ( mass_arena_players < 4 )
        {
            if ( mass_arena_valid_ticks > 0 )
                mass_arena_valid_ticks--;
        }
        else
        {
            if ( mass_arena_valid_ticks < 30 )
                mass_arena_valid_ticks++;
	    else mass_arena_valid_ticks = 0;
        }

        if ( mass_arena_valid_ticks >= 2 && !is_mass_arena &&
             !is_mass_arena_fight )
        {
            do_gmessage
                ( "{R[{WARENA{R] {xThe tournament is now {GOPEN{x.\n\r{R[{WARENA{R] {xType: {RTOURNAMENT JOIN{x. You have 2 minutes to do so.{x\n\r" );
            mass_arena_ticks = 0;
            mass_arena_players_joined = 0;
            is_mass_arena = TRUE;
            do_tclear ( NULL, NULL );        
	}
        if ( mass_arena_ticks >= 4 && !is_mass_arena_fight && is_mass_arena )
        {
            do_gmessage
                ( "{R[{WARENA{R] {xNot enough people joined, tournament cancelled.{x\n\r" );
            is_mass_arena = FALSE;
            is_mass_arena_fight = FALSE;
            mass_arena_valid_ticks = 0;
            mass_arena_players_joined = 0;
            mass_arena_ticks = 0;
            do_aclear ( NULL, NULL );
        }
    }

    mass_arena_players = 0;
    mass_arena_players_joined = 0;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
       if ( !IS_NPC ( ch ) )
       {
        if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) )
                mass_arena_players++;

            if ( IS_SET ( ch->act2, PLR2_MASS_JOINER ) &&
                 IS_SET ( ch->act2, PLR2_MASS_ARENA ) )
                mass_arena_players_joined++;
            if ( !IS_SET ( ch->act2, PLR2_MASS_ARENA ) )
                REMOVE_BIT ( ch->act2, PLR2_MASS_JOINER );

            if ( mass_arena_ticks >= 2 && is_mass_arena &&
                 mass_arena_players_joined >= 4 )
            {
                do_start_massarena (  );
            }
            if ( mass_arena_ticks == 1 && is_mass_arena )
            {
                send_to_char ( "{R[{WARENA{R] {xThe tournament will start in {R60 seconds{x. {w{zLast call!{x\n\r", ch );
            }
            if ( mass_arena_players < 2 && is_mass_arena && is_mass_arena_fight
                 && IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) &&
                 IS_SET ( ch->act2, PLR2_MASS_JOINER ) &&
                 IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
                 is_name ( ch->name, lastwinner ) )
            {
                char wbuf[MSL];
                sprintf ( wbuf,
                         "{R[{WARENA{R] %s has won the tournament!{x\n\r",
                          ch->name );
                do_gmessage ( wbuf );
                ch->qps += 2;
                stop_fighting ( ch, TRUE );
                ch->hit = ch->max_hit;
                ch->mana = ch->max_mana;
                ch->move = ch->max_move;
		ch->stunned = 0;
                update_pos ( ch );
                sprintf ( lastwinner, " " );
                send_to_char
                    ( "You have been awarded 2 iqp for your victory!\n\r",
                      ch );
		ch->pcdata->twins += 1;
                REMOVE_BIT ( ch->act2, PLR2_MASS_JOINER );
                REMOVE_BIT ( ch->act2, PLR2_MASS_ARENA );
                REMOVE_BIT ( ch->act2, PLR2_PKVIOLENT );
                REMOVE_BIT ( ch->act2, PLR_NORESTORE );
                is_mass_arena = FALSE;
                is_mass_arena_fight = FALSE;

	        stop_fighting ( ch, TRUE );
      	        char_from_room ( ch );
      		char_to_room ( ch, get_room_index ( ROOM_VNUM_AWINNER ) );
      		ch->hit = ch->max_hit;
      		ch->mana = ch->max_mana;
      		ch->move = ch->max_move;
		ch->pcdata->pkviolenttime = 0;
     	 	update_pos ( ch );
      		do_look ( ch, "auto" );

                do_aclear ( NULL, NULL );
                break;
            }
	}
    }
    tail_chain (  );
    return;
}

WOLLA ( rtimer_update )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    char rebootbuf[MAX_STRING_LENGTH];
    
    if ( --reboot_countdown >= 0 )
    {
        if ( reboot_countdown == 0 )
        {
            reboot_countdown = -1;
            do_reboot ( NULL, "wolla" );
        }
        else
        {
            if ( reboot_countdown == 5 || reboot_countdown == 1 ||
                 reboot_countdown == 2 )
            {
                if ( reboot_countdown == 1 )
                {
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
       
       if ( !IS_NPC ( ch ) )
       {
	if ( ch->fighting != NULL )
	{
	   stop_fighting ( ch, TRUE );
	   send_to_char ( "{YYou can continue this fight after the reboot.{x\n\r", ch );
	}
        if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
	{
	    REMOVE_BIT ( ch->act2, PLR2_PKVIOLENT );
	    ch->pcdata->pkviolenttime = 0;
	}
	if ( IS_QUESTOR ( ch ) )
	{
	   end_quest ( ch, 0 );
	   send_to_char ( "{YYour quest has been cleared.{x\n\r", ch );
	}
        save_char_obj ( ch );	
	SET_BIT ( ch->act2, PLR2_PKSAFE );		
	}    
    }
                    do_aclear ( NULL, NULL );
		    do_alock ( NULL, NULL );
		    end_tag (  );
                    sprintf ( rebootbuf, "{R[{WReboot{R] {CLogin {YLOCKDOWN{C is now {GON{x.\n\r" );
                    do_gmessage ( rebootbuf );
/*                    sprintf ( rebootbuf, "{R[{Wreboot{R] {CYour character will not be saved beyond this point.{x.\n\r" );
                    do_gmessage ( rebootbuf );
                    sprintf ( rebootbuf, "{R[{Wreboot{R] {CIf you wish to save your character, do it manually.\n\r" );
                    do_gmessage ( rebootbuf );
                    sprintf ( rebootbuf, "{R[{Wreboot{R] {CIf you do not save, your character will be reverted to the state you're in now after the reboot.\n\r" ); */
                    do_gmessage ( rebootbuf );
                }
                else
                {
                    if ( reboot_countdown == 2 )
                    {
                        sprintf ( rebootbuf,
                                  "{R[{WReboot{R]{C Command {YLOCKDOWN{C is now {GON{x.\n\r" );
                        do_gmessage ( rebootbuf );
                    }
                }
		if ( reboot_countdown > 1 )
		{
                sprintf ( rebootbuf, "{R[{WReboot{R] {CReboot will execute in {R%d{C minutes.\n\r{x", reboot_countdown );
                do_gmessage ( rebootbuf );
		}
		else
                {
                sprintf ( rebootbuf, "{R[{WReboot{R] {CReboot will execute in {R%d{C minute.{x\n\r", reboot_countdown );
                do_gmessage ( rebootbuf );
                }
                sprintf ( rebootbuf, "{R[{WReboot{R] {CPlease see '{chelp reboot{C' for more information.{x\n\r" );
                do_gmessage ( rebootbuf );
            }
	}
    }
    tail_chain (  );    
    return;
}    

/*

void tele_update ( void ) 
{
	CHAR_DATA 	*ch;
 	CHAR_DATA	*ch_next;
	ROOM_INDEX_DATA *pRoomIndex;
     

	for (ch = char_list ; ch != NULL; ch = ch_next )
	{
	  ch_next = ch->next;
	  if ( IS_SET(ch->in_room->room_flags, ROOM_TELEPORT ) )
	   {
             
        if (IS_IMMORTAL(ch) && IS_SET(ch->act,PLR_NOSUMMON))
        {
        return;
        }
		do_look ( ch, "tele" );
        	if ( ch->in_room->tele_dest == 0 )
		  pRoomIndex = get_random_room (ch);
		else
		  pRoomIndex = get_room_index(ch->in_room->tele_dest);

		send_to_char ("You have been teleported!!!\n\r", ch);
		act("$n vanishes!!!\n\r", ch, NULL, NULL, TO_ROOM);
		char_from_room(ch);
		char_to_room(ch, pRoomIndex);
		act("$n slowly fades into existence.\n\r", ch, NULL, NULL,TO_ROOM);
		do_look(ch, "auto");
            }
	}
}*/

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

WOLLA ( update_handler )
{
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
//    static int pulse_music;
    static int pulse_auction;
    // static  int           pulse_tele;
    // static int pulse_quest;
   /* For dns lookup */
   recycle_descriptors();
  
    if ( --pulse_area <= 0 )
    {
        pulse_area = PULSE_AREA;
        /* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
        olcautosave (  );
        area_update (  );
/* Additional stuff by Rhaelar */
        sync_stats (  );	
        hint_update ( );
        if ( is_reboot_countdown == FALSE ) dailyevents( );
	/* Recycle running DNS lookups */
	recycle_dnss();    
    }
/*
    if ( --pulse_tele <= 0 )
    {
	pulse_tele	= PULSE_TELEPORT;
	tele_update	( );
    }    

    if ( --pulse_music <= 0 )
    {
        pulse_music = PULSE_MUSIC;
        song_update (  );
    }


    if ( --pulse_quest <= 0 )
    {
        pulse_quest = PULSE_QUEST;
        quest_update (  );
    }
*/
    if ( --pulse_mobile <= 0 )
    {
        pulse_mobile = PULSE_MOBILE;
        mobile_update (  );
    }

    if ( --pulse_violence <= 0 )
    {
        pulse_violence = PULSE_VIOLENCE;
        violence_update (  );
    }

    if ( --pulse_auction <= 0 )
    {
        pulse_auction = PULSE_AUCTION;
        update_auc (  );
    }
   
    if ( --pulse_point <= 0 )
    {        
	wiznet ( "TICK!", NULL, NULL, WIZ_TICKS, 0, 0 );
        pulse_point = PULSE_TICK;
	maintenancecheck (  );        
	autosave (  );	
	obj_update (  );	
	/* Custom stuff by Rhaelar */
	pk_update (  );
        quest_update( );   
        //bank_update (  );
	kinky_update (  );
	tag_update (  );
	tournament_update (  );
	rtimer_update (  );
	/* Stuff put last on list for stability reasons */
	weather_update (  );
	char_update (  );
	/* Disconnect people hanging at quit and such */
/*        randomize_entrances ( ROOM_VNUM_CHAIN ); */
    }
    aggr_update (  );    
    tail_chain (  );    
    return;
}
