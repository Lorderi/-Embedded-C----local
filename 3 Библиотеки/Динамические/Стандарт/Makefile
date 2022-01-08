bin := main
source := main.c
obj := main.o
header := func.h
dinlib := libfunc.so
CC := gcc

.PHONY: debug clean start
.INTERMEDIATE: $(dinlib) %.o
#CFlags/LDFLAGS

$(bin): $(source) $(header) $(dinlib)
		$(CC) $(source) -o $@ -L. -lfunc
$(dinlib): func*.o
		$(CC) -shared $^ -o $@
%.o: func*.c
		$(CC) $^ -fPIC -c


debug: $(source) $(header)
		$(CC) $(source) -o $@ -L. -lfunc -g
clean:
		rm -rf $(bin) *.o
start:
		LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./$(bin)
