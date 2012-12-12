<html>
<body>

<?php

$a = 5; // global scope
$b = 8;

function func1()
{
	echo $a; //local scope
}

func1();

function func2()
{
	global $a, $b; // global scope
	$b = $a + $b;
}

func2();
echo "$a + $b = "; echo $b;
?><br>

<?php
function func3()
{
	$GLOBALS['b'] = $GLOBALS['a'] + $GLOBALS['b'];
}

func3();
echo "$a + $b = "; echo $b;
?><br>

<?php

function func4($para1, $para2)
{
	$GLOBALS['b'] = $para1 + $para2;
}

func4(9, 10);
echo "9 + 10 = "; echo $b;

?>

<?php

function func5($para1)
{
	return $para1 % 5;
}

echo "<br/>23 % 5 = " . func5(23);

?>


</body>
</html>
