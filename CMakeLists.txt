cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 17)

project(telegraph)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#GTEST
# add_subdirectory(${PROJECT_SOURCE_DIR}/googletest)
# include(AddGTest)
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

#SDL


#VST3sdk
set(VST3_SDK_ROOT "${PROJECT_SOURCE_DIR}/vst3sdk" CACHE PATH "Location of VST3 SDK")
include(AddVST3SDK)
# find_package( VST3SDK  )

# if( VST3SDK_FOUND )
#     target_include_directories( ${PROJECT_NAME} PRIVATE ${VST3SDK_INCLUDE_DIR} )
#     target_link_libraries( ${PROJECT_NAME} ${VST3SDK_LIBRARIES} )
# else()
#     message("VST SDK was NOT FOUND!")
# endif()

# include_directories(${CMAKE_CURRENT_BINARY_DIR})

#Algae (DSP)
# add_library( algae STATIC algae/src/dsp.cpp)
set(ALGAE_ROOT ${PROJECT_SOURCE_DIR}/algae)


#Production Build
# add_executable(
#     telegraph
#     main.cpp
# )
# target_link_libraries(telegraph LibAlgae_dsp)
set(TELEGRAPH_SOURCES ${PROJECT_SOURCE_DIR}/src/telegraph-vst-shell)
set(telegraph-vst_sources

    ${TELEGRAPH_SOURCES}/mdaBaseController.cpp
    ${TELEGRAPH_SOURCES}/mdaBaseController.h
    ${TELEGRAPH_SOURCES}/mdaBaseProcessor.cpp
    ${TELEGRAPH_SOURCES}/mdaBaseProcessor.h
    ${TELEGRAPH_SOURCES}/mdaParameter.cpp
    ${TELEGRAPH_SOURCES}/mdaParameter.h
    ${TELEGRAPH_SOURCES}/telegraphVersion.h
    ${TELEGRAPH_SOURCES}/telegraphFactory.cpp
    ${TELEGRAPH_SOURCES}/telegraphSynthController.cpp
    ${TELEGRAPH_SOURCES}/telegraphSynthController.h
    ${TELEGRAPH_SOURCES}/telegraphSynthProcessor.cpp
    ${TELEGRAPH_SOURCES}/telegraphSynthProcessor.h
)
#SDL

#VST
# set(SMTG_LINUX TRUE)
set(target telegraph)
# set(target ${PROJECT_NAME})
set(VST3_OUTPUT_DIR ${CMAKE_BINARY_DIR}/VST3)
# smtg_enable_vst3_sdk()
smtg_add_vst3plugin(${target} ${VST3_SDK_ROOT} ${telegraph-vst_sources})
# set_target_properties(${target} PROPERTIES ${SDK_IDE_PLUGIN_EXAMPLES_FOLDER})
target_include_directories(${target} PRIVATE ${VST3_SDK_ROOT})
target_include_directories(${target} PUBLIC ${ALGAE_ROOT})
# target_link_libraries(${target} PRIVATE base sdk)
target_link_libraries(${target} PRIVATE base sdk algae)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -ftree-vectorize")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -fno-rtti -ftree-vectorize -std=c++17")



# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

#Tests

# add_executable( TelegraphTests 
    # libFungi_lua_interpreter/test/interpreter_test.cpp
    # libFungi_lua_interpreter/test/maybe_test.cpp
    # libFungi_lua_interpreter/test/either_test.cpp
    # libAlgae_dsp/test/math_functions_test.cpp
    # libAlgae_dsp/test/oscillator_test.cpp
    # libAlgae_dsp/test/filter_test.cpp
# )
# target_link_libraries(TelegraphTests telegraph_core)
# target_link_libraries(TelegraphTests GTest::GTest GTest::Main)
# gtest_discover_tests(TelegraphTests)



