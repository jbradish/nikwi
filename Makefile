#
# Nikwi Deluxe makefile. Needs GNU Make
#

OUTPUT = nikwi
SOURCES = $(wildcard src/nikwi/*.cpp) src/slashfx/main.c $(wildcard src/slashtdp/*.cpp) $(wildcard src/us/*.cpp) src/badcfg/main.c
OBJECTS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
HEADERS = $(wildcard src/nikwi/*.h) src/slashfx/slashfx.h $(wildcard src/slashtdp/slashtdp/*.h) $(wildcard src/us/uscript/*.h) src/badcfg/badcfg.h
LIBS = -lSDL
CFLAGS := -g3 -Wall -Wno-write-strings -Isrc/badcfg -Isrc/nikwi -Isrc/slashfx -Isrc/slashtdp -Isrc/us $(CFLAGS) $(CPPFLAGS)
CXXFLAGS = $(CFLAGS)

.PHONY: all
all: game data

.PHONY: game
game: $(OUTPUT)

.PHONY: data
data: tools justdata.up

.PHONY: tools
tools: src/tools/bmp2ut/bmp2ut src/tools/upack/upack

src/tools/bmp2ut/bmp2ut: src/tools/bmp2ut/bmp2ut.c
	$(CC) -o $@ $< -lSDL

src/tools/upack/upack: src/tools/upack/upack.c
	$(CC) -o $@ $<

justdata.up: tools
	./makedata.sh
	./makepack.sh

nikwi.res: nikwi.rc nikwi.ico
	windres -i nikwi.rc --input-format=rc -o nikwi.res -O coff

$(OUTPUT): $(OBJECTS)
	$(CXX) -o $(OUTPUT) $(OBJECTS) $(USELDFLAGS) $(LIBS)

.PHONY: clean
clean:
	$(RM) -fr $(OBJECTS) $(OUTPUT) $(OUTPUT).exe
	$(RM) -fr src/tools/bmp2ut/bmp2ut src/tools/bmp2ut/bmp2ut.exe
	$(RM) -fr src/tools/upack/upack src/tools/upack/upack.exe
	$(RM) -fr justdata.up nikwi.res
	$(RM) -fr images/tiles/*.ut images/objects/*.ut data/tiles/*.ut data/objects/*.ut

