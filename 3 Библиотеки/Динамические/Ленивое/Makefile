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
		$(CC) $(source) -o $@ -ldl
$(dinlib): func*.o
		$(CC) -shared $^ -o $@
%.o: func*.c
		$(CC) $^ -fPIC -c


debug: $(source) $(header)
		$(CC) $(source) -o $@ -ldl -g
clean:
		rm -rf *.o
start:
		LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./$(bin)
