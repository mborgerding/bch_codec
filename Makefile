CFLAGS += -g -Wall -O3
CXXFLAGS += -g -Wall -O3

bch_codec.o: bch_codec.c bch_codec.h
	$(CC) -c -o $@ $(CFLAGS) $<

bch_test: bch_test.o bch_codec.o
	$(CXX) -o $@ $(CXXFLAGS) $^

test: bch_test
	./bch_test

clean:
	@rm -f bch_test *.o
