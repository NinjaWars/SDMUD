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
#include <time.h>
#include "sd.h"
#include "tables.h"

/*
 * Log Off Quotes
 */

const struct quote_type quote_table[MAX_QUOTES] = {
    {"Now ill move on with my tits\n\rahhhhhhhhhhhhh\n\rdo i have stiff nippels if i can tuch the fan inside the safty cage?", "Peldrol" },
    {"i dont think the most horny woman would ever be able to fufill my sexual neads", "Peldrol" },
    {"i was reading some sex stories at www.sexnoveller.dk", "Peldrol" },
    {"i like to masturbatre", "Peldrol" },
    {"I have passed the transitional stage of internet geekhood\n\rI was cashiering at work today, and was punching in the code for plums, which is 4040.\n\rand the 0 key doesn't work this well, so I punched it in wrong.\n\rand the machine flashed up 'Item Not Found: 404'\n\rand I actually laughed out loud", "Ich" },
    {"i like stalked this girl sorta :D\n\rlike once she asked me for a ride home from work\n\rand i took her home... i dropped her off at her house\n\rand shes like... wait a minute..how did you know where i lived?", "BlackDeth" },
    {"i was gonna call 911...but i was downloading a file", "OpCode" },
    {"'The animals will hear!' bellowed the ear licking penguin as the awesomely endowed midget sucked her oozing charlies and plugged his purple middle leg into her festering cunt.\n\roops\n\rwrong window", "Draxon" },
    {"i don't have hard drives. i just keep 30 chinese teenagers in my basement and force them to memorize numbers", "ikkenai" },
    {"WARNING: DO NOT LET DR. MARIO TOUCH YOUR GENITALS. HE IS NOT A REAL DOCTOR", "Squizzle" },
    {"Tupac Shakur sounds like a Protoss name.", "Dr_DOS" },
    {"In a perfect world... spammers would get caught, go to jail, and share a cell with many men who have enlarged their penisses, taken Viagra and are looking for a new relationship.", "Fooz" },
    {"theres a fly on my windowsill and it's rubbing its hands... its contemplating.. OH SHIT!!!", "alien" },
    {"does anyone here have a computer?", "jaga" },
    {"This cake is soooo good, it's like sex, except I'm having it", "aryov" },
    {"and my back is kinda fragile already, from being a geek", "Rhaelar" },
    {"this morning my dad went to go to the bathroom, but my little bro was already in there, my dad turns the knob but its locked, and in his deepest UT voice my bro shouts 'DENIED', my dad just walked away scratching his head.", "Breadfan" },
    {"hm. I've lost a machine.. literally _lost_. it responds to ping, it works completely, I just can't figure out where in my apartment it is.", "erno" },
    {"Purely in the interests of science, I have replaced the word 'wand' with 'wang' in the first Harry Potter Book\n\rLet's see the results...\n\r'Why aren't you supposed to do magic?' asked Harry.\n\r'Oh, well -- I was at Hogwarts meself but I -- er -- got expelled, ter tell yeh the truth. In me third year. They snapped me wang in half an' everything.\n\rA magic wang... this was what Harry had been really looking forward to.\n\r'Yes, yes. I thought I'd be seeing you soon. Harry Potter.' It wasn't a question. 'You have your mother's eyes. It seems only yesterday she was in here herself, buying her first wang. Ten and a quarter inches long, swishy, made of willow. Nice wang for charm work.'\n\r'Your father, on the other hand, favored a mahogany wang. Eleven inches.'\n\rHarry took the wang. He felt a sudden warmth in his fingers. He raised the wang above his head, brought it swishing down through the dusty air and a stream of red and gold sparks shot from the end like a firework, throwing dancing spots of light on to the walls\n\r'Oh, move over,' Hermione snarled. She grabbed Harry's wang, tapped the lock, and whispered, 'Alohomora!'\n\rThe troll couldn't feel Harry hanging there, but even a troll will notice if you stick a long bit of wood up its nose, and Harry's wang had still been in his hand when he'd jumped - it had gone straight up one of the troll's nostrils.\n\rHe bent down and pulled his wang out of the troll's nose. It was covered in what looked like lumpy gray glue.\n\rHe ran onto the field as you fell, waved his wang, and you sort of slowed down before you hit the ground. Then he whirled his wang at the dementors. Shot silver stuff at them.\n\rOk\n\rI have found, definitive proof\n\rthat J.K Rowling is a dirty DIRTY woman, making a fool of us all\n\r'Yes,' Harry said, gripping his wang very tightly, and moving into the middle of the deserted classroom. He tried to keep his mind on flying, but something else kept intruding.... Any second now, he might hear his mother again... but he shouldn't think that, or he would hear her again, and he didn't want to... or did he?\n\rSomething silver-white, something enormous, erupted from the end of his wang\n\rThen, with a sigh, he raised his wang and prodded the silvery substance with its tip.\n\r'Get - off - me!' Harry gasped. For a few seconds they struggled, Harry pulling at his uncles sausage-like fingers with his left hand, his right maintaining a firm grip on his raised wang.", "jonjonb" },
    {"Microsoft has also managed to upset women and entire countries.\n\rA Spanish-language version of Windows XP, destined for Latin American markets, asked users to select their gender between 'not specified,' 'male' or 'bitch,' because of an unfortunate error in translation.", "news.com.com" },
    {"-->> Trailen tried 'help burger'. That helpfile does not exist.", "Wiznet" },
    {"-->> Mortanius tried 'help jackoff'. That helpfile does not exist.", "Wiznet" },
    {"I'll give someone my first born child for a restore on my alt.", "Wolfyn" },
    {"kok kok kok kok kok kok kok kok kok give me a kok", "Kolfaxi" },
    {"My grandmother gave me a wish.\n\rI could either have a perfect memory or a huge penis.\n\rI can't remember my decision", "Nuyuzai" },
    {"This was null","Rhaelar"}
};

