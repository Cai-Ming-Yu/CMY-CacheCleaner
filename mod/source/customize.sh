#!/bin/sh

SKIPMOUNT='false'

[[ ! "${KSU}" ]] && KSU='false'

rm -rf "${MODPATH}"
mkdir -p "${MODPATH}"

[[ "${API}" -lt '21' ]] && {
    ui_print "! API ${API} is too old"
    ui_print "- Cache Cleaner is not supported on this Android version"
    abort
}

[[ "${BOOTMODE}" && "$(pm path Cai_Ming_Yu.CacheCleaner)" == '' ]] && {
    ui_print "! Cache Cleaner xposed version is not installed."
    ui_print "- Please install it for a better experience."
}

verifyList="$(unzip -p "${ZIPFILE}" "files.conf")"

verifyFile() {
    local trueSum="$(echo -n "${verifyList}" | grep " ${1}" | awk '{print $1}')"
    unzip -oj "${ZIPFILE}" "${1}" -d "${MODPATH}/${2}" 1>&2
    [[ "${trueSum}" != "$(md5sum -b "${MODPATH}/${2}/$(basename "${1}")" | cut -d' ' -f1)" ]] && {
        ui_print "! Failed to extract the file: ${1}"
        ui_print "- Please go to https://github.com/Cai-Ming-Yu/CMY-CacheCleaner to re-download the module!"
        abort
    } || {
        ui_print "- Extracted and verified the file: ${1}"
    }
}

verifyFile 'module.prop'
verifyFile 'service.sh'
verifyFile "bin/${ARCH}/CacheCleaner"
verifyFile 'config.yaml'

[[ ! -d "/data/adb/C-M-Y/CacheCleaner" ]] && mkdir -p "/data/adb/C-M-Y/CacheCleaner"
[[ ! -f "/data/adb/C-M-Y/CacheCleaner/config.yaml" ]] && {
    cp -rf "${MODPATH}/config.yaml" "/data/adb/C-M-Y/CacheCleaner/config.yaml"
}

sh "${MODPATH}/config.yaml" &

ui_print "- Cache Cleaner is running, configuration file: /data/adb/C-M-Y/CacheCleaner/config.yaml"
ui_print "- Installation is complete, thank you for using"
