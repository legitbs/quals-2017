all: reeses reeses_p1

reeses:
	rm -rf reeses *.o *.d
	$(MAKE) -f ./Makefile.nopatch

reeses_p1:
	rm -rf reeses_p1 *.o *.d
	$(MAKE) -f ./Makefile.p1

clean:
	rm -f reeses reeses_p1 *.o *.d
