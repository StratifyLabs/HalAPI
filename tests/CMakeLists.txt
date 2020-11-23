

set(DEPENDENCIES TestAPI FsAPI)

api_add_test_executable(${PROJECT_NAME} 32768 "${DEPENDENCIES}")



