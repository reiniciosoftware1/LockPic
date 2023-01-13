/*
*
*/

/*!
 * \file
 * \brief Archivo Princpal test cerradura V2
 *
 *  Version: 1.0.0
 * \date 20-09-2022
 */

        /********************************************************
        *          Encabezados de librerias estandares          *
        ********************************************************/

#include <stdio.h>
#include <iostream>
#include <string.h>

#include "Main.h"

using namespace std;


        /********************************************************
        *          Declaracion de Constantes y Macros           *
        ********************************************************/


        /********************************************************
        * Declaracion de struct's, enum's, union's y typedef's  *
        ********************************************************/


        /********************************************************
        *         Prototipos de las funciones globales          *
        *                   (orden alfabetico)                  *
        ********************************************************/


        /********************************************************
        *           Definicion de variables globales            *
        ********************************************************/

        /********************************************************
        *             Definicion de las  funciones              *
        *                   (orden alfabetico)                  *
        ********************************************************/



/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn int main ( int argc, char **argv )
 * \brief Funcion principal del programa.
 * \param argc:
 * \param argv:
 * \return
 *
 * \pre
 * \post
 *
 * DESCRIPCION: Mantiene el proceso principal del MoneyManager
 *
 */
/*------------------------------------------------------------------------------------------------*/
int
main ( int argc, char **argv )
{
    char tempText [ 100 ];  
    unsigned char tempSensor;
    int tempCont;  
    int tempCont2;
    bool tempStatus;
    int pos;
    unsigned int tempOldKey1;
    unsigned int tempNewKey;
    std::string tempString;
    unsigned int tempTime;
    int tempErrorCount;
    int tempLimit;
    int tempOkCount;
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;
    unsigned int tempTokenKey;
    unsigned int tempKey1;
    unsigned int tempKey2;
    unsigned int tempKey3;
    unsigned char tempBuffer [ 255 ];
    printf ( "Iniciando\n" );

    objFile = new CFile ( );
    objFile -> SetPathLog (  "Logs//" );
    objFile -> SetLevelLog ( 4 );

    // Set object PIC
    objPic = new CPic ( );
    objPic -> SetObjFile ( objFile );
    objPic -> SetLevelDebug ( 4 );
    //objPic -> SetNamePort ( "/dev/ttyUSB0" );    
    objPic -> SetNamePort ( "/dev/ttyS1" );
    objPic -> SetMainKey ( 0x01020304 );
    //objPic -> Run ( 1 );    

    // Set token block format and token key
    tempTokenKey = 0x99887766;
    tempKey1 = 0x00000001;
    tempKey2 = 0x00000002;
    tempKey3 = 0x00000003;  	

    objPic -> OpenPort ( );

    printf ( "\n TEST CERRADURAS V 1.2 para PIC 1.1\n\n" );    

    while ( true )
    {
        printf ( "\n0-  FORMATEAR.\n1-  VERSION.\n2-  BUZZER.\n3-  CERRADURA 1.\n4-  CERRADURA 2.\n5-  CERRADURA 3.\n6-  BOLSA CASHBOX.\n\n      OPCION:" );    
        cin.getline ( tempText, 100 );  

#ifdef VERSION_GAPB        

        printf ( "\n\n ***********************************    VERSION EXCLUSIVA\n\n" );
        //  check option
        if ( strcmp (tempText, "900" ) == 0 )
        {  
            objPic -> AddLog ( "Test lectura", 1 );

            printf ( "Obteniendo datos\n" );
            objPic -> AddLog ( "Obteniendo datos", 1 );

            tempStatus = objPic -> CommandReadEEProm ( 0x00, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x05, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x0A, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x0F, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x14, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x19, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x1E, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            tempStatus = objPic -> CommandReadEEProm ( 0x23, 5, tempBuffer );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo datos\n");
                objPic -> AddLog ( "Error obteniendo datos", 1 );

                continue;
            }

            printf ( "Memoria leida: \n" );
            continue;
        }

        //  check option
        if ( strcmp (tempText, "800" ) == 0 )
        {  
            objPic -> AddLog ( "Test Borrar", 1 );

            printf ( "Obteniendo version\n" );
            objPic -> AddLog ( "Obteniendo version", 1 );

            tempStatus = objPic -> CommandVersion ( &tempString );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo version\n");
                objPic -> AddLog ( "Error obteniendo version", 1 );

                continue;
            }

            printf ( "Version leida: %s\n", tempString.c_str ( ) );

            objPic -> AddLog ( "Version leida: " + tempString, 1 );

            printf ( "Borrando memoria\n");

            // Erase memory
            for ( tempCont = 0; tempCont < 50; tempCont ++ )
            {
                printf ( "Borando: %02X\n", tempCont );
                tempStatus = objPic -> CommandWriteEEProm ( tempCont, 0xFF );

                if ( tempStatus != true )
                {
                    printf ( "Erro borrando\n" );
                    break;
                }
            }      

            printf ( "Finaliado\n" );      
            continue;
        }

        //  check option
        if ( strcmp (tempText, "1000" ) == 0 )
        {  
            objPic -> AddLog ( "Test configuracion", 1 );

            printf ( "Obteniendo version\n" );
            objPic -> AddLog ( "Obteniendo version", 1 );

            tempStatus = objPic -> CommandVersion ( &tempString );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo version\n");
                objPic -> AddLog ( "Error obteniendo version", 1 );

                continue;
            }

            printf ( "Version leida: %s\n", tempString.c_str ( ) );

            objPic -> AddLog ( "Version leida: " + tempString, 1 );         

            // Set token block format and token key
            tempTokenKey = 0x99887766;
            tempKey1 = 0x11111111;
            tempKey2 = 0x22222222;
            tempKey3 = 0x33333333;                                    

            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 6, 0x73, tempTokenKey, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando token\n" );
                objPic -> AddLog ( "Error iniciando token", 1 ); 

                continue;
            }

            printf ( "Asignada llave token y tiempo inicial\n" );
            objPic -> AddLog ( "Asignada llave token y tiempo inicial", 1 );

            // Initialize block loock 1            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 0, 0x73, tempKey1, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 1\n" );
                objPic -> AddLog ( "Error iniciando cerradura 1", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 1\n" );
            objPic -> AddLog ( "Inicializado cerradura 1", 1 );

            // Initialize block loock 2            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus =  objPic -> CommandInitBlock2 ( 1, 0x73, tempKey2, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 2\n" );
                objPic -> AddLog ( "Error iniciando cerradura 2", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 2\n" );
            objPic -> AddLog ( "Inicializado cerradura 2", 1 );

            // Initialize block loock 3            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 2, 0x73, tempKey3, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 3\n" );
                objPic -> AddLog ( "Error iniciando cerradura 3", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 3\n" );
            objPic -> AddLog ( "Inicializado cerradura 3", 1 );     

            for ( pos = 0; pos < 500; pos ++ )
            {       
                printf ( "------------------------   Intento: %d\n", pos );

                // Open door 1
                printf ( "Preparando abrir cerradura 1\n" );
                objPic -> AddLog ( "Preparando abrir cerradura 1", 1 );  
                usleep ( 1500000 );
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandOpen2 ( 1, tempKey1, tempTokenKey, tempTime );            
                if ( tempStatus != true )
                {
                    printf ( "Error abriendo cerradura 1\n" );
                    objPic -> AddLog ( "Error abriendo cerradura 1", 1 ); 

                    continue;
                }

                usleep ( 3000000 );
                printf ( "Abierta cerradura 1\n" );            

                // Close door 1
                printf ( "Preparando cerrar cerradura 1\n" );
                objPic -> AddLog ( "Preparando cerrar cerradura 1", 1 );  
                usleep ( 1500000 );
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandClose2 ( 1, tempKey1, tempTokenKey, tempTime );            
                if ( tempStatus != true )
                {
                    printf ( "Error cerrando cerradura 1\n" );
                    objPic -> AddLog ( "Error cerrando cerradura 1", 1 ); 

                    continue;
                }

                // Open door 2
                printf ( "Preparando abrir cerradura 2\n" );
                objPic -> AddLog ( "Preparando abrir cerradura 2", 1 );  
                usleep ( 1500000 );
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandOpen2 ( 2, tempKey2, tempTokenKey, tempTime );            
                if ( tempStatus != true )
                {
                    printf ( "Error abriendo cerradura 2\n" );
                    objPic -> AddLog ( "Error abriendo cerradura 2", 1 ); 

                    continue;
                }

                usleep ( 3000000 );
                printf ( "Abierta cerradura 2\n" );

                // Close door 2
                printf ( "Preparando cerrar cerradura 2\n" );
                objPic -> AddLog ( "Preparando cerrar cerradura 2", 1 );  
                usleep ( 1500000 );
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandClose2 ( 2, tempKey2, tempTokenKey, tempTime );            
                if ( tempStatus != true )
                {
                    printf ( "Error cerrando cerradura 2\n" );
                    objPic -> AddLog ( "Error cerrando cerradura 2", 1 ); 

                    continue;
                }

                usleep ( 3000000 );
                printf ( "Cerrada cerradura 2\n" );

                // Open door 3
                printf ( "Preparando abrir cerradura 3\n" );
                objPic -> AddLog ( "Preparando abrir cerradura 3", 1 );  
                usleep ( 1500000 );
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandOpen2 ( 3, tempKey3, tempTokenKey, tempTime );            
                if ( tempStatus != true )
                {
                    printf ( "Error abriendo cerradura 3\n" );
                    objPic -> AddLog ( "Error abriendo cerradura 3", 1 ); 

                    continue;
                }            
                printf ( "Abierta cerradura 3\n" );
                usleep ( 3000000 );

                usleep ( 1500000 );
                tempOldKey1 = tempKey1;

                // Update random seed
                srand ( clock ( ) ); 
                tempNewKey = rand ( ) % 0xFFFFFF;

                printf ( "Cambiando llave cerradura 1 vieja: %08X nueva: %08X\n", tempOldKey1, tempNewKey );
                  
                tempTime = objPic -> CalcTimeToken ( );
                tempStatus = objPic -> CommandChangeKey ( 0, tempOldKey1, tempNewKey, tempTime );
                if ( tempStatus != true )
                {
                    printf ( "Error cambiando llave\n" );
                    objPic -> AddLog ( "Error cambiando llave", 1 ); 

                    continue;
                }

                tempKey1 = tempNewKey;

                usleep ( 1500000 );
            }

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }
#endif        

        //  check option
        if ( strcmp (tempText, "0" ) == 0 )
        {  
            objPic -> AddLog ( "Test Inicializar", 1 );            

            printf ( "Obteniendo version\n" );
            objPic -> AddLog ( "Obteniendo version", 1 );

            tempStatus = objPic -> CommandVersion ( &tempString );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo version\n");
                objPic -> AddLog ( "Error obteniendo version", 1 );

                continue;
            }

            printf ( "Version leida: %s\n", tempString.c_str ( ) );

            objPic -> AddLog ( "Version leida: " + tempString, 1 );     

            // Check version
            if ( strcmp (tempText, "1.0" ) == 0 )    
            {
                printf ( "\nError version de PIC vieja, no compatible\n" );
                continue;
            }            
            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 6, 0x73, tempTokenKey, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando token, Reprogramar PIC\n" );
                objPic -> AddLog ( "Error iniciando token", 1 ); 

                continue;
            }

            printf ( "Asignada llave token y tiempo inicial\n" );
            objPic -> AddLog ( "Asignada llave token y tiempo inicial", 1 );

            // Initialize block lock 1            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 0, 0x73, tempKey1, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 1, reprograme PIC\n" );
                objPic -> AddLog ( "Error iniciando cerradura 1", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 1\n" );
            objPic -> AddLog ( "Inicializado cerradura 1", 1 );

            // Initialize block lock 2            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus =  objPic -> CommandInitBlock2 ( 1, 0x73, tempKey2, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 2, reprograme PIC\n" );
                objPic -> AddLog ( "Error iniciando cerradura 2", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 2\n" );
            objPic -> AddLog ( "Inicializado cerradura 2", 1 );

            // Initialize block lock 3            
            tempTime = objPic -> CalcTimeToken ( );
            tempStatus = objPic -> CommandInitBlock2 ( 2, 0x73, tempKey3, 0xFFFFFFFF, tempTime );
            if ( tempStatus != true )
            {
                printf ( "Error iniciando cerradura 3, reprograme PIC\n" );
                objPic -> AddLog ( "Error iniciando cerradura 3", 1 ); 

                continue;
            }

            printf ( "Inicializado cerradura 3\n" );
            objPic -> AddLog ( "Inicializado cerradura 3", 1 );     

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }      

        //  check option
        if ( strcmp (tempText, "1" ) == 0 )
        {  
            objPic -> AddLog ( "Test detectar", 1 );

            printf ( "Obteniendo version\n" );
            objPic -> AddLog ( "Obteniendo version", 1 );

            tempStatus = objPic -> CommandVersion ( &tempString );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo version\n");
                objPic -> AddLog ( "Error obteniendo version", 1 );

                continue;
            }

            printf ( "Version leida: %s\n", tempString.c_str ( ) );

            objPic -> AddLog ( "Version leida: " + tempString, 1 );

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }

        //  check option
        if ( strcmp (tempText, "2" ) == 0 )
        {  
            objPic -> AddLog ( "Test buzzer", 1 );

            for ( tempCont = 0; tempCont < 5; tempCont++ )
            {
                printf ( "%d- Pitando\n", tempCont );
                objPic -> AddLog ( "Pitando", 1 );

                tempStatus = objPic -> CommandBuzzer ( 100 );

                if ( tempStatus != true )
                {
                    printf ( "Error comando buzzer\n" );
                    break;
                }

                usleep ( 2000000 );
            }

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Finalizado test", 1 );

            continue;            
        }

        //  check option
        if ( ( strcmp (tempText, "3" ) == 0 ) || ( strcmp (tempText, "31" ) == 0 ) )
        {  
            objPic -> AddLog ( "Test cerradura 1", 1 );  

            if ( ( strcmp (tempText, "31" ) == 0 ) )
            {
                tempLimit = 2000;                   
            }
            else
            {
                tempLimit = 5;
            }            

            tempErrorCount = 0;
            tempOkCount = 0;

            for ( tempCont = 0; tempCont < tempLimit; tempCont++ )
            {
                printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

                // Calculate time
                tempTime = objPic -> CalcTimeToken ( );                         

                // abrir puerta 1
                tempStatus = objPic -> CommandOpen2 ( 1, tempKey1, tempTokenKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Desbloqueando pestillo...\n" );  
                    objPic -> AddLog ( "Desbloqueando pestillo 1...", 1 );    
                    tempOkCount ++;              
                }
                else
                {
                    printf ( "Error desbloqueando pestillo...\n" );
                    objPic -> AddLog ( "Error desbloqueando pestillo 1...", 1 );
                    tempErrorCount ++;
                } 

                usleep ( 1000000 );

                for ( tempCont2 = 0; tempCont2 < 12; tempCont2 ++ )
                {
                    tempStatus = objPic -> CommandGetSensor ( 1, &tempSensor );

                    if ( tempStatus != true )
                    {
                        printf ( "Error Obteniendo estado sensores 1\n" );
                        objPic -> AddLog ( "Error Obteniendo estado sensores 1", 1 );
                        break;
                    }

                    objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_BLACK, "    ", &tempString );
                    objPic -> AddLog ( tempString, 1 );

                    printf ( "%s\n", tempString.c_str ( ) );
                    usleep ( 500000 );
                }

                 // Calculate time
                tempTime = objPic -> CalcTimeToken ( );                         

                // abrir puerta 1                
                tempStatus = objPic -> CommandClose2 ( 1, tempKey1, tempTokenKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bloqueando pestillo...\n" );   
                    objPic -> AddLog ( "Bloqueando pestillo", 1 );     
                    tempOkCount ++;            
                }
                else
                {
                    printf ( "Error bloqueando pestillo...\n" );
                    objPic -> AddLog ( "Error bloqueando", 1 );
                    tempErrorCount ++;
                } 

                usleep ( 1000000 );

                for ( tempCont2 = 0; tempCont2 < 12; tempCont2 ++ )
                {
                    tempStatus = objPic -> CommandGetSensor ( 1, &tempSensor );

                    if ( tempStatus != true )
                    {
                        printf ( "Error Obteniendo estado sensores 1\n" );
                        objPic -> AddLog ( "Error Obteniendo estado sensores 1", 1 );
                        break;
                    }

                    objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_BLACK, "    ", &tempString );
                    objPic -> AddLog ( tempString, 1 );

                    printf ( "%s\n", tempString.c_str ( ) );
                    usleep ( 500000 );
                }                
            }

            printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }


        //  check option
        if ( ( strcmp (tempText, "4" ) == 0 ) || ( strcmp (tempText, "41" ) == 0 ) )
        {  
            objPic -> AddLog ( "Test cerradura 2", 1 );                        

            tempErrorCount = 0;
            tempOkCount = 0;

            if ( ( strcmp (tempText, "41" ) == 0 ) )
            {
                tempLimit = 2000;                   
            }
            else
            {
                tempLimit = 5;
            }            

            for ( tempCont = 0; tempCont < tempLimit; tempCont++ )
            {
                printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

                // Calculate time
                tempTime = objPic -> CalcTimeToken ( );                         

                // abrir puerta 1                
                tempStatus = objPic -> CommandOpen2 ( 2, tempKey2, tempTokenKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Desbloqueando pestillo...\n" );  
                    objPic -> AddLog ( "Desbloqueando pestillo 2...", 1 );      
                    tempOkCount ++;            
                }
                else
                {
                    printf ( "Error desbloqueando pestillo...\n" );
                    objPic -> AddLog ( "Error desbloqueando pestillo 2...", 1 );
                    tempErrorCount ++;
                } 

                usleep ( 1000000 );

                for ( tempCont2 = 0; tempCont2 < 12; tempCont2 ++ )
                {
                    tempStatus = objPic -> CommandGetSensor ( 2, &tempSensor );

                    if ( tempStatus != true )
                    {
                        printf ( "Error Obteniendo estado sensores 2\n" );
                        objPic -> AddLog ( "Error Obteniendo estado sensores 2", 1 );
                        break;
                    }

                    objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_BLACK, "    ", &tempString );
                    objPic -> AddLog ( tempString, 1 );

                    printf ( "%s\n", tempString.c_str ( ) );
                    usleep ( 500000 );
                }

                 // Calculate time
                tempTime = objPic -> CalcTimeToken ( );                         

                // abrir puerta 2                
                tempStatus = objPic -> CommandClose2 ( 2, tempKey2, tempTokenKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bloqueando pestillo...\n" );   
                    objPic -> AddLog ( "Bloqueando pestillo", 1 );      
                    tempOkCount ++;           
                }
                else
                {
                    printf ( "Error bloqueando pestillo...\n" );
                    objPic -> AddLog ( "Error bloqueando", 1 );
                    tempErrorCount ++;
                }                 

                usleep ( 1000000 );

                for ( tempCont2 = 0; tempCont2 < 12; tempCont2 ++ )
                {
                    tempStatus = objPic -> CommandGetSensor ( 2, &tempSensor );

                    if ( tempStatus != true )
                    {
                        printf ( "Error Obteniendo estado sensores 2\n" );
                        objPic -> AddLog ( "Error Obteniendo estado sensores 2", 1 );
                        break;
                    }

                    objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_BLACK, "    ", &tempString );
                    objPic -> AddLog ( tempString, 1 );

                    printf ( "%s\n", tempString.c_str ( ) );
                    usleep ( 500000 );
                }                
            }

            printf ( "Cerradura 2 --- Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }



        //  check option
        if ( ( strcmp (tempText, "5" ) == 0 ) || ( strcmp (tempText, "51" ) == 0 ) )
        {  
            objPic -> AddLog ( "Test cerradura 3", 1 );            

            tempErrorCount = 0;
            tempOkCount = 0;

            if ( ( strcmp (tempText, "51" ) == 0 ) )
            {
                tempLimit = 2000;                   
            }
            else
            {
                tempLimit = 5;
            }            

            for ( tempCont = 0; tempCont < tempLimit; tempCont++ )
            {
                printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

                // Calculate time
                tempTime = objPic -> CalcTimeToken ( );                         

                // abrir puerta 3                
                tempStatus = objPic -> CommandOpen2 ( 3, tempKey3, tempTokenKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Desbloqueando pestillo 3...\n" );  
                    objPic -> AddLog ( "Desbloqueando pestillo 3...", 1 );      
                    tempOkCount++;            
                }
                else
                {
                    printf ( "Error desbloqueando pestillo...\n" );
                    objPic -> AddLog ( "Error desbloqueando pestillo 3...", 1 );
                    tempErrorCount ++;
                } 

                usleep ( 1000000 );

                for ( tempCont2 = 0; tempCont2 < 12; tempCont2 ++ )
                {
                    tempStatus = objPic -> CommandGetSensor ( 3, &tempSensor );

                    if ( tempStatus != true )
                    {
                        printf ( "Error Obteniendo estado sensores 3\n" );
                        objPic -> AddLog ( "Error Obteniendo estado sensores 3", 1 );
                        break;
                    }

                    objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_SOLENOIDE, "    ", &tempString );
                    objPic -> AddLog ( tempString, 1 );

                    printf ( "%s\n", tempString.c_str ( ) );
                    usleep ( 500000 );
                }
            }

            printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

            printf ( "\n  --------- Finalizado test ------------\n" );
            objPic -> AddLog ( "Fin test", 1 );

            continue;            
        }

        //  check option
        if ( strcmp ( tempText, "6" ) == 0 )
        {  
            objPic -> AddLog ( "Test Bolsa", 1 );            
         
            printf ( "Test bolsa\n" );            
            tempOkCount = 0;
            tempErrorCount = 0;
         
            for ( tempCont = 0; tempCont < 10; tempCont ++ )
            {
                printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );

                tempStatus = objPic -> CommandGetCashBox ( &tempSensor );

                if ( tempStatus != true )
                {
                    printf ( "Error Obteniendo estado bolsa\n" );
                    objPic -> AddLog ( "Error Obteniendo estado bolsa", 1 );
                    tempErrorCount++;
                    continue;
                }

                tempOkCount ++;

                objPic -> DecodeSensor ( tempSensor, objPic -> SENSOR_CASHBOX, "    ", &tempString );
                objPic -> AddLog ( tempString, 1 );

                printf ( "%s\n", tempString.c_str ( ) );                
                usleep ( 2000000 );
            }
            printf ( "Proceso %d --- bien: %d  mal: %d\n", tempCont, tempOkCount, tempErrorCount );
            objPic -> AddLog ( "Fin prueba", 1 );
            printf ( "Fin prueba\n" );
        }        
    }
    
    return 0;
}


/*------------------------------------------------------------------------------------------------
 * \fn bool CManager::StartThreadPic ( void )
 * \brief Start communication PIC
 * \param Ninguno.
 * \return true: thread started
 * \return false: Error
 *
 * \pre
 * \post
 *
 * DESCRIPCION: start thread for lock board
------------------------------------------------------------------------------------------------*/
bool StartThreadPic ( void )
{      
    // Launch function thread
    int x = 10;
    //std::thread threadObj ( &CPic::Run, objLock, x );
    //threadObj.detach ();

    return true;
}




