<?php
    checkFreeType();
    // define your settings here
    const ROOM = 'Raum 267';
    const PERSONS = array("John Doe", "Jane Doe", "Otto Normalverbraucher");

    // if display has a color, use it!
    $fontSize = $scale;
    if(DISPLAYS[$displayType]['color'] == "red") {
        $fontcolor = $red;
    }
    elseif (DISPLAYS[$displayType]['color'] == "yellow") {
        $fontcolor = $yellow;
    }
    else {
        $fontcolor = $black;
    }

    $cursorY += $fontSize*1.5;
    imagettftext($im, $fontSize, 0, 10, $cursorY, $fontcolor, $DEFAULT_FONT['bold'], ROOM);
    $cursorY += 5;
    imageline ($im , 10 , $cursorY , $displayWidth - 20, $cursorY , $black );

    $fontSize = 18;
    foreach(PERSONS as $person){
        $cursorY = $cursorY+$fontSize*1.5;
    imagettftext($im, $fontSize, 0, 20, $cursorY, -$black, $TERMINUS_FONT['regular'], $person );
    }

?>
