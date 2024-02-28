CCOMP  = mpicxx
CFLAGS = -o

run: prefix_sum_solution.o
	$(CCOMP) $(CFLAGS) prefix prefix_sum_solution.cpp

.PHONY: clean cleanall

clean:
	rm -f run *.o

cleanall:
	rm -f run *.o hello*
