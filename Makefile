# Compile variables
NAME := webserv
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror
DFLAGS := -MMD -MP

PROJECT_DIR := $(CURDIR)

SRCDIR := srcs
SRCS := $(shell find $(SRCDIR) -type f -name "*.cpp")

OBJDIR := objs
OBJS = $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.o))

INCLUDES = -Isrcs/Request -Isrcs/Router -Isrcs/Response -Isrcs/Handler
DEPENDS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCS:%.cpp=%.d))

# Print variables
PRINTF := printf
DEFAULT := \033[0;39m
BLUE := \033[0;94m
GREEN := \033[0;92m
RED := \033[0;91m
DEL := \033[2K
MOVE := \033[1F
CR := \033[1G

# Progress variables
SRC_TOT := $(shell expr $(words $(SRCS)) - $(shell find $(OBJDIR) -name '*.o' | wc -l))
ifndef SRC_TOT
	SRC_TOT := $(words $(SRCS))
endif
SRC_CNT := 0
SRC_PCT = $(shell expr 100 \* $(SRC_CNT) / $(SRC_TOT))
PROGRESS = $(eval SRC_CNT = $(shell expr $(SRC_CNT) + 1)) \
	$(PRINTF) "${DEL}${GREEN}[ %d/%d (%d%%) ] $(CXX) $(CXXFLAGS) $< ...$(DEFAULT)$(CR)" $(SRC_CNT) $(SRC_TOT) $(SRC_PCT)

# Main commands
$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@echo "\n${BLUE}--- ${NAME} is up to date! ---${DEFAULT}"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@-mkdir -p $(@D)
	@${PROGRESS}
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

debug:
	@echo $(shell expr $(words $(SRCS)))
	@echo $(shell ls -lR $(OBJDIR) | grep .o$ | wc -l)
	@echo $(shell ls -lR $(OBJDIR) | grep \\.o$)

#: Make executable file.
all: $(NAME)

%.o: %.cpp
	-mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@


#: Remove all object files.
clean:
	@$(RM) $(OBJS) $(DEPENDS)
	@${PRINTF} "${RED}Cleaned up object files in ${basename ${NAME}} ${DEFAULT}\n"

#: Remove all object and executable files.
fclean: clean
	@$(RM) $(NAME)
	@${PRINTF} "${RED}Removed executable file of ${NAME} ${DEFAULT}\n"

#: Remove and recompile all.
re: fclean
	@$(MAKE) -s all

#: Make executable file and exexute.
run: $(NAME)
	./$(NAME) ./conf/default.conf

#: Check source files.
echo:
	@echo $(SRCS)

# Unit tests
TEST_SRCS := $(shell find tests/srcs -type f -name "*.cpp")
TEST_OBJDIR := ./tests/objs
TEST_OBJS := $(patsubst tests/srcs/%, $(TEST_OBJDIR)/%, $(TEST_SRCS:%.cpp=%.o)) $(filter-out objs/main.o, $(OBJS))
TEST_NAME := test_webserv
TEST_CXXFLAGS := -Wall -Wextra -Werror -std=c++14 -g
TEST_LDFLAGS := -L$(PROJECT_DIR)/tests/lib -lgtest -lgtest_main
SRC_DIRS := $(shell find $(PROJECT_DIR)/srcs -type d)
INC_FLAGS := $(addprefix -I, $(SRC_DIRS))
TEST_INCS := -I$(PROJECT_DIR)/tests/include $(INC_FLAGS)

$(TEST_NAME): $(TEST_OBJS)
	@-mkdir -p $(@D)
	$(CXX) $(TEST_CXXFLAGS) $(TEST_INCS) $(TEST_OBJS) $(TEST_LDFLAGS) -o $(TEST_NAME)

$(TEST_OBJDIR)/%.o: tests/srcs/%.cpp
	@-mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(TEST_INCS) $(TEST_CXXFLAGS) -c $< -o $@

#: Run unit tests.
ut: $(TEST_NAME)
	@rm -f $(TEST_OBJS)
	@./$(TEST_NAME)
	@rm $(TEST_NAME)
	@rm -f $(TEST_OBJS)


#: Display all commands.
help:
	@grep -A1 -E "^#:" Makefile \
	| grep -v -- -- \
	| sed 'N;s/\n/###/' \
	| sed -n 's/^#: \(.*\)###\(.*\):.*/\2###\1/p' \
	| sed -e 's/^/make /' \
	| column -t -s '###'

.PHONY: all clean fclean echo run re ut help

-include $(DEPENDS)