/* for position */
const struct position_type position_table[] = {
    {"dead", "dead"},
    {"mortally wounded", "mort"},
    {"incapacitated", "incap"},
    {"stunned", "stun"},
    {"sleeping", "sleep"},
    {"resting", "rest"},
    {"sitting", "sit"},
    {"fighting", "fight"},
    {"standing", "stand"},
    {NULL, NULL}
};

/* for sex */
const struct sex_type sex_table[] = {
    {"none"},
    {"male"},
    {"female"},
    {"either"},
    {NULL}
};

/* for sizes */
const struct size_type size_table[] = {
    {"tiny"},
    {"small"},
    {"medium"},
    {"large"},
    {"huge",},
    {"giant"},
    {NULL}
};

/* various flag tables */
const struct flag_type act_flags[] = {
    {"npc", A, FALSE},
    {"sentinel", B, TRUE},
    {"scavenger", C, TRUE},
    {"key", D, FALSE},
    {"ranger", E, FALSE},
    {"aggressive", F, TRUE},
    {"stay_area", G, TRUE},
    {"wimpy", H, TRUE},
    {"pet", I, TRUE},
    {"train", J, TRUE},
    {"practice", K, TRUE},
    {"druid", L, FALSE},
    {"no_body", M, TRUE},
    {"nobd_drop", N, TRUE},
    {"undead", O, TRUE},
    {"vampire", P, FALSE},
    {"cleric", Q, FALSE},
    {"mage", R, FALSE},
    {"thief", S, FALSE},
    {"warrior", T, FALSE},
    {"noalign", U, TRUE},
    {"nopurge", V, TRUE},
    {"outdoors", W, TRUE},
    {"is_satan", X, TRUE},
    {"indoors", Y, TRUE},
    {"is_priest", Z, TRUE},
    {"healer", aa, TRUE},
    {"gain", bb, TRUE},
    {"update_always", cc, TRUE},
    {"changer", dd, TRUE},
/*    {"dealer", ee, TRUE}, */
    {NULL, 0, FALSE}
};

/* various flag tables */
const struct flag_type act2_flags[] = {
    {"forger", A, TRUE},
    {"dealer", B, TRUE},
    {"banker", C, TRUE},
    {"bounty", D, TRUE},
    {NULL, 0, FALSE}
};

const struct flag_type plr_flags[] = {
    {"npc", A, FALSE},
    {"color", B, FALSE},
    {"autoassist", C, FALSE},
    {"autoexit", D, FALSE},
    {"autoloot", E, FALSE},
    {"autosac", F, FALSE},
    {"autogold", G, FALSE},
    {"autosplit", H, FALSE},
    {"notran", I, FALSE},
    {"norestore", L, FALSE},
    {"notitle", M, FALSE},
    {"holylight", N, FALSE},
    {"can_loot", P, FALSE},
    {"nosummon", Q, FALSE},
    {"nofollow", R, FALSE},
    {"permit", U, TRUE},
    {"log", W, FALSE},
    {"deny", X, FALSE},
    {"freeze", Y, FALSE},
    {"twit", Z, FALSE},
    {"key", bb, FALSE},
    {"noclan", cc, FALSE},
    {"questor", S, TRUE},
    //   {"gquest", ee, TRUE},
    {NULL, 0, 0}
};

const struct clan_titles clan_rank_table[] = {
    {
     {"{GW{gildthing ", "{CF{creeman ", "{MF{mreewoman "}
     },
    {
     {"{GR{gecruit ", "{CR{cecruit ", "{MR{mecruit "}
     },
    {
     {"{GA{gcolyte ", "{CA{ccolyte ", "{MA{mcolyte "}
     },
    {
     {"{GA{gdept ", "{CA{cdept ", "{MA{mdept "}
     },
    {
     {"{GO{gfficer ", "{CO{cfficer ", "{MO{mfficer "}
     },
    {
     {"{GL{gieutenant ", "{CL{cieutenant ", "{ML{mieutenant "}
     },
    {
     {"{GV{gassal ", "{CV{cassal ", "{MV{massal "}
     },
    {
     {"{GA{gmbassador ", "{CA{cmbassador ", "{MA{mmbassadress "}
     },
    {
     {"{GM{gagistrate ", "{CM{cagistrate ", "{MM{magistrate "}
     },
    {
     {"{GL{gord ", "{CL{cord ", "{ML{mady "}
     },
    {
     {"{GL{geader ", "{CL{ceader ", "{ML{meader "}
     },
    {
     {"{GP{gatron ", "{CP{catron ", "{MP{matron "}
     },
    {
     {NULL, NULL, NULL}
     }
};

const struct flag_type plr2_flags[] = {
    {"challenged", A, FALSE},
    {"challenger", B, FALSE},
    {"consent", C, FALSE},
    {"true_trust", D, TRUE},
    {"wiped", E, FALSE},
    {"pkviolent", F, FALSE},
    {"pksafe", G, FALSE},
    {NULL, 0, 0}
};

