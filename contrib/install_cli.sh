 #!/usr/bin/env bash

 # Execute this file to install the gemma cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Gemma-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Gemma-Qt.app/Contents/MacOS/gemmad /usr/local/bin/gemmad
 sudo ln -s ${LOCATION}/Gemma-Qt.app/Contents/MacOS/gemma-cli /usr/local/bin/gemma-cli
