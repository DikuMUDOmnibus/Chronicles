CC      = gcc
#PROF   = -p
#NOCRYPT =

#    The lines below are for debugging information
USE_GDB = gdb
DEBUG_LEVEL = 2

#    The program executable's name
PROG_NAME = 13C

#    Uncomment the two lines below if compiling on a Solaris box
#SOLARIS_FLAG = -Dsun -DSYSV
#SOLARIS_LINK = -lnsl -lsocket

#    Uncomment the line below if compiling on a RedHat (and possibly other Linux) box
#OS = -DLINUX
#    Uncomment the line below if compiling on a Win32 box
#OS = -DWIN32

#    Uncomment the line below if you are getting a line like:
#interp.c:757: warning: int format, time_t arg (arg 7)
#TIME = -DTIMEFORMAT

#    Uncomment the line below if you are getting undefined crypt errors
NEED_CRYPT = -lcrypt

#IMC2 - Comment out to disable IMC2 support
IMC = 1

#    Directory locations of important lib files
LIBDIR	= ./lib

#    Modify the lines below if you want a performance increase though beware your core
#    files may not be as much of a benefit if you do. Suggested OPT_FLAG: -O
OPT_LEVEL = 
OPT_FLAG = -O$(OPT_LEVEL)

#    Format check flags
#FMT_FLG = -Wformat -Wformat-security -Wmissing-format-attribute

#    Warning check flags
WRN_FLG = -Wall

#    Compile and Link flags
CMP_FLG = -g$(USE_GDB)$(DEBUG_LEVEL) $(WRN_FLG) $(FMT_FLG)
C_FLAGS = $(OPT_FLAG) $(CMP_FLG) $(PROF) $(NOCRYPT) $(SOLARIS_FLAG) $(TIME) $(OS)
L_FLAGS = $(OPT_FLAG) $(PROF) $(SOLARIS_LINK) $(NEED_CRYPT) -L$(LIBDIR)

#    Listing of all '.o' files necessary to create the program binary
O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o ban.o bank.o boards.o \
          build.o colorize.o comm.o comments.o const.o copyover.o db.o deity.o factions.o \
          fight.o handler.o hashstr.o ident.o imm_host.o interp.o magic.o misc.o mud_comm.o \
          mud_prog.o news.o player.o polymorph.o quest.o reset.o save.o services.o shops.o \
          skills.o special.o tables.o track.o update.o

#    Listing of all '.c' files necessary to create the program binary (similar to above)
C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c ban.c bank.c boards.c \
          build.c colorize.c comm.c comments.c const.c copyover.c db.c deity.c factions.c \
          fight.c handler.c hashstr.c ident.c imm_host.c interp.c magic.c misc.c mud_comm.c \
          mud_prog.c news.c player.c polymorph.c quest.c reset.c save.c services.c shops.c \
          skills.c special.c tables.c track.c update.c

#    Listing of all '.h' files necessary to create the program binary
H_FILES = auction.h ban.h bank.h copyover.h councils.h deity.h factions.h files.h language.h \
          mud.h news.h polymorph.h quest.h

ifdef IMC
   C_FILES := imc.c sha256.c $(C_FILES)
   O_FILES := imc.o sha256.o $(O_FILES)
   C_FLAGS := $(C_FLAGS) -DIMC -DIMCSMAUG
endif

all:
	@echo ""
	@echo "(@)=======[ Chronicles Codebase ]=======(@)"
	@echo " Checking Chronicles, recompiling if necessary  "
	@$(MAKE) $(PROG_NAME)
	@echo " ..done"
	@echo "(@)=====================================(@)"

$(PROG_NAME): $(O_FILES)
	@echo ""
	@echo " Compiling MUD systems into binary now  "
	@rm -f $(PROG_NAME)
	@$(CC) -o $(PROG_NAME) $(O_FILES) $(L_FLAGS)
	@chmod 700 $(PROG_NAME)
	@chmod 700 $(O_FILES)

.c.o: $(H_FILES)
	$(CC) -c $(C_FLAGS) $<

clean:
	@echo "(@)=======[ Chronicles Codebase ]=======(@)"
	@echo " Cleaning up object and binary files  "
	@rm -f *.o $(PROG_NAME) *~
	@echo " ..done"
	@echo "(@)=====================================(@)"
