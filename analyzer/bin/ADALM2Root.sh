#!/bin/sh
	
PROGNAME=ADALM2Root

FILES="*.dat"

CONFFILE="*.cnf"
echo $CONFFILE
HEADER=`basename $CONFFILE .cnf`
echo $HEADER

mkdir -p "root_data"

for file in $FILES; do
	
	echo $file
	file_header=`basename $file .dat`
	
	$PROGNAME $file config.json
	
	outfile="${file_header}.root"
	echo $outfile
	mv $outfile root_data/.

done

# root -l -b -q "/home/shimada/ADALM2000/analyzer/0.0/root_macro/chain.cc(\"${HEADER}\")"


