// VER 1.0.1
// 06-14-2022

/*
PROGRAMA PARA LA CERRADURA ELECTRONICA

PIC 18F252
1   MCLR   IN
2  RA0   IN      SL-1
3   RA1   IN      SD-1
4   RA2   IN      SL-2
5   RA3   IN      SD-2
6   RA4   IN      CASHBOX
7   RA5   IN      SD-3
8   VSS   0V
9   OSC1   IN
10   OSC2   IN
11   RC0   OUT   LOCKA-1-IA
12   RC1   OUT   LOCKA-1-IB
13   RC2   OUT   LOCKA-2-IA
14   RC3   OUT   LOCKA-2-IB
15   RC4   OUT   LOCKB-3-IA
16   RC5   OUT   LED indicador
17   RC6   OUT   TX   
18   RC7   IN      RX
19   VSS   0V
20   VDD   5V
21   RB0   IN      LOCKA-1-LI
22   RB1   IN      LOCKA-1-LO
23   RB2   IN      LOCKA-2-LI
24   RB3   IN      LOCKA-2-LO
25   RB4   OUT   I2C-CLK
26   RB5   IN/OUT   I2C-SDA
27   RB6        
28   RB7   OUT   BUZZER


PIC 18F452

1   MCLR   IN
2  RA0   IN      SL-1
3   RA1   IN      SD-1
4   RA2   IN      SL-2
5   RA3   IN      SD-2
6   RA4   IN      SL-3
7   RA5   IN      SD-3
8  RE0     
9   RE1
10   RE2     
11   VDD
12   VSS   0V
13   OSC1   IN
14   OSC2   IN
15   RC0   OUT   LOCKA-1-IA
16   RC1   OUT   LOCKA-1-IB
17   RC2   OUT   LOCKA-2-IA
18   RC3   IN/OUT   I2C-SDA //// ERA RB5
19   RD0   OUT   LOCKA-2-IB ////ERA RC3
20   RD1   OUT   LOCKB-3-IA //// ERA RC4
21   RD2   IN      LOCKB-3-LO  /// ERA RB6
22   RD3   OUT   BUZZER  ////// ERA RB7
23   RC4   OUT   I2C-CLK   //// ERA RB4
24   RC5   OUT   LED indicador
25   RC6   OUT   TX   
26   RC7   IN      RX
27   RD4
28   RD5
29   RD6
30   RD7
31   VSS   0V
32   VDD   5V
33   RB0   IN      LOCKA-1-LI
34   RB1   IN      LOCKA-1-LO
35   RB2   IN      LOCKA-2-LI
36   RB3   IN      LOCKA-2-LO
37   RB4     
38   RB5   XX   PGM   /////// NUEVO
39   RB6   XX   PGC   /////// NUEVO
40   RB7   XX   PGD   /////// NUEVO

CERRADURAS: LOCKx-n-ss  x = tipo, n numero, ss senal

COMUNICACION
VELOCIDAD 9600
BITS 8
STOP 1
PARIDAD PAR
     
FORMATO DE TRAMA
ENCABEZADO (CBUFF 0)     :0X03
LONGITUD   (CBUFF 1)     :CODIGO + DATOS + CHECKSUM
CODIGO     (CBUFF 2)     :0x10, 0x11, 0x20, 0x21, 0x30, 0x31, 0x40, 0x41
DATOS      (CBUFF 3-N)   :INFORMACION ADICIONAL AL COMANDO
CHECKSUM   (CBUFF N+1)   :OPERACION XOR ENTRE LOS BYTES DE LONGITUD, COMANDO Y DATOS

*/

#include "Lock.h"

//--------------------------------------------------------------------------------------

//CODIGOS DE DEPURACION----------------------------------------------------------------------------------------------------
//#define DEBUG               // PERMITE ACTIVAR CODIGO DE DEPURACION
//#define ECO                 // PERMITE ACTIVAR ECO DEL DATO ENVIADO POR EL PC
//#define BUFFER              // PERMITE VERIFICAR LO QUE SE GUARDO EN EL BUFFER
//#define   PIC_40P            // HABILITA CAMBIOS PARA PIC DE 40 PINES
//-------------------------------------------------------------------------------------------------------------------------

// VARIABLES EN RAM
int8 bufferRx [ LENGTH_BUFFRX ];   // Buffer que almacena los caracteres recibidos                    
int8 commandRx [ LENGTH_BUFFRX ];   // Buffer para un comando recibido 
int8 tokenCheck [ 10 ];   // Buffer para una trama de token a analizar
unsigned int32 randomValue;      // Ultimo valor randomico recibido
int8 dataRx;                     // último byte recibido por el puerto USART
int8 bufferTx [ LENGTH_BUFFTX ];   // Buffer datos a transmitir
int8 flagNewCommand;               // Bandera nuevo comando recibido
int8 byteCountRx;                  // numero de bytes recibidos 
int8 commandSize;                  // Numero de bytes recibidos del ultimo comando
int8 byteCountTx;                  // numero de bytes transmitir
int8 flagRTCInstalled;            // Indica si el RTC esta isntalado
int8 versionOrder;               // Version del comando que se esta ejecutando
int8 statusNow;                  // Estado logico actual
int8 statusNew;                  // Estado logico futuro
int8 stepNow;                     // Sub proceso logico actual              
int8 flagExit;                     // Controla finalizacion principal
int8 timerMain1;                  // Timer en fracciones de 10ms-hasta 2.55s
int8 timerMain2;                  // Timer en fracciones de 10ms-hasta 2.55s
int16 timerMain3;                  // Timer en fracciones de 10ms-hasta 65.535s
int16 timerMain4;                  // Timer en fracciones de 10ms-hasta 65.535s
int8 timerRxCommand;               // Timer para temporizar recepcion comando
int16 timerPulse;                  // Timer para temporizar pulsos ordenes cerradura
int16 timerCommunication;          // Timer para temporizar recepcion de datos
int8 flagCommunication;                 //Bandera para indicar que hay comunicaciones desde el computador
int8 externalKey [ 4 ];         // llave a usar en la operacion
int8 valueKey [ 4 ];               // Nuevo valor de llave para escritura o lectura
int8 indexBlock;                  // Numero de bloque a operar
int8 accessBlock;                  // Condiciones de acceso al bloque
int8 selectLock;                  // ID cerradura sobre la que se esta ejecutando una operacion
int8 selectSensor;               // ID cerradura a la cual leer los sensores
int8 flagReadSensor;               // Indica si debe hacer proceso lectura sensonres
int8 flagGetVersion;               // Indica si se debe hacer reporte de version firmware
int8 flagPoll;                 // Indica si se debe hacer reporte de poll
int8 byteEEprom;                  // Byte a escribir en la EEPROM coamndo de recuperacion
int8 addressEEprom;               // Direccion a escribir en la EEPROM coamndo de recuperacion
int8 timeBuzzer;                  // Tiempo en decimas de segundo apra el buzzer
int8 failAuth;                     // Contador errores operaciones autenticadas
int8 failToken;                  // Contador de errores de token
int8 flagCashBox;                  // Indica si debe leer sensor cashbox


