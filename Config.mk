
################################################################################
##                                MULTIVECTOR                                 ##
################################################################################

# Project info
# ===============
PROJECT         := Multivector
VERSION         := 0.1.0
STD_NAMESPACE   := # multivector
GIT_REMOTE_PATH := # Remote path for git repository
MAINTAINER_NAME := Renato Cordeiro Ferreira
MAINTAINER_MAIL := renato.cferreira@hotmail.com
COPYRIGHT       := Renato Cordeiro Ferreira
SYNOPSIS        := Linearized multi-dimensional vectors
DESCRIPTION     := Linearized multi-dimensional vectors with a simple syntax

# Flags
# =======
CPPFLAGS        := # Precompiler Flags
CXXFLAGS        := -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -O3
LDFLAGS         := # Linker flags

# Makeball list
# ===============
include conf/googletest.mk
