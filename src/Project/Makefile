CC = gcc
CFLAGS = -I./
SRCS = applayer.c datalink.c
OBJS = $(SRCS:.c=.o)
MAIN = application

.PHONY: depend clean

all: $(MAIN)
	@echo  Binary $(MAIN) created successfuly

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it	
