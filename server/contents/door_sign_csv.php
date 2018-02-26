<?php
    checkFreeType();

// To use google docs (spreadsheet) as a source for the doorsigns, share the
// document publicly and add the google-doc-id in the following line.

// Format of the CSV file (use "users.csv" as a reference)
// 1st row: Ignore the line if text is in the first row. Used to deactivate the
//          the current line
// 2nd row: Room number
// 3rd row: Name
// 4th row: Mail address
// 5th row: Phone number
// 6th row: Status (i.e. available, on vacation etc.)

//    $spreadsheet_url="https://docs.google.com/spreadsheets/d/<gdoc-id>/gviz/tq?tqx=out:csv";
    $spreadsheet_url="users.csv";

// The logo shown in the upper right corner
    $logo_src="contents/static_image/ct_bwr.png";
    $logo_width=110;

    $size =getimagesize($logo_src);

// Allow different data formats for the logo
    if($size['mime']=='image/png'){ $logo = imagecreatefrompng($logo_src); }
    if($size['mime']=='image/jpg'){ $logo = imagecreatefromjpeg($logo_src); }
    if($size['mime']=='image/jpeg'){ $logo = imagecreatefromjpeg($logo_src); }
    if($size['mime']=='image/pjpeg'){ $logo = imagecreatefromjpeg($logo_src); }


// Scale down the logo
    $logo_height=round($logo_width/$size[0]*$size[1],0);

    $logo = imagescale($logo, $logo_width, $logo_height);


// We need to provide the "room" parameter to use one csv file for several rooms
    if (strlen($_GET['room'])){
            $room = $_GET['room'];
    }else{
            $room = "A 111";
    }

// Read the CSV file with the rooms
    if(!ini_set('default_socket_timeout', 15)) echo "<!-- unable to change socket timeout -->";

    if (($handle = fopen($spreadsheet_url, "r")) !== FALSE) {
        while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
            $spreadsheet_data[] = $data;
        }
        fclose($handle);
    }
    else {
        die("Problem reading csv");
    }

    $persons = array();

    foreach ($spreadsheet_data as $line){
            if (strtolower($line[0]) !== ""){
                    // First row has text? -> Ignore the line
                    continue;
            }

            if (strtolower($line[1]) !== strtolower($room)){
                    // Different room number? -> Ignore the line
                    continue;
            }
            $persons[] = array($line[2], $line[3], $line[4], $line[5]);
    }

    $fontSize = $scale;

    // Room number
    $cursorY += $fontSize*1.4;
    imagettftext($im, $fontSize, 0, 10, $cursorY, $red, $DEFAULT_FONT['bold'], $room);

    // Add logo
    imagecopymerge($im, $logo, $displayWidth - $logo_width - 20, 5, 0, 0 , $logo_width, $logo_height, 100);
    imagedestroy($logo);

    $cursorY += 5;
    imageline ($im , 10 , $cursorY , $displayWidth - 20 , $cursorY , $black );

    // Print all persons in the room
    $fontSize = 0.5*$scale;
    foreach($persons as $person){
        $cursorY = $cursorY+$fontSize*1.5;
        imagettftext($im, $fontSize, 0, 20, $cursorY, $black, $DEFAULT_FONT['bold'], $person[0] );
        imagettftext($im, $fontSize*0.6, 0, $displayWidth - 90, $cursorY, $black, $DEFAULT_FONT['regular'], $person[3]);
        $cursorY = $cursorY+$fontSize*1.5;
        imagettftext($im, $fontSize*0.8, 0, 20, $cursorY, $black, $DEFAULT_FONT['regular'], "@" );
        imagettftext($im, $fontSize*0.8, 0, 60, $cursorY, $black, $DEFAULT_FONT['regular'], $person[1]);
        $cursorY = $cursorY+$fontSize*1.5;
        imagettftext($im, $fontSize*0.8, 0, 20, $cursorY, $black, $DEFAULT_FONT['emoji'],"&#9742;");

        imagettftext($im, $fontSize*0.8, 0, 60, $cursorY, $black, $DEFAULT_FONT['regular'], $person[2]);
        $cursorY = $cursorY+$fontSize*0.6;

    }
    imagettftext($im, $fontSize*0.5, 0, $displayWidth - 90, $displayHeight - 8, $black, $DEFAULT_FONT['regular'], date("Y-m-d H:i"));
?>
