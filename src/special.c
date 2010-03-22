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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sd.h"
#include "magic.h"

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN ( spec_boaz );
DECLARE_SPEC_FUN ( spec_breath_any );
DECLARE_SPEC_FUN ( spec_breath_acid );
DECLARE_SPEC_FUN ( spec_breath_fire );
DECLARE_SPEC_FUN ( spec_breath_frost );
DECLARE_SPEC_FUN ( spec_breath_gas );
DECLARE_SPEC_FUN ( spec_breath_lightning );
DECLARE_SPEC_FUN ( spec_cast_adept );
DECLARE_SPEC_FUN ( spec_cast_cleric );
DECLARE_SPEC_FUN ( spec_cast_judge );
DECLARE_SPEC_FUN ( spec_cast_mage );
DECLARE_SPEC_FUN ( spec_antihero_evil );
DECLARE_SPEC_FUN ( spec_cast_undead );
DECLARE_SPEC_FUN ( spec_executioner );
DECLARE_SPEC_FUN ( spec_fido );
DECLARE_SPEC_FUN ( spec_guard );
DECLARE_SPEC_FUN ( spec_janitor );
DECLARE_SPEC_FUN ( spec_poison );
DECLARE_SPEC_FUN ( spec_thief );
DECLARE_SPEC_FUN ( spec_nasty );
DECLARE_SPEC_FUN ( spec_troll_member );
DECLARE_SPEC_FUN ( spec_ogre_member );
DECLARE_SPEC_FUN ( spec_patrolman );
DECLARE_SPEC_FUN ( spec_dog_pee );
DECLARE_SPEC_FUN ( spec_cast_clan_adept );
DECLARE_SPEC_FUN ( spec_questmaster );
DECLARE_SPEC_FUN ( spec_registar );
DECLARE_SPEC_FUN ( spec_mayor );
DECLARE_SPEC_FUN ( spec_defensive_stance );
DECLARE_SPEC_FUN ( spec_aggressive_stance );
DECLARE_SPEC_FUN ( spec_all_stances );
DECLARE_SPEC_FUN ( spec_basic_stances );
DECLARE_SPEC_FUN ( spec_advanced_stances );
DECLARE_SPEC_FUN ( spec_viper_stance );
DECLARE_SPEC_FUN ( spec_crane_stance );
DECLARE_SPEC_FUN ( spec_mongoose_stance );
DECLARE_SPEC_FUN ( spec_crab_stance );
DECLARE_SPEC_FUN ( spec_bull_stance );
DECLARE_SPEC_FUN ( spec_mantis_stance );
DECLARE_SPEC_FUN ( spec_tiger_stance );
DECLARE_SPEC_FUN ( spec_swallow_stance );
DECLARE_SPEC_FUN ( spec_dragon_stance );
DECLARE_SPEC_FUN ( spec_monkey_stance );
DECLARE_SPEC_FUN ( spec_prog1 );
DECLARE_SPEC_FUN ( spec_prog2 );
DECLARE_SPEC_FUN ( spec_impale );
DECLARE_SPEC_FUN ( spec_circle );

/* the function table */
const struct spec_type spec_table[] = {
    {"spec_boaz", spec_boaz},
    {"spec_breath_any", spec_breath_any},
    {"spec_breath_acid", spec_breath_acid},
    {"spec_breath_fire", spec_breath_fire},
    {"spec_breath_frost", spec_breath_frost},
    {"spec_breath_gas", spec_breath_gas},
    {"spec_breath_lightning", spec_breath_lightning},
    {"spec_cast_adept", spec_cast_adept},
    {"spec_cast_cleric", spec_cast_cleric},
    {"spec_cast_judge", spec_cast_judge},
    {"spec_cast_mage", spec_cast_mage},
    {"spec_cast_undead", spec_cast_undead},
    {"spec_executioner", spec_executioner},
    {"spec_fido", spec_fido},
    {"spec_guard", spec_guard},
    {"spec_janitor", spec_janitor},
    {"spec_poison", spec_poison},
    {"spec_thief", spec_thief},
    {"spec_nasty", spec_nasty},
    {"spec_troll_member", spec_troll_member},
    {"spec_ogre_member", spec_ogre_member},
    {"spec_patrolman", spec_patrolman},
    {"spec_dog_pee", spec_dog_pee},
    {"spec_cast_clan_adept", spec_cast_clan_adept},
    {"spec_questmaster", spec_questmaster}, /* Vassago */
    {"spec_registar", spec_registar},
    {"spec_antihero_evil", spec_antihero_evil},
    {"spec_mayor", spec_mayor},
    {"spec_defensive_stance", spec_defensive_stance},
    {"spec_aggressive_stance", spec_aggressive_stance},
    {"spec_all_stances", spec_all_stances},
    {"spec_basic_stances", spec_basic_stances},
    {"spec_advanced_stances", spec_advanced_stances},
    {"spec_viper_stance", spec_viper_stance},
    {"spec_crane_stance", spec_crane_stance},
    {"spec_mongoose_stance", spec_mongoose_stance},
    {"spec_crab_stance", spec_crab_stance},
    {"spec_bull_stance", spec_bull_stance},
    {"spec_mantis_stance", spec_mantis_stance},
    {"spec_tiger_stance", spec_tiger_stance},
    {"spec_swallow_stance", spec_swallow_stance},
    {"spec_dragon_stance", spec_dragon_stance},
    {"spec_monkey_stance", spec_monkey_stance},
    {"spec_prog1", spec_prog1},    
    {"spec_prog2", spec_prog2},
    {"spec_impale", spec_impale},          
    {"spec_circle", spec_circle},          
    {NULL, NULL}
};

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup ( const char *name )
{
    int i;

    for ( i = 0; spec_table[i].name != NULL; i++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( spec_table[i].name[0] ) &&
             !str_prefix ( name, spec_table[i].name ) )
            return spec_table[i].function;
    }

    return 0;
}

char *spec_name ( SPEC_FUN * function )
{
    int i;

    for ( i = 0; spec_table[i].function != NULL; i++ )
    {
        if ( function == spec_table[i].function )
            return spec_table[i].name;
    }

    return NULL;
}

