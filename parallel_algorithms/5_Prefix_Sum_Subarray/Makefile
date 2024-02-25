CCOMP  = mpicxx
CFLAGS = -o

run: prefix_sum.o
	$(CCOMP) $(CFLAGS) prefix prefix_sum.cpp

.PHONY: clean cleanall

clean:
	rm -f run *.o

cleanall:
	rm -f run *.o hello*
