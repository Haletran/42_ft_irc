CXX = c++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

SRCS = srcs/main.cpp \
	srcs/Server.cpp \
	srcs/Client.cpp \
	srcs/Message.cpp \
	srcs/Channel.cpp \
	srcs/Utils.cpp \
	srcs/Mode.cpp \
	srcs/Command.cpp \

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRCS:srcs/%.cpp=%.o))
OBJ_BONUS = $(OBJ_DIR)/Bot.o

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

bonus: $(OBJ_BONUS)
	$(CXX) $(CFLAGS) -o bot $^

bot: bonus
	@read -p "Do you want to launch valgrind with bot? [y/n] " valgrind;
	@if [ "$valgrind" = "y" ]; then valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./bot localhost 6697 mdp; else ./bot localhost 6697 mdp; fi

hexchat:
	@if ! flatpak list | grep -q io.github.Hexchat; then echo "Installing Hexchat..." && flatpak install flathub io.github.Hexchat -y ; else echo "Hexchat already installed"; fi

run: re
	@clear
	@echo "Starting "$(NAME)" and hexchat..."
	@flatpak run io.github.Hexchat > /dev/null 2>&1 &
	@./ircserv 6697 mdp

valgrind: re
	@clear
	@echo "Starting server with valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes  ./ircserv 6697 mdp

clean:
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f bot

re: fclean all

.PHONY: all clean fclean re bot valgrind run hexchat bonus
