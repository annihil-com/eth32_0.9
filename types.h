// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

/*
	The main header file we might say, contains all the info that is used while playing.
	When you're adding something in here, make sure it's being added to the right struct!

	Also, please comment the members you're adding so other developers know what's the function of each part.
*/

#pragma once

// stuff that we don't get from sdk
#include "q3_types.h"

// **************************
// Macros
// **************************

#define DETFUNC(func)				orig_##func = (func##_t) DetourFunction((BYTE *) func, (BYTE *) eth32_##func);
#define DETCGAME(func)				if (eth32.cgMod->##func != NULL) orig_##func = (func##_t) DetourFunction((BYTE *) eth32.cg.module + eth32.cgMod->##func, (BYTE *) eth32_##func);
#define DETGAME(func)				if (eth32.et->##func != NULL) orig_##func = (func##_t) DetourFunction((BYTE *) eth32.et->##func, (BYTE *) eth32_##func);
#define UNDETFUNC(func)				if (orig_##func) { DetourRemove((BYTE *) orig_##func, (BYTE *) eth32_##func); orig_##func = NULL; }
#define CTHREAD(thread)				(CreateThread(0, 0, (LPTHREAD_START_ROUTINE) thread, 0, 0, 0))
#define CLOSETHREAD(handle)			{ CloseHandle(handle); handle = NULL; }
#define GET_PAGE(a)					((void*)(((unsigned long)a)&0xfffff000))
static DWORD oldprot;
#define unprotect(addr,len)			(VirtualProtect(addr,len,PAGE_EXECUTE_READWRITE,&oldprot))
#define reprotect(addr,len)			(VirtualProtect(addr,len,PAGE_EXECUTE_READ,&oldprot))
#define BOUND_VALUE(var,min,max)	if((var)>(max)){(var)=(max);};if((var)<(min)){(var)=(min);}

#ifdef ETH32_RELEASE
#define INSTALL_EXCEPTION_HANDLER	__asm push eth32_except_handler __asm push FS:[0] __asm mov FS:[0],ESP
#else
#define INSTALL_EXCEPTION_HANDLER	
#endif

// **************************
// Structs
// **************************

typedef struct weapStat_s {
	int kills;
	int deaths;
	int shots;
	int hits;
	int headshots;
} weapStat_t;

#define WS_ABS_MAX	32
#define WS_ETM_MAX	22
#define WS_JAY_MAX	24 // ?
#define WS_NQ_MAX	32	

typedef struct stats_s {
	int kills;
	int deaths;
	int spree;
	int longestSpree;
	float kdRatio;
	float threat;
	float accuracy;			// EV_BULLET based
	int suicides;
	int bulletsFired;		// EV_BULLET based
	int bulletsHit;			// EV_BULLET based

	int my_kills;
	int	my_deaths;
	int my_kdRatio;
	
	int killer;
	
	// noquarter seems to have most weapons - make big enough to fit all modz
	weapStat_t weap[WS_ABS_MAX];
	int bulletsFiredWS;
	int bulletsHitWS;
	float bulletAcc;
	int headshotsWS;
	float hr_kills;			// headshot ratio -> headshots/kills
	float hr_hits;			// headshot ratio -> headshots/hits
	float hr_shots;			// headshot ratio -> headshots/shots
	
}stats_t;

typedef enum killSpamtype_s {
	KILLSPAM_ALL,
	KILLSPAM_SELECTED,
	KILLSPAM_MAX
} killSpamtype_t;

typedef enum namestealMode_s {
	NAMESTEAL_TEAM,
	NAMESTEAL_ENEMY,
	NAMESTEAL_ALL,
	NAMESTEAL_FILE,
	NAMESTEAL_MAX
} namestealMode_t;

