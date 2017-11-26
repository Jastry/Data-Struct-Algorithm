test : UnionFindSet.o test.o
	g++ -o $@ $^
%.o : %.cpp
	g++ -c UnionFindSet.cpp test.cpp
.PHONY:clean
clean:
	rm -f *.o test
