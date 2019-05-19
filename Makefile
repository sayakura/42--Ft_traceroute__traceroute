# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/13 02:07:27 by qpeng             #+#    #+#              #
#    Updated: 2019/05/13 02:07:54 by qpeng            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = traceroute
FLAGS = -Wall -Werror -Wextra -c 
FILES = main.c readopt.c helper.c
SRCS = $(addprefix src/, $(FILES))
OBJ = $(FILES:.c=.o)
all:
	@gcc -Iinclude $(FLAGS) $(SRCS)
	@gcc $(OBJ) -o $(NAME)
	@make clean
clean:
	@rm -rf *.o
fclean: clean
	@rm -f $(NAME)

re: clean all

.PHONY: all, $(NAME), clean, fclean, re