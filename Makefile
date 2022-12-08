COMMAND = q++ -g -Vgcc_ntox86_64 
COMMAND_BIN = $(COMMAND) *.o

all: timing-cycle.o pacemaker.o heart.o
	rm *.o

clean:
	rm *.o heart pacemaker

#classes
timing-cycle.o:
	$(COMMAND) -c timing-cycle.cpp
	
pacemaker.o: timing-cycle.o
	$(COMMAND) timing-cycle.o pacemaker.cpp -o pacemaker
	
heart.o:
	$(COMMAND) heart.cpp -o heart