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
