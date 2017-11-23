/***************************************************************************
 *                                                                         *
 *   .oOOOo.    o                                        o                 *
 *  .O     o   O                             o          O                  *
 *  o          o                                        o                  *
 *  o          O                                        O                  *
 *  o          OoOo.  `OoOo.  .oOo.  'OoOo.  O   .oOo   o   .oOo.  .oOo    *
 *  O          o   o   o      O   o   o   O  o   O      O   OooO'  `Ooo.   *
 *  `o     .o  o   O   O      o   O   O   o  O   o      o   O          O   *
 *   `OoooO'   O   o   o      `OoO'   o   O  o'  `OoO'  Oo  `OoO'  `OoO'   *
 *                                                                         *
 ***************************************************************************
 * - Chronicles Copyright 2001-2003 by Brad Ensley (Orion Elder)           *
 ***************************************************************************
 * - SMAUG 1.4  Copyright 1994, 1995, 1996, 1998 by Derek Snider           *
 * - Merc  2.1  Copyright 1992, 1993 by Michael Chastain, Michael Quan,    *
 *   and Mitchell Tse.                                                     *
 * - DikuMud    Copyright 1990, 1991 by Sebastian Hammer, Michael Seifert, *
 *   Hans-Henrik St�rfeldt, Tom Madsen, and Katja Nyboe.                   *
 ***************************************************************************
 * - Table load/save module                                                *
 ***************************************************************************/

#include <limits.h>
#include <string.h>
#include "mud.h"
#include "files.h"
#include "language.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

bool load_race_file( char *fname );
void write_race_file( int ra );


/* global variables */
int top_sn;
int top_herb;
int MAX_PC_CLASS;
int MAX_PC_RACE;

SKILLTYPE *		skill_table	[MAX_SKILL];
struct	class_type *	class_table	[MAX_CLASS];
RACE_TYPE *		race_table	[MAX_RACE];
char *			title_table	[MAX_CLASS]
					[MAX_LEVEL+1]
					[2];
SKILLTYPE *		herb_table	[MAX_HERB];
SKILLTYPE *		disease_table	[MAX_DISEASE];

LANG_DATA *		first_lang;
LANG_DATA *		last_lang;

char * const skill_tname[] = { "unknown", "Spell", "Skill", "Weapon", "Tongue", "Herb", "Racial", "Disease" };

SPELL_FUN *spell_function( char *name )
{
    if ( !str_cmp( name, "spell_smaug" ))	     return spell_smaug;
    if ( !str_cmp( name, "spell_acid_blast" ))	     return spell_acid_blast;
    if ( !str_cmp( name, "spell_animate_dead" ))     return spell_animate_dead;
    if ( !str_cmp( name, "spell_astral_walk" ))	     return spell_astral_walk;
    if ( !str_cmp( name, "spell_blindness" ))	     return spell_blindness;
    if ( !str_cmp( name, "spell_burning_hands" ))    return spell_burning_hands;
    if ( !str_cmp( name, "spell_call_lightning" ))   return spell_call_lightning;
    if ( !str_cmp( name, "spell_cause_critical" ))   return spell_cause_critical;
    if ( !str_cmp( name, "spell_cause_light" ))	     return spell_cause_light;
    if ( !str_cmp( name, "spell_cause_serious" ))    return spell_cause_serious;
    if ( !str_cmp( name, "spell_change_sex" ))	     return spell_change_sex;
    if ( !str_cmp( name, "spell_charm_person" ))     return spell_charm_person;
    if ( !str_cmp( name, "spell_chill_touch" ))	     return spell_chill_touch;
    if ( !str_cmp( name, "spell_colour_spray" ))     return spell_colour_spray;
    if ( !str_cmp( name, "spell_control_weather" ))  return spell_control_weather;
    if ( !str_cmp( name, "spell_create_food" ))	     return spell_create_food;
    if ( !str_cmp( name, "spell_create_water" ))     return spell_create_water;
    if ( !str_cmp( name, "spell_cure_blindness" ))   return spell_cure_blindness;
    if ( !str_cmp( name, "spell_cure_poison" ))	     return spell_cure_poison;
    if ( !str_cmp( name, "spell_curse" ))	     return spell_curse;
    if ( !str_cmp( name, "spell_detect_poison" ))    return spell_detect_poison;
    if ( !str_cmp( name, "spell_disenchant_weapon" ))   return spell_disenchant_weapon;
    if ( !str_cmp( name, "spell_dispel_evil" ))	     return spell_dispel_evil;
    if ( !str_cmp( name, "spell_dispel_magic" ))     return spell_dispel_magic;
    if ( !str_cmp( name, "spell_dream" ))	     return spell_dream;
    if ( !str_cmp( name, "spell_earthquake" ))	     return spell_earthquake;
    if ( !str_cmp( name, "spell_enchant_weapon" ))   return spell_enchant_weapon;
    if ( !str_cmp( name, "spell_energy_drain" ))     return spell_energy_drain;
    if ( !str_cmp( name, "spell_faerie_fire" ))	     return spell_faerie_fire;
    if ( !str_cmp( name, "spell_faerie_fog" ))	     return spell_faerie_fog;
    if ( !str_cmp( name, "spell_farsight" ))	     return spell_farsight;
    if ( !str_cmp( name, "spell_fireball" ))	     return spell_fireball;
    if ( !str_cmp( name, "spell_flamestrike" ))	     return spell_flamestrike;
    if ( !str_cmp( name, "spell_gate" ))	     return spell_gate;
    if ( !str_cmp( name, "spell_knock" ))	     return spell_knock;
    if ( !str_cmp( name, "spell_harm" ))	     return spell_harm;
    if ( !str_cmp( name, "spell_identify" ))	     return spell_identify;
    if ( !str_cmp( name, "spell_invis" ))	     return spell_invis;
    if ( !str_cmp( name, "spell_know_alignment" ))   return spell_know_alignment;
    if ( !str_cmp( name, "spell_lightning_bolt" ))   return spell_lightning_bolt;
    if ( !str_cmp( name, "spell_locate_object" ))    return spell_locate_object;
    if ( !str_cmp( name, "spell_magic_missile" ))    return spell_magic_missile;
    if ( !str_cmp( name, "spell_mist_walk" ))	     return spell_mist_walk;
    if ( !str_cmp( name, "spell_pass_door" ))	     return spell_pass_door;
    if ( !str_cmp( name, "spell_plant_pass" ))	     return spell_plant_pass;
    if ( !str_cmp( name, "spell_poison" ))	     return spell_poison;
    if ( !str_cmp( name, "spell_polymorph"))         return spell_polymorph;
    if ( !str_cmp( name, "spell_possess" ))	     return spell_possess;
    if ( !str_cmp( name, "spell_recharge" ))	     return spell_recharge;
    if ( !str_cmp( name, "spell_remove_curse" ))     return spell_remove_curse;
    if ( !str_cmp( name, "spell_remove_invis" ))     return spell_remove_invis;
    if ( !str_cmp( name, "spell_remove_trap" ))	     return spell_remove_trap;
    if ( !str_cmp( name, "spell_shocking_grasp" ))   return spell_shocking_grasp;
    if ( !str_cmp( name, "spell_sleep" ))	     return spell_sleep;
    if ( !str_cmp( name, "spell_solar_flight" ))     return spell_solar_flight;
    if ( !str_cmp( name, "spell_summon" ))	     return spell_summon;
    if ( !str_cmp( name, "spell_teleport" ))	     return spell_teleport;
    if ( !str_cmp( name, "spell_ventriloquate" ))    return spell_ventriloquate;
    if ( !str_cmp( name, "spell_weaken" ))	     return spell_weaken;
    if ( !str_cmp( name, "spell_word_of_recall" ))   return spell_word_of_recall;
    if ( !str_cmp( name, "spell_acid_breath" ))	     return spell_acid_breath;
    if ( !str_cmp( name, "spell_fire_breath" ))	     return spell_fire_breath;
    if ( !str_cmp( name, "spell_frost_breath" ))     return spell_frost_breath;
    if ( !str_cmp( name, "spell_gas_breath" ))	     return spell_gas_breath;
    if ( !str_cmp( name, "spell_lightning_breath" )) return spell_lightning_breath;
    if ( !str_cmp( name, "spell_spiral_blast" ))     return spell_spiral_blast;
    if ( !str_cmp( name, "spell_scorching_surge" ))  return spell_scorching_surge;
    if ( !str_cmp( name, "spell_helical_flow" ))     return spell_helical_flow;
    if ( !str_cmp( name, "spell_transport" ))	     return spell_transport;
    if ( !str_cmp( name, "spell_portal" ))	     return spell_portal;

    if ( !str_cmp( name, "spell_ethereal_fist" ))    return spell_ethereal_fist;
    if ( !str_cmp( name, "spell_spectral_furor" ))   return spell_spectral_furor;
    if ( !str_cmp( name, "spell_hand_of_chaos" ))    return spell_hand_of_chaos;
    if ( !str_cmp( name, "spell_disruption" ))	     return spell_disruption;
    if ( !str_cmp( name, "spell_sonic_resonance" ))  return spell_sonic_resonance;
    if ( !str_cmp( name, "spell_mind_wrack" ))	     return spell_mind_wrack;
    if ( !str_cmp( name, "spell_mind_wrench" ))	     return spell_mind_wrench;
    if ( !str_cmp( name, "spell_revive" ))	     return spell_revive;
    if ( !str_cmp( name, "spell_sulfurous_spray" ))  return spell_sulfurous_spray;
    if ( !str_cmp( name, "spell_caustic_fount" ))    return spell_caustic_fount;
    if ( !str_cmp( name, "spell_acetum_primus" ))    return spell_acetum_primus;
    if ( !str_cmp( name, "spell_galvanic_whip" ))    return spell_galvanic_whip;
    if ( !str_cmp( name, "spell_magnetic_thrust" ))  return spell_magnetic_thrust;
    if ( !str_cmp( name, "spell_quantum_spike" ))    return spell_quantum_spike;
    if ( !str_cmp( name, "spell_black_hand" ))	     return spell_black_hand;
    if ( !str_cmp( name, "spell_black_fist" ))	     return spell_black_fist;
    if ( !str_cmp( name, "spell_black_lightning" ))  return spell_black_lightning;
    if ( !str_cmp( name, "spell_midas_touch" ))      return spell_midas_touch;
    if ( !str_cmp( name, "spell_bethsaidean_touch")) return spell_bethsaidean_touch;
    if ( !str_cmp( name, "spell_expurgation" ))	     return spell_expurgation;
    if ( !str_cmp( name, "spell_sacral_divinity" ))  return spell_sacral_divinity;

    if ( !str_cmp( name, "reserved" ))		     return NULL;
    if ( !str_cmp( name, "spell_null" ))	     return spell_null;
    return spell_notfound;
}

