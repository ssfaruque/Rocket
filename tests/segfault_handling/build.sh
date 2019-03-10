cd ../..
make clean
make
cd tests/segfault_handling
make clean
make

while true
do
	./server
	sleep 5
done

