<html>
    <body>
    <div align="center">
        <a href="logout.php">logout</a><br>
        <b>Student Id:</b> <?session_start();echo $_SESSION['userid']?><br>
        <b>Student Name:</b> <?session_start();echo $_SESSION['username']?><br>

<?php 
    $dbhost = 'localhost'; 
    $dbuser = 'stucou'; 
    $dbpass = 'stucou123'; 
    $dbname = 'STUCOU'; 
    
    $student_query_course = 'SELECT COURSE.CID AS ID, 
                                    COURSE.CNAME AS NAME, 
                                    TEACHER.TNAME AS TEACHER, 
                                    STUCOU.SCORE AS SCORE
                             FROM STUDENT, COURSE, STUCOU, TEACHER
                             WHERE STUCOU.SID = %s
                             AND TEACHER.TID = COURSE.TID
                             AND STUDENT.SID = STUCOU.SID
                             AND COURSE.CID = STUCOU.CID;';

    $connect = mysql_connect($dbhost,$dbuser,$dbpass); 
    if ($connect)
    {
        mysql_select_db($dbname, $connect);
        $sql = sprintf($student_query_course, $_SESSION['userid']);
        $result = mysql_query($sql, $connect);
        if(!$result)
        {
            echo mysql_error($connect);
            mysql_close($connect);
        }
        //echo mysql_num_rows($result);
        ?>
        <br>
        <br>
        <table width="50%" border="0">
            <caption><b>COURSE and SCORE</b></caption>
            <tr>
                <td><b>COURSE ID</b></td>
                <td><b>COURSE NAME</b></td>
                <td><b>TEACHER</b></td>
                <td><b>SCORE</b></td>
            </tr>
        <?
        while($row = mysql_fetch_object($result))
        {?>
            <tr>
                <td><?echo $row->ID;?></td>
                <td><?echo $row->NAME;?></td>
                <td><?echo $row->TEACHER;?></td>
                <td><?echo $row->SCORE;?></td>
            </tr>
        <?}?>
        </table>
        <?
    } 
    else
    { 
        echo "can not connect";
    }
?> 
    </div>
    </body>
</html>
