# Compiling
 -  install gcc, libarchive-devel and nlohmann json (json-devel)
 -  compile with: `gcc main.cpp -o apm -larchive`

# Usage

 - `sudo apm -i` install from APPCONF 
 - `sudo apm -u <package name>` uninstall package
 - `apm -e` generate example appconf
 - `apm -l` list all installed apps
