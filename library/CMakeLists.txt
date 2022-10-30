cmsdk2_check_version(
  NAME SosAPI
  VERSION ${SosAPI_VERSION}
  MINIMUM_VERSION 1.4.4)

api2_add_library(
  NAME ${PROJECT_NAME}
  DEPENDENCIES SosAPI FsAPI ThreadAPI)



