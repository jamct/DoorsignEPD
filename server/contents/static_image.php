<?php
    //Place your image in folder "static_image". This code will take a random image
    $contents = scandir('contents/static_image');
    if(!count($contents)){
        exit;
    }
    foreach ($contents as $content) {
        $contentFile = pathinfo($content);

        if($contentFile['extension'] == "png" OR $contentFile['extension'] == "jpg" OR $contentFile['extension'] == "jpeg"){
            $someFile['ext'] = $contentFile['extension'];
            $someFile['path'] = "contents/static_image/".$content;
            $allFiles[] = $someFile;
        }
    }

        $content = $allFiles[array_rand($allFiles)];

        if($content['ext'] == "png"){
            $imageSource = imagecreatefrompng($content['path']);

        }
        if($content['ext'] == "jpg" OR $content['ext'] == "jpeg" ){
            $imageSource = imagecreatefromjpeg($content['path']);

        }

        list($origW, $origH) = getimagesize($content['path']);

    $width = $origW;
    $height = $origH;

    $maxW = $displayWidth ;
    $maxH = $displayHeight;

    if ($height > $maxH) {
        $width = ($maxH / $height) * $width;
        $height = $maxH;
    }

    if ($width > $maxW) {
        $height = ($maxW / $width) * $height;
        $width = $maxW;
    }
    $imageNew = imagecreate($displayWidth, $displayHeight);
    imagecopyresampled($imageNew, $imageSource, 0, 0, 0, 0, $width, $height, $origW, $origH);
    imagecopymerge($im, $imageNew, 0, 0, 0, 0, $displayWidth, $displayHeight,100);
    imagedestroy($imagenew);
?>