bool spec_troll_member ( CHAR_DATA * ch )
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if ( !IS_AWAKE ( ch ) || IS_AFFECTED ( ch, AFF_CALM ) ||
         ch->in_room == NULL || IS_AFFECTED ( ch, AFF_CHARM ) ||
         ch->fighting != NULL )
        return FALSE;

    /* find an ogre to beat up */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC ( vch ) || ch == vch )
            continue;

        if ( vch->pIndexData->vnum == MOB_VNUM_PATROLMAN )
            return FALSE;

        if ( vch->pIndexData->group == GROUP_VNUM_OGRES &&
             ch->level > vch->level - 2 && !is_safe ( ch, vch ) )
        {
            if ( number_range ( 0, count ) == 0 )
                victim = vch;

            count++;
        }
    }

    if ( victim == NULL )
        return FALSE;

    /* say something, then raise hell */
    switch ( number_range ( 0, 6 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = "$n yells '{aI've been looking for you, punk!{x'";
            break;
        case 1:
            message = "With a scream of rage, $n attacks $N.";
            break;
        case 2:
            message =
                "$n says '{aWhat's slimy Ogre trash like you doing around here?{x'";
            break;
        case 3:
            message =
                "$n cracks his knuckles and says '{SDo ya feel lucky?{x'";
            break;
        case 4:
            message = "$n says '{aThere's no cops to save you this time!{x'";
            break;
        case 5:
            message = "$n says '{aTime to join your brother, spud.{x'";
            break;
        case 6:
            message = "$n says '{aLet's rock.{x'";
            break;
    }

    if ( message != NULL )
        act ( message, ch, NULL, victim, TO_ALL );
    multi_hit ( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_ogre_member ( CHAR_DATA * ch )
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if ( !IS_AWAKE ( ch ) || IS_AFFECTED ( ch, AFF_CALM ) ||
         ch->in_room == NULL || IS_AFFECTED ( ch, AFF_CHARM ) ||
         ch->fighting != NULL )
        return FALSE;

    /* find an troll to beat up */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC ( vch ) || ch == vch )
            continue;

        if ( vch->pIndexData->vnum == MOB_VNUM_PATROLMAN )
            return FALSE;

        if ( vch->pIndexData->group == GROUP_VNUM_TROLLS &&
             ch->level > vch->level - 2 && !is_safe ( ch, vch ) )
        {
            if ( number_range ( 0, count ) == 0 )
                victim = vch;

            count++;
        }
    }

    if ( victim == NULL )
        return FALSE;

    /* say something, then raise hell */
    switch ( number_range ( 0, 6 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = "$n yells '{aI've been looking for you, punk!{x'";
            break;
        case 1:
            message = "With a scream of rage, $n attacks $N.'";
            break;
        case 2:
            message =
                "$n says '{aWhat's Troll filth like you doing around here?{x'";
            break;
        case 3:
            message =
                "$n cracks his knuckles and says '{SDo ya feel lucky?{x'";
            break;
        case 4:
            message = "$n says '{aThere's no cops to save you this time!{x'";
            break;
        case 5:
            message = "$n says '{aTime to join your brother, spud.{x'";
            break;
        case 6:
            message = "$n says '{aLet's rock.{x'";
            break;
    }

    if ( message != NULL )
        act ( message, ch, NULL, victim, TO_ALL );
    multi_hit ( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_boaz ( CHAR_DATA * ch )
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;
    bool found = FALSE;

    if ( !IS_AWAKE ( ch ) || IS_AFFECTED ( ch, AFF_CALM ) ||
         ch->in_room == NULL || IS_AFFECTED ( ch, AFF_CHARM ) ||
         ch->fighting != NULL )
        return FALSE;

    /* find clayton */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC ( vch ) )
            found = TRUE;

        if ( !IS_NPC ( vch ) || ch == vch )
            continue;

        if ( vch->pIndexData->vnum == MOB_VNUM_CLAYTON )
        {
            if ( number_range ( 0, count ) == 0 )
                victim = vch;

            count++;
        }
    }

    if ( !found )
        return FALSE;

    if ( victim == NULL )
        return FALSE;

    switch ( number_range ( 0, 9 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = NULL;
            break;
        case 1:
            message = NULL;
            break;
        case 2:
            message = "blank";
            break;
        case 3:
            message = NULL;
            break;
        case 4:
            message = NULL;
            break;
        case 5:
            message = "blank";
            break;
        case 6:
            message = NULL;
            break;
        case 7:
            message = NULL;
            break;
        case 8:
            message = NULL;
            break;
        case 9:
            message = NULL;
            break;
    }

    if ( message == NULL )
        return FALSE;

    /* say something, then raise hell */
    switch ( number_range ( 0, 9 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = "$n yells '{aLeave me alone!{x'";
            break;
        case 1:
            message = "With a scream of rage, $n attacks $N.";
            break;
        case 2:
            message = "$n shouts '{aStop that! Stop that!{x' and attacks.";
            break;
        case 3:
            message = NULL;
            break;
        case 4:
            message = "$n snorts a line of cocaine.";
            break;
        case 5:
            message = "$n yells '{aGet out!{x'";
            break;
        case 6:
            message = "$n shouts '{aLeave mommy alone, you little shit!{x'";
            break;
        case 7:
            message = "$n lights up another cigarette.";
            break;
        case 8:
            message = NULL;
            break;
        case 9:
            message = NULL;
            break;
    }

    if ( message != NULL )
    {
        act ( message, ch, NULL, victim, TO_ALL );
        switch ( number_range ( 0, 8 ) )
        {
            default:
                message = NULL;
                break;
            case 0:
                message =
                    "$n yanks $N up by the arm and throws him through the door.";
                break;
            case 1:
                message = "$n slaps $N across the face.";
                break;
            case 2:
                message = NULL;
                break;
            case 3:
                message = "$n throws $N onto the couch.";
                break;
            case 4:
                message = NULL;
                break;
            case 5:
                message = "$n spanks $N.";
                break;
            case 6:
                message = "$n sits down and ignores $N.";
                break;
            case 7:
                message = NULL;
                break;
            case 8:
                message = NULL;
                break;
        }
        if ( message != NULL )
        {
            act ( message, ch, NULL, victim, TO_ALL );
        }
    }
    switch ( number_range ( 0, 6 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = "$N screams at $n.";
            break;
        case 1:
            message = "$N screams in pain.";
            break;
        case 2:
            message = "$N lies down and cries.";
            break;
        case 3:
            message = "$N steals his mommy's cigarettes.";
            break;
        case 4:
            message = "$N screams in agony!";
            break;
        case 5:
            message = "$N starts wailing at the top of his lungs.";
            break;
        case 6:
            message = "$N runs to you to get away from his mommy.";
            break;
    }

    if ( message != NULL )
    {
        act ( message, ch, NULL, victim, TO_ALL );
    }

    return TRUE;
}

bool spec_patrolman ( CHAR_DATA * ch )
{
    CHAR_DATA *vch, *victim = NULL;
    OBJ_DATA *obj;
    char *message;
    int count = 0;

    if ( !IS_AWAKE ( ch ) || IS_AFFECTED ( ch, AFF_CALM ) ||
         ch->in_room == NULL || IS_AFFECTED ( ch, AFF_CHARM ) ||
         ch->fighting != NULL )
        return FALSE;

    /* look for a fight in the room */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( vch == ch )
            continue;

        if ( vch->fighting != NULL )    /* break it up! */
        {
            if ( number_range ( 0, count ) == 0 )
                victim =
                    ( vch->level >
                      vch->fighting->level ) ? vch : vch->fighting;
            count++;
        }
    }

    if ( victim == NULL ||
         ( IS_NPC ( victim ) && victim->spec_fun == ch->spec_fun ) )
        return FALSE;

    if ( ( ( obj = get_eq_char ( ch, WEAR_NECK_1 ) ) != NULL &&
           obj->pIndexData->vnum == OBJ_VNUM_WHISTLE ) ||
         ( ( obj = get_eq_char ( ch, WEAR_NECK_2 ) ) != NULL &&
           obj->pIndexData->vnum == OBJ_VNUM_WHISTLE ) )
    {
        act ( "You blow down hard on $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n blows on $p, ***WHEEEEEEEEEEEET***", ch, obj, NULL,
              TO_ROOM );

        for ( vch = char_list; vch != NULL; vch = vch->next )
        {
            if ( vch->in_room == NULL )
                continue;

            if ( !IS_NPC ( ch ) && vch->in_room != ch->in_room &&
                 vch->in_room->area == ch->in_room->area )
                send_to_char ( "You hear a shrill whistling sound.\n\r",
                               vch );
        }
    }

    switch ( number_range ( 0, 6 ) )
    {
        default:
            message = NULL;
            break;
        case 0:
            message = "$n yells '{aAll roit! All roit! break it up!{x'";
            break;
        case 1:
            message =
                "$n says '{aSociety's to blame, but what's a bloke to do?{x'";
            break;
        case 2:
            message =
                "$n mumbles '{abloody kids will be the death of us all.{x'";
            break;
        case 3:
            message = "$n shouts '{aStop that! Stop that!{T' and attacks.";
            break;
        case 4:
            message = "$n pulls out his billy and goes to work.";
            break;
        case 5:
            message =
                "$n sighs in resignation and proceeds to break up the fight.";
            break;
        case 6:
            message = "$n says '{aSettle down, you hooligans!{x'";
            break;
    }

    if ( message != NULL )
        act ( message, ch, NULL, NULL, TO_ALL );

    multi_hit ( ch, victim, TYPE_UNDEFINED );

    return TRUE;
}

bool spec_dog_pee ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA *poo;

    if ( !IS_AWAKE ( ch ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim != ch && can_see ( ch, victim ) && number_bits ( 1 ) == 0
             && !IS_NPC ( victim ) )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    switch ( number_bits ( 5 ) )
    {
        case 0:
            return FALSE;

        case 1:
            return FALSE;

        case 2:
            act ( "$n lifts $s hind leg, and pees on $N's feet.", ch, NULL,
                  victim, TO_NOTVICT );
            act ( "$n lifts $s hind leg, and pees on your feet.", ch, NULL,
                  victim, TO_VICT );
            return TRUE;
	case 3:
	   return FALSE;

	case 4:
            act ( "$n starts moaning loudly and craps on $N's feet.", ch, NULL,
                  victim, TO_NOTVICT );
            act ( "$n starts moaning loudly and craps on your feet.", ch, NULL,
                  victim, TO_VICT );
	    poo = create_object(get_obj_index(OBJ_VNUM_FIDO_POO), 0);
	    poo->timer = 20;
	    obj_to_room(poo,victim->in_room);
            return TRUE;	
    }

    return FALSE;
}

bool spec_nasty ( CHAR_DATA * ch )
{
    CHAR_DATA *victim, *v_next;
    long gold;

    if ( !IS_AWAKE ( ch ) )
    {
        return FALSE;
    }

    if ( ch->position != POS_FIGHTING )
    {
        for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
        {
            v_next = victim->next_in_room;
            if ( !IS_NPC ( victim ) && ( victim->level > ch->level ) &&
                 ( victim->level < ch->level + 10 ) )
            {
//                do_backstab ( ch, victim->name );
                if ( ch->position != POS_FIGHTING )
                    do_murder ( ch, victim->name );
                /* should steal some coins right away? :) */
                return TRUE;
            }
        }
        return FALSE;           /*  No one to attack */
    }

    /* okay, we must be fighting.... steal some coins and flee */
    if ( ( victim = ch->fighting ) == NULL )
        return FALSE;           /* let's be paranoid.... */

    switch ( number_bits ( 2 ) )
    {
        case 0:
            act ( "$n rips apart your coin purse, spilling your gold!", ch,
                  NULL, victim, TO_VICT );
            act ( "You slash apart $N's coin purse and gather his gold.", ch,
                  NULL, victim, TO_CHAR );
            act ( "$N's coin purse is ripped apart!", ch, NULL, victim,
                  TO_NOTVICT );
            gold = victim->gold / 10;   /* steal 10% of his gold */
            victim->gold -= gold;
            ch->gold += gold;
            return TRUE;

        case 1:
            do_flee ( ch, "" );
            return TRUE;

        default:
            return FALSE;
    }
}


void NPC_set_stance( CHAR_DATA *ch )
{
	int i;
	if (ch->stance[9]== 100)
	{
		return;
	}

	for (i=1;i<11;i++)
	{
		ch->stance[i]=100;
	}

	return;
}


/*
 * Core procedure for dragons.
 */
bool dragon ( CHAR_DATA * ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 3 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( spell_name ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

/*
 * Special procedures for mobiles.
 */
bool spec_breath_any ( CHAR_DATA * ch )
{
    if ( ch->position != POS_FIGHTING )
        return FALSE;

    switch ( number_bits ( 3 ) )
    {
        case 0:
            return spec_breath_fire ( ch );
        case 1:
        case 2:
            return spec_breath_lightning ( ch );
        case 3:
            return spec_breath_gas ( ch );
        case 4:
            return spec_breath_acid ( ch );
        case 5:
        case 6:
        case 7:
            return spec_breath_frost ( ch );
    }

    return FALSE;
}

bool spec_breath_acid ( CHAR_DATA * ch )
{
    return dragon ( ch, "acid breath" );
}

bool spec_breath_fire ( CHAR_DATA * ch )
{
    return dragon ( ch, "fire breath" );
}

bool spec_breath_frost ( CHAR_DATA * ch )
{
    return dragon ( ch, "frost breath" );
}

bool spec_breath_gas ( CHAR_DATA * ch )
{
    int sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( "gas breath" ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, NULL, TARGET_CHAR );
    return TRUE;
}

bool spec_breath_lightning ( CHAR_DATA * ch )
{
    return dragon ( ch, "lightning breath" );
}

bool spec_cast_adept ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE ( ch ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim != ch && can_see ( ch, victim ) && number_bits ( 1 ) == 0 
             && !IS_NPC ( victim ) && victim->level < 201 )
	    break;
    }

    if ( victim == NULL )
        return FALSE;

    switch ( number_bits ( 8 ) )
    {
        case 0:
            act ( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
            spell_armor ( skill_lookup ( "armor" ), ch->level, ch, victim,
                          TARGET_CHAR );
            return TRUE;

        case 1:
            act ( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
            spell_bless ( skill_lookup ( "bless" ), ch->level, ch, victim,
                          TARGET_CHAR );
            return TRUE;

        case 2:
            act ( "$n utters the words 'judicandus noselacri'.", ch, NULL,
                  NULL, TO_ROOM );
            spell_cure_blindness ( skill_lookup ( "cure blindness" ),
                                   ch->level, ch, victim, TARGET_CHAR );
            return TRUE;

        case 3:
            act ( "$n utters the words 'judicandus dies'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_cure_light ( skill_lookup ( "cure light" ), ch->level, ch,
                               victim, TARGET_CHAR );
            return TRUE;

        case 4:
            act ( "$n utters the words 'judicandus sausabru'.", ch, NULL,
                  NULL, TO_ROOM );
            spell_cure_poison ( skill_lookup ( "cure poison" ), ch->level, ch,
                                victim, TARGET_CHAR );
            return TRUE;

        case 5:
            act ( "$n utters the word 'candusima'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_refresh ( skill_lookup ( "refresh" ), ch->level, ch, victim,
                            TARGET_CHAR );
            return TRUE;

        case 6:
            act ( "$n utters the words 'judicandus eugzagz'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_cure_disease ( skill_lookup ( "cure disease" ), ch->level,
                                 ch, victim, TARGET_CHAR );
            return TRUE;

/*        case 7:
            act ( "$n utters the words 'judicandus eqtuyp'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_sanctuary ( skill_lookup ( "sanctuary" ), ch->level, ch,
                              victim, TARGET_CHAR );
            return TRUE; */

        case 7:
            act ( "$n utters the words 'qwerty'.", ch, NULL, NULL, TO_ROOM );
            spell_shield ( skill_lookup ( "shield" ), ch->level, ch, victim,
                           TARGET_CHAR );
            return TRUE;

        case 8:
            act ( "$n utters the words 'saspell'.", ch, NULL, NULL, TO_ROOM );
            spell_heal ( skill_lookup ( "heal" ), ch->level, ch, victim,
                         TARGET_CHAR );
    }

    return FALSE;
}

bool spec_cast_clan_adept ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE ( ch ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim != ch && can_see ( ch, victim ) && number_bits ( 1 ) == 0
             && !IS_NPC ( victim ) && ( victim->level < 201 ) &&
             ( victim->clan == ch->in_room->clan ) )
	break;
    }

    if ( victim == NULL )
        return FALSE;

    switch ( number_bits ( 8 ) )
    {
        case 0:
            act ( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
            spell_armor ( skill_lookup ( "armor" ), ch->level / 2, ch, victim,
                          TARGET_CHAR );
            return TRUE;

        case 1:
            act ( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
            spell_bless ( skill_lookup ( "bless" ), ch->level / 2, ch, victim,
                          TARGET_CHAR );
            return TRUE;

        case 2:
            act ( "$n utters the words 'judicandus noselacri'.", ch, NULL,
                  NULL, TO_ROOM );
            spell_cure_blindness ( skill_lookup ( "cure blindness" ),
                                   ch->level / 2, ch, victim, TARGET_CHAR );
            return TRUE;

        case 3:
            act ( "$n utters the words 'judicandus dies'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_cure_light ( skill_lookup ( "cure light" ), ch->level / 2,
                               ch, victim, TARGET_CHAR );
            return TRUE;

        case 4:
            act ( "$n utters the words 'judicandus sausabru'.", ch, NULL,
                  NULL, TO_ROOM );
            spell_cure_poison ( skill_lookup ( "cure poison" ), ch->level / 2,
                                ch, victim, TARGET_CHAR );
            return TRUE;

        case 5:
            act ( "$n utters the word 'candusima'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_refresh ( skill_lookup ( "refresh" ), ch->level / 2, ch,
                            victim, TARGET_CHAR );
            return TRUE;

        case 6:
            act ( "$n utters the words 'judicandus eugzagz'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_cure_disease ( skill_lookup ( "cure disease" ),
                                 ch->level / 2, ch, victim, TARGET_CHAR );
            return TRUE;
/*
        case 7:
            act ( "$n utters the words 'judicandus eqtuyp'.", ch, NULL, NULL,
                  TO_ROOM );
            spell_sanctuary ( skill_lookup ( "sanctuary" ), ch->level / 2, ch,
                              victim, TARGET_CHAR );
            return TRUE;
*/
        case 7:
            act ( "$n utters the words 'qwerty'.", ch, NULL, NULL, TO_ROOM );
            spell_shield ( skill_lookup ( "shield" ), ch->level / 2, ch,
                           victim, TARGET_CHAR );
            return TRUE;

        case 8:
            act ( "$n utters the words 'saspell'.", ch, NULL, NULL, TO_ROOM );
            spell_heal ( skill_lookup ( "heal" ), ch->level / 2, ch, victim,
                         TARGET_CHAR );
    }

    return FALSE;
}

bool spec_cast_cleric ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

	if (!IS_AWAKE(ch) )
	{
		do_stance(ch, "");
		return FALSE;
	}    
    
	if (ch->stunned)
	{
		do_stance(ch, "" );
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		if (ch->level <= 50)
		{
			do_stance(ch, "mongoose");
		}
		else if (ch->level <= 100)
		{
			do_stance(ch, "crab");
		}
		else if (ch->level <= 150)
		{
			do_stance(ch, "swallow");
		}
		else
		{
			do_stance(ch, "monkey");
		}
	}
	
    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    for ( ;; )
    {
        int min_level;

        switch ( number_bits ( 4 ) )
        {
            case 0:
                min_level = 0;
                spell = "blindness";
                break;
            case 1:
                min_level = 3;
                spell = "firestorm";
                break;
            case 2:
                min_level = 7;
                spell = "earthquake";
                break;
            case 3:
                min_level = 9;
                spell = "fire breath";
                break;
            case 4:
                min_level = 10;
                spell = "ray of truth";
                break;
            case 5:
                min_level = 12;
                spell = "demonfire";
                break;
            case 6:
            case 7:
                min_level = 13;
                spell = "fire breath";
                break;
            case 8:
            case 9:
            case 10:
                min_level = 15;
                spell = "acid storm";
                break;
            case 11:
                min_level = 15;
                spell = "plague";
                break;
            default:
                min_level = 16;
                spell = "dispel magic";
                break;
        }

        if ( ch->level >= min_level )
            break;
    }

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( spell ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_cast_judge ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    if ( ch->stunned )
        return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup ( spell ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_cast_mage ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

	if (!IS_AWAKE(ch) )

	{

		return FALSE;

	}

	if (ch->stunned)

	{

		do_stance(ch, "" );						/* too stunned to do anything	*/

		return FALSE;

	}

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))

	{

		NPC_set_stance(ch);

                if (ch->level <= 50)

                {

                        do_stance(ch, "mongoose");

                }

                else if (ch->level <= 100)

                {

                        do_stance(ch, "crab");

                }

                else if (ch->level <= 150)

                {

                        do_stance(ch, "swallow");

                }

                else

                {

                        do_stance(ch, "monkey");

                }

	}
    
    
    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    for ( ;; )
    {
        int min_level;

        switch ( number_bits ( 4 ) )
        {
            case 0:
                min_level = 0;
                spell = "blindness";
                break;
            case 1:
            case 2:
                min_level = 7;
                spell = "fire breath";
                break;
            case 3:
                min_level = 8;
                spell = "teleport";
                break;
            case 4:
                min_level = 11;
                spell = "meteor storm";
                break;
            case 5:
            case 6:
                min_level = 13;
                spell = "acid storm";
                break;
            case 7:
            case 8:
            case 9:
                min_level = 15;
                spell = "gas breath";
                break;
            case 10:
                min_level = 20;
                spell = "plague";
                break;
            default:
                min_level = 20;
                spell = "demonfire";
                break;
        }

        if ( ch->level >= min_level )
            break;
    }

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( spell ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_antihero_evil ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    for ( ;; )
    {
        int min_level;

        switch ( number_bits ( 4 ) )
        {
            case 0:
                min_level = 0;
                spell = "meteor storm";
                break;
            case 1:
                min_level = 3;
                spell = "call darkness";
                break;
            case 2:
                min_level = 7;
                spell = "vampire blast";
                break;
            case 3:
                min_level = 8;
                spell = "blood blast";
                break;
            case 4:
                min_level = 11;
                spell = "leech";
                break;
            case 5:
                min_level = 12;
                spell = "firestorm";
                break;
            case 6:
                min_level = 13;
                spell = "meteor storm";
                break;
            case 7:
                min_level = 14;
                spell = "acid rain";
                break;
            case 8:
                min_level = 15;
                spell = "call darkness";
                break;
            case 9:
                min_level = 16;
                spell = "meteor storm";
                break;
            case 10:
                min_level = 18;
                spell = "acid rain";
                break;
            default:
                min_level = 20;
                spell = "fire rain";
                break;
        }

        if ( ch->level >= min_level )
            break;
    }

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( spell ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_cast_undead ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

	if (!IS_AWAKE(ch) )

	{

		return FALSE;

	}



	/*SV*/

	if (ch->stunned)

	{

		do_stance(ch, "" );						/* too stunned to do anything	*/

		return FALSE;

	}



	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))

	{

		NPC_set_stance(ch);

                if (ch->level <= 50)

                {

                        do_stance(ch, "viper");

                }

                else if (ch->level <= 100)

                {

                        do_stance(ch, "bull");

                }

                else if (ch->level <= 150)

                {

                        do_stance(ch, "tiger");

                }

                else

                {

                        do_stance(ch, "monkey");

                }

	}
    
    
    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits ( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    for ( ;; )
    {
        int min_level;

        switch ( number_bits ( 4 ) )
        {
            case 0:
                min_level = 0;
                spell = "curse";
                break;
            case 1:
                min_level = 3;
                spell = "curse";
                break;
            case 2:
                min_level = 6;
                spell = "curse";
                break;
            case 3:
                min_level = 9;
                spell = "blindness";
                break;
            case 4:
                min_level = 12;
                spell = "poison";
                break;
            case 5:
                min_level = 15;
                spell = "energy drain";
                break;
            case 6:
                min_level = 18;
                spell = "harm";
                break;
            case 7:
                min_level = 21;
                spell = "teleport";
                break;
            case 8:
                min_level = 20;
                spell = "plague";
                break;
            default:
                min_level = 18;
                spell = "harm";
                break;
        }

        if ( ch->level >= min_level )
            break;
    }

    if ( ch->stunned )
        return FALSE;

    if ( ( sn = skill_lookup ( spell ) ) < 0 )
        return FALSE;
    ( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_executioner ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if ( !IS_AWAKE ( ch ) || ch->fighting != NULL )
        return FALSE;

    crime = "";
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_TWIT ) &&
             can_see ( ch, victim ) )
        {
            crime = "TWIT";
            break;
        }
    }

    if ( victim == NULL )
        return FALSE;

    sprintf ( buf, "{a%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!{x",
              victim->name, crime );
    REMOVE_BIT ( ch->comm, COMM_NOSHOUT );
    do_say ( ch, buf );
/*    if ( chance ( 60 ) )
    do_backstab ( ch, victim->name ); */
    multi_hit ( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_fido ( CHAR_DATA * ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE ( ch ) )
        return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
        c_next = corpse->next_content;
        if ( corpse->item_type != ITEM_CORPSE_NPC )
            continue;

        act ( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
        for ( obj = corpse->contains; obj; obj = obj_next )
        {
            obj_next = obj->next_content;
            obj_from_obj ( obj );
            obj_to_room ( obj, ch->in_room );
        }
        extract_obj ( corpse );
        return TRUE;
    }

    return FALSE;
}

bool spec_guard ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;

    if ( !IS_AWAKE ( ch ) || ch->fighting != NULL )
        return FALSE;

    max_evil = 300;
    ech = NULL;
    crime = "";

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_TWIT ) &&
             can_see ( ch, victim ) )
        {
            crime = "TWIT";
            break;
        }

        if ( victim->fighting != NULL && victim->fighting != ch &&
             victim->alignment < max_evil )
        {
            max_evil = victim->alignment;
            ech = victim;
        }
    }

    if ( victim != NULL )
    {
        sprintf ( buf, "{a%s is a %s!  MUST KILL!!  HARRRRRR!!{x",
                  victim->name, crime );
        REMOVE_BIT ( ch->comm, COMM_NOSHOUT );
        do_yell ( ch, buf );
/*	if ( chance ( 60 ) )
	do_backstab ( ch, victim->name ); */
        multi_hit ( ch, victim, TYPE_UNDEFINED );
        return TRUE;
    }

    if ( ech != NULL )
    {
        act ( "$n screams '{aWHAT THE F-WORD ARE YOU DOING!!  HARRRRAWWRWR!!{x", ch, NULL,
              NULL, TO_ROOM );
/*	if ( chance ( 60 ) )
	do_backstab ( ch, victim->name );*/
        multi_hit ( ch, ech, TYPE_UNDEFINED );
        return TRUE;
    }

    return FALSE;
}