/*----------------------------------------------------------------------
                        INTERRUPCIONES
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Interrupcion de dato recibido por el modulo USART
 @param Ninguno
 @return Ninguno
----------------------------------------------------------------------------*/
#int_rda
void serial_isr ( void )
{
   // Borra dato recibido
   dataRx = 0x00;
   
   // Valida si ya llego un byte
   if ( kbhit ( ) == true )
   {
      // Reinicia timer de recepcion
      timerRxCommand = TIMEOUT_RXCOMMAND;
        
      // Lee el byte del USART
      dataRx = getc ( );  
      
      // valida si es el byte de encabezado
      if ( ( byteCountRx == 0 ) && ( dataRx != HEAD_BYTE ) )
      {
         // Error de byte no valida todabi incio de comando
         return;
      }
      
      // Agrega el dato al buffer de recepcion
      bufferRx [ byteCountRx ] = dataRx;    

      #ifdef ECO
      SendByte ( dataRx );            // ECO del dato recibido en el buffer.
      #endif
      
      // Inidica que hay comunicacion desed el computador
      flagCommunication = true;

      // Incrementa contador de bytes recibidos
      byteCountRx++;

      // Valida si ya recibio almenos el encabezado y longitud
      if ( byteCountRx >= 2 )
      {
         // Valida si llegaron los datos esperados del comando
         if ( ( byteCountRx >= ( bufferRx [ 1 ] + 2 ) ) && 
               ( kbhit ( ) == false ) && ( flagNewCommand == FALSE ) )
         {
            // Copia los datos del buffer de recepcion al de comandos
            memcpy ( commandRx, bufferRx, byteCountRx );
              
            // Disponible nuevo comando pero sin verificar
            flagNewCommand = TRUE;
              
            // Copia longitud de comando recibido
            commandSize = byteCountRx;
              
            byteCountRx = 0;
         }
         else
         {
            // No ha llegado el comando
            flagNewCommand = FALSE;
         }
      }
   }
}

/*----------------------------------------------------------------------------
 Interrupcion TMR0 cada 10ms
 @param Ninguno
 @return Ninguno
----------------------------------------------------------------------------*/
#int_timer0
void timer0_isr ( void )
{
   // Asiga nuevamente valor para desbordar cada 10ms
   set_timer0 ( TIMER_TMR0_10M );
     
   // decrementa timerMain1
   if ( timerMain1 > 0 )
   {
      // Decrementa contador
      timerMain1 --;
   }
     
   // decrementa timerMain2
   if ( timerMain2 > 0 )
   {
      // Decrementa contador
      timerMain2 --;
   }
     
   // decrementa timerMain3
   if ( timerMain3 > 0 )
   {
      // Decrementa contador
      timerMain3 --;
   }
     
   // decrementa timerMain4
   if ( timerMain4 > 0 )
   {
      // Decrementa contador
      timerMain4 --;
   }     
     
   // decrementa timerPulse para pulso cerradura
   if ( timerPulse > 0 )
   {
      // Decrementa contador
      timerPulse --;
   }   
   
   // decrementa timerPulse para pulso comunicaciones
   if ( timerCommunication > 0 )
   {
      // Decrementa contador
      timerCommunication --;
   }               
     
   // decrementa timerMain4
   if ( timerRxCommand > 0 )
   {
      // Decrementa contador
      timerRxCommand --;
        
      // Valida si ya expiro el tiemout
      if ( timerRxCommand == 0 )
      {
         // Descarta bytes recibidos hasta el momento     
         byteCountRx = 0;
      }
   }

}
/*----------------------------------------------------------------------
                  FUNCIONES
----------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 Calcula el CRC de un frame de datos. Operacion XOR de todos los datos
 @param tempBuffer Puntero al buffer que se debe calcular el CRC
 @return CRC calculado
----------------------------------------------------------------------------*/
int8 CalcCRC ( unsigned char * tempBuffer )
{
   int8 tempCrc;
   int8 cont;
   int8 tempLimit;
     
   // Valor inicial para el calculo del CRC
   tempCrc = 0x00;
     
   // Calcula la cantidad de bytes hasta la que se tiene en cuenta el CRC
   tempLimit = tempBuffer [ 1 ] + 1;
     
   // Ciclo para calcular el CRC
   for ( cont = 1; cont < tempLimit; cont ++ )
   {
       tempCrc = tempCrc ^ ( * ( tempBuffer + cont ) );
   }
     
   return tempCrc;
}

/*-----------------------------------------------------------------------------
 Borra los datos del buffer de recepcion
 @param Ninguno
 @return Nada
------------------------------------------------------------------------------*/
void ClearBufferRx ( void )
{  
   memset ( bufferRx, 0x00, LENGTH_BUFFRX );
  
   // Apunta al inicio del buffer para recibir datos
   byteCountRx = 0x00;
}

/*-----------------------------------------------------------------------------
 Borra los datos del buffer de transmision
 @param Ninguno
 @return Nada
------------------------------------------------------------------------------*/
void ClearBufferTx ( void )
{                  
   memset ( bufferRx, 0x00, LENGTH_BUFFTX );
}

/*-----------------------------------------------------------------------------
 Compara los datos de dos arrays 
 @param tempData1 Apuntador a el array 1
 @param Apuntador a el array 2
 @param tempLength Numero de bytes a comparar
 @return TRUE Ambos array tiene los mismos datos
 @return FALSE Los array son diferentes
------------------------------------------------------------------------------*/
int8 CompareBuffer ( int8 * tempData1, int8 * tempData2, int8 tempLength )
{
   int pos;
     
   // Ciclo para comparar cada byte de ambos buffers
   for ( pos = 0; pos < tempLength; pos++ )
   {
      // Compara cada byte
      if ( tempData1 [ pos ] != tempData2 [ pos ] )
      {
         return FALSE;
      }
   }
     
   return TRUE;     
}

/*-----------------------------------------------------------------------------
 Depura varible de 4Bytes
 @param tempData1 Apuntador a el array 1
 @param tempData2 Apuntador a el array 2
 @param tempLength Numero de bytes a comparar
 @return TRUE Ambos array tiene los mismos datos
 @return FALSE Los array son diferentes
------------------------------------------------------------------------------*/
void DebugInt32 ( unsigned int32 tempValue )
{      
#ifdef DEBUG  
   unsigned int8 tempBuffer [ 8 ];
   
   // Convert time to array, offset 0 is highest byte
   tempBuffer [ 0 ] = tempValue >> 24;
   tempBuffer [ 1 ] = tempValue >> 16;
   tempBuffer [ 2 ] = tempValue >> 8;
   tempBuffer [ 3 ] = tempValue;

   SendByte ( 0xDE );
   SendByte (tempBuffer [ 0 ] );
   SendByte (tempBuffer [ 1 ] );
   SendByte (tempBuffer [ 2 ] );
   SendByte (tempBuffer [ 3 ] );
#endif           
}