const struct flag_type affect_flags[] = {
    {"blind", A, TRUE},
    {"detect_evil", C, TRUE},
    {"detect_invis", D, TRUE},
    {"detect_magic", E, TRUE},
    {"detect_hidden", F, TRUE},
    {"detect_good", G, TRUE},
    {"faerie_fire", I, TRUE},
    {"infrared", J, TRUE},
    {"curse", K, TRUE},
    {"farsight", L, TRUE},
    {"poison", M, TRUE},
    {"sneak", P, TRUE},
    {"hide", Q, TRUE},
    {"sleep", R, TRUE},
    {"charm", S, TRUE},
    {"flying", T, TRUE},
    {"pass_door", U, TRUE},
    {"haste", V, TRUE},
    {"calm", W, TRUE},
    {"plague", X, TRUE},
    {"weaken", Y, TRUE},
    {"dark_vision", Z, TRUE},
    {"berserk", aa, TRUE},
    {"swim", bb, TRUE},
    {"regeneration", cc, TRUE},
    {"slow", dd, TRUE},
    {NULL, 0, 0}
};

const struct flag_type shield_flags[] = {
    {"protect_voodoo", A, TRUE},
    {"invisible", B, TRUE},
    {"ice", C, TRUE},
    {"fire", D, TRUE},
    {"shock", E, TRUE},
    {"acid", F, TRUE},
    {"poison", G, TRUE},
    {"briar", I, TRUE},
    {"sanctuary", H, TRUE},
    {"protect_evil", N, TRUE},
    {"protect_good", O, TRUE},
    {"divine protection", P, TRUE},
    {"ward", S, TRUE},
    {NULL, 0, 0}
};

const struct flag_type off_flags[] = {
    {"area_attack", A, TRUE},
    {"backstab", B, TRUE},
    {"bash", C, TRUE},
    {"berserk", D, TRUE},
    {"disarm", E, TRUE},
    {"dodge", F, TRUE},
    {"fade", G, TRUE},
    {"fast", H, TRUE},
    {"kick", I, TRUE},
    {"dirt_kick", J, TRUE},
    {"parry", K, TRUE},
    {"rescue", L, TRUE},
    {"tail", M, TRUE},
    {"trip", N, TRUE},
    {"crush", O, TRUE},
    {"assist_all", P, TRUE},
    {"assist_align", Q, TRUE},
    {"assist_race", R, TRUE},
    {"assist_players", S, TRUE},
    {"assist_guard", T, TRUE},
    {"assist_vnum", U, TRUE},
    {NULL, 0, 0}
};

const struct flag_type imm_flags[] = {
    {"summon", A, TRUE},
    {"charm", B, TRUE},
    {"magic", C, TRUE},
    {"weapon", D, TRUE},
    {"bash", E, TRUE},
    {"pierce", F, TRUE},
    {"slash", G, TRUE},
    {"fire", H, TRUE},
    {"cold", I, TRUE},
    {"lightning", J, TRUE},
    {"acid", K, TRUE},
    {"poison", L, TRUE},
    {"negative", M, TRUE},
    {"holy", N, TRUE},
    {"energy", O, TRUE},
    {"mental", P, TRUE},
    {"disease", Q, TRUE},
    {"drowning", R, TRUE},
    {"light", S, TRUE},
    {"sound", T, TRUE},
    {"wood", X, TRUE},
    {"silver", Y, TRUE},
    {"iron", Z, TRUE},
    {NULL, 0, 0}
};

const struct flag_type item_extra[] = {
    {"glowing", A, TRUE},
    {"humming", B, TRUE},
    {"dark", C, TRUE},
    {"lock", D, FALSE},
    {"evil", E, TRUE},
    {"invisible", F, TRUE},
    {"magic", G, TRUE},
    {"nodrop", H, TRUE},
    {"bless", I, TRUE},
    {"anti_good", J, TRUE},
    {"anti_evil", K, TRUE},
    {"anti_neutral", L, TRUE},
    {"noremove", M, TRUE},
    {"inventory", N, TRUE},
    {"nopurge", O, TRUE},
    {"rot_death", P, TRUE},
    {"vis_death", Q, TRUE},
    {"nosacrifice", R, TRUE},
    {"nonmetal", S, FALSE},
    {"nolocate", T, TRUE},
    {"melt_drop", U, TRUE},
    {"had_timer", U, FALSE},
    {"sell_extract", W, TRUE},
    {"burn_proof", Y, TRUE},
    {"nouncurse", Z, TRUE},
    {"quest", aa, TRUE},
    {"forced", bb, TRUE},
    {"questpoint", cc, TRUE},
    {"nosteal", dd, TRUE},
    {NULL, 0, 0}
};

const struct flag_type item_wear[] = {
    {"take", A, TRUE},
    {"finger", B, TRUE},
    {"neck", C, TRUE},
    {"head", E, TRUE},
    {"legs", F, TRUE},
    {"feet", G, TRUE},
    {"hands", H, TRUE},
    {"arms", I, TRUE},
    {"shield", J, TRUE},
    {"body", K, TRUE},
    {"waist", L, TRUE},
    {"wrist", M, TRUE},
    {"wield", N, TRUE},
    {"hold", O, TRUE},
    {"no_sac", P, TRUE},
    {"float", Q, TRUE},
    {"face", R, TRUE},
    {"ankle", S, TRUE},
    {"tattoo", T, TRUE},
    {"aura", U, TRUE},
    {"ear", V, TRUE},
    {"ctattoo", W, TRUE},
    {NULL, 0, 0}
};

const struct flag_type item_weapon[] = {
    {"flaming", A, TRUE},
    {"frost", B, TRUE},
    {"vampiric", C, TRUE},
    {"sharp", D, TRUE},
    {"vorpal", E, TRUE},
    {"two-handed", F, TRUE},
    {"shocking", G, TRUE},
    {"poisoned", H, TRUE},
    {"manadrain", I, TRUE},
    {NULL, 0, 0}
};

