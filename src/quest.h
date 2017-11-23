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
 * - Quest module header                                                   *
 ***************************************************************************/

typedef struct	quest_data		QUEST_DATA;
typedef struct	qprize_data		QPRIZE_DATA;
typedef struct	qtarg_data		QTARG_DATA;

/*
 * Local Function Routines
 */
CHAR_DATA	*find_questmaster	args(( CHAR_DATA *ch, bool canBeImm ));
void		quest_update		args(( void ));
void		clear_questdata		args(( CHAR_DATA *ch, bool quit ));
void		generate_quest		args(( CHAR_DATA *ch, CHAR_DATA *questmaster ));
bool		valid_leveldiff		args(( CHAR_DATA *ch, CHAR_DATA *target ));
bool		quest_chance		args(( int percentage ));
void		check_quest_obj		args(( OBJ_DATA *obj ));

bool		load_qdata		args(( QUEST_DATA *qd ));
void		load_questdata		args(( void ));
void		write_qdata		args(( void ));
void		write_questdata		args(( void ));
void		remove_questdata	args(( bool qPrize, int nCount ));
void		fread_qdata		args(( QUEST_DATA *qd, FILE *fpin ));
QPRIZE_DATA	*fread_qprize		args(( FILE *fpin ));
QTARG_DATA	*fread_qtarg		args(( FILE *fpin ));


/*
 * Quest Data File
 */
#define QUEST_DIR		"../quest/"
#define QDATA_FILE		QUEST_DIR "qdata.dat"
#define QUESTDATA_FILE		QUEST_DIR "quest.dat"

/*
 * Used to set/change questmaster settings/options -Orion
 */
struct  quest_data
{
    /*
     * Quest Reward/Punishment Amounts
     */
    int			minprac_gain;
    int			maxprac_gain;
    int			mingold_gain;
    int			maxgold_gain;
    int			minqp_gain;
    int			maxqp_gain;
    int			minqt_comp;
    int			maxqt_comp;
    int			minq_time;
    int			maxq_time;
    int			qp_loss;
    int			qtime_next_quit;
    int			qtime_next_fail;
    int			qtime_next_clear;

    /*
     * Quest Parameters
     */
    int			qlevel_diff;
    int			quest_chance;
    int			object_chance;
    int			give_quest;
    int			prac_chance;
};

/*
 * Used to set/change questmaster object prizes -Orion
 */
struct  qprize_data
{
    /*
     * Object Values for Questmaster's Prizes
     */
    int			vnum;				/* Prize VNum           */
    int			cost;				/* Prize Cost           */
    int			min_level;			/* Prize Minimum Level  */
    int			max_level;			/* Prize Maximum Level  */
    QPRIZE_DATA *	next;				/* Next Prize in LL     */
    QPRIZE_DATA *	prev;				/* Previous Prize in LL */
};

/*
 * Used to set/change questmaster object targets -Orion
 */
struct  qtarg_data
{
    /*
     * Object Target Values
     */
    int			vnum;				/* Target VNum		*/
    QTARG_DATA  *	next;				/* Next Target in LL	*/
    QTARG_DATA  *	prev;				/* Prev Target in LL	*/
};

extern		QPRIZE_DATA	*	first_qprize;	/* Orion Elder		*/
extern		QPRIZE_DATA	*	last_qprize;	/* Orion Elder		*/
extern		QTARG_DATA	*	first_qtarg;	/* Orion Elder		*/
extern		QTARG_DATA	*	last_qtarg;	/* Orion Elder		*/
extern		struct  quest_data	qdata;
/*
 * Quest ENum Defines,
 * Quest Type Enum Listing -Orion
 */
typedef enum
{
  QT_NONE = -1, QT_MOB_KILL, QT_MOB_GIVE, QT_MOB_GIVE_COMPLETE, QT_OBJ, QT_COMPLETE, QT_FAILED, MAX_QT
} quest_types;


/*
 * Quest Timer Enum Listing -Orion
 *
 * If adding a new timer, add to the quest_timer definitions in the mud.h file.
 */
typedef enum
{
  QTIMER_NEXT, QTIMER_CURR
} quest_timers;

