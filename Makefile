CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

SRCS = src/Common.cpp src/Mobility.cpp src/Node.cpp src/NetworkLayer.cpp src/RoutingEngine.cpp src/SimulatorEngine.cpp src/MacLayer.cpp src/ErrorControl.cpp src/ReliableTransport.cpp src/ServiceDiscovery.cpp src/DisasterScenario.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = manet_sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) 1

experiments: $(TARGET)
	./$(TARGET) 2

.PHONY: all clean run experiments
