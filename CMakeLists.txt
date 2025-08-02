cmake_minimum_required(VERSION 3.22)

enable_language(C)

set(Ebi_Defines_Syms 

)

set(Ebi_Include_Dirs
    ${CMAKE_SOURCE_DIR} inc
)

set(Ebi_Src
    ${CMAKE_SOURCE_DIR} src/str/str.c
    ${CMAKE_SOURCE_DIR} src/dsa/list.c
    ${CMAKE_SOURCE_DIR} src/dsa/dict.c
    ${CMAKE_SOURCE_DIR} src/dsa/sta/stack.c
    ${CMAKE_SOURCE_DIR} src/dsa/sta/queue.c
    ${CMAKE_SOURCE_DIR} src/dsa/dyn/stack.c
    ${CMAKE_SOURCE_DIR} src/dsa/dyn/queue.c
    ${CMAKE_SOURCE_DIR} src/json/json.c
)

add_library(ebi OBJECT)
target_compile_definitions(ebi PUBLIC ${Ebi_Defines_Syms})
target_include_directories(ebi PUBLIC ${Ebi_Include_Dirs})
target_sources(ebi PRIVATE ${Ebi_Src})

if((CMAKE_C_STANDARD EQUAL 90))
    message(ERROR "Generated code requires C99 or higher")
endif()
