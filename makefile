# RPi-CONTROL MAKEFILE
# Updated: September 17 2014
INCL = -Ih -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -Ilib/openvg -I/usr/include
ODIR = obj
HDIR = h
LDIR = lib
SDIR = src
LIBS = -lGLESv2 -ljpeg

EXEC = main_console
OBJS = GPIO.o main_console.o RPi_general.o RPi_generror.o RPi_USBGPS.o SPI_control.o
OBJP = $(patsubst %,$(ODIR)/%,$(OBJS))

CC = gcc
CFLAGS = -g -Wall

$(ODIR)/%.o : $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCL)

$(EXEC): $(OBJP)
	$(CC) -o $@ $^ $(CFLAGS) $(INCL) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*$(EXEC) core $(ODIR)/*~