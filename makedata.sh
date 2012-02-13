#!/bin/sh

mkdir -p data/tiles data/objects
rm -f images/tiles/*.ut images/objects/*.ut data/tiles/*.ut data/objects/*.ut

# convert tiles
printf "**\n** Converting tiles\n**\n";
for FILE in `ls images/tiles/`; do

	printf "Converting %20s ...  " $FILE
	src/tools/bmp2ut/bmp2ut --halfbits -i images/tiles/$FILE

done;

printf "**\n** Converting objects\n**\n";
for FILE in `ls images/objects/`; do

	printf "Converting %20s ...  " $FILE
	src/tools/bmp2ut/bmp2ut --halfbits -i images/objects/$FILE

done;

mv images/tiles/*.ut data/tiles
mv images/objects/*.ut data/objects

