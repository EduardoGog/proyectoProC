#include <stdio.h>
#include <atmi.h>
#include <fml32.h>	
#include <string.h>
#include "biblioProyecto.fml.h"

typedef struct pagos{
   int numpago;
   char nomdes[51];
   char nomrem[151];
   char concepto[251];
}pagos;

int main(int argc, char **argv){
   int ivL_resLlamd;
   long lvL_tamLongt;
   float fvL_recValor;
   FBFR32 *fbfr;
   FBFR32 *recv;
   FLDLEN32 flen;
   char msgbuf[64];
   int zaL_nomPago;
   char zaL_concepto[251];
   pagos *spL_cliPagos;
   pagos *spL_concept;
   int ivL_numOcurr;
   int ivl_iterador;

   /* Inicializamos una referencia a tabla */
   pagos pgs;

   int opcion;
   do {
       printf("\n******** Menú ********\n");
       printf("1. Insertar pago\n");
       printf("2. Buscar pago\n");
       printf("3. Mostrar pagos\n");
       printf("4. Actualizar pago\n");
       printf("5. Eliminar pago\n");
       printf("0. Salir\n");
       printf("Seleccione una opción: ");
       scanf("%d", &opcion);

       switch(opcion)
       {
          case 1:
   printf("\n*************************************************\n");
   printf("Numero de pago : "); scanf("%d", &pgs.numpago);  
   printf("Nombre destinatario : "); scanf("%s", pgs.nomdes);
   printf("Nombre remitente : "); scanf("%s", pgs.nomrem);
   printf("Concepto de pago : "); scanf("%s", pgs.concepto);
   printf("\n*************************************************\n");

  /* Conectamos con la aplicacion Tuxedo Server */
   printf ("Conectamos con la aplicacion\n") ;
   if (tpinit((TPINIT *) NULL) == -1){
      printf("Error en la conexion, tperrno = %d \n", tperrno);
      return (1) ;
   }
   // Reservamos espacio para el buffer FML
   printf ("Reservamos Espacio para los buffers FML\n");
   if ((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
      printf("Error Reservando espacio para Buffer fbfr\n");
      tpterm();
      return(1);
   }
   if ((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
      printf("Error Reservando espacio para Buffer recv\n");
      tpterm();
      return(1);
   }
   /* Manejo del Buffer FML */
   printf ("\nInsertamos datos en buffer FML.");
   if(Fadd32 (fbfr, NUMPAGO, (char *)&pgs.numpago, 0) < 0) {
      printf ("\n\tError insertando campo FML (NUMPAGO)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }
   if(Fadd32 (fbfr, NOMDES, (char *)&pgs.nomdes, 0) < 0) {
      printf ("\n\tError insertando campo FML (NOMDES)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }
   if(Fadd32 (fbfr, NOMREM, (char *)&pgs.nomrem, 0) < 0) {
      printf ("\n\tError insertando campo FML (NOMREM)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }
   if(Fadd32 (fbfr, CONCEPTO, (char *)&pgs.concepto, 0) < 0) {
      printf ("\n\tError insertando campo FML (CONCEPTO)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }

   //Invocamos el servicio
   printf("Llamada al servicio 'INSERT_PAGO_FML32'\n");
   if (tpcall("INSERT_PAGO_FML32", (char *)fbfr, 0, (char **)&recv, &lvL_tamLongt, 0L) == -1){
       printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
       tpfree((char *)fbfr);
       tpfree((char *)recv);
       tpterm();
       return (1);
   }
   flen = sizeof(msgbuf);
   Fget32(recv, OUTPUT, 0, (char *)msgbuf, &flen);
   printf("Respuesta del servidor: %s\n", msgbuf);
   
   // Liberamos el buffer y desconectamos de la aplicacion
   printf("Liberamos Buffer y desconectamos de la aplicacion\n");
   tpfree((char *)fbfr);
   tpfree((char *)recv);
   tpterm();
          break;

	  case 2:
   printf("*************************************************\n");
   printf("Numero de pago : "); scanf("%d", &zaL_nomPago);
   printf("*************************************************\n");

   /* Conectamos con la aplicacion Tuxedo Server */
   printf ("Conectamos con la aplicacion\n");
   if (tpinit((TPINIT *) NULL) == -1){
       printf("Error en la conexion, tperrno = %d \n", tperrno);
       return (1);
   }

   // Reservamos espacio para el buffer FML
   printf ("Reservamos Espacio para los buffers FML\n");
   if ((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
       printf("Error Reservando espacio para Buffer fbfr\n");
       tpterm();
       return(1);
   }
   if ((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
       printf("ErrorReservando espacio para Buffer recv\n");
       tpterm();
       return(1);
   }
   /* Manejo del Buffer FML */
   if(Fadd32 (fbfr, NUMPAGO, (char *)&zaL_nomPago, 0) < 0) {
      printf ("\n\tError insertando campo FML (NUMPAGO)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }
   // Invocamos el servicio 
   printf("Llamada al servicio 'SELECT_PAGO_FML32'\n");
   if(tpcall("SELECT_PAGO_FML32", (char *)fbfr, 0, (char **)&recv, &lvL_tamLongt, 0L)==-1){
      printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1);
   }
   if((ivL_numOcurr = Foccur32(recv, NUMPAGO)) < 0) {
      printf("Error en Foccur32\n");
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1); 
   }
   printf("Numero de ocurrencias: %d\n", ivL_numOcurr);
   spL_cliPagos = (pagos *) malloc(sizeof(pagos) * ivL_numOcurr);

  flen = sizeof(msgbuf);
  printf("Respuesta del servidor para numero de pago: %d\n", zaL_nomPago);
  printf("***************************************************\n");
  for(ivl_iterador = 0;ivl_iterador < ivL_numOcurr; ivl_iterador++) {
      printf("Numero de registro: %d\n", (ivl_iterador + 1));
      Fget32(recv, NOMREM, ivl_iterador, (char *)&spL_cliPagos[ivl_iterador].nomrem,0);
      printf("Numero de pago : %s\n", spL_cliPagos[ivl_iterador].nomrem);
      Fget32(recv, NOMDES, ivl_iterador, (char *)spL_cliPagos[ivl_iterador].nomdes,0);
      printf("Nombre destinatario : %s\n", spL_cliPagos[ivl_iterador].nomdes);
      Fget32(recv, CONCEPTO, ivl_iterador, (char *)spL_cliPagos[ivl_iterador].concepto,0);
      printf("Estado de cuenta : %s\n", spL_cliPagos[ivl_iterador].concepto);
      }
   // Liberamos el buffer y desconectamos de la aplicacion
   printf("Liberamos Buffer y desconectamos de la aplicacion\n");
   tpfree((char *)fbfr);
   tpfree((char *)recv);
   tpterm();
 	  break;

	  case 3:
   printf("*************************************************\n");
   printf("Concepto : "); scanf("%s", zaL_concepto);
   printf("*************************************************\n");

   /* Conectamos con la aplicacion Tuxedo Server */
   printf ("Conectamos con la aplicacion\n");
   if (tpinit((TPINIT *) NULL) == -1){
       printf("Error en la conexion, tperrno = %d \n", tperrno);
       return (1);
   }

   // Reservamos espacio para el buffer FML
   printf ("Reservamos Espacio para los buffers FML\n");
   if ((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
       printf("Error Reservando espacio para Buffer fbfr\n");
       tpterm();
       return(1);
   }
   if ((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
       printf("ErrorReservando espacio para Buffer recv\n");
       tpterm();
       return(1);
   }
   /* Manejo del Buffer FML */
   if(Fadd32 (fbfr, CONCEPTO, zaL_concepto, 0) < 0) {
      printf ("\n\tError insertando campo FML (CONCEPTO)");
      tpfree((char*)fbfr);
      tpterm();
      return (0);
   }
   // Invocamos el servicio 
   printf("Llamada al servicio 'SELECTALL_PAGO_FML32'\n");
   if(tpcall("SELECTALL_PAGO_FML32", (char *)fbfr, 0, (char **)&recv, &lvL_tamLongt, 0L)==-1){
      printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1);
   }
   if((ivL_numOcurr = Foccur32(recv, NUMPAGO)) < 0) {
      printf("Error en Foccur32\n");
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1); 
   }
   printf("Numero de ocurrencias: %d\n", ivL_numOcurr);
   spL_concept = (pagos *) malloc(sizeof(pagos) * ivL_numOcurr);

  flen = sizeof(msgbuf);
  printf("Respuesta del servidor para concepto: %s\n", zaL_concepto);
  printf("***************************************************\n");
  for(ivl_iterador = 0;ivl_iterador < ivL_numOcurr; ivl_iterador++) {
      printf("Numero de registro: %d\n", (ivl_iterador + 1));
      Fget32(recv, NUMPAGO, ivl_iterador, (char *)&spL_concept[ivl_iterador].numpago,0);
      printf("Numero de pago : %d\n", spL_concept[ivl_iterador].numpago);
      Fget32(recv, NOMDES, ivl_iterador, (char *)spL_concept[ivl_iterador].nomdes,0);
      printf("Nombre destinatario : %s\n", spL_concept[ivl_iterador].nomdes);
      Fget32(recv, NOMREM, ivl_iterador, (char *)spL_concept[ivl_iterador].nomrem,0);
      printf("Nombre remitente : %s\n", spL_concept[ivl_iterador].nomrem);
      }
   // Liberamos el buffer y desconectamos de la aplicacion
   printf("Liberamos Buffer y desconectamos de la aplicacion\n");
   tpfree((char *)fbfr);
   tpfree((char *)recv);
   tpterm();
          break;
   	  case 4:
   printf("\n*****************\n");
   printf("Numero de pago a actualizar: ");
   scanf("%d", &pgs.numpago); 
   printf("Nuevo nombre destinatario: ");
   scanf("%s", pgs.nomdes);
   printf("Nuevo nombre remitente: ");
   scanf("%s", pgs.nomrem);
   printf("Nuevo concepto: ");
   scanf("%s", pgs.concepto);
   printf("\n*****************\n");

   /* Conectamos con la aplicacion Tuxedo Server */
    printf("Conectamos con la aplicacion\n");
    if(tpinit((TPINIT *) NULL) == -1){
        printf("Error en la conexion, tperrno = %d \n", tperrno);
        return (1);
    }
   
    // Reservamos espacio para el buffer FML
    printf("Reservamos Espacio para los buffers FML\n");
    if((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error Reservando espacio para Buffer fbfr\n");
        tpterm();
        return (1);
    }

    if((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error Reservando espacio para Buffer recv\n");
        tpterm();
        return (1);
    }
  
    /* Manejo del Buffer FML */
    printf("\nInsertamos datos en buffer FML.\n");
    if(Fchg32(fbfr, NUMPAGO, 0, (char *) &pgs.numpago, 0) < 0){
        printf("\n\tError actualizando campo FML (NUMPAGO)\n");
        tpfree((char *) fbfr);
        tpterm();
        return (0);
    }

    if(Fchg32(fbfr, NOMDES, 0, pgs.nomdes, 0) < 0){
        printf("\n\tError actualizando campo FML (NOMDES)\n");
        tpfree((char *) fbfr);
        tpterm();
        return (0);
    }

    if(Fchg32(fbfr, NOMREM, 0, pgs.nomrem, 0) < 0){
        printf("\n\tError actualizando campo FML (NOMREM)\n");
        tpfree((char *) fbfr);
        tpterm();
        return (0);
    }

    if(Fchg32(fbfr, CONCEPTO, 0, pgs.concepto, 0) < 0){
        printf("\n\tError actualizando campo FML (CONCEPTO)\n");
        tpfree((char *) fbfr);
        tpterm();
        return (0);
    }
    
    // Invocamos el servicio 
   printf("Llamada al servicio 'UPDATE_PAGO_FML32'\n");
   if(tpcall("UPDATE_PAGO_FML32", (char *)fbfr, 0, (char **)&recv, &lvL_tamLongt, 0L)==-1){
      printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1);
   }

   flen = sizeof(msgbuf);
   Fget32(recv, OUTPUT, 0, (char *) msgbuf, &flen);
   printf("Respuesta del servidor: %s\n", msgbuf);

   // Liberamos el buffer y desconectamos de la aplicacion
   printf("Liberamos Buffer y desconectamos de la aplicacion\n");
   tpfree((char *) fbfr);
   tpfree((char *) recv);
   tpterm();
  	  break;
 	  case 5:
   printf("\n*****************\n");
   printf("Número de pago a borrar: ");
   scanf("%d", &pgs.numpago);
   printf("\n*****************\n");

   /* Conectamos con la aplicacion Tuxedo Server */
    printf("Conectamos con la aplicacion\n");
    if(tpinit((TPINIT *) NULL) == -1){
        printf("Error en la conexion, tperrno = %d \n", tperrno);
        return (1);
    }
   
    // Reservamos espacio para el buffer FML
    printf("Reservamos Espacio para los buffers FML\n");
    if((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error Reservando espacio para Buffer fbfr\n");
        tpterm();
        return (1);
    }

    if((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error Reservando espacio para Buffer recv\n");
        tpterm();
        return (1);
    }
  
    /* Manejo del Buffer FML */
    printf("\nInsertamos datos en buffer FML.\n");
    if(Fchg32(fbfr, NUMPAGO, 0, (char *) &pgs.numpago, 0) < 0){
        printf("\n\tError actualizando campo FML (NUMERO)\n");
        tpfree((char *) fbfr);
        tpterm();
        return (0);
    }

    // Invocamos el servicio 
   printf("Llamada al servicio 'DELETE_PAGO_FML32'\n");
   if(tpcall("DELETE_PAGO_FML32", (char *)fbfr, 0, (char **)&recv, &lvL_tamLongt, 0L)==-1){
      printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
      tpfree((char *)fbfr);
      tpfree((char *)recv);
      tpterm();
      return (1);
   }

   flen = sizeof(msgbuf);
   Fget32(recv, OUTPUT, 0, (char *) msgbuf, &flen);
   printf("Respuesta del servidor: %s\n", msgbuf);

   // Liberamos el buffer y desconectamos de la aplicacion
   printf("Liberamos Buffer y desconectamos de la aplicacion\n");
   tpfree((char *) fbfr);
   tpfree((char *) recv);
   tpterm();
	  break;
       }
   }while(opcion != 0);
    return 0;
}

