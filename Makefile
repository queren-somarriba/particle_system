NAME	= particle_system
CC		= c++

DEBUG	= 0

RED		= \033[1;31m
GREEN	= \033[1;32m
BLUE	= \033[1;34m
ORANGE	= \033[1;33m
RESET	= \033[0m

ifeq ($(DEBUG), 1)
	CXXFLAGS	= -Wall -Wextra -Werror -g -march=native -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS		= -fsanitize=address
else
	CXXFLAGS	= -Wall -Wextra -Werror -O3 -march=native
	LDFLAGS		=
endif

INCLUDES_DIR		= includes
CLASS_DIR			= includes/class
EXTERNAL_LIBS_DIR	= includes/external
MATH_LIB_DIR		= includes/mathlib
INCLUDES			= -I$(INCLUDES_DIR) -I$(CLASS_DIR) -I$(EXTERNAL_LIBS_DIR) -I$(MATH_LIB_DIR)
OBJ_DIR				= obj
SRCS_DIR			= srcs
CLASS_CPP			= $(SRCS_DIR)/class
GLFLAGS				= -lglfw -lGL -lX11 -lpthread -ldl -lXrandr -lXinerama -lXcursor -lXi
CLFLAGS				= -L./libs -lOpenCL

SRCS		= $(SRCS_DIR)/main.cpp \
			$(SRCS_DIR)/InputHandler.cpp \
			$(SRCS_DIR)/renderer.cpp \
			$(SRCS_DIR)/external/glad.cpp \
			$(CLASS_CPP)/VBO.cpp \
			$(CLASS_CPP)/EBO.cpp \
			$(CLASS_CPP)/VAO.cpp \
			$(CLASS_CPP)/shader.cpp \
			$(CLASS_CPP)/camera.cpp \
			$(SRCS_DIR)/cl.cpp

OBJS		= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(BLUE)Compiling $<...$(RESET)"
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
#	ln -s /usr/lib64/libOpenCL.so.1 ./libs/libOpenCL.so 
	@echo -e "$(BLUE)Linking $(NAME)...$(RESET)"
	@$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(GLFLAGS) $(CLFLAGS)
	@echo -e "$(GREEN)Done! Binary $(NAME) is ready.$(RESET)"

clean:
	@echo -e "$(ORANGE)Cleaning objects...$(RESET)"
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo -e "$(RED)Full cleaning $(NAME)...$(RESET)"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re