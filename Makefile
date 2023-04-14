 #=============================================================================#
#								SOURCES											#
 #=============================================================================#

UTLS_FLS = ansi\

UTLS_DIR = utils
UTLS = $(addsuffix .cpp, $(UTLS_FLS))

SRCS_FLS = Channel\
			Client\
			Irc\
			Server\
			Socket\
			main\

SRCS_DIR = sources
SRCS = $(addsuffix .cpp, $(SRCS_FLS))

 #=============================================================================#
#							OBJETS	& DEPENDANCE								#
 #=============================================================================#

OBJS_DIR = objets
OBJS = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)
UTLS_OBJ = $(UTLS:%.cpp=$(OBJS_DIR)/%.o)

DEPS = $(SRCS:%.cpp=$(OBJS_DIR)/%.d)
UTLS_DEP = $(UTLS:%.cpp=$(OBJS_DIR)/%.d)

 #=============================================================================#
#									COMPILATION									#
 #=============================================================================#

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
CIFLAGS = -Iincludes
CDFLAGS = -MMD -MP

 #=============================================================================#
#									MAKEFILE									#
 #=============================================================================#

NAME = ircserv

all : $(NAME)

$(NAME) : $(OBJS_DIR) $(UTLS_OBJ) $(OBJS)
	@echo "\033[0;1mCompilation objets : \033[1;5;32mSUCCESS\033[0m\n"
	@echo "Compiling excutable :\033[35m"
	$(CC) $(CFLAGS) $(CIFLAGS) $(UTLS_OBJ) $(OBJS) -o $(NAME)
	@echo "\033[0;1mCompilation excutables : \033[1;5;32mSUCCESS\033[0m\n"

$(OBJS_DIR) :
	@clear
	@mkdir $(OBJS_DIR)
	@echo "\033[0mCompiling objet :\033[36m"

$(OBJS) : $(OBJS_DIR)/%.o : $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) $(CIFLAGS) $(CDFLAGS) -c $< -o $@

$(UTLS_OBJ) : $(OBJS_DIR)/%.o : $(UTLS_DIR)/%.cpp
	$(CC) $(CFLAGS) $(CIFLAGS) $(CDFLAGS) -c $< -o $@

clean :
	@rm -rf $(OBJS_DIR)
	@echo "\033[0;3;31m$(OBJS_DIR) \033[0;1;31mdeleted."

fclean : clean
	@rm -rf $(NAME)
	@echo "\033[0;3;31m$(NAME) \033[0;1;31mdeleted."

re : fclean all

-include $(DEPS) $(UTLS_DEP)

.PHONY: all clean fclean re

# .SILENT: