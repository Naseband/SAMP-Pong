#if defined _PONG_INC_
#endinput
#endif
#define _PONG_INC_

/*
	1v1 Pong Minigame by NaS

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


	GetPongPlayers(id, playerid_a = 0, playerid_b = 0)
		Gets the current players (optional) and returns the number of players in a pong game, -1 if invalid


	GetPlayerPongID(playerid)
		Returns the Player's Pong ID if in any game, -1 if not connected or not in a game

*/

// ####################################################################################### // Defines (Limits/Config)

#define MAX_PONG_GAMES			10 // Max. number of Pong TVs

#define PONG_TIMER_INTERVAL		75 // Timer Interval for Controls & Ball
#define PONG_MODEL				19786
#define PONG_CAMERA_DISTANCE	2.8 // Distance of the camera to the Screen

#define PONG_PADDLE_MODEL		19874
#define PONG_PADDLE_OFF_RX		0.0 // Rotation offset of the Paddles
#define PONG_PADDLE_OFF_RY		270.0
#define PONG_PADDLE_OFF_RZ		0.0
#define PONG_PADDLE_PAD_Y 		0.05 // Y Padding so the Paddles do not move into the border
#define PONG_PADDLE_HEIGHT		0.17		

#define PONG_BALL_MODEL			19177
#define PONG_BALL_OFF_RX		270.0 // Rotation offset for the Ball
#define PONG_BALL_OFF_RY		0.0
#define PONG_BALL_OFF_RZ		0.0

#define PONG_WIDTH				1.03 // Width of the Screen
#define PONG_HEIGHT				0.52 // Height of the Screen

#define PONG_PADDLE_SPEED		0.02 // Base Speed of the Paddles (dependant on Timer Rate)
#define PONG_BALL_SPEED 		0.035 // Base Speed of the Ball (dependant on Timer Rate)

#define PONG_MAT_FONT 			"Fixedsys" // Courier also looks nice
#define PONG_MAT_FONTSIZE		26
#define PONG_MAT_BOLD			0

// ####################################################################################### // Sound IDs

#define PONG_SOUND_PADDLE_A		1054
#define PONG_SOUND_PADDLE_B		1055
#define PONG_SOUND_BOUNCE		6400 
#define PONG_SOUND_SCORE		5201 
#define PONG_SOUND_ROUND		5205 
#define PONG_SOUND_WIN			5203  
#define PONG_SOUND_LOSE			5206 

// ####################################################################################### // Constants

enum 
{
	PONG_STATE_NONE = 0,
	PONG_STATE_LOBBY,
	PONG_STATE_RUNNING,
	PONG_STATE_ROUND_END,
	PONG_STATE_GAME_END
};

// ####################################################################################### // Data

enum E_PONG_GAME
{
	bool:pgUsed,

	// TV

	Float:pgX,
	Float:pgY,
	Float:pgZ,
	Float:pgRX,
	Float:pgRY,
	Float:pgRZ,
	pgInterior,
	pgVirtualWorld,

	pgObjectID,
	pgAreaID,

	// State

	pgState,
	pgTick,
	pgTimerID,

	// Paddles

	pgPaddleID[2],
	Float:pgPaddleY[2],

	// Ball

	pgBallID,

	Float:pgBallX,
	Float:pgBallY,
	Float:pgBallVX,
	Float:pgBallVY,

	// Players

	pgPlayers[2],
	pgScore[2],
	pgWins[2],

	// Game

	pgGameScore,
	pgGameRounds,
	Float:pgGameSpeed
};
new PongGames[MAX_PONG_GAMES][E_PONG_GAME];

enum E_PLAYER_PONG_INFO
{
	ppgID,

	// State before entering

	Float:ppgX,
	Float:ppgY,
	Float:ppgZ,
	Float:ppgA,

	ppgInterior,
	ppgVirtualWorld,

	Float:ppgHealth,
	Float:ppgArmor,
	ppgWeapons[13],
	ppgAmmo[13]
};
new PlayerPongInfo[MAX_PLAYERS][E_PLAYER_PONG_INFO];

// ####################################################################################### // 

new bool:PONG_Initialized = false; // If this script is used in a Filterscript, we want to make it re-load completely on GMX properly. This prevents double-initialization.

// ####################################################################################### // Forward Declarations

forward PONG_SetCamera(playerid, id);
forward PONG_GameTimer(id);
forward Float:GetPongGameSpeed(id);

// ####################################################################################### // Init / Exit

Pong_Init(initplayers)
{
	if(PONG_Initialized) return 0;

	PONG_Initialized = true;

	if(initplayers) for(new i = 0; i < MAX_PLAYERS; i ++) if(IsPlayerConnected(i)) OnPlayerConnect(i);

	return 1;
}

