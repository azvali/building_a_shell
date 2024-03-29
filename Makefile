# Set the C compiler to gcc.
CC = gcc

# For the C flags, choose the -Wall option, which enables most warning
# messages to help catch potential issues in the code.
CFLAGS = -Wall

# Specify the names of the final executables.
MAIN_TARGET = shell
TASK_TARGET = task

# Default target: build both the main program and the task program.
all: $(MAIN_TARGET) $(TASK_TARGET)

# Rule to build the main program.
$(MAIN_TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(MAIN_TARGET)

# Rule to build the task program.
$(TASK_TARGET): task.c
	$(CC) $(CFLAGS) task.c -o $(TASK_TARGET)

# Clean up the project by removing executables.
clean:
	rm -f $(MAIN_TARGET) $(TASK_TARGET)
