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

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/$(PROJNAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	-rm -f $(OBJS)