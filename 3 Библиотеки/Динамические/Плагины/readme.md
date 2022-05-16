make не работает т.к. там требуется раздельная компиляция библиотек.

gcc plugin/func_del.c plugin/func_minus.c plugin/func_um.c plugin/func_plus.c -fPIC -c
gcc -shared plugin/func_del.o -o libfunc_del.so
gcc -shared plugin/func_minus.o -o libfunc_minus.so
gcc -shared plugin/func_um.o -o libfunc_um.so
gcc -shared plugin/func_plus.o -o libfunc_plus.so
gcc main.c -o main -ldl -Wl,-rpath,.