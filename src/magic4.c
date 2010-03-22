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
#include "sd.h"
#include "magic.h"
#include "recycle.h"

/* Spells by Rhaelar of Shattered Dreams MUD - sdmud.com 7000 */

MAGIC ( spell_megafaerie_fire )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED ( victim, AFF_FAERIE_FIRE ) )
    {
	send_to_char ( "They are already surrounded by a pink outline.\n\r", ch );
        return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level * 2;
    af.duration = level / 3;
    af.location = APPLY_AC;
    af.modifier = 30 * level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char ( victim, &af );
    send_to_char ( "You are surrounded by a huge pink outline.\n\r", victim );
    act ( "$n is surrounded by a huge pink outline.", victim, NULL, NULL,
          TO_ROOM );
    return;
}

MAGIC ( spell_tsunami )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    send_to_char( "You call forth a massive wave of water!\n\r", ch );
    act( "A massive wave of water flows over you!", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    dam = dice(550 + 22, 22);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE, 0 );

    dam = dice(345 + 21, 21);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE, 0 );

    if (chance ( 40 ) )
    {
        return;
    }

    dam = dice(450 + 20, 20);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE, 0 );

    if (chance ( 20 ) )
    {
        return;
    }
    dam = dice(615 + 20, 20);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE, 0 );
    return;
}

MAGIC ( spell_fear )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

 if ( chance ( 15 ) )
 {
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 3;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * (level / 5);
    af.bitvector = 0;
    affect_to_char(victim, &af);
    
    af.location  = APPLY_DAMROLL;
    af.modifier  = -1 * (level/10);
    affect_to_char( victim, &af );
    send_to_char( "You are {6overcome{0 with {5fear{0.\n\r", victim );
    act("$n looks {6scared{0 and {5cowers{0 in terror.",victim,NULL,NULL,TO_ROOM);
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    do_flee( victim, "" );   
    return;
 }
 else
 {
  send_to_char ( "You failed.\n\r", ch );
  return;
 }
}

MAGIC ( spell_lightning_storm )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	static const int dam_each[] =
	{
	 0,
	 85, 85, 85, 85, 85, 85, 88, 88, 88, 88,
	 88, 88, 88, 89, 89, 89, 80, 80, 80, 81,
	 81, 81, 82, 82, 82, 83, 83, 83, 84, 84,
	 84, 88, 88, 88, 86, 86, 86, 88, 88, 88,
	 89, 89, 89, 89, 89, 89, 90, 90, 90, 91,
	 91, 91, 92, 92, 92, 93, 93, 93, 94, 94
	};
	int dam;

	level	= UMIN((unsigned)level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
	level	= UMAX(0, level);
	dam	= number_range(dam_each[level]*10,(number_range((ch->level),2800)))*6;
	if ( saves_spell( level, victim,DAM_LIGHTNING) )
	dam /= 2;
	if ( ( ch->fighting == NULL )
	&& ( !IS_NPC( ch ) )
	&& ( !IS_NPC( victim ) ) )
	{
	ch->attacker = TRUE;
	victim->attacker = FALSE;
	}
	damage_old( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE, 0);
	if ( (ch->class == 7))
	damage_old( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE, 0);
	return;
}

