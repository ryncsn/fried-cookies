#!/bin/sh
MP3="mp3"
MP3C="MP3"
OGG="ogg"
OGGC="OGG"
jpg=".jpg"
jpgC=".JPG"
jpeg=".jpeg"
jpegC=".JPEG"
png=".png"
pngC=".PNGC"
for file in /media/data/Users/Song/Games/osu/Songs/*  
do
if [ -d "$file" ]
then
	osufile=$file/$(ls -S "$file" | grep -i '.osu$' | head -n 1)
	if [ -f $file/$osufile ]
		then
		cd "$file/"
		picfile=$file/$(ls -S| grep -i -e '.jpg$' -e '.png$' | head -n 1)
#		picfile=$(grep -i -e '.jpg$' -e '.png$' -e '.JPG$' -e '.PNG$' -e '.jpeg$' -e '.JPEG$' "$osufile")
		TITLE=$(grep "Title:" "$osufile")
		TITLE=${TITLE#*:}
		TITLE=${TITLE%?}
		SONG=$(grep "AudioFilename:" "$osufile")
		SONG=${SONG#*: }
		SONG=${SONG%?}
		if [[ $SONG =~ ^"AudioFilename:" ]]
		then SONG=${SONG#*:}
		fi
		ARTIST=$(grep 'Artist:' "$osufile")
		ARTIST=${ARTIST#*:}
		ARTIST=${ARTIST%?}
		SOURCE=$(grep 'Source:' "$osufile")
		SOURCE=${SOURCE#*:}
		SOURCE=${SOURCE%?}
		TAGS=$(grep 'Tags:' "$osufile")
		TAGS=${TAGS#*:}
		TAGS=${TAGS%?}
#		if [[ $picfile =~ $jpg$ ]] || [[ $picfile =~ $jpgC$ ]]
#			then 	EXTP=".jpg"
#		elif [[ $picfile =~ $jpeg$ ]] || [[ $picfile =~ $jpegC$ ]]
#			then 	EXTP="jpeg"
#		elif [[ $picfile =~ $png$ ]] || [[ $picfile =~ $pngC$ ]]
#			then 	EXTP="png"
#		else echo "Image file error on $osufile" >> ~/log
#		fi
#
		if [[ $SONG =~ $MP3$ ]] || [[ $SONG =~ $MP3C$ ]]
			then 	EXTS="mp3"
		elif [[ $SONG =~ $OGG$ ]] || [[ $SONG =~ $OGGC$ ]]
			then 	EXTS="ogg"
		else echo "Song file error on $osufile" >> ~/log
		fi

#	if [ ! -e "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTP" ]
#		then cp "$picfile" "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTP"
#	fi

	if [ ! -e "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS" ] 
		then cp "$SONG" "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS"
	fi
	if [ ! -e "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS" ] 
		then cp "${SONG%.mp3}.MP3" "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS"
		elif [ ! -e "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS" ] 
			then echo "file copy fail on $osufile" >> ~/log
	fi
	if [ "$EXTS" = "$MP3" ]
	then eyeD3 -Q -a "$ARTIST" -A "$SOURCE" -t "$TITLE" -c "${TAGS} ${BID}" --add-image "$picfile":FRONT_COVER "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS"
	fi
	if [ "$EXTS" = "$OGG" ]
	then 
	coverart=$(base64 $picfile)
		if [[ $picfile =~ $jpg$ ]]||[[ $picfile =~ $jpgC$ ]] ||  [[ $picfile =~ $jpeg$ ]] || [[ $picfile =~ $jpegC$ ]]
		then vorbiscomment -a "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS" -t "ARTIST=$ARTIST" -t "TITLE=$TITLE" -t "ALBUM=$SOURCE" -t "COVERART=$coverart" -t "COVERARTMIME=image/jpeg"
		elif [[ $picfile =~ $png$ ]] || [[ $picfile =~ $pngC$ ]]
		then vorbiscomment -a "/media/data/Programs/run/song/$ARTIST - $TITLE.$EXTS" -t "ARTIST=$ARTIST" -t "TITLE=$TITLE" -t "ALBUM=$SOURCE" -t "COVERART=$coverart" -t "COVERARTMIME=image/png"
		else "Image error on $osufile"
		fi
	fi
fi
done
