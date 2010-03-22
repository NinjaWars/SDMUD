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

/***************************************************************************
 * (c) 2000 TAKA and the GHOST DANCER MUD PROJECT TEAM
 *
 * You may use this code provided you accept it's usage agreements
 *
 *  Usage agreement
 *  1) Is that you do not remove or modify this comment block.
 *  2) You must give due credit in the help files
 *  3) You email me at a_ghost_dancer@excite.com
 *     this helps me judge what snippets are popular and i welcome
 *     any ideas and if i find them worthy i may decide to code them
 *     for GHOST and distribute them on our page.
 *  4) You must abid by all other ROM and MERC licences
 *  5) Finally any improvements/bugs you find or make please share them
 *     so we can continue to put out quality snippets.
 *
 *  Last thank you to all the ROM amd MERC folks for this wounderful code
 *  base know as ROM.
 *
 *          The Ghost Dancer MUD Project Team and me TAKA thank you
 *          for your interest.
 *
 *          You can email me at:
 *          TAKA
 *          a_ghost_dancer@excite.com
 *
 * All these Spells are by Taka of Ghost Dancer MUD Project team!
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
#include "sd.h"
#include "magic.h"
#include "recycle.h"

/* SPELLS BY TAKA     */
/* Stone Meld */
MAGIC ( spell_stone_meld )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already as hard as a rock.\n\r",
                           ch );
        else
            act ( "$N is already as hard as can be.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -80;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin melds into stone.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin melds into stone.\n\r", victim );
    return;
}

/* smoke screen */
MAGIC ( spell_screen )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your are surrounded by a wall of smoke.\n\r",
                           ch );
        else
            act ( "$N is already surrounded by smoke.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = ( -30 * ( level / 26 ) );
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n is hidden in smoke screen.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your are hidden in a smoke screen.\n\r", victim );
    return;
}

/*
 * ice rain spell by TAKA     ghost dancer
 */

MAGIC ( spell_icerain )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 8; i++ )
    {
        dam = dice ( level, 8 );

        if ( saves_spell ( level, victim, DAM_COLD ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_COLD, TRUE, 0 );
        }
    }
    return;
}

/*
 * fire rain spell by TAKA     ghost dancer
 */

MAGIC ( spell_firerain )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 8; i++ )
    {
        dam = dice ( level, 8 );

        if ( saves_spell ( level, victim, DAM_FIRE ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_FIRE, TRUE, 0 );
        }
    }
    return;
}

/*
 * acid storm spell by TAKA     ghost dancer
 */

MAGIC ( spell_acidstorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( ( level / 2 ), 8 );
        if ( saves_spell ( level, victim, DAM_ACID ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {Dacid {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {Dacid {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_ACID, TRUE, 0 );
        }
    }
    return;
}

/*
 * ice storm spell by TAKA     ghost dancer
 */

MAGIC ( spell_icestorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( ( level / 2 ), 8 );

        if ( saves_spell ( level, victim, DAM_COLD ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_COLD, TRUE, 0 );
        }
    }
    return;
}

/*
 * fire storm spell by TAKA     ghost dancer
 */

MAGIC ( spell_firestorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( ( level / 2 ), 12 );

        if ( saves_spell ( level, victim, DAM_FIRE ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_FIRE, TRUE, 0 );
        }
    }
    return;
}

/*
	skin spells by TAKA
 */

MAGIC ( spell_mud_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already as mud.\n\r", ch );
        else
            act ( "$N already has the texture of mud.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -50;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin turns to mud.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin turns to mud.\n\r", victim );
    return;
}

MAGIC ( spell_moss_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already covered in moss.\n\r", ch );
        else
            act ( "$N already has the texture of moss.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -65;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin turns to moss.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin turns to moss.\n\r", victim );
    return;
}

MAGIC ( spell_bark_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already covered in bark.\n\r", ch );
        else
            act ( "$N is already as the texture of bark.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -50;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin is covered by bark.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin is covered by bark.\n\r", victim );
    return;
}

MAGIC ( spell_steel_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already protected by steel.\n\r",
                           ch );
        else
            act ( "$N is already protected by steel.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -100;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin is covered by steel.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin is covered by steel.\n\r", victim );
    return;
}

