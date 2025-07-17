CC = gcc
CFLAGS = -Wall -W -O2

GOAL = findaes
OBJ  = main.o aes.o

all: $(GOAL)

cross: CC = i586-mingw32msvc-gcc
cross: EXT = .exe
cross: $(GOAL)

findaes: $(OBJ)
	$(CC) $(CFLAGS) -o $(GOAL)$(EXT) $(OBJ)

nice:
	rm -f *~

clean: nice
	rm -f $(GOAL) $(GOAL).exe $(OBJ)

DESTDIR = $(GOAL)-1.0

package:
	rm -rf $(DESTDIR) $(DESTDIR).rar
	mkdir $(DESTDIR)
	cp main.c aes.c aes.h Makefile $(DESTDIR)
	rar a $(DESTDIR).rar -m5 $(DESTDIR)
	rm -rf $(DESTDIR)
