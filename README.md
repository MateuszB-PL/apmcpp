# APM - Package Manager without limitations

## Compiling
 -  install clang (g++), libarchive-devel and nlohmann json (json-devel)
 -  compile with: `g++ main.cpp -o apm -larchive`

## Usage

 - `sudo apm -i` install from APPCONF 
 - `sudo apm -u <package name>` uninstall package
 - `apm -e` generate example appconf
 - `apm -l` list all installed apps
 - `apm -v` displays c++ compilation version and APM version

### APMcpp newest version is prebeta0.4 
![image](https://github.com/MateuszB-PL/apmcpp/assets/99821157/8fbb6984-e42c-4571-89e3-ec5bdf987ad3)
