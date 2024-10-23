CXX = c++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

SRC = srcs/main.cpp \
	srcs/Server.cpp \
	srcs/Client.cpp \
	srcs/Message.cpp \
	srcs/Channel.cpp \

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:srcs/%.cpp=%.o))

TARGET = ircserv

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
