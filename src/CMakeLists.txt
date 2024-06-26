file(GLOB_RECURSE SRC_FILES 
     "*.cpp"
     "*.h")

target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
