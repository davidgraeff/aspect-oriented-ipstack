# Expected Environent:
#
# CIAOBASE:    directory with the CiAO os source (i.e., the trunk/os subdirectory)

#### Configuration ####
ARCH := _x86

# additional parameters to the transform script (e.g., -l to create hardlinks)
TRANSFORMFLAGS := -l
#######################

# include standard make modules
ROOT_DIR := $(CIAOBASE)/os
MODULES	 := kconf
include ../../make/standard.mk

.PHONY: default
default: $(APP).config
