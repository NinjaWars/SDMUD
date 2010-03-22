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

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN ( spell_sate );
DECLARE_SPELL_FUN ( spell_quench );
DECLARE_SPELL_FUN ( spell_heroes_feast );
DECLARE_SPELL_FUN ( spell_holy_mace );
DECLARE_SPELL_FUN ( spell_acid_rain );
DECLARE_SPELL_FUN ( spell_4x );
DECLARE_SPELL_FUN ( spell_null );
DECLARE_SPELL_FUN ( spell_acid_blast );
DECLARE_SPELL_FUN ( spell_animate );
DECLARE_SPELL_FUN ( spell_anger );
DECLARE_SPELL_FUN ( spell_armor );
DECLARE_SPELL_FUN ( spell_blade_bless );
DECLARE_SPELL_FUN ( spell_bless );
DECLARE_SPELL_FUN ( spell_blindness );
DECLARE_SPELL_FUN ( spell_burning_hands );
DECLARE_SPELL_FUN ( spell_call_lightning );
DECLARE_SPELL_FUN ( spell_calm );
DECLARE_SPELL_FUN ( spell_call_darkness );
DECLARE_SPELL_FUN ( spell_camouflage );
DECLARE_SPELL_FUN ( spell_cancellation );
DECLARE_SPELL_FUN ( spell_cause_critical );
DECLARE_SPELL_FUN ( spell_cause_light );
DECLARE_SPELL_FUN ( spell_cause_serious );
DECLARE_SPELL_FUN ( spell_change_sex );
DECLARE_SPELL_FUN ( spell_chain_lightning );
DECLARE_SPELL_FUN ( spell_charm_person );
DECLARE_SPELL_FUN ( spell_chill_touch );
DECLARE_SPELL_FUN ( spell_colour_spray );
DECLARE_SPELL_FUN ( spell_concentration );
DECLARE_SPELL_FUN ( spell_conjure );
DECLARE_SPELL_FUN ( spell_continual_light );
DECLARE_SPELL_FUN ( spell_control_weather );
DECLARE_SPELL_FUN ( spell_create_bed );
DECLARE_SPELL_FUN ( spell_create_food );
DECLARE_SPELL_FUN ( spell_create_rose );
DECLARE_SPELL_FUN ( spell_create_spring );
DECLARE_SPELL_FUN ( spell_create_water );
DECLARE_SPELL_FUN ( spell_cry );
DECLARE_SPELL_FUN ( spell_cure_blindness );
DECLARE_SPELL_FUN ( spell_cure_critical );
DECLARE_SPELL_FUN ( spell_cure_disease );
DECLARE_SPELL_FUN ( spell_cure_light );
DECLARE_SPELL_FUN ( spell_cure_poison );
DECLARE_SPELL_FUN ( spell_cure_serious );
DECLARE_SPELL_FUN ( spell_curse );
DECLARE_SPELL_FUN ( spell_dark_gift );
DECLARE_SPELL_FUN ( spell_demonfire );
DECLARE_SPELL_FUN ( spell_detect_evil );
DECLARE_SPELL_FUN ( spell_detect_good );
DECLARE_SPELL_FUN ( spell_detect_hidden );
DECLARE_SPELL_FUN ( spell_invigorate );
DECLARE_SPELL_FUN ( spell_detect_invis );
DECLARE_SPELL_FUN ( spell_detect_magic );
DECLARE_SPELL_FUN ( spell_detect_poison );
DECLARE_SPELL_FUN ( spell_dispel_evil );
DECLARE_SPELL_FUN ( spell_dispel_good );
DECLARE_SPELL_FUN ( spell_dispel_magic );
DECLARE_SPELL_FUN ( spell_divine_fury );
DECLARE_SPELL_FUN ( spell_divine_protection );
DECLARE_SPELL_FUN ( spell_dragon_skin );
DECLARE_SPELL_FUN ( spell_dragon_wisdom );
DECLARE_SPELL_FUN ( spell_earthquake );
DECLARE_SPELL_FUN ( spell_empower );
DECLARE_SPELL_FUN ( spell_enchant_armor );
DECLARE_SPELL_FUN ( spell_enchant_weapon );
DECLARE_SPELL_FUN ( spell_energy_drain );
DECLARE_SPELL_FUN ( spell_faerie_fire );
DECLARE_SPELL_FUN ( spell_faerie_fog );
DECLARE_SPELL_FUN ( spell_farsight );
DECLARE_SPELL_FUN ( spell_fireball );
DECLARE_SPELL_FUN ( spell_fireproof );
DECLARE_SPELL_FUN ( spell_fireshield );
DECLARE_SPELL_FUN ( spell_fire_storm );
DECLARE_SPELL_FUN ( spell_flamestrike );
DECLARE_SPELL_FUN ( spell_floating_disc );
DECLARE_SPELL_FUN ( spell_fly );
DECLARE_SPELL_FUN ( spell_frenzy );
DECLARE_SPELL_FUN ( spell_gate );
DECLARE_SPELL_FUN ( spell_gods_armor );
DECLARE_SPELL_FUN ( spell_giant_strength );
DECLARE_SPELL_FUN ( spell_harm );
DECLARE_SPELL_FUN ( spell_haste );
DECLARE_SPELL_FUN ( spell_heal );
DECLARE_SPELL_FUN ( spell_heat_metal );
DECLARE_SPELL_FUN ( spell_holy_word );
DECLARE_SPELL_FUN ( spell_iceshield );
DECLARE_SPELL_FUN ( spell_identify );
DECLARE_SPELL_FUN ( spell_immolation );
DECLARE_SPELL_FUN ( spell_infravision );
DECLARE_SPELL_FUN ( spell_invis );
DECLARE_SPELL_FUN ( spell_know_alignment );
DECLARE_SPELL_FUN ( spell_lightning_bolt );
DECLARE_SPELL_FUN ( spell_locate_object );
DECLARE_SPELL_FUN ( spell_leech );
DECLARE_SPELL_FUN ( spell_magic_missile );
DECLARE_SPELL_FUN ( spell_mana_shield );
DECLARE_SPELL_FUN ( spell_mass_healing );
DECLARE_SPELL_FUN ( spell_mass_invis );
DECLARE_SPELL_FUN ( spell_master_healing );
DECLARE_SPELL_FUN ( spell_meteor_storm );
DECLARE_SPELL_FUN ( spell_nexus );
DECLARE_SPELL_FUN ( spell_pass_door );
DECLARE_SPELL_FUN ( spell_plague );
DECLARE_SPELL_FUN ( spell_poison );
DECLARE_SPELL_FUN ( spell_portal );
DECLARE_SPELL_FUN ( spell_protection_evil );
DECLARE_SPELL_FUN ( spell_protection_good );
DECLARE_SPELL_FUN ( spell_protection_voodoo );
DECLARE_SPELL_FUN ( spell_quest_pill );
DECLARE_SPELL_FUN ( spell_ray_of_truth );
DECLARE_SPELL_FUN ( spell_recharge );
DECLARE_SPELL_FUN ( spell_refresh );
DECLARE_SPELL_FUN ( spell_remove_curse );
DECLARE_SPELL_FUN ( spell_restore_mana );
DECLARE_SPELL_FUN ( spell_resurrect );
DECLARE_SPELL_FUN ( spell_sanctuary );
DECLARE_SPELL_FUN ( spell_shocking_grasp );
DECLARE_SPELL_FUN ( spell_shockshield );
DECLARE_SPELL_FUN ( spell_acidshield );
DECLARE_SPELL_FUN ( spell_poisonshield );
DECLARE_SPELL_FUN ( spell_shield );
DECLARE_SPELL_FUN ( spell_sleep );
DECLARE_SPELL_FUN ( spell_slow );
DECLARE_SPELL_FUN ( spell_stone_skin );
DECLARE_SPELL_FUN ( spell_summon );
DECLARE_SPELL_FUN ( spell_summon_familiar );
DECLARE_SPELL_FUN ( spell_superior_healing );
DECLARE_SPELL_FUN ( spell_teleport );
DECLARE_SPELL_FUN ( spell_transport );
DECLARE_SPELL_FUN ( spell_ventriloquate );
DECLARE_SPELL_FUN ( spell_voodoo );
DECLARE_SPELL_FUN ( spell_blind_rage );
DECLARE_SPELL_FUN ( spell_weaken );
DECLARE_SPELL_FUN ( spell_word_of_recall );
DECLARE_SPELL_FUN ( spell_vampire_blast );
DECLARE_SPELL_FUN ( spell_blood_laceration );
DECLARE_SPELL_FUN ( spell_acid_breath );
DECLARE_SPELL_FUN ( spell_fire_breath );
DECLARE_SPELL_FUN ( spell_frost_breath );
DECLARE_SPELL_FUN ( spell_gas_breath );
DECLARE_SPELL_FUN ( spell_lightning_breath );
DECLARE_SPELL_FUN ( spell_general_purpose );
DECLARE_SPELL_FUN ( spell_high_explosive );
DECLARE_SPELL_FUN ( spell_briarshield );

