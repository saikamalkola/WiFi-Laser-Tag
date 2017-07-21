<!DOCTYPE html>
<html>
<head>
	<title>Results</title>
	<meta charset="utf-8">
	<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<h1>BLITZKREIG TECHNITES</h1>
<?php 
include 'connect_to_db.php';
session_start();
	$team_ID = array($_SESSION['team_A'],$_SESSION['team_B']);
	$hp = $_SESSION['hp'];
	$ammo = $_SESSION['ammo'];
	$players = $_SESSION['players'];
	$emails = $_SESSION['emails'];
	$total_hp_rem = array(0,0);
	$total_ammo_rem = array(0,0);
	$Score = array(0,0);

$sql = "UPDATE player_data SET played = 'played!' WHERE team_id = $team_ID[0] OR team_id = $team_ID[1]";
$response = mysqli_query($connection,$sql);



if(isset($_POST['analyse']))
{	

	for($val = 0; $val < 8; $val++)
	{
		if($val < 4)
		{
			$total_hp_rem[0] += $hp[$val];
			$total_ammo_rem[0] += $ammo[$val];
			$Score[0] += $hp[$val];
			$Score[0] += $ammo[$val];
		}
		else
		{
			$total_hp_rem[1] += $hp[$val];
			$total_ammo_rem[1] += $ammo[$val];
			$Score[1] += $hp[$val];
			$Score[1] += $ammo[$val];
		}
	}
	if($Score[0] > $Score[1])
	{
		//echo "team-1 is the winner!";
		$winner = 0;
	}
	elseif($Score[0] < $Score[1])
	{
		//echo "team-2 is the winner!";
		$winner = 1;

	}
	else
	{
		echo "It's a tie :)";
		$winner = 1;

	}
	$disp_msg = "
	<html>
	<head>
	<title>RESULTS</title>
	<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">
	</head>
	<body>
	<h1>RESULTS</h1> ";

	$table = "<table>
	 	 	<strong>";
	 	 if($val/4 == $winner)
	 	 {	
	 	 	$table = $table."<h2> team:$table_ID[$winner] won the game! </h2>";
	 	 }
	$table = $table."<h2> Winner team Details<br><br> Team: $team_ID[$winner] Score: $Score[$winner] </h2>
	 		<tr>
	 			<th>Player Name</th>
	 			<th>Hp left</th>
	 			<th>Ammo left</th>
	 		</tr>
	 	</strong>";

	 	for($val2 = 0; $val2 < 8 ;$val2++)
	 	{
	 		if((integer)($val2/4) == $winner)
	 		{
	 			$table = $table."<tr>
				<td>$players[$val2]</td>
				<td>$hp[$val2]</td>
				<td>$ammo[$val2]</td>
				</tr>";
	 		}
	 	}
	 	$table = $table."</table> ";
	 	$tmp = !$winner;
		$table = $table."<table><strong><h2> Team: $team_ID[$tmp] Score: $Score[$tmp]
		<tr>
	 			<th>Player Name</th>
	 			<th>Hp left</th>
	 			<th>Ammo left</th>
	 		</tr>
	 	</strong>";
		for($val2 = 0; $val2 < 8; $val2++)
		{
			if((integer)($val2/4) != $winner)
			{
				$table = $table."<tr>
				<td>$players[$val2]</td>
				<td>$hp[$val2]</td>
				<td>$ammo[$val2]</td>
				</tr>";	
			}
		}

	$table = $table."</table> 
	</body>
	</html>";	
	$disp_msg = $disp_msg.$table;
	echo $disp_msg;

	for($val = 0; $val < 8; $val++)
	{
		//send_email($val,$winner);
	}

}
echo "<form action = \"start_session.php\" method=\"POST\">
		<input type=\"submit\" name = \"new_session\" value=\"New Session\">
	  </form>";

function send_email($val,$winner)
{
	global $team_ID ;
	global $hp;
	global $ammo;
	global $players;
	global $emails;
	global $total_hp_rem;
	global $total_ammo_rem;
	global $Score;

	$to = "$emails[$val]";
	$subject = "BLITZKREIG RESULTS";

	$message = "
	<html>
	<head>
	<title>BLITZKREIG RESULTS</title>
	<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">
	</head>
	<body>
	<h1>BLITZKREIG RESULTS</h1> ";

	$table = "<table>
	 	 	<strong>";
	 	 if($val/4 == $winner)
	 	 {	
	 	 	$table = $table."<h2> Congratulations! Your team won the game! </h2>";
	 	 }
	 	 $table = $table."<h2> Winner team Details: Team: $team_ID[$winner] Score: $Score[$winner] </h2>
	 		<tr>

	 			<th>Player Name</th>
	 			<th>Hp left</th>
	 			<th>Ammo left</th>
	 		</tr>
	 	</strong>";

	 	for($val2 = 0; $val2 < 8 ;$val2++)
	 	{
	 		if((integer)($val2/4) == $winner)
	 		{
	 			$table = $table."<tr>
				<td>$players[$val2]</td>
				<td>$hp[$val2]</td>
				<td>$ammo[$val2]</td>
				</tr>";
	 		}
	 	}
	 	$table = $table."</table> ";
	 	$tmp = !$winner;
		$table = $table."<table><h2> Team: $team_ID[$tmp] Score: $Score[$tmp]";

		for($val2 = 0; $val2 < 8; $val2++)
		{
			if((integer)($val2/4) != $winner)
			{
				$table = $table."<tr>
				<td>$players[$val2]</td>
				<td>$hp[$val2]</td>
				<td>$ammo[$val2]</td>
				</tr>";	
			}
		}

	$table = $table."</table> 
	</body>
	</html>";	

	$message = $message.$table;

	// Always set content-type when sending HTML email
	$headers = "MIME-Version: 1.0" . "\r\n";
	$headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";

	// More headers
	$headers .= 'From: <blitzkreig@saikamalkola.com>' . "\r\n";
	$headers .= 'Cc: myboss@example.com' . "\r\n";

	mail($to,$subject,$message,$headers);
}
?>
</body>
</html>