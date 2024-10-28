/* >>EXPORT
Ÿƒtƒ@ƒCƒ‹–¼
SC59A.dll: E231Œn‹ßxŒ^—p’è‘¬E—}‘¬ƒ‚ƒWƒ…[ƒ‹

Ÿƒtƒ@ƒCƒ‹‚Ìà–¾
‚±‚Ìƒtƒ@ƒCƒ‹‚ÍE231Œn‹ßxŒ^—p’è‘¬E—}‘¬ƒ‚ƒWƒ…[ƒ‹B
DetailManager.dll‚Æ‚Ì•¹—pê—pB

’è‘¬‚Í60km/hˆÈã‚Å“®ìA58km/hˆÈ‰º‚Å‰ğœB
’á’è‘¬‚Í5km/hˆÈã10km/hˆÈ‰º‚Å“®ìA5km/hˆÈ‰º‚Å‰ğœB
—}‘¬‚Í55km/hˆÈã‚Å“®ìA54km/hˆÈ‰º‚Å‰ğœB
*/

#pragma warning(disable:4996)

#include "stdafx.h"
#include <windows.h>
#include <math.h>
#include "atsplugin.h"
#include "ATSIni.h"
#include "SC59A.h"

#define CSC_SPEED_MAX 60.0f
#define CSC_SPEED_MIN 58.0f

#define CSCL_SPEED_MAX 11.0f
#define CSCL_SPEED_MIN 5.0f

#define CSCY_SPEED_MAX 55.0f
#define CSCY_SPEED_MIN 54.0f

bool g_csc_state; // ’è‘¬‰^“]‚Ìó‘Ô
bool g_csc_active; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
bool g_csc_reset; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é

bool g_cscl_state; // ’á’è‘¬‰^“]‚Ìó‘Ô
bool g_cscl_active; // ’á’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
bool g_cscl_reset; // ’á’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é

bool g_cscy_state; // —}‘¬‰^“]‚Ìó‘Ô
bool g_cscy_active; // —}‘¬‰^“]‚ğ—LŒø‚É‚·‚é
bool g_cscy_reset; // —}‘¬‰^“]‚ğ–³Œø‚É‚·‚é

#ifdef __cplusplus
extern "C" {
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		{
			case DLL_PROCESS_ATTACH:
			char *posIni;
			char filePath[ _MAX_PATH + 1 ] = _T( "" );
			::GetModuleFileName((HMODULE)hModule,filePath,_MAX_PATH);
			posIni = strstr(filePath,".dll");
			memmove(posIni,".ini",4);
			g_ini.load(filePath);
			if (!g_ini.load(filePath)) {
				g_ini.save();}
		}
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return true;
}

#ifdef __cplusplus
};
#endif

ATS_API void WINAPI Load()
{
}

ATS_API void WINAPI Dispose()
{
}

ATS_API int WINAPI GetPluginVersion()
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;
	g_emgBrake = g_svcBrake + 1;
}

