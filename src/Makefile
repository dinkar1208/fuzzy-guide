# Declaration of variables
CC_FLAGS = -Wall -O3
LIBS=

# File names
EXEC 		= eeg
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
OUTPUT  = wavelet.ppm

#depending on SIMPLE_SCALAR flag, select toolflow 
ifneq (,$(SIMPLE_SCALAR))
#Simple scalar compiler
CC			  = sslittle-na-sstrix-gcc
SIM				=	sim-outorder $(EXEC)
else
#regular gcc compiler
CC_FLAGS += -DMATLAB_FILE_SUPPORT
LIBS 		 += -lz -lm
SIM				=	./$(EXEC)
endif


# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEC)

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -rf $(EXEC) $(OBJECTS) $(OUTPUT)

$(OUTPUT):$(EXEC)
	$(SIM)

sim:run
run:$(EXEC)
	$(SIM)
