# openfpgaOS SDK -- Build All Bundled Apps
#
# Each subdirectory is an app. Builds app.elf in each.
# Apps with their own Makefile are built with it directly;
# the rest use the shared app.mk.
#

APPS = $(shell find . -maxdepth 1 -mindepth 1 -type d \
         | sort | sed 's|^\.\/||')

all:
	@for app in $(APPS); do \
		echo "Building $$app..."; \
		if [ -f $$app/Makefile ]; then \
			$(MAKE) -C $$app || exit 1; \
		else \
			$(MAKE) -C $$app -f ../app.mk || exit 1; \
		fi \
	done

clean:
	@for app in $(APPS); do \
		if [ -f $$app/Makefile ]; then \
			$(MAKE) -C $$app clean; \
		else \
			$(MAKE) -C $$app -f ../app.mk clean; \
		fi \
	done

.PHONY: all clean