Pong_Exit(exitplayers)
{
	if(!PONG_Initialized) return 0;

	for(new i = 0; i < MAX_PONG_GAMES; i ++) DestroyPongGame(i);

	if(exitplayers) for(new i = 0; i < MAX_PLAYERS; i ++) if(IsPlayerConnected(i)) OnPlayerDisconnect(i, 1);

	PONG_Initialized = false;

	return 1;
}

public OnGameModeInit()
{
	Pong_Init(0);

	#if defined PONG_OnGameModeInit
	PONG_OnGameModeInit();
	#endif

	return 1;
}

public OnGameModeExit()
{
	Pong_Exit(0);

	#if defined PONG_OnGameModeExit
	PONG_OnGameModeExit();
	#endif

	return 1;
}

public OnFilterScriptInit()
{
	Pong_Init(1);

	#if defined PONG_OnFilterScriptInit
	PONG_OnFilterScriptInit();
	#endif

	return 1;
}

public OnFilterScriptExit()
{
	Pong_Exit(1);

	#if defined PONG_OnFilterScriptExit
	PONG_OnFilterScriptExit();
	#endif

	return 1;
}

public OnPlayerConnect(playerid)
{
	if(!PONG_Initialized) return 1;

	PlayerPongInfo[playerid][ppgID] = -1;

	#if defined PONG_OnPlayerConnect
	PONG_OnPlayerConnect(playerid);
	#endif

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	if(!PONG_Initialized) return 1;

	EndPongGameForPlayer(playerid);

	#if defined PONG_OnPlayerDisconnect
	PONG_OnPlayerDisconnect(playerid, reason);
	#endif

	return 1;
}

// ####################################################################################### // External/API Functions

stock CreatePongGame(Float:x, Float:y, Float:z, Float:rx, Float:rz, interior = 0, virtualworld = 0)
{
	new id = -1;
	for(new i = 0; i < MAX_PONG_GAMES; i ++) if(!PongGames[i][pgUsed]) { id = i; break; }

	if(id == -1) return -1;

	PongGames[id][pgUsed] = true;
	PongGames[id][pgState] = PONG_STATE_NONE;

	PongGames[id][pgPlayers][0] = -1;
	PongGames[id][pgPlayers][1] = -1;

	PongGames[id][pgX] = x;
	PongGames[id][pgY] = y;
	PongGames[id][pgZ] = z;
	PongGames[id][pgRX] = rx;
	//PongGames[id][pgRY] = ry;
	PongGames[id][pgRZ] = rz;
	PongGames[id][pgInterior] = interior;
	PongGames[id][pgVirtualWorld] = virtualworld;

	PongGames[id][pgTimerID] = -1;

	PongGames[id][pgObjectID] = CreateDynamicObject(PONG_MODEL, x, y, z, rx, 0.0, rz, virtualworld, interior, -1, 150.0, 150.0);

	PongGames[id][pgAreaID] = CreateDynamicSphere(x, y, z, 2.5, virtualworld, interior, -1);
	Streamer_SetIntData(STREAMER_TYPE_AREA, PongGames[id][pgAreaID], E_STREAMER_EXTRA_ID, id);

	PONG_UpdateScreen(id);

	// Test

	return id;
}

stock DestroyPongGame(id)
{
	if(!IsValidPongGame(id)) return 0;

	EndPongGame(id);

	DestroyDynamicObject(PongGames[id][pgObjectID]);
	DestroyDynamicArea(PongGames[id][pgAreaID]);

	PongGames[id][pgUsed] = false;

	return 1;
}

stock IsValidPongGame(id)
{
	if(id < 0 || id >= MAX_PONG_GAMES) return 0;

	return PongGames[id][pgUsed];
}

stock HostPongGame(id, playerid_a, playerid_b = -1)
{
	if(!IsValidPongGame(id) || PongGames[id][pgState] != PONG_STATE_NONE) return 0;

	if(IsPlayerConnected(playerid_a) && (PlayerPongInfo[playerid_a][ppgID] != -1 || !PONG_IsAlive(playerid_a))) return 0;

	if(IsPlayerConnected(playerid_b) && (PlayerPongInfo[playerid_b][ppgID] != -1 || !PONG_IsAlive(playerid_b))) return 0;

	PongGames[id][pgPlayers][0] = -1;
	PongGames[id][pgPlayers][1] = -1;

	PongGames[id][pgGameScore] = 5;
	PongGames[id][pgGameRounds] = 3;
	PongGames[id][pgGameSpeed] = 1.0;

	PongGames[id][pgTimerID] = SetTimerEx("PONG_GameTimer", PONG_TIMER_INTERVAL, 1, "i", id);

	PongGames[id][pgState] = PONG_STATE_LOBBY;

	PutPlayerInPongGame(playerid_a, id);
	if(playerid_b != -1) PutPlayerInPongGame(playerid_b, id);

	PONG_UpdateScreen(id);

	return 1;
}

