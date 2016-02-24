#!/bin/bash
if [ "$1" == "" ]; then
	echo "Only building, not flashing. Add a tty port for flashing."
else
	port="$1";
fi


echo "Uploading to the server.."
command="curl http://imegumii.space:12001/upradio.php -w '\n'  --progress-bar  ";
for i in *.c *.h; do
   command="$command -F \"file[]=@$i\"";
done

output=$(eval $command);

echo "$output";

if [ "$output" = "success" ]; then
	rm ipac.hex -f
	echo "Downloading hex file.."
	wget http://imegumii.space:12001/source/ipac.hex -q --show-progress;

	if [ "$port" != "" ]; then
		echo "Starting flash..";
		mono internetradioflash.exe f=ipac.hex p="$port"
	fi
else
	echo "Errors when making; Stopped";
fi