bool spec_janitor ( CHAR_DATA * ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE ( ch ) )
        return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
        trash_next = trash->next_content;
        if ( !IS_SET ( trash->wear_flags, ITEM_TAKE ) ||
             !can_loot ( ch, trash ) )
            continue;
        if ( trash->item_type == ITEM_DRINK_CON ||
             trash->item_type == ITEM_TRASH || trash->cost < 10 )
        {
            act ( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
            obj_from_room ( trash );
            obj_to_char ( trash, ch );
            return TRUE;
        }
    }

    return FALSE;
}

bool spec_mayor ( CHAR_DATA * ch )
{
    static const char open_path[] =
        "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
        "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
    static int pos;
    static bool move;

    if ( !move )
    {
        if ( time_info.hour == 6 )
        {
            path = open_path;
            move = TRUE;
            pos = 0;
        }

        if ( time_info.hour == 20 )
        {
            path = close_path;
            move = TRUE;
            pos = 0;
        }
    }

    if ( ch->fighting != NULL )
        return spec_cast_mage ( ch );
    if ( !move || ch->position < POS_SLEEPING )
        return FALSE;

    switch ( path[pos] )
    {
        case '0':
        case '1':
        case '2':
        case '3':
            move_char ( ch, path[pos] - '0', FALSE, FALSE );
            break;

        case 'W':
            ch->position = POS_STANDING;
            act ( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
            break;

        case 'S':
            ch->position = POS_SLEEPING;
            act ( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
            break;

        case 'a':
            act ( "{x$n says '{aHello Honey!{x'", ch, NULL, NULL, TO_ROOM );
            break;

        case 'b':
            act ( "{x$n says '{aWhat a view!  I must do something about that dump!{x'", ch, NULL, NULL, TO_ROOM );
            break;

        case 'c':
            act ( "{x$n says '{aVandals!  Youngsters have no respect for anything!{x'", ch, NULL, NULL, TO_ROOM );
            break;

        case 'd':
            act ( "{x$n says '{aGood day, citizens!{x'", ch, NULL, NULL,
                  TO_ROOM );
            break;

        case 'e':
            act ( "{x$n says '{aI hereby declare the city of SilverXire open!{x'", ch, NULL, NULL, TO_ROOM );
            break;

        case 'E':
            act ( "{x$n says '{aI hereby declare the city of SilverXire closed!{x'", ch, NULL, NULL, TO_ROOM );
            break;

        case 'O':
            do_open ( ch, "gate" );
            break;

        case 'C':
            do_close ( ch, "gate" );
            break;

        case '.':
            move = FALSE;
            break;
    }

    pos++;
    return FALSE;
}

bool spec_poison ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING || ( victim = ch->fighting ) == NULL ||
         number_percent (  ) > 2 * ch->level )
        return FALSE;

    if ( ch->stunned )
        return FALSE;

    act ( "You bite $N!", ch, NULL, victim, TO_CHAR );
    act ( "$n bites $N!", ch, NULL, victim, TO_NOTVICT );
    act ( "$n bites you!", ch, NULL, victim, TO_VICT );
    spell_poison ( gsn_poison, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_thief ( CHAR_DATA * ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    long gold, silver;

    if ( ch->position != POS_STANDING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( IS_NPC ( victim ) || victim->level >= LEVEL_IMMORTAL ||
             number_bits ( 5 ) != 0 || !can_see ( ch, victim ) )
            continue;

        if ( IS_AWAKE ( victim ) && number_range ( 0, ch->level ) == 0 )
        {
            act ( "You discover $n's hands in your {zwallet!{x", ch, NULL,
                  victim, TO_VICT );
            act ( "$N discovers $n's hands in $S {zwallet!{x", ch, NULL,
                  victim, TO_NOTVICT );
            return TRUE;
        }
        else
        {
            gold =
                victim->gold * UMIN ( number_range ( 1, 20 ),
                                      ch->level / 2 ) / 100;
            gold = UMIN ( gold, ch->level * ch->level * 10 );
            ch->gold += gold;
            victim->gold -= gold;
            silver =
                victim->silver * UMIN ( number_range ( 1, 20 ),
                                        ch->level / 2 ) / 100;
            silver = UMIN ( silver, ch->level * ch->level * 25 );
            ch->silver += silver;
            victim->silver -= silver;
            return TRUE;
        }
    }

    return FALSE;
}

bool spec_questmaster ( CHAR_DATA * ch )
{
    if ( ch->fighting != NULL )
        return spec_cast_mage ( ch );
    return FALSE;
}

bool spec_registar ( CHAR_DATA * ch )
{
    if ( ch->fighting != NULL )
        return spec_cast_mage ( ch );
    return FALSE;
}

void taunt( CHAR_DATA *ch, CHAR_DATA *victim )
{
	char buf [MSL];
	switch( number_range( 1, 8 ) )
	{
		case 1:
			strcpy(buf,"Fool, do you think you stand a chance against me?");
			do_say(ch,buf);
			break;
		case 2:
			if (IS_NPC(victim))
			{
				sprintf(buf,"I shall destroy you utterly for this, %s",victim->short_descr);
			}
			else
			{
				sprintf(buf,"I shall destroy you utterly for this, %s",victim->name);
			}
			do_say(ch,buf);
			break;
		case 3:
			if (IS_NPC(victim))
			{
				sprintf(buf,"%s shall pay for their arrogance!",victim->short_descr);
			}
			else
			{
				sprintf(buf,"%s shall pay for their arrogance!",victim->name);
			}
			do_shout(ch,buf);
			break;
		case 4:
			if (IS_NPC(victim))
			{
				sprintf(buf,"This fight truly shall be your last, %s!",victim->short_descr);
			}
			else
			{
				sprintf(buf,"This fight truly shall be your last, %s!",victim->name);
			}
			do_say(ch,buf);
			break;
		case 5:
			if (ch->hit < (ch->max_hit * 0.1) )
			{
				if (IS_NPC(victim))
				{
					sprintf(buf,"You may defeat me this time %s, but beware!",victim->short_descr);
				}
				else
				{
					sprintf(buf,"You may defeat me this time %s, but beware!",victim->name);
				}
			}
			else if (victim->hit < (victim->max_hit * 0.1) )	/* Edhelan guesses at author's intent */
			{
				if (IS_NPC(victim))
				{
					sprintf(buf,"I hear the horsemen coming for you, %s!",victim->short_descr);
				}
				else
				{
					sprintf(buf,"I hear the horsemen coming for you, %s!",victim->name);
				}
			}
			do_say(ch,buf);
			break;
	}
}

void defensive_spell( CHAR_DATA *ch )
{
//  char buf[MSL];
	int did_something = 0;			  /* a little less deterministic on what to heal next */
	while (!did_something)			  /* these are purely defensive, suitable for doing   */
	{								   /* even in private */
		switch (number_range(1,12))
		{
			case 0:
				if ((did_something = IS_AFFECTED(ch, AFF_FAERIE_FIRE)))
				{
					act("$n's eyes flare bright {Mpink{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_dispel_magic( skill_lookup("dispel magic" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 1:
				if ((did_something = IS_AFFECTED(ch, AFF_POISON)))
				{
					act("$n's eyes flare bright {bblue{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_cure_poison( skill_lookup("cure poison" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 2:
				if ((did_something = IS_AFFECTED(ch, AFF_BLIND)))
				{
					act("$n's eyes flare bright {bblue{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_cure_blindness( skill_lookup("cure blindness" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 3:
				if ((did_something = IS_AFFECTED(ch, AFF_CURSE)))
				{
					act("$n's eyes flare bright {ggreen{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_remove_curse( skill_lookup("remove curse" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 4:
				if ((did_something = !IS_AFFECTED(ch, SHD_SANCTUARY)))
				{
					act("$n's eyes flare bright {7white{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_sanctuary( skill_lookup("sanctuary" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 5:
				if ((did_something = !is_affected(ch, skill_lookup("bless")) ))
				{
					act("$n's eyes flare bright {Bblue{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_bless( skill_lookup("bless" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 6:
				if ((did_something = !is_affected(ch, skill_lookup("shield")) ))
				{
					act("$n's eyes flare bright {Ccyan{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_shield( skill_lookup("shield" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 7:
				if ((did_something = !is_affected(ch, skill_lookup("armor")) ))
				{
					act("$n's eyes flare bright {&Silver{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_armor( skill_lookup("armor" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 8:
				if ((did_something = !is_affected(ch, skill_lookup("stone skin")) ))
				{
					act("$n's eyes flare bright {ccyan{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_stone_skin( skill_lookup("stone skin" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
			case 9:
				did_something = 1; break;				/* better luck next round */
			default:
				if ((did_something = (ch->hit < (ch->max_hit * 0.5) )))
				{
					act("$n's eyes flare bright {ccyan{x for a moment.", ch, NULL, NULL, TO_ROOM);
					spell_heal( skill_lookup("heal" ), ch->level, ch, ch, TAR_CHAR_SELF);
				}
				break;
		}
	}
}

bool spec_prog1( CHAR_DATA *ch )
{
	CHAR_DATA *victim;
	CHAR_DATA *v_next;
	char *spell;
	int sn;

	if ( !IS_AWAKE(ch) )		/* too busy sleeping */
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		return FALSE;
	}

	if ( ch->fighting )		 /* if we are fighting, don't pick any new fights */
	{
		for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
		{
			v_next = victim->next_in_room;
			if ( (victim->fighting == ch) && (number_bits( 2 ) == 0) )
			{
				break;		  /* pick one of our current assailants */
			}
		}
	}
	else						/* if we are not fighting, pick someone to attack */
	{
		for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
		{
			v_next = victim->next_in_room;

			if  ( (victim != ch) && can_see( ch, victim )		 /* I see him */
			&&	!IS_NPC(victim)									/* only attack PC's */
			&&	  (victim->level > UMIN(145, ch->level-15) )	/* and he's not a wimp */
			&&  ( (IS_GOOD(ch) ? 2 : IS_EVIL(ch) ? 0 : 1 ) !=	/* unmatched alignment */
				  (IS_GOOD(victim) ? 2 : IS_EVIL(victim) ? 0 : 1 ))
			&&	(number_bits( 1 ) == 0) )
			{
				break;
			}
		}
	}

	defensive_spell(ch);		/* improve oneself, in or out of combat */

	if ( victim == NULL )	   /* nobody special to attack, this time */
	{
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);	 /* SV */
		do_stance(ch,"dragon");
	}

	if (victim->fighting == NULL)		/* okay.. here we get to start the fight */
	{
		do_backstab(ch, victim->name);
		if (victim->fighting == NULL)	/* and if the backstab didn't work */
		{
			do_murder(ch, victim->name);		/* just plain ckill them*/
		}
	}
	else							/* otherwise, let's have fun */
	{							/* with circle, but not twice */
			/*		do_circle(ch,"");	*/			/* on the same target */
								/* apparently vonBlazen doesn't
								 * have this skill, so I'll fake it */
		multi_hit(ch, ch->fighting, gsn_circle);
		if (victim != ch->fighting)			/* maybe the mobs don't have the skill! */
		{
			multi_hit(ch, victim, gsn_circle);
		}
	}

	switch ( number_bits( 4 ) )	 /* 16 options -- most do nothing (these are rare) */
	{
		case 0:
			taunt(ch, victim);	  /* moved to another place for convenience */
			break;	  /* and now we no both sides are fighting  */
		case 1:
			switch( number_range(1,10) )		/* funky combination platter, left as is  */
			{
				case 1:
					do_disarm(ch,"");
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					act("$n's eyes flare bright {Rred{x for a moment.",ch,NULL,NULL,TO_ROOM);
					spell_dispel_magic(skill_lookup("dispel magic"),ch->level,ch,victim,TARGET_CHAR);
					break;
				case 6:
					do_impale(ch,"");
					break;
				case 7:
				case 8:
					act("$n's eyes flare bright {Rred{x for a moment.",ch,NULL,NULL,TO_ROOM);
					spell_harm( skill_lookup( "harm" ), ch->level, ch, victim, TARGET_CHAR );
					break;
			}
		case 2:	 /* occasionally boost the odds */
			if (!is_affected(ch,skill_lookup("frenzy")) )
			{
				act("$n's eyes flare bright {yyellow{x for a moment.", ch,NULL,NULL,TO_ROOM);
				spell_frenzy( skill_lookup( "frenzy" ), ch->level, ch, ch, TAR_CHAR_SELF );
			}
			else if (!IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
			{
				act("$n's eyes flare bright {Mpink{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_faerie_fire( skill_lookup( "faerie fire" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (!IS_AFFECTED(victim, AFF_BLIND) )
			{
				act("$n's eyes flare dull {8grey{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_blindness( skill_lookup( "blindness" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (!IS_AFFECTED(victim, AFF_CURSE) )
			{
				act("$n's eyes flare {8black{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_curse( skill_lookup( "curse" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (ch->hit < (ch->max_hit * 0.1) )
			{
				act("$n's eyes flare bright {Ggreen{x for a moment.", ch,NULL,NULL,TO_ROOM);
				spell_teleport( skill_lookup( "teleport" ), ch->level, ch, victim, TARGET_CHAR );
			}
	}
	// end of Rare switch

	for ( ;; )	  /* this spell selection left untouched */
	{
		int min_level;
		switch ( number_bits( 4 ) )
		{
			case  0: min_level =  0; spell = "blindness";	  break;
			case  1: min_level =  3; spell = "meteor storm";	break;
			case  2: min_level =  7; spell = "faerie fire";		 break;
			case  3: min_level =  8; spell = "teleport";	   break;
			case  4: min_level = 11; spell = "weaken";   break;
			case  5: min_level = 12; spell = "earthrise";	 break;
			case  6: min_level = 13; spell = "call darkness";   break;
			case  7: min_level = 90; spell = "gas breath";	 break;
			case  8: min_level = 90; spell = "fire breath";	break;
			case  9: min_level = 15; spell = "firestorm";	   break;
			case 10: min_level = 20; spell = "plague";		   break;
			default: min_level = 20; spell = "acid blast";	 break;
		}

		if ( ch->level >= min_level )	   /* try a spell we can manage */
		{
			break;
		}
	}

	if ( ( sn = skill_lookup( spell ) ) < 0 )	   /* aw, drat!  we don't know this one! */
	{
		return FALSE;
	}

	(*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
}


/*  Edhelan's note:  this only differs in the default attack spell selection		*/
bool spec_prog2( CHAR_DATA *ch )
{
	CHAR_DATA *victim;
	CHAR_DATA *v_next;
	char *spell;
	int sn;

	if ( !IS_AWAKE(ch) )		/* too busy sleeping			*/
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );	 /* too stunned to do anything	*/
		return FALSE;
	}

	if ( ch->fighting )	 /* if we are fighting, don't pick any new fights */
	{
		for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
		{
			v_next = victim->next_in_room;

			if ( victim->fighting == ch && number_bits( 2 ) == 0 )
			{
				break;	  /* pick one of our current assailants */
			}
		}
	}
	else		/* if we are not fighting, pick someone to attack */
	{
		for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
		{
			v_next = victim->next_in_room;

			if  ( (victim != ch) && can_see( ch, victim )			/* I see him */
			&&	!IS_NPC(victim)						/* only attack PC's */
			&&	  (victim->level > UMIN(145, ch->level-15) )		/* and he's not a wimp */
			&&  ( (IS_GOOD(ch) ? 2 : IS_EVIL(ch) ? 0 : 1 ) !=		/* unmatched alignment */
				  (IS_GOOD(victim) ? 2 : IS_EVIL(victim) ? 0 : 1 ))
			&& ( number_bits( 1 ) == 0) )
			{
				break;
			}
		}
	}

	if ( victim == NULL )
	{
		return FALSE;
	}

	defensive_spell(ch);		/* improve oneself, in or out of combat */

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch); /*SV*/
		do_stance(ch, "dragon");
	}


	if (victim->fighting == NULL)		/* okay.. here we get to start the fight */
	{
		do_backstab(ch, victim->name);
		if (victim->fighting == NULL)		/* and if the backstab didn't work */
		{
			do_murder(ch, victim->name);	/* just plain ckill them		   */
		}
	}
	else								/* otherwise, let's have fun */
	{									/* with circle, but not twice */
										/* apparently some mobs don't have
										 * the skill, so I'll fake it for now */
/*		do_circle(ch,"");	*/			/* on the same target */
		multi_hit(ch, ch->fighting, gsn_circle);
		if (victim != ch->fighting)			/* maybe the mobs don't have the skill! */
		{
			multi_hit(ch, victim, gsn_circle);
		}
	}


	switch ( number_bits( 4 ) )		 /* 16 options -- most do nothing (these are rare) */
	{
		case 0:
			taunt(ch, victim);				/* moved to another place for convenience */
			break;							/* and now we no both sides are fighting  */
		case 1:
			switch( number_range(1,10) )		/* funky combination platter, left as is  */
			{
				case 1:
					do_disarm(ch,"");
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					act("$n's eyes flare bright {Rred{x for a moment.",ch,NULL,NULL,TO_ROOM);
					spell_dispel_magic(skill_lookup("dispel magic"),ch->level,ch,victim,TARGET_CHAR);
					break;
				case 6:
					do_impale(ch,"");
					break;
				case 7:
				case 8:
					act("$n's eyes flare bright {Rred{x for a moment.",ch,NULL,NULL,TO_ROOM);
					spell_harm( skill_lookup( "harm" ), ch->level, ch, victim, TARGET_CHAR );
					break;
			}
		case 2:		/* occasionally boost the odds */
			if (!is_affected(ch,skill_lookup("frenzy")) )
			{
				act("$n's eyes flare bright {yyellow{x for a moment.", ch,NULL,NULL,TO_ROOM);
				spell_frenzy( skill_lookup( "frenzy" ), ch->level, ch, ch, TAR_CHAR_SELF );
			}
			else if (!IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
			{
				act("$n's eyes flare bright {Mpink{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_faerie_fire( skill_lookup( "faerie fire" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (!IS_AFFECTED(victim, AFF_BLIND) )
			{
				act("$n's eyes flare dull {8grey{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_blindness( skill_lookup( "blindness" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (!IS_AFFECTED(victim, AFF_CURSE) )
			{
				act("$n's eyes flare {8black{x for a moment.",ch,NULL,NULL,TO_ROOM);
				spell_curse( skill_lookup( "curse" ), ch->level, ch, victim, TARGET_CHAR );
			}
			else if (ch->hit < (ch->max_hit * 0.1) )
			{
				act("$n's eyes flare bright {Ggreen{x for a moment.", ch,NULL,NULL,TO_ROOM);
				spell_teleport( skill_lookup( "teleport" ), ch->level, ch, victim, TARGET_CHAR );
			}
	}
	// end of Rare switch


	for ( ;; )		/* this spell selection left untouched */
	{
		int min_level;

		switch ( number_bits( 4 ) )
		{
			case  0: min_level =  0; spell = "blindness";		break;
			case  1: min_level =  3; spell = "chill touch";		break;
			case  2: min_level =  7; spell = "weaken";			break;
			case  3: min_level =  8; spell = "teleport";		break;
			case  4: min_level = 11; spell = "colour spray";	break;
			case  5: min_level = 12; spell = "change sex";		break;
			case  6: min_level = 13; spell = "energy drain";	break;
			case  7: min_level = 90; spell = "gas breath";		break;
			case  8: min_level = 90; spell = "fire breath";		break;
			case  9: min_level = 15; spell = "fireball";		break;
			case 10: min_level = 20; spell = "plague";			break;
			default: min_level = 20; spell = "lightning bolt";	break;
		}

		if ( ch->level >= min_level )			/* try a spell we can manage */
		{
			break;
		}
	}

	if ( ( sn = skill_lookup( spell ) ) < 0 )	/* aw, drat!  we don't know this one! */
	{
		return FALSE;
	}

	(*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
}

bool spec_impale( CHAR_DATA *ch )
{
	char buf[MSL];
	CHAR_DATA *victim;
	CHAR_DATA *v_next;

	if ( ch->fighting == NULL )
	{
		return FALSE;
	}

	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );						/* too stunned to do anything	*/
		return FALSE;
	}

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		if ( victim->position != POS_FIGHTING )
		{
			return FALSE;
		}

		v_next = victim->next_in_room;
		if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
		&& !IS_NPC(victim))
		{
			break;
		}
	}

	if ( victim == NULL )
	{
		return FALSE;
	}

	switch ( number_bits( 1 ) )
	{
		case 0:
			if (ch->position == POS_FIGHTING ) /*SV*/
			{
				do_stance(ch,"");
				if (IS_STANCE(victim, STANCE_NONE))
				{
					do_stance(ch,"bull");
					return TRUE;
				}
				else if (IS_STANCE(victim,  STANCE_VIPER ))
				{
					do_stance(ch,"crab");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_CRANE ) )
				{
					do_stance(ch,"viper");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MONGOOSE) )
				{
					do_stance(ch,"bull");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_BULL ))
				{
					do_stance(ch,"mongoose");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_CRAB ))
				{
					do_stance(ch,"bull");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MANTIS)  )
				{
					do_stance(ch,"tiger");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_TIGER ))
				{
					do_stance(ch,"dragon");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_DRAGON) )
				{
					do_stance(ch,"monkey");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MONKEY) )
				{
					do_stance(ch,"monkey");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_SWALLOW) )
				{
					do_stance(ch,"tiger");
					return TRUE;
				}
				return FALSE;
			}
	}
	switch ( number_range( 1,5 ) )
	{
		case  1: sprintf( buf, "{!Impaling {x is the true end...."); break;
		case  2: sprintf( buf, "{CTime to feel impaled{x ...."); break;
		default: return FALSE;
	}
	do_say( ch, buf );
	do_impale(ch,"");
	return FALSE;
}

bool spec_circle( CHAR_DATA *ch )
{
	char buf[MSL];
	CHAR_DATA *victim;
	CHAR_DATA *v_next;

	if ( ch->fighting == NULL )
	{
		return FALSE;
	}

	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );						/* too stunned to do anything	*/
		return FALSE;
	}

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		/*  An obvious bug.  Don't let innocent bystanders cancel this ability!
			if ( victim->position != POS_FIGHTING ) { return FALSE; }  */
		v_next = victim->next_in_room;
		if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
		&& (victim->position == POS_FIGHTING) && !IS_NPC(victim))
		{
			break;
		}
	}


	if ( victim == NULL )
	{
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
	}


	switch ( number_bits( 1 ) )
	{
		case 0:
			if (ch->position == POS_FIGHTING ) /*SV*/
			{
				do_stance(ch,"");

				if (IS_STANCE(victim, STANCE_NONE))
				{
					do_stance(ch,"bull");
					return TRUE;
				}
				else if (IS_STANCE(victim,  STANCE_VIPER ))
				{
					do_stance(ch,"crab");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_CRANE ) )
				{
					do_stance(ch,"viper");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MONGOOSE) )
				{
					do_stance(ch,"viper");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_BULL ))
				{
					do_stance(ch,"mongoose");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_CRAB ))
				{
					do_stance(ch,"bull");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MANTIS)  )
				{
					do_stance(ch,"tiger");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_TIGER ))
				{
					do_stance(ch,"dragon");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_DRAGON) )
				{
					do_stance(ch,"monkey");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_MONKEY) )
				{
					do_stance(ch,"monkey");
					return TRUE;
				}
				else if (IS_STANCE(victim, STANCE_SWALLOW) )
				{
					do_stance(ch,"tiger");
					return TRUE;
				}
				return FALSE;
			}
	}
	switch ( number_range( 1,9 ) )
	{
		case  1: sprintf( buf, "{RDeath{x is the true end...."); break;
		case  2: sprintf( buf, "{CTime{x to die...."); break;
		case  3: sprintf ( buf, "{rR{Dhaela{rr{x wants to eat your {rbones{x... "); break;
		case  4: sprintf( buf, "{GWelcome{x to your nightmare...."); break;
		case  5: sprintf( buf, "A {Rsacrifice to {Ythe gods{x...."); break;
		case  6: sprintf( buf, "The {Mjustice {xof {MS{mina {xwill not {RSAVE {xyou now...."); break;
		default: return FALSE;
	}
	do_say( ch, buf );
	do_circle(ch,"");
	multi_hit( ch, victim ,gsn_circle );
	return TRUE;
}

bool spec_aggressive_stance( CHAR_DATA *ch )
{
	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch,"");

		if (ch->level <= 25)
		{
			do_stance(ch, "viper");
		}
		else if (ch->level <= 75)
		{
			do_stance(ch, "bull");
		}
		else if (ch->level <= 125)
		{
			do_stance(ch, "mantis");
		}
		else
		{
			do_stance(ch, "tiger");
		}
	}
	return TRUE;
}

bool spec_defensive_stance( CHAR_DATA *ch )
{
	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NONE) || IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch,"");

		if (ch->level <= 25)
		{
			do_stance(ch, "crab");
		}
		else if (ch->level <= 75)
		{
			do_stance(ch, "mongoose");
		}
		else if (ch->level <= 125)
		{
			do_stance(ch, "swallow");
		}
		else
		{
			do_stance(ch, "monkey");
		}
	}
	return TRUE;
}