MAGIC ( spell_flying_blades )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	int dam;

	act("Many flying blades appear around $n and crash $N.",
	ch,NULL,victim,TO_ROOM);
	act("Many flying blades appear around you and crash $N.",
	ch,NULL,victim,TO_CHAR);
	act("Many flying blades appear around $n and crash you!",
	ch,NULL,victim,TO_VICT);

	dam = dice(level,40);
	if (saves_spell(level,victim,DAM_PIERCE))
	dam /= 2;
	damage(ch,victim,dam,sn,DAM_PIERCE,TRUE, 0);

	if (!IS_NPC(ch) && victim != ch &&
		ch->fighting != victim && victim->fighting != ch &&
		(IS_SET(victim->affected_by,AFF_CHARM) || !IS_NPC(victim)))

	act("The flying blades pierce into $n!",victim,NULL,NULL,TO_ROOM);
	dam = dice(level,35);
	if (saves_spell(level,victim,DAM_PIERCE))
	dam /= 2;
   damage(ch,victim,dam,sn,DAM_PIERCE,TRUE, 0);
	act("The flying blades pierce into you!",victim,NULL,NULL,TO_CHAR);

	act("The flying blades pierce into $n!",victim,NULL,NULL,TO_ROOM);
	dam = dice(level,30);
	if (saves_spell(level,victim,DAM_PIERCE))
	dam /= 2;
	damage(ch,victim,dam,sn,DAM_PIERCE,TRUE, 0);
	act("The flying blades pierce into you!",victim,NULL,NULL,TO_CHAR);

	act("The flying blades pierce into  $n!",victim,NULL,NULL,TO_ROOM);
	dam = dice(level,5);
	if (saves_spell(level,victim,DAM_PIERCE))
	dam /= 2;
	damage(ch,victim,dam,sn,DAM_PIERCE,TRUE, 0);
	act("The flying blades pierce into you!",victim,NULL,NULL,TO_CHAR);

}
/*
MAGIC ( spell_fire_elemental )
{
	AFFECT_DATA af;
	int count;
	CHAR_DATA *check, *pet;

	if (is_affected(ch,sn) && (!IS_IMMORTAL(ch)))
	{
		send_to_char("You can't call upon a {1fire{x elemental yet.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
	{
		stc("You are unable to call upon a {1fire{x elemental in this room.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
	{
		send_to_char("You can't call elementals in the Arena.\n\r",ch);
		return;
	}

	count = 0;

	for (check = char_list; check != NULL; check = check->next)
	{
	if (IS_NPC(check))
		if ( (check->master == ch)
		&& ( (check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL) ) )
			count++;
	}
	if ( (count == 1 && ch->level < 100) || (count == 2 && ch->level < 125)
	  || (count == 3) )
	{
		send_to_char("You already control as many elementals as you can.\n\r",ch);
		return;
	}

	if ( (ch->in_room->sector_type == SECT_WATER_SWIM)
	  || (ch->in_room->sector_type == SECT_WATER_NOSWIM) )
	{
		send_to_char("There is too much water here to summon a fire elemental.\n\r",ch);
		return;
	}
	if ((weather_info.sky > SKY_RAINING) && (!IS_SET(ch->in_room->room_flags,ROOM_INDOORS)))
	{

		send_to_char("The weather is to wet to summon a fire elemental.\n\r",ch);
		return;
	}
	pet = create_mobile(get_mob_index(MOB_VNUM_FIRE_ELEMENTAL) );
	pet->level = level-1;
	pet->max_hit = ch->max_hit + dice(level,10);
	pet->hit = pet->max_hit;
	pet->damroll += level/2;
	pet->alignment = ch->alignment;
	pet->clan = ch->clan;
		add_follower(pet,ch);
		pet->leader = ch;
		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = 0;
	af.level = level;
	af.modifier = 0;
	af.duration = 15;
	af.bitvector = 0;
	affect_to_char(ch,&af);
    char_to_room(pet,ch->in_room);

	act("{g$N arrives in a column of {rsearing {Rflames.{x",ch,NULL,pet,TO_CHAR);
	act("{g$N arrives in a column of {rsearing {Rflames.{x",ch,NULL,pet,TO_ROOM);

	if (number_percent() > ch->pcdata->learned[sn])
	{
	act("{G$N says, 'How dare you call me{R!?!{x",ch,NULL,pet,TO_CHAR);
	act("{G$N says, 'How dare you call me{R!?!{x",ch,NULL,pet,TO_ROOM);
	multi_hit(pet,ch,TYPE_UNDEFINED);
	return;
	}
}


MAGIC ( spell_water_elemental )
{
	AFFECT_DATA af;
	int count;
	CHAR_DATA *check, *pet;

	if (is_affected(ch,sn) && (!IS_IMMORTAL(ch)))
	{
		send_to_char("You can't call upon a {bWater{x elemental yet.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
	{
		stc("You are unable to call upon a {bwater{x elemental in this room.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
	{
		send_to_char("You can't call elementals in the Arena.\n\r",ch);
		return;
	}
	count = 0;
	for (check = char_list; check != NULL; check = check->next)
	{
	if (IS_NPC(check))
		if ( (check->master == ch)
		&& ( (check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL) ) )
			count++;
	}

	if ( (count == 1 && ch->level < 100) || (count == 2 && ch->level < 125)
	  || (count == 3) )
	{
		send_to_char("You already control as many elementals as you can.\n\r",ch);
		return;
	}

	if ( (ch->in_room->sector_type != SECT_WATER_SWIM)
	  && (ch->in_room->sector_type != SECT_WATER_NOSWIM) )
	{
		send_to_char("There is not enough water here to summon a water elemental.\n\r",ch);
		return;
	}

	pet = create_mobile(get_mob_index(MOB_VNUM_WATER_ELEMENTAL) );
	pet->level = level-1;
	pet->max_hit = ch->max_hit + dice(level,10);
	pet->hit = pet->max_hit;
	pet->damroll += level/2;
	pet->alignment = ch->alignment;
	pet->clan = ch->clan;
		add_follower(pet,ch);
		pet->leader = ch;
		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = 0;
	af.level = level;
	af.modifier = 0;
	af.duration = 15;
	af.bitvector = 0;
	affect_to_char(ch,&af);
	char_to_room(pet,ch->in_room);

	act("{4Water surges around you as {$$N arrives.",ch,NULL,pet,TO_CHAR);
	act("{4Water surges around you as {$$N arrives.",ch,NULL,pet,TO_ROOM);

	if (number_percent() > ch->pcdata->learned[sn])
	{
	act("{$$N says, You dare to call upon me!?!",ch,NULL,pet,TO_CHAR);
	act("{$$N says, You dare to call upon me!?!",ch,NULL,pet,TO_ROOM);
	multi_hit(pet,ch,TYPE_UNDEFINED);
	return;
	}
}

MAGIC ( spell_earth_elemental )
{
	AFFECT_DATA af;
	int count;
	CHAR_DATA *check, *pet;

	if (is_affected(ch,sn) && (!IS_IMMORTAL(ch)))
	{
		send_to_char("You can't call upon a {mEarth{x elemental yet.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
	{
		stc("You are unable to call upon a {mearth{x elemental in this room.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
	{
		send_to_char("You can't call elementals in the Arena.\n\r",ch);
		return;
	}

	count = 0;
	for (check = char_list; check != NULL; check = check->next)
	{
	if (IS_NPC(check))
		if ( (check->master == ch)
		&& ( (check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL) ) )
			count++;
	}

	if ( (count == 1 && ch->level < 100) || (count == 2 && ch->level < 125)
	  || (count == 3) )
	{
		send_to_char("You already control as many elementals as you can.\n\r",ch);
		return;
	}

	  if(ch->in_room->sector_type == SECT_AIR)
	{
		send_to_char("You are not in an area with enough raw earth.\n\r",ch);
		return;
	}

	pet = create_mobile(get_mob_index(MOB_VNUM_EARTH_ELEMENTAL) );
	pet->level = level-1;
	pet->max_hit = ch->max_hit + dice(level,10);
	pet->hit = pet->max_hit;
	pet->damroll += level/2;
	pet->alignment = ch->alignment;
	pet->clan = ch->clan;
		add_follower(pet,ch);
		pet->leader = ch;
		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = 0;
	af.level = level;
	af.modifier = 0;
	af.duration = 15;
	af.bitvector = 0;
	affect_to_char(ch,&af);
	char_to_room(pet,ch->in_room);

	act("The {mearth {Mrumbles and {#$N bursts out from the ground.{x",ch,NULL,pet,TO_CHAR);
	act("The {mearth {Mrumbles and {#$N bursts out from the ground.{x",ch,NULL,pet,TO_ROOM);

	if (number_percent() > ch->pcdata->learned[sn])
	{
	act("{$$N says, You dare to call upon me{!!?!{x",ch,NULL,pet,TO_CHAR);
	act("{$$N says, You dare to call upon me{!!?!{x",ch,NULL,pet,TO_ROOM);
	multi_hit(pet,ch,TYPE_UNDEFINED);
	return;
	}
}

MAGIC ( spell_air_elemental )
{
	AFFECT_DATA af;
	int count;
	CHAR_DATA *check, *pet;

	if (is_affected(ch,sn) && (!IS_IMMORTAL(ch)))
	{
		send_to_char("You can't call upon a {bAir{x elemental yet.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
	{
		stc("You are unable to call upon a {cair{x elemental in this room.\n\r",ch);
		return;
	}
	if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
	{
		send_to_char("You can't call elementals in the Arena.\n\r",ch);
		return;
	}

	count = 0;
	for (check = char_list; check != NULL; check = check->next)
	{
	if (IS_NPC(check))
		if ( (check->leader == ch)
		&& ( (check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL)
		  || (check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL) ) )
			count++;
	}

	if ( (count == 1 && ch->level < 100) || (count == 2 && ch->level < 125)
	  || (count == 3) )
	{
		send_to_char("You already control as many elementals as you can.\n\r",ch);
		return;
	}

	pet = create_mobile(get_mob_index(MOB_VNUM_AIR_ELEMENTAL) );
	pet->level = level-1;
	pet->max_hit = ch->max_hit + dice(level,10);
	pet->hit = pet->max_hit;
	pet->damroll += level/2;
	pet->alignment = ch->alignment;
	pet->clan = ch->clan;
	add_follower(pet,ch);
		pet->leader = ch;
		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = 0;
	af.level = level;
	af.modifier = 0;
	af.duration = 15;
	af.bitvector = 0;
	affect_to_char(ch,&af);
	char_to_room(pet,ch->in_room);

	act("{4$N forms before you in a violent {^vortex of wind.{x",ch,NULL,pet,TO_CHAR);
	act("{4$N forms before you in a violent {^vortex of wind.{x",ch,NULL,pet,TO_ROOM);

	if (number_percent() > ch->pcdata->learned[sn])
	{
	act("{$$N says, 'You dare to call upon me{R!?!{x",ch,NULL,pet,TO_CHAR);
	act("{$$N says, 'You dare to call upon me{R!?!{x",ch,NULL,pet,TO_ROOM);
	multi_hit(pet,ch,TYPE_UNDEFINED);
	return;
	}
}
*/
MAGIC ( spell_cure_all )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	int nsn;

	if (IS_AFFECTED(victim,AFF_BLIND))
	{
	 nsn = skill_lookup("cure blindness");
	 spell_cure_blindness(nsn,level,ch,(void *)victim,TARGET_CHAR);
	}
	if (IS_AFFECTED(victim,AFF_CURSE))
	{
	 nsn = skill_lookup("remove curse");
	 spell_remove_curse(nsn,level,ch,(void *)victim,TARGET_CHAR);
	}
	if (IS_AFFECTED(victim,AFF_POISON))
	{
		 nsn = skill_lookup("cure poison");
	 spell_cure_poison(gsn_poison,level,ch,(void *)victim,TARGET_CHAR);
	}
	if (IS_AFFECTED(victim,AFF_PLAGUE))
	{
	 nsn = skill_lookup("cure disease");
	 spell_cure_disease(nsn,level,ch,(void *)victim,TARGET_CHAR);
	}
	update_pos( victim );
	send_to_char( "A warm feeling fills your body.\n\r", victim );
	if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
	return;
}

