#include <18F252.h> // C:\Program Files (x86)\PICC\Devices


/*-----------------------------------------------------------------------------
                                     CONFIGURACION
-----------------------------------------------------------------------------*/
#device ADC=16

#FUSES PROTECT // Protege la memoria de programa para no lecturas

#ifndef DEBUG
#FUSES CPD     // Protege la memoria EEPROM
#endif

#FUSES NOWDT // DESACTIVA EL PERRO GUARDIAN DEL FUSE---> NECESARIO PARA ACTIVAR/DESACTIVAR EL PERRO GUARDIAN EN LA RUTINA PRINCIPAL
#FUSES WDT64 // CONFIGURA PRE ESCALER EN 1,152 SEGUNDOS

#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOPUT   // No power up timer
#FUSES HS      // Oscilador 

#USE delay ( crystal=20000000 )  // Velocidad

// Configura usar el modulo USART por hardware
#USE rs232 ( UART1, BAUD=9600, PARITY=E, BITS = 8, STOP=1, TIMEOUT=20 )


/*-----------------------------------------------------------------------------
                                     DEFINICIONES
-----------------------------------------------------------------------------*/
// Version firmware
#define VERSIONH         0x01
#define VERSIONL         0x01

// Tamanos buffers
#define   LENGTH_BUFFRX   40
#define   LENGTH_BUFFTX   20

// Encabezado trama
#define HEAD_BYTE   0x03

// Respuestas funciones 
#define ERROR_SIGNATURE 10
#define ERROR_VALUE 11


// Definicion de comandos---------------------------------------------------------------------------------------------------
#define COMMAND_OPEN             0x10  // Comando desbloqueo cerradura
#define COMMAND_LOCK               0x11  // Comando bloqueo cerradura
#define COMMAND_OPENV2           0x12  // Comando desbloqueo cerradura V2
#define COMMAND_LOCKV2           0x13  // Comando bloqueo cerradura V2
#define COMMAND_STATUS           0x20  // Comando lectura sensores
#define COMMAND_EEPROM_WRITE     0x40  // COMANDO ESCRITURA EEPROM
#define COMMAND_EEPROM_READ      0x41  // COMANDO LECTURA EEPROM
#define COMMAND_EEPROM_FORMAT    0x42  // COMANDO FORMATEAR BLOQUE EEPROM
#define COMMAND_CHANGE_KEY       0x43  // COMANDO CAMBIAR LLAVE
#define COMMAND_EEPROM_FORMAT2   0x44  // COMANDO FORMATEAR BLOQUE EEPROM V2
#define COMMAND_VERSION            0x50  // COMANDO VERSION FIRMWARE
#define COMMAND_BUZZER            0x52   // COMANDO BUZZER
#define COMMAND_CASHBOX            0x60   // COMANDO CASHBOX
#define COMMAND_POLL            0x70   // COMANDO CASHBOX
#define COMMAND_SETTOKEN            0x80   // COMANDO CASHBOX

// Defincion de respuestas
#define ANS_OK                0xA0
#define ANS_ERROR_FORMAT      0xE0   // Error formato comando
#define ANS_ERROR_CRC         0xE1   // Error CRC
#define ANS_ERROR_UNKNOWN      0xE2   // Error comando desconocido
#define ANS_ERROR_VALUE         0xE3   // Error uno de los valores fuera de rango
#define ANS_ERROR_LENGTH      0xE4   // Error longitud comando y datos
#define ANS_ERROR_INITIALIZE   0xF0   // Error bloque sin inicializar bloque
#define ANS_ERROR_CONDITION   0XF1   // Error condiciones de acceso
#define ANS_ERROR_KEY         0xF2   // Error la llave no es la correcta
#define ANS_ERROR_TOKEN       0xF3   // Error la llave no es la correcta
#define ANS_ERROR_TIME        0xF4   // Error la llave no es la correcta

// Formatos para campos de fecha y hora
#define FORMAT_24H   0
#define FORMAT_12H   1
#define TIME_AM      0
#define TIME_PM      1

// Constante para el desbordamiento del TMR0 cada 10ms OSC 20MHZ
#define TIMER_TMR0_10M      15535

// Tiempo maximo de espera de un byte luego de iniciar recepcion comando en 10ms*
#define TIMEOUT_RXCOMMAND 2

// Tiempo para mantener el pulso necesario actuador lineal 10ms*
#define TIMEOUT_PULSELOCK 200

// Tiempo para mantener el pulso que activa el solenoide en 10ms*
#define TIMEOUT_SOLENOIDE 300

// Tiempo para mantener el pulso de no comuncaciones
#define TIMEOUT_COMMUNICATION 50

// Direccion para escritura llave token
#define ADDRESS_KEYTOKEN  0x1E
#define ADDRESS_TIMETOKEN  0x23

