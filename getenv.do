[ -z "$CC" ] && export CC=gcc

export -p | grep "export CC=" >.env
