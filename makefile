# RPi-CONTROL MAKEFILE
# Updated: September 17 2014
INCL = -Ih -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -Ilib/openvg -I/usr/include
ODIR = obj
LDIR = lib
SDIR = src
LIBS = -lGLESv2 -ljpeg
EXEC = main_console

CC = gcc
CFLAGS = -g -Wall

$(EXEC): $(ODIR)/m_$(EXEC).o $(ODIR)/f_$(EXEC).o
	$(CC) -o $@ $^

$(ODIR)/%.o : $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCL) -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*$(EXEC) core $(ODIR)/*~