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
 *   Hans-Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.                   *
 ***************************************************************************
 * - Deity module header                                                   *
 ***************************************************************************/

typedef struct	deity_data		DEITY_DATA;

/*
 * Deity macros.
 */
#define IS_DEVOTED(ch)		(!IS_NPC((ch)) && (ch)->pcdata->deity)

/*
 * Deity function definitions.
 */
DEITY_DATA *	get_deity	args( ( char *name ) );
void		load_deity	args( ( void ) );
void		save_deity	args( ( DEITY_DATA *deity ) );
void		adjust_favor	args( ( CHAR_DATA *ch, int field, int mod ) );

struct  deity_data
{
    DEITY_DATA * next;
    DEITY_DATA * prev;
    char *	filename;
    char *	name;
    char *	description;
    sh_int	alignment;
    sh_int	worshippers;
    sh_int	scorpse;
    sh_int	sdeityobj;
    sh_int	savatar;
    sh_int	srecall;
    sh_int	flee;
    sh_int	flee_npcrace;
    sh_int	flee_npcfoe;
    sh_int	kill;
    sh_int	kill_magic;
    sh_int	kill_npcrace;
    sh_int	kill_npcfoe;
    sh_int	sac;
    sh_int	bury_corpse;
    sh_int	aid_spell;
    sh_int	aid;
    sh_int	backstab;
    sh_int	steal;
    sh_int	die;
    sh_int	die_npcrace;
    sh_int	die_npcfoe;
    sh_int	spell_aid;
    sh_int	dig_corpse;
    int		race;
    int		race2;
    int		class;
    int		sex;
    int		npcrace;
    int		npcfoe;
    int		suscept;
    int		element;
    EXT_BV	affected;
    int		susceptnum;
    int		elementnum;
    int		affectednum;
    int		objstat;
};

extern		DEITY_DATA	*	first_deity;
extern		DEITY_DATA	*	last_deity;