const struct flag_type area_room[] = {
    {"dark", A, TRUE},
    {"no_mobs", C, TRUE},
    {"indoors", D, TRUE},
    {"private", J, TRUE},
    {"safe", K, TRUE},
    {"solitary", L, TRUE},
    {"pet_shop", M, FALSE},
    {"no_recall", N, TRUE},
    {"imp", O, FALSE},
    {"gods", P, FALSE},
    {"heroes", Q, FALSE},
    {"newbies", R, FALSE},
    {"law", S, TRUE},
    {"nowhere", T, FALSE},
    {"locked", X, FALSE},
    {NULL, 0, 0}
};

const struct flag_type form_flags[] = {
    {"edible", FORM_EDIBLE, TRUE},
    {"poison", FORM_POISON, TRUE},
    {"magical", FORM_MAGICAL, TRUE},
    {"instant_decay", FORM_INSTANT_DECAY, TRUE},
    {"other", FORM_OTHER, TRUE},
    {"animal", FORM_ANIMAL, TRUE},
    {"sentient", FORM_SENTIENT, TRUE},
    {"undead", FORM_UNDEAD, TRUE},
    {"construct", FORM_CONSTRUCT, TRUE},
    {"mist", FORM_MIST, TRUE},
    {"intangible", FORM_INTANGIBLE, TRUE},
    {"biped", FORM_BIPED, TRUE},
    {"centaur", FORM_CENTAUR, TRUE},
    {"insect", FORM_INSECT, TRUE},
    {"spider", FORM_SPIDER, TRUE},
    {"crustacean", FORM_CRUSTACEAN, TRUE},
    {"worm", FORM_WORM, TRUE},
    {"blob", FORM_BLOB, TRUE},
    {"mammal", FORM_MAMMAL, TRUE},
    {"bird", FORM_BIRD, TRUE},
    {"reptile", FORM_REPTILE, TRUE},
    {"snake", FORM_SNAKE, TRUE},
    {"dragon", FORM_DRAGON, TRUE},
    {"amphibian", FORM_AMPHIBIAN, TRUE},
    {"fish", FORM_FISH, TRUE},
    {"cold_blood", FORM_COLD_BLOOD, TRUE},
    {NULL, 0, 0}
};

const struct flag_type part_flags[] = {
    {"head", PART_HEAD, TRUE},
    {"arms", PART_ARMS, TRUE},
    {"legs", PART_LEGS, TRUE},
    {"heart", PART_HEART, TRUE},
    {"brains", PART_BRAINS, TRUE},
    {"guts", PART_GUTS, TRUE},
    {"hands", PART_HANDS, TRUE},
    {"feet", PART_FEET, TRUE},
    {"fingers", PART_FINGERS, TRUE},
    {"ear", PART_EAR, TRUE},
    {"eye", PART_EYE, TRUE},
    {"long_tongue", PART_LONG_TONGUE, TRUE},
    {"eyestalks", PART_EYESTALKS, TRUE},
    {"tentacles", PART_TENTACLES, TRUE},
    {"fins", PART_FINS, TRUE},
    {"wings", PART_WINGS, TRUE},
    {"tail", PART_TAIL, TRUE},
    {"claws", PART_CLAWS, TRUE},
    {"fangs", PART_FANGS, TRUE},
    {"horns", PART_HORNS, TRUE},
    {"scales", PART_SCALES, TRUE},
    {"tusks", PART_TUSKS, TRUE},
    {NULL, 0, 0}
};

const struct flag_type comm_flags[] = {
    {"quiet", COMM_QUIET, TRUE},
    {"deaf", COMM_DEAF, TRUE},
    {"nowiz", COMM_NOWIZ, TRUE},
    {"noclangossip", COMM_NOOOC, TRUE},
    {"nogossip", COMM_NOGOSSIP, TRUE},
    {"nocgossip", COMM_NOCGOSSIP, TRUE},
    {"noqgossip", COMM_NOQGOSSIP, TRUE},
    {"noask", COMM_NOASK, TRUE},
    {"nomusic", COMM_NOMUSIC, TRUE},
    {"noclan", COMM_NOCLAN, TRUE},
    {"nosocial", COMM_NOSOCIAL, TRUE},
    {"noquote", COMM_NOQUOTE, TRUE},
    /* {   "shoutsoff",          COMM_SHOUTSOFF,         TRUE    }, */
/*    {"noarena", COMM_NOARENA, TRUE}, */
    /* {   "true_trust",         COMM_TRUE_TRUST,        TRUE    }, */
    {"compact", COMM_COMPACT, TRUE},
    {"brief", COMM_BRIEF, TRUE},
    {"noannounce", COMM_NOANNOUNCE, TRUE},
    {"prompt", COMM_PROMPT, TRUE},
    {"long", COMM_LONG, TRUE},
    {"store", COMM_STORE, TRUE},
    {"combine", COMM_COMBINE, TRUE},
/*    {"telnet_ga", COMM_TELNET_GA, TRUE},*/
    {"show_affects", COMM_SHOW_AFFECTS, TRUE},
    {"nograts", COMM_NOGRATS, TRUE},
    /* {   "wiped",              COMM_WIPED,             FALSE   }, */
    {"noemote", COMM_NOEMOTE, FALSE},
    {"noshout", COMM_NOSHOUT, FALSE},
    {"notell", COMM_NOTELL, FALSE},
    {"nochannels", COMM_NOCHANNELS, FALSE},
/*    {"nocheer", COMM_NOCHEER, FALSE}, */
/*    {"snoop_proof", COMM_SNOOP_PROOF, FALSE}, */
    {"afk", COMM_AFK, FALSE},
    {"norace", COMM_NORACE, TRUE},
    {"curse", COMM_CURSE, TRUE},
    {"nohint", COMM_NOHINT, FALSE},
    {"nogsocial", COMM_NOGSOCIAL, FALSE},
    {"nosay", COMM_NOSAY, FALSE},

    {NULL, 0, 0}
};