DO_FUN *skill_function( char *name )
{
    switch( name[3] )
    {
    case 'a':
	if ( !str_cmp( name, "do_aassign" ))		return do_aassign;
	if ( !str_cmp( name, "do_advance" ))		return do_advance;
	if ( !str_cmp( name, "do_add_imm_host" ) )      return do_add_imm_host;
	if ( !str_cmp( name, "do_affected" ))		return do_affected;
	if ( !str_cmp( name, "do_afk" ))		return do_afk;
	if ( !str_cmp( name, "do_aid" ))		return do_aid;
	if ( !str_cmp( name, "do_allow" ))		return do_allow;
	if ( !str_cmp( name, "do_ansi" ))		return do_ansi;
	if ( !str_cmp( name, "do_answer" ))		return do_answer;
	if ( !str_cmp( name, "do_apply" ))		return do_apply;
	if ( !str_cmp( name, "do_appraise" ))		return do_appraise;
	if ( !str_cmp( name, "do_areas" ))		return do_areas;
	if ( !str_cmp( name, "do_aset" ))		return do_aset;
	if ( !str_cmp( name, "do_ask" ))		return do_ask;
	if ( !str_cmp( name, "do_astat" ))		return do_astat;
	if ( !str_cmp( name, "do_at" ))			return do_at;
	if ( !str_cmp( name, "do_atobj"))		return do_atobj;
	if ( !str_cmp( name, "do_auction" ))		return do_auction;
	if ( !str_cmp( name, "do_authorize" ))		return do_authorize;
	if ( !str_cmp( name, "do_avtalk" ))		return do_avtalk;
	break;
    case 'b':
	if ( !str_cmp( name, "do_backstab" ))		return do_backstab;
	if ( !str_cmp( name, "do_balance" ))		return do_balance;
	if ( !str_cmp( name, "do_balzhur" ))		return do_balzhur;
	if ( !str_cmp( name, "do_bamfin" ))		return do_bamfin;
	if ( !str_cmp( name, "do_bamfout" ))		return do_bamfout;
	if ( !str_cmp( name, "do_ban" ))		return do_ban;
	if ( !str_cmp( name, "do_bash" ))		return do_bash;
	if ( !str_cmp( name, "do_bashdoor" ))		return do_bashdoor;
	if ( !str_cmp( name, "do_berserk" ))		return do_berserk;
	if ( !str_cmp( name, "do_bestow" ))		return do_bestow;
	if ( !str_cmp( name, "do_bestowarea" ))		return do_bestowarea;
	if ( !str_cmp( name, "do_bio" ))		return do_bio;
        if ( !str_cmp( name, "do_bite" ))               return do_bite;
        if ( !str_cmp( name, "do_bloodlet" ))           return do_bloodlet;
	if ( !str_cmp( name, "do_boards" ))		return do_boards;
	if ( !str_cmp( name, "do_bodybag" ))		return do_bodybag;
	if ( !str_cmp( name, "do_bolt" ))		return do_bolt;
	if ( !str_cmp( name, "do_brandish" ))		return do_brandish;
	if ( !str_cmp( name, "do_brew" ))		return do_brew;
	if ( !str_cmp( name, "do_broach" ))		return do_broach;
	if ( !str_cmp( name, "do_bset" ))		return do_bset;
	if ( !str_cmp( name, "do_bstat" ))		return do_bstat;
	if ( !str_cmp( name, "do_bug" ))		return do_bug;
	if ( !str_cmp( name, "do_bury" ))		return do_bury;
	if ( !str_cmp( name, "do_buy" ))		return do_buy;
	break;
    case 'c':
	if ( !str_cmp( name, "do_cast" ))		return do_cast;
	if ( !str_cmp( name, "do_cedit" ))		return do_cedit;
	if ( !str_cmp( name, "do_channels" ))		return do_channels;
	if ( !str_cmp( name, "do_chat" ))		return do_chat;
	if ( !str_cmp( name, "do_check_vnums" ))	return do_check_vnums;
	if ( !str_cmp( name, "do_circle" ))		return do_circle;
	if ( !str_cmp( name, "do_clans" ))		return do_clans;
	if ( !str_cmp( name, "do_clantalk" ))		return do_clantalk;
	if ( !str_cmp( name, "do_climate" ))		return do_climate;
	if ( !str_cmp( name, "do_climb" ))		return do_climb;
	if ( !str_cmp( name, "do_close" ))		return do_close;
	if ( !str_cmp( name, "do_cmdtable" ))		return do_cmdtable;
	if ( !str_cmp( name, "do_colorize" ))		return do_colorize;
	if ( !str_cmp( name, "do_commands" ))		return do_commands;
	if ( !str_cmp( name, "do_comment" ))		return do_comment;
	if ( !str_cmp( name, "do_compare" ))		return do_compare;
	if ( !str_cmp( name, "do_config" ))		return do_config;
	if ( !str_cmp( name, "do_consider" ))		return do_consider;
	if ( !str_cmp( name, "do_cook" ))		return do_cook;
	if ( !str_cmp( name, "do_copyover" ))		return do_copyover;
	if ( !str_cmp( name, "do_council_induct" ))	return do_council_induct;
	if ( !str_cmp( name, "do_council_outcast" ))	return do_council_outcast;
	if ( !str_cmp( name, "do_councils" ))		return do_councils;
	if ( !str_cmp( name, "do_counciltalk" ))	return do_counciltalk;
	if ( !str_cmp( name, "do_credits" ))		return do_credits;
	if ( !str_cmp( name, "do_cset" ))		return do_cset;
	break;
    case 'd':
	if ( !str_cmp( name, "do_deities" ))		return do_deities;
	if ( !str_cmp( name, "do_delay" ))		return do_delay;
	if ( !str_cmp( name, "do_deny" ))		return do_deny;
	if ( !str_cmp( name, "do_deposit" ))		return do_deposit;
	if ( !str_cmp( name, "do_description" ))	return do_description;
	if ( !str_cmp( name, "do_destroy" ))		return do_destroy;
	if ( !str_cmp( name, "do_detrap" ))		return do_detrap;
	if ( !str_cmp( name, "do_devote" ))		return do_devote;
	if ( !str_cmp( name, "do_dig" ))		return do_dig;
	if ( !str_cmp( name, "do_disarm" ))		return do_disarm;
	if ( !str_cmp( name, "do_disconnect" ))		return do_disconnect;
	if ( !str_cmp( name, "do_dismiss" ))		return do_dismiss;
	if ( !str_cmp( name, "do_dismount" ))		return do_dismount;
	if ( !str_cmp( name, "do_dmesg" ))		return do_dmesg;
	if ( !str_cmp( name, "do_dnd" ))		return do_dnd;
	if ( !str_cmp( name, "do_down" ))		return do_down;
	if ( !str_cmp( name, "do_drag" ))		return do_drag;
	if ( !str_cmp( name, "do_drink" ))		return do_drink;
	if ( !str_cmp( name, "do_drop" ))		return do_drop;
	break;
    case 'e':
	if ( !str_cmp( name, "do_east" ))		return do_east;
	if ( !str_cmp( name, "do_eat" ))		return do_eat;
	if ( !str_cmp( name, "do_echo" ))		return do_echo;
        if ( !str_cmp( name, "do_elevate" ))            return do_elevate;
	if ( !str_cmp( name, "do_emote" ))		return do_emote;
	if ( !str_cmp( name, "do_empty" ))		return do_empty;
	if ( !str_cmp( name, "do_enter" ))		return do_enter;
	if ( !str_cmp( name, "do_equipment" ))		return do_equipment;
	if ( !str_cmp( name, "do_examine" ))		return do_examine;
	if ( !str_cmp( name, "do_exits" ))		return do_exits;
	break;
    case 'f':
	if ( !str_cmp( name, "do_feed" ))		return do_feed;
	if ( !str_cmp( name, "do_fill" ))		return do_fill;
	if ( !str_cmp( name, "do_findnote" ))		return do_findnote;
	if ( !str_cmp( name, "do_fire" ))		return do_fire;
	if ( !str_cmp( name, "do_fixchar" ))		return do_fixchar;
	if ( !str_cmp( name, "do_fixed" ))		return do_fixed;
	if ( !str_cmp( name, "do_flee" ))		return do_flee;
	if ( !str_cmp( name, "do_foldarea" ))		return do_foldarea;
	if ( !str_cmp( name, "do_follow" ))		return do_follow;
	if ( !str_cmp( name, "do_for" ))		return do_for;
	if ( !str_cmp( name, "do_force" ))		return do_force;
	if ( !str_cmp( name, "do_forceclose" ))		return do_forceclose;
	if ( !str_cmp( name, "do_form_password" ))	return do_form_password;
	if ( !str_cmp( name, "do_fprompt" ))		return do_fprompt;
	if ( !str_cmp( name, "do_fquit" ))		return do_fquit;
	if ( !str_cmp( name, "do_freeze" ))		return do_freeze;
	if ( !str_cmp( name, "do_fshow" ))		return do_fshow;
	break;
    case 'g':
	if ( !str_cmp( name, "do_get" ))		return do_get;
	if ( !str_cmp( name, "do_gfighting" ))		return do_gfighting;
	if ( !str_cmp( name, "do_give" ))		return do_give;
	if ( !str_cmp( name, "do_glance" ))		return do_glance;
        if ( !str_cmp( name, "do_gold" ))               return do_gold;
	if ( !str_cmp( name, "do_goto" ))		return do_goto;
	if ( !str_cmp( name, "do_gouge" ))		return do_gouge;
	if ( !str_cmp( name, "do_group" ))		return do_group;
	if ( !str_cmp( name, "do_gtell" ))		return do_gtell;
	if ( !str_cmp( name, "do_guilds" ))		return do_guilds;
	if ( !str_cmp( name, "do_guildtalk" ))		return do_guildtalk;
	if ( !str_cmp( name, "do_gwhere" ))		return do_gwhere;
	break;
    case 'h':
	if ( !str_cmp( name, "do_hedit" ))		return do_hedit;
	if ( !str_cmp( name, "do_hell" ))		return do_hell;
	if ( !str_cmp( name, "do_help" ))		return do_help;
	if ( !str_cmp( name, "do_hide" ))		return do_hide;
	if ( !str_cmp( name, "do_hitall" ))		return do_hitall;
	if ( !str_cmp( name, "do_hlist" ))		return do_hlist;
	if ( !str_cmp( name, "do_holylight" ))		return do_holylight;
	if ( !str_cmp( name, "do_homepage" ))		return do_homepage;
	if ( !str_cmp( name, "do_hset" ))		return do_hset;
	break;
    case 'i':
	if ( !str_cmp( name, "do_idea" ))		return do_idea;
	if ( !str_cmp( name, "do_ignore" ))		return do_ignore;
	if ( !str_cmp( name, "do_immortalize" ))	return do_immortalize;
	if ( !str_cmp( name, "do_immtalk" ))		return do_immtalk;
	if ( !str_cmp( name, "do_imm_morph" ))		return do_imm_morph;
	if ( !str_cmp( name, "do_imm_unmorph" ))	return do_imm_unmorph;
	if ( !str_cmp( name, "do_induct" ))		return do_induct;
	if ( !str_cmp( name, "do_installarea" ))	return do_installarea;
	if ( !str_cmp( name, "do_instaroom" ))		return do_instaroom;
	if ( !str_cmp( name, "do_instazone" ))		return do_instazone;
	if ( !str_cmp( name, "do_inventory" ))		return do_inventory;
	if ( !str_cmp( name, "do_invis" ))		return do_invis;
        if ( !str_cmp( name, "do_ipcompare" ))		return do_ipcompare;
	break;
    case 'k':
    	if ( !str_cmp( name, "do_khistory" ))		return do_khistory;
	if ( !str_cmp( name, "do_kick" ))		return do_kick;
	if ( !str_cmp( name, "do_kill" ))		return do_kill;
	break;
    case 'l':
	if ( !str_cmp( name, "do_languages" ))		return do_languages;
	if ( !str_cmp( name, "do_last" ))		return do_last;
	if ( !str_cmp( name, "do_laws" ))		return do_laws;
	if ( !str_cmp( name, "do_leave" ))		return do_leave;
	if ( !str_cmp( name, "do_level" ))		return do_level;
	if ( !str_cmp( name, "do_light" ))		return do_light;
	if ( !str_cmp( name, "do_list" ))		return do_list;
	if ( !str_cmp( name, "do_litterbug" ))		return do_litterbug;
	if ( !str_cmp( name, "do_loadarea" ))		return do_loadarea;
	if ( !str_cmp( name, "do_loadup" ))		return do_loadup;
	if ( !str_cmp( name, "do_lock" ))		return do_lock;
	if ( !str_cmp( name, "do_log" ))		return do_log;
	if ( !str_cmp( name, "do_look" ))		return do_look;
	if ( !str_cmp( name, "do_low_purge" ))		return do_low_purge;
	break;
    case 'm':
	if ( !str_cmp( name, "do_mailroom" ))		return do_mailroom;
	if ( !str_cmp( name, "do_make" ))		return do_make;
	if ( !str_cmp( name, "do_makeboard" ))		return do_makeboard;
	if ( !str_cmp( name, "do_makecouncil" ))	return do_makecouncil;
	if ( !str_cmp( name, "do_makedeity" ))		return do_makedeity;
	if ( !str_cmp( name, "do_makefaction" ))	return do_makefaction;
	if ( !str_cmp( name, "do_makerepair" ))		return do_makerepair;
	if ( !str_cmp( name, "do_makeshop" ))		return do_makeshop;
	if ( !str_cmp( name, "do_makewizlist" ))	return do_makewizlist;
	if ( !str_cmp( name, "do_massign" ))		return do_massign;
	if ( !str_cmp( name, "do_mcreate" ))		return do_mcreate;
	if ( !str_cmp( name, "do_mdelete" ))		return do_mdelete;
	if ( !str_cmp( name, "do_memory" ))		return do_memory;
	if ( !str_cmp( name, "do_mfind" ))		return do_mfind;
	if ( !str_cmp( name, "do_minvoke" ))		return do_minvoke;
	if ( !str_cmp( name, "do_mistwalk" ))		return do_mistwalk;
	if ( !str_cmp( name, "do_mlist" ))		return do_mlist;
        if (!str_cmp (name, "do_morphcreate" ) ) 	return do_morphcreate;
        if (!str_cmp (name, "do_morphdestroy" ) ) 	return do_morphdestroy;
        if (!str_cmp (name, "do_morphset")) 		return do_morphset;
        if (!str_cmp (name, "do_morphstat")) 		return do_morphstat;

        if ( !str_cmp( name, "do_mortalize" ))          return do_mortalize;
	if ( !str_cmp( name, "do_mount" ))		return do_mount;
	if ( !str_cmp( name, "do_mp_close_passage" ))	return do_mp_close_passage;
	if ( !str_cmp( name, "do_mp_damage" ))		return do_mp_damage;
	if ( !str_cmp( name, "do_mp_deposit" ))		return do_mp_deposit;
	if ( !str_cmp( name, "do_mp_fill_in" ) )	return do_mp_fill_in;
	if ( !str_cmp( name, "do_mp_log" ))		return do_mp_log;
	if ( !str_cmp( name, "do_mp_open_passage" ))	return do_mp_open_passage;
	if ( !str_cmp( name, "do_mp_practice" ))	return do_mp_practice;
	if ( !str_cmp( name, "do_mp_restore" ))		return do_mp_restore;
	if ( !str_cmp( name, "do_mp_slay" ))		return do_mp_slay;
	if ( !str_cmp( name, "do_mp_withdraw" ))	return do_mp_withdraw;
	if ( !str_cmp( name, "do_mpadvance" ))		return do_mpadvance;
	if ( !str_cmp( name, "do_mpapply" ))		return do_mpapply;
	if ( !str_cmp( name, "do_mpapplyb" ))		return do_mpapplyb;
	if ( !str_cmp( name, "do_mpasound" ))		return do_mpasound;
	if ( !str_cmp( name, "do_mpasupress" ))		return do_mpasupress;
	if ( !str_cmp( name, "do_mpat" ))		return do_mpat;
	if ( !str_cmp( name, "do_mpbodybag") )		return do_mpbodybag;
	if ( !str_cmp( name, "do_mpcopy" ))		return do_mpcopy;
	if ( !str_cmp( name, "do_mpdelay" ))		return do_mpdelay;
	if ( !str_cmp( name, "do_mpdream" ))		return do_mpdream;
	if ( !str_cmp( name, "do_mpecho" ))		return do_mpecho;
	if ( !str_cmp( name, "do_mpechoaround" ))	return do_mpechoaround;
	if ( !str_cmp( name, "do_mpechoat" ))		return do_mpechoat;
	if ( !str_cmp( name, "do_mpechozone" ))		return do_mpechozone;
	if ( !str_cmp( name, "do_mpedit" ))		return do_mpedit;
	if ( !str_cmp( name, "do_mpfavor" ))		return do_mpfavor;
	if ( !str_cmp( name, "do_mpforce" ))		return do_mpforce;
	if ( !str_cmp( name, "do_mpgoto" ))		return do_mpgoto;
	if ( !str_cmp( name, "do_mpinvis" ))		return do_mpinvis;
	if ( !str_cmp( name, "do_mpjunk" ))		return do_mpjunk;
	if ( !str_cmp( name, "do_mpkill" ))		return do_mpkill;
	if ( !str_cmp( name, "do_mpmload" ))		return do_mpmload;
        if ( !str_cmp( name, "do_mpmorph" ))		return do_mpmorph;
	if ( !str_cmp( name, "do_mpmset" ))		return do_mpmset;
	if ( !str_cmp( name, "do_mpmusic" ))		return do_mpmusic;
	if ( !str_cmp( name, "do_mpmusicaround" ))	return do_mpmusicaround;
	if ( !str_cmp( name, "do_mpmusicat" ))		return do_mpmusicat;
	if ( !str_cmp( name, "do_mpnothing" ))		return do_mpnothing;
	if ( !str_cmp( name, "do_mpnuisance"))		return do_nuisance;
	if ( !str_cmp( name, "do_mpoload" ))		return do_mpoload;
	if ( !str_cmp( name, "do_mposet" ))		return do_mposet;
	if ( !str_cmp( name, "do_mppardon" ))		return do_mppardon;
	if ( !str_cmp( name, "do_mppeace" ))		return do_mppeace;
	if ( !str_cmp( name, "do_mppkset" ))		return do_mppkset;
	if ( !str_cmp( name, "do_mppurge" ))		return do_mppurge;
        if ( !str_cmp( name, "do_mpscatter" ))          return do_mpscatter;
	if ( !str_cmp( name, "do_mpsound" ))		return do_mpsound;
	if ( !str_cmp( name, "do_mpsoundaround" ))	return do_mpsoundaround;
	if ( !str_cmp( name, "do_mpsoundat" ))		return do_mpsoundat;
	if ( !str_cmp( name, "do_mpstat" ))		return do_mpstat;
	if ( !str_cmp( name, "do_mptransfer" ))		return do_mptransfer;
        if ( !str_cmp( name, "do_mpunmorph" ))		return do_mpunmorph;
	if ( !str_cmp( name, "do_mpunnuisance" ) )	return do_mpunnuisance;
	if ( !str_cmp( name, "do_mrange" ))		return do_mrange;
	if ( !str_cmp( name, "do_mpfind" ))		return do_mpfind;
	if ( !str_cmp( name, "do_mset" ))		return do_mset;
	if ( !str_cmp( name, "do_mstat" ))		return do_mstat;
	if ( !str_cmp( name, "do_murder" ))		return do_murder;
	if ( !str_cmp( name, "do_muse" ))		return do_muse;
	if ( !str_cmp( name, "do_music" ))		return do_music;
	if ( !str_cmp( name, "do_mwhere" ))		return do_mwhere;
	break;
    case 'n':
	if ( !str_cmp( name, "do_name" ))		return do_name;
	if ( !str_cmp( name, "do_newbiechat" ))		return do_newbiechat;
	if ( !str_cmp( name, "do_newbieset" ))		return do_newbieset;
	if ( !str_cmp( name, "do_news" ))               return do_news;
	if ( !str_cmp( name, "do_newzones" ))		return do_newzones;
	if ( !str_cmp( name, "do_noemote" ))		return do_noemote;
	if ( !str_cmp( name, "do_noresolve" ))		return do_noresolve;
	if ( !str_cmp( name, "do_north" ))		return do_north;
	if ( !str_cmp( name, "do_northeast" ))		return do_northeast;
	if ( !str_cmp( name, "do_northwest" ))		return do_northwest;
	if ( !str_cmp( name, "do_notell" ))		return do_notell;
        if ( !str_cmp( name, "do_notitle" ))            return do_notitle;
	if ( !str_cmp( name, "do_noteroom" ))		return do_noteroom;
  	if ( !str_cmp( name, "do_nuisance" ))		return do_nuisance;
	break;
    case 'o':
	if ( !str_cmp( name, "do_oassign" ))		return do_oassign;
	if ( !str_cmp( name, "do_ocreate" ))		return do_ocreate;
	if ( !str_cmp( name, "do_odelete" ))		return do_odelete;
	if ( !str_cmp( name, "do_ofind" ))		return do_ofind;
	if ( !str_cmp( name, "do_oinvoke" ))		return do_oinvoke;
	if ( !str_cmp( name, "do_olist" ))		return do_olist;
	if ( !str_cmp( name, "do_opcopy" ))		return do_opcopy;
	if ( !str_cmp( name, "do_opedit" ))		return do_opedit;
	if ( !str_cmp( name, "do_open" ))		return do_open;
	if ( !str_cmp( name, "do_opfind" ))		return do_opfind;
	if ( !str_cmp( name, "do_opstat" ))		return do_opstat;
	if ( !str_cmp( name, "do_orange" ))		return do_orange;
	if ( !str_cmp( name, "do_order" ))		return do_order;
	if ( !str_cmp( name, "do_orders" ))		return do_orders;
	if ( !str_cmp( name, "do_ordertalk" ))		return do_ordertalk;
	if ( !str_cmp( name, "do_oset" ))		return do_oset;
	if ( !str_cmp( name, "do_ostat" ))		return do_ostat;
	if ( !str_cmp( name, "do_outcast" ))		return do_outcast;
	if ( !str_cmp( name, "do_owhere" ))		return do_owhere;
	break;
    case 'p':
	if ( !str_cmp( name, "do_pager" ))		return do_pager;
	if ( !str_cmp( name, "do_pardon" ))		return do_pardon;
	if ( !str_cmp( name, "do_password" ))		return do_password;
	if ( !str_cmp( name, "do_pcrename" ) )		return do_pcrename;
	if ( !str_cmp( name, "do_peace" ))		return do_peace;
	if ( !str_cmp( name, "do_pick" ))		return do_pick;
	if ( !str_cmp( name, "do_poison_weapon" ))	return do_poison_weapon;
	if ( !str_cmp( name, "do_practice" ))		return do_practice;
	if ( !str_cmp( name, "do_project" ))		return do_project;
	if ( !str_cmp( name, "do_prompt" ))		return do_prompt;
	if ( !str_cmp( name, "do_pull" ))		return do_pull;
	if ( !str_cmp( name, "do_punch" ))		return do_punch;
	if ( !str_cmp( name, "do_purge" ))		return do_purge;
	if ( !str_cmp( name, "do_push" ))		return do_push;
	if ( !str_cmp( name, "do_put" ))		return do_put;
	break;
    case 'q':
	if ( !str_cmp( name, "do_qpset" ))		return do_qpset;
        if ( !str_cmp( name, "do_qpstat" ))             return do_qpstat;
	if ( !str_cmp( name, "do_qset" ))		return do_qset;
	if ( !str_cmp( name, "do_quaff" ))		return do_quaff;
	if ( !str_cmp( name, "do_quest" ))		return do_quest;
	if ( !str_cmp( name, "do_questtalk" ))		return do_questtalk;
	if ( !str_cmp( name, "do_quit" ))		return do_quit;
	break;
    case 'r':
	if ( !str_cmp( name, "do_racetalk" ))		return do_racetalk;
	if ( !str_cmp( name, "do_rank" ))		return do_rank;
	if ( !str_cmp( name, "do_rap" ))		return do_rap;
	if ( !str_cmp( name, "do_rassign" ))		return do_rassign;
	if ( !str_cmp( name, "do_rat" ))		return do_rat;
	if ( !str_cmp( name, "do_rdelete" ))		return do_rdelete;
	if ( !str_cmp( name, "do_reboot" ))		return do_reboot;
	if ( !str_cmp( name, "do_recho" ))		return do_recho;
	if ( !str_cmp( name, "do_recite" ))		return do_recite;
	if ( !str_cmp( name, "do_redit" ))		return do_redit;
	if ( !str_cmp( name, "do_regoto" ))		return do_regoto;
	if ( !str_cmp( name, "do_remove" ))		return do_remove;
	if ( !str_cmp( name, "do_remains" ))		return do_remains;
	if ( !str_cmp( name, "do_rent" ))		return do_rent;
	if ( !str_cmp( name, "do_repair" ))		return do_repair;
	if ( !str_cmp( name, "do_repairset" ))		return do_repairset;
	if ( !str_cmp( name, "do_repairshops" ))	return do_repairshops;
	if ( !str_cmp( name, "do_repairstat" ))		return do_repairstat;
	if ( !str_cmp( name, "do_repeat" ))		return do_repeat;
	if ( !str_cmp( name, "do_reply" ))		return do_reply;
	if ( !str_cmp( name, "do_report" ))		return do_report;
	if ( !str_cmp( name, "do_rescue" ))		return do_rescue;
	if ( !str_cmp( name, "do_reserve" ))		return do_reserve;
	if ( !str_cmp( name, "do_reset" ))		return do_reset;
	if ( !str_cmp( name, "do_resign" ))		return do_resign;
	if ( !str_cmp( name, "do_rest" ))		return do_rest;
	if ( !str_cmp( name, "do_restore" ))		return do_restore;
	if ( !str_cmp( name, "do_restoretime" ))	return do_restoretime;
	if ( !str_cmp( name, "do_restrict" ))		return do_restrict;
	if ( !str_cmp( name, "do_retell" ))		return do_retell;
	if ( !str_cmp( name, "do_retire" ))		return do_retire;
	if ( !str_cmp( name, "do_retran" ))		return do_retran;
	if ( !str_cmp( name, "do_return" ))		return do_return;
	if ( !str_cmp( name, "do_rlist" ))		return do_rlist;
	if ( !str_cmp( name, "do_rpedit" ))		return do_rpedit;
	if ( !str_cmp( name, "do_rpfind" ))		return do_rpfind;
	if ( !str_cmp( name, "do_rpstat" ))		return do_rpstat;
 	if ( !str_cmp( name, "do_rreset" ))		return do_rreset;
 	if ( !str_cmp( name, "do_rset" ))		return do_rset;
 	if ( !str_cmp( name, "do_rstat" ))		return do_rstat;
	break;
    case 's':
	if ( !str_cmp( name, "do_sacrifice" ))		return do_sacrifice;
	if ( !str_cmp( name, "do_save" ))		return do_save;
	if ( !str_cmp( name, "do_savearea" ))		return do_savearea;
	if ( !str_cmp( name, "do_say" ))		return do_say;
	if ( !str_cmp( name, "do_scan" ))		return do_scan;
        if ( !str_cmp( name, "do_scatter" ) )           return do_scatter;
	if ( !str_cmp( name, "do_score" ))		return do_score;
	if ( !str_cmp( name, "do_scribe" ))		return do_scribe;
	if ( !str_cmp( name, "do_search" ))		return do_search;
	if ( !str_cmp( name, "do_sedit" ))		return do_sedit;
	if ( !str_cmp( name, "do_sell" ))		return do_sell;
	if ( !str_cmp( name, "do_set_boot_time" ))	return do_set_boot_time;
	if ( !str_cmp( name, "do_setclass" ))		return do_setclass;
	if ( !str_cmp( name, "do_setcouncil" ))		return do_setcouncil;
	if ( !str_cmp( name, "do_setdeity" ))		return do_setdeity;
	if ( !str_cmp( name, "do_setfaction" ))		return do_setfaction;
	if ( !str_cmp( name, "do_setrace" ))		return do_setrace;
	if ( !str_cmp( name, "do_setweather" ))		return do_setweather;
	if ( !str_cmp( name, "do_shops" ))		return do_shops;
	if ( !str_cmp( name, "do_shopset" ))		return do_shopset;
	if ( !str_cmp( name, "do_shopstat" ))		return do_shopstat;
	if ( !str_cmp( name, "do_shout" ))		return do_shout;
	if ( !str_cmp( name, "do_shove" ))		return do_shove;
	if ( !str_cmp( name, "do_showclass" ))		return do_showclass;
	if ( !str_cmp( name, "do_showcouncil" ))	return do_showcouncil;
	if ( !str_cmp( name, "do_showdeity" ))		return do_showdeity;
	if ( !str_cmp( name, "do_showfaction" ))	return do_showfaction;
	if ( !str_cmp( name, "do_showrace" ))		return do_showrace;
	if ( !str_cmp( name, "do_showweather" ))	return do_showweather;
	if ( !str_cmp( name, "do_shutdown" ))		return do_shutdown;
	if ( !str_cmp( name, "do_silence" ))		return do_silence;
	if ( !str_cmp( name, "do_sit" ))		return do_sit;
	if ( !str_cmp( name, "do_skin" ))		return do_skin;
	if ( !str_cmp( name, "do_slay" ))		return do_slay;
	if ( !str_cmp( name, "do_sleep" ))		return do_sleep;
	if ( !str_cmp( name, "do_slice" ))		return do_slice;
	if ( !str_cmp( name, "do_slist" ))		return do_slist;
	if ( !str_cmp( name, "do_slookup" ))		return do_slookup;
	if ( !str_cmp( name, "do_smoke" ))		return do_smoke;
	if ( !str_cmp( name, "do_snoop" ))		return do_snoop;
	if ( !str_cmp( name, "do_sober" ))		return do_sober;
	if ( !str_cmp( name, "do_socials" ))		return do_socials;
	if ( !str_cmp( name, "do_south" ))		return do_south;
	if ( !str_cmp( name, "do_southeast" ))		return do_southeast;
	if ( !str_cmp( name, "do_southwest" ))		return do_southwest;
	if ( !str_cmp( name, "do_speak" ))		return do_speak;
	if ( !str_cmp( name, "do_split" ))		return do_split;
	if ( !str_cmp( name, "do_sset" ))		return do_sset;
	if ( !str_cmp( name, "do_stand" ))		return do_stand;
	if ( !str_cmp( name, "do_stat" ) )		return do_stat;
	if ( !str_cmp( name, "do_statreport" ) )	return do_statreport;
	if ( !str_cmp( name, "do_statshield" ) )	return do_statshield;
	if ( !str_cmp( name, "do_steal" ))		return do_steal;
	if ( !str_cmp( name, "do_strew" ))		return do_strew;
	if ( !str_cmp( name, "do_strip" ))		return do_strip;
	if ( !str_cmp( name, "do_stun" ))		return do_stun;
	if ( !str_cmp( name, "do_style" ))		return do_style;
	if ( !str_cmp( name, "do_supplicate" ))		return do_supplicate;
	if ( !str_cmp( name, "do_switch" ))		return do_switch;
	break;
    case 't':
	if ( !str_cmp( name, "do_tamp" ))		return do_tamp;
	if ( !str_cmp( name, "do_tell" ))		return do_tell;
	if ( !str_cmp( name, "do_think" ))		return do_think;
	if ( !str_cmp( name, "do_time" ))		return do_time;
	if ( !str_cmp( name, "do_timecmd" ))		return do_timecmd;
	if ( !str_cmp( name, "do_title" ))		return do_title;
	if ( !str_cmp( name, "do_track" ))		return do_track;
	if ( !str_cmp( name, "do_traffic" ))		return do_traffic;
	if ( !str_cmp( name, "do_transfer" ))		return do_transfer;
	if ( !str_cmp( name, "do_trust" ))		return do_trust;
	if ( !str_cmp( name, "do_typo" ))		return do_typo;
	break;
    case 'u':
	if ( !str_cmp( name, "do_unbolt" ))		return do_unbolt;
	if ( !str_cmp( name, "do_unhell" ))		return do_unhell;
	if ( !str_cmp( name, "do_unlock" ))		return do_unlock;
	if ( !str_cmp( name, "do_unnuisance" ))		return do_unnuisance;
        if ( !str_cmp( name, "do_unsilence" ))          return do_unsilence;
	if ( !str_cmp( name, "do_up" ))			return do_up;
	if ( !str_cmp( name, "do_users" ))		return do_users;
	break;
    case 'v':
	if ( !str_cmp( name, "do_value" ))		return do_value;
	if ( !str_cmp( name, "do_version" ))		return do_version;
	if ( !str_cmp( name, "do_victories" ))		return do_victories;
	if ( !str_cmp( name, "do_visible" ))		return do_visible;
	if ( !str_cmp( name, "do_vnums" ))		return do_vnums;
	if ( !str_cmp( name, "do_vsearch" ))		return do_vsearch;
	break;
    case 'w':
	if ( !str_cmp( name, "do_wake" ))		return do_wake;
	if ( !str_cmp( name, "do_wartalk" ))		return do_wartalk;
        if ( !str_cmp( name, "do_warn" ))		return do_warn;
	if ( !str_cmp( name, "do_watch" ))		return do_watch;
	if ( !str_cmp( name, "do_wear" ))		return do_wear;
	if ( !str_cmp( name, "do_weather" ))		return do_weather;
	if ( !str_cmp( name, "do_west" ))		return do_west;
	if ( !str_cmp( name, "do_where" ))		return do_where;
	if ( !str_cmp( name, "do_whisper"))		return do_whisper;
	if ( !str_cmp( name, "do_who" ))		return do_who;
	if ( !str_cmp( name, "do_whois" ))		return do_whois;
	if ( !str_cmp( name, "do_wimpy" ))		return do_wimpy;
	if ( !str_cmp( name, "do_withdraw" ))		return do_withdraw;
	if ( !str_cmp( name, "do_wizhelp" ))		return do_wizhelp;
	if ( !str_cmp( name, "do_wizlist" ))		return do_wizlist;
	if ( !str_cmp( name, "do_wizlock" ))		return do_wizlock;
        if ( !str_cmp( name, "do_worth" ))              return do_worth;
	break;
    case 'y':
	if ( !str_cmp( name, "do_yell" ))		return do_yell;
	break;
    case 'z':
	if ( !str_cmp( name, "do_zap" ))		return do_zap;
	if ( !str_cmp( name, "do_zones" ))		return do_zones;
    }
    return skill_notfound;
}

