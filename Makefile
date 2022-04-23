all : main main_threaded compare create_images

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
METRIC_FILES = $(wildcard ./metrics/*.cpp)

main : main.cpp
	g++ -o bin/main main.cpp $(OPTIM_FILES) $(BASE_ALGO_FILES) $(METRIC_FILES) `pkg-config --cflags --libs opencv` -lpthread -g -Wall

main_threaded : 
	g++ -o bin/main_threaded main_threaded.cpp $(OPTIM_FILES) $(BASE_ALGO_FILES) $(METRIC_FILES) `pkg-config --cflags --libs opencv` -lpthread -g -Wall

compare : 
	g++ -o bin/comparison compare_algos.cpp $(OPTIM_FILES) $(BASE_ALGO_FILES) $(METRIC_FILES) `pkg-config --cflags --libs opencv` -lpthread -g -Wall

compare_theta : 
	g++ -o bin/cmp_theta compare_thetaSteps.cpp $(OPTIM_FILES) $(BASE_ALGO_FILES) $(METRIC_FILES) `pkg-config --cflags --libs opencv` -lpthread -Wall

create_images : 
	g++ -o bin/create_images create_images.cpp `pkg-config --cflags --libs opencv` -lpthread -g -Wall

clean :
	rm  $(DIR_OBJ)/*.o ; rm main ;
