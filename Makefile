LDFLAGS=`pkg-config --libs --cflags --static libmicrohttpd uuid libcurl`
CCFLAGS = -Wextra -Wall $LDFLAGS

.PHONY: run, run-check, run-test, install, clean

compile-views: compile-views.c

views_funcs.c: compile-views views/*.html
	./compile-views

estimate: helpers.c  string.c views_funcs.c views_funcs.c

test: estimate.o test.c

run: estimate
	./estimate 4000

run-check: estimate
	valgrind --leak-check=full --track-origins=yes ./estimate 4000

run-test: test
	./test

install: estimate
	install -Dm755 estimate /usr/bin/estimate

clean:
	rm -f *.o estimate test
