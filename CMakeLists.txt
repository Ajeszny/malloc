#
# DANTE :: System Automatycznych Testów Jednostkowych oraz Akceptacyjnych
# Tomasz Jaworski, 2018-2020
#
# Plik wygenerowany automatycznie
# Znacznik czasowy: 2023-12-29 13:07:39.743952
#

cmake_minimum_required(VERSION 3.17)
project(project1 C)

# Przyjmij standard C11
set(CMAKE_C_STANDARD 11)

# Ustaw opcje kompilatora (z raportu Dante)
add_compile_options(
      "-xc"
      "-Wno-parentheses"
      "-Werror=vla"
      "-pedantic"
      "-std=c11"
      "-fdiagnostics-color"
      "-ggdb3"
      "-D_GNU_SOURCE"
      "-DINSIDE_DANTE"
      "-Wall"
      "-D_ANSI_OUTPUT"
      "-Wno-error=parentheses"
      "-Wno-error=implicit-fallthrough"
      "-fmax-errors=5"
      "-Werror"
      "-Wno-error=unused-parameter"
      "-Wextra"

)

# Ustaw opcje konsolidatora (z raportu Dante)
add_link_options(
        "-Wl,-cref"
        "-Wl,-wrap,main"
        "-Wl,-Map=main.map"
        "-ggdb3"
)

# Utwórz target 'project1' i dodaj do niego pliki źródlowe:
add_executable(project1
        "heap.c"
        "main.c"
        "unit_helper_v2.c"
        "unit_test_v2.c"
        "rdebug.c"
        "memmanager.c"
)

# Dołącz niezbędne biblioteki
target_link_libraries(project1
        "m"
        "pthread"
)