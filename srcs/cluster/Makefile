# NAME = webserv
# CXX = c++
# CXXFLAGS = -Wall -Wextra -Werror -g
# DFLAGS = -MMD -MP

# SRCDIR = $(PWD) $(PWD)../Request
# SRCS = $(wildcard $(SRCDIR)/*.cpp)
# OBJDIR = ./objs

# OBJ := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.o))
# DEPENDS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.d))

# INCLUDES = -I../Request

# all: $(NAME)

# $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
# 	-mkdir -p $(@D)
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

# %.o: %.cpp
# 	-mkdir -p $(@D)
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

# $(NAME): $(OBJ)
# 	$(CXX) $(CXXFLAGS) $(OBJ) $(INCLUDES) -o $@

# FORCE:

# clean:
# 	rm -f $(OBJ) $(DEPENDS)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re FORCE

# norm:
# 	norminette

# -include $(DEPENDS)
