# --------------------------------------------------------------------------------------------------------------------------------------------------------------------#

# --------------------------------------------------------------------------------------------------------------------------------------------------------------------#
#				         				IF YOU WANT TO EDIT VARIABLES IN THIS FILE DO THIS BY ENV VARIABLES OR THE CUSTOM FILE				     					  #
#---------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# ------------ Definitions ------------ # # --------------------------- Defualt Values --------------------------- # # ------------------ Comments -------------------#
# ------------------------------------------------------------------------- General ----------------------------------------------------------------------------------#
BUILD_NAME	  								?=		  NovaHostingSetup#				  # Build Name											  	 		  			  #
BUILD_SYSTEM_VERSION						?=		  0.0.1#						  # Build System Version (Major.Minor.Patch)					  				  #
SOURCE_DIRECTORY							?= 		  ${shell pwd}/Source#		  	  # Build Directory												  				  #
BUILD_DIRECTORY								?= 		  ${shell pwd}/Binaries#		  # Source Directory												  		  	  #
BUILD_OBJECT_DIRECTORY						?= 		  ${shell pwd}/Intermediate#	  # Build Object Directory												  		  #
THIRD_PARTY_DIRECTORY						?= 		  ${shell pwd}/ThirdParty#		  # Third Party Directory												  		  #
# ------------------------------------------------------------------- Build Configuration ----------------------------------------------------------------------------#
BUILD_CONFIGURATION 						?=		  Development#  				  # Build configuration (Production, Staging, Development, Testing)				  #
BUILD_TOOLCHAIN								?=		  g++# 	  					      # Build toolchain								  				  				  #
BUILD_FLAGS									?=		  -std=c++17 -Wall -g#			  # Build flags											  						  #
BUILD_OPTIMSATIONS							?=		  -O3#							  # Build optimsations											  				  #
# --------------------------------------------------------------------------------------------------------------------------------------------------------------------#

#This adds the source public directory to the include paths
PUBLIC_DIRECTORY = ${SOURCE_DIRECTORY}/Public
BUILD_FLAGS += -I ${PUBLIC_DIRECTORY}
BUILD_FLAGS += -I ${PUBLIC_DIRECTORY}/Core

#This adds the ThirdParty include directory to the include paths
THIRD_PARTY_INCLUDE_DIRECTORY = ${SOURCE_DIRECTORY}/ThirdParty
BUILD_FLAGS += -I ${THIRD_PARTY_INCLUDE_DIRECTORY}
BUILD_FLAGS += -I ${THIRD_PARTY_INCLUDE_DIRECTORY}/ftxui
BUILD_FLAGS += -I ${THIRD_PARTY_INCLUDE_DIRECTORY}/restclient-cpp
BUILD_FLAGS += -I ${THIRD_PARTY_INCLUDE_DIRECTORY}/json

#This adds the third party libraries to the link paths
FTXUI_LIB_DIR := ${THIRD_PARTY_DIRECTORY}/ftxui
THIRD_PARTY_LIBRARIES += ${FTXUI_LIB_DIR}/libftxui-component.a
THIRD_PARTY_LIBRARIES += ${FTXUI_LIB_DIR}/libftxui-dom.a
THIRD_PARTY_LIBRARIES += ${FTXUI_LIB_DIR}/libftxui-screen.a
THIRD_PARTY_LIBRARIES += -lpthread

RESTCLIENT_LIB_DIR := ${THIRD_PARTY_DIRECTORY}/restclient
THIRD_PARTY_LIBRARIES += ${RESTCLIENT_LIB_DIR}/librestclient-cpp.a
THIRD_PARTY_LIBRARIES += ${RESTCLIENT_LIB_DIR}/librestclient-cpp.so
THIRD_PARTY_LIBRARIES += -lcurl