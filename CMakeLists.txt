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

#Algae (DSP)
set(ALGAE_ROOT ${PROJECT_SOURCE_DIR}/algae)
# add_subdirectory(${ALGAE_ROOT}) #done automatically by vst cmake script :(


#CORE PROCESSING CODE
set(TELEGRAPH_CORE_ROOT ${PROJECT_SOURCE_DIR}/src/telegraph-core/)
add_library( telegraph-core STATIC ${TELEGRAPH_CORE_ROOT}/voice.cpp)
set_target_properties(telegraph-core PROPERTIES PUBLIC_HEADER ${TELEGRAPH_CORE_ROOT}/telegraph-core.h)
target_include_directories(telegraph-core PUBLIC 
    ${ALGAE_ROOT}/src)
target_link_libraries(telegraph-core PRIVATE algae )


# get_target_property(TC_INCLUDES telegraph-core INCLUDE_DIRECTORIES)
# message("THE PATH FOR CORE IS..." ${TC_INCLUDES})
# message("INTERFACE PATH IS..." ${INTERFACE_LINK_LIBRARIES})

#Production Build



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




# #VST Target
set(target telegraph)
set(VST3_OUTPUT_DIR ${CMAKE_BINARY_DIR}/VST3)
smtg_add_vst3plugin(${target} ${VST3_SDK_ROOT} ${telegraph-vst_sources})
target_include_directories(${target} PRIVATE ${VST3_SDK_ROOT})
target_include_directories(${target} PUBLIC ${ALGAE_ROOT} ${TELEGRAPH_CORE_ROOT})
target_link_libraries(${target} PRIVATE base sdk telegraph-core)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -ftree-vectorize")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -fno-rtti -ftree-vectorize -std=c++17")



# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

#Tests

add_executable( TelegraphTests 
    ${PROJECT_SOURCE_DIR}/test/telegraph-core/voice_test.cpp
   
)
target_include_directories(TelegraphTests PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(TelegraphTests GTest::GTest GTest::Main telegraph-core)
gtest_discover_tests(TelegraphTests)



