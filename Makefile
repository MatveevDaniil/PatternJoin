# Variables
CXX = g++
CXXFLAGS = -O3 -std=c++20
UNORDERED_DENSE_PATH = ../unordered_dense

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = pattern_join

# Rules
all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(BIN_DIR)/$(TARGET)

.PHONY: all clean