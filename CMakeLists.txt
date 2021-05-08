cmake_minimum_required (VERSION 3.8)
#Project Name
project ("PA3")

#find OpenGL
find_package(OpenGL REQUIRED)

#include subdirectory
add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(glm)



#Searching header and cpp files in current folder 
file(GLOB_RECURSE CURRENT_HEADERS *.h)
file(GLOB CURRENT_SOURCES  *.cpp)
#Searching header and cpp files in softbody folder 
file(GLOB_RECURSE CURRENT_HEADERS ./softbody/*.h)
file(GLOB CURRENT_SOURCES  ./softbody/*.cpp)
source_group("Include" FILES ${CURRENT_HEADERS}) 
source_group("Source" FILES  ${CURRENT_SOURCES}) 


#Searching header files 
add_executable(PA3  "main.cpp" ${CURRENT_HEADERS} ${CURRENT_SOURCES})
target_include_directories(PA3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
target_include_directories(PA3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
target_include_directories(PA3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glm)


#Add to the libraries
target_link_libraries(PA3
        glfw
        libglew_static
        glm
        )

#Add config shaders to BIN
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/core.vs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/core.fs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/shadow.vs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/shadow.fs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/texture.vs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/texture.fs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)   

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/softbody/resources/shaders/core.vs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/softbody/resources/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/softbody/resources/shaders/core.fs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/softbody/resources/shaders)   
   
     
#Add config texture to BIN
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/box.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)  
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/box1.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)   
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/leather1.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)   
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/metal.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)       
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/model1.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)    
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/model2.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)      
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/model3.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)      
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/model4.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/model5.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)   
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/tiled.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/wood.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/tiled.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)     
     
   