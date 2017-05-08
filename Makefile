EXEC_NAME = watchdog-test
SRCS = watchdog_test.c
CFLAGS = -O2 -Wall
OBJS = $(SRCS:.c=.o)

all: $(EXEC_NAME)

OBJS: $(SRCS:.c=.o)
	$(CC) $(CFLAGS) -c $<

$(EXEC_NAME): $(OBJS)
	$(CC) -o $(EXEC_NAME) ${OBJS} $(LDFLAGS) $(LIBS)


clean:
	rm -f *.o $(EXEC_NAME)

