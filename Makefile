# Definições de compilador e flags
CXX = clang++
CXXFLAGS = `sdl2-config --cflags` -Wall -Wextra -std=c++17
LDFLAGS = `sdl2-config --libs`

# Diretórios
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = build

# Nome do executável
TARGET = Main

# Encontrar todos os arquivos .cpp na pasta src
SRC = $(wildcard $(SRC_DIR)/*.cpp)

# Correspondente arquivos .o na pasta build
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Regra padrão
all: $(TARGET)

# Regra para linkar o executável
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Regra para compilar arquivos .cpp em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Regra para limpar arquivos compilados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
