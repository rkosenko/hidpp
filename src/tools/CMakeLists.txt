cmake_minimum_required(VERSION 2.8)
project(hidpp_tools)

include_directories(../libhidpp)

add_library(common OBJECT
	common/common.cpp
	common/Option.cpp
	common/CommonOptions.cpp)

add_executable(hidpp-check-device
	hidpp-check-device.cpp
	$<TARGET_OBJECTS:common>)
target_link_libraries(hidpp-check-device hidpp ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS hidpp-check-device RUNTIME DESTINATION bin)

set(TOOLS
	hidpp-list-devices
	hidpp-list-features
	hidpp-mouse-resolution
	hidpp10-dump-page
	hidpp10-write-page
	hidpp10-raw-command
	hidpp10-active-profile
	hidpp20-call-function
	hidpp20-onboard-profiles-get-description
	hidpp20-reprog-controls
	hidpp20-led-control
	hidpp20-dump-page
	hidpp20-write-page
	hidpp20-write-data
	hidpp20-init-call
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(TOOLS ${TOOLS}
		hidpp20-mouse-event-test
		hidpp20-raw-touchpad-driver
	)
endif()

foreach(TOOL_NAME ${TOOLS})
	add_executable(${TOOL_NAME}
		${TOOL_NAME}.cpp
		$<TARGET_OBJECTS:common>)
	target_link_libraries(${TOOL_NAME}
		hidpp
		${CMAKE_THREAD_LIBS_INIT}
	)
	install(TARGETS ${TOOL_NAME} RUNTIME DESTINATION bin)
endforeach()

if(TINYXML2_FOUND)
	add_library(profile OBJECT
		profile/MacroText.cpp
		profile/ProfileXML.cpp)
	
	foreach(TOOL_NAME
		hidpp-persistent-profiles
		hidpp10-load-temp-profile
	)
		add_executable(${TOOL_NAME}
			${TOOL_NAME}.cpp
			$<TARGET_OBJECTS:profile>
			$<TARGET_OBJECTS:common>)
		target_include_directories(${TOOL_NAME}
			PUBLIC ${TINYXML2_INCLUDE_DIRECTORIES})
		target_link_libraries(${TOOL_NAME}
			hidpp
			${TINYXML2_LIBRARIES}
			${CMAKE_THREAD_LIBS_INIT}
		)
		install(TARGETS ${TOOL_NAME} RUNTIME DESTINATION bin)
	endforeach()
	
else()
	message("Profile tools require tinyxml2.")
endif()
