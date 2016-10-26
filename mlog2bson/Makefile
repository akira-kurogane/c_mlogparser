mlog2bson:	mlog2bson.c datemanip.c lineparsers.c
	gcc -c lineparsers.c
	gcc -c datemanip.c
	gcc -O2 -c mlog2bson.c
	gcc -l bson-1.0 -o mlog2bson mlog2bson.o datemanip.o lineparsers.o

clean:
	rm -f mlog2bson *.o
