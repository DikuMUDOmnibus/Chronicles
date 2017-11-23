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
 * - Ban module header                                                     *
 ***************************************************************************/

typedef struct  ban_data                BAN_DATA;

/*
 * Ban function definitions.
 */
/* ban.c */
int	add_ban			args( ( CHAR_DATA *ch, char *arg1, char *arg2,int time,int type ) );
void	show_bans		args( ( CHAR_DATA *ch, int type ) );
void	save_banlist		args( ( void ) );
void	load_banlist		args( ( void ) );
bool	check_total_bans	args( ( DESCRIPTOR_DATA *d ) );
bool	check_bans		args( ( CHAR_DATA *ch, int type ) );

/*
 * Ban Data File.
 */
#define	BAN_LIST	"ban.lst"	/* List of bans		*/

/*
 * Ban Types --- Shaddai
 */
typedef enum
{
  BAN_WARN = -1, BAN_SITE = 1, BAN_CLASS, BAN_RACE
} ban_types;

/*
 * Site ban structure.
 */
struct  ban_data
{
    BAN_DATA *next;
    BAN_DATA *prev;
    char     *name;     /* Name of site/class/race banned */
    char     *user;     /* Name of user from site */
    char     *note;     /* Why it was banned */
    char     *ban_by;   /* Who banned this site */
    char     *ban_time; /* Time it was banned */
    int      flag;      /* Class or Race number */
    int      unban_date;/* When ban expires */
    sh_int   duration;  /* How long it is banned for */
    sh_int   level;     /* Level that is banned */
    bool     warn;      /* Echo on warn channel */
    bool     prefix;    /* Use of *site */
    bool     suffix;    /* Use of site* */
};

extern          BAN_DATA          *     first_ban;
extern          BAN_DATA          *     last_ban;
extern          BAN_DATA          *     first_ban_class;
extern          BAN_DATA          *     last_ban_class;
extern          BAN_DATA          *     first_ban_race;
extern          BAN_DATA          *     last_ban_race;
