# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(TARGET_TOOL_NAME "armv7a-hf")
SET(ELDK_HOME "/opt/eldk-5.5.3")
SET(TARGET_PATH ${ELDK_HOME}/${TARGET_TOOL_NAME})

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${TARGET_PATH}/sysroots/i686-eldk-linux/usr/bin/arm-linux-gnueabi/arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER ${TARGET_PATH}/sysroots/i686-eldk-linux/usr/bin/arm-linux-gnueabi/arm-linux-gnueabi-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH 
    ${TARGET_PATH}/sysroots/armv7ahf-vfp-neon-linux-gnueabi
    ${TARGET_PATH}/rootfs-lsb-sdk
    /home/snikulov/eldk-${TARGET_TOOL_NAME}-inst
    /home/snikulov/${TARGET_TOOL_NAME})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
