<?php
require_once 'ThumbLib.inc.php';
$thumb = PhpThumbFactory::create('http://localhost/commons/sites/default/files/wang_die_.jpg');
$thumb->resize(131, 131);
$thumb->cropFromCenter(131, 77);

//$thumb->show();
$thumb->save('/var/www/ss/wang_jietry.png', 'png');
?>
