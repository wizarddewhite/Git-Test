<html>
    <body>
    <div align="center">
<a href="logout.php">logout</a><br>
<b>Staff Id:</b> <?session_start();echo $_SESSION['userid']?><br>
<b>Staff Name:</b> <?session_start();echo $_SESSION['username']?><br>
<?php 
    $dbhost = 'localhost'; 
    $dbuser = 'stucou'; 
    $dbpass = 'stucou123'; 
    $dbname = 'STUCOU'; 
    
    $teacher_query_course = 'SELECT COURSE.CID AS ID, 
                             COURSE.CNAME AS NAME, 
                             AVG( STUCOU.SCORE ) AS AVG_SCORE
                             FROM STUDENT, COURSE, STUCOU
                             WHERE COURSE.TID = %s
                             AND STUDENT.SID = STUCOU.SID
                             AND COURSE.CID = STUCOU.CID
                             GROUP BY STUCOU.CID;';

    $connect = mysql_connect($dbhost,$dbuser,$dbpass); 
    if ($connect)
    {
        mysql_select_db($dbname, $connect);
        session_start();
        $sql = sprintf($teacher_query_course, $_SESSION['userid']);
        $result = mysql_query($sql, $connect);
        if(!$result)
        {
            echo mysql_error($connect);
            mysql_close($connect);
        }
        //echo mysql_num_rows($result);
        ?>
        <br>
        <table width="50%" border="0">
            <caption><b>OPEN COURSE and AVG SCORE</b></caption>
            <tr>
                <td><b>COURSE ID</b></td>
                <td><b>COURSE NAME</b></td>
                <td><b>AVG SCORE</b></td>
            </tr>
        <?
        while($row = mysql_fetch_object($result))
        {?>
            <tr>
                <td><?echo $row->ID;?></td>
                <td>
                    <a href="course_detail.php?cid=<?echo $row->ID;?>">
                           <?echo $row->NAME;?>
                    </a>
                </td>
                <td><?echo $row->AVG_SCORE;?></td>
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
