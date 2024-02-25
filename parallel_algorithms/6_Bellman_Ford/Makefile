solution: bf.o 
	gcc -o bellmanford bf.o -lm -fopenmp -O3

%.o: %.c
	gcc -c -o $@ $<

clean:
	rm -f *.o bellmanford