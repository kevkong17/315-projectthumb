all:
	g++ -g main.cpp decode.cpp thumbsim_driver.cpp parse.cpp execute.cpp -o thumbsim

run:
	./thumbsim -i -d -f inputs/fib.sim

clean:
	rm -rf ./*.o ./thumbsim

stats:
	 ./thumbsim -i -d -s -f inputs/fib.sim


cache:
	 ./thumbsim -c 256 -i -d -s -f inputs/fib.sim

