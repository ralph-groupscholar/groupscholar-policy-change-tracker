CXX ?= g++
LIBPQ_PREFIX ?= /opt/homebrew/opt/libpq
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -I$(LIBPQ_PREFIX)/include
LDFLAGS ?= -L$(LIBPQ_PREFIX)/lib -lpq

BIN_DIR := bin
BIN := $(BIN_DIR)/gs-policy-change
TEST_BIN := $(BIN_DIR)/policy-tracker-tests

SRC := src/main.cpp src/policy_tracker.cpp
TEST_SRC := tests/test_policy_tracker.cpp src/policy_tracker.cpp

all: $(BIN)

$(BIN): $(SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

$(TEST_BIN): $(TEST_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all test clean
