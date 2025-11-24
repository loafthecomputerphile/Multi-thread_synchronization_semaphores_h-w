all:
	g++ -g main.cpp -o main -lpthread -lm 
	g++ -g cse4001_sync1.cpp -o cse4001_sync1 -lpthread -lm
	g++ -g cse4001_sync2.cpp -o cse4001_sync2 -lpthread -lm
	g++ -g cse4001_sync3.cpp -o cse4001_sync3 -lpthread -lm
	g++ -g cse4001_sync4.cpp -o cse4001_sync4 -lpthread -lm
clean:
	rm main
	rm cse4001_sync1
	rm cse4001_sync2
	rm cse4001_sync3
	rm cse4001_sunc4

