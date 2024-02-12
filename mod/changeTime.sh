#!/bin/sh

fileDir="$(dirname "$(readlink -f "${0}")")"
modDir="${fileDir}/build"

for path in $(find "${modDir}"); do
    touch -c -t 000001010000 "${path}" 2>/dev/null || touch -c -t 197101010000 "${path}" 2>/dev/null || touch -c -t 200001010000 "${path}" 2>/dev/null
done