stock EndPongGame(id, bool:finished = false)
{
	if(PongGames[id][pgState] == PONG_STATE_NONE) return 0;

	switch(PongGames[id][pgState])
	{
		case PONG_STATE_LOBBY:
		{
			for(new i = 0; i < 2; i ++) if(PongGames[id][pgPlayers][i] != -1) EndPongGameForPlayer(PongGames[id][pgPlayers][i]);
		}
		case PONG_STATE_RUNNING, PONG_STATE_ROUND_END, PONG_STATE_GAME_END:
		{
			for(new i = 0; i < 2; i ++)
			{
				if(PongGames[id][pgPlayers][i] != -1) EndPongGameForPlayer(PongGames[id][pgPlayers][i], finished);

				if(PongGames[id][pgPaddleID][i] != -1) DestroyDynamicObject(PongGames[id][pgPaddleID][i]);
				PongGames[id][pgPaddleID][i] = -1;
			}

			if(PongGames[id][pgBallID] != -1) DestroyDynamicObject(PongGames[id][pgBallID]);
			PongGames[id][pgBallID] = -1;
		}
	}

	if(PongGames[id][pgTimerID] != -1) KillTimer(PongGames[id][pgTimerID]);
	PongGames[id][pgTimerID] = -1;

	PongGames[id][pgState] = PONG_STATE_NONE;

	PONG_UpdateScreen(id);

	return 1;
}

stock PutPlayerInPongGame(playerid, id)
{
	if(!IsPlayerConnected(playerid) || !PONG_IsAlive(playerid) || !IsValidPongGame(id) || PongGames[id][pgState] != PONG_STATE_LOBBY/* || PlayerPongInfo[playerid][ppgID] != -1*/) return 0;

	new playerslot;

	if(PongGames[id][pgPlayers][0] == -1)
	{
		PongGames[id][pgPlayers][0] = playerid;
		playerslot = 0;
	}
	else if(PongGames[id][pgPlayers][1] == -1)
	{
		PongGames[id][pgPlayers][1] = playerid;
		playerslot = 1;
	}
	else return 0;

	PlayerPongInfo[playerid][ppgID] = id;
	
	GetPlayerPos(playerid, PlayerPongInfo[playerid][ppgX], PlayerPongInfo[playerid][ppgY], PlayerPongInfo[playerid][ppgZ]);
	GetPlayerFacingAngle(playerid, PlayerPongInfo[playerid][ppgA]);
	PlayerPongInfo[playerid][ppgInterior] = GetPlayerInterior(playerid);
	PlayerPongInfo[playerid][ppgVirtualWorld] = GetPlayerVirtualWorld(playerid);
	for(new i = 0; i < 13; i ++)
	{
		GetPlayerWeaponData(playerid, i, PlayerPongInfo[playerid][ppgWeapons][i], PlayerPongInfo[playerid][ppgAmmo][i]);
	}

	TogglePlayerSpectating(playerid, 1);

	SetPlayerInterior(playerid, PongGames[id][pgInterior]);
	SetPlayerVirtualWorld(playerid, PongGames[id][pgVirtualWorld]);

	PONG_SetCamera(playerid, id);
	SetTimerEx("PONG_SetCamera", 100, 0, "ii", playerid, id);

	new text[60];
	format(text, sizeof text, "[PONG] You joined Pong Game %d as Player %c.", id, playerslot ? 'B' : 'A');
	SendClientMessage(playerid, -1, text);

	Streamer_ToggleIdleUpdate(playerid, 1); // Important

	PONG_UpdateScreen(id);

	return 1;
}

