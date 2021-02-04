cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 17)

# project(Lichen LANGUAGES CXX VERSION 14)
project(Telegraph)


set(SOURCE_DIR src)
set(TEST_DIR test)
list(APPEND CMAKE_MODULE_PATH cmake_modules)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#GTEST
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

#SDL

#VST3sdk




include_directories(${CMAKE_CURRENT_BINARY_DIR})

#Algae (DSP)
add_library( LibAlgae_dsp STATIC
             libAlgae_dsp/dsp.cpp)

#Fungi (Interpreter)
add_library( LibFungi_lua_interpreter STATIC
             libFungi_lua_interpreter/interpreter.cpp)
target_link_libraries(LibFungi_lua_interpreter ${LUA_LIBRARIES})
target_link_libraries(LibFungi_lua_interpreter LibSol2)
target_link_libraries(LibFungi_lua_interpreter LibAlgae_dsp)



# add_library( LibFungi_interpreter STATIC
#              libFungi_interpreter/src/eval.h
#              ${BISON_MyParser_OUTPUTS}
#              ${FLEX_MyScanner_OUTPUTS}
# )
# target_link_libraries(LibFungi_interpreter ${FLEX_LIBRARIES})



#Production Build
add_executable(
    lichen
    main.cpp
    # ${BISON_MyParser_OUTPUTS}
    # ${FLEX_MyScanner_OUTPUTS}
)
target_link_libraries(lichen LibAlgae_dsp)
target_link_libraries(lichen ${SOUNDIO_LIBRARY})
target_link_libraries(lichen ${LUA_LIBRARIES})
target_link_libraries(lichen LibSol2)
target_link_libraries(lichen LibFungi_lua_interpreter)

install(TARGETS lichen DESTINATION /usr/bin)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

#Tests
# add_executable( LichenSporeLangInterpreterTests 
    # libFungi_interpreter/test/interpreter_test.cpp
    # libFungi_interpreter/test/semantic_analyzer_test.cpp
    # libFungi_interpreter/test/data_variant_test.cpp
    # libFungi_interpreter/test/symbol_variant_test.cpp
    # libFungi_interpreter/test/scoped_symbol_table_test.cpp
    # libFungi_interpreter/test/activation_record_test.cpp
    # libFungi_interpreter/test/maybe_test.cpp
    # libFungi_interpreter/test/either_test.cpp
    # libFungi_interpreter/test/signal_wrapper_test.cpp
    # libFungi_interpreter/test/eval_test.cpp
    # ${BISON_MyParser_OUTPUTS}
    # ${FLEX_MyScanner_OUTPUTS}
# )
# target_link_libraries(LichenSporeLangInterpreterTests GTest::GTest GTest::Main)
# gtest_discover_tests(LichenSporeLangInterpreterTests)

add_executable( LichenTests 
    libFungi_lua_interpreter/test/interpreter_test.cpp
    libFungi_lua_interpreter/test/maybe_test.cpp
    libFungi_lua_interpreter/test/either_test.cpp
    libAlgae_dsp/test/math_functions_test.cpp
    libAlgae_dsp/test/oscillator_test.cpp
    libAlgae_dsp/test/filter_test.cpp
)
target_link_libraries(LichenTests ${SOUNDIO_LIBRARY})
target_link_libraries(LichenTests ${LUA_LIBRARIES})
target_link_libraries(LichenTests LibSol2)
target_link_libraries(LichenTests LibFungi_lua_interpreter)
target_link_libraries(LichenTests GTest::GTest GTest::Main)
gtest_discover_tests(LichenTests)



