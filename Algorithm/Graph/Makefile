test:test.o UnionFindSet.o
	g++ -o $@ $^

%.o : %.cpp
	g++ -c ../Union/UnionFindSet.cpp test.cpp

.PHONY:clean
clean:
	rm -f *.o test