typedef struct {					// where we can access all the info we need on a player
	bool			*infoValid;		// we can add more data here later (aimpoints, visible, invunerable, etc.)
	centity_t		*cent;			// will be all pointers to the information we need in cg_entities and clientinfo
	entityState_t	*currentState;
	float			*lerpOrigin;	
	char			*name;
	int				*team;
	int				*health;
	int				*cls;			// class
	lerpFrame_t		*lf;			// for animation correction
	
	orientation_t	orHead;			// head orientation (position & rotation)
	bool			friendly;		// true for teammates
	bool			invulnerable;	// to disable aiming at them, and possibly for use in chams
	bool			visible;		// is this player visible
	bool			targetingUs;	// is this player targeting us?
	float			distance;		// distance from view origin to player
	bool			allowPMSpam;	// allow PM killspam to this client

	int				screenX;		// x coord on screen
	int				screenY;		// y coord on screen

	qhandle_t		headModel;

	stats_t			stats;
	vec3_t			headPt;			// visible point on the head
	vec3_t			bodyPt;			// visible point on body box
	vec3_t			aimPt;			// if aimbot is used, this will need to be set
	vec3_t			preAimPt;		// for early aim to reduce spread in the first shots
	
	int				clientNum;		// so we can use player_t* easier
	bool			eth32user;
	bool			eth32friend;
	char			portalName[64];	// store portal name
} player_t;

// for use by portal
typedef struct {
	char username[64];
	char server[32];
	int clientNum;
	int etVersion;
	int modVersion;
	float eth32Version;
	char serverName[64];
	char serverMapName[64];
	player_t *player;
	bool anon;
	bool frnd;	// friend
} eth32user_t;

// for tracking server info
typedef struct {
	char serverAddr[32];
	char serverName[64];
	char serverMapName[64];
	int numUsers;
	int numFriends;
	bool rampage;
} eth32server_t;

typedef struct {
	bool			*infoValid;	
	centity_t		*cent;			// will be all pointers to the information we need in cg_entities and clientinfo
	entityState_t	*currentState;
	float			*lerpOrigin;		// for greater portability

	bool			visible;		// is this gentity drawn inside screen coords?
	int				screenX;		// x coord on screen
	int				screenY;		// y coord on screen

	int				clientNum;		
}gentity_t;

typedef enum {
	WT_NONE,
	WT_KNIFE,
	WT_PISTOL,
	WT_SMG,
	WT_MG,
	WT_RIFLE,
	WT_SNIPER,
	WT_SHOTGUN,
	WT_HEAVY,
	WT_EXPLOSIVE,
	WT_MAX,
} weapType_t;

// sol: weapon attribs will be used for figuring out what kind of weapon we are dealing with
typedef enum {
	WA_NONE	=			0,
	WA_USER_DEFINED =	(1 << 0),	// allowed to configure through weapon window when enabled
	WA_OVERHEAT =		(1 << 1),	// this weapon can overheat
	WA_BLOCK_FIRE =		(1 << 2),	// this weapon should block fire in certain autofire modes
	WA_AKIMBO =			(1 << 3),	// dual weapons, need to know for drawing ammo properly
	WA_SCOPED =			(1 << 4),	// this weapon is in scoped mode
	WA_UNSCOPED =		(1 << 5),	// this weapon is in unscoped mode
	WA_RIFLE_GRENADE =	(1 << 6),	// camera use, grenade bot
	WA_MORTAR =			(1 << 7),	// camera use
	WA_PANZER =			(1 << 8),	// camera use
	WA_SATCHEL =		(1 << 9),	// camera use
	WA_NO_AMMO =		(1 << 10),	// this weapon doesn't use ammo
	WA_ONLY_CLIP =		(1 << 11),	// this weapon only uses clip (disregard ammo)
	WA_BALLISTIC =		(1 << 12),	// this weapon can be used for the ballistics aimbot
	WA_GRENADE =		(1 << 13),	// grenade bot (different code then riflenade)
} weapAttrib_t;