MAGIC ( spell_restore_megamana )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	victim->mana = UMIN( victim->mana + 750, victim->max_mana );
	if (victim->max_mana == victim->mana)
		send_to_char("You feel fully focused!\n\r",victim);
	else
		send_to_char( "You feel more focused.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Ok.\n\r", ch );
	return;
}

MAGIC ( spell_powerstun )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;


	if (is_affected(victim,sn ) || saves_spell( level, victim,DAM_OTHER) )
	return;

	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  =  3;
	af.location  = APPLY_DEX;
	af.modifier  = - 3;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	send_to_char( "You are {Ystunned{x.\n\r", victim );
	act("$n is {Ystunned{x.",victim,NULL,NULL,TO_ROOM);
	WAIT_STATE( ch,(int)(0.75 * PULSE_VIOLENCE) );
	victim->stunned += number_fuzzy(3);
	return;
}

MAGIC ( spell_rasp )
{
	CHAR_DATA *victim  = (CHAR_DATA *) vo;
	int dam,mana;
	dam = 0;

   if ( is_affected( victim, sn ) || saves_spell( level, victim,DAM_NONE) )
	{
		act("You fail in picking $N's convoluded brain.",ch,NULL,victim,TO_CHAR);  /* Kira */
		return;
	}
	if ((victim->mana >= 250 ) && (victim->level >= 10))
	{
	dam = victim->mana/5 * 4;

	mana = number_range(ch->level,victim->mana)/4;
	ch->mana	+= mana;
	victim->mana	-= mana;
	}
	if ( ( ch->fighting == NULL )
	&& ( !IS_NPC( ch ) )
	&& ( !IS_NPC( victim ) ) )
	{
	ch->attacker = TRUE;
	victim->attacker = FALSE;
	}

	send_to_char("You feel your power being drained.\r",victim);
	damage_old( ch, victim, dam, sn, DAM_MENTAL, TRUE, 0);
	WAIT_STATE( ch,(int)(0.75 * PULSE_VIOLENCE) );

	return;
}

