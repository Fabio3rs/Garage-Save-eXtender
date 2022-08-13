#!/bin/bash

set -e

CONTENT=$(echo "#!/bin/bash

wine $1.exe \$@
exit \$?
")

echo "$CONTENT" > "$1"
chmod +x "$1"

echo "placeholder" > "$1.placeholder"

ls -lahtr