typedef struct {
	char	*name;
	// non user defined
	vec3_t	r_origin;		// weapon render: adjusted origin
	vec3_t	r_angle;		// weapon render: angle to place weapon
	float	r_dist;			// weapon render: camera distance from adjusted origin

	weapType_t	type;		// type of weapon, user can set a preference to all weapons of the same type
	unsigned int attribs;	// a mix of weapAttrib_t for filtering weapons
	int		otherAkimbo;	// if this weapon is akimbo, this is the matching weapon to use for ammo

	// things user can set per weapon if userDefined
	int		range;			// max range aimbot will search for targets
	int		headTraces;		// number of head traces to check
	int		bodyTraces;		// number of body traces to check
	int		delay;			// time in msec between firing
	bool	autofire;		// use autofire for this weap	
	bool	antiOverheat;	// if weap can overheat, enable anti-overheat

} weapdef_t;

typedef enum {
	CHAMS_OFF,
	CHAMS_CRYSTAL,
	CHAMS_WAVE,
	CHAMS_SOLID,
	CHAMS_QUAD,
	MAX_CHAMS
} eth32Chams_t;

typedef enum {
	AUTOPUSH_OFF,
	AUTOPUSH_ENEMY,
	AUTOPUSH_TEAM,
	AUTOPUSH_BOTH,
	MAX_AUTOPUSH,
} autopush_t;

typedef enum {
	ESP_PLAYER_MIDDLE,
	ESP_PLAYER_SIDE,
	ESP_PLAYER_STAGERED,
	ESP_PLAYER_MAX
} playerESP_t;

typedef enum {
	ESP_GENT_GRENADE,
	ESP_GENT_GRENADEP,
	ESP_GENT_DYNAMITE,
	ESP_GENT_LANDMINE,
	ESP_GENT_MORTAR,
	ESP_GENT_PANZER,
	ESP_GENT_RIFLENADE,
	ESP_GENT_SATCHEL,
	ESP_GENT_SMOKE,
	ESP_GENT_MAX
} gentityESP_t;

typedef enum {
	ESP_GENT_TCE_GRENADE,
	ESP_GENT_TCE_FLASH,
	ESP_GENT_TCE_SMOKE,
	ESP_GENT_TCE_DYNAMITE,
	ESP_GENT_TCE_MAX
} gentityESPTCE_t;


typedef enum {
	AUTOPT_OFF,
	AUTOPT_LOW,
	AUTOPT_HIGH,
	AUTOPT_MAX
} eth32AutoPoints_t;


typedef enum
{
	SOUND_LEVEL_ONE,
	SOUND_LEVEL_TWO,
	SOUND_LEVEL_THREE,
	SOUND_LEVEL_FOUR,
	SOUND_LEVEL_FIVE,
	SOUND_LEVEL_MAX
	
} soundLevel_t;

typedef enum
{
	SOUND_LEVEL_SIZE_ONE,
	SOUND_LEVEL_SIZE_TWO,
	SOUND_LEVEL_SIZE_THREE,
	SOUND_LEVEL_SIZE_FOUR,
	SOUND_LEVEL_SIZE_FIVE,
	SOUND_LEVEL_SIZE_MAX

} soundLevelSize_t;

typedef struct {

	sfxHandle_t		hitsound;
	sfxHandle_t		hitsoundtm;
	
	sfxHandle_t		playsound[SOUND_LEVEL_MAX][SOUND_LEVEL_SIZE_MAX];
} eth32sounds_t;