MAGIC ( spell_emerald_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin already has a green glow.\n\r", ch );
        else
            act ( "$N already has a green glow.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -150;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin is protected by a greenish arua.", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "Your skin is protected by a greenish aura.\n\r", victim );
    return;
}

MAGIC ( spell_ruby_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin already has a red glow.\n\r", ch );
        else
            act ( "$N already has a red glow.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -250;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin is protected by a redish arua.", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "Your skin is protected by a redish aura.\n\r", victim );
    return;
}

MAGIC ( spell_diamond_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin already has a brilliant glow.\n\r",
                           ch );
        else
            act ( "$N already has a brilliant glow.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -320;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin is protected by a brilliant arua.", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "Your skin is protected by a brilliant aura.\n\r",
                   victim );
    return;
}

MAGIC ( spell_mystic_armor )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        send_to_char ( "They are already armored", ch );
        return;
    }

    af.type = sn;
    af.duration = 4 + ( level / 3 );
    af.location = APPLY_AC;
    af.modifier = -40 - ( level / 8 );
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    if ( ch != victim )
    {
        act ( "$n is surrounded by $N's mystic armour.", victim, NULL, ch,
              TO_ROOM );
        act ( "$N is surrounded by your mystic armour.", ch, NULL, victim,
              TO_CHAR );
        act ( "You are surrounded by $N's mystic armour.", victim, NULL, ch,
              TO_CHAR );
    }
    else
    {
        act ( "$n is surrounded by a mystic armour.", victim, NULL, ch,
              TO_ROOM );
        send_to_char ( "You are protected by a mystic armor!", ch );
    }
    return;
}

MAGIC ( spell_ice_bolt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = 150 + dice ( level / 4, 12 );

    if ( saves_spell ( level, victim, DAM_COLD ) )
        dam = ( dam * 100 ) / 120;

    act ( "$N struck in the chest by a bolt of ice from $n!", ch, NULL, ch,
          TO_NOTVICT );
    send_to_char ( "You strike $n with a large bolt of ice!\n\r", ch );

    damage ( ch, victim, dam, sn, DAM_COLD, TRUE, 0 );

    return;
}

MAGIC ( spell_fire_bolt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = 150 + dice ( level / 4, 12 );

    if ( saves_spell ( level, victim, DAM_FIRE ) )
        dam = ( dam * 100 ) / 120;

    act ( "$N struck in the chest by a bolt of fire from $n!", ch, NULL, ch,
          TO_NOTVICT );
    send_to_char ( "You strike $n with a large bolt of fire!\n\r", ch );

    damage ( ch, victim, dam, sn, DAM_FIRE, TRUE, 0 );

    return;
}

MAGIC ( spell_gas_bolt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = 150 + dice ( level / 4, 12 );

    if ( saves_spell ( level, victim, DAM_OTHER ) )
        dam = ( dam * 100 ) / 120;

    act ( "$N struck in the chest by a bolt of gas from $n!", ch, NULL, ch,
          TO_NOTVICT );
    send_to_char ( "You strike $n with a large bolt of gas!\n\r", ch );

    damage ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );

    return;
}

MAGIC ( spell_acid_bolt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = 150 + dice ( level / 4, 12 );

    if ( saves_spell ( level, victim, DAM_ACID ) )
        dam = ( dam * 100 ) / 120;

    act ( "$N struck in the chest by a bolt of acid from $n!", ch, NULL, ch,
          TO_NOTVICT );
    send_to_char ( "You strike $n with a large bolt of acid!\n\r", ch );

    damage ( ch, victim, dam, sn, DAM_ACID, TRUE, 0 );

    return;
}

MAGIC ( spell_fireblast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam =
        ( level / 2 ) + dice ( ( level / 6 ), 10 ) + dice ( ( level / 6 ),
                                                            10 );
    act ( "A blast of fire flies from $n's hands!", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "A blast of fire flies from your hands!\n\r", ch );

    if ( saves_spell ( level, victim, DAM_FIRE ) )
        dam /= 2;

    act ( "$n is struck by the blast of fire!!", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "You are struck by the fire blast!!\n\r", victim );

    damage ( ch, victim, dam, sn, DAM_FIRE, TRUE, 0 );

    return;
}