MAGIC ( spell_acid_storm )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	static const int dam_each[] =
	{
	 0,
	 85, 85, 85, 85, 85, 85, 88, 88, 88, 88,
	 88, 88, 88, 89, 89, 89, 80, 80, 80, 81,
	 81, 81, 82, 82, 82, 83, 83, 83, 84, 84,
	 84, 88, 88, 88, 86, 86, 86, 88, 88, 88,
	 89, 89, 89, 89, 89, 89, 90, 90, 90, 91,
	 91, 91, 92, 92, 92, 93, 93, 93, 94, 94
	};
	int dam;

	level	= UMIN((unsigned)level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
	level	= UMAX(0, level);
	dam	= number_range(dam_each[level]*10,(number_range((ch->level),2800)))*6;
	if ( saves_spell( level, victim,DAM_ACID) )
	dam /= 2;
	if ( ( ch->fighting == NULL )
	&& ( !IS_NPC( ch ) )
	&& ( !IS_NPC( victim ) ) )
	{
	ch->attacker = TRUE;
	victim->attacker = FALSE;
	}
	damage_old( ch, victim, dam, sn, DAM_ACID ,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_ACID ,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_ACID ,TRUE, 0);
	return;
}

MAGIC ( spell_frost_storm )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	static const int dam_each[] =
	{
	 0,
	 85, 85, 85, 85, 85, 85, 88, 88, 88, 88,
	 88, 88, 88, 89, 89, 89, 80, 80, 80, 81,
	 81, 81, 82, 82, 82, 83, 83, 83, 84, 84,
	 84, 88, 88, 88, 86, 86, 86, 88, 88, 88,
	 89, 89, 89, 89, 89, 89, 90, 90, 90, 91,
	 91, 91, 92, 92, 92, 93, 93, 93, 94, 94
	};
	int dam;

	level	= UMIN((unsigned)level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
	level	= UMAX(0, level);
	dam	= number_range(dam_each[level]*10,(number_range((ch->level),2800)))*6;
	if ( saves_spell( level, victim,DAM_COLD) )
	dam /= 2;
	if ( ( ch->fighting == NULL )
	&& ( !IS_NPC( ch ) )
	&& ( !IS_NPC( victim ) ) )
	{
	ch->attacker = TRUE;
	victim->attacker = FALSE;
	}
	damage_old( ch, victim, dam, sn, DAM_COLD,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_COLD,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_COLD ,TRUE, 0);
	return;
}