stock EndPongGameForPlayer(playerid, bool:finished = false)
{
	new id = PlayerPongInfo[playerid][ppgID];

	if(!IsValidPongGame(id)) return 0;

	for(new i = 0; i < 2; i ++) if(PongGames[id][pgPlayers][i] == playerid) PongGames[id][pgPlayers][i] = -1;

	if(PongGames[id][pgPlayers][0] == -1 && PongGames[id][pgPlayers][1] != -1)
	{
		PongGames[id][pgPlayers][0] = PongGames[id][pgPlayers][1];
		PongGames[id][pgPlayers][1] = -1;

		SendClientMessage(PongGames[id][pgPlayers][0], -1, "[PONG] You are now the Host of this Pong Game.");
	}

	switch(PongGames[id][pgState])
	{
		case PONG_STATE_LOBBY:
		{
			PONG_UpdateScreen(id);
		}
		case PONG_STATE_RUNNING, PONG_STATE_ROUND_END, PONG_STATE_GAME_END:
		{
			EndPongGame(id);
		}
	}

	TogglePlayerSpectating(playerid, 0);

	SetPlayerPos(playerid, PlayerPongInfo[playerid][ppgX], PlayerPongInfo[playerid][ppgY], PlayerPongInfo[playerid][ppgZ]);
	SetPlayerFacingAngle(playerid, PlayerPongInfo[playerid][ppgA]);

	SetCameraBehindPlayer(playerid);

	SetPlayerInterior(playerid, PlayerPongInfo[playerid][ppgInterior]);
	SetPlayerVirtualWorld(playerid, PlayerPongInfo[playerid][ppgVirtualWorld]);

	Streamer_UpdateEx(playerid, PlayerPongInfo[playerid][ppgX], PlayerPongInfo[playerid][ppgY], PlayerPongInfo[playerid][ppgZ], PlayerPongInfo[playerid][ppgVirtualWorld], PlayerPongInfo[playerid][ppgInterior], -1, 700, 1);

	for(new i = 0; i < 13; i ++) if(PlayerPongInfo[playerid][ppgWeapons][i] != 0) GivePlayerWeapon(playerid, PlayerPongInfo[playerid][ppgWeapons][i], PlayerPongInfo[playerid][ppgAmmo][i]);

	PlayerPongInfo[playerid][ppgID] = -1;

	if(!finished)
	{
		SendClientMessage(playerid, -1, "[PONG] Game Over. The Pong Game has been terminated.");
	}

	return 1;
}

stock StartPongGame(id)
{
	if(!IsValidPongGame(id) || PongGames[id][pgState] != PONG_STATE_LOBBY || PongGames[id][pgPlayers][0] == -1 || PongGames[id][pgPlayers][1] == -1) return 0;

	for(new i = 0; i < 2; i ++)
	{
		PongGames[id][pgScore][i] = 0;
		PongGames[id][pgWins][i] = 0;
		PongGames[id][pgPaddleID][i] = -1;
		PongGames[id][pgPaddleY] = 0.0;
	}

	PongGames[id][pgBallID] = -1;
	PongGames[id][pgBallX] = 0.0;
	PongGames[id][pgBallY] = 0.0;
	PONG_RandomizeBallVector(id);

	PongGames[id][pgState] = PONG_STATE_RUNNING;

	PONG_UpdateScreen(id);

	return 1;
}


stock GetPlayerPongArea(playerid)
{
	if(!IsPlayerConnected(playerid)) return -1;

	new dynareas[15] = {-1, ...}, id;
	GetPlayerDynamicAreas(playerid, dynareas, sizeof(dynareas));

	for(new i = 0; i < sizeof(dynareas); i ++) if(IsValidDynamicArea(dynareas[i]))
	{
		id = Streamer_GetIntData(STREAMER_TYPE_AREA, dynareas[i], E_STREAMER_EXTRA_ID);

		if(!IsValidPongGame(id) || PongGames[id][pgAreaID] != dynareas[i]) continue;

		return id;
	}

	return -1;
}


stock SetPongGameScore(id, score)
{
	if(!IsValidPongGame(id)) return 0;

	PongGames[id][pgGameScore] = score;

	if(PongGames[id][pgState] == PONG_STATE_LOBBY) PONG_UpdateScreen(id);

	return 1;
}

stock GetPongGameScore(id)
{
	if(!IsValidPongGame(id)) return -1;

	return PongGames[id][pgGameScore];
}


stock SetPongGameRounds(id, rounds)
{
	if(!IsValidPongGame(id)) return 0;

	PongGames[id][pgGameRounds] = rounds;

	if(PongGames[id][pgState] == PONG_STATE_LOBBY) PONG_UpdateScreen(id);

	return 1;
}

stock GetPongGameRounds(id)
{
	if(!IsValidPongGame(id)) return -1;

	return PongGames[id][pgGameRounds];
}


stock SetPongGameSpeed(id, Float:speed)
{
	if(!IsValidPongGame(id)) return 0;

	PongGames[id][pgGameSpeed] = speed;

	if(PongGames[id][pgState] == PONG_STATE_LOBBY) PONG_UpdateScreen(id);

	return 1;
}

stock Float:GetPongGameSpeed(id)
{
	if(!IsValidPongGame(id)) return 0.0;

	return PongGames[id][pgGameSpeed];
}


stock GetPongGameState(id)
{
	if(!IsValidPongGame(id)) return -1;

	return PongGames[id][pgState];
}


stock GetPongGameInterior(id)
{
	if(!IsValidPongGame(id)) return -1;

	return PongGames[id][pgInterior];
}


stock GetPongGameVirtualWorld(id)
{
	if(!IsValidPongGame(id)) return -1;

	return PongGames[id][pgVirtualWorld];
}


