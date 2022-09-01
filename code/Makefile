.PHONY: all clear fclear re

SRC_DIR = src/
OBJ_DIR = obj/
INC_DIR = inc/

SRC = main.c entropy.c utils.c \
	fs_monitor/fs_monitor.c fs_monitor/fs_events.c fs_monitor/fs_utils.c
OBJ = $(patsubst %.c, $(OBJ_DIR)%.o, $(SRC))
INC = irondome.h

NAME = irondome

CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra -fsanitize=address

all: $(NAME)

$(NAME): $(OBJ) $(INC_DIR)$(INC)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) -lm -lpthread

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@
	@echo ${CY} [ ok ] ${FN} "$@: compiled"

$(DIR_SRC)%.c:
	@echo ${RD} [ error ] ${FN} "$@: file not found"

$(DIR_INC)%.h:
	@echo ${RD} [ error ] ${FN} "$@: file not found"

clean:
	@rm -rf $(NAME)
	@echo ${RD} [ rm ] ${FN} "$(NAME): removed"

fclean:	clean
	@rm -rf $(OBJ_DIR)
	@echo ${RD} [ rm ] ${FN} "$(OBJ_DIR): removed"

re: fclean all

# ~  aesthetica ~
GR="\033[32m"
RD="\033[31m"
CY="\033[36m"
FN="\033[0m"
# ~      **     ~
