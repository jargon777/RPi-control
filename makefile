# RPi-CONTROL MAKEFILE
# Updated: September 17 2014
INCL = -Ih -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -Ilib/openvg -I/usr/include
ODIR = obj
HDIR = h
LDIR = lib
SDIR = src
LIBS = -lGLESv2 -ljpeg -lm

EXEC = main_console
OBJS = GPIO.o main_console.o RPi_general.o RPi_generror.o RPi_USBGPS.o SPI_control.o
OBJP = $(patsubst %,$(ODIR)/%,$(OBJS))

RM = rm
CC = gcc
CFLAGS = -g -Wall

$(ODIR)/%.o : $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCL)

$(EXEC): $(OBJP)
	$(CC) -o $@ $^ $(CFLAGS) $(INCL) $(LIBS)

$(shell mkdir -p $(ODIR))

.PHONY: clean

help:
	@echo Options:
	@echo -e "\tclean"
	@echo -e "\tclnmk - cleans and runs make"

clean:
	$(RM) -fdr $(ODIR) 
	$(RM) -f $(EXEC)
	@echo Cleaned results of make.
	
cleanmake:
	$(RM) -fdr $(ODIR) 
	$(RM) -f $(EXEC)
	make