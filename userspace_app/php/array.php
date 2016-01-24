<html>
<body>


<?php

$cars=array("Saab", "Volvo", "BMW", "Toyota");
echo $cars[0] . " and " . $cars[1] . " are Swedish cars.";

?>

<br/>

<?php

//array in php could "grow"
$cars[4]="Benz";
echo $cars[4] . " is German car.";

?>

<br/>

<?php
$ages=array("Peter"=>32, "Quagmire"=>30);
$ages["Joe"]=34;

echo "Peter is " . $ages["Peter"] . " years old.\n";
echo "Joe is " . $ages["Joe"] . " years old.\n";
?>

</body>
</html>
