specs:		main.o  spec_params.o spec_build.o spec_vars.o spec_convert.o
	g++ -g main.o spec_params.o spec_build.o spec_vars.o spec_convert.o -o specs
	
main.o:			main.cc specs.h
	g++ -g -c -o main.o main.cc
	
spec_params.o:	spec_params.cc specs.h conversion.h
	g++ -g -c -o spec_params.o spec_params.cc

spec_build.o:	spec_build.cc specs.h
	g++ -g -c -o spec_build.o spec_build.cc

spec_vars.o:	spec_vars.cc specs.h
	g++ -g -c -o spec_vars.o spec_vars.cc

spec_convert.o:	spec_convert.cc specs.h conversion.h
	g++ -g -c -o spec_convert.o spec_convert.cc

clean:
	rm specs *.o
