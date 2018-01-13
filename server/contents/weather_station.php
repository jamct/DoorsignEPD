<?php
	//Using Weather-Icons by Erik Flowers. All codes can be found here: https://github.com/erikflowers/weather-icons/blob/master/css/weather-icons.css
	const ICONS = array("sunny"=>'&#xf00d;', "cloudy"=>'&#xf002;', "foggy"=>'&#xf003;', "windy"=>'&#xf085;', "snow"=>"&#xf00a;");
	
	//YOUR DATA
	$weather = array("today"=>array("icon"=>"snow", "temp"=>"-2°/1°"),"tomorrow"=>array("icon"=>"cloudy", "temp"=>"-3°/-1°"));
	
	
	$fontSize = $scale;	
	//Write weather for today
    imagettftext($im, $scale*4, 0, 10, $scale*6, $black, realpath("./fonts/weathericons-regular-webfont.ttf"), ICONS[$weather['today']['icon']]);
	imagettftext($im, $scale*4, 0, $scale*9, $scale*6, $black, $DEFAULT_FONT['regular'], $weather['today']['temp']);
	
	//Write weather for tomorrow
    imagettftext($im, $scale*2, 0, $scale*8, $scale*10, $black, realpath("./fonts/weathericons-regular-webfont.ttf"), ICONS[$weather['tomorrow']['icon']]);
	imagettftext($im, $scale*2, 0, $scale*13, $scale*10, $black, $DEFAULT_FONT['regular'], $weather['tomorrow']['temp']);