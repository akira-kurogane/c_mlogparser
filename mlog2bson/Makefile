mlog2bson:	mlog2bson.c datemanip.c
	gcc -c datemanip.c
	gcc -O2 -c mlog2bson.c
	gcc -o mlog2bson mlog2bson.o datemanip.o

clean:
	rm -f mlog2bson *.o
