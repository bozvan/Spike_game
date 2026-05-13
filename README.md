# Простая игра про Ёжика

Учебный платформер на C++17 и SFML 3.0.2.

C++
CMake
Stage

## Быстрый запуск (Windows)

Нужны **CMake**, **MinGW GCC 14.2 UCRT** и **SFML 3.0.2** под тот же компилятор (список ссылок — в разделе «Что нужно установить с нуля» ниже). Первая конфигурация скачает `tinyxml2` из интернета.

**Сборка на Ubuntu / Linux** — в разделе «Ubuntu и другие Linux» ниже на этой странице.

1. Откройте PowerShell в папке `**Spike_game`** (рядом с `CMakeLists.txt`).
2. Подставьте свой путь к SFML в `SFML_ROOT` (часто `C:\SFML-3.0.2` после распаковки архива с сайта SFML).
3. Укажите `**cmake**`: если команда `cmake` не находится (часто после установки с сайта), задайте полный путь к exe, например `C:\Program Files\CMake\bin\cmake.exe`, или добавьте эту папку в переменную среды `PATH` и перезапустите терминал.
4. Выполните конфигурацию, сборку и запуск из **той же папки**, куда положит exe CMake (рядом скопируются `assets` и DLL). В начале каждого блока задайте `$CMAKE`: полный путь к `cmake.exe` (часто `C:\Program Files\CMake\bin\cmake.exe`) или строку `cmake`, если CMake уже в `PATH`.

**Вариант A — MSYS2 UCRT64 (если уже стоит `C:\msys64\ucrt64`) и MinGW Makefiles:**

```powershell
cd путь\к\Spike_game
$CMAKE = "C:\Program Files\CMake\bin\cmake.exe"   # при необходимости
$SFML_ROOT = "C:\SFML-3.0.2"   # или путь к распакованному SFML-3.0.2-...-mingw-64-bit
& $CMAKE -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe" `
  -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" `
  -DSFML_ROOT="$SFML_ROOT"
& $CMAKE --build build
Set-Location build
.\Spike_game.exe
```

**Вариант B — WinLibs MinGW без Ninja (MinGW Makefiles)** — не нужен `ninja.exe`, только `gcc`/`g++` из WinLibs. Подставьте папку `bin` вашего MinGW (где лежат `gcc.exe` и `mingw32-make.exe`):

```powershell
cd путь\к\Spike_game
$CMAKE = "C:\Program Files\CMake\bin\cmake.exe"   # при необходимости
$MINGW  = "C:/tools/mingw64"   # пример: корень WinLibs, внутри bin\gcc.exe
$SFML_ROOT = "C:\SFML-3.0.2"
& $CMAKE -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="$MINGW/bin/gcc.exe" `
  -DCMAKE_CXX_COMPILER="$MINGW/bin/g++.exe" `
  -DSFML_ROOT="$SFML_ROOT"
& $CMAKE --build build
Set-Location build
.\Spike_game.exe
```

**Вариант C — Ninja** (нужен установленный **Ninja** и путь к нему в `PATH`, либо укажите `-DCMAKE_MAKE_PROGRAM` на `ninja.exe`, например `C:/Qt/Tools/Ninja/ninja.exe`). Если до этого уже запускали конфигурацию с ошибкой, удалите папку `**build`** и начните снова.

С Ninja в `PATH`:

```powershell
cd путь\к\Spike_game
$CMAKE = "C:\Program Files\CMake\bin\cmake.exe"
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
& $CMAKE -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ `
  -DSFML_ROOT="C:\SFML-3.0.2"
& $CMAKE --build build
Set-Location build
.\Spike_game.exe
```

Если Ninja не в `PATH`, добавьте в команду конфигурации, перед `-DSFML_ROOT`:

`-DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/Ninja/ninja.exe"`

