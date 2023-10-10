LDFLAGS=`pkg-config --libs --cflags --static libmicrohttpd uuid libcurl`
CCFLAGS = -O3 -Wextra -Wall $LDFLAGS

estimate: estimate.c
compile-views: compile-views.c

test: estimate.o test.c

.PHONY: run, run-check, run-test, install, clean, run-compile-views

run-compile-views: compile-views
	valgrind ./compile-views

run: estimate
	./estimate 4000

run-check: estimate
	valgrind --leak-check=full ./estimate 4000

run-test: test
	./test

install: estimate
	install -Dm755 estimate /usr/bin/estimate

clean:
	rm -f *.o estimate test
