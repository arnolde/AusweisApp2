SET(REMOTE_CONFIG_URL_PROD https://appl.governikus-asp.de/ausweisapp2)
IF(JENKINS_APPCAST)
	SET(REMOTE_CONFIG_URL https://ausweisapp-ci.govkg.de/job/${JENKINS_APPCAST}/lastSuccessfulBuild/artifact CACHE STRING "Remote config download URL" FORCE)
ELSE()
	SET(REMOTE_CONFIG_URL ${REMOTE_CONFIG_URL_PROD} CACHE STRING "Remote config download URL" FORCE)
ENDIF()

FUNCTION(CONFIGURE_DEFAULT_FILES _destination)
	IF(JENKINS_APPCAST)
		SET(REMOTE_CONFIG_PATH_APPCAST /build)
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /build)
		SET(REMOTE_CONFIG_PATH_PROVIDERS /source/resources)
		SET(REMOTE_CONFIG_PATH_READERS /source/resources)
	ELSE()
		SET(REMOTE_CONFIG_PATH_APPCAST )
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /beta)
		SET(REMOTE_CONFIG_PATH_PROVIDERS )
		SET(REMOTE_CONFIG_PATH_READERS /reader)
	ENDIF()
	SET(REMOTE_CONFIG_URL_READER_IMAGES ${REMOTE_CONFIG_URL_PROD}/reader)

	# Copy secure storage file, so that the AusweisApp2 can be started from the build directory.
	CONFIGURE_FILE(${RESOURCES_DIR}/config.json.in ${_destination}/config.json @ONLY)

	# Copy qtlogging.ini file
	CONFIGURE_FILE(${RESOURCES_DIR}/qtlogging.ini ${_destination}/qtlogging.ini COPYONLY)
ENDFUNCTION()
