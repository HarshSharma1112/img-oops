CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./sdl2_local/include -I./sdl2_local/include/SDL2
LDFLAGS = -L./sdl2_local/lib64 -L./sdl2_local/lib -Wl,-rpath,"$(CURDIR)/sdl2_local/lib64" -Wl,-rpath,"$(CURDIR)/sdl2_local/lib" -lSDL2

SRCDIR = src
SRCS = $(SRCDIR)/main.cpp \
       $(SRCDIR)/Game.cpp \
       $(SRCDIR)/GameObject.cpp \
       $(SRCDIR)/Player.cpp \
       $(SRCDIR)/Enemy.cpp \
       $(SRCDIR)/SecurityBot.cpp \
       $(SRCDIR)/Collectible.cpp \
       $(SRCDIR)/Door.cpp \
       $(SRCDIR)/Level.cpp \
       $(SRCDIR)/InputHandler.cpp

TARGET = game

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
