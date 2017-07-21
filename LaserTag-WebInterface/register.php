<!DOCTYPE html>
<html>
<head>
	<title>Registration Successfull</title>
	<meta charset="utf-8">
	<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>


<?php
include 'connect_to_db.php';

$sql = "SELECT * FROM player_data WHERE player_id=(SELECT MAX(player_id) FROM player_data)";
$response = mysqli_query($connection,$sql);
$max_id = 0;
if($response)
{
	$last_row = mysqli_fetch_assoc($response);
	$max_id = (integer)($last_row["player_id"]);
	//echo $max_id;
}
$player_id = $max_id + 1;	
$team_id =  ($max_id/4)+1;
//echo "<br>".$team_id;
if(isset($_POST['register']))
{
	$players = array($_POST['player_1_name'],$_POST['player_2_name'],$_POST['player_3_name'],$_POST['player_1_name']);
	$emails	 = array($_POST['player_1_email'],$_POST['player_2_email'],$_POST['player_3_email'],$_POST['player_4_email']);
	$mobileNos = array($_POST['player_1_mobile'],$_POST['player_2_mobile'],$_POST['player_3_mobile'],$_POST['player_4_mobile']);

	for($val = 0; $val < 4 ; $val++)
	{	
		//echo "Player 1: $players[$val] Email : $emails[$val] Mobile : $mobileNos[$val] <br>";
		$sql = "INSERT INTO player_data (player_id,player_name,email,mobile,team_id) VALUES('$player_id','$players[$val]','$emails[$val]','$mobileNos[$val]','$team_id')";
		$player_id++;
		$result = mysqli_query($connection,$sql);
		//echo $result."<br>";
	}

	$player_arr = array();
	$email_arr = array();
	$mobileNo_arr = array();
	$hp = array();
	$ammo = array();
	$player_id_arr = array();
	$team_status = array();

	$sql = "SELECT * FROM player_data where team_id = '$team_id'";
	$result = mysqli_query($connection,$sql);

	while($row = mysqli_fetch_assoc($result))
	{	
		for($val = 0 ; $val < 7; $val++)
		{
			switch ($val) 
			{
				case 0:
					$player_arr[] = $row['player_name'];
					break;
				case 1:
					$email_arr[] = $row['email'];
					break;
				case 2:
		 			$mobileNo_arr[] = $row['mobile'];
					break;
				case 3:
					$hp[] = $row['hp'];
					break;
				case 4:
					$ammo[] = $row['ammo'];
					break;
				case 5:
					$player_id_arr[] = $row['player_id'];
					break;
				case 6:
					$team_status[] = $row['team_status'];
					break;
				default:
					echo "fetching data failed";
					break;
			}
		}
	}
	echo "<table>
 	 <strong>
 	 	<h2> Registration Successful! Your Team ID is : $team_id</h2>
 		<tr>
 			<th>Player ID</th>
 			<th>Player Name</th>
 			<th>Email</th>
 			<th>Mobile No</th>
 			<th>Hp</th>
 			<th>Ammo</th>
 			<th>Team Status</th>
 		</tr>
 	</strong>";

 	for($val2 = 0; $val2 < 4; $val2++)
	 	{
	 		echo "<tr>";
			echo "<td>$player_id_arr[$val2]</td>";
			echo "<td>$player_arr[$val2]</td>";
			echo "<td>$email_arr[$val2]</td>";
			echo "<td>$mobileNo_arr[$val2]</td>";
			echo "<td>$hp[$val2]</td>";
			echo "<td>$ammo[$val2]</td>";
			echo "<td>$team_status[$val2]</td>";
			echo "</tr>";
		}
		echo "</table>";
 	mysqli_close($connection);
}
?>

<h2> Note down ur team ID. You need this ID to play this game during Technites :) </h2>
<form action = "user_form.php" method="POST">
		<input type="submit" name = "" value="Home">
	  </form>"
</body>
</html>