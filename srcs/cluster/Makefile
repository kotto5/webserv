NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g
DFLAGS = -MMD -MP

SRCDIR = srcs
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJDIR = ./objs

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
$(NAME): $(OBJ)
    $(CXX) $(CXXFLAGS) $(OBJ)  $(INCLUDES) -o $@

FORCE:

clean:
    rm -f $(OBJ) $(DEPENDS)

fclean: clean
    rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re FORCE

norm:
    norminette

-include $(DEPENDS)
