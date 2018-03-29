pidemo.exe: PiSpiBus.o pidemo.o
	c++ -Wall -W -Wextra -Werror -o $@ -std=c++17 -O3 $^

%.o: %.cpp PiSpiBus.h
	c++ -Wall -W -Wextra -Werror -c -o $@ -std=c++17 -O3 $<
