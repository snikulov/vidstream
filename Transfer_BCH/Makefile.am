bin_PROGRAMS = main encode send read decode
ECC_LIB_SRC = ecc.cpp ecc.h bch.cpp bch.h err.cpp err.h transport.h transport.cpp threaded_coder.h bshift.cpp bshift.h

main_SOURCES = main.cpp $(ECC_LIB_SRC)
main_LDADD = -lrt -lpthread

read_SOURCES = main_read.cpp $(ECC_LIB_SRC)
read_LDADD = -lrt -lpthread

send_SOURCES = main_send.cpp $(ECC_LIB_SRC)
send_LDADD = -lrt -lpthread

encode_SOURCES = main_encode.cpp $(ECC_LIB_SRC)
encode_LDADD = -lrt -lpthread

decode_SOURCES = main_decode.cpp $(ECC_LIB_SRC)
decode_LDADD = -lrt -lpthread
