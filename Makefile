.PHONY: all
all: coder

coder: codec.h ThreadPool.cpp
	g++ ThreadPool.cpp -ldl -pthread ./libCodec.so -o coder

.PHONY: clean
clean:
	-rm coder 2>/dev/null