ATS_API void WINAPI Initialize(int brake)
{
	g_speed = 0;
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	g_time = vehicleState.Time;
	g_speed = vehicleState.Speed;

	// ’è‘¬‰^“]‚ğÀs‚·‚é
	if(g_csc_state == false && g_csc_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_csc_state = true; // ’è‘¬‰^“]‚Ìó‘Ô
			g_csc_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		}
	}
	else if(g_csc_state == true && (g_csc_reset == true || g_speed <= CSC_SPEED_MIN))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_csc_state = false; // ’è‘¬‰^“]‚Ìó‘Ô
		g_csc_reset = false; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
	/*
	else
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	}
	*/

	// ’á’è‘¬‰^“]‚ğÀs‚·‚é
	else if(g_cscl_state == false && g_cscl_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_cscl_state = true; // ’è‘¬‰^“]‚Ìó‘Ô
			g_cscl_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		}
	}
	else if(g_cscl_state == true && (g_cscl_reset == true))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_cscl_state = false; // ’è‘¬‰^“]‚Ìó‘Ô
		g_cscl_reset = false; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}

	// —}‘¬‰^“]‚ğÀs‚·‚é
	else if(g_cscy_state == false && g_cscy_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_cscy_state = true; // ’è‘¬‰^“]‚Ìó‘Ô
			g_cscy_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		}
	}
	else if(g_cscy_state == true && (g_cscy_reset == true || g_speed <= CSCY_SPEED_MIN))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_cscy_state = false; // ’è‘¬‰^“]‚Ìó‘Ô
		g_cscy_reset = false; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
	else
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	}

	// ƒnƒ“ƒhƒ‹o—Í
	g_output.Reverser = g_reverser;
	g_output.Brake = g_brakeNotch;
	g_output.Power = g_powerNotch;

	panel[g_ini.Index.ConstantSpeedControl] = ((g_csc_state == 1 && g_powerNotch >= 4 && g_brakeNotch == 0) || ((g_cscl_state == true && g_powerNotch == 1 && g_brakeNotch == 0) == true ? (g_cscl_state == true && g_powerNotch == 1 && g_brakeNotch == 0) + ((g_time % 800) / 400) : 0)); // ’è‘¬
	panel[g_ini.Index.HoldingSpeedBrake] = (g_cscy_state == true && g_powerNotch == 0 && g_brakeNotch == 0); // —}‘¬

	return g_output;
}

/* >>EXPORT
ŸPanelƒCƒ“ƒfƒbƒNƒX
ats140 ’è‘¬
ats141 —}‘¬
*/

/* >>EXPORT
ŸSoundƒCƒ“ƒfƒbƒNƒX
‚È‚µ
*/

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
	if(g_powerNotch < 4)
	{
		g_csc_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		g_csc_reset = true; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
	if(g_powerNotch != 1)
	{
		g_cscl_active = false; // ’á’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		g_cscl_reset = true; // ’á’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
	if (g_powerNotch != 0)
	{
		g_cscy_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		g_cscy_reset = true; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_brakeNotch = notch;
	if(g_brakeNotch != 0)
	{
		g_cscy_active = false; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
		g_cscy_reset = true; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
	}
}

ATS_API void WINAPI SetReverser(int pos)
{
	g_reverser = pos;
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
	if(atsKeyCode == KeyIndex(g_ini.Key.Switch)) // 2 key
	{
		if((g_powerNotch >= 4 && g_brakeNotch == 0) && g_speed >= CSC_SPEED_MAX)
		{
			g_csc_active = true; // ’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
			g_csc_reset = false; // ’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
		}
		if((g_powerNotch == 1 && g_brakeNotch == 0) && g_speed >= CSCL_SPEED_MIN && g_speed <= CSCL_SPEED_MAX)
		{
			g_cscl_active = true; // ’á’è‘¬‰^“]‚ğ—LŒø‚É‚·‚é
			g_cscl_reset = false; // ’á’è‘¬‰^“]‚ğ–³Œø‚É‚·‚é
		}
		if((g_powerNotch == 0 && g_brakeNotch == 0) && g_speed >= CSCY_SPEED_MAX)
		{
			g_cscy_active = true; // —}‘¬‰^“]‚ğ—LŒø‚É‚·‚é
			g_cscy_reset = false; // —}‘¬‰^“]‚ğ–³Œø‚É‚·‚é
		}
	}
}

ATS_API void WINAPI KeyUp(int atsKeyCode)
{
}

/* >>EXPORT
ŸƒL[ƒAƒTƒCƒ“
‚È‚µ
*/

ATS_API void WINAPI HornBlow(int hornType)
{
}

ATS_API void WINAPI DoorOpen()
{
	g_pilotlamp = false;
}

ATS_API void WINAPI DoorClose()
{
	g_pilotlamp = true;
}

ATS_API void WINAPI SetSignal(int signal)
{
}

/* >>EXPORT
ŸSignalƒCƒ“ƒfƒbƒNƒX
‚È‚µ
*/

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
}

/* >>EXPORT
ŸBeaconƒCƒ“ƒfƒbƒNƒX
‚È‚µ
*/
