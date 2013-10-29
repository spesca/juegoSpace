#include "utilidades_configuracion.hpp"

/**
 * Extrae de una cadena que contiene un caracter '=',
 * la subcadena contigua al '=', que es el dato deseado.
 * @param cad
 * @return cadena con dato deseado
 */
char* obtener_datos_configuracion(char *cad)
{
	char *aux=cad,*aux2=cad;

	while(*aux!='=')
		aux++;
	aux++;

    while(*aux)
	{
		*aux2=*aux;
		aux2++;
		aux++;
	}

	*aux2='\0';

	return cad;
}

/**
 * Se encarga de validar la estructura que contiene la
 * configuracion, para que no haya datos invalidos en la misma.
 * @param configuracion
 * @return 0, si hubo error. 1, si todo es correcto
 *
 */
short int validar_configuracion_server(t_config_server *configuracion)
{
	if(configuracion->puerto_escucha < 1000 || configuracion->puerto_escucha > 65000)
		return 0;
	//La cantidad de vidas debe ser entre 1 a 5
	if(configuracion->cantidad_vidas < 0 || configuracion->cantidad_vidas > 5)
		return 0;

	return 1;
}

/**
 * Lee un archivo de configuracion 'config_server.ini', para
 * extraer los datos de configuracion que el programa
 * necesita para ejecutarse correctamente.
 * Las dos primeras lineas contendran la informacion
 * del puerto de escucha y la cantidad de vidas de los jugadores
 * En caso de error, se finaliza la ejecucion del programa.
 * @param configuracion
 */
void leer_configuracion_server(t_config_server *configuracion)
{
	char aux[250];

	FILE *pf=fopen("config_server.ini","r");

	if(!pf)
	{
		printf("Error al abrir el archivo\n");
		exit(1);
	}

	fgets(aux,sizeof(aux),pf);
	configuracion->puerto_escucha=atoi(obtener_datos_configuracion(aux));

	fgets(aux,sizeof(aux),pf);
	configuracion->cantidad_vidas=atoi(obtener_datos_configuracion(aux));


	fclose(pf);

	if(!validar_configuracion_server(configuracion))
	{
		printf("Error en el archivo de configuracion, no se pudo continuar con la ejecucion.");
		exit(1);
	}
}


