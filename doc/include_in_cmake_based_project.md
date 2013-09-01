Integrate libipstack via cmake into your project
================================================
[back](../readme.md)

Use add_subdirectory() to include libipstack in your own CMake based application
or operating system. You need to provide the kconfig executable either as file in
BUILD_DIR/qconfig or via the variable KCONFIG_EXECUTABLE.

Already using kconfig
---------------------
If you also use kconfig to configure your os or app, you may want to include buildsystem/ipstack.fm
into the file you handle over to kconfig. You may use the build tool in buildsystem/merge_kconfig_input.

If you want a preselection (by adding relevant keys from a .config file before executing kconfig) for the ipstack,
you need to add buildsystem/kconfig_output_default to your .config file. You may use the build tool in buildsystem/merge_kconfig_output to merge your defaults with ipstacks defaults.
Place the .config output file from kconfig in KCONFIG_RESULT_FILE_DIR (usually the build dir) to avoid
calling kconfig a second time.

Additional (aspect) files for the ipstack
-----------------------------------------
If you want additional files to be considered by the aspectc++ compiler for the ipstack, for example
if you want to add aspect files to manipulate the stack, set the variable IPSTACK_ADDITIONAL_SRC.