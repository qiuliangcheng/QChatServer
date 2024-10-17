aux_source_directory(./ SRC_LIST)
add_executable(ChatClient ${SRC_LIST})
target_link_libraries(ChatClient pthread)
message("这是一个测试 client里面")


