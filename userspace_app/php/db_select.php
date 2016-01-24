<html>
<body>

<?php

$con = mysql_connect("localhost","root","144025");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}

mysql_select_db("php", $con);

$result = mysql_query("SELECT * FROM Persons");

while($row = mysql_fetch_array($result))
{
	echo $row['FirstName'] . " " . $row['LastName'];
	echo "<br/>";
}

mysql_close($con);

?>

</body>
</html>