bool load_class_file( char *fname )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    struct class_type *class;
    int cl = -1;
    int tlev = 0;
    FILE *fp;

    sprintf( buf, "%s%s", CLASS_DIR, fname );
    if ( ( fp = fopen( buf, "r" ) ) == NULL )
    {
	perror( buf );
	return FALSE;
    }

    CREATE( class, struct class_type, 1 );

    /* Setup defaults for additions to class structure */
    class->attr_second = 0;
    class->attr_deficient = 0;
    xCLEAR_BITS(class->affected);
    class->resist = 0;
    class->suscept = 0;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Affected",	class->affected,	fread_bitvector(fp));
	    KEY( "AttrPrime",	class->attr_prime,	fread_number( fp )	);
	    KEY( "AttrSecond",	class->attr_second,	fread_number( fp )	);
	    KEY( "AttrDeficient",class->attr_deficient,	fread_number( fp )	);
	    break;

	case 'C':
	    KEY( "Class",	cl,			fread_number( fp )	);
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		FCLOSE( fp );
		if ( cl < 0 || cl >= MAX_CLASS )
		{
		    sprintf( buf, "Load_class_file: Class (%s) bad/not found (%d)",
			class->who_name ? class->who_name : "name not found", cl );
		    bug( buf, 0 );
		    if ( class->who_name )
			STRFREE( class->who_name );
		    DISPOSE( class );
		    return FALSE;
		}
		class_table[cl] = class;
		return TRUE;
	    }
	    KEY( "ExpBase",	class->exp_base,	fread_number( fp )	);
	    break;

	case 'H':
	    KEY( "HpMax",	class->hp_max,		fread_number( fp )	);
	    KEY( "HpMin",	class->hp_min,		fread_number( fp )	);
	    break;

	case 'M':
	    KEY( "Mana",	class->fMana,		fread_number( fp )	);
	    break;

	case 'N':
	    KEY( "Name",	class->who_name,	fread_string( fp )	);
	    break;

	case 'R':
	    KEY( "Resist",	class->resist,		fread_number( fp ) );
	    break;

	case 'S':
	    if ( !str_cmp( word, "Skill" ) )
	    {
		int sn, lev, adp;

		word = fread_word( fp );
		lev = fread_number( fp );
		adp = fread_number( fp );
		sn = skill_lookup( word );
		if ( cl < 0 || cl >= MAX_CLASS )
		{
		    sprintf( buf, "load_class_file: Skill %s -- class bad/not found (%d)", word, cl );
		    bug( buf, 0 );
		}
		else
		if ( !IS_VALID_SN(sn) )
		{
		    sprintf( buf, "load_class_file: Skill %s unknown", word);
		    bug( buf, 0 );
		}
		else
		{
		    skill_table[sn]->skill_level[cl] = lev;
		    skill_table[sn]->skill_adept[cl] = adp;
		}
		fMatch = TRUE;
		break;
	    }
	    KEY( "Skilladept",	class->skill_adept,	fread_number( fp )	);
	    KEY( "Suscept",	class->suscept,		fread_number( fp ) );
	    break;

	case 'T':
	    if ( !str_cmp( word, "Title" ) )
	    {
		if ( cl < 0 || cl >= MAX_CLASS )
		{
		    char *tmp;

		    sprintf( buf, "load_class_file: Title -- class bad/not found (%d)", cl );
		    bug( buf, 0 );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		}
		else
		if ( tlev < MAX_LEVEL+1 )
		{
		    title_table[cl][tlev][0] = fread_string_nohash( fp );
		    title_table[cl][tlev][1] = fread_string_nohash( fp );
		    ++tlev;
		}
		else
		    bug( "load_class_file: Too many titles" );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Thac0",	class->thac0_00,	fread_number( fp )	);
	    KEY( "Thac32",	class->thac0_32,	fread_number( fp )	);
	    break;

	case 'W':
	    KEY( "Weapon",	class->weapon,	fread_number( fp )	);
	    break;
	}
	if ( !fMatch )
	{
            sprintf( buf, "load_class_file: no match: %s", word );
	    bug( buf, 0 );
	}
    }
    return FALSE;
}