typedef struct {					// all the medias (pictures, sounds, models, etc)
	qhandle_t		white;
	qhandle_t		cursor;
	qhandle_t		binoc;
	qhandle_t		smokePuff;
	qhandle_t		reticleSimple;
	qhandle_t		eth32Icon;
	qhandle_t		circle;
	qhandle_t		disk;

	fontInfo_t		fontArial;

	qhandle_t		chams[MAX_CHAMS];
	
	eth32sounds_t   sounds;
	// menu stuff
	qhandle_t		statbar;
	qhandle_t		statbar2;
	qhandle_t		colorTable;

	// TCE model
	qhandle_t			grenadeTCE;			// TCE grenade Model
	qhandle_t			dynamiteTCE;		// TCE dynamite Model
	qhandle_t			flashGrenadeTCE;	// TCE flash Grenade Model
	qhandle_t			smokeGrenadeTCE;	// TCE smoke Grenade Model
	qhandle_t			medpackTCE;			// TCE med pack model
	// TCE icons
	qhandle_t			grenadeTCEIcon;		// TCE grenade Icon
	qhandle_t			dynamiteTCEIcon;	// TCE dynamite Icon
	qhandle_t			smokeGrenadeTCEIcon;// TCE flash Grenade Icon
	qhandle_t			flashGrenadeTCEIcon;// TCE smoke Grenade Icon

	// Regular Models
	qhandle_t			disguisedIcon;		// for disguised players
	qhandle_t			supplyRackAmmo;		// rack for Ammo Model	
	qhandle_t			supplyRackHealth;	// rack for Health Model
	qhandle_t			rackAmmo;			// Ammo on rack Model
	qhandle_t			rackHealth;			// Health on rack Model
	qhandle_t 			medPack;			// Health pack pickup Model
	qhandle_t			ammoPack;			// Ammo pack pickup Model
	qhandle_t			grenadeAxisModel;	// Axis grenade
	qhandle_t			grenadeAlliedModel;	// Allied grenade
	qhandle_t			dynamiteModel;		// Dynamite Model
	qhandle_t			satchelModel;		// Satchel Model
	qhandle_t			rocketModel;		// Panzer rocket Model - some cheat servers use this for grenade
	qhandle_t			mortarModel;		// Mortar shell Model
	qhandle_t			thompsonModel;		// Thompson Model
	qhandle_t			mp40Model;			// Mp40 Model
	qhandle_t			stenModel;			// Sten Model
	qhandle_t			rifleNadeAxis;		// axis grenade model for riflelauncher
	qhandle_t			rifleNadeAllied;	// allied grenade model for riflelauncher
	// icons
	qhandle_t			grenadeAxisIcon;		// Axis grenade Icon
	qhandle_t			grenadeAlliedIcon;	// Allied grenade Icon
	qhandle_t			dynamiteIcon;		// Dynamite Icon
	qhandle_t			satchelIcon;		// Satchel Icon
	qhandle_t			landmineIcon;		// Landmine Icon

	qhandle_t			railCore;			// shader for railtrails
	qhandle_t			portalMarker;	
	qhandle_t			combtnLeft, combtnRight, combtnCenter;
	qhandle_t			comselLeft, comselRight, comselCenter;

	//winamp stuff
	qhandle_t			waPlay, waStop, waPause, waNext, waPrev;

	//grenade target
	qhandle_t			grenTarget;

	qhandle_t			nullShader;
	qhandle_t			etlogoShader;

} media_t;

typedef struct {
	bool spawntimerReady;					// we can't use spawntimer until cgame gathers some info
} bools_t;

typedef enum {
	TEAM_ENEMY,
	TEAM_FRIEND,
	MAX_TEAMSTATES,
} teamState_t;

typedef struct {
	int soldier[2];
	int medic[2];
	int engineer[2];
	int lt[2];
	int covops[2];
} chargeTimes_t;

