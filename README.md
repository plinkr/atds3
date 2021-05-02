# Administrador de Transferencias para toDus (S3)

`ATDS3` es una aplicación para escritorio que automatiza el proceso de descarga y subida de archivos desde/hacia los servidores de la red toDus (S3).

## Tabla de contenidos
- [Características](#caracteristicas)
- [Dependencias](#dependencias)
- [Instalando dependencias](#instalando-dependencias)
- [Construcción](#construccion)
- [Licencia](#licencia)

<a name="caracteristicas"></a>
## Características

`ATDS3` posee las siguientes características::

* **Multiplataforma**: `ATDS3` puede ser utilizado en cualquier sistema operativo que en donde la librería Qt pueda funcionar: UNIX (FreeBSD, NetBSD, OpenBSD), Linux, macOS y Windows.
* **Fácil de usar**: `ATDS3` es fácil de usar gracias a la intuitiva interfaz de usuario que posee.
* **Configurable**: `ATDS3` ofrece a diversas opciones configurables que definen el comportamiento de diversas secciones.
* **Completamente asincrónico**: `ATDS3` es totalmente asincrónico, por lo que podrá realizar varias operaciones de descargas y subidas al mismo tiempo.
* **Ajustado al protocolo de red de toDus**: `ATDS3` Está ajustado lo mayormente posible al protocolo de red que utiliza toDus, incluyendo inicios de sesión partiendo del número telefónico.

<a name="dependencias"></a>
## Dependencias
`ATDS3` depende de:

* Compilador C++ compatible con el estándar C++20 (Clang 3.4+, GCC 4.9+, MSVC 19.0+ (Visual Studio 2015+), Intel C++ Compiler 17+)
* qmake
* Qt
  * Qt 5 Core
  * Qt 5 GUI
  * Qt 5 SQL (SQLite3)
  * Qt 5 Network
  * Qt 5 Widgets
* Google Protocol Buffers

<a name="instalando-dependencias"></a>
### Instalando dependencias

<a name="freebsd"></a>
#### FreeBSD
* Instalar las herramientas y librerías:
```
pkg install qt5-qmake qt5-core qt5-gui qt5-sql qt5-sqldrivers-sqlite3 qt5-network qt5-widgets protobuf
```

<a name="macos"></a>
#### macOS
* Instalar las herramientas de línea de comandos de Xcode vía `xcode-select --install`.
* Instalar las dependencias utilizando [Homebrew](https://brew.sh):
```
brew install qt5-qmake qt5-core qt5-gui qt5-sql qt5-sqldrivers-sqlite3 qt5-network qt5-widgets protobuf
```

<a name="linux"></a>
#### Linux
* Install all [dependencies](#dependencies) using your package manager.

<a name="windows"></a>
#### Windows
* Download and install Microsoft Visual Studio 2015 or later.
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

<a name="construccion"></a>
## Construcción

La forma mas simple de construir `ATDS3`:

```
mkdir _construccion
cd _construccion
qmake ../atds3.pro CONFIG+=release
make -j2
```

<a name="licencia"></a>
## Licencia
`ATDS3` está licenciado bajo la licencia BSD versión 3. Vea [LICENCE.txt](blob/main/LICENSE.txt) para más información.
