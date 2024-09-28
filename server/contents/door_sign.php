<?php
    checkFreeType();
    // define your settings here
    const ROOM = 'Raum 267';
    const PERSONS = array("John Doe", "Jane Doe", "Otto Normalverbraucher");

    $fontSize = $scale;
    $cursorY = 0;

    $cursorY += (int)round($fontSize*1.5);
    imagettftext($im, $fontSize, 0, 10, $cursorY, $red, $DEFAULT_FONT['bold'], ROOM);
    $cursorY += 5;
    imageline ($im , 10 , $cursorY , $displayWidth - 20, $cursorY , $black );

    $fontSize = (int)round(0.5*$scale);
    foreach(PERSONS as $person){
        $cursorY = (int)round($cursorY+$fontSize*1.5);
    imagettftext($im, $fontSize, 0, 20, $cursorY, $black, $DEFAULT_FONT['regular'], $person );
    }

?>
