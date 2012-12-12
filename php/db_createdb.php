<html>
<body>

<?php

$con = mysql_connect("localhost","root","144025");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}

if (mysql_query("CREATE DATABASE php", $con))
{
	echo "Database php created";
}


mysql_close($con);

?>

</body>
</html>
