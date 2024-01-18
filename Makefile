NAME = ircServer
CC	= c++
SRC = main.cpp \
serverInitiate.cpp serverConnections.cpp serverTools.cpp \
serverReplies.cpp replyTools.cpp \
User.cpp Channel.cpp

OBJFILES = $(SRC:%.cpp=%.o)
CFLAGS = -Wall -Wextra -Werror -g3 #-fsanitize=address

all	: $(NAME)

$(NAME) : $(OBJFILES)
	@$(CC) $(CFLAGS) $(OBJFILES) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean all