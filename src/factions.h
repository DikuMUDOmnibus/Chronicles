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
 * - Faction module header                                                 *
 ***************************************************************************/

typedef struct	faction_data		FACTION_DATA;

/*
 * Factions macros.
 */
#define HAS_FACTION_FLAG(faction, flag)	(xIS_SET((faction)->flags, flag))
#define IS_CLANNED(ch)			(!IS_NPC((ch))						\
					&& (ch)->pcdata->faction				\
					&& (ch)->pcdata->faction->faction_type == FACTION_CLAN)
#define IS_ORDERED(ch)			(!IS_NPC((ch))						\
					&& (ch)->pcdata->faction				\
					&& (ch)->pcdata->faction->faction_type == FACTION_ORDER)
#define IS_GUILDED(ch)			(!IS_NPC((ch))						\
					&& (ch)->pcdata->faction				\
					&& (ch)->pcdata->faction->faction_type == FACTION_GUILD)

/*
 * Faction function definitions.
 */
FACTION_DATA *	get_faction	args( ( char *name ) );
void		load_factions	args( ( void ) );
void		save_faction	args( ( FACTION_DATA *faction ) );

/*
 * Faction types listing.
 */
typedef enum
{
  FACTION_CLAN,		FACTION_ORDER,		FACTION_GUILD,
  MAX_FACTION
} faction_types;

/*
 * Faction flags for multiple uses
 */
typedef enum
{
  FACTION_ANTI_MAGE,	FACTION_ANTI_CLERIC,	FACTION_ANTI_THIEF,
  FACTION_ANTI_WARRIOR,	FACTION_ANTI_VAMPIRE,	FACTION_ANTI_DRUID,
  FACTION_ANTI_RANGER,	FACTION_ANTI_AUGURER,	FACTION_ANTI_PALADIN,
  FACTION_ANTI_GOOD,	FACTION_ANTI_NEUTRAL,	FACTION_ANTI_EVIL,
  MAX_FACTION_FLAG
} faction_flags;

struct  faction_data
{
    FACTION_DATA *	next;	/* Next faction in list		*/
    FACTION_DATA *	prev;	/* Previous faction in list	*/
    char *	filename;	/* Filename			*/
    char *	name;		/* Name				*/
    char *	motto;		/* Motto			*/
    char *	description;	/* A brief description		*/
    char *	deity;		/* Deity			*/
    char *	leader;		/* Leader			*/
    char *	number1;	/* First officer		*/
    char *	number2;	/* Second officer		*/
    char *	badge;		/* Badge on who/where/to_room	*/
    char *	leadrank;	/* Leader's rank		*/
    char *	onerank;	/* Number One's rank		*/
    char *	tworank;	/* Number Two's rank		*/
    int		pkills[7];	/* Number of pkills		*/
    int		pdeaths[7];	/* Number of pdeaths		*/
    int		mkills;		/* Number of mkills		*/
    int		mdeaths;	/* Number of mdeaths		*/
    int		illegal_pk;	/* Number of illegal pks	*/
    int		score;		/* Overall score		*/
    sh_int	faction_type;	/* See type defines		*/
    sh_int	favour;		/* Deities favour		*/
    sh_int	strikes;	/* Number of strikes		*/
    sh_int	members;	/* Number of members		*/
    sh_int	mem_limit;	/* Number of members allowed	*/
    sh_int	alignment;	/* General alignment		*/
    int		board;		/* Vnum of board		*/
    int		obj1;		/* Vnum of first obj		*/
    int		obj2;		/* Vnum of second obj		*/
    int		obj3;		/* Vnum of third obj		*/
    int		obj4;		/* Vnum of fourth obj		*/
    int		obj5;		/* Vnum of fifth obj		*/
    int		recall;		/* Vnum of recall room		*/
    int		storeroom;	/* Vnum of store room		*/
    EXT_BV	flags;		/* Flags for various things	*/
};

extern          FACTION_DATA	*	first_faction;
extern          FACTION_DATA	*	last_faction;