/*-----------------------------------------------------------------------------
 Funcion principal
 @param Ninguno
 @return Ninguno
------------------------------------------------------------------------------*/
void main ( )
{   
   int8 tempStatus;         
   int8 tempEEprom [ 10 ];
   int8 tempAccess;
   int8 tempByte;
   int8 pos;
   int8 tempBuffAns [ 50 ];   
   int8 tempFlagRx;
   int32 tempNum;   
   unsigned char tempNewKey [ 4 ];
   unsigned int32 tempData2;
   
   // Define estado logico inicial
   statusNow = STATUS_UNITIALIZE;
   statusNew = STATUS_INITIALIZE;
   stepNow = STEP_START;
   
   // reinicia contador de errores autenticacion
   failAuth = 0;
   failToken = 0;

   // Apaga el perro guardian
   setup_wdt ( WDT_OFF );   
   
   // Estados iniciales pines cerraduras
   output_high ( PIN_1_IA );
   output_high ( PIN_1_IB );   
   output_high ( PIN_2_IA );
   output_high ( PIN_2_IB );
   output_low ( PIN_3_IA );   
   
   // No ha rebido ningun dato
   dataRx = 0;         
   byteCountRx = 0;           
   commandSize = 0;
   byteCountTx = 0;    
   timerRxCommand = 0;
   flagNewCommand = FALSE;
     
   // No hay cerrdura en operacion 
   selectLock = 0;
   selectSensor = 0;
   flagReadSensor = false;
     
   // Borra solicitud cashbox   
   flagCashBox = false;   
  
   // Borra otros estados
   flagGetVersion = false;   
   flagPoll = false;
   timeBuzzer = 0;
   
   tempFlagRx = false;

   // Borra buffer de datos
   ClearBufferRx ( );
   ClearBufferTx ( );     

   // RTC esta funcioanando bien
   flagRTCInstalled = FALSE;                               

   // Secuencia de inicio          
   output_high ( PIN_LED );
   output_high ( PIN_BUZZER );
   delay_ms ( 500 );
   output_low ( PIN_LED );
   delay_ms ( 200 );   
   output_high ( PIN_LED );
   delay_ms ( 200 );
   output_low ( PIN_LED );
   delay_ms ( 200 );   
   output_high ( PIN_LED );      
   output_low ( PIN_BUZZER );          
      
   // Activa interrupcion recepcion de datos
   enable_interrupts ( int_rda );   
     
   // Activa interrupciones globales
   enable_interrupts ( global );
   
   // Configura el TMR0 como reloj principla para genera interrupcion cada 10ms
   setup_timer_0 ( T0_INTERNAL | T0_DIV_1 );
   set_timer0 ( TIMER_TMR0_10M );
   
   // Activa interrupcion TMR0
   enable_interrupts ( int_timer0 );       
   
   // Inicia timer para senalizar comunicacion
   timerCommunication = TIMEOUT_COMMUNICATION;
   flagCommunication = false;               
  
   // Ciclo principal
   flagExit = FALSE;
   while ( flagExit == FALSE )
   {
      // Valida timer comunicaciones
      if ( timerCommunication == 0 )
      {
         // Valida si llego un comando dentro del tiempo de parpadeo
         if ( flagCommunication == true )
         {
            flagCommunication = false;
            
            // Apaga el LED
            output_toggle ( PIN_LED );
         }
         else
         {
            // Mantiene el LED encendido
            output_high ( PIN_LED );
         }
         
         // Inicia timer para senalizar comunicacion
         timerCommunication = TIMEOUT_COMMUNICATION;         
      }
   
      // Valida el proceso actual
      if ( statusNew == STATUS_INITIALIZE )
      {
         statusNow = STATUS_INITIALIZE;
         statusNew = STATUS_FREE;
         continue;
      }              
        
      // Valida si hay comando recibido
      if ( flagNewCommand == TRUE )
      {
         // Verifica formato comando
         tempStatus = ValidateCommand ( );
           
         commandSize = 0;
           
         // Valida si hubo error
         if ( tempStatus != ANS_OK )
         {              
            // Envia respuesta de error
            SendAns ( commandRx [ 2 ], tempStatus );              
              
            // Indica que ya fue procesado el comando              
            flagNewCommand = FALSE;
              
            continue;
         }                                                  
      }    
        
      // Valida contador de errores autenticacion
      if ( ( failAuth >= 3 ) || ( failToken >= 3 ) )
      {
         SleepMinute ( 5 );
      }
        
      // Valida si debe reportar version firmware
      if ( flagGetVersion == true )
      {                 
         tempBuffAns [ 0 ] = VERSIONH;
         tempBuffAns [ 1 ] = VERSIONL;
           
         // Comando terminado procesar
         flagNewCommand = FALSE;
           
         // Envia respuesta
         SendAnsComplex ( COMMAND_VERSION, ANS_OK, tempBuffAns, 2 );
           
         flagGetVersion = false;
         continue;
      }      
      
      // Valida si debe reportar poll
      if ( flagPoll == true )
      {                 
         tempBuffAns [ 0 ] = 0x00;
         tempBuffAns [ 1 ] = 0x00;
           
         // Comando terminado procesar
         flagNewCommand = FALSE;
           
         // Envia respuesta
         SendAnsComplex ( COMMAND_POLL, ANS_OK, tempBuffAns, 2 );
           
         flagPoll = false;
         continue;
      } 
        
      // Valida si es activacion buzzer
      if ( statusNew == STATUS_BUZZER )
      {
         statusNow = STATUS_BUZZER;     
           
         // Valida paso configuracion inicial
         if ( stepNow == STEP_START )
         {        
            // Comando terminado procesar
            flagNewCommand = FALSE;
              
            timerMain1 = timeBuzzer;
              
            // Envia respuesta
            SendAns ( commandRx [ 2 ], ANS_OK );     
              
            stepNow = STEP_WAIT; 
              
            // Activa buzzer
            output_high ( PIN_BUZZER );
            continue;                 
         }
           
         // Valida terminacion tiempo
         if ( stepNow == STEP_WAIT )
         {        
            // Valida si transcurrio el tiempo
            if ( timerMain1 == 0 )
            {              
               // Apaga buzzer
               output_low ( PIN_BUZZER );
                 
               // Termina proceso
               statusNew = STATUS_FREE;
               stepNow = STEP_START;
                 
               continue;                 
            }     
              
            continue;
         }                    
           
         continue;
      }        
        
      // Valida si debe hacer lectura sensores
      if ( flagReadSensor == true )
      {                 
         tempByte = 0x00;                          

         // Valida ID cerradura
         if ( selectSensor == 1 )
         {
            // Valida sensor
            if ( input ( PIN_1_SD ) == 1 )
            {
               tempByte |= MASK_SD;
            }
              
            // Valida sensor
            if ( input ( PIN_1_SL ) == 1 )
            {
               tempByte |= MASK_SL;              
            }
              
            // Valida sensor
            if ( input ( PIN_1_LO ) == 1 )
            {
               tempByte |= MASK_LO;
            }
              
            // Valida sensor
            if ( input ( PIN_1_LI ) == 1 )
            {
               tempByte |= MASK_LI;
            }
         }
           
         // Valida ID cerradura
         if ( selectSensor == 2 )
         {
            // Valida sensor
            if ( input ( PIN_2_SD ) == 1 )
            {
               tempByte |= MASK_SD;
            }
              
            // Valida sensor
            if ( input ( PIN_2_SL ) == 1 )
            {
               tempByte |= MASK_SL;
            }
              
            // Valida sensor
            if ( input ( PIN_2_LO ) == 1 )
            {
               tempByte |= MASK_LO;
            }
              
            // Valida sensor
            if ( input ( PIN_2_LI ) == 1 )
            {
               tempByte |= MASK_LI;
            }
         }
           
         // Valida ID cerradura
         if ( selectSensor == 3 )
         {
            // Valida sensor
            if ( input ( PIN_3_SD ) == 1 )
            {
               tempByte |= MASK_SD;
            }
              
            /*
            // Valida sensor
            if ( input ( PIN_3_SL ) == 1 )
            {
               tempByte |= MASK_SL;
            }
            */
         }                 
           
         // Coamndo termiando procesar
         flagNewCommand = FALSE;                    

         // Asigna byte de estado
         tempBuffAns [ 0 ] = tempByte;
           
         // Envia respuesta
         SendAnsComplex ( COMMAND_STATUS, ANS_OK, tempBuffAns, 1 );
           
         flagReadSensor = false;
     
         continue;
      }              
        
      // Valida si debe hacer lectura cashbox
      if ( flagCashBox == true )
      {                 
         tempByte = 0x00;                          

         // Valida sensor
         if ( input ( PIN_CASHBOX ) == 1 )
         {
            tempByte = 0x01;
         }                                
           
         // Coamndo termiando procesar
         flagNewCommand = FALSE;                    

         // Asigna byte de estado
         tempBuffAns [ 0 ] = tempByte;
           
         // Envia respuesta
         SendAnsComplex ( COMMAND_CASHBOX, ANS_OK, tempBuffAns, 1 );
           
         flagCashBox = false;
     
         continue;
      }
        
      // Valida estado escritura EEPROM
      if ( statusNew == STATUS_WRITEEEPROM )
      {
         // Escribe el dato en la EEPROM
         WRITE_EEPROM ( addressEEprom, byteEEprom );        
           
         // Comando terminado procesar
         flagNewCommand = FALSE;        

         // Envia respuesta
         SendAns ( commandRx [ 2 ], ANS_OK );     
           
         // Liberar para nuevo proceso
         statusNew = STATUS_FREE;
     
         continue;
      }       
      
      // Valida estado lectura EEPROM
      if ( statusNew == STATUS_READEEPROM )
      {
         // cilo para leer datos de la EEPROM         
         for ( pos = 0; pos < byteEEprom; pos ++ )
         {         
            tempBuffAns [ pos ] = READ_EEPROM ( addressEEprom + pos );            
         }
         
         // Envia respuesta
         SendAnsComplex ( COMMAND_EEPROM_READ, ANS_OK, tempBuffAns, byteEEprom );
           
         // Comando terminado procesar
         flagNewCommand = FALSE;          
           
         // Liberar para nuevo proceso
         statusNew = STATUS_FREE;
     
         continue;
      }   
      
      // Valida estado nuevo token
      if ( statusNew == STATUS_SETTOKEN )
      {                           
         // check token data
         tempStatus = ValidateToken ( ADDRESS_KEYTOKEN );         
         
         // Valida si debe incrementar contador errores
         if ( tempStatus == TRUE )
         {
            failToken = 0;
         }
         else
         {                  
            // Incrementa contador de errores
            if ( failToken < 250 )
            {
               failAuth ++;
            }              
         }
         
         // Check ans
         if ( tempStatus == ERROR_SIGNATURE )
         {               
            // Envia respuesta
            SendAns ( commandRx [ 2 ], ANS_ERROR_TOKEN );
         }
         else
         {
            if ( tempStatus == ERROR_VALUE )
            {                  
               // Envia respuesta
               SendAns ( commandRx [ 2 ], ANS_ERROR_TIME );
            }
            else
            {                 
               // Envia respuesta
               SendAns ( commandRx [ 2 ], ANS_OK );                              
            }         
         }                   
         
         // Comando terminado procesar
         flagNewCommand = FALSE;  
         
         // Liberar para nuevo proceso
         statusNew = STATUS_FREE;
     
         continue;
      }
      
      // Valida proceso de formatear bloque
      if ( statusNew == STATUS_BLOCKFORMAT )
      {      
         statusNow = STATUS_BLOCKFORMAT;
         statusNew = STATUS_FREE;
         
          // Obtiene los datos del bloque actual
         ReadEEprom ( tempEEprom, indexBlock * BLOCK_LENGTH, BLOCK_LENGTH );
           
         // Obtiene las condiciones de acceso
         tempAccess = tempEEprom [ 0 ];
        
         // Valida si es comando version 2
         if ( versionOrder == VERSION_2 )
         {        
            // Valida informacion del token
            tempStatus = ValidateToken ( indexBlock * BLOCK_LENGTH );
            
            // Valida si el bloque fue inicializado
            if ( tempAccess != 0xFF )
            {               
               if ( tempStatus != TRUE )
               {
                  // Error validando token
                  flagNewCommand = FALSE;
                  
                  // Check ans
                  if ( tempStatus == ERROR_SIGNATURE )
                  {               
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TOKEN );
                  }
         
                  if ( tempStatus == ERROR_VALUE )
                  {                  
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TIME );
                  }                                                                  
                    
                  statusNew = STATUS_FREE;
                    
                  continue;
               }                                                                        
            }            
            
            // Convierte a numero la llave random            
            tempNum = valueKey [ 0 ] << 24;
            tempData2 = valueKey [ 1 ] << 16;
            tempNum += tempData2;
            tempData2 = valueKey [ 2 ] << 8;
            tempNum += tempData2;
            tempData2 = valueKey [ 3 ];
            tempNum += tempData2;  
            
            // Retira mascara a la llave recibida
            tempNum -= randomValue;     
            
            // Convierte a byte el valor
            valueKey [ 0 ] = tempNum >> 24;
            valueKey [ 1 ] = tempNum >> 16;
            valueKey [ 2 ] = tempNum >> 8;
            valueKey [ 3 ] = tempNum;
            
#ifdef DEBUG
// llave autenticacion recibida
SendByte ( 0xDE );
SendByte ( 0xD7 );
SendByte ( valueKey [ 0] );
SendByte ( valueKey [ 1] );
SendByte ( valueKey [ 2] );
SendByte ( valueKey [ 3] );

// Llave token recibida
SendByte ( 0xDE );
SendByte ( 0xD8 );
SendByte ( externalKey [ 0] );
SendByte ( externalKey [ 1] );
SendByte ( externalKey [ 2] );
SendByte ( externalKey [ 3] );

#endif            
            
            memcpy ( externalKey, valueKey, 4 );
         }           
           
         // Valida si tiene permiso de inicializar el bloque
         if ( ( tempAccess & PERMISSION_FORMAT ) == 0 )
         {
            // Debe validar si puede cambiar condiciones con autenticacion
            
            
#ifdef DEBUG
// l;l;ave EEPROM leida
SendByte ( 0xDE );
SendByte ( 0xD4 );
SendByte ( tempEEprom [ 0] );
SendByte ( tempEEprom [ 1] );
SendByte ( tempEEprom [ 2] );
SendByte ( tempEEprom [ 3] );
SendByte ( tempEEprom [ 4] );

// Llave recibida
SendByte ( 0xDE );
SendByte ( 0xD5 );
SendByte ( externalKey [ 0] );
SendByte ( externalKey [ 1] );
SendByte ( externalKey [ 2] );
SendByte ( externalKey [ 3] );
SendByte ( externalKey [ 4] );

#endif            
              
            // Compara la llave recibida con la actual
            tempStatus = CompareBuffer ( tempEEprom + 1, externalKey, 4 );
              
            // Valida si coinciden
            if ( tempStatus != TRUE )
            {
               flagNewCommand = FALSE;
               SendAns ( commandRx [ 2 ], ANS_ERROR_KEY );        
                 
               // Incrementa contador de errores autenticacion
               if ( failAuth < 250 )
               {
                  failAuth ++;
               }
                 
               continue;
            }
            
            failAuth = 0;
              
            // Asigna las nuevas condiciones de acceso
            tempEEprom [ 0 ] = accessBlock;
              
            // Graba las nuevas condiciones
            WriteEEprom ( tempEEprom, indexBlock * BLOCK_LENGTH, 1 );
         }        
         else
         {           
            // Escribe la nueva llave y condiciones de acceso     
            tempEEprom [ 0 ] = accessBlock;
            memcpy ( tempEEprom + 1, valueKey, 4 );
              
            // Graba las nuevas condiciones
            WriteEEprom ( tempEEprom, indexBlock * BLOCK_LENGTH, BLOCK_LENGTH );              
         }
           
         // Lee el byte de acceso del bloque seleccionado                                   
         flagNewCommand = FALSE;
         SendAns ( commandRx [ 2 ], ANS_OK );     
         continue;
      }    
      
      // Valida proceso cambiar llave
      if ( statusNew == STATUS_CHANGE_KEY )
      {
         statusNow = STATUS_CHANGE_KEY;
         statusNew = STATUS_FREE;                    
          
         // Obtiene los datos del bloque actual
         ReadEEprom ( tempEEprom, indexBlock * BLOCK_LENGTH, BLOCK_LENGTH );
           
         // Obtiene las condiciones de acceso
         tempAccess = tempEEprom [ 0 ];           
           
         // Valida si tiene permiso de escritura sin autenticacion
         if ( ( tempAccess & PERMISSION_WR ) == 0 )
         {             
            // Graba la nueva llave
            WriteEEprom ( valueKey, ( indexBlock * BLOCK_LENGTH ) + 1, 
                     BLOCK_LENGTH - 1 );
                     
            // Comando procesado
            flagNewCommand = FALSE;
            statusNew = STATUS_FREE;
            SendAns ( commandRx [ 2 ], ANS_OK );  
            continue;
         }
         
         // Valida si tiene permiso de escritura con autenticacion
         if ( ( tempAccess & PERMISSION_WRAUTH ) == 0 )
         {                     
            // Valida informacion del token
            tempStatus = ValidateToken ( indexBlock * BLOCK_LENGTH );
            if ( tempStatus != TRUE )
            {
               // Error validando token
               flagNewCommand = FALSE;
               
               // Check ans
               if ( tempStatus == ERROR_SIGNATURE )
               {               
                  // Envia respuesta
                  SendAns ( commandRx [ 2 ], ANS_ERROR_TOKEN );
               }
            
               if ( tempStatus == ERROR_VALUE )
               {                  
                  // Envia respuesta
                  SendAns ( commandRx [ 2 ], ANS_ERROR_TIME );
               }                                                                  
                 
               statusNew = STATUS_FREE;
               
               // Incrementa contador de errores autenticacion
               if ( failAuth < 250 )
               {
                  failAuth ++;
               }                                                         
                 
               continue;
            }    
            
            failAuth = 0;
            
            // Convierte a numero la llave random            
            tempNum = valueKey [ 0 ] << 24;
            tempData2 = valueKey [ 1 ] << 16;
            tempNum += tempData2;
            tempData2 = valueKey [ 2 ] << 8;
            tempNum += tempData2;
            tempData2 = valueKey [ 3 ];
            tempNum += tempData2;  
            
            // Retira mascara a la llave recibida
            tempNum -= randomValue;                    
            
            // Convierte a byte el valor
            tempNewKey [ 0 ] = tempNum >> 24; 
            tempNewKey [ 1 ] = tempNum >> 16;
            tempNewKey [ 2 ] = tempNum >> 8;
            tempNewKey [ 3 ] = tempNum;                        
              
            // Graba las nueva llave
            WriteEEprom ( tempNewKey, ( indexBlock * BLOCK_LENGTH ) + 1,
                     BLOCK_LENGTH - 1 );
                     
            // Comando procesado
            flagNewCommand = FALSE;
            statusNew = STATUS_FREE;
            SendAns ( commandRx [ 2 ], ANS_OK );  
            continue;
         }        
         else
         {           
            // Escribe la nueva llave y condiciones de acceso     
            tempEEprom [ 0 ] = accessBlock;
            memcpy ( tempEEprom + 1, valueKey, 4 );
              
            // Graba las nuevas condiciones
            WriteEEprom ( tempEEprom, indexBlock * BLOCK_LENGTH, BLOCK_LENGTH );              
         }
           
         // Las condiciones de acceso no son las correctas                                 
         flagNewCommand = FALSE;
         statusNew = STATUS_FREE;
         SendAns ( commandRx [ 2 ], ANS_ERROR_CONDITION );     
         continue;
      }    
        
      // Valida proceso de abrir cerradura
      if ( statusNew == STATUS_OPEN )
      {
         statusNow = STATUS_OPEN;                       
           
         // Valida paso de autenticacion
         if ( stepNow == STEP_AUTHENTICATE )                 
         {          
            // Valida si es comando version 2
            if ( versionOrder == VERSION_2 )
            {         
               // Valida informacion del token
               tempStatus = ValidateToken ( ADDRESS_KEYTOKEN );
               if ( tempStatus != TRUE )
               {
                  // Error validando token
                  flagNewCommand = FALSE;
                  
                  // Check ans
                  if ( tempStatus == ERROR_SIGNATURE )
                  {               
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TOKEN );
                  }
         
                  if ( tempStatus == ERROR_VALUE )
                  {                  
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TIME );
                  }                                                                  
                    
                  statusNew = STATUS_FREE;
                    
                  continue;
               }                            
               
               // Convierte a numero la llave random            
               tempNum = valueKey [ 0 ] << 24;
               tempData2 = valueKey [ 1 ] << 16;
               tempNum += tempData2;
               tempData2 = valueKey [ 2 ] << 8;
               tempNum += tempData2;
               tempData2 = valueKey [ 3 ];
               tempNum += tempData2; 
               
               // Retira mascara a la llave recibida
               tempNum -= randomValue;                    
               
               // Convierte a byte el valor
               externalKey [ 0 ] = tempNum >> 24;
               externalKey [ 1 ] = tempNum >> 16;
               externalKey [ 2 ] = tempNum >> 8;
               externalKey [ 3 ] = tempNum;                                  
            }
            
            // Obtiene los datos del bloque que contiene la llave de la cerradura
            ReadEEprom ( tempEEprom, ( selectLock - 1 ) * BLOCK_LENGTH, BLOCK_LENGTH );
              
            // Obtiene las condiciones de acceso
            tempAccess = tempEEprom [ 0 ];           
              
            // Valida si el bloque esta inicializado
            if ( ( ( tempAccess & PERMISSION_FORMAT ) != 0 ) || 
                  ( ( tempAccess & PERMISSION_AUTH ) != 0 ) )
            {
               // Error no hay condiciones de acceso suficientes
               flagNewCommand = FALSE;
               SendAns ( commandRx [ 2 ], ANS_ERROR_CONDITION );    
                 
               statusNew = STATUS_FREE;
                 
               continue;
            }
              
            // Compara la llave recibida con la actual
            tempStatus = CompareBuffer ( tempEEprom + 1, externalKey, 4 );
                 
            // Valida si coinciden
            if ( tempStatus != TRUE )
            {
               flagNewCommand = FALSE;
               SendAns ( commandRx [ 2 ], ANS_ERROR_KEY );        
                 
               // Incremnta contador de errores autenticacion
               if ( failAuth < 250 )
               {
                  failAuth ++;
               }
                 
               statusNew = STATUS_FREE;
                    
               continue;
            }   
            
            failAuth = 0;
              
            // Lee el byte de acceso del bloque seleccionado                                   
            flagNewCommand = FALSE;
            SendAns ( commandRx [ 2 ], ANS_OK );     
              
            // Pasa a iniciar pulso
            stepNow = STEP_UNLOCK_START;
            continue;        
         }
                    
         // Valida paso iniciar pulso
         if ( stepNow == STEP_UNLOCK_START )
         {
            // Valida Id cerradura 1 tipo A
            if ( selectLock == 1 ) 
            {
               // Coloca senal a bajo
               output_low ( PIN_1_IA );
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_PULSELOCK;
            }
              
            // Valida Id cerradura 2 tipo A
            if ( selectLock == 2 ) 
            {
               // Coloca senal a bajo
               output_low ( PIN_2_IA );
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_PULSELOCK;
            }
              
            // Valida Id cerradura 3 tipo B
            if ( selectLock == 3 ) 
            {
               // Coloca senal a alto
               output_high ( PIN_3_IA );           
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_SOLENOIDE;
            }                          
              
            // Pasa a esperar pulso
            stepNow = STEP_UNLOCK_PULSE;
            continue;     
         }
           
         // Valida si esta esperando fin del pulso
         if ( stepNow == STEP_UNLOCK_PULSE )
         {
            // Valida si ya termino el timer del pulso
            if ( timerPulse == 0 )
            {
               // Valida Id cerradura 1 tipo A
               if ( selectLock == 1 ) 
               {
                  // Desactiva senal 
                  output_high ( PIN_1_IA );
               }
                 
               // Valida Id cerradura 2 tipo A
               if ( selectLock == 2 ) 
               {
                  // Desactiva senal 
                  output_high ( PIN_2_IA );
               }
                 
               // Valida Id cerradura 3 tipo A
               if ( selectLock == 3 ) 
               {
                  // Desactiva senal 
                  output_low ( PIN_3_IA );
               }
                 
               // Termina proceso
               statusNew = STATUS_FREE;
               stepNow = STEP_START;
            }
              
            continue;
         }                                      
      }    
        
      // Valida proceso de bloquear cerradura
      if ( statusNew == STATUS_LOCK )
      {
         statusNow = STATUS_LOCK;     
           
         // Valida paso de autenticacion
         if ( stepNow == STEP_AUTHENTICATE )
         {             
            // Valida si es comando version 2
            if ( versionOrder == VERSION_2 )
            {         
               // Valida informacion del token
               tempStatus = ValidateToken ( ADDRESS_KEYTOKEN );
               if ( tempStatus != TRUE )
               {
                  // Error validando token
                  flagNewCommand = FALSE;
                  
                  // Check ans
                  if ( tempStatus == ERROR_SIGNATURE )
                  {               
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TOKEN );
                  }
         
                  if ( tempStatus == ERROR_VALUE )
                  {                  
                     // Envia respuesta
                     SendAns ( commandRx [ 2 ], ANS_ERROR_TIME );
                  }                                                                  
                    
                  statusNew = STATUS_FREE;
                    
                  continue;
               }                            
               
               // Convierte a numero la llave random            
               tempNum = valueKey [ 0 ] << 24;
               tempData2 = valueKey [ 1 ] << 16;
               tempNum += tempData2;
               tempData2 = valueKey [ 2 ] << 8;
               tempNum += tempData2;
               tempData2 = valueKey [ 3 ];
               tempNum += tempData2;
               
               // Retira mascara a la llave recibida
               tempNum -= randomValue;                    
               
               // Convierte a byte el valor
               externalKey [ 0 ] = tempNum >> 24;
               externalKey [ 1 ] = tempNum >> 16;
               externalKey [ 2 ] = tempNum >> 8;
               externalKey [ 3 ] = tempNum;                                  
            }
            
            // Obtiene los datos del bloque que contiene la llave de la cerradura
            ReadEEprom ( tempEEprom, ( selectLock - 1 ) * BLOCK_LENGTH, BLOCK_LENGTH );
              
            // Obtiene las condiciones de acceso
            tempAccess = tempEEprom [ 0 ];           
              
            // Valida si el bloque esta inicializado
            if ( ( ( tempAccess & PERMISSION_FORMAT ) != 0 ) || 
                  ( ( tempAccess & PERMISSION_AUTH ) != 0 ) )
            {
               // Error no hay condiciones de acceso suficientes
               flagNewCommand = FALSE;
               SendAns ( commandRx [ 2 ], ANS_ERROR_CONDITION );    
                 
               statusNew = STATUS_FREE;
                 
               continue;
            }                 
              
            // Compara la llave recibida con la actual
            tempStatus = CompareBuffer ( tempEEprom + 1, externalKey, 4 );
                 
            // Valida si coinciden
            if ( tempStatus != TRUE )
            {
               flagNewCommand = FALSE;
               SendAns ( commandRx [ 2 ], ANS_ERROR_KEY );    
                 
               // Incremnta contador de errores autenticacion
               if ( failAuth < 250 )
               {
                  failAuth ++;
               }
                 
               statusNew = STATUS_FREE;
                    
               continue;
            }    
            
            failAuth = 0;
              
            // Lee el byte de acceso del bloque seleccionado                                   
            flagNewCommand = FALSE;
            SendAns ( commandRx [ 2 ], ANS_OK );     
              
            // Pasa a iniciar pulso
            stepNow = STEP_LOCK_START;
            continue;        
         }
           
         // Valida paso iniciar pulso
         if ( stepNow == STEP_LOCK_START )
         {
            // Valida Id cerradura 1 tipo A
            if ( selectLock == 1 ) 
            {              
               // Coloca senal a bajo
               output_low ( PIN_1_IB );
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_PULSELOCK;
            }
              
            // Valida Id cerradura 2 tipo A
            if ( selectLock == 2 ) 
            {              
               // Coloca senal a bajo
               output_low ( PIN_2_IB );
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_PULSELOCK;
            }
              
            // Valida Id cerradura 3 tipo A
            if ( selectLock == 3 ) 
            {              
               // Coloca senal a alto
               output_high ( PIN_3_IA );
                 
               // Inicia timer para pulso
               timerPulse = TIMEOUT_SOLENOIDE;
            }
                 
            // Pasa a esperar pulso
            stepNow = STEP_LOCK_PULSE;
            continue;     
         }
           
         // Valida si esta esperando fin del pulso
         if ( stepNow == STEP_LOCK_PULSE )
         {
            // Valida si ya termino el timer del pulso
            if ( timerPulse == 0 )
            {
               // Valida Id cerradura 1 tipo A
               if ( selectLock == 1 ) 
               {
                  // Desactiva senal 
                  output_high ( PIN_1_IB );
               }
                 
               // Valida Id cerradura 2 tipo A
               if ( selectLock == 2 ) 
               {
                  // Desactiva senal 
                  output_high ( PIN_2_IB );
               }
                 
               // Valida Id cerradura 3 tipo A
               if ( selectLock == 3 ) 
               {
                  // Desactiva senal 
                  output_low ( PIN_3_IA );
               }
                 
               // Termina proceso
               statusNew = STATUS_FREE;
               stepNow = STEP_START;
            }
              
            continue;
         }                                      
      }
   }             
}