typedef struct {
	HMODULE		module;				// module (base) of cgame.dll in memory
	bool		automod;			// did we redirect unsupported mod to etmain?
	int			supported;			// used by VM_Create, if supported then modify vm->entryPoint
	
	int			clientNum;			// our ID on a server
	int			team;				// set once per frame to allow viewing team/enemy chams even while spec
	int			time;				// current server time
	int			frametime;			// time needed to draw this frame - can be used for prediction later
	int			xhairClientNum;		// client num at crosshair (-1 if none)

	player_t	players[MAX_CLIENTS];		// our player data , NOTE: we must know mod cent base/size and clientinfo base/size
	gentity_t	gentity[MAX_GENTITIES];		// gentity data

	snapshot_t		*snap;
	gameState_t		*gameState;
	pmoveExt_t		*pmext;
	playerState_t	*predictedps;

	weapdef_t   *currentWeapon;		// current weapon out of weaponList
	weapdef_t	*weaponList;		// current mod list of weapons
	weapdef_t	*modifyWeapon;		// used by weapon window
	int	modifyWeapNum;				// used by weapon window
	int numWeapons;					// mod dependent
	
	// rendering info
	float		screenXScale;
	float		screenYScale;
	refdef_t	*refdef;
	float		*refdefViewAngles;
	int			centerX, centerY;

	char		serverName[50];
	char		serverMapName[25];

	// muzzle, viewangles
	vec3_t		muzzle;
	float		ourViewAngleX;
	float		ourViewAngleY;

	media_t		media;				// all our medias

	bools_t		bools;				// various checks

	int				spawnTimes[MAX_TEAMSTATES];	// spawntimes
	chargeTimes_t	chargeTimes;	// for use by weap charge

	int			lastTrackerUpdate;	// tracker update interval check
	char		selectedPortalUser[64];	// used by portal users window
	char		replyPlayer[128];	// stores sender name of last priv msg
	int			cgTime;				// cgame time
	bool		hasInit;			// finished CG_Init()
	float		spreadIndex;		// spread index for current weapon
	float		ping;

	DWORD		defaultConsoleShader;	// name says it all

	vec3_t		tempRenderVector;

	vmCvar_t	*cg_teamchatheight;

	bool		pbserver;

	int			lastStatsReq;		// time of last stats request
	bool		wantStats;			// +stats down
	int			ws_max;				// mod dependant WS_MAX
	int			zerohour;			// first cg frametime
	int			limbo;				// players in limbo
	bool		serverObits;

	player_t	*self;

} cgame_t;

typedef struct {
	HMODULE		module;				// module (base) of ui.dll in memory
} ui_t;

typedef struct {
	HMODULE		module;				// et.exe module
	char		path[MAX_PATH];		// C:\path\to\ET\ 
	bool		supported;			// ET.exe version supported or not :f

	float		*viewAngleX;		// view angle x
	float		*viewAngleY;		// view angle y
	glconfig_t	*glconfig;			// et's glconfig
	int			*com_frameTime;		// continous, cgame independent msec counter

	qhandle_t	console[3];			// the three shares used for the console

	// ptrs to some cvars so we don't have to do Cvar_VariableStringBuffer
	cvar_t	*cg_fov;
	cvar_t	*sensitivity;
	cvar_t	*cg_railTrailTime;
} game_t;

// MOD/ET defines - these values must never change
// Every new mod/et version must be added to the portal defines as well
#define MOD_ETMAIN		0
#define MOD_ETPUB		1
#define MOD_JAYMOD1		2
#define MOD_JAYMOD2		3
#define MOD_NOQUARTER	4
#define MOD_ETPRO		5
#define MOD_TCE			6

#define ET_255			0
#define	ET_256			1
#define ET_260			2
#define	ET_260b			3

typedef struct {
	char			version[64];	// ET.exe version
	DWORD			crc32;		// crc32 checksum of ET.exe
	int				ver;		// for portal communication

	DWORD			viewAngleX;
	DWORD			viewAngleY;
	DWORD			inButtons;
	DWORD			glConfig;
	DWORD			com_frameTime;
	DWORD			consoleshader;

	DWORD			etroq_video;
	char			etroq_safe[5];

	DWORD			syscall;
	DWORD			Cvar_Get;
	DWORD			Cvar_Set2;
	DWORD			fs_searchpaths;
	DWORD			FS_PureServerSetLoadedPaks;
	DWORD			FS_AddGameDirectory;
	DWORD			FS_AddGameDirectory2;
	DWORD			FS_Startup;
	DWORD			FS_Startup2;
	DWORD			Cmd_AddCommand;
	DWORD			VM_Create;
	DWORD			Sys_LoadDll;
	DWORD			LoadTGA;
	DWORD			server_ip;
} et_t;

