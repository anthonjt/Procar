<?php
//Basic table for argon IPMC
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

//sql to create table can change values to whatever
$sql = "CREATE TABLE $tablename(
reg_date TIMESTAMP,
id INT(12) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
k64fval VARCHAR(15) NOT NULL,
espval VARCHAR(15) NOT NULL,
IPaddress VARCHAR(15) NOT NULL
)";

if(mysqli_query($conn,$sql)){
        echo "Table created successfully";
}
else{
        echo "Error creating table: " . mysqli_error($conn);;
}
mysqli_close($conn);
?>
