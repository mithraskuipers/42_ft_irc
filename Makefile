NAME = ircServer
CC	= c++
SRC = main.cpp \
server/initiateServer.cpp server/recvSend.cpp server/Server.cpp server/serverUtils.cpp \
replies/allReplies.cpp replies/replyUtils.cpp \
channel/Channel.cpp \
user/User.cpp

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