modDir="$('dirname' "$('readlink' '-f' "${0}")")"

[ ! -d "/data/adb/C-M-Y/CacheCleaner" ] && 'mkdir' '-p' "/data/adb/C-M-Y/CacheCleaner"
[ ! -f "/data/adb/C-M-Y/CacheCleaner/config.yaml" ] && {
    'cp' '-rf' "${modDir}/config.yaml" "/data/adb/C-M-Y/CacheCleaner/config.yaml"
}

while [ $('getprop' 'sys.boot_completed') != '1' ]; do
    'sleep' '1'
done
'sleep' '10'
[ -f '/sdcard/.CacheCleaner' ] && 'rm' '-rf' '/sdcard/.CacheCleaner'
while [ ! -f '/sdcard/.CacheCleaner' ]; do
    'true' >'/sdcard/.CacheCleaner'
    'sleep' '1'
done
'rm' '-rf' '/sdcard/.CacheCleaner'

'sleep' '30'

[ "$('pidof' 'CacheCleaner')" != '' ] && 'killall' '-15' 'CacheCleaner'

'chmod' 'a+x' "${modDir}/CacheCleaner"
'nohup' "${modDir}/CacheCleaner" "/data/adb/C-M-Y/CacheCleaner/config.yaml" >'/dev/null' 2>&'1' &

'exit' '0'
