HWDIR = ../US_FUNCTIONS/
HWOBJS = $(HWDIR)GY521.o $(HWDIR)HMC5883L.o $(HWDIR)GY80.o
INCLUDEFLAG = -I../US_FUNCTIONS

hk : hk.c $(HWOBJS)
	cc $(INCLUDEFLAG) hk.c -o hk $(HWOBJS)

#$(HWDIR)GY80.o : $(HWDIR)GY80.c
#	cd $(HWDIR) && cc -c GY80.c
#$(HWDIR)GY521.o : $(HWDIR)GY521.c
#	cd $(HWDIR) && cc -c GY521.c
#$(HWDIR)HMC5883L.o : $(HWDIR)HMC5883L.c
#	cd $(HWDIR) && cc -c HMC5883L.c

.PHONY: clean
clean:
	rm $(HWOBJS) hk hk.o fullhk.txt