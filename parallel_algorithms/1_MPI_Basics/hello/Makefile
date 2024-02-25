CCOMP  = mpicxx
CFLAGS = -o

run: mpi_hello.o
	$(CCOMP) $(CFLAGS) hello mpi_hello.cpp

.PHONY: clean cleanall

clean:
	rm -f run *.o

cleanall:
	rm -f run *.o hello*
