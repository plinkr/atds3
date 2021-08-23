set PATH=c:\qt\5.15.2\tools\cmake_64\bin;c:\qt\tools\ninja;%PATH%
set NDK_ROOT=c:\android\sdk\ndk\21.3.6528147

mkdir _construccion
cd _construccion

cmake -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_VERBOSE_MAKEFILE=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=%NDK_ROOT%/build/cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_NDK=%NDK_ROOT% -DANDROID_TOOLCHAIN=clang -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_STL=c++_shared -DANDROID_LINKER_FLAGS="-landroid -llog -lz" -DANDROID_CPP_FEATURES="rtti exceptions" -DANDROID_ABI=x86 -DCMAKE_INSTALL_PREFIX=android/x86 -G "Ninja" ../cmake
cmake --build . --target install
cmake --build . --target clean
cmake -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_VERBOSE_MAKEFILE=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=%NDK_ROOT%/build/cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_NDK=%NDK_ROOT% -DANDROID_TOOLCHAIN=clang -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_STL=c++_shared -DANDROID_LINKER_FLAGS="-landroid -llog -lz" -DANDROID_CPP_FEATURES="rtti exceptions" -DANDROID_ABI=x86_64 -DCMAKE_INSTALL_PREFIX=android/x86_64 -G "Ninja" ../cmake
cmake --build . --target install
cmake --build . --target clean
cmake -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_VERBOSE_MAKEFILE=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=%NDK_ROOT%/build/cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_NDK=%NDK_ROOT% -DANDROID_TOOLCHAIN=clang -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_STL=c++_shared -DANDROID_LINKER_FLAGS="-landroid -llog -lz" -DANDROID_CPP_FEATURES="rtti exceptions" -DANDROID_ABI=armeabi-v7a -DCMAKE_INSTALL_PREFIX=android/armeabi-v7a -G "Ninja" ../cmake
cmake --build . --target install
cmake --build . --target clean
cmake -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_VERBOSE_MAKEFILE=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=%NDK_ROOT%/build/cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_NDK=%NDK_ROOT% -DANDROID_TOOLCHAIN=clang -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_STL=c++_shared -DANDROID_LINKER_FLAGS="-landroid -llog -lz" -DANDROID_CPP_FEATURES="rtti exceptions" -DANDROID_ABI=arm64-v8a -DCMAKE_INSTALL_PREFIX=android/arm64-v8a -G "Ninja" ../cmake
cmake --build . --target install
cmake --build . --target clean