DECLARE_SPELL_FUN ( spell_drain_blade );
DECLARE_SPELL_FUN ( spell_shocking_blade );
DECLARE_SPELL_FUN ( spell_flame_blade );
DECLARE_SPELL_FUN ( spell_frost_blade );
DECLARE_SPELL_FUN ( spell_sharp_blade );
DECLARE_SPELL_FUN ( spell_vorpal_blade );
DECLARE_SPELL_FUN ( spell_ward );

/* spells added by TAKA */
DECLARE_SPELL_FUN ( spell_stone_meld );
DECLARE_SPELL_FUN ( spell_screen );
DECLARE_SPELL_FUN ( spell_acidrain );
DECLARE_SPELL_FUN ( spell_icerain );
DECLARE_SPELL_FUN ( spell_firerain );
DECLARE_SPELL_FUN ( spell_firestorm );
DECLARE_SPELL_FUN ( spell_acidstorm );
DECLARE_SPELL_FUN ( spell_icestorm );
DECLARE_SPELL_FUN ( spell_mud_skin );
DECLARE_SPELL_FUN ( spell_moss_skin );
DECLARE_SPELL_FUN ( spell_bark_skin );
DECLARE_SPELL_FUN ( spell_steel_skin );
DECLARE_SPELL_FUN ( spell_emerald_skin );
DECLARE_SPELL_FUN ( spell_ruby_skin );
DECLARE_SPELL_FUN ( spell_diamond_skin );
DECLARE_SPELL_FUN ( spell_mystic_armor );
DECLARE_SPELL_FUN ( spell_ice_bolt );
DECLARE_SPELL_FUN ( spell_fire_bolt );
DECLARE_SPELL_FUN ( spell_acid_bolt );
DECLARE_SPELL_FUN ( spell_gas_bolt );
DECLARE_SPELL_FUN ( spell_fireblast );
DECLARE_SPELL_FUN ( spell_iceblast );
DECLARE_SPELL_FUN ( spell_electricblast );
DECLARE_SPELL_FUN ( spell_gasblast );
DECLARE_SPELL_FUN ( spell_lightningblast );
DECLARE_SPELL_FUN ( spell_banish );
DECLARE_SPELL_FUN ( spell_blood_blast );
DECLARE_SPELL_FUN ( spell_earthrise );
DECLARE_SPELL_FUN ( spell_immortal_wrath );
DECLARE_SPELL_FUN ( spell_powersurge );
DECLARE_SPELL_FUN ( spell_shadow_barrier );
DECLARE_SPELL_FUN ( spell_mistblend );
DECLARE_SPELL_FUN ( spell_swalk );

