CC=gcc
CFLAGS=-I. -lncurses -lm

SRCDIR=src
OBJDIR=obj
BINDIR=bin
PROJNAME=pacman

DEPS=$(wildcard $(SRCDIR)/*.h)
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

.PHONY : clean

all: directories $(BINDIR)/$(PROJNAME)

directories: 
	@mkdir -p $(OBJDIR) $(BINDIR)

$(BINDIR)/$(PROJNAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	-rm -f $(OBJS)