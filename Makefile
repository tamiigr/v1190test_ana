TARGET = offline

ROOTCFLAGS  = $(shell $(CFG_ROOT_BIN)root-config --cflags)
ROOTLIBS    = $(shell $(CFG_ROOT_BIN)root-config --libs)
CFLAGS = -I$(TARTSYS)/include -L$(TARTSYS)/lib -lanacore

GXX = g++

all: $(TARGET)

offline: offline.o
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ $^

.cpp.o:
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -c $<


clean:
	rm -f $(TARGET) *.o