MAGIC ( spell_iceblast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam =
        ( level / 2 ) + dice ( ( level / 6 ), 10 ) + dice ( ( level / 6 ),
                                                            10 );
    act ( "A blast of ice flies from $n's hands!", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "A blast of ice flies from your hands!\n\r", ch );

    if ( saves_spell ( level, victim, DAM_COLD ) )
        dam /= 2;

    act ( "$n is struck by the blast of ice!!", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "You are struck by the ice blast!!\n\r", victim );

    damage ( ch, victim, dam, sn, DAM_COLD, TRUE, 0 );

    return;
}

MAGIC ( spell_gasblast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam =
        ( level / 2 ) + dice ( ( level / 6 ), 10 ) + dice ( ( level / 6 ),
                                                            10 );
    act ( "A blast of gas flies from $n's hands!", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "A blast of gas flies from your hands!\n\r", ch );

    if ( saves_spell ( level, victim, DAM_OTHER ) )
        dam /= 2;

    act ( "$n is struck by the blast of gas!!", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "You are struck by the gas blast!!\n\r", victim );

    damage ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );

    return;
}

MAGIC ( spell_electricblast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam =
        ( level / 2 ) + dice ( ( level / 6 ), 10 ) + dice ( ( level / 6 ),
                                                            10 );
    act ( "A blast of electricity flies from $n's hands!", ch, NULL, NULL,
          TO_ROOM );
    send_to_char ( "A blast of electricity flies from your hands!\n\r", ch );

    if ( saves_spell ( level, victim, DAM_ENERGY ) )
        dam /= 2;

    act ( "$n is struck by the blast of electricity!!", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "You are struck by the electric blast!!\n\r", victim );

    damage ( ch, victim, dam, sn, DAM_ENERGY, TRUE, 0 );

    return;
}

MAGIC ( spell_lightningblast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam =
        ( level / 2 ) + dice ( ( level / 6 ), 10 ) + dice ( ( level / 6 ),
                                                            10 );
    act ( "A blast of lightning flies from $n's hands!", ch, NULL, NULL,
          TO_ROOM );
    send_to_char ( "A blast of lightning flies from your hands!\n\r", ch );

    if ( saves_spell ( level, victim, DAM_LIGHTNING ) )
        dam /= 2;

    act ( "$n is struck by the blast of lightning!!", victim, NULL, NULL,
          TO_ROOM );
    send_to_char ( "You are struck by the lightning blast!!\n\r", victim );

    damage ( ch, victim, dam, sn, DAM_LIGHTNING, TRUE, 0 );

    return;
}

MAGIC ( spell_meteor_storm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 7; i++ )
    {
        dam = dice ( level, 100 );

        if ( victim->in_room == ch->in_room )
        {
            act ( "{DM{re{Dt{re{Do{rr{Ds{x rain down upon {W$N!!!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{DM{re{Dt{re{Do{rr{Ds{x rain down upon {W$N!{x", ch, NULL,
                  victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_FIRE, TRUE, 0 );
        }
    }

    return;
}

MAGIC ( spell_call_darkness )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 3; i++ )
    {
        dam = dice ( level, 110 );

        if ( victim->in_room == ch->in_room )
        {
            act ( "{D$n calls on the powers of the nights to attack $N!{x",
                  ch, NULL, victim, TO_ROOM );
            act ( "{D$N is attacked by the powers of the night!!{x", ch, NULL,
                  victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_NEGATIVE, TRUE, 0 );
        }
    }
    return;
}

