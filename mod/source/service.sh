#!/bin/sh

modDir="$(dirname "$(readlink -f "${0}")")"

[[ ! -d "/data/adb/C-M-Y/CacheCleaner" ]] && mkdir -p "/data/adb/C-M-Y/CacheCleaner"
[[ ! -f "/data/adb/C-M-Y/CacheCleaner/config.yaml" ]] && {
    cp -rf "${modDir}/config.yaml" "/data/adb/C-M-Y/CacheCleaner/config.yaml"
}

chmod 0777 "${modDir}/CacheCleaner"
nohup "${modDir}/CacheCleaner" "/data/adb/C-M-Y/CacheCleaner/config.yaml" >/dev/null 2>&1 &

exit 0
