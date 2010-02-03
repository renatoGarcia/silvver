C_ARGS = `pkg-config --cflags playerc`
C_LINK = `pkg-config --libs playerc`

OBJECTS = fiducial
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.c
	$(CC) $(C_ARGS) $(C_LINK) $^ -o $@
