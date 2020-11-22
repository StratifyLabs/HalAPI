
if(NOT DEFINED IS_SDK)
	include(API)
	sos_sdk_include_target(HalAPI "${API_CONFIG_LIST}")
endif()
