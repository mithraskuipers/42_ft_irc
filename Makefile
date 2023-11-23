# Exe name
NAME = ircserv

# Compiler and flags
CC = c++
CFLAGS = -Wall -Werror -Wextra

# Color codes
RESET = \033[0m
GREEN = \033[32m
YELLOW = \033[33m
RED = \033[31m

# Directories for source, object, and header files
SRC_DIR = src/
HDR_DIR = incs/
OBJ_DIR = objs/

# Source, object, and header files
SRC_FILES = main.cpp Server.cpp splash.cpp Client.cpp Command.cpp Channel.cpp CommandProcessor.cpp \
            cmdJoin.cpp cmdTopic.cpp cmdNick.cpp cmdPart.cpp cmdInvite.cpp cmdKick.cpp \
            cmdPass.cpp cmdPing.cpp cmdPong.cpp cmdPrivmsg.cpp cmdUser.cpp utils.cpp cmdMode.cpp
HDR_FILES = Channel.hpp Client.hpp splash.hpp CommandProcessor.hpp cmdJoin.hpp cmdNick.hpp \
            cmdPart.hpp cmdPass.hpp cmdPing.hpp cmdPong.hpp cmdPrivmsg.hpp cmdKick.hpp\
            cmdUser.hpp cmdTopic.hpp Command.hpp \
            Replies.hpp Server.hpp utils.hpp \
			cmdInvite.hpp cmdMode.hpp
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

# Run rule
execute: $(NAME)
	@./$(NAME) 6667 pw

# Declare phony targets (targets that do not represent files)
.PHONY: all clean fclean re execute
