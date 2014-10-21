add_subdirectory(split)
add_subdirectory(config)
add_subdirectory(corrupt)
add_subdirectory(perf)
add_subdirectory(channel)

if(UNIX)
    add_subdirectory(bch_codec)
endif()

if(WITH_ITPP)
    add_subdirectory(itpp)
endif()