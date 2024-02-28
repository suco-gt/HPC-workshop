CCOMP  = mpicxx
CFLAGS = -o

run: mpi_hello_solution.o
	$(CCOMP) $(CFLAGS) hello mpi_hello_solution.cpp

.PHONY: clean cleanall

clean:
	rm -f run *.o

cleanall:
	rm -f run *.o hello*
