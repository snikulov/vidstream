# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(TARGET_TOOL_NAME "cortexa9t2hf-vfp-neon")
SET(CROSS_TOOL_HOME "/opt/poky/1.6.2/sysroots")

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${CROSS_TOOL_HOME}/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER ${CROSS_TOOL_HOME}/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH 
    ${CROSS_TOOL_HOME}/cortexa9t2hf-vfp-neon-poky-linux-gnueabi
    )

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
