LDLIBS = -lm
CFLAGS = -Wall -Wextra

all: fnsingen

fsingen: fsingen.c

clean:
	rm fnsingen

.PHONY: clean all
