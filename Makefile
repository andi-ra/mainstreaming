CXX = g++
CXXFLAGS = -Wall -Iinclude -Llib
LDFLAGS = -lssl -lcrypto -lpthread -ljsoncpp

OBJDIR = obj
BINDIR = bin
SRCDIR = src
INCDIR = include

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
TARGET = $(BINDIR)/server

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

setup:
	mkdir -p $(OBJDIR) $(BINDIR) logs content certs
	echo '{"port":8080, "cache_ttl":3600, "content_path":"./content", "certs_path":"./certs", "log_path":"./logs"}' > config.json
