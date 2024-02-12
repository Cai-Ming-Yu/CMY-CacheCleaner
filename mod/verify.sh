#!/bin/sh

fileDir="$(dirname "$(readlink -f "${0}")")"
modDir="${fileDir}/source"
verifyFile="${modDir}/files.conf"
echo -n '' >"${verifyFile}"

for file in $(find "${modDir}" -type f -not -path "*/META-INF/*" -not -name "files.conf"); do
    echo "$(md5sum -b "${file}" | cut -d' ' -f1) ${file#${modDir}/}" >>"${verifyFile}"
done
truncate -s $(($(stat -c %s "${verifyFile}") - 1)) "${verifyFile}"