typedef struct {
	char			name[64];						// mod name
	char			version[64];					// mod version
	DWORD			crc32;						// crc32 checksum
	int				type;						// mod type

	char			etproAacSafe[32];			// save backup, for 'undetour' on shutdown

	DWORD			refdef;						// addr of refdef
	DWORD			refdefViewAngles;			// addr of refdefVA

	DWORD			pmext;						// addr of cg.pmext

	// Refer to CG_ConsoleCommand to find these (or vmmain command case 0x2)
	DWORD			commands;					// addr of console command list
	int				numCommands;				// number of commands in list
	DWORD			predictedPS;				// the predicted playerstate

	// These will be used to fill in the pointers to player data
	DWORD			centBase;					// &cg_entities[0]
	DWORD			centSize;					// sizeof(centity_t)
	DWORD			centCurrentStateOffset;		// currentState offset
	DWORD			centLerpOrigin;				// lerpOrgin offset

	DWORD			ciBase;						// &clientInfo[0]
	DWORD			ciSize;						// sizeof(clientInfo_t)
	DWORD			ciInfoValidOffset;			// infoValid offset
	DWORD			ciNameOffset;				// name offset
	DWORD			ciTeamOffset;				// team offset
	DWORD			ciHealthOffset;				// health offset
	DWORD			ciClassOffset;				// class offset
	
	DWORD			CG_Player;					// called for each player every frame
	DWORD			CG_AddPlayerWeapon;			// called to add a weapon to entity
	DWORD			CG_PositionRotatedEntityOnTag;	// for grabbing head origin and orientation
	DWORD			CG_SetLerpFrameAnimationRate;	// used to capture clientinfo base & size for unknown mods
	DWORD			CG_FinishWeaponChange;		// changes between 2 weapons. i.e. forced change scoped -> normal
	DWORD			CG_PlayVoiceChat;			// capture voice chat messages for GUI
	DWORD			CG_EntityEvent;				// get EntitiyEvent info
	DWORD			CG_Trace;					// run entity traces
	DWORD			CG_DamageFeedback;			// disable view distortion from being hit
	DWORD			CG_WeaponFireRecoil;		// disable view distortion when firing
	DWORD			CG_Respawn;					// lots of stuff we can reset or whatever on respawn
	DWORD			CG_CalculateReinfTime;		// so we dont have to do calcs. let the cgame do it for us
	DWORD			CG_AllocLocalEntity;		// creative with rays (railtrail)

	DWORD			CG_AddPMItem;
	DWORD			CG_AddPMItem2;				// jaymod version
	DWORD			CG_DrawPlayerStatusHead;	
	DWORD			CG_DrawPlayerStats;
	DWORD			CG_DrawPlayerStatus;
	DWORD			CG_CenterPrint;
	DWORD			CG_PriorityCenterPrint;
	DWORD			CG_ChargeTimesChanged;
	DWORD			BG_FindClipForWeapon;
	DWORD			BG_FindAmmoForWeapon;
	DWORD			CG_CalcViewValues;
} cgMod_t;

typedef enum {
	CAM_MORTAR,
	CAM_FOLLOW,
	CAM_RIFLE,
	CAM_ENEMY,
	CAM_MIRROR,
	CAM_PANZER,
	CAM_SATCHEL,
	CAM_TARGET,
	CAM_DROP_ONE,
	CAM_DROP_TWO,
	CAM_DROP_THREE,
	CAM_DROP_FOUR,
	CAM_MAX
} camType_t;

typedef struct {
	int x1, y1, x2, y2;
	bool	display;
	float	distance;
	float	angle;
	float	fov;
	// dropcams
	bool	active;
	vec3_t	origin;
	vec3_t	axis;
} camInfo_t; 

typedef struct {
	bool	chams;
	bool	text;
	bool	icon;
	bool	radarIcon;
	int		distance;
} gentityInfo_t; 