const struct flag_type mprog_flags[] = {
    {"act", TRIG_ACT, TRUE},
    {"bribe", TRIG_BRIBE, TRUE},
    {"death", TRIG_DEATH, TRUE},
    {"entry", TRIG_ENTRY, TRUE},
    {"fight", TRIG_FIGHT, TRUE},
    {"give", TRIG_GIVE, TRUE},
    {"greet", TRIG_GREET, TRUE},
    {"grall", TRIG_GRALL, TRUE},
    {"kill", TRIG_KILL, TRUE},
    {"hpcnt", TRIG_HPCNT, TRUE},
    {"random", TRIG_RANDOM, TRUE},
    {"speech", TRIG_SPEECH, TRUE},
    {"exit", TRIG_EXIT, TRUE},
    {"exall", TRIG_EXALL, TRUE},
    {"delay", TRIG_DELAY, TRUE},
    {"surr", TRIG_SURR, TRUE},
    {NULL, 0, TRUE}
};

const struct flag_type area_flags[] = {
    {"none", AREA_NONE, FALSE},
    {"changed", AREA_CHANGED, TRUE},
    {"added", AREA_ADDED, TRUE},
    {"loading", AREA_LOADING, FALSE},
    {NULL, 0, 0}
};

const struct flag_type sex_flags[] = {
    {"male", SEX_MALE, TRUE},
    {"female", SEX_FEMALE, TRUE},
    {"neutral", SEX_NEUTRAL, TRUE},
    {"random", 3, TRUE},        /* ROM */
    {"none", SEX_NEUTRAL, TRUE},
    {NULL, 0, 0}
};

const struct flag_type exit_flags[] = {
    {"door", EX_ISDOOR, TRUE},
    {"closed", EX_CLOSED, TRUE},
    {"locked", EX_LOCKED, TRUE},
    {"pickproof", EX_PICKPROOF, TRUE},
    {"nopass", EX_NOPASS, TRUE},
    {"easy", EX_EASY, TRUE},
    {"hard", EX_HARD, TRUE},
    {"infuriating", EX_INFURIATING, TRUE},
    {"noclose", EX_NOCLOSE, TRUE},
    {"nolock", EX_NOLOCK, TRUE},
    {NULL, 0, 0}
};

const struct flag_type door_resets[] = {
    {"open and unlocked", 0, TRUE},
    {"closed and unlocked", 1, TRUE},
    {"closed and locked", 2, TRUE},
    {NULL, 0, 0}
};

const struct flag_type room_flags[] = {
    {"dark", ROOM_DARK, TRUE},
    {"no_mob", ROOM_NO_MOB, TRUE},
    {"indoors", ROOM_INDOORS, TRUE},
    {"private", ROOM_PRIVATE, TRUE},
    {"safe", ROOM_SAFE, TRUE},
    {"solitary", ROOM_SOLITARY, TRUE},
    {"pet_shop", ROOM_PET_SHOP, TRUE},
    {"no_recall", ROOM_NO_RECALL, TRUE},
    {"imp_only", ROOM_IMP_ONLY, TRUE},
    {"gods_only", ROOM_GODS_ONLY, TRUE},
    {"heroes_only", ROOM_HEROES_ONLY, TRUE},
    {"newbies_only", ROOM_NEWBIES_ONLY, TRUE},
    {"law", ROOM_LAW, TRUE},
    {"arena", ROOM_ARENA, TRUE},
    {"nowhere", ROOM_NOWHERE, TRUE},
    {"home", ROOM_HOME, TRUE},
    {"teleport", ROOM_TELEPORT, TRUE},
    {NULL, 0, 0}
};

const struct flag_type sector_flags[] = {
    {"inside", SECT_INSIDE, TRUE},
    {"city", SECT_CITY, TRUE},
    {"field", SECT_FIELD, TRUE},
    {"forest", SECT_FOREST, TRUE},
    {"hills", SECT_HILLS, TRUE},
    {"mountain", SECT_MOUNTAIN, TRUE},
    {"swim", SECT_WATER_SWIM, TRUE},
    {"noswim", SECT_WATER_NOSWIM, TRUE},
    {"unused", SECT_UNUSED, TRUE},
    {"air", SECT_AIR, TRUE},
    {"desert", SECT_DESERT, TRUE},
    {NULL, 0, 0}
};

const struct flag_type type_flags[] = {
    {"light", ITEM_LIGHT, TRUE},
    {"scroll", ITEM_SCROLL, TRUE},
    {"wand", ITEM_WAND, TRUE},
    {"staff", ITEM_STAFF, TRUE},
    {"weapon", ITEM_WEAPON, TRUE},
    {"treasure", ITEM_TREASURE, TRUE},
    {"armor", ITEM_ARMOR, TRUE},
    {"potion", ITEM_POTION, TRUE},
    {"furniture", ITEM_FURNITURE, TRUE},
    {"trash", ITEM_TRASH, TRUE},
    {"container", ITEM_CONTAINER, TRUE},
    {"drinkcontainer", ITEM_DRINK_CON, TRUE},
    {"key", ITEM_KEY, TRUE},
    {"food", ITEM_FOOD, TRUE},
    {"money", ITEM_MONEY, TRUE},
    {"boat", ITEM_BOAT, TRUE},
    {"npccorpse", ITEM_CORPSE_NPC, TRUE},
    {"pc corpse", ITEM_CORPSE_PC, FALSE},
    {"fountain", ITEM_FOUNTAIN, TRUE},
    {"pill", ITEM_PILL, TRUE},
    /* { "protect",              ITEM_PROTECT,           TRUE    }, */
    {"map", ITEM_MAP, TRUE},
    {"portal", ITEM_PORTAL, TRUE},
    {"warpstone", ITEM_WARP_STONE, TRUE},
    {"roomkey", ITEM_ROOM_KEY, TRUE},
    {"gem", ITEM_GEM, TRUE},
    {"jewelry", ITEM_JEWELRY, TRUE},
    {"jukebox", ITEM_JUKEBOX, TRUE},
    {"demonstone", ITEM_DEMON_STONE, TRUE},
    {"slotmachine", ITEM_SLOT_MACHINE, TRUE},
    {"pit", ITEM_PIT, TRUE},
    {NULL, 0, 0}
};

