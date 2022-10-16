# ----------------------------
# Makefile Options
# ----------------------------

NAME = Ball8
ICON = icon.png
DESCRIPTION = "eightball billiards"
COMPRESSED = NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

HAS_PRINTF := NO

# ----------------------------

include $(shell cedev-config --makefile)