/*
 * Load in all the class files.
 */
void load_classes( )
{
    FILE *fpList;
    char *filename;
    char classlist[256];
    char buf[MAX_STRING_LENGTH];
    int i;
     
    MAX_PC_CLASS = 0;

    /*
     * Pre-init the class_table with blank classes
     */
    for ( i=0;i<MAX_CLASS;i++ )
	class_table[i] = NULL;
    	
    sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
    if ( ( fpList = fopen( classlist, "r" ) ) == NULL )
    {
	perror( classlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;

	if ( !load_class_file( filename ) )
	{
	  sprintf( buf, "Cannot load class file: %s", filename );
	  bug( buf, 0 );
	}
	else
	  MAX_PC_CLASS++;
    }
    FCLOSE( fpList );
    for ( i=0;i<MAX_CLASS;i++ )
    {
    	if ( class_table[i] == NULL ) 
	{
	  CREATE( class_table[i], struct class_type, 1 );
    	  create_new_class( i, "" );
	}
    }
    return;
}


void write_class_file( int cl )
{
    FILE *fpout;
    char buf[MAX_STRING_LENGTH];
    char filename[MAX_INPUT_LENGTH];
    struct class_type *class = class_table[cl];
    int x, y;

    sprintf( filename, "%s%s.class", CLASSDIR, class->who_name );
    if ( (fpout=fopen(filename, "w")) == NULL )
    {
	sprintf( buf, "Cannot open: %s for writing", filename );
	bug( buf, 0 );
	return;
    }
    fprintf( fpout, "Name        %s~\n",	class->who_name		);
    fprintf( fpout, "Class       %d\n",		cl			);
    fprintf( fpout, "AttrPrime   %d\n",		class->attr_prime	);
    fprintf( fpout, "AttrSecond   %d\n",	class->attr_second	);
    fprintf( fpout, "AttrDeficient   %d\n",	class->attr_deficient	);
    fprintf( fpout, "Weapon      %d\n",		class->weapon		);
    fprintf( fpout, "Skilladept  %d\n",		class->skill_adept	);
    fprintf( fpout, "Thac0       %d\n",		class->thac0_00		);
    fprintf( fpout, "Thac32      %d\n",		class->thac0_32		);
    fprintf( fpout, "Hpmin       %d\n",		class->hp_min		);
    fprintf( fpout, "Hpmax       %d\n",		class->hp_max		);
    fprintf( fpout, "Mana        %d\n",		class->fMana		);
    fprintf( fpout, "Expbase     %d\n",		class->exp_base		);
    fprintf( fpout, "Affected    %s\n",		print_bitvector(&class->affected));
    fprintf( fpout, "Resist	 %d\n",		class->resist		);
    fprintf( fpout, "Suscept	 %d\n",		class->suscept		);
    for ( x = 0; x < top_sn; x++ )
    {
	if ( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
	   break;
	if ( (y=skill_table[x]->skill_level[cl]) < LEVEL_IMMORTAL )
	  fprintf( fpout, "Skill '%s' %d %d\n",
		skill_table[x]->name, y, skill_table[x]->skill_adept[cl] );
    }
    for ( x = 0; x <= MAX_LEVEL; x++ )
	fprintf( fpout, "Title\n%s~\n%s~\n",
		title_table[cl][x][0], title_table[cl][x][1] );
    fprintf( fpout, "End\n" );
    FCLOSE( fpout );
}


/*
 * Load in all the race files.
 */
void load_races( )
{
    FILE *fpList;
    char *filename;
    char racelist[256];
    char buf[MAX_STRING_LENGTH];
    int i; 

    MAX_PC_RACE = 0;
   /*
    * Pre-init the race_table with blank races
    */
    for(i=0;i<MAX_RACE;i++)
    	race_table[i] = NULL;

    sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
    if ( ( fpList = fopen( racelist, "r" ) ) == NULL )
    {
	perror( racelist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;

	if ( !load_race_file( filename ) )
	{
	  sprintf( buf, "Cannot load race file: %s", filename );
	  bug( buf, 0 );
	}
	else
	  MAX_PC_RACE++;
    }
    for ( i = 0; i < MAX_RACE; i++ )
    {
        if ( race_table[i] == NULL )
        {
          CREATE( race_table[i], struct race_type, 1 );
          sprintf( race_table[i]->race_name, "%s", "unused" );
        }
    }
    FCLOSE( fpList );
    return;
}

void write_race_file( int ra )
{
    FILE *fpout;
    char buf[MAX_STRING_LENGTH];
    char filename[MAX_INPUT_LENGTH];
    struct race_type *race = race_table[ra];
    int i;
    int x,y;

    if( !race->race_name)
    {
	sprintf( buf, "Race %d has null name, not writing .race file.", ra );
	bug( buf, 0 );
	return;
    }

    sprintf( filename, "%s%s.race", RACEDIR, race->race_name );
    if ( (fpout=fopen(filename, "w+")) == NULL )
    {
	sprintf( buf, "Cannot open: %s for writing", filename );
	bug( buf, 0 );
	return;
    }

    fprintf( fpout, "Name        %s~\n",	race->race_name		);
    fprintf( fpout, "Race        %d\n",		ra  			);
    fprintf( fpout, "Classes     %d\n",		race->class_restriction );
    fprintf( fpout, "Str_Plus    %d\n",		race->str_plus		);
    fprintf( fpout, "Dex_Plus    %d\n",		race->dex_plus		);
    fprintf( fpout, "Wis_Plus    %d\n",		race->wis_plus		);
    fprintf( fpout, "Int_Plus    %d\n",		race->int_plus		);
    fprintf( fpout, "Con_Plus    %d\n",		race->con_plus		);
    fprintf( fpout, "Cha_Plus    %d\n",		race->cha_plus		);
    fprintf( fpout, "Lck_Plus    %d\n",		race->lck_plus		);
    fprintf( fpout, "Hit         %d\n",		race->hit		);
    fprintf( fpout, "Mana        %d\n",		race->mana		);
    fprintf( fpout, "Affected    %s\n",		print_bitvector(&race->affected)	);
    fprintf( fpout, "Resist      %d\n",		race->resist		);
    fprintf( fpout, "Suscept     %d\n",		race->suscept		);
    fprintf( fpout, "Language    %d\n",		race->language		);
    fprintf( fpout, "Align       %d\n",		race->alignment		);
    fprintf( fpout, "Min_Align  %d\n",		race->minalign		);
    fprintf( fpout, "Max_Align	%d\n",		race->maxalign		);
    fprintf( fpout, "AC_Plus    %d\n",		race->ac_plus		);
    fprintf( fpout, "Exp_Mult   %d\n",		race->exp_multiplier	); 
    fprintf( fpout, "Attacks    %s\n",		print_bitvector(&race->attacks)	);
    fprintf( fpout, "Defenses   %s\n",		print_bitvector(&race->defenses) );
    fprintf( fpout, "Height     %d\n",		race->height		);
    fprintf( fpout, "Weight     %d\n",		race->weight		);
    fprintf( fpout, "Hunger_Mod  %d\n",		race->hunger_mod	);
    fprintf( fpout, "Thirst_mod  %d\n",		race->thirst_mod	);
    fprintf( fpout, "Mana_Regen  %d\n",		race->mana_regen	);
    fprintf( fpout, "HP_Regen    %d\n",		race->hp_regen		);
    fprintf( fpout, "Race_Recall %d\n",		race->race_recall	);
    for ( i = 0; i < MAX_WHERE_NAME; i++ )
	fprintf( fpout, "WhereName  %s~\n",
		race->where_name[i] );

    for ( x = 0; x < top_sn; x++ )
    {
	if ( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
	   break;
	if ( (y=skill_table[x]->race_level[ra]) < LEVEL_IMMORTAL )
	  fprintf( fpout, "Skill '%s' %d %d\n",
		skill_table[x]->name, y, skill_table[x]->race_adept[ra] );
    }
    fprintf( fpout, "End\n" );
    FCLOSE( fpout );
}

bool load_race_file( char *fname )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    char *race_name = NULL;
    bool fMatch;
    struct race_type *race;
    int ra = -1;
    FILE *fp;
    int i, wear=0;

    sprintf( buf, "%s%s", RACEDIR, fname );
    if ( ( fp = fopen( buf, "r" ) ) == NULL )
    {
	perror( buf );
	return FALSE;
    }

    CREATE( race, struct race_type, 1 );
    for(i=0;i<MAX_WHERE_NAME;i++)
       race->where_name[i] = where_name[i];

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Align",	race->alignment,		fread_number( fp )	);
	    KEY( "AC_Plus",	race->ac_plus,		fread_number( fp )	);
	    KEY( "Affected",	race->affected,		fread_bitvector( fp )	);
	    KEY( "Attacks",	race->attacks,		fread_bitvector( fp )	);
	    break;

	case 'C':
	    KEY( "Con_Plus",	race->con_plus,		fread_number( fp )	);
	    KEY( "Cha_Plus",	race->cha_plus,		fread_number( fp )	);
	    KEY( "Classes",	race->class_restriction,		fread_number( fp )	);
	    break;


	case 'D':
	    KEY( "Dex_Plus",	race->dex_plus,		fread_number( fp )	);
	    KEY( "Defenses",	race->defenses,		fread_bitvector( fp )	);
	    break;

	case 'E':
            if ( !str_cmp( word, "End" ) )
            {
                FCLOSE( fp );
                if ( ra < 0 || ra >= MAX_RACE )
                {
                    sprintf( buf, "Load_race_file: Race (%s) bad/not found (%d)",
                        race->race_name ? race->race_name : "name not found", ra );
		    if ( race_name )
			STRFREE( race_name );
                    bug( buf, 0 );
                    DISPOSE( race );
                    return FALSE;
                }
                race_table[ra] = race;
		if ( race_name )
		    STRFREE( race_name );
                return TRUE;
            }

	    KEY( "Exp_Mult",	race->exp_multiplier,	fread_number( fp )	);

	    break;


	case 'I':
	    KEY( "Int_Plus",	race->int_plus,		fread_number( fp )	);
	    break;

	case 'H':
	    KEY( "Height",	race->height,		fread_number( fp )	);
	    KEY( "Hit",		race->hit,		fread_number( fp )	);
	    KEY( "HP_Regen",	race->hp_regen,		fread_number( fp )	);
	    KEY( "Hunger_Mod",	race->hunger_mod,	fread_number( fp )	);
	    break;

	case 'L':
	    KEY( "Language",	race->language,		fread_number( fp )	);
	    KEY( "Lck_Plus",	race->lck_plus,		fread_number( fp )	);
	    break;


	case 'M':
	    KEY( "Mana",	race->mana,		fread_number( fp )	);
	    KEY( "Mana_Regen",	race->mana_regen,	fread_number( fp )	);
	    KEY( "Min_Align",	race->minalign,		fread_number( fp )	);
            race->minalign = -1000;
	    KEY( "Max_Align",	race->maxalign,		fread_number( fp )	);
            race->maxalign = -1000;
	    break;

	case 'N':
	    KEY( "Name",	race_name,	fread_string( fp )	);
	    break;

	case 'R':
	    KEY( "Race",	ra,			fread_number( fp )	);
	    KEY( "Race_Recall",	race->race_recall,		fread_number( fp )	);
	    KEY( "Resist",	race->resist,		fread_number( fp )	);
	    break;

	case 'S':
	    KEY( "Str_Plus",	race->str_plus,		fread_number( fp )	);
	    KEY( "Suscept",	race->suscept,		fread_number( fp )	);
	    if ( !str_cmp( word, "Skill" ) )
	    {
		int sn, lev, adp;

		word = fread_word( fp );
		lev = fread_number( fp );
		adp = fread_number( fp );
		sn = skill_lookup( word );
		if ( ra < 0 || ra >= MAX_RACE )
		{
		    sprintf( buf, "load_race_file: Skill %s -- race bad/not found (%d)", word, ra );
		    bug( buf, 0 );
		}
		else
		if ( !IS_VALID_SN(sn) )
		{
		    sprintf( buf, "load_race_file: Skill %s unknown", word);
		    bug( buf, 0 );
		}
		else
		{
		    skill_table[sn]->race_level[ra] = lev;
		    skill_table[sn]->race_adept[ra] = adp;
		}
		fMatch = TRUE;
		break;
            }
	    break;

	case 'T':
	    KEY( "Thirst_Mod",	race->thirst_mod,	fread_number( fp )	);
	    break;

	case 'W':
	    KEY( "Weight",	race->weight,		fread_number( fp )	);
	    KEY( "Wis_Plus",	race->wis_plus,		fread_number( fp )	);
	    if ( !str_cmp( word, "WhereName" ) )
	    {
		if ( ra < 0 || ra >= MAX_RACE )
		{
		    char *tmp;

		    sprintf( buf, "load_race_file: Title -- race bad/not found (%d)", ra );
		    bug( buf, 0 );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		}
		else
		if ( wear < MAX_WHERE_NAME+1 )
		{
		    race->where_name[wear] = fread_string_nohash( fp );
		    ++wear;
		}
		else
		    bug( "load_race_file: Too many where_names" );
		fMatch = TRUE;
		break;
	    }
	    break;
	}

        if ( race_name != NULL) 
             sprintf(race->race_name,"%-.16s",race_name);

	if ( !fMatch )
	{
            sprintf( buf, "load_race_file: no match: %s", word );
	    bug( buf, 0 );
	}
    }
    return FALSE;
}

/*
 * Function used by qsort to sort skills
 */
int skill_comp( SKILLTYPE **sk1, SKILLTYPE **sk2 )
{
    SKILLTYPE *skill1 = (*sk1);
    SKILLTYPE *skill2 = (*sk2);

    if ( !skill1 && skill2 )
	return 1;
    if ( skill1 && !skill2 )
	return -1;
    if ( !skill1 && !skill2 )
	return 0;
    if ( skill1->type < skill2->type )
	return -1;
    if ( skill1->type > skill2->type )
	return 1;
    return strcmp( skill1->name, skill2->name );
}

/*
 * Sort the skill table with qsort
 */
void sort_skill_table()
{
    log_string( "Sorting skill table..." );
    qsort( &skill_table[1], top_sn-1, sizeof( SKILLTYPE * ),
		(int(*)(const void *, const void *)) skill_comp );
}


/*
 * Remap slot numbers to sn values
 */
void remap_slot_numbers()
{
    SKILLTYPE *skill;
    SMAUG_AFF *aff;
    char tmp[32];
    int sn;

    log_string( "Remapping slots to sns" );

    for ( sn = 0; sn <= top_sn; sn++ )
    {
	if ( (skill=skill_table[sn]) != NULL )
	{
	    for ( aff = skill->affects; aff; aff = aff->next )
		if ( aff->location == APPLY_WEAPONSPELL
		||   aff->location == APPLY_WEARSPELL
		||   aff->location == APPLY_REMOVESPELL
		||   aff->location == APPLY_STRIPSN
		||   aff->location == APPLY_RECURRINGSPELL )
		{
		    sprintf( tmp, "%d", slot_lookup(atoi(aff->modifier)) );
		    DISPOSE(aff->modifier);
		    aff->modifier = str_dup(tmp);
		}
	}
    }
}

/*
 * Write skill data to a file
 */
void fwrite_skill( FILE *fpout, SKILLTYPE *skill )
{
    SMAUG_AFF *aff;
    int modifier;

    fprintf( fpout, "Name         %s~\n",	skill->name	);
    fprintf( fpout, "Type         %s\n",	skill_tname[skill->type]);
    fprintf( fpout, "Info         %d\n",	skill->info	);
    fprintf( fpout, "Flags        %s\n",	print_bitvector( &skill->flags ));
    if ( skill->target )
	fprintf( fpout, "Target       %d\n",	skill->target	);
    /*
     * store as new minpos (minpos>=100 flags new style in character loading)
     */
    if ( skill->minimum_position )
	fprintf( fpout, "Minpos       %d\n",	skill->minimum_position+100 );
    if ( skill->spell_sector )
    	fprintf( fpout, "Ssector      %d\n",	skill->spell_sector );
    if ( skill->saves )
	fprintf( fpout, "Saves        %d\n",	skill->saves    );
    if ( skill->slot )
	fprintf( fpout, "Slot         %d\n",	skill->slot	);
    if ( skill->min_mana )
	fprintf( fpout, "Mana         %d\n",	skill->min_mana );
    if ( skill->beats )
	fprintf( fpout, "Rounds       %d\n",	skill->beats	);
    if ( skill->range )
	fprintf( fpout, "Range        %d\n",	skill->range	);
    if ( skill->code && ( skill->spell_fun || skill->skill_fun )  )
	fprintf( fpout, "Code         %s\n",	skill->code	);
    fprintf( fpout, "Dammsg       %s~\n",	skill->noun_damage );
    if ( skill->msg_off && skill->msg_off[0] != '\0' )
	fprintf( fpout, "Wearoff      %s~\n",	skill->msg_off	);

    if ( skill->hit_char && skill->hit_char[0] != '\0' )
	fprintf( fpout, "Hitchar      %s~\n",	skill->hit_char );
    if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
	fprintf( fpout, "Hitvict      %s~\n",	skill->hit_vict );
    if ( skill->hit_room && skill->hit_room[0] != '\0' )
	fprintf( fpout, "Hitroom      %s~\n",	skill->hit_room );
    if ( skill->hit_dest && skill->hit_dest[0] != '\0' )
	fprintf( fpout, "Hitdest      %s~\n",	skill->hit_dest );

    if ( skill->miss_char && skill->miss_char[0] != '\0' )
	fprintf( fpout, "Misschar     %s~\n",	skill->miss_char );
    if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
	fprintf( fpout, "Missvict     %s~\n",	skill->miss_vict );
    if ( skill->miss_room && skill->miss_room[0] != '\0' )
	fprintf( fpout, "Missroom     %s~\n",	skill->miss_room );

    if ( skill->die_char && skill->die_char[0] != '\0' )
	fprintf( fpout, "Diechar      %s~\n",	skill->die_char );
    if ( skill->die_vict && skill->die_vict[0] != '\0' )
	fprintf( fpout, "Dievict      %s~\n",	skill->die_vict );
    if ( skill->die_room && skill->die_room[0] != '\0' )
	fprintf( fpout, "Dieroom      %s~\n",	skill->die_room );

    if ( skill->imm_char && skill->imm_char[0] != '\0' )
	fprintf( fpout, "Immchar      %s~\n",	skill->imm_char );
    if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	fprintf( fpout, "Immvict      %s~\n",	skill->imm_vict );
    if ( skill->imm_room && skill->imm_room[0] != '\0' )
	fprintf( fpout, "Immroom      %s~\n",	skill->imm_room );

    if ( skill->dice && skill->dice[0] != '\0' )
	fprintf( fpout, "Dice         %s~\n",	skill->dice );
    if ( skill->value )
	fprintf( fpout, "Value        %d\n",	skill->value );
    if ( skill->difficulty )
	fprintf( fpout, "Difficulty   %d\n",	skill->difficulty );
    if ( skill->participants )
	fprintf( fpout, "Participants %d\n",	skill->participants );
    if ( skill->components && skill->components[0] != '\0' )
	fprintf( fpout, "Components   %s~\n",	skill->components );
    if ( skill->teachers && skill->teachers[0] != '\0' )
	fprintf( fpout, "Teachers     %s~\n",	skill->teachers );
    for ( aff = skill->affects; aff; aff = aff->next )
    {
	fprintf( fpout, "Affect       '%s' %d ", aff->duration, aff->location );
	modifier = atoi(aff->modifier);
	if ((aff->location == APPLY_WEAPONSPELL
	||   aff->location == APPLY_WEARSPELL
	||   aff->location == APPLY_REMOVESPELL
	||   aff->location == APPLY_STRIPSN
	||   aff->location == APPLY_RECURRINGSPELL)
	&&   IS_VALID_SN(modifier) )
	    fprintf( fpout, "'%d' ", skill_table[modifier]->slot );
	else
	    fprintf( fpout, "'%s' ", aff->modifier );
	fprintf( fpout, "%d\n", aff->bitvector );
    }
    fprintf( fpout, "End\n\n" );
}


/*
 * Save the skill table to disk
 */
void save_skill_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( SKILL_FILE, "w" )) == NULL )
    {
	perror( SKILL_FILE );
	bug( "Cannot open skills.dat for writting", 0 );
	return;
    }

    for ( x = 0; x < top_sn; x++ )
    {
	if ( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#SKILL\n" );
	fwrite_skill( fpout, skill_table[x] );
    }
    fprintf( fpout, "#END\n" );
    FCLOSE( fpout );
}

