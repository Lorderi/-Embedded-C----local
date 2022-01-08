bin := main
source := main.c
obj := main.o
header := func.h
stlib := libfunc.a
CC := gcc

.PHONY: debug clean
#CFlags/LDFLAGS

$(bin): $(source) $(header) $(stlib)
		$(CC) $(source) -o $@ -L. -lfunc
$(stlib): func*.o
		ar rc $@ $^
%.o: func*.c
		$(CC) $^ -c


debug: $(source) $(header)
		$(CC) $(source) -o $(bin) -L. -lfunc -g
clean:
		rm -rf $(bin) *.o

