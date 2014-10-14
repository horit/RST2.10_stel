# default .profile.toolkit

#####################################################################
#                                                                   #
# Base Installation directory                                       #
#                                                                   #
#####################################################################

# Note. Uncomment the following line and set it to the directory
# that the software is RST software is installed in.

export HOMEPATH=/data1/g3/nisitani/sddata/software/current

#####################################################################
#                                                                   #
# Compilation directives                                            #
#                                                                   #
#####################################################################

# Path of the X11 packages

export XPATH="/usr/X11R6"

# Compile netCDF software

# export NETCDF_PATH="/usr"

# pathname for the CDF software

export CDF_PATH="$HOMEPATH/cdf31-dist"

# IDL header directory

#export IDL_IPATH="/usr/local/rsi/idl/external/include"
export IDL_IPATH="/usr/local/itt/idl/external/include"

#####################################################################
#                                                                   #
# IDL Configuration                                                 #
#                                                                   #
#####################################################################

#export IDL_PATH="+/usr/local/rsi:+/${HOMEPATH}/idl/lib"
export IDL_PATH="+/usr/local/itt:+/${HOMEPATH}/idl/lib"
export IDL_STARTUP="${HOMEPATH}/idl/startup.pro"

#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

export CODEBASE=${HOMEPATH}/code
export LOGPATH=${HOMEPATH}/log
export DOCPATH=${HOMEPATH}/doc

export LIBPATH=${HOMEPATH}/lib
export BINPATH=${HOMEPATH}/bin
export IPATH=${HOMEPATH}/include

export USR_CODEBASE=${HOMEPATH}/usr/code

export USR_LIBPATH=${HOMEPATH}/usr/lib
export USR_BINPATH=${HOMEPATH}/usr/bin
export USR_IPATH=${HOMEPATH}/usr/include
export PATH=${PATH}:${HOMEPATH}/bin:${HOMEPATH}/script:${HOMEPATH}/usr/bin

#####################################################################
#                                                                   #
# Base Data Tables                                                  #
#                                                                   #
#####################################################################

export FONTPATH="$HOMEPATH/tables/base/fonts"
export FONTDB="$HOMEPATH/tables/base/fonts/fontdb.xml"
export MAPDATA="$HOMEPATH/tables/general/map_data"
export BNDDATA="$HOMEPATH/tables/general/bnd_data"
export MAPOVERLAY="$HOMEPATH/tables/general/overlay.dat"

#####################################################################
#                                                                   #
# SuperDARN Data Tables                                             #
#                                                                   #
#####################################################################

export SD_HDWPATH="$HOMEPATH/tables/superdarn/hdw/"
export SD_RADAR="$HOMEPATH/tables/superdarn/radar.dat"

export AACGM_DAT_PREFIX="$HOMEPATH/tables/aacgm/aacgm_coeffs"
export IGRF_PATH="$HOMEPATH/tables/mag/"
export SD_MODEL_TABLE="$HOMEPATH/tables/superdarn/model"

#####################################################################
#                                                                   #
# IDL Libraries                                                     #
#                                                                   #
#####################################################################

export LIB_FITIDL="${HOMEPATH}/lib/libfitidl.1.so"
export LIB_RAWIDL="${HOMEPATH}/lib/librawidl.1.so"
export LIB_OLDFITIDL="${HOMEPATH}/lib/liboldfitidl.1.so"
export LIB_OLDRAWIDL="${HOMEPATH}/lib/liboldrawidl.1.so"
export LIB_RPOSIDL="${HOMEPATH}/lib/librposidl.1.so"
export LIB_AACGMIDL="${HOMEPATH}/lib/libaacgmidl.1.so"
export LIB_IGRFIDL="${HOMEPATH}/lib/libigrfidl.1.so"
export LIB_GEOPIDL="${HOMEPATH}/lib/libgeopidl.1.so"

#####################################################################
#                                                                   #
# Legacy environment variables                                      #
#                                                                   #
#####################################################################

export SD_TABLES="${HOMEPATH}/tables/idl"
export SD_RAWROPEN_PATH=":/data/raw/:."
export SD_FITROPEN_PATH=":/data/fit/:."
export SD_IDL="${HOMEPATH}/idl/"

#####################################################################
#                                                                   #
# Location of makefiles                                             #
#                                                                   #
#####################################################################
export MAKEBIN=${CODEBASE}/make/makebin.linux64
export MAKELIB=${CODEBASE}/make/makelib.linux64
export MAKELIBIDL=${CODEBASE}/make/makelibidl.linux64
