#!/bin/sh
#
cat > /tmp/jpeg$$
( $(XV)xv /tmp/jpeg$$ ; rm /tmp/jpeg$$ ) >/dev/null 2>&1 &
echo "Formatting - please wait..."
