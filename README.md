# Descargador de Archivos de la Red Todus (S3)

`DARTS3` es una aplicación para consola que automatiza el proceso de descarga de archivos desde los servidores de la red Todus (S3).

## Tabla de contenidos
- [Características](#caracteristicas)
- [Dependencias](#dependencias)
- [Construcción](#construccion)
- [Instalando dependencias](#instalando-dependencias)
- [Licencia](#licencia)

<a name="caracteristicas"></a>
## Características

`DARTS3` posee las siguientes características::

* **Multiplataforma**: `DARTS3` puede ser utilizado en cualquier sistema operativo que en donde la librería Qt pueda funcionar: UNIX (FreeBSD, NetBSD, OpenBSD), Linux, macOS y Windows.
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

<a name="instalando-dependencias"></a>
### Instalando dependencias

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

* Td::TdJson, Td::TdJsonStatic — dynamic and static version of a JSON interface. This has a simple C interface, so it can be easily used with any programming language that is able to execute C functions.
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

<a name="licencia"></a>
## Licencia
`DARTS3` está licenciado bajo la licencia BSD. Vea [LICENCIABSD3.txt](https://opensource.org/licenses/BSD-3-Clause) para más información.
