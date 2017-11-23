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
 * - Language module header                                                *
 ***************************************************************************/

typedef struct	lcnv_data		LCNV_DATA;
typedef struct	lang_data		LANG_DATA;

/*
 * Language function definitions.
 */
int	knows_language	args( ( CHAR_DATA *ch, int language, CHAR_DATA *cch ) );
char *	translate	args( ( int percent, const char *in, const char *name ) );

/*
 * Language bitvector defitions.
 */
#define	LANG_UNKNOWN        0	/* Anything that doesn't fit	*/
#define	LANG_COMMON      BV00	/* Human base language		*/
#define	LANG_ELVEN       BV01	/* Elven base language		*/
#define	LANG_DWARVEN     BV02	/* Dwarven base language	*/
#define	LANG_PIXIE       BV03	/* Pixie/Fairy base language	*/
#define	LANG_OGRE        BV04	/* Ogre base language		*/
#define	LANG_ORCISH      BV05	/* Orc base language		*/
#define	LANG_TROLLISH    BV06	/* Troll base language		*/
#define	LANG_RODENT      BV07	/* Small mammals		*/
#define	LANG_INSECTOID   BV08	/* Insects			*/
#define	LANG_MAMMAL      BV09	/* Larger mammals		*/
#define	LANG_REPTILE     BV10	/* Small reptiles		*/
#define	LANG_DRAGON      BV11	/* Large reptiles, Dragons	*/
#define	LANG_SPIRITUAL   BV12	/* Dead/spirit related stuff	*/
#define	LANG_MAGICAL     BV13	/* Magical creatures and spells	*/
#define	LANG_GOBLIN      BV14	/* Goblin base language		*/
#define	LANG_GOD         BV15	/* God creatures		*/
#define	LANG_ANCIENT     BV16	/* Prelude to reading glyphs?	*/
#define	LANG_HALFLING    BV17	/* Halfling base language	*/
#define	LANG_CLAN        BV18	/* Clan language		*/
#define	LANG_GITH        BV19	/* Gith Language		*/
#define	LANG_GNOMISH     BV20	/* Gnome Language		*/
#define	LANG_INFERNAL    BV21	/* Demon Language		*/
#define	LANG_CELESTIAL   BV22	/* Angel Language		*/
#define	LANG_UNCOMMON    BV23	/* Undercommon, not quite... eh	*/
#define	LANG_GNOLLISH    BV24	/* Gnoll Language		*/
#define	VALID_LANGS	( LANG_COMMON	| LANG_ELVEN	| LANG_DWARVEN		\
			| LANG_PIXIE	| LANG_OGRE	| LANG_ORCISH		\
			| LANG_TROLLISH	| LANG_GOBLIN	| LANG_HALFLING		\
			| LANG_GITH	| LANG_GNOMISH	| LANG_INFERNAL		\
			| LANG_CELESTIAL | LANG_DRAGON	| LANG_UNCOMMON		\
			| LANG_GNOLLISH	)

struct lcnv_data
{
    LCNV_DATA *	next;
    LCNV_DATA *	prev;
    char *	old;
    int		olen;
    char *	new;
    int		nlen;
};

struct lang_data
{
    LANG_DATA *	next;
    LANG_DATA *	prev;
    char *	name;
    LCNV_DATA *	first_precnv;
    LCNV_DATA *	last_precnv;
    char *	alphabet;
    LCNV_DATA *	first_cnv;
    LCNV_DATA *	last_cnv;
};

extern		LANG_DATA	*	first_lang;
extern		LANG_DATA	*	last_lang;

extern	int	const	lang_array[];
extern	char *	const	lang_names[];
