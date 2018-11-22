<?php
/**
 * Harrys Shoutbox - let harry hamster talk!
 * the last 3 messages are shown to the public
 * 
 * Be sure, that the message file (./contents/harry/messages.txt)
 * is writeable by the webserver, on you need to linux:
 * 		sudo chown www-data messages.txt
 * 		sudo chmod 774 messages.txt
 * 
 * New messages can be postet via harry-talk.php in main dir.
 */

/**
 * show qr-code public on display?
 *  
 * you may add your qr-code (./contents/harry/qr.png) which points to 
 * harry-talk.php on your server, so users can upload messages
 * use https://www.the-qrcode-generator.com/ and download the png
 * scaling from 200x200 to 67x67 is done by this script
 */
$showQr = true;

// how long the display sleeps in seconds - to show user the next update time
$deepSleepSeconds = 300;

// for harry you need a big display
if($displayHeight < 300 || $displayWidth < 400 ) {
    echo "Bad news for you, you need a bigger display. :(";
    exit;
}

// read message file
$messages = file("./contents/harry/messages.txt");

// place harry
$harry = imagecreatefrompng("./contents/harry/harry-scared.png"); # choose your harry (see images in folder harry)
imagecopy($im, $harry, 0, $displayHeight-imagesy($harry)-16, 0, 0, imagesx($harry), imagesy($harry));

// place bubble, qr code and status
imagerectangle($im, 80, 1, $displayWidth-2, $displayHeight-17, $black);
imagerectangle($im, 81, 0, $displayWidth-3, $displayHeight-16, $black);

imageline($im, 80, $displayHeight-90, 80, $displayHeight-100, $white);
imageline($im, 81, $displayHeight-90, 81, $displayHeight-100, $white);
imageline($im, 80, $displayHeight-90, 70, $displayHeight-100, $black);
imageline($im, 79, $displayHeight-90, 69, $displayHeight-100, $black);
imageline($im, 80, $displayHeight-100, 70, $displayHeight-100, $black);
imageline($im, 79, $displayHeight-101, 69, $displayHeight-101, $black);

if ($showQr == true) {
	imagettftext($im, 9, 0, 10, 15, -$black, $TERMINUS_FONT['regular'], "Let Harry\n  speak:");
	$qr = imagecreatefrompng("./contents/harry/qr.png");
	$qr = imagescale($qr, 67, 67);
	imagecopy($im, $qr, 5, 35, 0, 0, imagesx($qr), imagesy($qr));
}

imagefilledrectangle($im, 0, $displayHeight-13, $displayWidth, $displayHeight, $black);
imagettftext($im, 9, 0, 1, $displayHeight-3, -$white, $TERMINUS_FONT['regular'], "updated:".date("d.m.y H:i",time())." | next update:".date("H:i",time()+$deepSleepSeconds)." | Harry © by Tim 2017");

// write messages
$i=0;
foreach ($messages as $message){
	$msg = explode(";", $message);
	imagettftext($im, 18, 0, 85, 20+($i*98), -$black, $TERMINUS_FONT['bold'], fittext($msg[0], floor(($displayWidth-95)/12)));
	imagettftext($im, 9, 0, $displayWidth-180, 83+($i*98), -$black, $TERMINUS_FONT['regular'], $msg[1]);
	$i++;
}

/**
 * functions
 */

// fit text into lines
function fittext($text, $linelenght) {
	$words = explode(' ', $text);
	$output = "";
	$line = "";
	foreach ( $words as $word ) {
		if (strlen($line) == 0) {
            $line = $word;
		}
		else {
			if (strlen( $line." ".$word ) > $linelenght) {
				$output = $output.$line."\n";
				$line = $word;
			}
			else {
				$line = $line." ".$word;
			}
		}
	}
	$output = $output.$line;
	return $output;
}

?>