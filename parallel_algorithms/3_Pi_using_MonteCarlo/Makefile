CCOMP  = mpicxx
CFLAGS = -o

run: monte_carlo_for_pi_solution.o
	$(CCOMP) $(CFLAGS) calc_pi monte_carlo_for_pi_solution.cpp

.PHONY: clean cleanall

clean:
	rm -f run *.o

cleanall:
	rm -f run *.o calc_pi*
