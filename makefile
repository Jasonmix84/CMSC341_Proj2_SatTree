CXX = g++
CXXFLAGS = -Wall -g

mytest: satnet.o mytest.cpp
	$(CXX) $(CXXFLAGS) satnet.o mytest.cpp -g mytest

driver: satnet.o driver.cpp
	$(CXX) $(CXXFLAGS) satnet.o driver.cpp -g driver

satnet.o: satnet.h satnet.cpp
	$(CXX) $(CXXFLAGS) -c satnet.cpp

clean:
	rm *.o*
	rm *~ 

runDriver:
	./driver

runMytest:
	./mytest

val:
	valgrind driver

val mytest:
	valgrind mytest

debugDriver:
	gdb gdbarg1 --args ./driver

debugMytest:
	gdb gdbarg1 --args ./mytest

student:
	cp satnet.h satnet.cpp mytest.cpp ~/cs341proj/proj2/
