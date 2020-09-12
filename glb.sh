#!/bin/sh

TAGS="$1"

next_pid() {
	# $1 - Current PID
	local PID=$1
	if [ "$PID" = "" ]; then
		local PID=0
	fi
	#echo $(curl -s -L "https://gelbooru.com/index.php?page=post&s=list&tags=$TAGS&pid=$PID" | grep -Po "pid=[0-9]+\" alt=\"next\"" | grep -o "[0-9]*")
	echo $(( $PID + 42 )) # Hardcoded???
}

page_ids() {
	# $1 - PID
	local PID=$1
	if [ "$PID" = "" ]; then
		local PID=0
	fi
	echo $(curl -s -L "https://gelbooru.com/index.php?page=post&s=list&tags=$TAGS&pid=$PID" | grep -Po "\"//gelbooru.*?id=[0-9]+.*?\"" | grep -Po "id=[0-9]+" | grep -o "[0-9]*")
}

get_image() {
	# $1 - ID
	[ "$1" = "" ] && return
	echo $(curl -sL "https://gelbooru.com/index.php?page=post&s=view&id=$1" | grep -Po "href=\"http.*?Original image" | grep -Po "\"http.*?\"" | grep -o "[^\"]*")
}

download_ids() {
	# $1 - IDs
	for id in $1; do
		local URL=$(get_image $id)
		printf "\n[$id]: $URL\n"
		curl -LO $URL
	done
}

PID=0
IDS=$(page_ids $PID)
while [ "$IDS" != "" ]; do
	printf "\n=====\n"
	printf "= PAGE: $PID\n"
	printf "=====\n\n"
	download_ids "$IDS"

	PID=$(next_pid $PID)
	IDS=$(page_ids $PID)
done
