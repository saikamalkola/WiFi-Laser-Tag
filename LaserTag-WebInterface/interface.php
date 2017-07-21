<!DOCTYPE html>
<html>
<head>
	<title>Interface</title>
	<meta charset="utf-8">
	<link rel="stylesheet" type="text/css" href="style.css">
	<meta http-equiv="refresh" content="15">
</head>
<body>
<h1>BLITZKREIG INTERFACE</h1>
<?php 
include 'connect_to_db.php';
session_start();
$team_ID = array($_SESSION['team_A'],$_SESSION['team_B']);

echo "<form action = \"interface.php\" method=\"POST\">
		<input type=\"submit\" name = \"reset\" value=\"Reset\">
	  </form>";
echo "<form action = \"interface.php\" method=\"POST\">
		<input type=\"submit\" name = \"set\" value=\"set\">
	  </form>";
if(isset($_POST['reset']))
{	
	$sql = "UPDATE player_data SET reset = '0'";
	$response = mysqli_query($connection,$sql);
	$sql = "UPDATE player_data SET reset = '1' WHERE team_id = $team_ID[0] OR team_id = $team_ID[1]";
	$response = mysqli_query($connection,$sql);
}
if(isset($_POST['set']))
{	
	$sql = "UPDATE player_data SET reset = '0'";
	$response = mysqli_query($connection,$sql);
	$sql = "UPDATE player_data SET reset = '0' WHERE team_id = $team_ID[0] OR team_id = $team_ID[1]";
	$response = mysqli_query($connection,$sql);
}

for($id = 0; $id < 2 ;$id++)
	{	
		$int_id = (integer)($team_ID[$id]);
		$sql = "SELECT * FROM player_data where team_id = '$int_id'";
		$result = mysqli_query($connection,$sql);

		while($row = mysqli_fetch_assoc($result))
		{	

			for($val = 0 ; $val < 8; $val++)
			{
				switch ($val) 
				{
					case 0:

						$players[] = $row['player_name'];
						break;
					case 1:
						$emails[] = $row['email'];
						break;
					case 2:
						$mobileNos[] = $row['mobile'];
						break;
					case 3:
						$hp[] = $row['hp'];
						break;
					case 4:
						$ammo[] = $row['ammo'];
						break;
					case 5:
						$player_id[] = $row['player_id'];
						break;
					case 6:
						$team_status[] = $row['team_status'];
						break;
					case 7:
						$time[] = (integer)($row['time']/(1000));
						break;
					default:
						echo "fetching data failed";
						break;
				}
			}
		}
	}

foreach ($time as $val) 
	{
		if((integer)$val >= 299)
		{
			//TIME UP
			$time_up = 1;
			break;
		}
		else
		{
			$time_up = 0;
		}
	}	

$temp = 1;

for($val = 0 ; $val < 2 ; $val++)
{	
	echo "<table>
 	 <strong>
 	 	<h2> Team-$team_ID[$val] details </h2>
 		<tr>
 			<th>Player ID</th>
 			<th>Player Name</th>
 			<th>Email</th>
 			<th>Mobile No</th>
 			<th>Hp</th>
 			<th>Ammo</th>
 			<th>Team Status</th>
 			<th>Time Remaining </th>
 		</tr>
 	</strong>";
 	if($temp == 1)
 	{
	 	for($val2 = 0; $val2 < 4; $val2++)
	 	{
	 		echo "<tr>";
			echo "<td>$player_id[$val2]</td>";
			echo "<td>$players[$val2]</td>";
			echo "<td>$emails[$val2]</td>";
			echo "<td>$mobileNos[$val2]</td>";
			echo "<td>$hp[$val2]</td>";
			echo "<td>$ammo[$val2]</td>";
			echo "<td>$team_status[$val2]</td>";
			if($time_up == 0)
				echo "<td>$time[$val2]</td>";
			else
				echo "<td>Time Up!</td>";
			echo "</tr>";
		}
		echo "</table>";	
 	}
 	
 	if($temp == 2)
 	{
 		for($val2 = 4; $val2 < 8; $val2++)
	 	{
	 		echo "<tr>";
			echo "<td>$player_id[$val2]</td>";
			echo "<td>$players[$val2]</td>";
			echo "<td>$emails[$val2]</td>";
			echo "<td>$mobileNos[$val2]</td>";
			echo "<td>$hp[$val2]</td>";
			echo "<td>$ammo[$val2]</td>";
			echo "<td>$team_status[$val2]</td>";
			if($time_up == 0)
				echo "<td>$time[$val2]</td>";
			else
				echo "<td>Time Up!</td>";
			echo "</tr>";
		}
		echo "</table>";	
 	}
	$temp++;
}
if($time_up == 1)
{
	$_SESSION['hp'] = $hp;
	$_SESSION['ammo'] = $ammo;
	$_SESSION['players'] = $players;
	$_SESSION['emails'] = $emails;
	echo "<form action = \"analyse.php\" method=\"POST\">
		<input type=\"submit\" name = \"analyse\" value=\"Results\">
	  </form>";
}
 ?>
</body>
</html>