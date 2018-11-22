<html lang="en-EN">
<head>
<title>Doorsign</title>
</head>
<body>
     <h1>Let Harry speak!</h1>
<?php 
// list of bad words - 'feel like a little dictator' option
// don't remove '/;/' from filtering list or evil things might 
// happen to your dictatorship uhm... I mean csv file... ;)
$badWords = array('/\bkim\b/i','/fuck/i','/;/'); 

if (!isset($_GET['submess']) || !isset($_GET['name']))  {
    print("<p>Write your message here and let Harry Hamster speak! You have 52 characters.</p>
    <form action=\"harry-talk.php\">
        your message:<br><textarea name=\"submess\" rows=\"3\" cols=\"24\" maxlength=\"52\" autofocus required wrap=\"soft\"></textarea><br>
        your name: <input type=\"text\" name=\"name\" maxlength=\"8\" size=\"8\" required><br>
        <input type=\"submit\" value=\"Let Harry speak!\">
    </form>");
}
else {
    $writeout = true;

    // check for length
    if(mb_strlen($_GET['submess']) > 52) {
        print("<p>Message to long</p>");
        $writeout = false;
    } 
    if (strlen($_GET['name']) > 8) {
        print("<p>Name to long!</p>");
        $writeout = false;
    }

    // check for to long words
    $words = explode(' ', $_GET['submess']);
    foreach ( $words as $word ) {
        if (mb_strlen( $word ) > 13) {
            print("<p>You are using to long words. The character per word limit is 13.</p>");
            $writeout = false;
        }
    }

    // if everything is fine write message to file
    if ($writeout == true) {

        // some filtering I'm sure I've been missing something...
        $message = trim($_GET['submess']);
        $name = $_GET['name'];
        $message = preg_replace($badWords, "*", $message);
        $message = str_replace(array("\r", "\n"), '', $message);
        $name = preg_replace($badWords, "*", $name);
        $name = str_replace(array("\r", "\n"), '', $name);

        $messinput = file("./contents/harry/messages.txt");
        $fp = fopen("./contents/harry/messages.txt", "w");
        fwrite($fp, $message.";".$name." - ".date("d.m.Y H:i")."\n");
        fwrite($fp, $messinput[0]);
        fwrite($fp, $messinput[1]);
        fclose($fp);
        print("</p>Your message was accepted! Soon it will be shown on the display.</p>");
    }
    else {
        print("<p><a href=\"harry-talk.php\">I try it again!</a></p>");
    }
}

?>
    <h2>display content right now:<h2>
        <img src="index.php?debug=true&content=harry&display=4.2">
    <p>
</body>
