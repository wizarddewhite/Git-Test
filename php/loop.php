<html>
<body>


<?php

$i=1;
while($i<=5)
{
	echo "The number is " . $i . "<br>";
	$i++;
}

for ($i=6; $i<=10;$i++)
{
	echo "The number is " . $i . "<br>";
}

$x=array("one", "two", "three");
foreach ($x as $value)
{
	echo $value . "<br>";
}

?>

</body>
</html>