typedef struct {
	bool	loaded;		// not for normal use, to prevent saving settings when they are not rdy

	int		chamsPlayer;// player chams
	int		chamsWeapon;// weapon chams
	bool	chamsOurWeapon; // apply chams to our own weapon
	bool	espName;	// name esp
	bool	espTracker;	// draw portal names
	gentityInfo_t gentInfo[ESP_GENT_MAX];
	int		playerEspType; // sets value for esp type
	int		gentitySetting; // used for cycle throught type of gent for esp config

	bool	iconTracker;// draw portal icon
	bool	wallhack;	// wallhack :f
	bool	blackout;	// draw sniper/binocs blackout
	bool	weaponZoom;	// draw scoped weapon zoom
	float	scopedTurnSpeed;	// scoped turning speed
	int		smoketrnsp;	// smoke transparency changer
	bool	respawnTimers; // draw respawn timers window
	bool	autoTapout;	// tapout on death right to reinforce queue
	
	int		aimType;	// global setting to set aim style (always, on key, etc.) or disable aim globally
	int		aimSort;	// global target sort
	bool	autofire;	// global autofire settings (each weapon can be set for autofire as well)
	bool	lockTarget;
	bool	atkValidate;
	int		hitboxType;
	int		traceType;
	float	fov;
	float	predTarget;
	float	predSelf;
	int		predSelfType;
	float	dynamicHitboxScale;	// to scale dynamic hitboxes (or not)

	bool	drawHeadHitbox;
	bool	drawBodyHitbox;
	bool	drawBulletRail;
	bool	railWallhack;		// see the rails trough walls
	bool	drawHeadAxes;
	vec3_t	colorHeadHitbox;
	vec3_t	colorBodyHitbox;
	vec3_t	colorBulletRail;

	vec3_t	colorXAxis;	// originally just for head hitbox, but probably useful for whatever we want to draw axes with
	vec3_t	colorYAxis;
	vec3_t	colorZAxis;

	int		headRailTime;
	int		bulletRailTime;
	int		bodyRailTime;
	
	// for grenade aimbot
	bool	grenadeBot;
	bool	rifleBot;
	bool	grenadeBlockFire;
	bool	valGrenTrajectory;
	bool	valRifleTrajectory;
	bool	grenadeSenslock;
	int		grenadeFireDelay;
	float	grenadeZ;
	float	riflenadeZ;
	bool	grenadeAutoFire;
	bool	rifleAutoFire;
	bool	ballisticPredict;
	float	radiusDamage;
	bool	ballisticRadiusDamage;
	bool	autoGrenTargets;

	int		portalCoopMode;	// don't aim at other ETH32 users if true
	bool	portalAnon;			// send anonymous info to tracker
	int		portalRampageNum;	// minimum number of players on server to qualify as a rampage 
	bool	portalRampageNotify;// post current rampages to center print on portal update

	bool	killSounds;	// kill sounds :P
	bool	hitsounds;	// custom hitsound for mods that dont have it

	float	radarRange;	// range used by CRadarWindow

	bool	nameClick;	// allow clicking names in chat window to start a pm
	bool	xhairInfo;	// display esp window of xhair entity
	int		xhairInfoTime;	// time xhairinfo will stay up

	int		ircChatMsgs; // if true, route irc msgs to main chat window also
	int		ircConsoleMsgs; // if true, route irc msgs to console also
	bool	ircMention;	// send center print msg if someone mentions our nick or portal username?

	BYTE	colorTeam[3];
	BYTE	colorTeamHidden[3];
	BYTE	colorEnemy[3];
	BYTE	colorEnemyHidden[3];
	BYTE	colorInvulnerable[3];
	BYTE	colorTeamWeapon[3];
	BYTE	colorEnemyWeapon[3];
	BYTE	colorHealth[3];
	BYTE	colorAmmo[3];

	char	ircServer[MAX_STRING_CHARS];
	char	pk3file[MAX_STRING_CHARS];
	char	etproGuid[MAX_STRING_CHARS];
	bool	etproOs;
	char	jayMac[MAX_STRING_CHARS];

	bool	thirdPerson;
	bool	origViewValues;
	bool	interpolatedPs;
	bool	dmgFeedback;

	int		autoPoints;		// automatic hitpoint number selection
	bool	autoCrouch;
	int		headbody;

	bool	preAim;
	float	preAimTime;
	bool	preShoot;
	float	preShootTime;

	float	animCorrection;

	int		autoPush;
	float	autoPushDistance;
	
	bool	defaultSoundsLoaded;

	bool	transparantConsole;

	// cameras
	int			camType;
	int			dropCamCount;
	camInfo_t	cams[CAM_MAX];

	bool	grenadeTracer;
	bool	rifleTracer;

	// used for figuring out render settings per weapon, temporary
	vec3_t weapOrigin;
	vec3_t weapAngle;
	float weapDist;

	// gui display options
	bool	guiBanner;
	bool	guiChat;
	bool	guiCenterPrint;
	bool	guiCenterPrintAnim;
	float	guiCenterPrintScale;
	bool	guiOriginal;	
	float	BannerScale;
	char	BannerFmt[MAX_STRING_CHARS];

	// name steal options
	bool	doNamesteal;		// do namesteal - forced to off on connect!
	int		NamestealGrace;		// time to wait after joining
	int		NamestealMode;		// type of namestealing
	int		NamestealDelay;		// delay between steals

	bool	killSpam;			// enable killspam
	bool	multikillSpam;		// enable multikill spam
	bool	deathSpam;			// enable spam when you die
	bool	suicideSpam;		// PM spam when someone /kill
	bool	pmKillspam;			// PM killspam
	bool	selfkillSpam;		// /kill spam
	int		KillspamType;		// Selected clients only or everyone

	bool	doDistWallhack;	// for players only
	float	wallhackDistance;	// for players only
	bool	drawDisguised;	// draw disguised esp
	bool	grenadeDlight;	// extra grenade visuals
	bool	mortarDlight;	// extra mortar visuals
	bool	mortarTrace;	// extra mortar trace + esp
	bool	artyMarkers;	// extra arty markers

	int		mkResetTime;	// mkill reset time

	bool	panzerSuicide;			// auto suicide if imminent panzer hit
	float	panzerImpactDist;		// minimum distance to panzer impact point to commit suicide
} settings_t;

