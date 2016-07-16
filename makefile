CFLAGS = -O3 -g3 -Wall -c -fmessage-length=0 -fopenmp -fpermissive -MMD -MP
LIBS = -lopencv_core -lopencv_highgui -lgomp 
IDIR = ../headers
OPENCVDIR = ../headers/openCV
IFLAGS = -I$(IDIR) -I$(OPENCVDIR)

all: myFaceDetector

myFaceDetector: myFaceDetectorFull.o
	g++ -o myFaceDetector myFaceDetectorFull.o $(LIBS)
	
myFaceDetectorFull.o: myFaceDetectorFull.cpp
	g++ $(IFLAGS) $(CFLAGS) sources/myFaceDetectorFull.cpp
	
myFaceDetectorFull.cpp: sources/*.h

clean: 
	rm *.o myFaceDetector *.d