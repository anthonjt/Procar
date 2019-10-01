<?php
//Basic argon IPMC
$servername = "localhost";
$username = $_GET['username'];
$password = $_GET['password'];
$dbname = $_GET['dbname'];
$tablename = $_GET['tablename'];

//Create connection
$conn = mysqli_connect($servername,$username,$password,$dbname);
//check connection
if(!$conn){
        die("Connection Failed: . mysqli_connect_error()");
}
$k64fval = $_GET['k64fval'];
$espval = $_GET['espval'];
$IPaddress = $_GET['IPaddress'];
//sql to create table
$sql = "INSERT INTO $tablename(k64fval,espval,IPaddress)
VALUES('$k64fval','$espval','$IPaddress')";
if(mysqli_query($conn,$sql)){
        echo "table row created successfully";
}
else{
        echo "Error creating table: " .mysqli_error($conn);;
}
mysqli_close($conn);
?>
