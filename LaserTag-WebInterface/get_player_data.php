<?php 
//connecting to database

include 'connect_to_db.php';

if(isset($_GET['player']) && isset($_GET['hp']) && isset($_GET['ammo']) && isset($_GET['time']))
{
	//fetching data from URL sent by ESP8266
	$player_ID = $_GET['player'];
	$hp = $_GET['hp'];
	$ammo = $_GET['ammo'];
	$time = $_GET['time'];

	$player_data = array();

	if($player_ID < 7)
	{
		$sql = "SELECT * FROM player_data WHERE team_status = 'Playing-1'";
		$response = mysqli_query($connection,$sql);
		while($row = mysqli_fetch_assoc($response))
		{
			if($player_ID == $row['player_id']-4*($row['team_id']-1))
			{	
				$player_ID = $row['player_id'];
				$sql = "UPDATE player_data SET hp = '$hp',ammo = '$ammo',time = '$time' WHERE player_id = '$player_ID'";
				$result = mysqli_query($connection,$sql);
				if($result)
				{
					//echo "successfully updated";
				}

				break;
			}
		}
	}
	else
	{
		$sql = "SELECT * FROM player_data WHERE team_status = 'Playing-2'";
		$response = mysqli_query($connection,$sql);
		while($row = mysqli_fetch_assoc($response))
		{
			if($player_ID == $row['player_id']-4*($row['team_id']-1)+6)
			{	
				$player_ID = $row['player_id'];
				$sql = "UPDATE player_data SET hp = '$hp',ammo = '$ammo',time = '$time' WHERE player_id = '$player_ID'";
				$result = mysqli_query($connection,$sql);
				if($result)
				{
					//echo "successfully updated";
				}
				break;
			}
		}
	}
	$sql = "SELECT * FROM player_data WHERE player_id = '$player_ID'";
	$result = mysqli_query($connection,$sql);
	$row = mysqli_fetch_assoc($result);
	$player_data = array(
			'player_name'=> $row['player_name'],
			'email' => $row['email'],
			'mobile' => $row['mobile'],
			'hp' => $row['hp'],
			'ammo' => $row['ammo'],
			'time' => $row['time'],
			'reset' => $row['reset']
			);
	foreach($player_data as $name => $val)
	{
		switch ($name) {
			case 'player_name':
				echo "#_$val";
				break;
			case 'reset':
				echo "#_$val";
				break;
		}
	}
	echo "#_";
}
?>