// Estados logicos
#define STATUS_UNITIALIZE   0
#define STATUS_INITIALIZE   1
#define STATUS_ERROR         2
#define STATUS_FREE         4
#define STATUS_WAIT         5
#define STATUS_OPEN         6
#define STATUS_LOCK         7
#define STATUS_BLOCKFORMAT   8
#define STATUS_BUZZER      9
#define STATUS_SETTOKEN      10
#define STATUS_CHANGE_KEY      11

#define STATUS_WRITEEEPROM   50
#define STATUS_READEEPROM   51

// Definicion subprocesos
#define STEP_START         0
#define STEP_AUTHENTICATE   1
#define STEP_UNLOCK_START   20
#define STEP_UNLOCK_PULSE   21
#define STEP_UNLOCK_WAIT   22
#define STEP_LOCK_START      30
#define STEP_LOCK_PULSE      31
#define STEP_LOCK_WAIT      32
#define STEP_WAIT            40

// Version de comandos
#define VERSION_1    1
#define VERSION_2    2

// Valida para PIC de 28 pines
#ifndef PIC_40P  

   // Definicion de pines especiales
   #define PIN_LED            PIN_C5
   #define PIN_BUZZER         PIN_B7
     
   // Cerradura 1 tipo A
   #define PIN_1_IA            PIN_C0
   #define PIN_1_IB            PIN_C1
   #define PIN_1_LI            PIN_B0
   #define PIN_1_LO            PIN_B1
   #define PIN_1_SL            PIN_A0
   #define PIN_1_SD            PIN_A1
     
   // Cerradura 2 tipo A
   #define PIN_2_IA            PIN_C2
   #define PIN_2_IB            PIN_C3
   #define PIN_2_LI            PIN_B2
   #define PIN_2_LO            PIN_B3
   #define PIN_2_SL            PIN_A2
   #define PIN_2_SD            PIN_A3
     
   // Cerradura 3 tipo B
   #define PIN_3_IA            PIN_C4     
   #define PIN_3_SD            PIN_A5
     
   // Cashbox ( bolsa lata)
   #define PIN_CASHBOX         PIN_A4
     
   // MAscara de bits sensores
   #define MASK_SD            0b00000001
   #define MASK_SL            0b00000010
   #define MASK_LO            0b00010000
   #define MASK_LI            0b00100000     

#else
// PIC de 40 pines
     
   // Definicion de pines especiales
   #define PIN_LED            PIN_C5
   #define PIN_BUZZER         PIN_RD3   // ya cambiado
     
   // Cerradura 1 tipo A
   #define PIN_1_IA            PIN_C0
   #define PIN_1_IB            PIN_C1
   #define PIN_1_LI            PIN_B0
   #define PIN_1_LO            PIN_B1
   #define PIN_1_SL            PIN_A0
   #define PIN_1_SD            PIN_A1
     
   // Cerradura 2 tipo A
   #define PIN_2_IA            PIN_C2
   #define PIN_2_IB            PIN_D0   //ya cambiado
   #define PIN_2_LI            PIN_B2
   #define PIN_2_LO            PIN_B3
   #define PIN_2_SL            PIN_A2
   #define PIN_2_SD            PIN_A3
     
   // Cerradura 3 tipo B
   #define PIN_3_IA            PIN_D1   // ya cambiado
   #define PIN_3_SL            PIN_A4
   #define PIN_3_SD            PIN_A5
     
   // Mascara de bits sensores
   #define MASK_SD            0b00000001
   #define MASK_SL            0b00000010
   #define MASK_LO            0b00010000
   #define MASK_LI            0b00100000


#endif


// Numero de bytes por cada bloque
#define BLOCK_LENGTH         5

// Mascara de condicione de acceso
#define PERMISSION_FORMAT   0b10000000   // Permiso inicializar condiciones del bloque
#define PERMISSION_AUTH      0b00001000   // Permiso autenticar
#define PERMISSION_WRAUTH   0b00000100   // Permiso escritura con autenticacion
#define PERMISSION_WR      0b00000010   // Permiso escritura sin autenticacion
#define PERMISSION_R         0b00000001   // Permiso lectura sin autenticacion


/*-----------------------------------------------------------------------------
                                     FUNCIONES
-----------------------------------------------------------------------------*/
extern int8 CalcCRC ( unsigned char * data );   
extern void ClearBufferRx ( void ); 
extern void ClearBufferTx ( void );
extern int8 CompareBuffer ( int8 * tempData1, int8 * tempData2, int8 tempLenght );
extern void DebugInt32 ( unsigned int32 tempValue );
extern int8 GetSensorStatus ( void );
extern void ReadEEprom ( int8 * tempPtr, int8 tempAddress, int8 tempSize );
extern void SendByte ( int8 tempData );
extern void SendAns ( int8 tempCommand, int8 tempAns );   
extern void SendAnsComplex ( int8 tempCommand, int8 tempAns, int8 *tempPointer, 
                     int8 tempLength );
extern void SleepMinute ( int8 tempMinute );
extern int8 ValidateCommand ( void );
extern int8 ValidateToken ( unsigned int8 * tempKeyToken );
extern void WriteEEprom ( int8 * tempPtr, int8 tempAddress, int8 tempSize );





