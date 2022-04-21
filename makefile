cc = g++  # use g++ compiler

flags = -std=c++17  # compile with C++ 17 standard
flags += -Wall  # compile with all warnings
flags += -g  # produce debugging information
flags += -I .. # add pwd directory to src include path
flags += -pthread # support for POSIX threads

link = $(cc) $(flags) -o
compile = $(cc) $(flags) -c


# text-server.cc
#
text-server: text-server.cc shared.h -lrt 
	$(link) $@ $^

text-client: text-client.cc -lrt
	$(link) $@ $^

# Clean up
clean:
	$(RM) text-server text-client