/*-----------------------------------------------------------------------------
 Lee un segmento de la memoria EEPROM
 @param tempPtr Puntero donde dejara los datos leidos
 @param tempAddress Direcion EEPROM desde donde leer
 @parm tempSize Numero de bytes a leer
 @return Ninguno
------------------------------------------------------------------------------*/
void ReadEEprom ( int8 * tempPtr, int8 tempAddress, int8 tempSize )
{
   int8 pos;
   int8 tempByte;
     
   // Ciclo para leer los datos
   for ( pos = 0; pos < tempSize; pos ++ )
   {
      tempByte = READ_EEPROM ( tempAddress + pos );
     
      // Lo guarda en el buffer de respuesta
      tempPtr [ pos ] = tempByte;
   }
}

/*-----------------------------------------------------------------------------
Envia uan respueta simple
@param tempCommand Comando al que se da respuesta
@param tempAns Estado de la respuesta
@return Nada
-----------------------------------------------------------------------------*/
void SendAns ( int8 tempCommand, int8 tempAns )
{
   int8 pos;
     
   // Estructura el frame a enviar
   bufferTx [ 0 ] = HEAD_BYTE;
   bufferTx [ 1 ] = 0x03;
   bufferTx [ 2 ] = tempCommand;
   bufferTx [ 3 ] = tempAns;
   bufferTx [ 4 ] = CalcCRC ( bufferTx );
     
   // Ciclo para enviar todos los bytes     
   for ( pos = 0; pos <= 4; pos++ )
   {
      // Envia el byte
      SendByte ( bufferTx [ pos ] );    
   }
}

