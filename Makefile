.PHONY: all clear fclear re

SRC_DIR = src/
OBJ_DIR = obj/
INC_DIR = inc/

SRC = main.c
OBJ = $(patsubst %.c, $(OBJ_DIR)%.o, $(SRC))
INC = irondome.h

NAME = irondome

CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@
	@echo ${CY} [ ok ] ${FN} "$@: compiled"

$(DIR_SRC)%.c:
	@echo ${RD} [ error ] ${FN} "$@: file not found"

$(DIR_INC)%.h:
	@echo ${RD} [ error ] ${FN} "$@: file not found"

# ~  aesthetica ~
GR="\033[32m"
RD="\033[31m"
CY="\033[36m"
FN="\033[0m"
# ~      **     ~
