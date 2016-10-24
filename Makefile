mlogfilter:	mlogfilter.c mlogfilter_opts.c ffc.c
	gcc -c mlogfilter_opts.c
	gcc -c ffc.c
	gcc -O2 -c mlogfilter.c
	gcc -o mlogfilter mlogfilter.o mlogfilter_opts.o ffc.o

clean:
	rm -f mlogfilter *.o