bool spec_all_stances( CHAR_DATA *ch )
{
	CHAR_DATA *victim;
	CHAR_DATA *v_next;

	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		/*  An obvious bug.  Don't let innocent bystanders cancel this ability!
			if ( victim->position != POS_FIGHTING ) { return FALSE; }  */
		v_next = victim->next_in_room;
		if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
		&& (victim->position == POS_FIGHTING) && !IS_NPC(victim))
		{
			break;
		}
	}

	if ((ch->fighting==NULL))
	{
		if (IS_STANCE(ch, STANCE_NORMAL))
		{
			NPC_set_stance(ch);
			do_stance(ch,"");
		}
		if (IS_STANCE(ch, STANCE_NONE))
		{
			do_stance(ch, "dragon");
		}
		return FALSE;
	}

	if ( victim == NULL )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );
		return FALSE;
	}

	do_stance(ch,"");

	if (IS_STANCE(victim, STANCE_NONE))
	{
		do_stance(ch,"bull");
	}
	else if (IS_STANCE(victim,  STANCE_VIPER ))
	{
		do_stance(ch,"crab");
	}
	else if (IS_STANCE(victim, STANCE_CRANE ) )
	{
		do_stance(ch,"viper");
	}
	else if (IS_STANCE(victim, STANCE_MONGOOSE) )
	{
		do_stance(ch,"viper");
	}
	else if (IS_STANCE(victim, STANCE_BULL ))
	{
		do_stance(ch,"mongoose");
	}
	else if (IS_STANCE(victim, STANCE_CRAB ))
	{
		do_stance(ch,"bull");
	}
	else if (IS_STANCE(victim, STANCE_MANTIS)  )
	{
		do_stance(ch,"tiger");
	}
	else if (IS_STANCE(victim, STANCE_TIGER ))
	{
		do_stance(ch,"dragon");
	}
	else if (IS_STANCE(victim, STANCE_DRAGON) )
	{
		do_stance(ch,"swallow");
	}
	else if (IS_STANCE(victim, STANCE_MONKEY) )
	{
		do_stance(ch,"monkey");
	}
	else if (IS_STANCE(victim, STANCE_SWALLOW) )
	{		
		do_stance(ch,"tiger");
	}

	return TRUE;
}

