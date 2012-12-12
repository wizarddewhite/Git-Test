<html>
    <body>
    <div align="center">
<b>Staff Id:</b> <?session_start();echo $_SESSION['userid'];?><br>
<b>Staff Name:</b> <?echo $_SESSION['username'];?><br>

<?php 
    $dbhost = 'localhost'; 
    $dbuser = 'stucou'; 
    $dbpass = 'stucou123'; 
    $dbname = 'STUCOU'; 
    
    $course_query_detail = 'SELECT STUDENT.SID AS ID,
                            STUDENT.SNAME AS NAME,
                            STUCOU.SCORE AS SCORE
                            FROM STUDENT, COURSE ,STUCOU
                            WHERE COURSE.CID = %s
                            AND STUDENT.SID = STUCOU.SID
                            AND COURSE.CID = STUCOU.CID;';
    
    $connect = mysql_connect($dbhost,$dbuser,$dbpass); 
    if ($connect)
    {
        mysql_select_db($dbname, $connect);
        $sql = sprintf($course_query_detail, $_GET['cid']);
        $result = mysql_query($sql, $connect);
        if(!$result)
        {
            echo mysql_error($connect);
        }
        //echo mysql_num_rows($result);
        ?>
        <br>
        <table width="50%" border="0">
            <caption><b>STUDENT and SCORE</b></caption>
            <tr>
                <td>STUDENT ID</td>
                <td>STUDENT NAME</td>
                <td>SCORE</td>
            </tr>
        <?
        while($row = mysql_fetch_object($result))
        {?>
            <tr>
                <td><?echo $row->ID;?></td>
                <td><?echo $row->NAME;?></td>
                <td><?echo $row->SCORE;?></td>
            </tr>
        <?}?>
        </table>
        <a href="teacher_query.php">back</a>
        <?
    } 
    else
    { 
        echo "can not connect";
    }
    mysql_close($connect);
?> 
    </div>
    </body>
</html>
