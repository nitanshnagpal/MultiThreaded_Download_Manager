CC=g++
CFLAGS=-g -I include/
LDFLAGS=-lpthread -lcurl
SOURCES=main.cpp src/DownloadManager.cpp src/DownloadTask.cpp src/FileWritter.cpp src/TaskQueue.cpp src/ThreadPool.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
