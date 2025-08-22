CXX = clang++
CXXFLAGS = -std=c++17 -Wall \
	-Iinclude -I2.include

SRC_DIRS = 1.src 1.src/business_logic 1.src/data_layer 1.src/utilities
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o)

TARGET = library_app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

print:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"

clean:
	rm -f $(OBJS) $(TARGET)