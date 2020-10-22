

set(DEPENDENCIES TestAPI FsAPI)

stratify_api_add_test_executable(${PROJECT_NAME} 32768 "${DEPENDENCIES}")