bool spec_basic_stances( CHAR_DATA *ch )
{
	CHAR_DATA *victim;
	CHAR_DATA *v_next;

	if (!IS_AWAKE(ch) )
	{
		return FALSE;
	}

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
		/*  An obvious bug.  Don't let innocent bystanders cancel this ability!
			if ( victim->position != POS_FIGHTING ) { return FALSE; }  */
		v_next = victim->next_in_room;
		if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
		&& (victim->position == POS_FIGHTING) && !IS_NPC(victim))
		{
			break;
		}
	}

	if ((ch->fighting==NULL))
	{
		if (IS_STANCE(ch, STANCE_NORMAL))
		{
			NPC_set_stance(ch);
			do_stance(ch,"");
		}
		if (IS_STANCE(ch, STANCE_NONE))
		{
			do_stance(ch, "bull");
		}
		return FALSE;
	}

	if ( victim == NULL )
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "" );
		return FALSE;
	}

	do_stance(ch,"");

	if (IS_STANCE(victim, STANCE_NONE))
	{
		do_stance(ch,"bull");
	}
	else if (IS_STANCE(victim,  STANCE_VIPER ))
	{
		do_stance(ch,"crab");
	}
	else if (IS_STANCE(victim, STANCE_CRANE ) )
	{
		do_stance(ch,"viper");
	}
	else if (IS_STANCE(victim, STANCE_MONGOOSE) )
	{
		do_stance(ch,"viper");
	}
	else if (IS_STANCE(victim, STANCE_BULL ))
	{
		do_stance(ch,"mongoose");
	}
	else if (IS_STANCE(victim, STANCE_CRAB ))
	{
		do_stance(ch,"bull");
	}
	else if (IS_STANCE(victim, STANCE_MANTIS)  )
	{
		do_stance(ch,"crab");
	}
	else if (IS_STANCE(victim, STANCE_TIGER ))
	{
		do_stance(ch,"mongoose");
	}
	else if (IS_STANCE(victim, STANCE_DRAGON) )
	{
		do_stance(ch,"bull");
	}
	else if (IS_STANCE(victim, STANCE_MONKEY) )
	{
		do_stance(ch,"crane");
	}
	else if (IS_STANCE(victim, STANCE_SWALLOW) )
	{
		do_stance(ch,"viper");
	}

	return TRUE;
}

