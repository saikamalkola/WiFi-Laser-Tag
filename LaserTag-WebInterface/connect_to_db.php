<?php 
	//LaserTag Database Details
	define('DB_USER','root');	//username that can access database
	define('DB_PASSWORD', '');	//password of database user
	define('DB_NAME', 'lasertag');	//database name
	define('DB_HOST', 'localhost');	//host or domain in which database is located

//connecting to LaserTag database 	
$connection = mysqli_connect(DB_HOST,DB_USER,DB_PASSWORD,DB_NAME);

//If connection failed then show the error!
if(!$connection)
{	
	die("Connection Failed :".mysqli_connect_error());
}

?>