const struct flag_type extra_flags[] = {
    {"glow", ITEM_GLOW, TRUE},
    {"hum", ITEM_HUM, TRUE},
    {"dark", ITEM_DARK, TRUE},
    {"lock", ITEM_LOCK, TRUE},
    {"evil", ITEM_EVIL, TRUE},
    {"invis", ITEM_INVIS, TRUE},
    {"magic", ITEM_MAGIC, TRUE},
    {"nodrop", ITEM_NODROP, TRUE},
    {"bless", ITEM_BLESS, TRUE},
    {"antigood", ITEM_ANTI_GOOD, TRUE},
    {"antievil", ITEM_ANTI_EVIL, TRUE},
    {"antineutral", ITEM_ANTI_NEUTRAL, TRUE},
    {"noremove", ITEM_NOREMOVE, TRUE},
    {"inventory", ITEM_INVENTORY, TRUE},
    {"nopurge", ITEM_NOPURGE, TRUE},
    {"rotdeath", ITEM_ROT_DEATH, TRUE},
    {"visdeath", ITEM_VIS_DEATH, TRUE},
    {"nonmetal", ITEM_NONMETAL, TRUE},
    {"meltdrop", ITEM_MELT_DROP, TRUE},
    {"hadtimer", ITEM_HAD_TIMER, TRUE},
    {"sellextract", ITEM_SELL_EXTRACT, TRUE},
    {"burnproof", ITEM_BURN_PROOF, TRUE},
    {"nouncurse", ITEM_NOUNCURSE, TRUE},
    {"quest", ITEM_QUEST, TRUE},
    {"nolocate", ITEM_NOLOCATE, TRUE},
    {"forced", ITEM_FORCED, TRUE},
    {"questpoint", ITEM_QUESTPOINT, TRUE},
    {"nosteal", ITEM_NOSTEAL, TRUE},
    {NULL, 0, 0}
};

