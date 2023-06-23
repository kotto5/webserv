NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g
# CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -g
DFLAGS = -MMD -MP

SRCDIR = srcs
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJDIR = ./objs
LIBFTDIR = ./libft
LIBFT = $(LIBFTDIR)/libft.a

OBJ := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.c=%.o))
DEPENDS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.c=%.d))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
    -mkdir -p $(@D)
    $(CXX) $(CXXFLAGS) $(DFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
    -mkdir -p $(@D)
    $(CXX) $(CXXFLAGS) $(DFLAGS) $(INCLUDES) -c $< -o $@

# for offline
$(NAME): $(LIBFT) $(OBJ)
    $(CXX) $(CXXFLAGS) $(OBJ) $(LIBFT) $(INCLUDES) -L/usr/X11R6/lib -lmlx -lX11 -lXext -framework OpenGL -framework AppKit -o $@

$(LIBFT): FORCE
    make -C ./$(LIBFTDIR)

FORCE:

clean:
    rm -f $(OBJ) $(DEPENDS)
    make clean -C $(LIBFTDIR)

fclean: clean
    rm -f $(NAME)
    make fclean -C $(LIBFTDIR)

NAME_DEBUG = debugfile
CXXFLAGS_DEBUG = -g

$(NAME_DEBUG): $(LIBFT) $(LIBMLX) $(OBJ)
    @printf "$(GREEN)"
    $(CXX) $(CXXFLAGS) $(OBJ) $(LIBFT) $(INCLUDES) -L/usr/X11R6/lib -lmlx -lX11 -lXext -framework OpenGL -framework AppKi -o $@
    @printf "$(RESET)"

debug: CXXFLAGS += $(CXXFLAGS_DEBUG)
debug: $(NAME_DEBUG)

re: fclean all

.PHONY: all clean fclean re FORCE

norm:
    norminette

-include $(DEPENDS)