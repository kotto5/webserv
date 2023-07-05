NAME := webserv
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror
DFLAGS := -MMD -MP

PROJECT_DIR := $(CURDIR)

SRCDIR := srcs
SRCS := $(shell find $(SRCDIR) -type f -name "*.cpp")

OBJDIR = ./objs

INCLUDES = -Isrcs/Request -Isrcs/Router -Isrcs/Response -Isrcs/Handler

OBJS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.o))
DEPENDS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.d))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

%.o: %.cpp
	-mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

# for offline
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

run: $(NAME)
	./$(NAME) ./conf/default.conf

clean:
	rm -f $(OBJS) $(DEPENDS)

fclean: clean
	rm -f $(NAME)

re: fclean all

CXXFLAGS_DEBUG = -g

# Unit tests
TEST_SRCS := $(shell find tests/srcs -type f -name "*.cpp")
TEST_OBJDIR := ./tests/objs
TEST_OBJS := $(patsubst tests/srcs/%, $(TEST_OBJDIR)/%, $(TEST_SRCS:%.cpp=%.o)) $(filter-out ./objs/main.o, $(OBJS))
TEST_NAME := test_webserv
TEST_CXXFLAGS := -Wall -Wextra -Werror -std=c++14 -g
TEST_LDFLAGS := -L$(PROJECT_DIR)/tests/lib -lgtest -lgtest_main
SRC_DIRS := $(shell find $(PROJECT_DIR)/srcs -type d)
INC_FLAGS := $(addprefix -I, $(SRC_DIRS))
TEST_INCS := -I$(PROJECT_DIR)/tests/include $(INC_FLAGS)

$(TEST_NAME): $(TEST_OBJS)
	@-mkdir -p $(@D)
	@$(CXX) $(TEST_CXXFLAGS) $(TEST_INCS) $(TEST_OBJS) $(TEST_LDFLAGS) -o $(TEST_NAME)

$(TEST_OBJDIR)/%.o: tests/srcs/%.cpp
	@-mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(TEST_INCS) $(TEST_CXXFLAGS) -c $< -o $@

ut: $(TEST_NAME)
	@rm -f $(TEST_OBJS)
	@./$(TEST_NAME)
	@rm $(TEST_NAME)
	@rm -f $(TEST_OBJS)

.PHONY: all clean fclean re ut

-include $(DEPENDS)
