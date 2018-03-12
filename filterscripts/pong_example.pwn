/*
	Example Filterscript for the Pong Include

	Type /pong_info to see the Commands. Admin Commands require RCON Admin.

	Creates 2 Pong Games, one in CJ's House and one inside San Fierro Cranberry Station.

	/pong_create lets you create more TVs, note that any rotation on the Y Axis will be ignored.
*/

#include <a_samp>
#include <streamer>
#include <pong>
#include <zcmd>

// #######################################################################################

new TMPPlayerObjectID[MAX_PLAYERS];

// #######################################################################################

// ------------------------------- Callbacks

public OnFilterScriptInit()
{
	print("\n\tPong Game Example FS loaded.\n");

	CreatePongGame(2490.420166, -1697.212280, 1015.730041, 0.0, 90.0, 3, 0); // CJ's House

	CreatePongGame(-1979.120849, 143.456390, 29.197498, 0.000000, 90.047554, 0, 0); // Cranberry Station

	return 1;
}

public OnFilterScriptExit()
{
	print("\n\tPong Game Example FS unloaded.\n");

	return 1;
}

public OnPlayerConnect(playerid)
{
	TMPPlayerObjectID[playerid] = -1;

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

public OnPlayerEditObject(playerid, playerobject, objectid, response, Float:fX, Float:fY, Float:fZ, Float:fRotX, Float:fRotY, Float:fRotZ)
{
	if(playerobject && TMPPlayerObjectID[playerid] == objectid)
	{
		if(!IsPlayerAdmin(playerid))
		{
			DestroyPlayerObject(playerid, objectid);
			TMPPlayerObjectID[playerid] = -1;

			return 1;
		}

		switch(response)
		{
			case EDIT_RESPONSE_FINAL:
			{
				DestroyPlayerObject(playerid, objectid);
				TMPPlayerObjectID[playerid] = -1;

				new int = GetPlayerInterior(playerid), vw = GetPlayerVirtualWorld(playerid);

				if(CreatePongGame(fX, fY, fZ, fRotX, fRotZ, int, vw) == -1) SendClientMessage(playerid, 0xFF0000FF, "Failed to create Pong Game.");
				else SendClientMessage(playerid, 0x00FF00FF, "Successfully created Pong Game.");

				printf("\nCreatePongGame(%f, %f, %f, %f, %f, %d, %d);\n", fX, fY, fZ, fRotX, fRotZ, int, vw);
			}
			case EDIT_RESPONSE_CANCEL:
			{
				DestroyPlayerObject(playerid, objectid);
				TMPPlayerObjectID[playerid] = -1;

				SendClientMessage(playerid, -1, "Cancelled Pong Game Creation.");
			}
		}
	}	

	return 1;
}

// ------------------------------- Commands

CMD:pong_exit(playerid, params[]) // Debug CMD for exiting a Pong Game without stopping it
{
	if(!IsPlayerAdmin(playerid)) return 0; 

	TogglePlayerSpectating(playerid, 0);

	SetPlayerPos(playerid, PlayerPongInfo[playerid][ppgX], PlayerPongInfo[playerid][ppgY], PlayerPongInfo[playerid][ppgZ]);
	SetPlayerFacingAngle(playerid, PlayerPongInfo[playerid][ppgA]);

	SetCameraBehindPlayer(playerid);

	SetPlayerInterior(playerid, PlayerPongInfo[playerid][ppgInterior]);
	SetPlayerVirtualWorld(playerid, PlayerPongInfo[playerid][ppgVirtualWorld]);

	return 1;
}

CMD:pong_host(playerid, params[])
{
	new id = GetPlayerPongArea(playerid);

	if(id == -1 || !HostPongGame(id, playerid)) SendClientMessage(playerid, -1, "There's no Pong Game nearby.");

	return 1;
}

CMD:pong_hostlocal(playerid, params[])
{
	new id = GetPlayerPongArea(playerid);

	if(id == -1 || !HostPongGame(id, playerid, playerid)) SendClientMessage(playerid, -1, "There's no Pong Game nearby.");

	return 1;
}

CMD:pong_join(playerid, params[])
{
	new id = GetPlayerPongArea(playerid);

	if(id == -1 || !PutPlayerInPongGame(playerid, id)) SendClientMessage(playerid, -1, "There's no Pong Game nearby.");

	return 1;
}

CMD:pong_start(playerid, params[])
{
	new id = GetPlayerPongID(playerid);

	if(!IsValidPongGame(id)) return SendClientMessage(playerid, -1, "You are in no Pong Game.");

	StartPongGame(id);

	return 1;
}

CMD:pong_end(playerid, params[])
{
	if(!IsPlayerAdmin(playerid)) return 1;

	new id = GetPlayerPongID(playerid);

	if(!IsValidPongGame(id)) id = GetPlayerPongArea(playerid);

	if(id == -1) return SendClientMessage(playerid, -1, "There's no Pong Game nearby.");

	EndPongGame(id);

	return 1;
}

CMD:pong_score(playerid, params[])
{
	new id = GetPlayerPongID(playerid), playerid_a;

	GetPongPlayers(id, playerid_a);

	if(!IsValidPongGame(id) || playerid_a != playerid) return SendClientMessage(playerid, -1, "You are not the Host of a Pong Game.");

	if(isnull(params) || !strlen(params) || strlen(params) > 5) return SendClientMessage(playerid, -1, "Invalid Value.");

	new val = strval(params);

	if(val < 1 || val > 20) return SendClientMessage(playerid, -1, "Invalid Value.");

	SetPongGameScore(id, val);

	SendClientMessage(playerid, -1, "[PONG] Score updated.");

	return 1;
}

CMD:pong_rounds(playerid, params[])
{
	new id = GetPlayerPongID(playerid), playerid_a;

	GetPongPlayers(id, playerid_a);

	if(!IsValidPongGame(id) || playerid_a != playerid) return SendClientMessage(playerid, -1, "You are not the Host of a Pong Game.");

	if(isnull(params) || !strlen(params) || strlen(params) > 5) return SendClientMessage(playerid, -1, "Invalid Value.");

	new val = strval(params);

	if(val < 1 || val > 20) return SendClientMessage(playerid, -1, "Invalid Value.");

	SetPongGameRounds(id, val);

	SendClientMessage(playerid, -1, "[PONG] Num. Rounds updated.");

	return 1;
}

CMD:pong_speed(playerid, params[])
{
	new id = GetPlayerPongID(playerid), playerid_a;

	GetPongPlayers(id, playerid_a);
	
	if(!IsValidPongGame(id) || playerid_a != playerid) return SendClientMessage(playerid, -1, "You are not the Host of a Pong Game.");

	if(isnull(params) || !strlen(params) || strlen(params) > 5) return SendClientMessage(playerid, -1, "Invalid Value.");

	new Float:val = floatstr(params);

	if(val < 0.5 || val > 5.0) return SendClientMessage(playerid, -1, "Invalid Value.");

	SetPongGameSpeed(id, val);

	SendClientMessage(playerid, -1, "[PONG] Game Speed updated.");

	return 1;
}

CMD:pong_create(playerid, params[])
{
	if(!IsPlayerAdmin(playerid)) return 1;

	if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return 1;

	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	x += 1.7 * floatsin(-a, degrees);
	y += 1.7 * floatcos(-a, degrees);

	TMPPlayerObjectID[playerid] = CreatePlayerObject(playerid, PONG_MODEL, x, y, z, 0.0, 0.0, a, 999.0);

	if(IsValidPlayerObject(playerid, TMPPlayerObjectID[playerid])) EditPlayerObject(playerid, TMPPlayerObjectID[playerid]);
	else TMPPlayerObjectID[playerid] = -1;

	return 1;
}

CMD:pong_destroy(playerid, params[])
{
	if(!IsPlayerAdmin(playerid)) return 1;

	if(isnull(params) || strlen(params) > 5) return SendClientMessage(playerid, -1, "Invalid ID.");

	new id = strval(params);

	if(!IsValidPongGame(id)) return SendClientMessage(playerid, -1, "ID does not exist.");

	DestroyPongGame(id);

	return 1;
}

CMD:pong_info(playerid, params[])
{
	if(!IsPlayerAdmin(playerid)) return 1;

	new id = GetPlayerPongArea(playerid);

	if(id == -1) return SendClientMessage(playerid, -1, "There's no Pong Game nearby.");

	new text[80];
	format(text, sizeof(text), "Pong Game ID: %d, State: %d, Int: %d, VW: %d", id, GetPongGameState(id), GetPongGameInterior(id), GetPongGameVirtualWorld(id));

	SendClientMessage(playerid, 0x99FF00AA, text);

	return 1;
}

CMD:pong_help(playerid, params[])
{
	if(!IsPlayerAdmin(playerid)) return 1;

	SendClientMessage(playerid, 0xCCFFDD56, "[PONG] Commands");
	SendClientMessage(playerid, 0xCCFFDD56, "/pong_host|hostlocal|join|start|leave");
	SendClientMessage(playerid, 0xCCFFDD56, "/pong_score|rounds|speed [value]");

	if(IsPlayerAdmin(playerid))
	{
		SendClientMessage(playerid, 0xAAFFCC88, "[PONG] Admin Commands");
		SendClientMessage(playerid, 0xAAFFCC88, "/pong_create|end|info");
		SendClientMessage(playerid, 0xAAFFCC88, "/pong_destroy [id]");
	}

	return 1;
}

