
if (NOT DEFINED API_IS_SDK)
  include(API)
  include(SosAPI)
  sos_sdk_include_target(HalAPI "${API_CONFIG_LIST}")
endif ()
