NAME    = shadow_client
SRCS    = main.cpp settings.cpp
OBJS    = $(SRCS:.cpp=.o)

CPP     = g++
CXXFLAGS= -std=c++17
LDFLAGS = -lssl -lcrypto

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OBJS)

fclean: clean
  $(RM) -rd $(NAME)

re: fclean all

.PHONY: all clean fclean re
