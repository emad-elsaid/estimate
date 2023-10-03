LDFLAGS=`pkg-config --libs --cflags --static libmicrohttpd`
CCFLAGS = -O3 -Wextra -Wall $LDFLAGS

estimate: estimate.c

test: estimate.o test.c

.PHONY: run, run-check, run-test, install, clean

run: estimate
	./estimate 4000

run-check: estimate
	valgrind -s --leak-check=full ./estimate 4000

run-test: test
	./test

install: estimate
	install -Dm755 estimate /usr/bin/estimate

clean:
	rm -f *.o estimate test
