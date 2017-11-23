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
 * - Polymorph module header                                               *
 ***************************************************************************/

typedef struct  char_morph              CHAR_MORPH;
typedef struct  morph_data              MORPH_DATA;

/*
 * Polymorph macros.
 */
#define MORPHPERS(ch, looker)	( can_see( (looker), (ch) ) ?			\
				(ch)->morph->morph->short_desc : "someone" )

/*
 * Polymorph function definitions.
 */
void		fwrite_morph_data	args( ( CHAR_DATA *ch, FILE *fp ) );
void		fread_morph_data	args( ( CHAR_DATA *ch, FILE *fp ) );
void		clear_char_morph	args( ( CHAR_MORPH *morph ) );
CHAR_MORPH *	make_char_morph		args( ( MORPH_DATA *morph ) );
void		free_char_morph		args( ( CHAR_MORPH *morph ) );
char *		race_string		args( ( int bitvector ) );
char *		class_string		args( ( int bitvector ) );
void		setup_morph_vnum	args( ( void ) );
void		unmorph_all		args( ( MORPH_DATA *morph ) );
MORPH_DATA *	get_morph		args( ( char *arg ) );
MORPH_DATA *	get_morph_vnum		args( ( int arg ) );
int		do_morph_char		args( ( CHAR_DATA *ch, MORPH_DATA *morph ) );
MORPH_DATA *	find_morph		args( ( CHAR_DATA *ch, char *target, bool is_cast ) );
void		do_unmorph_char		args( ( CHAR_DATA *ch ) );
void		send_morph_message	args( ( CHAR_DATA *ch, MORPH_DATA *morph, bool is_morph ) );
bool		can_morph		args( ( CHAR_DATA *ch, MORPH_DATA *morph, bool is_cast ) );
void		do_morph		args( ( CHAR_DATA *ch, MORPH_DATA *morph ) );
void		do_unmorph		args( ( CHAR_DATA *ch ) );
void		save_morphs		args( ( void ) );
void		fwrite_morph		args( ( FILE *fp, MORPH_DATA *morph ) );
void		load_morphs		args( ( void ) );
MORPH_DATA *	fread_morph		args( ( FILE *fp ) );
void		free_morph		args( ( MORPH_DATA *morph ) );
void		morph_defaults		args( ( MORPH_DATA *morph ) );
void		sort_morphs		args( ( void ) );

/*
 * Definitions for use with the polymorph code.
 */
#define ONLY_PKILL	1
#define ONLY_PEACEFULL	2

struct char_morph
{
    MORPH_DATA * morph;
    EXT_BV	affected_by;		/* New affected_by added		*/
    EXT_BV	no_affected_by;		/* Prevents affects from being added	*/
    int		no_immune;		/* Prevents Immunities			*/
    int		no_resistant;		/* Prevents resistances			*/
    int		no_suscept;		/* Prevents Susceptibilities		*/
    int		immune;			/* Immunities added			*/
    int		resistant;		/* Resistances added			*/
    int		suscept;		/* Suscepts added			*/
    int		timer;			/* How much time is left		*/
    sh_int	ac;
    sh_int	blood;
    sh_int	cha;
    sh_int	con;
    sh_int	damroll;
    sh_int	dex;
    sh_int	dodge;
    sh_int	hit;
    sh_int	hitroll;
    sh_int	inte;
    sh_int	lck;
    sh_int	mana;
    sh_int	move;
    sh_int	parry;
    sh_int	saving_breath;
    sh_int	saving_para_petri;
    sh_int	saving_poison_death;
    sh_int	saving_spell_staff;
    sh_int	saving_wand;
    sh_int	str;
    sh_int	tumble;
    sh_int	wis;
};

struct morph_data
{
    MORPH_DATA * next;			/* Next morph file			*/
    MORPH_DATA * prev;			/* Previous morph file			*/
    char *	blood;			/* Blood added vamps only		*/
    char *	damroll;
    char *	deity;
    char *	description;
    char *	help;			/* What player sees for info on morph	*/
    char *	hit;			/* Hitpoints added			*/
    char *	hitroll;
    char *	key_words;		/* Keywords added to your name		*/
    char *	long_desc;		/* New long_desc for player		*/
    char *	mana;			/* Mana added not for vamps		*/
    char *	morph_other;		/* What others see when you morph	*/
    char *	morph_self;		/* What you see when you morph		*/
    char *	move;			/* Move added				*/
    char *	name;			/* Name used to polymorph into this	*/
    char *	short_desc;		/* New short desc for player		*/
    char *	no_skills;		/* Prevented Skills			*/
    char *	skills;
    char *	unmorph_other;		/* What others see when you unmorph	*/
    char *	unmorph_self;		/* What you see when you unmorph	*/
    EXT_BV	affected_by;		/* New affected_by added		*/
    int		class;			/* Classes not allowed to use this	*/
    int		defpos;			/* Default position			*/
    EXT_BV	no_affected_by;		/* Prevents affects from being added	*/
    int		no_immune;		/* Prevents Immunities			*/
    int		no_resistant;		/* Prevents resistances			*/
    int		no_suscept;		/* Prevents Susceptibilities		*/
    int		immune;			/* Immunities added			*/
    int		resistant;		/* Resistances added			*/
    int		suscept;		/* Suscepts added			*/
    int		obj[3];			/* Object needed to morph you		*/
    int		race;			/* Races not allowed to use this	*/
    int		timer;			/* Timer for how long it lasts		*/
    int		used;			/* The amount the morph has been used	*/
    int		vnum;			/* Unique identifier			*/
    sh_int	ac;
    sh_int	bloodused;		/* Blood morph requires of bloodsuckers	*/
    sh_int	cha;			/* Amount Cha gained/Lost		*/
    sh_int	con;			/* Amount of Con gained/Lost		*/
    sh_int	dayfrom;		/* Starting Day you can morph into this	*/
    sh_int	dayto;			/* Ending Day you can morph into this	*/
    sh_int	dex;			/* Amount of dex added			*/
    sh_int	dodge;			/* Percent of dodge added IE 1 = 1%	*/
    sh_int	favourused;		/* Amount of favour to morph		*/
    sh_int	gloryused;		/* Amount of glory used to morph	*/
    sh_int	hpused;			/* Amount of hps used to morph		*/
    sh_int	inte;			/* Amount of Int gained/lost		*/
    sh_int	lck;			/* Amount of Lck gained/lost		*/
    sh_int	level;			/* Minimum level to use this morph	*/
    sh_int	manaused;		/* Amount of mana used to morph		*/
    sh_int	moveused;		/* Amount of move used to morph		*/
    sh_int	parry;			/* Percent of parry added IE 1 = 1%	*/
    sh_int	pkill;			/* Pkill Only, Peacefull Only or Both	*/
    sh_int	saving_breath;		/* Below are saving adjusted		*/
    sh_int	saving_para_petri;
    sh_int	saving_poison_death;
    sh_int	saving_spell_staff;
    sh_int	saving_wand;
    sh_int	sex;			/* The sex that can morph into this	*/
    sh_int	str;			/* Amount of str gained lost		*/
    sh_int	timefrom;		/* Hour starting you can morph		*/
    sh_int	timeto;			/* Hour ending that you can morph	*/
    sh_int	tumble;			/* Percent of tumble added IE 1 = 1%	*/
    sh_int	wis;			/* Amount of Wis gained/lost		*/
    bool	no_cast;		/* Can you cast a spell to morph	*/
    bool	objuse[3];		/* Objects needed to morph		*/
};