/* spells added by Rhaelar */
DECLARE_SPELL_FUN ( spell_tsunami );
DECLARE_SPELL_FUN ( spell_fear );
DECLARE_SPELL_FUN ( spell_megafaerie_fire );
DECLARE_SPELL_FUN ( spell_divine_healing );
DECLARE_SPELL_FUN ( spell_scry );
DECLARE_SPELL_FUN ( spell_detect_scry );
DECLARE_SPELL_FUN ( spell_sleeth );
DECLARE_SPELL_FUN ( spell_flying_blades );
DECLARE_SPELL_FUN ( spell_fire_elemental );
DECLARE_SPELL_FUN ( spell_water_elemental );
DECLARE_SPELL_FUN ( spell_earth_elemental );
DECLARE_SPELL_FUN ( spell_air_elemental );
DECLARE_SPELL_FUN ( spell_cure_all );
DECLARE_SPELL_FUN ( spell_restore_megamana );
DECLARE_SPELL_FUN ( spell_powerstun );
DECLARE_SPELL_FUN ( spell_rasp_attack );
DECLARE_SPELL_FUN ( spell_acid_storm );
DECLARE_SPELL_FUN ( spell_lightning_storm );
DECLARE_SPELL_FUN ( spell_frost_storm );
DECLARE_SPELL_FUN ( spell_fire_storm );
DECLARE_SPELL_FUN ( spell_rasp );
