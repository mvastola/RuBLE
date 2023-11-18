function(get_pkg_config_lib_version OUT_VAR LIB_NAME) # + optional PC DIR
    if (ARGC GREATER 2)
        if(DEFINED ENV{PKG_CONFIG_LIBDIR})
            set(old_pkg_config_libdir "$ENV{PKG_CONFIG_LIBDIR}")
        endif()
        set($ENV{PKG_CONFIG_LIBDIR} "${ARGV2}")
    endif()

    execute_process(
        COMMAND pkg-config "${LIB_NAME}" --modversion
        OUTPUT_VARIABLE ${OUT_VAR}
        #        OUTPUT_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    if(ARGC GREATER 2)
        if (DEFINED old_pkg_config_libdir)
            set(ENV{PKG_CONFIG_LIBDIR} "${old_pkg_config_libdir}")
        else()
            unset(ENV{PKG_CONFIG_LIBDIR})
        endif()
    endif()
    return(PROPAGATE ${OUT_VAR})
endfunction()