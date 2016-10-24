mlogfilter:	mlogfilter.c
	gcc -c mlogfilter_opts.c
	gcc -O2 -c mlogfilter.c
	gcc -o mlogfilter mlogfilter.o mlogfilter_opts.o

clean:
	rm mlogfilter
#	rm mlogparser *.o