typedef struct cvarInfo_s {
	char name[MAX_STRING_CHARS];
	char ourValue[MAX_STRING_CHARS];
	char restoreValue[MAX_STRING_CHARS];
	bool isSet;
	cvarInfo_s *next;
} cvarInfo_t;

typedef struct {				// barebones of style management
	float fontScale;
	int	fontHeight;

	vec4_t titleColor;
	vec4_t textColor1;
	vec4_t textColor2;

	qhandle_t titleLeft, titleCenter, titleRight;

	qhandle_t winLeft;
	qhandle_t winTop;
	qhandle_t winTopLeft;
	qhandle_t winCenter;

	qhandle_t txtinputLeft, txtinputCenter, txtinputRight;
	qhandle_t btnLeft, btnCenter, btnRight;
	qhandle_t btnselLeft, btnselCenter, btnselRight;

	qhandle_t dropboxArrow;
	qhandle_t check, checkBox;
	qhandle_t scrollbarArrow, scrollbarBtn, scrollbarTrack;

	qhandle_t sliderTrack, sliderBtn;
	qhandle_t camCorner;

	qhandle_t mousePtr;
} guiassets_t;

typedef struct {
	cgame_t			cg;					// info from cgame lib
	ui_t			ui;					// info from ui lib
	game_t			game;				// info from et.exe

	const et_t		*et;				// needed info about et.exe
	const cgMod_t	*cgMod;				// needed info about cgame.dll

	settings_t		settings;			// our user settings

	stats_t			stats;				// EntitiyEvent info 

	guiassets_t		guiAssets;			// used for CGui for rendering

	char			path[MAX_PATH];		// C:\path\to\eth32\ 

	HINSTANCE		hInst;

	bool			authed;

	bool			cgameLoaded;		// tracker
	char			server[64];			// tracker

	char			pk3File[MAX_STRING_CHARS];	// pk3file
	DWORD			eth32CRC;
} eth32_t;

extern eth32_t eth32;

// kobject: imported this from Q3 SDK
// downtime & msec only have effect for movement buttons, so for attack we're in the clear
typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	int			active;			// current state
	int			wasPressed;		// set when down, not cleared when up
} kbutton_t;