MAGIC ( spell_fire_storm )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	static const int dam_each[] =
	{
	 0,
	 85, 85, 85, 85, 85, 85, 88, 88, 88, 88,
	 88, 88, 88, 89, 89, 89, 80, 80, 80, 81,
	 81, 81, 82, 82, 82, 83, 83, 83, 84, 84,
	 84, 88, 88, 88, 86, 86, 86, 88, 88, 88,
	 89, 89, 89, 89, 89, 89, 90, 90, 90, 91,
	 91, 91, 92, 92, 92, 93, 93, 93, 94, 94
	};
	int dam;

	level	= UMIN((unsigned)level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
	level	= UMAX(0, level);
	dam	= number_range(dam_each[level]*10,(number_range((ch->level),2800)))*6;
	if ( saves_spell( level, victim,DAM_FIRE) )
	dam /= 2;
	if ( ( ch->fighting == NULL )
	&& ( !IS_NPC( ch ) )
	&& ( !IS_NPC( victim ) ) )
	{
	ch->attacker = TRUE;
	victim->attacker = FALSE;
	}
	damage_old( ch, victim, dam, sn, DAM_FIRE,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_FIRE,TRUE, 0);
	damage_old( ch, victim, dam, sn, DAM_FIRE,TRUE, 0);
	return;
}

