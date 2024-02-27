insert-kv:	insert-kv.o
	gcc -o insert-kv insert-kv.o -lhiredis

insert-kv.o:	insert-kv.c
	gcc -Wall -c insert-kv.c  -I /usr/local/include/hiredis


clean:
	-rm insert-kv
	-rm *.o
