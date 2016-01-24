<html>
<body>

<?php

$con = mysql_connect("localhost","root","144025");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}

mysql_select_db("php", $con);

$sql="CREATE TABLE Persons 
(
FirstName varchar(15), 
LastName varchar(15), 
Age int
)";

mysql_query($sql, $con);

mysql_close($con);

?>

</body>
</html>
