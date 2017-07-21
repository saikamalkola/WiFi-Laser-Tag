<?php 
	include 'connect_to_db.php';
 ?>

 <!DOCTYPE html>
 <html>
 <head>
 	<title>Blitzkreig</title>
 	<meta charset="utf-8">
 	<link rel="stylesheet" type="text/css" href="style.css">
 </head>
 <body>
 <h1>
 	BLITZKREIG-TECHNITES
 </h1>
 <form action="register.php" method = "POST">
 	<table>
 	 		<strong>
 			<tr>
 				<th>S.No</th>
 				<th>Roll Number</th>
 				<th>Player Name</th>
 				<th>Email</th>
 				<th>Mobile No</th>
 			</tr>
 			</strong>
 			<tr>
 				<td>1</td>
 				<td><input type="text" name="player_1_rollNo" placeholder="Eg: 156721" required></td>
 				<td><input type="text" name="player_1_name" placeholder="Player-1" required></td>
 				<td><input type="email" name="player_1_email" placeholder="Email" required></td>
 				<td><input type="text" name="player_1_mobile" placeholder="Mobile No" required></td>
 			</tr>
 			<tr>
 				<td>2</td>
 				<td><input type="text" name="player_2_rollNo" placeholder="Eg: 156721" required></td>
 				<td><input type="text" name="player_2_name" placeholder="Player-2" required></td>
 				<td><input type="email" name="player_2_email" placeholder="Email" required></td>
 				<td><input type="text" name="player_2_mobile" placeholder="Mobile No" required></td>
 			</tr>
 			<tr>
 				<td>3</td>
 				<td><input type="text" name="player_3_rollNo" placeholder="Eg: 156721" required></td>
 				<td><input type="text" name="player_3_name" placeholder="Player-3" required></td>
 				<td><input type="email" name="player_3_email" placeholder="Email" required></td>
 				<td><input type="text" name="player_3_mobile" placeholder="Mobile No" required></td>
 			</tr>
 			<tr>
 				<td>4</td>
 				<td><input type="text" name="player_4_rollNo" placeholder="Eg: 156721" required></td>
 				<td><input type="text" name="player_4_name" placeholder="Player-4" required></td>
 				<td><input type="email" name="player_4_email" placeholder="Email" required></td>
 				<td><input type="text" name="player_4_mobile" placeholder="Mobile No" required></td>
 			</tr>
 	</table>
 	<input type="submit" name="register" value = "Register">
 </form>
 </body>
 </html>

