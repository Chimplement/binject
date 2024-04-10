# project

NAME	= smaller

SRC_DIR	= src
SRC		= $(shell find $(SRC_DIR)/ -type f -name "*.c")
OBJ_DIR	= obj
OBJ		= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# compiler options
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wpedantic -I$(SRC_DIR)

# colors
INFO	= \x1b[1;36m
CREATED	= \x1b[1;32m
REMOVED = \x1b[1;33m
ERROR	= \x1b[1;31m
END		= \x1b[0m

# targets
.PHONY: all clean fclean all

all: $(NAME)
	@printf '$(INFO)Done making $(NAME)$(END)\n'

clean:
	@rm -f $(OBJ)
	@printf '$(REMOVED)Removed$(OBJ)$(END)\n'
	@rm -rf $(OBJ_DIR)
	@printf '$(REMOVED)Removed $(OBJ_DIR)/$(END)\n'

fclean: clean
	@rm -f $(NAME)

re: fclean all

$(NAME): $(OBJ)
	@$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)
	@printf '$(CREATED)Created $(NAME)$(END)\n'

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir --parent $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $^
	@printf '$(CREATED)Created $@ from $^$(END)\n'

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)
	@printf '$(CREATED)Created $(OBJ_DIR)/$(END)\n'