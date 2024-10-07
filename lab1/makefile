CXX = g++
CXXFLAGS = -Iheaders -Wall -std=c++17
LDFLAGS = -lsqlite3
SOURCES = src/main.cpp src/user.cpp src/usercollection.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = lab1

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
