aux_source_directory(. SRC_LIST)
# set(EXECUTABLE_OUTPUT_PATH ${EXEC_PATH})
# 指定生成可执行文件

add_executable(test_server test_server.cpp) #这个会自己找这些src里面的main函数 
# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(test_server  pthread)
add_executable(test_client test_client.cpp) #这个会自己找这些src里面的main函数 
# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(test_client  pthread)
message("这是一个测试 server里面")