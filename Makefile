CFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter
CXXFLAGS=$(CFLAGS)
OBJECTS=demo.o imageDemo.o
BINARIES=demo imageDemo

RGB_LIB_DISTRIBUTION=./matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

#$(CXX) means g++
all : $(BINARIES) #If nothing specified then build all the binaries...
							#in this case we just have one 

#Always check and compile the rgbmatrix library if needed 
$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR)
	#$(MAKE) -C examples-api-use

#Create test executable from the object file
demo : demo.o

imageDemo : imageDemo.o


#Build the final binaries that all have the same name as the object file
% : %.o $(RGB_LIBRARY)
	$(CXX) $< -o $@ $(LDFLAGS)

#Build the .o file out of the original .cpp file
%.o : %.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) -I$(RGB_INCDIR) $(CFLAGS) -c -o $@ $<

clean:
	#$(MAKE) -C matrix/lib clean
	#$(MAKE) -C matrix/utils clean
	rm -f $(OBJECTS) $(BINARIES)



FORCE:
.PHONY: FORCE
