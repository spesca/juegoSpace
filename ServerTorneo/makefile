servidor:	servidorTorneo.o library.o utilidades_configuracion.o claseJugador.o utilidades_configuracion.hpp
			g++ -o servidor servidorTorneo.cpp library.o utilidades_configuracion.o claseJugador.o -lpthread -lrt -lSDL

servidorTorneo.o:	servidorTorneo.cpp library.hpp utilidades_configuracion.hpp claseJugador.hpp
			g++ -c servidorTorneo.cpp

utilidades_configuracion.o:	utilidades_configuracion.cpp utilidades_configuracion.hpp
							g++ -c utilidades_configuracion.cpp

library.o:	library.cpp library.hpp
				g++ -c library.cpp -lpthread

claseJugador.o:	claseJugador.cpp claseJugador.hpp
		g++ -c claseJugador.cpp 
		
all:	servidor

clean:
		rm -f *.o
		rm -f servidor
