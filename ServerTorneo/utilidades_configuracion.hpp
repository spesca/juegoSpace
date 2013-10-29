#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>


using namespace std;

/**
 * Estructura que almacenara la configuracion del
 * servidor, establecida en un archivo de configuracion 'config_server.ini'.
 */
typedef struct s_config_server
{
	unsigned short int 	puerto_escucha,
				cantidad_vidas;
} t_config_server;


char* obtener_datos_configuracion(char *cad);

short int validar_configuracion_server(t_config_server *configuracion);

void leer_configuracion_server(t_config_server *configuracion);


