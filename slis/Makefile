PROGRAM=slis
OBJS=slis.o

CC=gcc
CCFLAGS=

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $^

.c.o:
	$(CC) $(CCFLAGS) -c $<

clean:
	$(RM) $(PROGRAM) $(OBJS)