stock GetPongPlayers(id, playerid_a = 0, playerid_b = 0)
{
	playerid_a = -1;
	playerid_b = -1;

	if(!IsValidPongGame(id) || GetPongGameState(id) == PONG_STATE_NONE) return -1;

	new num;

	playerid_a = PongGames[id][pgPlayers][0];
	playerid_b = PongGames[id][pgPlayers][1];

	if(playerid_a != -1) num ++;
	if(playerid_b != -1) num ++;

	return num;
}


stock GetPlayerPongID(playerid)
{
	if(!IsPlayerConnected(playerid)) return -1;
	
	return PlayerPongInfo[playerid][ppgID];
}

// ####################################################################################### // Main Game Timer (Controls mainly)

public PONG_GameTimer(id)
{
	new tick = gettime(), pongstate = PongGames[id][pgState];

	if(pongstate != PONG_STATE_NONE && pongstate != PONG_STATE_LOBBY)
	{
		new playerid; 
		for(new i = 0; i < 2; i ++)
		{
			playerid = PongGames[id][pgPlayers][i];

			if(!IsPlayerConnected(playerid) || PlayerPongInfo[playerid][ppgID] != id)
			{
				EndPongGame(id, false);

				return;
			}
		}
	}

	switch(pongstate)
	{
		case PONG_STATE_RUNNING:
		{
			for(new i = 0; i < 2; i ++)
			{
				PONG_ProcessPaddle(id, i);
				PONG_UpdatePaddlePos(id, i);
			}

			if(PONG_ProcessBall(id)) return;
			PONG_UpdateBallPos(id);
		}
		case PONG_STATE_ROUND_END:
		{
			if(tick - PongGames[id][pgTick] > 0) PONG_Continue(id);
		}
		case PONG_STATE_GAME_END:
		{
			if(tick - PongGames[id][pgTick] > 0) EndPongGame(id, true);
		}
	}
}

// ####################################################################################### // Internal Game Functions

public PONG_SetCamera(playerid, id)
{
	if(!IsValidPongGame(id) || !IsPlayerConnected(playerid) || PlayerPongInfo[playerid][ppgID] != id) return;

	new Float:x, Float:y, Float:z;
	PONG_GetPointInFront3D(PongGames[id][pgX], PongGames[id][pgY], PongGames[id][pgZ], -PongGames[id][pgRX], PongGames[id][pgRZ] + 180.0, PONG_CAMERA_DISTANCE, x, y, z);

	SetPlayerCameraPos(playerid, x, y, z - 0.2);
	SetPlayerCameraLookAt(playerid, PongGames[id][pgX], PongGames[id][pgY], PongGames[id][pgZ]);
}

static stock PONG_UpdateScreen(id)
{
	if(!IsValidPongGame(id)) return 0;

	switch(PongGames[id][pgState])
	{
		case PONG_STATE_NONE:
		{
			PONG_SetObjectText(id, "{009933}Pong!{FFFFFF}\n\nPress the any key", PONG_MAT_FONTSIZE + 12);
		}
		case PONG_STATE_LOBBY:
		{
			new text[(MAX_PLAYER_NAME + 1) * 2 + 70], name_a[MAX_PLAYER_NAME + 1], name_b[MAX_PLAYER_NAME + 1];

			if(PongGames[id][pgPlayers][0] != -1) GetPlayerName(PongGames[id][pgPlayers][0], name_a, sizeof name_a);
			else strcat(name_a, "???");

			if(PongGames[id][pgPlayers][1] != -1) GetPlayerName(PongGames[id][pgPlayers][1], name_b, sizeof name_b);
			else strcat(name_b, "???");

			format(text, sizeof(text), "{009933}Pong!{FFFFFF}\n\n%s  vs.  %s\n\nScore: %d\nRounds: %d\nGame Speed: %.1f", name_a, name_b, PongGames[id][pgGameScore], PongGames[id][pgGameRounds], PongGames[id][pgGameSpeed]);
			PONG_SetObjectText(id, text, PONG_MAT_FONTSIZE + 2);
		}
		case PONG_STATE_RUNNING:
		{
			new text[(MAX_PLAYER_NAME + 1) * 2 + 95], name_a[MAX_PLAYER_NAME + 1], name_b[MAX_PLAYER_NAME + 1];

			GetPlayerName(PongGames[id][pgPlayers][0], name_a, sizeof name_a);
			GetPlayerName(PongGames[id][pgPlayers][1], name_b, sizeof name_b);

			format(text, sizeof(text), "{999999}|\n|\n|\n|\n\n{FFBB99}%s    VS    %s\n\n{AAAAAA}%d    score    %d\n\n{666666}%d    rounds    %d\n\n|\n|\n|\n|", name_a, name_b, PongGames[id][pgScore][0], PongGames[id][pgScore][1], PongGames[id][pgWins][0], PongGames[id][pgWins][1]);
			PONG_SetObjectText(id, text);
		}
	}

	return 1;
}

