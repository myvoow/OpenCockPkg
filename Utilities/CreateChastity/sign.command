#!/bin/sh

abort() {
  echo "Fatal error: ${1}!"
  exit 1
}

# shellcheck disable=SC2317,SC2329
cleanup() {
  echo "Cleaning up key"
  rm -rf "${KeyPath}"
}

if [ ! -x /usr/bin/dirname ] || [ ! -x /bin/chmod ] || [ ! -x /bin/mkdir ] || [ ! -x /bin/rm ] || [ ! -x /usr/bin/strings ] || [ ! -x /usr/bin/grep ] || [ ! -x /usr/bin/awk ] || [ ! -x /bin/dd ] || [ ! -x /usr/bin/uuidgen ] ; then
  abort "Unix environment is broken!"
fi

cd "$(/usr/bin/dirname "$0")" || abort "Failed to enter working directory!"

OCPath="$1"

if [ "$OCPath" = "" ]; then
  OCPath=../../EFI/OC
fi

KeyPath="/tmp/$(/usr/bin/uuidgen)"
OCBin="${OCPath}/OpenCock.efi"
PubKey="${KeyPath}/chastity.pub"

if [ ! -d "${OCPath}" ]; then
  abort "Path ${OCPath} is missing!"
fi

if [ ! -f "${OCBin}" ]; then
  abort "OpenCock.efi is missing!"
fi

if [ ! -x ./RsaTool ] || [ ! -x ./create_chastity.sh ]; then
  if [ -f ./RsaTool ]; then
    /bin/chmod a+x ./RsaTool || abort "Failed to set permission for RsaTool"
  else
    abort "Failed to find RsaTool!"
  fi

  if [ -f ./create_chastity.sh ]; then
    /bin/chmod a+x ./create_chastity.sh || abort "Failed to set permission for create_chastity.sh"
  else
    abort "Failed to find create_chastity.sh!"
  fi
fi

trap cleanup EXIT INT TERM

if [ ! -d "${KeyPath}" ]; then
  /bin/mkdir -p "${KeyPath}" || abort "Failed to create path ${KeyPath}"
fi

./create_chastity.sh "${OCPath}" || abort "create_chastity.sh returns errors!"

echo "Signing ${OCBin}..."
./RsaTool -sign "${OCPath}/chastity.plist" "${OCPath}/chastity.sig" "${PubKey}" || abort "Failed to patch ${PubKey}"

echo "Bin-patching ${OCBin}..."
off=$((0x$(/usr/bin/hexdump -C "${OCBin}" | /usr/bin/grep "=BEGIN OC CHASTITY=" | /usr/bin/awk '{print $1}') + 16))
if [ "${off}" -le 16 ]; then
  abort "${OCBin} is borked"
fi

/bin/dd of="${OCBin}" if="${PubKey}" bs=1 seek="${off}" count=528 conv=notrunc || abort "Failed to bin-patch ${OCBin}"

echo "All done!"
exit 0