/*-----------------------------------------------------------------------------
Envia uan respueta de con datos
@param tempCommand Comando al que se da respuesta
@param tempAns Estado de la respuesta
@param tempPonter Puntero al buffer con los datos a enviar
@param tempLength Numero de datos del buffer a enviar
@return Nada
-----------------------------------------------------------------------------*/
void SendAnsComplex ( int8 tempCommand, int8 tempAns, int8 *tempPointer, 
                     int8 tempLength )
{
   int8 pos;     
     
   // Estructura el frame a enviar
   bufferTx [ 0 ] = HEAD_BYTE;
   bufferTx [ 1 ] = 0x03 + tempLength;
   bufferTx [ 2 ] = tempCommand;
   bufferTx [ 3 ] = tempAns;         
     
   // Copia datos buffer
   memcpy ( bufferTx + 4, tempPointer, tempLength );     
     
   // Calcula el CRC
   bufferTx [ tempLength + 4 ] = CalcCRC ( bufferTx );
     
   // Calcula total de bytes  a enviar
   tempLength = tempLength + 5;          
     
   // Ciclo para enviar todos los bytes     
   for ( pos = 0; pos < tempLength; pos++ )
   {
      // Envia cada byte
      SendByte ( bufferTx [ pos ] );    
   }
}

