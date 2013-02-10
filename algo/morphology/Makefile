
MODULE_NAME = morphology
INTERFACE_DIR = ../interface/

############################################################################ 
# Языко-зависимые параметры

COMPILER = ghc
INTERFACE = ${INTERFACE_DIR}/interface.hs
SOURCE_FILE_SUFFIX = hs
FLAGS = --make -dynamic -shared -fPIC -threaded -no-hs-main -odir ${MODULE_BUILD_DIR} -hidir ${MODULE_BUILD_DIR} -stubdir ${MODULE_BUILD_DIR}
DEBUG_FLAGS =
RELEASE_FLAGS = -O2

############################################################################ 
# Независимые параметры

BUILD_DIR = ../../build
MODULE_BUILD_DIR = ${BUILD_DIR}/${MODULE_NAME}
DESTINATION = ${BUILD_DIR}/libamv_${MODULE_NAME}.so
SOURCES = init.cpp ${MODULE_NAME}.${SOURCE_FILE_SUFFIX}

debug: wash

	ghc ${FLAGS} ${DEBUG_FLAGS} ${SOURCES} ${INTERFACE} -o ${DESTINATION}

release: wash

	ghc ${FLAGS} ${RELEASE_FLAGS} ${SOURCES} ${INTERFACE}  -o ${DESTINATION}

wash:

	mkdir -p ${MODULE_BUILD_DIR}
	rm -f ${MODULE_BUILD_DIR}/*.o

clean:

	rm -Rf ${MODULE_BUILD_DIR}
	rm -f ${DESTINATION}

