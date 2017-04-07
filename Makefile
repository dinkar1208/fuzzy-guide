
TARGET  = eeg
N_PROC  = 1

CC_FLAGS +=  -fopenmp -mfpmath=sse -msse2
DEFINES   =  -DN_PROC=$(N_PROC)
SOURCES   =  $(wildcard src/*.c)
OBJECTS   =  $(SOURCES:.c=.o)
INC       =  -I../../include
LIBS      =  -lm -lz

include ../shared/Makefile.shared
$(TARGET): $(OBJECTS)
	$(CC)  $(OBJECTS) $(LIBS) $(SNIPER_LDFLAGS) $(DEFINES) -fopenmp -o $(TARGET)

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $(INC) $(DEFINES) $< -o $@

run_$(TARGET): $(TARGET)
	../../run-sniper --roi -n $(N_PROC)  -c proc -- ./$(TARGET)

mcpat:run_$(TARGET)
	../../tools/mcpat.py
	../../tools/mcpat.py -t area

CLEAN_EXTRA := $(OBJECTS) wavelet.ppm

