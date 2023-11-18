set(_Ruby_DEBUG_OUTPUT ${RUBLE_VERBOSE})
set(RUBY_FIND_VERSION ${rb_RUBY_API_VERSION})

# TODO: maybe used fixed path from build-config.cmake (or at least give hints)?
find_package(Ruby 3.2 REQUIRED Ruby_FIND_VIRTUALENV)
target_link_libraries(${EXTENSION_NAME} PUBLIC ${Ruby_LIBRARIES})
target_include_directories(${EXTENSION_NAME} PUBLIC ${Ruby_INCLUDE_DIRS})
target_include_directories(${EXTENSION_NAME} PRIVATE ${RICE_INCLUDE_PATH})


# FIXME: set rpath to rb_LIBRUBYARG_SHARED instead
set_target_properties(${EXTENSION_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH ${RUBLE_DEVELOPER})

# From the rice repo when it used cmake
if (MSVC)
  target_compile_definitions(${EXTENSION_NAME} PRIVATE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  target_compile_options(${EXTENSION_NAME} PRIVATE /bigobj)
elseif (MINGW)
  target_compile_options(${EXTENSION_NAME} PRIVATE -Wa,-mbig-obj)
endif ()

# TODO: ensure flags from RbConfig propagate (e.g. enabling stack protection)
message(STATUS "libruby ${Ruby_VERSION} found at ${Ruby_LIBRARY}")
message(STATUS "Rice ${RICE_VERSION} found in ${RICE_INCLUDE_PATH}")
