CC=gcc
CFLAGS=-I. -lncurses

SRCDIR=src
OBJDIR=obj
BINDIR=bin

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/project: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	