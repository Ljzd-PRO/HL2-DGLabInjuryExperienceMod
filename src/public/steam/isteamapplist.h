//====== Copyright Â© 1996-2008, Valve Corporation, All rights reserved. =======
//
// Purpose: interface to app data in Steam
//
//=============================================================================

#ifndef ISTEAMAPPLIST_H
#define ISTEAMAPPLIST_H
#ifdef _WIN32
#pragma once
#endif

#include "isteamclient.h"
#include "steamtypes.h"

//-----------------------------------------------------------------------------
// Purpose: This is a restricted interface that can only be used by previously approved apps,
//	contact your Steam Account Manager if you believe you need access to this API.
//	This interface lets you detect installed apps for the local Steam client, useful for debugging tools
//	to offer lists of apps to debug via Steam.
//-----------------------------------------------------------------------------
class ISteamAppList
{
public:
	virtual uint32 GetNumInstalledApps() = 0;
	virtual uint32 GetInstalledApps( AppId_t *pvecAppID, uint32 unMaxAppIDs ) = 0;

	virtual int  GetAppName( AppId_t nAppID, char *pchName, int cchNameMax ) = 0; // returns -1 if no name was found
	virtual int  GetAppInstallDir( AppId_t nAppID, char *pchDirectory, int cchNameMax ) = 0; // returns -1 if no dir was found

	virtual int GetAppBuildId( AppId_t nAppID ) = 0; // return the buildid of this app, may change at any time based on backend updates to the game
};

#define STEAMAPPLIST_INTERFACE_VERSION "STEAMAPPLIST_INTERFACE_VERSION001"

// callbacks
#if defined( VALVE_CALLBACK_PACK_SMALL )
#pragma pack( push, 4 )
#elif defined( VALVE_CALLBACK_PACK_LARGE )
#pragma pack( push, 8 )
#else
#error isteamclient.h must be included
#endif 


//---------------------------------------------------------------------------------
// Purpose: Sent when a new app is installed
//---------------------------------------------------------------------------------
DEFINE_CALLBACK( SteamAppInstalled_t, k_iSteamAppListCallbacks + 1 );
	CALLBACK_MEMBER( 0,	AppId_t,	m_nAppID )			// ID of the app that installs
END_DEFINE_CALLBACK_1()


//---------------------------------------------------------------------------------
// Purpose: Sent when an app is uninstalled
//---------------------------------------------------------------------------------
DEFINE_CALLBACK( SteamAppUninstalled_t, k_iSteamAppListCallbacks + 2 );
	CALLBACK_MEMBER( 0,	AppId_t,	m_nAppID )			// ID of the app that installs
END_DEFINE_CALLBACK_1()


#pragma pack( pop )
#endif // ISTEAMAPPLIST_H