MAGIC ( spell_ward )
{
    AFFECT_DATA af;

    if ( is_affected ( ch, sn ) )
    {
        send_to_char ( "You are already in a ward", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_WARD;
    affect_to_char ( ch, &af );
    act ( "$n is warded.", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "You are warded.\n\r", ch );
    return;
}

/*original writer unkown, out of paradox codebase RoT1.4/1.5 diriv*/
MAGIC ( spell_mana_shield )
{

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED ( victim, SHD_MANA ) )
    {
        if ( victim == ch )
            send_to_char ( "You are already surrounded by a mana shield.\n\r",
                           ch );
        else
            act ( "$N is already surrounded by a mana shield.", ch, NULL,
                  victim, TO_CHAR );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 22;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_MANA;

    affect_to_char ( victim, &af );
    send_to_char ( "You are surrounded by a mana shield.\n\r", victim );
    act ( "$n is surrounded by a mana shield.", victim, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_leech )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level, 100 );
    if ( victim->perm_stat[STAT_WIS] > ch->perm_stat[STAT_WIS] )
        dam /= 1.25;
    damage ( ch, victim, dam, sn, DAM_NEGATIVE, TRUE, 0 );
//    ch->hit += dam / 20;
    return;
}
/*
MAGIC ( spell_banish )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    victim = ( CHAR_DATA * ) vo;

    if ( IS_AFFECTED ( victim, AFF_BANISH ) ||
         saves_spell ( level, victim, DAM_NEGATIVE ) )
        return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 3 * level;
    af.location = APPLY_HITROLL;
    af.modifier = -1 * ( level / 5 );
    af.bitvector = AFF_BANISH;
    affect_to_char ( victim, &af );

    af.location = APPLY_SAVING_SPELL;
    af.modifier = level / 8;
    affect_to_char ( victim, &af );

    send_to_char ( "You have been banished.\n\r", victim );
    if ( ch != victim )
        act ( "$N has been banished.", ch, NULL, victim, TO_CHAR );
    return;
}
*/
/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_blood_blast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    dam = level + level / 3 + dice ( 60, 60 );
    if ( saves_spell ( level, victim, DAM_OTHER ) )
        dam /= 1.5;
    damage_old ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );
    if ( chance ( 50 ) )
    {
        dam = dice ( level, 10 ) + level + 50;
        if ( saves_spell ( level, victim, DAM_OTHER ) )
            dam /= 1.5;
        damage_old ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );
    }
    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_earthrise )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( level * 3, 35 );

        if ( victim->in_room == ch->in_room )
        {
            act ( "Chunks of the {ge{ba{gr{bt{gh {Dr{ri{Ds{re {xup and come crashing down upon {W$N!!!{x", ch, NULL, victim, TO_ROOM );
            act ( "Chunks of the {ge{ba{gr{bt{gh {Dr{ri{Ds{re {xup and come crashing down upon {W$N!{x", ch, NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );
        }
    }

    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

/* <<< SPELL SHOULD NOT BE USED IN SCROLLS OR ANYTHING THAT MORTALS MAY ACCESS!!!  Just for the IMMs to have Fun. >>> */

MAGIC ( spell_immortal_wrath )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( 3213, 6324 );

    if ( victim->in_room == ch->in_room )
    {
        act ( "The {BImm{Wort{Bals {xrelease their {DW{RR{WA{RT{DH {xupon {c$N!!!{x", ch, NULL, victim, TO_ROOM );
        act ( "Using your {BImm{Wor{Btal {DP{Yo{yw{Ye{Dr{x, you release your {DW{RR{WA{RT{DH {xupon {c$N!{x", ch, NULL, victim, TO_CHAR );
        damage ( ch, victim, dam, sn, DAM_OTHER, TRUE, 0 );
    }

    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_powersurge )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( ch, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "{WYour power is at its peak!{x\n\r", ch );
        else
            act ( "This spell can't be cast on others! ", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_HIT;
    af.modifier = 2000;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_MANA;
    af.modifier = 6000;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_DAMROLL;
    af.modifier = level;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_HITROLL;
    af.modifier = level;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_AC;
    af.modifier = -500;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    act ( "$n gets a RUSH of power!", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your power gets a huge BOOST!\n\r", ch );

    ch->hit = ch->max_hit;
    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_shadow_barrier )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "You are already protected by shadows.\n\r", ch );
        else
            act ( "$N is already protected by shadows.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = 2 + level;
    af.duration = level / 7;
    af.modifier = -50 - level * 2;
    af.location = APPLY_AC;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    send_to_char ( "The shadows surround you and create a barrier.\n\r",
                   victim );
    if ( ch != victim )
        act ( "$N is protected by a barrier of shadows.", ch, NULL, victim,
              TO_CHAR );
    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_mistblend )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    victim = ( CHAR_DATA * ) vo;

    if ( IS_SHIELDED ( victim, SHD_MISTBLEND ) )
        return;

    act ( "$n blends into the mist.", victim, NULL, NULL, TO_ROOM );

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level + 98;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_MISTBLEND;
    affect_to_char ( victim, &af );

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level + 98;
    af.location = APPLY_DAMROLL;    /*high damroll since it goes away after first hit */
    af.modifier = 500;
    af.bitvector = SHD_MISTBLEND;
    affect_to_char ( victim, &af );

    send_to_char ( "You blend into the mist.\n\r", victim );
    return;
}
