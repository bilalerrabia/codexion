NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCS	= coders/main.c \
		coders/parsing.c \
		coders/smart_sleep.c \
		coders/helpers.c \
		coders/queue.c \
		coders/coder_dongle_init.c \
		coders/coder_routine.c \
		coders/dongle_manager.c \
		coders/monitor.c \
		coders/start_simulation.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
