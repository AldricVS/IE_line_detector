all : main main_threaded

VERSION = 0.3
DIR_OBJ = ./objs
DIR_EXEC = ./
LIBRARIES = -lGL -lGLU -lglut -lm -lX11   # LINUX
#LIBRARIES = -L"/System/Library/Frameworks/OpenGL.framework/Libraries" # MacOs
FRAMEWORK = -framework GLUT   #Macos
FRAMEWORK += -framework OpenGL #Macos
LIBPATH += $(LIBRARIES)
COMPILERFLAGS = -Wall
CFLAGS = $(COMPILERFLAGS)

# Search all cpp files in the 'optim' folder
OPTIM_FILES = $(wildcard ./optim/*.cpp)
BASE_ALGO_FILES = $(wildcard ./base_algos/*.cpp)

main : main.cpp
	g++ -o bin/main main.cpp `pkg-config --cflags --libs opencv` -g -Wall

main_threaded : 
	g++ -o bin/main_threaded main_threaded.cpp $(OPTIM_FILES) $(BASE_ALGO_FILES) `pkg-config --cflags --libs opencv` -lpthread -g -Wall

clean :
	rm  $(DIR_OBJ)/*.o ; rm main ;