Если CMake ещё не установлен, скачайте установщик с [cmake.org/download](https://cmake.org/download/) (галочка *Add CMake to the system PATH* решает проблему для новых терминалов) или откройте проект в **Qt Creator**.

Готовый блок с явными путями к Qt CMake/Ninja и WinLibs — в разделе «Сборка из PowerShell (Ручной способ)» ниже.

## Ubuntu и другие Linux

Ubuntu (и в целом на Linux) проект можно **собрать и запустить**. В `CMakeLists.txt` для не-Windows используется `**find_package(SFML 3 ...)*`* и копируется только каталог `**assets**` (без DLL).

Нужны **компилятор с C++17** и установленный **SFML 3** так, чтобы CMake находил пакет (часто через `CMAKE_PREFIX_PATH` к каталогу установки SFML).

Типичные зависимости для сборки графики и SFML:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
  libfreetype6-dev libgl1-mesa-dev libx11-dev libxrandr-dev \
  libxcursor-dev libxi-dev libopenal-dev libflac-dev libvorbis-dev libogg-dev libudev-dev
```

**Важно:** в репозиториях Ubuntu часто до сих пор **SFML 2.x**. Этому проекту нужна **SFML 3**. Если `apt` не даёт версию 3, соберите SFML с [официального репозитория](https://github.com/SFML/SFML), установите в префикс (например `/usr/local`) и укажите путь при конфигурации:

```bash
export CMAKE_PREFIX_PATH="/usr/local"   # каталог, куда `cmake --install` поставил SFML 3
cd Spike_game
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/Spike_game
```

Запускайте `**./build/Spike_game**` из каталога `**Spike_game**`, либо перейдите в `build` и запустите `./Spike_game` — рядом с бинарником уже должна лежать скопированная папка `assets`.

## Сборка из PowerShell (Ручной способ)

Для ручной сборки отредактируйте переменные в начале блока кода и вставьте его в терминал:

```powershell
# --- НАСТРОЙТЕ ПУТИ ПОД СЕБЯ ---
$PROJECT_DIR = "ПУТЬ_К_ПАПКЕ_ПРОЕКТА"
$MINGW_PATH  = "ПУТЬ_К_MINGW64"      # Например: C:/tools/mingw64
$SFML_PATH   = "ПУТЬ_К_SFML"         # Например: C:/SFML-3.0.2
$CMAKE_PATH  = "ПУТЬ_К_CMAKE_EXE"    # Например: C:/Qt/Tools/CMake_64/bin/cmake.exe
$NINJA_PATH  = "ПУТЬ_К_NINJA_EXE"    # Например: C:/Qt/Tools/Ninja/ninja.exe
# -------------------------------

cd "$PROJECT_DIR"
$BUILD = "build/manual-Debug"

# Очистка предыдущих сборок
if (Test-Path $BUILD) { Remove-Item $BUILD -Recurse -Force }
if (Test-Path CMakeCache.txt) { Remove-Item CMakeCache.txt -Force }

# Конфигурация и сборка
& "$CMAKE_PATH" -S . -B $BUILD -G "Ninja" `
    -DCMAKE_MAKE_PROGRAM="$NINJA_PATH" `
    -DCMAKE_BUILD_TYPE=Debug `
    -DCMAKE_C_COMPILER="$MINGW_PATH/bin/gcc.exe" `
    -DCMAKE_CXX_COMPILER="$MINGW_PATH/bin/g++.exe" `
    -DSFML_ROOT="$SFML_PATH"

& "$CMAKE_PATH" --build $BUILD --target all

# Запуск
& "$BUILD/Spike_game.exe"
```

## Что нужно установить с нуля

1. **MinGW-w64 UCRT GCC 14.2.0**
  Скачайте архив:
   [https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2.7z](https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2.7z)
   Нужна именно UCRT-версия. MSVCRT-версия может собрать проект, но игра потом не запустится из-за несовместимых DLL.
2. **SFML 3.0.2 для GCC 14.2.0 MinGW 64-bit**
  Скачайте архив:
   [https://www.sfml-dev.org/files/SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit.zip](https://www.sfml-dev.org/files/SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit.zip)
   Распакуйте так, чтобы были папки:
3. **Qt Creator или CMake + Ninja**
  В Qt Creator уже есть удобная сборка через CMake и Ninja. Для ручной сборки выше используются:

## Проверка правильного MinGW

В PowerShell:

```powershell
& "ПУТЬ_К_MINGW\bin\g++.exe" --version
```

В первой строке должно быть:

```text
x86_64-ucrt-posix-seh
```

Если там `x86_64-msvcrt-posix-seh`, это неправильный MinGW для скачанного пакета SFML.

## Что появляется после сборки

Рядом с `Spike_game.exe` CMake автоматически копирует:

```text
assets
sfml-system-3.dll
sfml-window-3.dll
sfml-graphics-3.dll
libstdc++-6.dll
libgcc_s_seh-1.dll
libwinpthread-1.dll
```

Поэтому запускать нужно exe прямо из папки сборки.

## Управление


| Клавиша          | Действие                              |
| ---------------- | ------------------------------------- |
| `Left` / `Right` | Движение влево и вправо               |
| `Space`          | Прыжок                                |
| `Up` / `Down`    | Подъем и спуск по лестнице            |
| `Left Ctrl`      | Перекат                               |
| `F`              | Выстрел                               |
| `E`              | Взаимодействие с дверями и переходами |


Цель игры: собрать ключи и цветные части, открывать двери и добраться до финального перехода.

## Частые проблемы

- `Имя "cmake" не распознано` / `cmake` не найден: CMake не в `PATH`. Задайте `$CMAKE = "C:\Program Files\CMake\bin\cmake.exe"` и вызывайте `& $CMAKE ...`, либо добавьте эту папку в `PATH` и откройте новый терминал. См. раздел «Быстрый запуск».
- `Точка входа в процедуру ... не найдена` в `sfml-system-3.dll`: проект собран MSVCRT-компилятором. Выберите UCRT MinGW GCC 14.2.0 и пересоберите.
- `SFML 3.0.2 was not found`: проверьте путь `C:\SFML-3.0.2`.
- `Could not find a package configuration file` / `SFML 3` не найден (Linux): установите **SFML 3** и задайте `CMAKE_PREFIX_PATH` на префикс установки, см. раздел «Ubuntu и другие Linux».
- Первая сборка требует интернет, потому что CMake скачивает `tinyxml2`.

