/* >>EXPORT
���t�@�C����
SC59A.dll: E231�n�ߍx�^�p�葬�E�}�����W���[��

���t�@�C���̐���
���̃t�@�C����E231�n�ߍx�^�p�葬�E�}�����W���[���B
DetailManager.dll�Ƃ̕��p��p�B

�葬��60km/h�ȏ�œ���A58km/h�ȉ��ŉ����B
��葬��5km/h�ȏ�10km/h�ȉ��œ���A5km/h�ȉ��ŉ����B
�}����55km/h�ȏ�œ���A54km/h�ȉ��ŉ����B
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

bool g_csc_state; // �葬�^�]�̏��
bool g_csc_active; // �葬�^�]��L���ɂ���
bool g_csc_reset; // �葬�^�]�𖳌��ɂ���

bool g_cscl_state; // ��葬�^�]�̏��
bool g_cscl_active; // ��葬�^�]��L���ɂ���
bool g_cscl_reset; // ��葬�^�]�𖳌��ɂ���

bool g_cscy_state; // �}���^�]�̏��
bool g_cscy_active; // �}���^�]��L���ɂ���
bool g_cscy_reset; // �}���^�]�𖳌��ɂ���

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

	// �葬�^�]�����s����
	if(g_csc_state == false && g_csc_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_csc_state = true; // �葬�^�]�̏��
			g_csc_active = false; // �葬�^�]��L���ɂ���
		}
	}
	else if(g_csc_state == true && (g_csc_reset == true || g_speed <= CSC_SPEED_MIN))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_csc_state = false; // �葬�^�]�̏��
		g_csc_reset = false; // �葬�^�]�𖳌��ɂ���
	}
	/*
	else
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	}
	*/

	// ��葬�^�]�����s����
	else if(g_cscl_state == false && g_cscl_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_cscl_state = true; // �葬�^�]�̏��
			g_cscl_active = false; // �葬�^�]��L���ɂ���
		}
	}
	else if(g_cscl_state == true && (g_cscl_reset == true))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_cscl_state = false; // �葬�^�]�̏��
		g_cscl_reset = false; // �葬�^�]�𖳌��ɂ���
	}

	// �}���^�]�����s����
	else if(g_cscy_state == false && g_cscy_active == true)
	{
		if(g_pilotlamp == true)
		{
			g_output.ConstantSpeed = ATS_CONSTANTSPEED_ENABLE;
			g_cscy_state = true; // �葬�^�]�̏��
			g_cscy_active = false; // �葬�^�]��L���ɂ���
		}
	}
	else if(g_cscy_state == true && (g_cscy_reset == true || g_speed <= CSCY_SPEED_MIN))
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;
		g_cscy_state = false; // �葬�^�]�̏��
		g_cscy_reset = false; // �葬�^�]�𖳌��ɂ���
	}
	else
	{
		g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	}

	// �n���h���o��
	g_output.Reverser = g_reverser;
	g_output.Brake = g_brakeNotch;
	g_output.Power = g_powerNotch;

	panel[g_ini.Index.ConstantSpeedControl] = ((g_csc_state == 1 && g_powerNotch >= 4 && g_brakeNotch == 0) || ((g_cscl_state == true && g_powerNotch == 1 && g_brakeNotch == 0) == true ? (g_cscl_state == true && g_powerNotch == 1 && g_brakeNotch == 0) + ((g_time % 800) / 400) : 0)); // �葬
	panel[g_ini.Index.HoldingSpeedBrake] = (g_cscy_state == true && g_powerNotch == 0 && g_brakeNotch == 0); // �}��

	return g_output;
}

/* >>EXPORT
��Panel�C���f�b�N�X
ats140 �葬
ats141 �}��
*/

/* >>EXPORT
��Sound�C���f�b�N�X
�Ȃ�
*/

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
	if(g_powerNotch < 4)
	{
		g_csc_active = false; // �葬�^�]��L���ɂ���
		g_csc_reset = true; // �葬�^�]�𖳌��ɂ���
	}
	if(g_powerNotch != 1)
	{
		g_cscl_active = false; // ��葬�^�]��L���ɂ���
		g_cscl_reset = true; // ��葬�^�]�𖳌��ɂ���
	}
	if (g_powerNotch != 0)
	{
		g_cscy_active = false; // �葬�^�]��L���ɂ���
		g_cscy_reset = true; // �葬�^�]�𖳌��ɂ���
	}
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_brakeNotch = notch;
	if(g_brakeNotch != 0)
	{
		g_cscy_active = false; // �葬�^�]��L���ɂ���
		g_cscy_reset = true; // �葬�^�]�𖳌��ɂ���
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
			g_csc_active = true; // �葬�^�]��L���ɂ���
			g_csc_reset = false; // �葬�^�]�𖳌��ɂ���
		}
		if((g_powerNotch == 1 && g_brakeNotch == 0) && g_speed >= CSCL_SPEED_MIN && g_speed <= CSCL_SPEED_MAX)
		{
			g_cscl_active = true; // ��葬�^�]��L���ɂ���
			g_cscl_reset = false; // ��葬�^�]�𖳌��ɂ���
		}
		if((g_powerNotch == 0 && g_brakeNotch == 0) && g_speed >= CSCY_SPEED_MAX)
		{
			g_cscy_active = true; // �}���^�]��L���ɂ���
			g_cscy_reset = false; // �}���^�]�𖳌��ɂ���
		}
	}
}

ATS_API void WINAPI KeyUp(int atsKeyCode)
{
}

/* >>EXPORT
���L�[�A�T�C��
�Ȃ�
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
��Signal�C���f�b�N�X
�Ȃ�
*/

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
}

/* >>EXPORT
��Beacon�C���f�b�N�X
�Ȃ�
*/
