mlog2bson:	mlog2bson.c datemanip.c
	gcc -c datemanip.c
	gcc -O2 -c mlog2bson.c
	gcc -l bson-1.0 -o mlog2bson mlog2bson.o datemanip.o

clean:
	rm -f mlog2bson *.o
