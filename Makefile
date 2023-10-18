# Executable name
NAME = irc

# Compiler and flags
CC = c++
CFLAGS = -Wall -Werror -Wextra
#-std=c++98

# Color codes for messages
RESET = \033[0m
GREEN = \033[32m
YELLOW = \033[33m
RED = \033[31m

# Directories for source, object, and header files
SRC_DIR = srcs/
HDR_DIR = incs/
OBJ_DIR = objs/

# Source, object, and header files
SRC_FILES = main.cpp IRCServer.cpp splash.cpp User.cpp Command.cpp ft_split.cpp
HDR_FILES = includes.hpp IRCServer.hpp splash.hpp User.hpp Command.hpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Paths for source, object, and header files
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
HDR = $(addprefix $(HDR_DIR), $(HDR_FILES))
OBJ = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

# Build rules
all: $(NAME)

$(NAME): $(OBJ) $(HDR)
	@echo "$(YELLOW)Compiling..[$(NAME)]$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Finished compiling..$(NAME)$(RESET)"

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp $(HDR)
	@mkdir -p $(OBJ_DIR)
	@echo "$(YELLOW)Compiling..[$@]$(RESET)"
	@$(CC) $(CFLAGS) -I $(HDR_DIR) -o $@ -c $<

# Clean rules
clean:
	@if [ -d "$(OBJ_DIR)" ]; then \
		rm -rf $(OBJ_DIR); \
		echo "$(RED)Deleting all object files..$(RESET)"; \
	else \
		echo "No object files to delete.."; \
	fi;

fclean: clean
	@if [ -f "$(NAME)" ]; then \
		rm -f $(NAME); \
		echo "$(RED)Deleting $(NAME)$(RESET)"; \
	else \
		echo "$(RED)No executable to delete.$(RESET)"; \
	fi;

# Rebuild rule
re: fclean all

# Declare phony targets (targets that do not represent files)
.PHONY: all clean fclean re

