# Descargador de Archivos de la Red Todus (S3)

DARTS3 es una aplicación para consola que automatiza el proceso de descarga de archivos desde los servidores de la red Todus (S3).

## Tabla de contenidos
- [Características](#caracteristicas)
- [Dependencias](#dependencias)
- [Construcción](#construccion)
- [Installing dependencies](#installing-dependencies)
- [Using in CMake C++ projects](#using-cxx)
- [Using in Java projects](#using-java)
- [Using in .NET projects](#using-dotnet)
- [Using with other programming languages](#using-json)
- [License](#license)

<a name="caracteristicas"></a>
## Características

`DARTS3` posee las siguientes características::

* **Multiplataforma**: `DARTS3` puede ser utilizado en cualquier sistema operativo que en donde la librería Qt pueda funcionar: UNIX (FreeBSD, NetBSD, OpenBSD), Linux, macOS y Windows.
* **A prueba de fallos**: `DARTS3` es consistente y a prueba de fallos. Por lo que una vez iniciado, no se cerrará abruptamente en caso de algún error.
* **Fácil de usar**: `DARTS3` es fácil de usar gracias a la intuitiva interfaz de usuario que posee.
* **Completamente asincrónico**: `DARTS3` es totalmente asincrónico, por lo que no se bloqueará debido a que realice varias operaciones al mismo tiempo dentro del programa, tales como descargar y agregar un nuevo listado a la descarga.

<a name="dependencias"></a>
## Dependencias
`DARTS3` depende de:

* Compilador C++ compatible con el estándar C++20 (Clang 3.4+, GCC 4.9+, MSVC 19.0+ (Visual Studio 2015+), Intel C++ Compiler 17+)
* Qt

<a name="construccion"></a>
## Construcción

La forma mas simple de construir `DARTS3`:

```
mkdir _construccion
cd _construccion
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target all
```

<a name="installing-dependencies"></a>
### Installing dependencies

<a name="macos"></a>
#### macOS
* Install the latest Xcode command line tools, for example, via `xcode-select --install`.
* Install other [dependencies](#dependencies), for example, using [Homebrew](https://brew.sh):
```
brew install gperf cmake openssl
```
* Build `TDLib` with CMake as explained in [building](#building). You will likely need to manually specify path to the installed OpenSSL to CMake, e.g.,
```
cmake -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl/ ..
```

<a name="windows"></a>
#### Windows
* Download and install Microsoft Visual Studio 2015 or later.
* Download and install [gperf](https://sourceforge.net/projects/gnuwin32/files/gperf/3.0.1/). Add the path to gperf.exe to the PATH environment variable.
* Install [vcpkg](https://github.com/Microsoft/vcpkg#quick-start).
* Run the following commands to install `TDLib` dependencies using vcpkg:
```
cd <path to vcpkg>
.\vcpkg.exe install openssl:x64-windows openssl:x86-windows zlib:x64-windows zlib:x86-windows
```
* Download and install [CMake](https://cmake.org/download/); choose "Add CMake to the system PATH" option while installing.
* Build `TDLib` with CMake as explained in [building](#building), but instead of `cmake -DCMAKE_BUILD_TYPE=Release ..` use
```
cmake -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg>/scripts/buildsystems/vcpkg.cmake ..
```

To build 32-bit/64-bit `TDLib` using MSVC, you will need to additionally specify parameter `-A Win32`/`-A x64` to CMake.
To build `TDLib` in Release mode using MSVC, you will need to additionally specify parameter `--config Release` to the `cmake --build .` command.

<a name="linux"></a>
#### Linux
* Install all [dependencies](#dependencies) using your package manager.

<a name="using-cxx"></a>
## Using in CMake C++ projects
For C++ projects that use CMake, the best approach is to build `TDLib` as part of your project or to install it system-wide.

There are several libraries that you could use in your CMake project:

* Td::TdJson, Td::TdJsonStatic — dynamic and static version of a JSON interface. This has a simple C interface, so it can be easily used with any programming language that is able to execute C functions.
  See [td_json_client](https://core.telegram.org/tdlib/docs/td__json__client_8h.html) and [td_log](https://core.telegram.org/tdlib/docs/td__log_8h.html) documentation for more information.
* Td::TdStatic — static library with C++ interface for general usage.
  See [Client](https://core.telegram.org/tdlib/docs/classtd_1_1_client.html) and [Log](https://core.telegram.org/tdlib/docs/classtd_1_1_log.html) documentation for more information.
* Td::TdCoreStatic — static library with low-level C++ interface intended mostly for internal usage.
  See [ClientActor](https://core.telegram.org/tdlib/docs/classtd_1_1_client_actor.html) and [Log](https://core.telegram.org/tdlib/docs/classtd_1_1_log.html) documentation for more information.

For example, part of your CMakeLists.txt may look like this:
```
add_subdirectory(td)
target_link_libraries(YourTarget PRIVATE Td::TdStatic)
```

Or you could install `TDLib` and then reference it in your CMakeLists.txt like this:
```
find_package(Td 1.7.0 REQUIRED)
target_link_libraries(YourTarget PRIVATE Td::TdStatic)
```
See [example/cpp/CMakeLists.txt](https://github.com/tdlib/td/tree/master/example/cpp/CMakeLists.txt).

<a name="using-java"></a>
## Using in Java projects
`TDLib` provides native Java interface through JNI. To enable it, specify option `-DTD_ENABLE_JNI=ON` to CMake.

See [example/java](https://github.com/tdlib/td/tree/master/example/java) for example of using `TDLib` from Java and detailed build and usage instructions.

<a name="using-dotnet"></a>
## Using in .NET projects
`TDLib` provides native .NET interface through `C++/CLI` and `C++/CX`. To enable it, specify option `-DTD_ENABLE_DOTNET=ON` to CMake.
.NET Core supports `C++/CLI` only since version 3.1 and only on Windows, so if older .NET Core is used or portability is needed, then `TDLib` JSON interface should be used through P/Invoke instead.

See [example/csharp](https://github.com/tdlib/td/tree/master/example/csharp) for example of using `TDLib` from C# and detailed build and usage instructions.
See [example/uwp](https://github.com/tdlib/td/tree/master/example/uwp) for example of using `TDLib` from C# UWP application and detailed build and usage instructions for Visual Studio Extension "TDLib for Universal Windows Platform".

When `TDLib` is built with `TD_ENABLE_DOTNET` option enabled, `C++` documentation is removed from some files. You need to checkout these files to return `C++` documentation back:
```
git checkout td/telegram/Client.h td/telegram/Log.h td/tl/TlObject.h
```

<a name="using-json"></a>
## Using from other programming languages
`TDLib` provides efficient native C++, Java, and .NET interfaces.
But for most use cases we suggest to use the JSON interface, which can be easily used with any programming language that is able to execute C functions.
See [td_json_client](https://core.telegram.org/tdlib/docs/td__json__client_8h.html) and [td_log](https://core.telegram.org/tdlib/docs/td__log_8h.html) documentation for detailed JSON interface description,
the [td_api.tl](https://github.com/tdlib/td/blob/master/td/generate/scheme/td_api.tl) scheme or the automatically generated [HTML documentation](https://core.telegram.org/tdlib/docs/td__api_8h.html) for a list of
all available `TDLib` [methods](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_function.html) and [classes](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_object.html).

`TDLib` JSON interface adheres to semantic versioning and versions with the same major version number are binary and backward compatible, but the underlying `TDLib` API can be different for different minor and even patch versions.
If you need to support different `TDLib` versions, then you can use a value of the `version` option to find exact `TDLib` version to use appropriate API methods.

See [example/python/tdjson_example.py](https://github.com/tdlib/td/tree/master/example/python/tdjson_example.py) for an example of such usage.

<a name="license"></a>
## License
`TDLib` is licensed under the terms of the Boost Software License. See [LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt) for more information.
