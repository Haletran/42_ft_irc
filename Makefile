CXX = c++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

SRCS = srcs/main.cpp \
	srcs/Server.cpp \
	srcs/Client.cpp \
	srcs/Message.cpp \
	srcs/Channel.cpp \

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRCS:srcs/%.cpp=%.o))

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<


bot:
	@clear
	@echo "Starting bot..."
	$(CXX) $(CFLAGS) -o bot srcs/Bot.cpp
	@./bot

hexchat:
	@if ! flatpak list | grep -q io.github.Hexchat; then echo "Installing Hexchat..." && flatpak install flathub io.github.Hexchat -y ; else echo "Hexchat already installed"; fi

run: re
	@clear
	@echo "Starting "$(NAME)" and hexchat..."
	@flatpak run io.github.Hexchat > /dev/null 2>&1 &
	@./ircserv 6666 testtest2

valgrind: re
	@clear
	@echo "Starting server with valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes  ./ircserv 6666 testtest2

clean:
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f bot

re: fclean all

.PHONY: all clean fclean re
