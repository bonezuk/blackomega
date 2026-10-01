include(FetchContent)

set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)   # static hwy.lib
set(BUILD_TESTING       OFF CACHE BOOL "" FORCE)
set(HWY_ENABLE_TESTS    OFF CACHE BOOL "" FORCE)   # skips the GoogleTest download
set(HWY_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(HWY_ENABLE_CONTRIB  OFF CACHE BOOL "" FORCE)   # turn ON if you need vqsort, etc.
set(HWY_ENABLE_INSTALL  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  highway
  GIT_REPOSITORY https://github.com/google/highway.git
  GIT_TAG        1.4.0                              # or a newer release tag
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(highway)