static stock PONG_SetObjectText(id, text[], fontsize = PONG_MAT_FONTSIZE)
{
	if(!IsValidPongGame(id)) return 0;

	SetDynamicObjectMaterialText(PongGames[id][pgObjectID], 1, text, OBJECT_MATERIAL_SIZE_512x256, PONG_MAT_FONT, fontsize, PONG_MAT_BOLD, 0xFFFFFFFF, 0xFF000000, 1);

	return 1;
}

static PONG_ProcessPaddle(id, playerslot)
{
	new playerid = PongGames[id][pgPlayers][playerslot], keys, ud, lr;
	GetPlayerKeys(playerid, keys, ud, lr);

	if(PongGames[id][pgPlayers][0] != PongGames[id][pgPlayers][1] || playerslot)
	{
		if(ud > 0)
		{
			PongGames[id][pgPaddleY][playerslot] -= PONG_PADDLE_SPEED;
			if(PongGames[id][pgPaddleY][playerslot] < -PONG_HEIGHT + PONG_PADDLE_PAD_Y) PongGames[id][pgPaddleY][playerslot] = -PONG_HEIGHT + PONG_PADDLE_PAD_Y;
		}
		else if(ud < 0)
		{
			PongGames[id][pgPaddleY][playerslot] += PONG_PADDLE_SPEED;
			if(PongGames[id][pgPaddleY][playerslot] > PONG_HEIGHT - PONG_PADDLE_PAD_Y) PongGames[id][pgPaddleY][playerslot] = PONG_HEIGHT - PONG_PADDLE_PAD_Y;
		}
	}
	else
	{
		if(ud < 0)
		{
			PongGames[id][pgPaddleY][playerslot] -= PONG_PADDLE_SPEED;
			if(PongGames[id][pgPaddleY][playerslot] < -PONG_HEIGHT + PONG_PADDLE_PAD_Y) PongGames[id][pgPaddleY][playerslot] = -PONG_HEIGHT + PONG_PADDLE_PAD_Y;
		}
		else if(ud > 0)
		{
			PongGames[id][pgPaddleY][playerslot] += PONG_PADDLE_SPEED;
			if(PongGames[id][pgPaddleY][playerslot] > PONG_HEIGHT - PONG_PADDLE_PAD_Y) PongGames[id][pgPaddleY][playerslot] = PONG_HEIGHT - PONG_PADDLE_PAD_Y;
		}
	}
}

static PONG_UpdatePaddlePos(id, playerslot)
{
	new Float:x = PongGames[id][pgX], Float:y = PongGames[id][pgY], Float:z = PongGames[id][pgZ], Float:rx = PongGames[id][pgRX], Float:ry = PongGames[id][pgRY], Float:rz = PongGames[id][pgRZ];

	PONG_GetPointInFront3D(x, y, z, rx + 180.0, rz, 0.1, x, y, z); // Front
	PONG_GetPointInFront3D(x, y, z, rx + 90.0, rz, PongGames[id][pgPaddleY][playerslot], x, y, z); // Up

	x += ((playerslot == 0 ? -PONG_WIDTH - 0.03 : PONG_WIDTH + 0.03) + 0.015) * floatsin(-rz + 90.0, degrees);
	y += ((playerslot == 0 ? -PONG_WIDTH - 0.03 : PONG_WIDTH + 0.03) + 0.015) * floatcos(-rz + 90.0, degrees);

	if(PongGames[id][pgPaddleID][playerslot] == -1)
	{
		PongGames[id][pgPaddleID][playerslot] = CreateDynamicObject(PONG_PADDLE_MODEL, x, y, z, rx + PONG_PADDLE_OFF_RX, ry + PONG_PADDLE_OFF_RY, rz + PONG_PADDLE_OFF_RZ, PongGames[id][pgVirtualWorld], PongGames[id][pgInterior], -1, 25.0);
		SetDynamicObjectMaterial(PongGames[id][pgPaddleID][playerslot], 0, 10765, "airportgnd_sfse", "white", 0);
	}
	else
	{
		SetDynamicObjectPos(PongGames[id][pgPaddleID][playerslot], x, y, z);
	}
}

