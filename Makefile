# Esto en verdad no es un Makefile, simplemente compila
# el módulo de Arduino y lo carga en el procesador AVR

all:
	arduino --upload juego.ino --port /dev/ttyUSB0 --board arduino:avr:uno