/*
 * Save the herb table to disk
 */
void save_herb_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( HERB_FILE, "w" )) == NULL )
    {
	bug( "Cannot open herbs.dat for writting", 0 );
	perror( HERB_FILE );
	return;
    }

    for ( x = 0; x < top_herb; x++ )
    {
	if ( !herb_table[x]->name || herb_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#HERB\n" );
	fwrite_skill( fpout, herb_table[x] );
    }
    fprintf( fpout, "#END\n" );
    FCLOSE( fpout );
}

/*
 * Save the socials to disk
 */
void save_socials()
{
    FILE *fpout;
    SOCIALTYPE *social;
    int x;

    if ( (fpout=fopen( SOCIAL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open socials.dat for writting", 0 );
	perror( SOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
	for ( social = social_index[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
	    {
		bug( "Save_socials: blank social in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#SOCIAL\n" );
	    fprintf( fpout, "Name        %s~\n",	social->name );
	    if ( social->char_no_arg )
		fprintf( fpout, "CharNoArg   %s~\n",	social->char_no_arg );
	    else
	        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
	    if ( social->others_no_arg )
		fprintf( fpout, "OthersNoArg %s~\n",	social->others_no_arg );
	    if ( social->char_found )
		fprintf( fpout, "CharFound   %s~\n",	social->char_found );
	    if ( social->others_found )
		fprintf( fpout, "OthersFound %s~\n",	social->others_found );
	    if ( social->vict_found )
		fprintf( fpout, "VictFound   %s~\n",	social->vict_found );
	    if ( social->char_auto )
		fprintf( fpout, "CharAuto    %s~\n",	social->char_auto );
	    if ( social->others_auto )
		fprintf( fpout, "OthersAuto  %s~\n",	social->others_auto );
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    FCLOSE( fpout );
}

int get_skill( char *skilltype )
{
    if ( !str_cmp( skilltype, "Race" ) )
      return SKILL_RACIAL;
    if ( !str_cmp( skilltype, "Spell" ) )
      return SKILL_SPELL;
    if ( !str_cmp( skilltype, "Skill" ) )
      return SKILL_SKILL;
    if ( !str_cmp( skilltype, "Weapon" ) )
      return SKILL_WEAPON;
    if ( !str_cmp( skilltype, "Tongue" ) )
      return SKILL_TONGUE;
    if ( !str_cmp( skilltype, "Herb" ) )
      return SKILL_HERB;
    return SKILL_UNKNOWN;
}

/*
 * Save the commands to disk
 * Added flags Aug 25, 1997 --Shaddai
 */
void save_commands()
{
    FILE *fpout;
    CMDTYPE *command;
    int x;

    if ( (fpout=fopen( COMMAND_FILE, "w" )) == NULL )
    {
	bug( "Cannot open commands.dat for writing", 0 );
	perror( COMMAND_FILE );
	return;
    }

    for ( x = 0; x < 126; x++ )
    {
	for ( command = command_hash[x]; command; command = command->next )
	{
	    if ( !command->name || command->name[0] == '\0' )
	    {
		bug( "Save_commands: blank command in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#COMMAND\n" );
	    fprintf( fpout, "Name        %s~\n", command->name		);
	    fprintf( fpout, "Code        %s\n",	 (command->do_fun != skill_notfound && command->do_fun != NULL) ? command->code : "NULL" );
/* Oops I think this may be a bad thing so I changed it -- Shaddai */
	    if ( command->position < 100 )
	       fprintf( fpout, "Position    %d\n",	 command->position+100);
	    else
	       fprintf( fpout, "Position    %d\n",	 command->position);
	    fprintf( fpout, "Level       %d\n",	 command->level		);
	    fprintf( fpout, "Log         %d\n",	 command->log		);
            if ( command->flags )
                fprintf( fpout, "Flags       %d\n",  command->flags);
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    FCLOSE( fpout );
}

SKILLTYPE *fread_skill( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    bool got_info = FALSE;
    SKILLTYPE *skill;
    int x;

    CREATE( skill, SKILLTYPE, 1 );
    skill->slot = 0;
    skill->min_mana = 0;
    for ( x = 0; x < MAX_CLASS; x++ )
    {
	skill->skill_level[x] = LEVEL_IMMORTAL;
	skill->skill_adept[x] = 95;
    }
    for ( x = 0; x < MAX_RACE; x++ )
    {
	skill->race_level[x] = LEVEL_IMMORTAL;
	skill->race_adept[x] = 95;
    }
    skill->target = 0;
    skill->skill_fun = NULL;
    skill->spell_fun = NULL;
    skill->spell_sector = 0;


    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Affect" ) )
	    {
		SMAUG_AFF *aff;

		CREATE( aff, SMAUG_AFF, 1 );
		aff->duration = str_dup( fread_word( fp ) );
		aff->location = fread_number( fp );
		aff->modifier = str_dup( fread_word( fp ) );
		aff->bitvector = fread_number( fp );

		if ( !got_info )
		{
		    for ( x = 0; x < 32; x++ )
		    {
			if ( IS_SET(aff->bitvector, 1 << x) )
			{
			    aff->bitvector = x;
			    break;
			}
		    }
		    if ( x == 32 )
			aff->bitvector = -1;
		}
		aff->next = skill->affects;
		skill->affects = aff;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'C':
	    if ( !str_cmp( word, "Class" ) )
	    {
		int class = fread_number( fp );

		skill->skill_level[class] = fread_number( fp );
		skill->skill_adept[class] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Code" ) )
	    {
		SPELL_FUN *spellfun;
		DO_FUN	  *dofun;
		char	  *w = fread_word(fp);
		
		fMatch = TRUE;
		if ( (spellfun=spell_function(w)) != spell_notfound )
		{
		   skill->spell_fun = spellfun;
		   if ( skill->code )
			DISPOSE( skill->code );
		   skill->code      = str_dup(w);
		   skill->skill_fun = NULL;
		}
		else
		if ( (dofun=skill_function(w)) != skill_notfound )
		{
		   skill->skill_fun = dofun;
		   if ( skill->code )
			DISPOSE( skill->code );
		   skill->code      = str_dup(w);
		   skill->spell_fun = NULL;
		}
		else
		{
		   bug( "fread_skill: unknown skill/spell %s", w );
		   skill->spell_fun = spell_null;
		}
		break;
	    }
	    KEY( "Components",	skill->components,	fread_string_nohash( fp ) );
	    break;
 
	case 'D':
            KEY( "Dammsg",	skill->noun_damage,	fread_string_nohash( fp ) );
	    KEY( "Dice",	skill->dice,		fread_string_nohash( fp ) );
	    KEY( "Diechar",	skill->die_char,	fread_string_nohash( fp ) );
	    KEY( "Dieroom",	skill->die_room,	fread_string_nohash( fp ) );
	    KEY( "Dievict",	skill->die_vict,	fread_string_nohash( fp ) );
	    KEY( "Difficulty",	skill->difficulty,	fread_number( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( skill->saves != 0 && SPELL_SAVE(skill) == SE_NONE )
		{
		    bug( "fread_skill(%s):  Has saving throw (%d) with no saving effect.",
			skill->name, skill->saves );
		    SET_SSAV(skill, SE_NEGATE);
		}
		if ( !skill->code )
		{
		    bug( "%s", "Fread_skill: NULL skill->code." );
		    skill->code	     = str_dup( "" );
		}
		return skill;
	    }
	    break;
	    
	case 'F':
	    if ( !str_cmp( word, "Flags" ) )
	    {
		skill->flags = fread_bitvector(fp);
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'H':
	    KEY( "Hitchar",	skill->hit_char,	fread_string_nohash( fp ) );
	    KEY( "Hitdest",	skill->hit_dest,	fread_string_nohash( fp ) );
	    KEY( "Hitroom",	skill->hit_room,	fread_string_nohash( fp ) );
	    KEY( "Hitvict",	skill->hit_vict,	fread_string_nohash( fp ) );
	    break;

	case 'I':
	    KEY( "Immchar",	skill->imm_char,	fread_string_nohash( fp ) );
	    KEY( "Immroom",	skill->imm_room,	fread_string_nohash( fp ) );
	    KEY( "Immvict",	skill->imm_vict,	fread_string_nohash( fp ) );
	    if ( !str_cmp( word, "Info" ) )
	    {
		skill->info = fread_number(fp);
		got_info = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'M':
	    KEY( "Mana",	skill->min_mana,	fread_number( fp ) );
	    /*KEY( "Minpos",	skill->minimum_position, fread_number( fp ) ); */
	   /*
	    * 
	    */
	    if ( !str_cmp(word, "Minpos") )
	    {
		fMatch = TRUE;
		skill->minimum_position = fread_number(fp);
		if ( skill->minimum_position < 100 )
		{
		    switch( skill->minimum_position )
		    {
			default:
			case 0:
			case 1:
			case 2:
			case 3:
			case 4: break;
			case 5: skill->minimum_position=6; break;
			case 6: skill->minimum_position=8; break;
			case 7: skill->minimum_position=9; break;
			case 8: skill->minimum_position=12; break;
			case 9: skill->minimum_position=13; break;
			case 10: skill->minimum_position=14; break;
			case 11: skill->minimum_position=15; break;
		    }
		}
		else
		    skill->minimum_position-=100;
		break;
	    }

	    KEY( "Misschar",	skill->miss_char,	fread_string_nohash( fp ) );
	    KEY( "Missroom",	skill->miss_room,	fread_string_nohash( fp ) );
	    KEY( "Missvict",	skill->miss_vict,	fread_string_nohash( fp ) );
	    break;
	
	case 'N':
            KEY( "Name",	skill->name,		fread_string_nohash( fp ) );
	    break;

	case 'P':
	    KEY( "Participants",skill->participants,	fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Range",	skill->range,		fread_number( fp ) );
	    KEY( "Rounds",	skill->beats,		fread_number( fp ) );
	    if ( !str_cmp( word, "Race" ) )
	    {
		int race = fread_number( fp );

		skill->race_level[race] = fread_number( fp );
		skill->race_adept[race] = fread_number( fp );
		fMatch = TRUE;
		break;
            }
	    break;

	case 'S':
	    KEY( "Saves",	skill->saves,		fread_number( fp ) );
	    KEY( "Slot",	skill->slot,		fread_number( fp ) );
	    KEY( "Ssector",     skill->spell_sector,    fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Target",	skill->target,		fread_number( fp ) );
	    KEY( "Teachers",	skill->teachers,	fread_string_nohash( fp ) );
	    KEY( "Type",	skill->type,  get_skill(fread_word( fp ))  );
	    break;

	case 'V':
	    KEY( "Value",	skill->value,		fread_number( fp ) );
	    break;

	case 'W':
	    KEY( "Wearoff",	skill->msg_off,		fread_string_nohash( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_skill: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_skill_table()
{
    FILE *fp;

    if ( ( fp = fopen( SKILL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_skill_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SKILL"      ) )
	    {
		if ( top_sn >= MAX_SKILL )
		{
		    bug( "load_skill_table: more skills than MAX_SKILL %d", MAX_SKILL );
		    FCLOSE( fp );
		    return;
		}
		skill_table[top_sn++] = fread_skill( fp );
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_skill_table: bad section.", 0 );
		continue;
	    }
	}
	FCLOSE( fp );
    }
    else
    {
	perror( SKILL_FILE );
	bug( "Cannot open skills.dat", 0 );
 	exit(0);
    }
}


void load_herb_table()
{
    FILE *fp;

    if ( ( fp = fopen( HERB_FILE, "r" ) ) != NULL )
    {
	top_herb = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_herb_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "HERB"      ) )
	    {
		if ( top_herb >= MAX_HERB )
		{
		    bug( "load_herb_table: more herbs than MAX_HERB %d", MAX_HERB );
		    FCLOSE( fp );
		    return;
		}
		herb_table[top_herb++] = fread_skill( fp );
		if ( herb_table[top_herb-1]->slot == 0 )
		    herb_table[top_herb-1]->slot = top_herb-1;
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_herb_table: bad section.", 0 );
		continue;
	    }
	}
	FCLOSE( fp );
    }
    else
    {
	bug( "Cannot open herbs.dat", 0 );
 	exit(0);
    }
}

void fread_social( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SOCIALTYPE *social;

    CREATE( social, SOCIALTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "CharNoArg",	social->char_no_arg,	fread_string_nohash(fp) );
	    KEY( "CharFound",	social->char_found,	fread_string_nohash(fp) );
	    KEY( "CharAuto",	social->char_auto,	fread_string_nohash(fp) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    KEY( "Name",	social->name,		fread_string_nohash(fp) );
	    break;

	case 'O':
	    KEY( "OthersNoArg",	social->others_no_arg,	fread_string_nohash(fp) );
	    KEY( "OthersFound",	social->others_found,	fread_string_nohash(fp) );
	    KEY( "OthersAuto",	social->others_auto,	fread_string_nohash(fp) );
	    break;

	case 'V':
	    KEY( "VictFound",	social->vict_found,	fread_string_nohash(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_socials()
{
    FILE *fp;

    if ( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_socials: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SOCIAL"      ) )
	    {
                fread_social( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_socials: bad section.", 0 );
		continue;
	    }
	}
	FCLOSE( fp );
    }
    else
    {
	bug( "Cannot open socials.dat", 0 );
 	exit(0);
    }
}

/*
 *  Added the flags Aug 25, 1997 --Shaddai
 */

void fread_command( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    CMDTYPE *command;

    CREATE( command, CMDTYPE, 1 );
    command->lag_count = 0; /* can't have caused lag yet... FB */
    command->flags   = 0;  /* Default to no flags set */

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    if ( !str_cmp(word, "Code") )
	    {
		char *w		= fread_word(fp);

		fMatch		= TRUE;

		command->do_fun	= skill_function( w );
		if ( command->code )
		    DISPOSE( command->code );
		command->code	= str_dup( w );

		break;
	    }
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !command->name )
		{
		    bug( "%s", "Fread_command: Name not found" );
		    free_command( command );
		    return;
		}
		if ( !command->do_fun )
		{
		    bug( "%s", "Fread_command: Function not found" );
		    free_command( command );
		    return;
		}
		if ( !command->code )
		{
		    bug( "%s", "Fread_command: Function name not found" );
		    free_command( command );
		    return;
		}
		add_command( command );
		return;
	    }
	    break;

        case 'F':
            KEY ("Flags", command->flags, fread_number (fp));
            break;

	case 'L':
	    KEY( "Level",	command->level,		fread_number(fp) );
	    KEY( "Log",		command->log,		fread_number(fp) );
	    break;

	case 'N':
	    KEY( "Name",	command->name,		fread_string_nohash(fp) );
	    break;

	case 'P':
	    /* KEY( "Position",	command->position,	fread_number(fp) ); */
	    if ( !str_cmp(word, "Position") )
	    {
		fMatch = TRUE;
		command->position = fread_number(fp);
		if( command->position<100 )
		{
		    switch(command->position)
		    {
			default:
			case 0:
			case 1:
			case 2:
			case 3:
			case 4: break;
			case 5: command->position=6; break;
			case 6: command->position=8; break;
			case 7: command->position=9; break;
 			case 8: command->position=12; break;
			case 9: command->position=13; break;
			case 10: command->position=14; break;
			case 11: command->position=15; break;
		    }
		}
		else
		    command->position-=100;
		break;
	    }
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_command: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_commands()
{
    FILE *fp;

    if ( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_commands: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "COMMAND"      ) )
	    {
                fread_command( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_commands: bad section.", 0 );
		continue;
	    }
	}
	FCLOSE( fp );
    }
    else
    {
	bug( "Cannot open commands.dat", 0 );
 	exit(0);
    }

}

void save_classes()
{
    int x;

    for ( x = 0; x < MAX_PC_CLASS; x++ )
      write_class_file( x );
}

/*
 * Tongues / Languages loading/saving functions			-Altrag
 */
void fread_cnv(FILE * fp, LCNV_DATA **first_cnv, LCNV_DATA **last_cnv)
{
    LCNV_DATA *cnv;
    char letter;

    for (;;)
    {
	letter = fread_letter(fp);
	if (letter == '~' || letter == EOF)
	    break;
	ungetc(letter, fp);
	CREATE(cnv, LCNV_DATA, 1);

	cnv->old = str_dup(fread_word(fp));
	cnv->olen = strlen(cnv->old);
	cnv->new = str_dup(fread_word(fp));
	cnv->nlen = strlen(cnv->new);
	fread_to_eol(fp);
	LINK(cnv, *first_cnv, *last_cnv, next, prev);
    }
}

void load_tongues()
{
    FILE *fp;
    LANG_DATA *lng;
    char *word;
    char letter;

    if (!(fp=fopen(TONGUE_FILE, "r")))
    {
	perror("Load_tongues");
	return;
    }
    for (;;)
    {
	letter = fread_letter(fp);
	if (letter == EOF)
	    return;
	else if (letter == '*')
	{
	    fread_to_eol(fp);
	    continue;
	}
	else if (letter != '#')
	{
	    bug("Letter '%c' not #.", letter);
	    exit(0);
	}
	word = fread_word(fp);
	if (!str_cmp(word, "end"))
	    break;
	fread_to_eol(fp);
	CREATE(lng, LANG_DATA, 1);
	lng->name = STRALLOC(word);
	fread_cnv(fp, &lng->first_precnv, &lng->last_precnv);
	lng->alphabet = fread_string(fp);
	fread_cnv(fp, &lng->first_cnv, &lng->last_cnv);
	fread_to_eol(fp);
	LINK(lng, first_lang, last_lang, next, prev);
    }
    FCLOSE(fp);
    return;
}

void fwrite_langs(void)
{
    FILE *fp;
    LANG_DATA *lng;
    LCNV_DATA *cnv;

    if (!(fp=fopen(TONGUE_FILE, "w")))
    {
	perror("fwrite_langs");
	return;
    }
    for (lng = first_lang; lng; lng = lng->next)
    {
	fprintf(fp, "#%s\n", lng->name);
	for (cnv = lng->first_precnv; cnv; cnv = cnv->next)
	    fprintf(fp, "'%s' '%s'\n", cnv->old, cnv->new);
	fprintf(fp, "~\n%s~\n", lng->alphabet);
	for (cnv = lng->first_cnv; cnv; cnv = cnv->next)
	    fprintf(fp, "'%s' '%s'\n", cnv->old, cnv->new);
	fprintf(fp, "\n");
    }
    fprintf(fp, "#end\n\n");
    FCLOSE(fp);
    return;
}

