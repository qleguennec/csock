NAME = csock_serv
SRC = $(filter %.c, $(shell ls))
OBJ = $(SRC:.c=.o)
LIB =

INCS +=
CFLAGS += -g -Wall -Wextra -Werror $(INCS)
LDLIBS += -lpthread
LOADLIBES +=
LDFLAGS +=

all: $(NAME)

$(NAME): $(OBJ)

clean:
	@rm $(OBJ) 2> /dev/null || true

fclean: clean
	@rm $(NAME) 2> /dev/null || true

re: fclean all

.PHONY: all clean fclean re
