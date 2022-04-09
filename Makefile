CC = gcc
CFLAGS = -g -Wall

all: wserver simple term slowcgi testprogtable large

wserver: wserver.o wrapsock.o progtable.o ws_helpers.o process_request.o
	${CC} ${CFLAGS} -o $@ $^  

slowcgi : slowcgi.o
	${CC} ${CFLAGS} -o $@ $^  

testprogtable : testprogtable.o progtable.o
	${CC} ${CFLAGS} -o $@ $^  

simple : simple.o cgi.o
	${CC} ${CFLAGS} -o $@ $^  
large : large.o cgi.o
	${CC} ${CFLAGS} -o $@ $^  
term : term.o
	${CC} ${CFLAGS} -o $@ $^  

%.o : %.c
	${CC} ${CFLAGS}  -c $<

clean:
	rm *.o wserver simple term slowcgi large testprogtable

# Dependencies
cgi.o : cgi.h
large.o : cgi.h
process_request.o : ws_helpers.h wrapsock.h
simple.o : cgi.h
wrapsock.o : wrapsock.h
ws_helpers.o : wrapsock.h ws_helpers.h
wserver.o : wrapsock.h ws_helpers.h
