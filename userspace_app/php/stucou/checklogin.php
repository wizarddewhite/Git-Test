<?php 
    $dbhost = 'localhost'; 
    $dbuser = 'stucou'; 
    $dbpass = 'stucou123'; 
    $dbname = 'STUCOU'; 
    
    $student_sql = "SELECT SID AS ID, SNAME AS NAME, 
                        PASSWORD FROM STUDENT WHERE SID = %s AND PASSWORD = '%s';";
    $teacher_sql = "SELECT TID AS ID, TNAME AS NAME, 
                        PASSWORD FROM TEACHER WHERE TID = %s AND PASSWORD = '%s';";
    
    $connect = mysql_connect($dbhost,$dbuser,$dbpass); 
    if ($connect)
    {
        mysql_select_db($dbname, $connect);
        $sql = '';
        $target = '';
        $usertype = $_POST['type'];
        if($usertype == 't')
        {
            $sql = $teacher_sql;   
            $target = 'teacher_query.php';
        }
        else if($usertype =='s')
        {
            $sql = $student_sql;
            $target = 'student_query.php';
        }
        else
        {
            echo 'user type error';
            mysql_close($connect);
            die();
        }
        $sql = sprintf($sql, $_POST['userid'], md5($_POST['password']));
        $result = mysql_query($sql, $connect);
        if(!$result)
        {
            echo mysql_error($connect);
            mysql_close($connect);
            die();
        }
        if(mysql_num_rows($result) != 0)
        {
            $row = mysql_fetch_object($result);
            session_start();
            $_SESSION['userid'] = $row->ID;
            $_SESSION['username'] = $row->NAME;
            mysql_close($connect);
            header("Location: $target");
        }
        else
        {
            mysql_close($connect);
            header('Location: login.php');
        }
    } 
    else
    { 
        echo "can not connect";
    }
?> 
