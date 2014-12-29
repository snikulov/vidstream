include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()

ExternalProject_Add(ext.itpp
    GIT_REPOSITORY http://git.code.sf.net/p/itpp/git
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} 
    PREFIX "${DEPS_LOCATION}"
    INSTALL_DIR "${DEPS_LOCATION}"
)

