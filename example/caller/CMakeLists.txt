set(SRC_LIST  calleruserservice.cc  ../user.pb.cc)

add_executable(conserum ${SRC_LIST})

target_link_libraries(conserum mprpc protobuf)