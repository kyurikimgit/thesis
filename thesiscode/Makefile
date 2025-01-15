CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -I"C:/Users/rlarb/boost_1_86_0"

TARGET = sample_2.exe
SRC = sample_2.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)  # .o 파일을 링크하여 실행 파일 생성

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # .cpp -> .o 변환 (컴파일 과정)

clean:
	rm -f $(OBJ) $(TARGET)
