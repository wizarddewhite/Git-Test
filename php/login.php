<html>
    <body>
        <div align="center">
        <form action="checklogin.php" method="post">
            <table border=0>
            <tr>
                <td>User ID:</td>
                <td><input name="userid" type="text"/></td>
            </tr>
            <tr>
                <td>Password:</td>
                <td><input name="password" type="password"/></td>
            </tr>
            <tr>
                <td>User Type:</td>
                <td>
                    <select name="type">
                        <option value="t">Teacher</option>
                        <option value="s">Student</option>
                    </select>
                </td>
            </tr>
            <tr>
                <td></td>
                <td align="right"><input value="login" type="submit"/></td>
            </tr>
            </table>
        </form>
        </div>
    </body>
</html>
