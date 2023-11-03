LDFLAGS=`pkg-config --libs --cflags --static libmicrohttpd uuid libcurl`
CCFLAGS = -Wextra -Wall -g $LDFLAGS

.PHONY: run, install, clean

compile-views: compile-views.o

views.c: compile-views views/*.html
	./compile-views

main: helpers.o string.o views.o hash.o models.o

run: main
	valgrind --leak-check=full --track-origins=yes ./main 4000

install: estimate
	install -Dm755 estimate /usr/bin/estimate

clean:
	rm -f *.o estimate compile-views
