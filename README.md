# Pong
1v1 Pong Minigame Include (played on an LCD Screen)


Create TVs anywhere to play Pong versus another player.


	Functions:

	CreatePongGame(Float:x, Float:y, Float:z, Float:rx, Float:rz, interior = 0, virtualworld = 0)
		Creates an LCD TV at the given coordinates and returns its ID, -1 if failed

	DestroyPongGame(id)
		Destroys a Pong Game TV

	IsValidPongGame(id)
		Returns whether or not the specified ID is valid.


	HostPongGame(id, playerid_a, playerid_b = -1)
		Starts a specific Pong Game for a player (Lobby)

	PutPlayerInPongGame(playerid, id)
		Puts a Player into a Pong Game

	StartPongGame(id)
		Starts a Pong Game


	EndPongGameForPlayer(playerid, bool:finished = false)
		Ends a Pong Game for a specific player (the game will be aborted if it's currently running)
		During the Lobby new players can join

	EndPongGame(id, bool:finished = false)
		Ends a Pong Game


	GetPlayerPongArea(playerid)
		Returns the Pong Game ID the player is currently close to, -1 if not close to any


	SetPongGameScore(id, score)
		Sets the target score per Round for a specific game

	GetPongGameScore(id)
		Returns the target score


	SetPongGameRounds(id, rounds)
		Sets the target number of rounds for a specific game

	GetPongGameRounds(id)
		Returns the number of rounds


	SetPongGameSpeed(id, Float:speed)
		Sets the Ball Speed Multiplier for a specific game

	GetPongGameRounds(id)
		Returns the game speed (float)


	GetPongGameState(id)
		Returns the Game's State, -1 if invalid


	GetPongGameInterior(id)
		Returns the Game's Interior, -1 if invalid

	GetPongGameVirtualWorld(id)
		Returns the Game's Virtual World, -1 if invalid

https://imgur.com/a/MhhPt
