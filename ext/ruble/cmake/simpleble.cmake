# TODO: try changing these into functions
function(setup_simpleble build_target)
    include(FetchContent)
    # TODO: would it be easier/better to use git and the commit hash
    #   and build SimpleBLE from source? The main downside is more
    #   dependencies. The upside is vastly simplifying configuring
    #   simpleble_DIR, as well as choosing between shared/static linking.
    FetchContent_Declare(
            simpleble
            URL "${rb_simpleble_asset_browser_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            #            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(simpleble)
    set(simpleble_DIR ${simpleble_SOURCE_DIR}/${rb_simpleble_archive_path})
    find_package(simpleble REQUIRED CONFIG PATHS "${simpleble_DIR}" NO_DEFAULT_PATH)
    target_link_libraries("${build_target}" PUBLIC simpleble::simpleble)
    include("${simpleble_CONFIG}")
    message(STATUS "SimpleBLE found in ${simpleble_DIR}")
endfunction()
