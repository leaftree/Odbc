CC = gcc -g3 -Wall

CCCOLOR   = "\033[33m"
LINKCOLOR = "\033[34;1m"
SRCCOLOR  = "\033[31m"
RMCOLOR   = "\033[1;31m"
BINCOLOR  = "\033[37;1m"
MAKECOLOR = "\033[32;1m"
ENDCOLOR  = "\033[0m"

QUIET_CC  = @printf '%b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_RM  = @printf '%b %b\n' $(LINKCOLOR)REMOVE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_EXE = @printf '%b %b\n' $(LINKCOLOR)EXEC$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '%b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;

QUIET_PROC=$(QUIET_CC)$(CC)
QUIET_EXEC=$(QUIET_EXE)exec
QUIET_REMOVE=$(QUIET_RM)rm -f

inc += DB_Api.h Log.h main.h List.h
src  = DB_Api.c Log.c main.c List.c
obj  = DB_Api.o Log.o main.o List.o

bin  = Main

lib += -lodbc

all:$(bin)

$(bin):$(obj) $(inc)
	$(QUIET_PROC) $(obj) $(lib) -o $(bin)

.c.o:$(src)
	$(QUIET_PROC) -c $<

run:$(obj) $(bin)
	$(QUIET_EXEC) $(PWD)/$(bin)

tags:$(src) $(inc)
	-@ctags $(src) $(inc)

.PHONY:clean
clean:
	$(QUIET_REMOVE) -f $(obj) $(bin) *.gch core.*