static PONG_ProcessBall(id)
{
	PongGames[id][pgBallY] += PongGames[id][pgBallVY] * PongGames[id][pgGameSpeed];

	if(PongGames[id][pgBallY] < -PONG_HEIGHT)
	{
		PongGames[id][pgBallY] = -PONG_HEIGHT;
		PongGames[id][pgBallVY] *= -1.0;

		PONG_PlaySound(id, PONG_SOUND_BOUNCE);
	}
	else if(PongGames[id][pgBallY] > PONG_HEIGHT)
	{
		PongGames[id][pgBallY] = PONG_HEIGHT;
		PongGames[id][pgBallVY] *= -1.0;

		PONG_PlaySound(id, PONG_SOUND_BOUNCE);
	}

	PongGames[id][pgBallX] += PongGames[id][pgBallVX] * PongGames[id][pgGameSpeed];

	if(PongGames[id][pgBallX] < -PONG_WIDTH)
	{
		new Float:dist = floatabs(PongGames[id][pgBallY] - PongGames[id][pgPaddleY][0]);
		
		if(dist > PONG_PADDLE_HEIGHT * 0.5)
		{
			PONG_GiveScore(id, 1);

			return 1;
		}

		PongGames[id][pgBallX] = -PONG_WIDTH + 0.01;
		PongGames[id][pgBallVX] *= -1.0;

		PONG_PlaySound(id, PONG_SOUND_PADDLE_A);
	}
	else if(PongGames[id][pgBallX] > PONG_WIDTH)
	{
		new Float:dist = floatabs(PongGames[id][pgBallY] - PongGames[id][pgPaddleY][1]);
		
		if(dist > PONG_PADDLE_HEIGHT * 0.5)
		{
			PONG_GiveScore(id, 0);

			return 1;
		}

		PongGames[id][pgBallX] = PONG_WIDTH - 0.01;
		PongGames[id][pgBallVX] *= -1.0;

		PONG_PlaySound(id, PONG_SOUND_PADDLE_B);
	}

	return 0;
}

static PONG_UpdateBallPos(id)
{
	new Float:x = PongGames[id][pgX], Float:y = PongGames[id][pgY], Float:z = PongGames[id][pgZ], Float:rx = PongGames[id][pgRX], Float:ry = PongGames[id][pgRY], Float:rz = PongGames[id][pgRZ];

	PONG_GetPointInFront3D(x, y, z, rx + 180.0, rz, 0.128, x, y, z); // Front
	PONG_GetPointInFront3D(x, y, z, rx + 90.0, rz, PongGames[id][pgBallY], x, y, z); // Up

	x += PongGames[id][pgBallX] * floatsin(-rz + 90.0, degrees);
	y += PongGames[id][pgBallX] * floatcos(-rz + 90.0, degrees);

	if(PongGames[id][pgBallID] == -1)
	{
		PongGames[id][pgBallID] = CreateDynamicObject(PONG_BALL_MODEL, x, y, z, rx + PONG_BALL_OFF_RX, ry + PONG_BALL_OFF_RY, rz + PONG_BALL_OFF_RZ, PongGames[id][pgVirtualWorld], PongGames[id][pgInterior], -1, 25.0);
	}
	else
	{
		SetDynamicObjectPos(PongGames[id][pgBallID], x, y, z);
	}
}

static PONG_GiveScore(id, playerslot)
{
	if(PongGames[id][pgState] != PONG_STATE_RUNNING) return 0;

	if(PongGames[id][pgBallID] != -1)
	{
		DestroyDynamicObject(PongGames[id][pgBallID]);
		PongGames[id][pgBallID] = -1;
	}

	for(new i = 0; i < 2; i ++) if(PongGames[id][pgPaddleID][i] != -1)
	{
		DestroyDynamicObject(PongGames[id][pgPaddleID][i]);
		PongGames[id][pgPaddleID][i] = -1;
	}

	PongGames[id][pgScore][playerslot] ++;

	if(PongGames[id][pgScore][playerslot] == PongGames[id][pgGameScore])
	{
		return PONG_GiveRound(id, playerslot);
	}

	PONG_PlaySound(id, PONG_SOUND_SCORE);

	PongGames[id][pgState] = PONG_STATE_ROUND_END;
	PongGames[id][pgTick] = gettime() + 2;

	new text[80];
	GetPlayerName(PongGames[id][pgPlayers][playerslot], text, sizeof(text));

	format(text, sizeof(text), "{009933}%s scored a Point", text);
	PONG_SetObjectText(id, text);

	return 1;
}

static PONG_GiveRound(id, playerslot)
{
	if(PongGames[id][pgState] != PONG_STATE_RUNNING) return 0;

	PongGames[id][pgWins][playerslot] ++;
	PongGames[id][pgScore][0] = 0;
	PongGames[id][pgScore][1] = 0;

	if(PongGames[id][pgWins][playerslot] == PongGames[id][pgGameRounds])
	{
		return PONG_GiveGame(id, playerslot);
	}

	PONG_PlaySound(id, PONG_SOUND_ROUND);

	PongGames[id][pgState] = PONG_STATE_ROUND_END;
	PongGames[id][pgTick] = gettime() + 4;

	new text[80];
	GetPlayerName(PongGames[id][pgPlayers][playerslot], text, sizeof(text));

	format(text, sizeof(text), "{00E6FF}%s won the Round", text);
	PONG_SetObjectText(id, text);

	return 2;
}

