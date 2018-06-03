SOURCES = $(shell find src -name '*.c')
OBJECTS = $(SOURCES:.c=.o)
TARGET = dvdcat
VERSION = 0.1.0

CFLAGS = $(shell pkg-config --cflags dvdread) \
	-D "DVDCAT_VERSION=\"$(VERSION)\""
	
CFLAGS_ALL = -g -Wall

LDFLAGS = $(shell pkg-config --libs dvdread)
LDFLAGS_ALL = -g -Wall


all: dvdcat

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDFLAGS_ALL) $(OBJECTS) -o $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $(CFLAGS_ALL) $< -o $@

install: dvdcat
	install -D dvdcat $(DESTDIR)/usr/bin/dvdcat

clean:
	rm -f $(OBJECTS) $(TARGET)