MAGIC ( spell_drain_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_VAMPIRIC ) )
    {
        send_to_char ( "This weapon is already vampiric.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) )
    {
        send_to_char ( "This weapon is too blessed.\n\r", ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = 20;
    af.duration = 50;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_VAMPIRIC;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n turns dark and vampiric.", ch, obj, NULL,
          TO_ROOM );
    act ( "$p carried by you turns dark and vampiric.", ch, obj, NULL,
          TO_CHAR );
    return;
}

MAGIC ( spell_shocking_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_SHOCKING ) )
    {
        send_to_char ( "This weapon is already electrical.\n\r", ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = 20;
    af.duration = 50;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_SHOCKING;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n sparks with electricity.", ch, obj, NULL,
          TO_ROOM );
    act ( "$p carried by you sparks with electricity.", ch, obj, NULL,
          TO_CHAR );
    return;
}

MAGIC ( spell_flame_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_FLAMING ) )
    {
        send_to_char ( "This weapon is already flaming.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_FROST ) )
    {
        send_to_char ( "This weapon is too frost to handle this magic.\n\r",
                       ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = 20;
    af.duration = 50;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_FLAMING;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n gets a fiery aura.", ch, obj, NULL, TO_ROOM );
    act ( "$p carried by you gets a fiery aura.", ch, obj, NULL, TO_CHAR );
    return;
}

MAGIC ( spell_frost_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_FROST ) )
    {
        send_to_char ( "This weapon is already frost.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_FLAMING ) )
    {
        send_to_char ( "This weapon is too cold to handle this magic.\n\r",
                       ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = 20;
    af.duration = 50;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_FROST;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n grows wickedly cold.", ch, obj, NULL, TO_ROOM );
    act ( "$p carried by you grows wickedly cold.", ch, obj, NULL, TO_CHAR );
    return;
}

MAGIC ( spell_sharp_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_SHARP ) )
    {
        send_to_char ( "This weapon is already sharp.\n\r", ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = 20;
    af.duration = 50;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_SHARP;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n looks newly honed.", ch, obj, NULL, TO_ROOM );
    act ( "$p carried by you looks newly honed.", ch, obj, NULL, TO_CHAR );
    return;
}

MAGIC ( spell_vorpal_blade )
{
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    AFFECT_DATA af;

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "You can only cast this spell on weapons.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_VORPAL ) )
    {
        send_to_char ( "This weapon is already vorpal.\n\r", ch );
        return;
    }

    af.where = TO_WEAPON;
    af.type = sn;
    af.level = level / 2;
    af.duration = level;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_VORPAL;
    affect_to_obj ( obj, &af );

    act ( "$p carried by $n gleams witch magical strengh.", ch, obj, NULL,
          TO_ROOM );
    act ( "$p carried by you gleams witch magical strengh.", ch, obj, NULL,
          TO_CHAR );
    return;
}
