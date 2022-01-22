CC ?= gcc
AR ?= ar

USER_SRCS := malloc_main.c
USER_OBJS := $(patsubst %.c,%.o,$(wildcard $(USER_SRCS)))
USER_D_SRCS := malloc_so.c malloc_free_so.c
USER_D_OBJS := $(patsubst %.c,%.o,$(wildcard $(USER_D_SRCS)))
USER_S_SRCS := malloc_a.c malloc_free_a.c
USER_S_OBJS := $(patsubst %.c,%.o,$(wildcard $(USER_S_SRCS)))

OUTPUT_DIR := output

SLD_LIBS := -lmalloca
LD_FLAGS := -lmalloc

S_LIB_NAME := libmalloca.a
D_LIB_NAME := libmalloc.so

USER_PROGRAM_NAME := malloc_main

CFLAGS = -Wall -ldl -g

.PHONY: all clean

$(OUTPUT_DIR)/libs/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTPUT_DIR)/libs/$(S_LIB_NAME): $(USER_S_OBJS)
	mkdir -p $(dir $@)
	$(AR) -rcs $@ $^
	@echo "Build $@ is finished"

$(OUTPUT_DIR)/libs/$(D_LIB_NAME): $(USER_D_OBJS)
	mkdir -p $(dir $@)
	$(CC) -fPIC -shared -o $@ $^
	echo "Build $@ is finished"

$(USER_PROGRAM_NAME): $(USER_SRCS)
	$(CC) $^ $(CFLAGS) -L./$(OUTPUT_DIR)/libs -Wl,-Bstatic $(SLD_LIBS) -Wl,-Bdynamic $(LD_FLAGS) -o $@

all: $(OUTPUT_DIR)/libs/$(S_LIB_NAME) \
	$(OUTPUT_DIR)/libs/$(D_LIB_NAME) \
	$(USER_PROGRAM_NAME)

clean:
	rm *.o -rf
	rm *.a -rf
	rm *.so -rf
	rm -rf $(USER_PROGRAM_NAME)
	rm output -rf