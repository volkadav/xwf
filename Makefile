# a simple makefile for xwf

# OPTIONS
CC=gcc
# distrib:
CCFLAGS=-lexpat -O3 -march=native -ffast-math -s -Wall
# debug:
#CCFLAGS=-lexpat -Wall -g

PROGNAME=xwf
INSTPATH=/usr/local/bin

OWNER=root
GROUP=staff
MODE=0555

############################

${PROGNAME}: xwf.c
	${CC} xwf.c ${CCFLAGS} -o ${PROGNAME}
	
install:
	/usr/bin/install --mode ${MODE} --owner ${OWNER} --group ${GROUP} ./${PROGNAME} ${INSTPATH}/${PROGNAME}

clean:
	rm *.o
	rm ${PROGNAME}
