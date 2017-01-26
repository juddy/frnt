# Makefile for the FRNT Framework and its modules.
#

VERBOSE=1


frnt: frnt.o menu.so clock.so calculator.so notepad.so help.so run_program.so log_viewer.so
	gcc -ldl -lncurses -o frnt frnt.o

frnt.o: frnt.c frnt.h
	gcc -c -Wall -o frnt.o frnt.c

menu.so: menu.o
	gcc -shared -Wl,-soname,menu.so -o menu.so menu.o -lc

menu.o: menu.c frnt.h
	gcc -fPIC -c -Wall -o menu.o menu.c

clock.so: clock.o
	gcc -shared -Wl,-soname,clock.so -o clock.so clock.o -lc

clock.o: clock.c frnt.h
	gcc -fPIC -c -Wall -o clock.o clock.c

calculator.so: calculator.o
	gcc -shared -Wl,-soname,calculator.so -o calculator.so calculator.o -lc

calculator.o: calculator.c frnt.h
	gcc -fPIC -c -Wall -o calculator.o calculator.c

notepad.so: notepad.o
	gcc -shared -Wl,-soname,notepad.so -o notepad.so notepad.o -lc

notepad.o: notepad.c frnt.h
	gcc -fPIC -c -Wall -o notepad.o notepad.c

help.so: help.o
	gcc -shared -Wl,-soname,help.so -o help.so help.o -lc

help.o: help.c frnt.h
	gcc -fPIC -c -Wall -o help.o help.c

run_program.so: run_program.o
	gcc -shared -Wl,-soname,run_program.so -o run_program.so \
          run_program.o -lc

run_program.o: run_program.c frnt.h
	gcc -fPIC -c -Wall -o run_program.o run_program.c

log_viewer.so: log_viewer.o
	gcc -shared -Wl,-soname,log_viewer.so -o log_viewer.so log_viewer.o -lc

log_viewer.o: log_viewer.c frnt.h
	gcc -fPIC -c -Wall -o log_viewer.o log_viewer.c

.PHONY: clean cleanall
clean:
	rm *.o

clean-all:
	rm frnt *.o *.so

