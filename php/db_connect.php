<html>
<body>

<?php

$con = mysql_connect("localhost","root","144025");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}
else
{
	echo "connected!";
	mysql_close($con);
}

?>

</body>
</html>
