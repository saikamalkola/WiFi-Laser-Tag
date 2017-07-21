<!DOCTYPE html>
<html>
<head>
	<title>Teams Details</title>
	<meta charset="utf-8">
	<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<form method="POST">
<h1>BLITZKREIG-TECHNITES</h1>
	<p >Enter ID of Team-A:	<input type="text" name="team_1" placeholder="Team-1 ID" required><br></p>
	<p>Enter ID of Team-B:	<input type="text" name="team_2" placeholder="Team-2 ID" required><br></p>
	<input id = 'team_button' type="submit" name = "submit" value="submit">
</form>

<?php  
include 'connect_to_db.php';
$players = array();
$emails = array();
$mobileNos = array();
$hp = array();
$ammo = array();
$player_id = array();
$team_status = array();

if(isset($_POST['submit']))
{

	$sql = "UPDATE player_data SET team_status = 'not played'";
	$response = mysqli_query($connection,$sql);
	if(!$response)
	{
		echo "update failed";
	}

	$team_ID = array($_POST['team_1'],$_POST['team_2']);
	for($id = 0; $id < 2 ;$id++)
	{	
		$status = array('Playing-1','Playing-2');
		$int_id = (integer)($team_ID[$id]);
		$sql_update_status = "UPDATE player_data SET team_status = '$status[$id]' WHERE team_id = $team_ID[$id]";
		$status_update = mysqli_query($connection,$sql_update_status);
		if(!$status_update)
		{
			echo "Status update failed".mysqli_error($connection);
		}

		$sql = "SELECT * FROM player_data where team_id = '$int_id'";
		$result = mysqli_query($connection,$sql);

		while($row = mysqli_fetch_assoc($result))
		{	

			for($val = 0 ; $val < 7; $val++)
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
					default:
						echo "fetching data failed";
						break;
				}
			}
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
			echo "<td>$team_status[$val]</td>";
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
			echo "</tr>";
		}
		echo "</table>";	
 	}
	$temp++;
}
session_start();
$_SESSION['team_A'] = $team_ID[0];
$_SESSION['team_B'] = $team_ID[1];

echo "<form action = \"interface.php\" method=\"POST\">
		<input type=\"submit\" name = \"start_session\" value=\"Start Session\">
	  </form>";
}

?>

</body>
</html>