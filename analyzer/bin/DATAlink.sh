#!/bin/sh
	
PROGNAME=$(basename $0)
TARGETDIR=/home/shimada/ADALM2000/data

DIR=$PWD
echo $DIR
DATE=`pwd | awk -F "/" '{ print $NF}'`
echo $DATE
number=$1
PER_NUM=$(printf "per%04d\n" "${number}")
echo $PER_NUM

DATADIR=$TARGETDIR/$DATE/$PER_NUM
echo $DATADIR

FILES=$DATADIR/*

mkdir -p $PWD/$PER_NUM

for file in $FILES; do
	#echo $file
	ln -s $file $DIR/$PER_NUM
done


