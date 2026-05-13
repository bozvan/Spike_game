# Простая игра про Ёжика

Учебный платформер на C++17 и SFML 3.0.2.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-brightgreen.svg)
![Stage](https://img.shields.io/badge/stage-Development-red.svg)

## Быстрый запуск

Нужны **CMake**, **MinGW GCC 14.2 UCRT** и **SFML 3.0.2** под тот же компилятор (список ссылок — в разделе «Что нужно установить с нуля» ниже). Первая конфигурация скачает `tinyxml2` из интернета.

1. Откройте PowerShell в папке **`Spike_game`** (рядом с `CMakeLists.txt`).
2. Подставьте свой путь к SFML в `SFML_ROOT` (часто `C:\SFML-3.0.2` после распаковки архива с сайта SFML).
3. Выполните конфигурацию, сборку и запуск из **той же папки**, куда положит exe CMake (рядом скопируются `assets` и DLL):

**Вариант A — MSYS2 UCRT64 (если уже стоит `C:\msys64\ucrt64`) и MinGW Makefiles:**

```powershell
cd путь\к\Spike_game
$SFML_ROOT = "C:\SFML-3.0.2"   # или путь к распакованному SFML-3.0.2-...-mingw-64-bit
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe" `
  -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" `
  -DSFML_ROOT="$SFML_ROOT"
cmake --build build
Set-Location build
.\Spike_game.exe
```

**Вариант B — WinLibs MinGW + Ninja + CMake из PATH** (пути к `cmake`/`ninja`/`gcc` должны быть в `PATH`):

```powershell
cd путь\к\Spike_game
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ `
  -DSFML_ROOT="C:\SFML-3.0.2"
cmake --build build
Set-Location build
.\Spike_game.exe
```

Если `cmake` не находится, установите [CMake](https://cmake.org/download/) или используйте Qt Creator и откройте эту папку как CMake-проект.

Готовый блок с явными путями к Qt CMake/Ninja и WinLibs — в разделе «Сборка из PowerShell (Ручной способ)» ниже.

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
   <https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2.7z>

   Нужна именно UCRT-версия. MSVCRT-версия может собрать проект, но игра потом не запустится из-за несовместимых DLL.

2. **SFML 3.0.2 для GCC 14.2.0 MinGW 64-bit**

   Скачайте архив:
   <https://www.sfml-dev.org/files/SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit.zip>

   Распакуйте так, чтобы были папки:

   ```text
   C:\SFML-3.0.2\bin
   C:\SFML-3.0.2\include
   C:\SFML-3.0.2\lib
   ```

3. **Qt Creator или CMake + Ninja**

   В Qt Creator уже есть удобная сборка через CMake и Ninja. Для ручной сборки выше используются:

   ```text
   C:\Qt\Tools\CMake_64\bin\cmake.exe
   C:\Qt\Tools\Ninja\ninja.exe
   ```

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

| Клавиша | Действие |
| --- | --- |
| `Left` / `Right` | Движение влево и вправо |
| `Space` | Прыжок |
| `Up` / `Down` | Подъем и спуск по лестнице |
| `Left Ctrl` | Перекат |
| `F` | Выстрел |
| `E` | Взаимодействие с дверями и переходами |

Цель игры: собрать ключи и цветные части, открывать двери и добраться до финального перехода.

## Частые проблемы

- `Точка входа в процедуру ... не найдена` в `sfml-system-3.dll`: проект собран MSVCRT-компилятором. Выберите UCRT MinGW GCC 14.2.0 и пересоберите.
- `SFML 3.0.2 was not found`: проверьте путь `C:\SFML-3.0.2`.
- `ninja.exe` не найден: проверьте путь `C:\Qt\Tools\Ninja\ninja.exe`.
- Первая сборка требует интернет, потому что CMake скачивает `tinyxml2`.
