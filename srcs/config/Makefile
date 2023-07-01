# Variables
CXX = c++
CXXFLAGS = -std=c++98
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=obj/%.o)
EXEC = config_test

# Default target
all: $(EXEC)

# Link
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile
obj/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf obj

# Full clean
fclean: clean
	rm -f $(EXEC)

# Re-make
re: fclean all

# Make sure all targets run even if a file with the same name exists
.PHONY: all clean fclean re