/*-----------------------------------------------------------------------------
Envia un byte por el puerto USART
@param tempData Dato a enviar
@return Nada
-----------------------------------------------------------------------------*/
void SendByte ( int8 tempData )
{
   // Envia el dato por el puerto serial
   putc ( tempData );
   
   // Retardo entre bytes
   delay_ms ( 2 );
}

/*-----------------------------------------------------------------------------
Espera x minutos
@param tempMinute cantiad de minutos a esperar
@return Nada
-----------------------------------------------------------------------------*/
void SleepMinute ( int8 tempMinute )
{     
   int8 tempSecond;
     
   tempSecond = 60;
     
   // Ciclo para contar segundos
   while ( ( tempMinute > 0 ) || ( tempSecond > 0 ) )
   {        
      // Asigna timer para 1 segundo
      timerMain2 = 100;
        
      // Espera un segundo
      while ( timerMain2 > 0 )
      {           
      }
        
      // Decrementa contador de segundos        
      tempSecond --;        
        
      // Valida si cumplio 1 minuto
      if ( tempSecond == 0 )
      {
         // Decrementa minutos
         tempMinute --;           
           
         // Valida si termino temporizador
         if ( tempMinute > 0 )
         {
            tempSecond = 60;
         }
      }        
   }          
}

/*-----------------------------------------------------------------------------
Verifica que un comando recibido cumpla con la definicion del protocolo.
El comando debe estar ya en bufferRx
@param Ninguno
@return Nada
-----------------------------------------------------------------------------*/
int8 ValidateCommand ( void )
{
   int8 tempByte;     
     
   // Calcula el CRC
   tempByte = CalcCRC ( commandRx );

   // Valida el CRC del comando recibido con el calculado
   if ( tempByte != commandRx [ ( commandRx [ 1 ] + 1 ) ] )
   {
      return ANS_ERROR_CRC;
   }

   // Segun el comando valida los datos recibidos
   switch ( commandRx [ 2 ] )
   {             
      // Comando desbloqueo cerradura
      case COMMAND_OPEN:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 9 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Valida los posibles valores
         if ( ( commandRx [ 3 ] > 0x00 ) && ( commandRx [ 3 ] < 0x04 ) )
         {
            // Obtiene el ID de cerradura a abrir
            selectLock = commandRx [ 3 ] ;           
           
            // Copia nuevo valor de llave
            memcpy ( externalKey, commandRx + 4, 4 );     
              
            // Cambia a nuevo estado
            statusNew = STATUS_OPEN;    
            stepNow = STEP_AUTHENTICATE;
            versionOrder = VERSION_1;
              
            // Comando puede ser procesado
             return ANS_OK;
         }
         else
         {   // Error valor fuera de rango
            return ANS_ERROR_VALUE;
         }
      }
      break;
        
      // Comando bloqueo cerradura
      case COMMAND_LOCK:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 9 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Valida los posibles valores
         if ( ( commandRx [ 3 ] > 0x00 ) && ( commandRx [ 3 ] < 0x04 ) )
         {
            // Obtiene el ID de cerradura a abrir
            selectLock = commandRx [ 3 ] ;           
           
            // Copia nuevo valor de llave
            memcpy ( externalKey, commandRx + 4, 4 );   
              
            // Cambia a nuevo estado
            statusNew = STATUS_LOCK;     
            stepNow = STEP_AUTHENTICATE;
            versionOrder = VERSION_1;
              
            // Comando puede ser procesado
            return ANS_OK;
         }
         else
         {   // Error valor fuera de rango
            return ANS_ERROR_VALUE;
         }
      }
      break;
      
      // Comando desbloqueo cerradura version 2
      case COMMAND_OPENV2:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 19 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Valida los posibles valores
         if ( ( commandRx [ 3 ] > 0x00 ) && ( commandRx [ 3 ] < 0x04 ) )
         {
            // Obtiene el ID de cerradura a abrir
            selectLock = commandRx [ 3 ] ;           
           
            // Copia nuevo valor de llave
            memcpy ( valueKey, commandRx + 4, 4 );
            
            // Copia bytes token del comando
            memcpy ( tokenCheck, commandRx + 8, 10 );
              
            // Cambia a nuevo estado
            statusNew = STATUS_OPEN;    
            stepNow = STEP_AUTHENTICATE;
            versionOrder = VERSION_2;
              
            // Comando puede ser procesado
             return ANS_OK;
         }
         else
         {   // Error valor fuera de rango
            return ANS_ERROR_VALUE;
         }
      }
      break;
      
       // Comando desbloqueo cerradura version 2
      case COMMAND_LOCKV2:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 19 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Valida los posibles valores
         if ( ( commandRx [ 3 ] > 0x00 ) && ( commandRx [ 3 ] < 0x04 ) )
         {
            // Obtiene el ID de cerradura a abrir
            selectLock = commandRx [ 3 ] ;           
           
            // Copia nuevo valor de llave
            memcpy ( valueKey, commandRx + 4, 4 );
            
            // Copia bytes token del comando
            memcpy ( tokenCheck, commandRx + 8, 10 );
              
            // Cambia a nuevo estado
            statusNew = STATUS_LOCK;    
            stepNow = STEP_AUTHENTICATE;
            versionOrder = VERSION_2;
              
            // Comando puede ser procesado
             return ANS_OK;
         }
         else
         {   // Error valor fuera de rango
            return ANS_ERROR_VALUE;
         }
      }
      break;
        
      // Comando estado sensores
      case COMMAND_STATUS:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 5 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Valida los posibles valores
         if ( ( commandRx [ 3 ] > 0x00 ) && ( commandRx [ 3 ] < 0x04 ) )
         {
            // ACtiva lectura de sensor
            selectSensor = commandRx [ 3 ] ;     
            flagReadSensor = true;
              
            // Comando puede ser procesado
            return ANS_OK;
         }
         else
         {   // Error valor fuera de rango
            return ANS_ERROR_VALUE;
         }
      }
      break;        
        
      // Comando escritura en la EEPROM
      case COMMAND_EEPROM_WRITE:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 6 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Direccion a escribir
         addressEEprom = commandRx [ 3 ];
           
         // VAlro a escribir
         byteEEprom = commandRx [ 4 ];
           
         // Asigna estado escritura EEPROM
         statusNew = STATUS_WRITEEEPROM;           
           
         return ANS_OK;
      }
      break;
      
      // Comando escritura token en EEPROM
      case COMMAND_SETTOKEN:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 14 )
         {
            return ANS_ERROR_LENGTH;
         }
         
         // copia buffer token para proceso
         memcpy ( tokenCheck, commandRx + 3, 10 );         
           
         // Asigna estado nuevo token
         statusNew = STATUS_SETTOKEN;  
           
         return ANS_OK;
      }
      break;

      // Comando lectura EEPROM
      case COMMAND_EEPROM_READ:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 6 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Direccion iniciaal a leer
         addressEEprom = commandRx [ 3 ];
           
         // Numero de bytes a leer
         byteEEprom = commandRx [ 4 ];
           
         // Asigna estado escritura EEPROM
         statusNew = STATUS_READEEPROM;           
           
         return ANS_OK;
      }
      break;
        
      // Comando formatear bloque EEPROM
      case COMMAND_EEPROM_FORMAT:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 10 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // valida rango de bloque
         if ( commandRx [ 3 ] > 7 )
         {
            return ANS_ERROR_VALUE;
         }
           
         // Obtiene el indice del bloque
         indexBlock = commandRx [ 3 ] ;
           
         // Obtiene las condiciones de acceso
         accessBlock  = commandRx [ 4 ] ;
           
         // Copia nuevo valor de llave
         memcpy ( valueKey, commandRx + 5, 4 );
         memcpy ( externalKey, commandRx + 5, 4 );
           
         // Cambia a nuevo estado
         statusNew = STATUS_BLOCKFORMAT;           
           
         return ANS_OK;
      }
      break;
      
      // Comando formatear V2
      case COMMAND_EEPROM_FORMAT2:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 20 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // valida rango de bloque
         if ( commandRx [ 3 ] > 7 )
         {
            return ANS_ERROR_VALUE;
         }
           
         // Obtiene el indice del bloque
         indexBlock = commandRx [ 3 ] ;
           
         // Obtiene las condiciones de acceso
         accessBlock  = commandRx [ 4 ] ;
           
         // Copia nuevo valor de llave
         memcpy ( valueKey, commandRx + 5, 4 );
         memcpy ( externalKey, commandRx + 5, 4 );
         
         // Copia bytes token del comando
         memcpy ( tokenCheck, commandRx + 9, 10 );
           
         // Cambia a nuevo estado
         statusNew = STATUS_BLOCKFORMAT;  
         versionOrder = VERSION_2;
           
         return ANS_OK;
      }
      break;
      
       // Comando cambiar llave
      case COMMAND_CHANGE_KEY:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 19 )
         {
            return ANS_ERROR_LENGTH;
         }    
         
         // valida rango de bloque
         if ( commandRx [ 3 ] > 7 )
         {
            return ANS_ERROR_VALUE;
         }
         
         // Obtiene el indice del bloque
         indexBlock = commandRx [ 3 ] ;
           
         // Copia nuevo valor de llave
         memcpy ( valueKey, commandRx + 4, 4 );
         
         // Copia bytes token del comando
         memcpy ( tokenCheck, commandRx + 8, 10 );
         
         // Cambia a nuevo estado
         statusNew = STATUS_CHANGE_KEY;
           
         return ANS_OK;
      }
      break;
        
      // Comando lectura version
      case COMMAND_VERSION:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 4 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         flagGetVersion = true;        
           
         return ANS_OK;
      }
      break;
      
      // Comando poll
      case COMMAND_POLL:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 4 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         flagPoll = true;        
           
         return ANS_OK;
      }
      break;
        
      // Comando buzzer
      case COMMAND_BUZZER:
      {
         // Valida numero de datos necesarios
         if ( commandSize != 5 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         // Obtiene el tiempo de encendido
         timeBuzzer = commandRx [ 3 ];
           
         statusNew = STATUS_BUZZER;        
         stepNow = STEP_START;
           
         return ANS_OK;
      }
      break;
        
      // Comando estado cashbox
      case COMMAND_CASHBOX:
      {        
         // Valida numero de datos necesarios
         if ( commandSize != 4 )
         {
            return ANS_ERROR_LENGTH;
         }
           
         flagCashBox = true;    
           
         return ANS_OK;
      }
      break;     
        
      // comando no definido
      default:
      {
         return ANS_ERROR_UNKNOWN;
      }
   }
}

