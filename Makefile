CFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter
CXXFLAGS=$(CFLAGS)
OBJECTS=demo.o imageDemo.o Item.o Image.o StockManager.o Scroller.o
BINARIES=demo imageDemo Scroller

RGB_LIB_DISTRIBUTION=./matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm 

#$(CXX) means g++
all : $(BINARIES) #If nothing specified then build all the binaries...
							#in this case we just have one 

#Always check and compile the rgbmatrix library if needed 
$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR)
	#$(MAKE) -C examples-api-use

#Create test executable from the object file...
#Why does the demo work if I include Item.0 in the actual line and not at the top...
demo : demo.o Item.o StockManager.o Image.o $(RGB_LIBRARY) #TODO: Look for a better way to link in Item.0
	$(CXX) $< -o $@ Item.o StockManager.o Image.o $(LDFLAGS) 

imageDemo : imageDemo.o Image.o $(RGB_LIBRARY)
	$(CXX) $< -o $@ Image.o $(LDFLAGS) 

Scroller : Scroller.o Image.o Item.o StockManager.o $(RGB_LIBRARY)
	$(CXX) $< -o $@ Image.o Item.o StockManager.o $(LDFLAGS) 

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
