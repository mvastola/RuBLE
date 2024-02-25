function(get_restorable_save_name var_name)
  #set(old_var_name "${var_name}_old")
  string(REGEX REPLACE "^(ENV|CACHE)\{(.*)\}$" 
    "old_\\1_\\2" old_var_name "${var_name}")
  set(var_special_type "${CMAKE_MATCH_1}")
  return(PROPAGATE old_var_name var_special_type)
endfunction()

function(set_restorable var_name)
  get_restorable_save_name(${var_name}) 
  unset(${old_var_name} PARENT_SCOPE)
  # Only save if var exists
  if(DEFINED ${var_name})
    set(${old_var_name} ${${var_name}} PARENT_SCOPE)
  endif()
  if (var_special_type STREQUAL "ENV")
    set(${var_name} ${ARGN})
  else()
    set(${var_name} ${ARGN} PARENT_SCOPE)
  endif()
endfunction()

function(set_restore_old var_name)
  get_restorable_save_name(${var_name}) 
  unset(${var_name} PARENT_SCOPE)
  # Only save if var exists
  if(DEFINED ${old_var_name})
    set(${var_name} ${${old_var_name}} PARENT_SCOPE)
  endif()
  if (var_special_type STREQUAL "ENV")
    unset(${old_var_name})
  else()
    unset(${old_var_name} PARENT_SCOPE)
  endif()
endfunction()

function(get_pkg_config_lib_version OUT_VAR LIB_NAME) # + optional PKG_CONFIG_PATH
    if (ARGC GREATER 2)
	if(DEFINED ENV{PKG_CONFIG_PATH})
		set(old_pkg_config_path "$ENV{PKG_CONFIG_PATH}")
        endif()
	set(ENV{PKG_CONFIG_PATH} "${ARGV2}")
    endif()

    execute_process(
        COMMAND pkg-config "${LIB_NAME}" --modversion --env-only
        OUTPUT_VARIABLE ${OUT_VAR}
        #        OUTPUT_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    if(ARGC GREATER 2)
        if (DEFINED old_pkg_config_path)
            set(ENV{PKG_CONFIG_PATH} "${old_pkg_config_path}")
        else()
	    unset(ENV{PKG_CONFIG_PATH})
        endif()
    endif()
    return(PROPAGATE ${OUT_VAR})
endfunction()
