all: skype_rc4.so

skype_rc4.o: skype_rc4.c skype_rc4.h
	gcc -fPIC -g -c -Wall skype_rc4.c

skype_rc4.so: skype_rc4.o
	gcc -shared -Wl,-soname,skype_rc4.so.1 -o skype_rc4.so \
	skype_rc4.o -lc

