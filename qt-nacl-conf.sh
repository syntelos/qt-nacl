# https://git.gitorious.org/qtnativeclient/qt.git
# branch 4.8-nacl-sdk-15-newlib
#
# PATH=${PATH}:${NACL_TOOLCHAIN_BIN}
#
# xplatform -> mkspecs/{value}/qmake.conf -> mkspecs/common/{included}
#
# -I ${NACL_INCLUDE} -L ${NACL_LIB} -l nacl -l nacl_dyncode -l nacl_exception -l nacl_list_mappings
#
./configure -prefix /usr/local/qt -nacl ${NACL_SDK_ROOT} -xplatform qws/linux-nacl64-g++ -nomake tests -developer-build -opensource -confirm-license -release -no-sse2 -no-sse3 -no-ssse3 -no-sse4.1 -no-sse4.2

