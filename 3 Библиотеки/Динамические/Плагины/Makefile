bin := main
source := main.c
obj := main.o
header := func.h
dinlib := libfunc_del.so libfunc_minus.so libfunc_plus.so libfunc_um.so

CC := gcc

.PHONY: debug clean start
#.INTERMEDIATE: $(dinlib) %.o
#CFlags/LDFLAGS

$(bin): $(source) $(header) $(dinlib) ./plugin/func*.c ./plugin/func*.o
		$(CC) $(source) -o $@ -ldl -Wl,-rpath,.

/plugin/libfunc_del.so: /plugin/func_del.o
		$(CC) -shared $^ -o $@
/plugin/libfunc_minus.so: /plugin/func_minus.o
		$(CC) -shared $^ -o $@
/plugin/libfunc_plus.so: /plugin/func_plus.o
		$(CC) -shared $^ -o $@
/plugin/libfunc_um.so: /plugin/func_um.o
		$(CC) -shared $^ -o $@

%.o: ./plugin/func*.c
		$(CC) $^ -fPIC -c


debug: $(source) $(header)
		$(CC) $(source) -o $@ -ldl -g
clean:
		rm -rf *.o
start:
		LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./$(bin)
