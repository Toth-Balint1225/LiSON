CFLAGS:=-Wall -Werror -std=c++17 -g

all: test.exe
run: test.exe
	.\test.exe

test.exe:  main.o lison.o serializer.o parser.o tokenizer.o object.o
	g++ $(CFLAGS) $^ -o $@

%.o: %.cpp LiSON_base.h
	g++ $(CFLAGS) -c $<

clean: 
	del *.o
	del *.exe
	del copy.lison

