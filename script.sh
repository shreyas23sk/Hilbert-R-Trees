gcc -c linked_list.c
gcc -c stack_2.c
gcc -c insertion.c
gcc -c hilbert.c
gcc -o hrtree_exe hilbert.o insertion.o stack_2.o linked_list.o
./hrtree_exe