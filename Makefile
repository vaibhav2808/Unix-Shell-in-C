all: date rm cat mkdir task2 ls
date:date.c
	gcc date.c -o date

rm:rm.c
	gcc rm.c -o rm

cat:cat.c
	gcc cat.c -o cat

mkdir:mkdir.c
	gcc mkdir.c -o mkdir

ls:ls.c
	gcc ls.c -o ls

task2: task2.c
	gcc task2.c -o task2

clean:
	rm -f date rm cat ls mkdir a.out task2