#=========================================================================#
#             makefile para el proyecto "MoneyManager"                           #
#=========================================================================#

# +--------------------------------+
# | Definicion de Macros genericas |
# +--------------------------------+

# Opcion para seleccionar destino
#TARGET_PC = Opcion para compilar al computador
#TARGET_RASPBERRY = Opcipon para compilar version raspberry
#TARGET_GAPB = Version exclusiva

TARGET = TARGET_GAPB

#TARGET = TARGET_PC

# Para el computador
ifeq ($(TARGET),TARGET_PC)
CC = g++ -D TARGET_PC
SALIDA	= Test_pic_1_1.bin
endif

# Para el computador
ifeq ($(TARGET),TARGET_GAPB)
CC = g++ -D TARGET_PC -D VERSION_GAPB
SALIDA	= GAPB_pic1_1.bin
endif

# Para el computador
ifeq ($(TARGET),TARGET_RASPBERRY)
CC = g++ 
SALIDA	= Test_pic1_1.bin
endif



#para la TS7600
ifeq ($(TARGET),TARGET_TS7600)
CC = /opt/crossCompiler/arm-fsl-linux-gnueabi/bin/arm-linux-gcc
endif

#para la Cubieboard
ifeq ($(TARGET),TARGET_CUBIEBOARD)
CC = g++
endif



# Opciones

#CFLAGMOD = -D DEBUG  -D DEBUG_CASHIERB -D DEBUG_CASHIERA -D DEBUG_PICA -D DEBUG_PICB -D DEBUG_TBVA -D DEBUG_TBVB
CFLAGMOD =  -std=c++11

# Opciones includes
CINCLIB = -pthread

# Opciones para librerias
CFLAGLIB =

# +------------------------------+
# | Definicion de Macros propias |
# +------------------------------+

# Lista de archivos *.cpp que se deben compilar
FUENTES =	Main.cpp CFile.cpp CTool.cpp CPic.cpp 
	

# Lista de archivos *.o que deben ser vinculados para generar el binario final
OBJETOS =  Main.o CFile.o CTool.o CPic.o 

# Lista de archivos *.h que se requieren
ENCABEZADOS = Main.h CFile.h CTool.h CPic.h 

# Ruta de la carpeta principal del proyecto
ifeq ($(TARGET),TARGET_PC)
RUTAPROYECTO 	= /home/german/Personal/Trabajo/Reinicio/SmartBox/LockPic/Trunk/Resource/Test_Pic
RUTALIBITL	=	
endif

# Ruta de la carpeta principal del proyecto
ifeq ($(TARGET),TARGET_GAPB)
RUTAPROYECTO 	= /home/german/Personal/Trabajo/Reinicio/SmartBox/LockPic/Trunk/Resource/Test_Pic
RUTALIBITL	=	
endif

# Ruta de la carpeta principal del proyecto
ifeq ($(TARGET),TARGET_RASPBERRY)
RUTAPROYECTO 	= 
RUTALIBITL	=
endif

ifeq ($(TARGET),TARGET_CUBIEBOARD)
RUTAPROYECTO 	=
RUTALIBITL	=	
endif

# Ruta donde se encuentran los archivos *.c y *.h del proyecto
RUTASOURCE 		= 	$(RUTAPROYECTO)

# Ruta donde estan los *.o despues del proceso de compilacion intermedio
RUTAOBJETOS 	= 	$(RUTAPROYECTO)/Objects

# Ruta donde quedara el binario final
RUTARELEASE 	=  	$(RUTAPROYECTO)/Release

# Ruta donde se encuentran las librerias adicionales para compilar
ifeq ($(TARGET),TARGET_PC)
RUTALIBRERIAS =	Library
endif

ifeq ($(TARGET),TARGET_CUBIEBOARD)
RUTALIBRERIAS =	Library
endif

ifeq ($(TARGET),TARGET_RASPBERRY)
RUTALIBRERIAS =	Library
endif

# Direccion IP de descarga
ifeq ($(TARGET),TARGET_TS7600)
IP = 192.168.0.50
endif

# +------------------------------------+
# | Re-Definicion de Reglas implicitas |
# +------------------------------------+

# Objetivo principal de compilacion que obliga a borrar los archivos de compilaciones pasadas y
# generar el nuevo
all: limpiar .c.o
	$(CC) $(CFLAGMOD) $(CINCLIB) $(RUTAOBJETOS)/*.o -pthread \
	-lpthread -L$(RUTAPROYECTO)/$(RUTALIBRERIAS) -o $(RUTARELEASE)/$(SALIDA)
	chmod 777 $(RUTARELEASE)/$(SALIDA)
	echo " **** MD5 **** "
	md5sum $(RUTARELEASE)/$(SALIDA)
	date


# Compila los archivos *.c para generar los *.o y los deja en la carpeta destinada para los *.o
.c.o: $(OBJETOS)
	mv $(OBJETOS) $(RUTAOBJETOS)

# Compila los *.o con el compilador y opcionespredefinidas
$(OBJETOS):  $(FUENTES) $(ENCABEZADOS)
	$(CC) -c $(CFLAGMOD) $(CINCLIB) $(RUTASOURCE)/$(FUENTES)


# +---------------------------------+
# | Otras reglas para mantenimiento |
# +---------------------------------+

# Borra los *.o y el binario
limpiar:
	-rm $(RUTAOBJETOS)/*.o
	-rm $(RUTARELEASE)/$(SALIDA)