bool spec_advanced_stances( CHAR_DATA *ch )
{        
	CHAR_DATA *victim;
        CHAR_DATA *v_next;

        if (!IS_AWAKE(ch) )
        {
                return FALSE;
        }

        for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
        {
                /*  An obvious bug.  Don't let innocent bystanders cancel this ability!
                        if ( victim->position != POS_FIGHTING ) { return FALSE; }  */
                v_next = victim->next_in_room;
                if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
                && (victim->position == POS_FIGHTING) && !IS_NPC(victim))
                {
                        break;
                }
        }

        if ((ch->fighting==NULL))
        {
                if (IS_STANCE(ch, STANCE_NORMAL))
                {
                        NPC_set_stance(ch);
                        do_stance(ch,"");
                }
                if (IS_STANCE(ch, STANCE_NONE))
                {
                        do_stance(ch, "dragon");
                }
                return FALSE;
        }

        if ( victim == NULL )
        {
                return FALSE;
        }

        if (ch->stunned)
        {
                do_stance(ch, "" );
                return FALSE;
        }

        do_stance(ch,"");

        if (IS_STANCE(victim, STANCE_NONE))
        {
                do_stance(ch,"mantis");
        }
        else if (IS_STANCE(victim,  STANCE_VIPER))
        {
                do_stance(ch,"swallow");
        }
        else if (IS_STANCE(victim, STANCE_CRANE))
        {
                do_stance(ch,"tiger");
        }
        else if (IS_STANCE(victim, STANCE_MONGOOSE))
        {
                do_stance(ch,"mantis");
        }
        else if (IS_STANCE(victim, STANCE_BULL))
        {
                do_stance(ch,"swallow");
        }
        else if (IS_STANCE(victim, STANCE_CRAB))
        {
                do_stance(ch,"dragon");
        }
        else if (IS_STANCE(victim, STANCE_MANTIS))
        {
                do_stance(ch,"dragon");
        }
        else if (IS_STANCE(victim, STANCE_TIGER))
        {
                do_stance(ch,"dragon");
        }
        else if (IS_STANCE(victim, STANCE_DRAGON))
        {
                do_stance(ch,"swallow");
        }
        else if (IS_STANCE(victim, STANCE_MONKEY))
        {
                do_stance(ch,"monkey");
        }
        else if (IS_STANCE(victim, STANCE_SWALLOW))
        {
                do_stance(ch,"tiger");
        }

        return TRUE;
}

bool spec_viper_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "viper");
	}

	return TRUE;
}

bool spec_crane_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "crane");
	}

	return TRUE;
}

bool spec_mongoose_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "mongoose");
	}

	return TRUE;
}

bool spec_crab_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "crab");
	}

	return TRUE;
}

bool spec_bull_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "bull");
	}

	return TRUE;
}

bool spec_mantis_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "mantis");
	}

	return TRUE;
}

bool spec_tiger_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "tiger");
	}

	return TRUE;
}

bool spec_swallow_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "swallow");
	}

	return TRUE;
}

bool spec_dragon_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "dragon");
	}

	return TRUE;
}

bool spec_monkey_stance (CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
	{
		return FALSE;
	}

	if (ch->stunned)
	{
		do_stance(ch, "");
		return FALSE;
	}

	if (IS_STANCE(ch, STANCE_NORMAL))
	{
		NPC_set_stance(ch);
		do_stance(ch, "");
	}

	if (IS_STANCE(ch, STANCE_NONE))
	{
		do_stance(ch, "monkey");
	}

	return TRUE;
}

