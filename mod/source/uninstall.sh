[ -d "/data/adb/C-M-Y/CacheCleaner" ] && 'rm' '-rf' "/data/adb/C-M-Y/CacheCleaner"
{ [ -d "/data/adb/C-M-Y" ] && [ "$('ls' '-A' "/data/adb/C-M-Y")" == '' ] ; } && 'rm' '-rf' "/data/adb/C-M-Y"

'exit' '0'