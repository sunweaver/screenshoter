CFLAGS += $(shell pkg-config --cflags gdk-2.0 glib-2.0)
LDFLAGS += $(shell pkg-config --libs gdk-2.0 glib-2.0)
LDFLAGS += -lpopt

screenshoter: screenshoter.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f screenshoter

.PHONY: clean