const struct flag_type wear_flags[] = {
    {"take", ITEM_TAKE, TRUE},
    {"finger", ITEM_WEAR_FINGER, TRUE},
    {"neck", ITEM_WEAR_NECK, TRUE},
    {"body", ITEM_WEAR_BODY, TRUE},
    {"head", ITEM_WEAR_HEAD, TRUE},
    {"legs", ITEM_WEAR_LEGS, TRUE},
    {"feet", ITEM_WEAR_FEET, TRUE},
    {"hands", ITEM_WEAR_HANDS, TRUE},
    {"arms", ITEM_WEAR_ARMS, TRUE},
    {"shield", ITEM_WEAR_SHIELD, TRUE},
    {"about", ITEM_WEAR_ABOUT, TRUE},
    {"waist", ITEM_WEAR_WAIST, TRUE},
    {"wrist", ITEM_WEAR_WRIST, TRUE},
    {"wield", ITEM_WIELD, TRUE},
    {"hold", ITEM_HOLD, TRUE},
    {"nosac", ITEM_NO_SAC, TRUE},
    {"wearfloat", ITEM_WEAR_FLOAT, TRUE},
/*    {   "twohands",            ITEM_TWO_HANDS,         TRUE    }, */
    {"face", ITEM_WEAR_FACE, TRUE},
    {"ankle", ITEM_WEAR_ANKLE, TRUE},
    {"tattoo", ITEM_WEAR_TATTOO, TRUE},
    {"aura", ITEM_WEAR_AURA, TRUE},
    {"ear", ITEM_WEAR_EAR, TRUE},
    {"ctattoo", ITEM_WEAR_CTATTOO, TRUE},
    {NULL, 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] = {
    {"none", APPLY_NONE, TRUE},
    {"strength", APPLY_STR, TRUE},
    {"dexterity", APPLY_DEX, TRUE},
    {"intelligence", APPLY_INT, TRUE},
    {"wisdom", APPLY_WIS, TRUE},
    {"constitution", APPLY_CON, TRUE},
    {"sex", APPLY_SEX, TRUE},
    {"class", APPLY_CLASS, TRUE},
    {"level", APPLY_LEVEL, TRUE},
    {"age", APPLY_AGE, TRUE},
    {"height", APPLY_HEIGHT, TRUE},
    {"weight", APPLY_WEIGHT, TRUE},
    {"mana", APPLY_MANA, TRUE},
    {"hp", APPLY_HIT, TRUE},
    {"move", APPLY_MOVE, TRUE},
    {"gold", APPLY_GOLD, TRUE},
    {"experience", APPLY_EXP, TRUE},
    {"ac", APPLY_AC, TRUE},
    {"hitroll", APPLY_HITROLL, TRUE},
    {"damroll", APPLY_DAMROLL, TRUE},
    {"saves", APPLY_SAVES, TRUE},
    {"savingpara", APPLY_SAVING_PARA, TRUE},
    {"savingrod", APPLY_SAVING_ROD, TRUE},
    {"savingpetri", APPLY_SAVING_PETRI, TRUE},
    {"savingbreath", APPLY_SAVING_BREATH, TRUE},
    {"savingspell", APPLY_SAVING_SPELL, TRUE},
    {"spellaffect", APPLY_SPELL_AFFECT, FALSE},
    {NULL, 0, 0}
};

/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] = {
    {"in the inventory", WEAR_NONE, TRUE},
    {"as a light", WEAR_LIGHT, TRUE},
    {"on the left finger", WEAR_FINGER_L, TRUE},
    {"on the right finger", WEAR_FINGER_R, TRUE},
    {"around the neck (1)", WEAR_NECK_1, TRUE},
    {"around the neck (2)", WEAR_NECK_2, TRUE},
    {"on the body", WEAR_BODY, TRUE},
    {"over the head", WEAR_HEAD, TRUE},
    {"on the legs", WEAR_LEGS, TRUE},
    {"on the feet", WEAR_FEET, TRUE},
    {"on the hands", WEAR_HANDS, TRUE},
    {"on the arms", WEAR_ARMS, TRUE},
    {"as a shield", WEAR_SHIELD, TRUE},
    {"about the shoulders", WEAR_ABOUT, TRUE},
    {"around the waist", WEAR_WAIST, TRUE},
    {"on the left wrist", WEAR_WRIST_L, TRUE},
    {"on the right wrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"held in the hands", WEAR_HOLD, TRUE},
    {"floating nearby", WEAR_FLOAT, TRUE},
    {"on the face", WEAR_FACE, TRUE},
    {"on the left ankle", WEAR_ANKLE_L, TRUE},
    {"on the right ankle", WEAR_ANKLE_R, TRUE},
    {"as a tattoo", WEAR_TATTOO, TRUE},
    {"as aura", WEAR_AURA, TRUE},
    {"worn on left ear", WEAR_EAR_L, TRUE},
    {"worn on right ear", WEAR_EAR_R, TRUE},
    {"worn as clan tattoo", WEAR_CTATTOO, TRUE},
    {NULL, 0, 0}
};

const struct flag_type wear_loc_flags[] = {
    {"none", WEAR_NONE, TRUE},
    {"light", WEAR_LIGHT, TRUE},
    {"lfinger", WEAR_FINGER_L, TRUE},
    {"rfinger", WEAR_FINGER_R, TRUE},
    {"neck1", WEAR_NECK_1, TRUE},
    {"neck2", WEAR_NECK_2, TRUE},
    {"body", WEAR_BODY, TRUE},
    {"head", WEAR_HEAD, TRUE},
    {"legs", WEAR_LEGS, TRUE},
    {"feet", WEAR_FEET, TRUE},
    {"hands", WEAR_HANDS, TRUE},
    {"arms", WEAR_ARMS, TRUE},
    {"shield", WEAR_SHIELD, TRUE},
    {"about", WEAR_ABOUT, TRUE},
    {"waist", WEAR_WAIST, TRUE},
    {"lwrist", WEAR_WRIST_L, TRUE},
    {"rwrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"hold", WEAR_HOLD, TRUE},
    {"floating", WEAR_FLOAT, TRUE},
    {"face", WEAR_FACE, TRUE},
    {"ankle", WEAR_ANKLE_L, TRUE},
    {"ankle", WEAR_ANKLE_R, TRUE},
    {"tattoo", WEAR_TATTOO, TRUE},
    {"aura", WEAR_AURA, TRUE},
    {"ear", WEAR_EAR_L, TRUE},
    {"ear", WEAR_EAR_R, TRUE},
    {"clan tattoo", WEAR_CTATTOO, TRUE},
    {NULL, 0, 0}

};

const struct flag_type container_flags[] = {
    {"closeable", 1, TRUE},
    {"pickproof", 2, TRUE},
    {"closed", 4, TRUE},
    {"locked", 8, TRUE},
    {"puton", 16, TRUE},
    {NULL, 0, 0}
};

/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/

const struct flag_type ac_type[] = {
    {"pierce", AC_PIERCE, TRUE},
    {"bash", AC_BASH, TRUE},
    {"slash", AC_SLASH, TRUE},
    {"exotic", AC_EXOTIC, TRUE},
    {NULL, 0, 0}
};

const struct flag_type size_flags[] = {
    {"tiny", SIZE_TINY, TRUE},
    {"small", SIZE_SMALL, TRUE},
    {"medium", SIZE_MEDIUM, TRUE},
    {"large", SIZE_LARGE, TRUE},
    {"huge", SIZE_HUGE, TRUE},
    {"giant", SIZE_GIANT, TRUE},
    {NULL, 0, 0},
};

const struct flag_type weapon_class[] = {
    {"exotic", WEAPON_EXOTIC, TRUE},
    {"sword", WEAPON_SWORD, TRUE},
    {"dagger", WEAPON_DAGGER, TRUE},
    {"spear", WEAPON_SPEAR, TRUE},
    {"mace", WEAPON_MACE, TRUE},
    {"axe", WEAPON_AXE, TRUE},
    {"flail", WEAPON_FLAIL, TRUE},
    {"whip", WEAPON_WHIP, TRUE},
    {"polearm", WEAPON_POLEARM, TRUE},
    {NULL, 0, 0}
};

const struct flag_type weapon_type2[] = {
    {"flaming", WEAPON_FLAMING, TRUE},
    {"frost", WEAPON_FROST, TRUE},
    {"vampiric", WEAPON_VAMPIRIC, TRUE},
    {"sharp", WEAPON_SHARP, TRUE},
    {"vorpal", WEAPON_VORPAL, TRUE},
    {"twohands", WEAPON_TWO_HANDS, TRUE},
    {"shocking", WEAPON_SHOCKING, TRUE},
    {"poison", WEAPON_POISON, TRUE},
    {"manadrain", WEAPON_MANADRAIN, TRUE},
    {NULL, 0, 0}
};

const struct flag_type res_flags[] = {
    {"summon", RES_SUMMON, TRUE},
    {"charm", RES_CHARM, TRUE},
    {"magic", RES_MAGIC, TRUE},
    {"weapon", RES_WEAPON, TRUE},
    {"bash", RES_BASH, TRUE},
    {"pierce", RES_PIERCE, TRUE},
    {"slash", RES_SLASH, TRUE},
    {"fire", RES_FIRE, TRUE},
    {"cold", RES_COLD, TRUE},
    {"lightning", RES_LIGHTNING, TRUE},
    {"acid", RES_ACID, TRUE},
    {"poison", RES_POISON, TRUE},
    {"negative", RES_NEGATIVE, TRUE},
    {"holy", RES_HOLY, TRUE},
    {"energy", RES_ENERGY, TRUE},
    {"mental", RES_MENTAL, TRUE},
    {"disease", RES_DISEASE, TRUE},
    {"drowning", RES_DROWNING, TRUE},
    {"light", RES_LIGHT, TRUE},
    {"sound", RES_SOUND, TRUE},
    {"wood", RES_WOOD, TRUE},
    {"silver", RES_SILVER, TRUE},
    {"iron", RES_IRON, TRUE},
    {NULL, 0, 0}
};

const struct flag_type vuln_flags[] = {
    {"summon", VULN_SUMMON, TRUE},
    {"charm", VULN_CHARM, TRUE},
    {"magic", VULN_MAGIC, TRUE},
    {"weapon", VULN_WEAPON, TRUE},
    {"bash", VULN_BASH, TRUE},
    {"pierce", VULN_PIERCE, TRUE},
    {"slash", VULN_SLASH, TRUE},
    {"fire", VULN_FIRE, TRUE},
    {"cold", VULN_COLD, TRUE},
    {"lightning", VULN_LIGHTNING, TRUE},
    {"acid", VULN_ACID, TRUE},
    {"poison", VULN_POISON, TRUE},
    {"negative", VULN_NEGATIVE, TRUE},
    {"holy", VULN_HOLY, TRUE},
    {"energy", VULN_ENERGY, TRUE},
    {"mental", VULN_MENTAL, TRUE},
    {"disease", VULN_DISEASE, TRUE},
    {"drowning", VULN_DROWNING, TRUE},
    {"light", VULN_LIGHT, TRUE},
    {"sound", VULN_SOUND, TRUE},
    {"wood", VULN_WOOD, TRUE},
    {"silver", VULN_SILVER, TRUE},
    {"iron", VULN_IRON, TRUE},
    {NULL, 0, 0}
};

const struct flag_type position_flags[] = {
    {"dead", POS_DEAD, FALSE},
    {"mortal", POS_MORTAL, FALSE},
    {"incap", POS_INCAP, FALSE},
    {"stunned", POS_STUNNED, FALSE},
    {"sleeping", POS_SLEEPING, TRUE},
    {"resting", POS_RESTING, TRUE},
    {"sitting", POS_SITTING, TRUE},
    {"fighting", POS_FIGHTING, FALSE},
    {"standing", POS_STANDING, TRUE},
    {NULL, 0, 0}
};

const struct flag_type portal_flags[] = {
    {"normal_exit", GATE_NORMAL_EXIT, TRUE},
    {"no_curse", GATE_NOCURSE, TRUE},
    {"go_with", GATE_GOWITH, TRUE},
    {"buggy", GATE_BUGGY, TRUE},
    {"random", GATE_RANDOM, TRUE},
    {NULL, 0, 0}
};

const struct flag_type furniture_flags[] = {
    {"stand_at", STAND_AT, TRUE},
    {"stand_on", STAND_ON, TRUE},
    {"stand_in", STAND_IN, TRUE},
    {"sit_at", SIT_AT, TRUE},
    {"sit_on", SIT_ON, TRUE},
    {"sit_in", SIT_IN, TRUE},
    {"rest_at", REST_AT, TRUE},
    {"rest_on", REST_ON, TRUE},
    {"rest_in", REST_IN, TRUE},
    {"sleep_at", SLEEP_AT, TRUE},
    {"sleep_on", SLEEP_ON, TRUE},
    {"sleep_in", SLEEP_IN, TRUE},
    {"put_at", PUT_AT, TRUE},
    {"put_on", PUT_ON, TRUE},
    {"put_in", PUT_IN, TRUE},
    {"put_inside", PUT_INSIDE, TRUE},
    {NULL, 0, 0}
};

const struct flag_type apply_types[] = {
    {"affects", TO_AFFECTS, TRUE},
    {"object", TO_OBJECT, TRUE},
    {"immune", TO_IMMUNE, TRUE},
    {"resist", TO_RESIST, TRUE},
    {"vuln", TO_VULN, TRUE},
    {"weapon", TO_WEAPON, TRUE},
    {NULL, 0, TRUE}
};

const struct bit_type bitvector_type[] = {
    {affect_flags, "affect"},
    {apply_flags, "apply"},
    {imm_flags, "imm"},
    {res_flags, "res"},
    {vuln_flags, "vuln"},
    {weapon_type2, "weapon"}
};

const	struct	hometown_type	hometown_table [] =
{
/*	{	"name",		recall,	school,	morgue,	death	},	*/
	{	"SilverXire",	3014,	3700,	2932,	3054	},
	{	"Maerkai",	18052,	3700,	2932,	18053	},
	{	"Vrugar",	1405,	3700,	2932,	1405	},
	{	"Lemtorak",	353,	3700,	2932,	355	},	
	{	"Drakyri",	11719,	3700,	2932,	11794	},	
	{	NULL,		3014,	3700,	2932,	3054	}
};
