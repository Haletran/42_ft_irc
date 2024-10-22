CC = c++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

SRC = main.cpp \
	Server.cpp \
	Client.cpp \
	Message.cpp \

OBJ = $(SRC:.cpp=.o)

TARGET = irc

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all
