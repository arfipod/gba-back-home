#---------------------------------------------------------------------------------------------------------------------
# gba-back-home - Butano project Makefile
#---------------------------------------------------------------------------------------------------------------------
TARGET       :=  gba-back-home
BUILD        :=  build
LIBBUTANO    ?=  vendor/butano/butano
PYTHON       ?=  python3

ifndef LIBBUTANOABS
    export LIBBUTANOABS := $(realpath $(LIBBUTANO))
endif

BUTANO_COMMON := $(shell realpath --relative-to="$(CURDIR)" "$(LIBBUTANOABS)/../common")

SOURCES      :=  src
INCLUDES     :=  include $(BUTANO_COMMON)/include
DATA         :=
GRAPHICS     :=  graphics $(BUTANO_COMMON)/graphics
AUDIO        :=  audio
AUDIOBACKEND :=  maxmod
AUDIOTOOL    :=
DMGAUDIO     :=  dmg_audio
DMGAUDIOBACKEND := default
ROMTITLE     :=  BACK HOME
ROMCODE      :=  GBHM
USERFLAGS    :=  -DBN_CFG_LOG_ENABLED=true -DBN_CFG_LOG_BACKEND=BN_LOG_BACKEND_MGBA
USERCXXFLAGS :=
USERASFLAGS  :=
USERLDFLAGS  :=
USERLIBDIRS  :=
USERLIBS     :=
DEFAULTLIBS  :=
STACKTRACE   :=
USERBUILD    :=
EXTTOOL      :=

include $(LIBBUTANOABS)/butano.mak
