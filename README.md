# Простая игра про ёжика

Учебный проект на C++17 и SFML 3.0.2.  

## Что нужно установить

1. **Компилятор** – MinGW-w64 (GCC 14.2.0)  
   Скачайте архив с [winlibs](https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2.7z).  
   Распакуйте, например, в `C:\mingw64`.  
   Добавьте `C:\mingw64\bin` в переменную `PATH`.

2. **SFML 3.0.2** (точно под этот компилятор)  
   Скачайте [отсюда](https://www.sfml-dev.org/files/SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit.zip).  
   Распакуйте, например, в `C:\SFML-3.0.2`.

3. **CMake** – скачайте с [официального сайта](https://cmake.org/download/) и установите.  
   Убедитесь, что `cmake` доступен из командной строки.


## Сборка проекта

1. Откройте командную строку (терминал) в **корневой папке проекта** (там, где лежит `CMakeLists.txt`).

2. Выполните последовательно:

   ```bash
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/SFML-3.0.2"
   cmake --build .

## Запуск

После успешной сборки в папке `build` появится файл `Spike_game.exe`.  
**В проекте настроено автоматическое копирование всех необходимых DLL** (SFML и MinGW) прямо в папку с программой – это делается на этапе сборки. Поэтому просто откройте папку с `Spike_game.exe` и запустите его двойным щелчком.

Если по какой-то причине DLL не скопировались, их можно взять вручную:
- из папки `C:\SFML-3.0.2\bin` – `sfml-graphics-3.dll`, `sfml-window-3.dll`, `sfml-system-3.dll`;
- из папки `C:\mingw64\bin` – `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`.
