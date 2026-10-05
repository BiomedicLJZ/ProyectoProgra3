# =============================================================================
#  Alternativa de terminal a CMake (macOS / Linux / WSL). NO MODIFICAR.
#  En CLion usa los perfiles de CMake (ENUNCIADO §1).
#
#  make            -> ./minidb        (-O0 -g)
#  make asan       -> ./minidb_asan   (AddressSanitizer + UBSan)
#  make exp        -> ./minidb_exp    (-O2)
#  make exper      -> ./minidb_experimentos (si hay archivos en experimentos/)
# =============================================================================
CXX      ?= g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -pedantic -Iinclude -pthread
SRCS      = $(wildcard src/*.cpp)
SRCS_LIB  = $(filter-out src/main.cpp,$(SRCS))
SRCS_EXP  = $(wildcard experimentos/*.cpp)
HDRS      = $(wildcard include/*.h)

minidb: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -O0 -g $(SRCS) -o $@

asan: minidb_asan
minidb_asan: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer $(SRCS) -o $@

exp: minidb_exp
minidb_exp: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -O2 $(SRCS) -o $@

exper: minidb_experimentos
minidb_experimentos: $(SRCS_EXP) $(SRCS_LIB) $(HDRS)
	$(CXX) $(CXXFLAGS) -O2 $(SRCS_EXP) $(SRCS_LIB) -o $@

limpiar:
	rm -f minidb minidb_asan minidb_exp minidb_experimentos

.PHONY: asan exp exper limpiar
