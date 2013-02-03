####
##
## NetFrag
##
## Author: Gabriel Gonzalez - 2013
##
#${PREFIX}=
CC=${PREFIX}gcc
LD=${PREFIX}ld
SOURCES=${wildcard *.c}
OBJECTS=${SOURCES:%.c=%.o}
CFLAGS+=-Wall -ggdb
APP=netfrag

all:	${OBJECTS}
	${CC} -o ${APP} $< ${LDFLAGS}

%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

clean:
	rm -f *.o ${APP}