/*-----------------------------------------------------------------------------
Verifica un paquete de token
En tokenCheck debe estar los datos del token a validar
Si se obtinee un proceso exitoso en randomValue
@param Ninguno
@return 
-----------------------------------------------------------------------------*/
int8 ValidateToken ( unsigned int8 * tempKeyToken )
{
   int8 tempByte;     
   unsigned int8 tempSh;
   unsigned int8 tempSl;
   unsigned int32 tempData;
   unsigned int32 tempCurrentTime;
   unsigned int32 tempKey;
   unsigned char tempBuffer [ 10 ];
   unsigned char tempBufferRand [ 4 ];
   unsigned char tempBufferData [ 4 ];
   unsigned char tempBufferTime [ 5 ];
   unsigned int32 tempData2;
   
   // Get S bytes
   tempSh = tokenCheck [ 0 ];
   tempSl = tokenCheck [ 9 ];
   
   // Get random data offset 0 is lower byte
   tempBufferRand [ 0 ] = tokenCheck [ 1 ];
   tempBufferRand [ 1 ] = tokenCheck [ 3 ];
   tempBufferRand [ 2 ] = tokenCheck [ 5 ];
   tempBufferRand [ 3 ] = tokenCheck [ 7 ];
   
   randomValue = tempBufferRand [ 3 ] << 24;
   tempData2 = tempBufferRand [ 2 ] << 16;
   randomValue += tempData2;
   tempData2 = tempBufferRand [ 1 ] << 8;
   randomValue += tempData2;
   tempData2 = tempBufferRand [ 0 ];
   randomValue += tempData2;  
   
   // Get data bytes ofset 0 is lower byte
   tempBufferData [ 0 ] = tokenCheck [ 6 ] - tempBufferRand [ 3 ];
   tempBufferData [ 1 ] = tokenCheck [ 2 ] - tempBufferRand [ 2 ];
   tempBufferData [ 2 ] = tokenCheck [ 8 ] - tempBufferRand [ 1 ];
   tempBufferData [ 3 ] = tokenCheck [ 4 ] - tempBufferRand [ 0 ];   
     
   // Get key token 0 is Highest byte
   ReadEEprom ( tempBuffer,  tempKeyToken + 1, 4 );
   tempKey = tempBuffer [ 0 ] << 24;
   tempData2 = tempBuffer [ 1 ] << 16;
   tempKey += tempData2;
   tempData2 = tempBuffer [ 2 ] << 8;
   tempKey += tempData2;
   tempData2 = tempBuffer [ 3 ];
   tempKey += tempData2; 
   
   // Copy external key calculate
   memcpy ( externalKey, tempKey, 4 );   
   
   // Check SH
   tempByte = tempBufferRand [ 2 ] + tempBufferData [ 0 ] + tempBuffer [ 0 ] +
               tempBuffer [ 1 ];
   if ( tempByte != tempSh )
   {
      return ERROR_SIGNATURE;
   }
   
   // Check SL
   tempByte = tempBufferRand [ 1 ] + tempBufferData [ 0 ] + tempBuffer [ 2 ] +
               tempBuffer [ 3 ];
   if ( tempByte != tempSl )
   {
      return ERROR_SIGNATURE;
   }
   
   // inverter each byte
   tempBufferData [ 0 ] = ~ ( tempBufferData [ 0 ] );
   tempBufferData [ 1 ] = ~ ( tempBufferData [ 1 ] );
   tempBufferData [ 2 ] = ~ ( tempBufferData [ 2 ] );
   tempBufferData [ 3 ] = ~ ( tempBufferData [ 3 ] );
   
   // assemble data negative value, highest byte is offset 0
   tempData = tempBufferData [ 3 ] << 24;
   tempData2 = tempBufferData [ 2 ] << 16;
   tempData += tempData2;
   tempData2 = tempBufferData [ 1 ] << 8;
   tempData += tempData2;
   tempData2 = tempBufferData [ 0 ];
   tempData += tempData2;    
   
   // Add key to data to get real time
   tempData += tempKey;
  
   // Lee el bloque tiempo token
   ReadEEprom ( tempBuffer, ADDRESS_TIMETOKEN, 5 );
   
   // Valida estado bandera bloqueo token
   if ( tempBuffer [ 0 ] == 0x00 )
   {
      // Debe convertir buffer de tiempo a un numero      
      tempCurrentTime = tempBuffer [ 1 ] << 24;
      tempData2 = tempBuffer [ 2 ] << 16;
      tempCurrentTime += tempData2;
      tempData2 = tempBuffer [ 3 ] << 8;
      tempCurrentTime += tempData2;
      tempData2 = tempBuffer [ 4 ];
      tempCurrentTime += tempData2;  
      
      // Valida si el nuevo tiempo es mayor al actual          
      if ( tempCurrentTime >= tempData )
      {                  
         // Tiempo nuevo no es mayor al actual
         return ERROR_VALUE;
      }
   }         
   
   // Convert time to array, offset 0 is highest byte
   tempBufferTime [ 0 ] = 0x00;
   tempBufferTime [ 1 ] = ( tempData & 0xFF000000 ) >> 24;
   tempBufferTime [ 2 ] = ( tempData & 0x00FF0000 ) >> 16;
   tempBufferTime [ 3 ] = ( tempData & 0x0000FF00 ) >> 8;
   tempBufferTime [ 4 ] = tempData & 0x000000FF;
   
   // Update new time
   WriteEEprom ( tempBufferTime, ADDRESS_TIMETOKEN, 5 );
   
   return TRUE;
}

/*-----------------------------------------------------------------------------
Escribe datos en la EEPROM
@param tempPtr Puntero al buffer con los datos
@param tempSize Numero de datos a escribir
@param tempAddress Direccion a donde escibir en la EEPROM
@return Nada
-----------------------------------------------------------------------------*/
void WriteEEprom ( int8 * tempPtr, int8 tempAddress, int8 tempSize )
{
   int8 pos;
     
   // Ciclo para escribir datos en la EEPROM
   for ( pos = 0; pos < tempSize; pos++ )
   {
      // Envia el dato a escribir
      WRITE_EEPROM ( tempAddress + pos, *( tempPtr + pos ) );        

      // Espera entre datos escritos
      delay_ms ( 10 );
   }
}








