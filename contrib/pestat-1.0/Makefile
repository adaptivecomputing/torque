
# Configure PBSHOME so that it points to your PBS source directory
PBSHOME = $$HOME/src/PBS/OpenPBS/OpenPBS_2_3_2/src

# Linux (with OpenPBS RPM installed)
CFLAGS = -g -I$(PBSHOME)/include
LDFLAGS = -g
LIBS = -L/usr/pbs/lib -lnet -lpbs -lcmds 

# Compaq Tru64 UNIX 4.0F
# CFLAGS = -g -DTRU64 -I/usr/local/include -I$(PBSHOME)/include
# LDFLAGS = -g -L/usr/local/lib
# LIBS = /usr/local/lib/libnet.a -lpbs -lcmds 

pestat: pestat.o
	$(CC) -o $@ pestat.o $(LDFLAGS) $(LIBS)

clean:
	rm -f pestat *.o core
