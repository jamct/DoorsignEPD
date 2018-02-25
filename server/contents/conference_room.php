<?php
    checkFreeType();
    // define your settings here
    const ROOM = 'Raum 267';
    const TIMES = array("7:30", "8:00", "8:30", "9:00", "9:30", "10:00", "10:30", "11:00", "11:30", "12:00");
    const DAYS = array("Mo", "Di", "Mi", "Do", "Fr");


    //YOUR DATA
    $bookings = array(0=>array(0=>"Müller"),2=>array(4=>"Schulze", 2=>"Doe"),4=>array(1=>"Meyer") );

    $fontSize = $scale;
    //Write room name
    $cursorY += $fontSize*1.5;
    imagettftext($im, $fontSize, 0, 10, $cursorY, $black, $DEFAULT_FONT['bold'], ROOM);
    $cursorY += 5;
    imageline ($im , 10 , $cursorY , 1000 , $cursorY , $black );

    $cursorX = $scale*2.1;
    $cursorY += $scale*1.5;
    $line0 = $cursorY;
    imageline ($im ,  $cursorX ,$cursorY ,  $cursorX , 1000, $black );
    $cursorY2 = $cursorY;
    //
    foreach(TIMES as $time){
        $cursorY2 += $scale*1.2;
        imagettftext($im, 0.5*$scale, 0, 5,$cursorY2, $black, $DEFAULT_FONT['regular'], $time);
    }


    for($i=0; $i<count(DAYS)-1; $i++){
        $cursorX += $scale*3.8;
        imageline ($im ,  $cursorX ,$cursorY ,  $cursorX , 1000, $black );

    }

    //Write Column-Heads
    $cursorX = -$scale*3;
    for($i=0; $i<5; $i++){
        $cursorX += $scale*3.8;
        imagettftext($im, 0.5*$scale, 0, $cursorX+50, $cursorY, $black, $DEFAULT_FONT['regular'], DAYS[$i]);
    }

    //Write Table cells with bookings
    $cursorX = -$scale*1.7;
    for($col=0; $col<5; $col++){
        $cursorY = $line0;
        $cursorX += $scale*3.8;
        for($row=0; $row<10; $row++){
            $cursorY += $scale*1.2;
            imagettftext($im, 0.5*$scale, 0, $cursorX, $cursorY, $black, $DEFAULT_FONT['regular'], $bookings[$col][$row]);
        }
    }
?>
