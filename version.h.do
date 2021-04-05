# Generate the header template
cat <<EOF >$3
#ifndef __VERSION_H
#define __VERSION_H

#define VERSION "%VERSION%"

#endif
EOF

# Check git for version or use UNKNOWN
if ! VER=$(git describe); then
	echo "$0: Git describe unknown version" >&2
	VER="UNKNOWN"
fi

# Replace version in the template
sed -i "s/%VERSION%/$VER/" $3

# Always redo this and provide a stamp
redo-always
redo-stamp <$3