static PONG_GiveGame(id, playerslot)
{
	if(PongGames[id][pgState] != PONG_STATE_RUNNING) return 0;

	PONG_PlaySound(id, PONG_SOUND_WIN, playerslot);
	PONG_PlaySound(id, PONG_SOUND_LOSE, playerslot ? 0 : 1);

	PongGames[id][pgState] = PONG_STATE_GAME_END;
	PongGames[id][pgTick] = gettime() + 7;

	new text[110], name[MAX_PLAYER_NAME + 1];
	GetPlayerName(PongGames[id][pgPlayers][playerslot], text, sizeof(text));
	GetPlayerName(PongGames[id][pgPlayers][playerslot == 0 ? 1 : 0], name, sizeof(name));

	format(text, sizeof(text), "{63AFF0}%s won the Game\n\n{FF9900}%s lost", text, name);
	PONG_SetObjectText(id, text);

	return 3;
}

static PONG_Continue(id)
{
	if(PongGames[id][pgState] != PONG_STATE_ROUND_END) return 0;

	for(new i = 0; i < 2; i ++)
	{
		PongGames[id][pgPaddleY][i] = 0.0;

		PONG_UpdatePaddlePos(id, i);
	}

	PongGames[id][pgBallX] = 0.0;
	PongGames[id][pgBallY] = 0.0;
	PONG_RandomizeBallVector(id);
	PONG_UpdateBallPos(id);

	PongGames[id][pgState] = PONG_STATE_RUNNING;

	PONG_UpdateScreen(id);

	return 1;
}

static stock PONG_RandomizeBallVector(id)
{
	new Float:x, Float:y, Float:len;

	do
	{
		x = float(random(101)); // -100.0 to 100.0
		y = float(random(101));
	}
	while (x < 50.0 || y < 30.0);

	if(random(2)) x = -x;
	if(random(2)) y = -y;

	len = floatsqroot(x * x + y * y);

	PongGames[id][pgBallVX] = (x / len) * PONG_BALL_SPEED;
	PongGames[id][pgBallVY] = (y / len) * PONG_BALL_SPEED;

	return 1;
}

static stock PONG_PlaySound(id, soundid, playerslot = -1)
{
	if(playerslot != -1)
	{
		if(playerslot == 0 || playerslot == 1)
		{
			PlayerPlaySound(PongGames[id][pgPlayers][playerslot], soundid, 0.0, 0.0, 0.0);
		}
	}
	else
	{
		for(new i = 0; i < MAX_PLAYERS; i ++)
		{
			if(IsPlayerConnected(i) && (PlayerPongInfo[i][ppgID] == id || IsPlayerInDynamicArea(i, PongGames[id][pgAreaID])))
			{
				PlayerPlaySound(PongGames[id][pgPlayers][i], soundid, 0.0, 0.0, 0.0);
			}
		}
	}
}

// ####################################################################################### // Misc Functions

static stock PONG_GetPointInFront3D(Float:x, Float:y, Float:z, Float:rx, Float:rz, Float:distance, &Float:tx, &Float:ty, &Float:tz)
{
	tx = x - (distance * floatcos(rx,degrees) * floatsin(rz,degrees));
	ty = y + (distance * floatcos(rx,degrees) * floatcos(rz,degrees));
	tz = z + (distance * floatsin(rx,degrees));

	return 1;
}

static stock PONG_IsAlive(playerid)
{
	switch(GetPlayerState(playerid))
	{
		case PLAYER_STATE_ONFOOT, PLAYER_STATE_DRIVER, PLAYER_STATE_PASSENGER:
		{
			new Float:h;
			GetPlayerHealth(playerid, h);

			if(h > 0.0) return 1;
		}
	}

	return 0;
}

// ####################################################################################### // Callback Hooks

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit PONG_OnFilterScriptInit
#if defined PONG_OnFilterScriptInit
	forward PONG_OnFilterScriptInit();
#endif

#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit PONG_OnFilterScriptExit
#if defined PONG_OnFilterScriptExit
	forward PONG_OnFilterScriptExit();
#endif


#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit PONG_OnGameModeInit
#if defined PONG_OnGameModeInit
	forward PONG_OnGameModeInit();
#endif

#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
#define OnGameModeExit PONG_OnGameModeExit
#if defined PONG_OnGameModeExit
	forward PONG_OnGameModeExit();
#endif


#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect PONG_OnPlayerConnect
#if defined PONG_OnPlayerConnect
	forward PONG_OnPlayerConnect(playerid);
#endif

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect PONG_OnPlayerDisconnect
#if defined PONG_OnPlayerDisconnect
	forward PONG_OnPlayerDisconnect(playerid, reason);
#endif

// ####################################################################################### // EOF
