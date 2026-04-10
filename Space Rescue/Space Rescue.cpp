#include "framework.h"
#include "Space Rescue.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "D2BMPLOADER.h"
#include "ErrH.h"
#include "FCheck.h"
#include "gifresizer.h"
#include "RescueDLL.h"
#include <chrono>
#include <clocale>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "rescuedll.lib")


constexpr wchar_t bWinClassName[]{ L"SpRescue" };
constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };
constexpr wchar_t sound_file[]{ L".\\res\\snd\\main.wav" };

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int record{ 3001 };
constexpr int first_record{ 3002 };
constexpr int no_record{ 3003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HICON mainIcon{ nullptr };
HCURSOR mainCursor{ nullptr };
HCURSOR outCursor{ nullptr };
HWND bHwnd{ nullptr };
HDC PaintDC{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
PAINTSTRUCT bPaint{};
MSG bMsg{};
BOOL bRet{ 0 };
POINT cur_pos{};
UINT bTimer{ 0 };

D2D1_RECT_F b1Rect{ 50.0f, 5.0f, scr_width / 3.0f - 50.0f, 45.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 50.0f, 5.0f, scr_width * 2.0f / 3.0f - 50.0f, 45.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 50.0f, 5.0f, scr_width - 50.0f, 45.0f };

D2D1_RECT_F b1txtRect{ 80.0f, 15.0f, scr_width / 3.0f - 50.0f, 45.0f };
D2D1_RECT_F b2txtRect{ scr_width / 3.0f + 80.0f, 15.0f, scr_width * 2.0f / 3.0f - 50.0f, 45.0f };
D2D1_RECT_F b3txtRect{ scr_width * 2.0f / 3.0f + 70.0f, 15.0f, scr_width - 50.0f, 45.0f };

bool pause = false;
bool show_help = false;
bool in_client = true;
bool sound = true;
bool b1hglt = false;
bool b2hglt = false;
bool b3hglt = false;

bool name_set = false;
bool hero_killed = false;

wchar_t current_player[16]{ L"TARLYO" };

int level = 1;
int score = 0;
float distance = 600;
int civs_saved = 0;

float scale_x{ 0 };
float scale_y{ 0 };

dll::RANDIT RandIt{};

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpLogo{ nullptr };
ID2D1Bitmap* bmpLoose{ nullptr };
ID2D1Bitmap* bmpWin{ nullptr };
ID2D1Bitmap* bmpRecord{ nullptr };
ID2D1Bitmap* bmpLevel{ nullptr };

ID2D1Bitmap* bmpBullet{ nullptr };
ID2D1Bitmap* bmpField{ nullptr };
ID2D1Bitmap* bmpGround{ nullptr };

ID2D1Bitmap* bmpPoints{ nullptr };
ID2D1Bitmap* bmpRepair{ nullptr };
ID2D1Bitmap* bmpShield{ nullptr };
ID2D1Bitmap* bmpGun{ nullptr };

ID2D1Bitmap* bmpBomb{ nullptr };

ID2D1Bitmap* bmpCivil[17]{ nullptr };

ID2D1Bitmap* bmpEvilL[4]{ nullptr };
ID2D1Bitmap* bmpEvilR[4]{ nullptr };

ID2D1Bitmap* bmpExplosion[24]{ nullptr };

ID2D1Bitmap* bmpHeroL[4]{ nullptr };
ID2D1Bitmap* bmpHeroR[4]{ nullptr };

ID2D1Bitmap* bmpCannon[4]{ nullptr };
ID2D1Bitmap* bmpIntro[16]{ nullptr };
ID2D1Bitmap* bmpMeteor1[20]{ nullptr };
ID2D1Bitmap* bmpMeteor2[20]{ nullptr };
ID2D1Bitmap* bmpSupply[27]{ nullptr };

///////////////////////////////////////////////////////////////

std::vector<dll::FIELDS*> vFields;
std::vector<dll::FIELDS*> vGrounds;

std::vector<dll::BONUS> vBonuses;

std::vector<dll::ASSETS*>vCivilians;

std::vector<dll::ASSETS*>vPowerups;

std::vector<dll::GUN*> vGuns;

std::vector<dll::EVIL*>vEvils;

std::vector<dll::SHOTS*>vEvilShots;

std::vector<dll::SHOTS*>vMyShots;

std::vector<dll::SHOTS*>vBombs;

std::vector<EXPLOSION> vExplosions;

dll::HERO* Hero{ nullptr };

dirs nature_dir = dirs::stop;

bool need_field_left = false;
bool need_field_right = false;

bool need_ground_left = false;
bool need_ground_right = false;

///////////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
	check.Release();
};
template<HasRelease T>bool FreeMem(T** var)
{
	if ((*var))
	{
		(*var)->Release();
		(*var) = nullptr;
		return true;
	}
	return false;
};
void LogErr(const wchar_t* what)
{
	std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
	err << what << L" Time stamp: " << std::chrono::system_clock::now() << std::endl;
	err.close();
}
void ClearResources()
{
	if (!FreeMem(&iFactory))LogErr(L"Error releasing main D2D1 Factory !");
	if (!FreeMem(&Draw))LogErr(L"Error releasing main D2D1 HwndRenderTarget !");
	if (!FreeMem(&b1BckgBrush))LogErr(L"Error releasing main D2D1 b1BckgBrush !");
	if (!FreeMem(&b2BckgBrush))LogErr(L"Error releasing main D2D1 b2BckgBrush !");
	if (!FreeMem(&b3BckgBrush))LogErr(L"Error releasing main D2D1 b3BckgBrush !");

	if (!FreeMem(&txtBrush))LogErr(L"Error releasing main D2D1 txtBrush !");
	if (!FreeMem(&statBrush))LogErr(L"Error releasing main D2D1 statBrush !");
	if (!FreeMem(&inactBrush))LogErr(L"Error releasing main D2D1 inactBrush !");
	if (!FreeMem(&hgltBrush))LogErr(L"Error releasing main D2D1 hgltBrush !");

	if (!FreeMem(&iWriteFactory))LogErr(L"Error releasing main D2D1 WriteFactory !");
	if (!FreeMem(&nrmFormat))LogErr(L"Error releasing main D2D1 nrmFormat !");
	if (!FreeMem(&midFormat))LogErr(L"Error releasing main D2D1 midFormat !");
	if (!FreeMem(&bigFormat))LogErr(L"Error releasing main D2D1 bigFormat !");

	if (!FreeMem(&bmpLogo))LogErr(L"Error releasing main D2D1 bmpLogo !");
	if (!FreeMem(&bmpWin))LogErr(L"Error releasing main D2D1 bmpWin !");
	if (!FreeMem(&bmpLoose))LogErr(L"Error releasing main D2D1 bmpLoose !");
	if (!FreeMem(&bmpRecord))LogErr(L"Error releasing main D2D1 bmpRecord !");
	if (!FreeMem(&bmpLevel))LogErr(L"Error releasing main D2D1 bmpLevel !");

	if (!FreeMem(&bmpBullet))LogErr(L"Error releasing main D2D1 bmpBullet !");
	if (!FreeMem(&bmpBomb))LogErr(L"Error releasing main D2D1 bmpBomb !");
	if (!FreeMem(&bmpField))LogErr(L"Error releasing main D2D1 bmpField !");
	if (!FreeMem(&bmpGround))LogErr(L"Error releasing main D2D1 bmpGround !");

	if (!FreeMem(&bmpPoints))LogErr(L"Error releasing main D2D1 bmpPoints !");
	if (!FreeMem(&bmpShield))LogErr(L"Error releasing main D2D1 bmpShield !");
	if (!FreeMem(&bmpRepair))LogErr(L"Error releasing main D2D1 bmpRepair !");
	if (!FreeMem(&bmpGun))LogErr(L"Error releasing main D2D1 bmpGun !");

	for (int i = 0; i < 17; ++i)if (!FreeMem(&bmpCivil[i]))LogErr(L"Error releasing main D2D1 bmpCivil !");

	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpEvilL[i]))LogErr(L"Error releasing main D2D1 bmpEvilL !");
	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpEvilR[i]))LogErr(L"Error releasing main D2D1 bmpEvilR !");

	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpCannon[i]))LogErr(L"Error releasing main D2D1 bmpCannon !");
	for (int i = 0; i < 17; ++i)if (!FreeMem(&bmpExplosion[i]))LogErr(L"Error releasing main D2D1 bmpExplosion !");

	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpHeroL[i]))LogErr(L"Error releasing main D2D1 bmpHeroL !");
	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpHeroR[i]))LogErr(L"Error releasing main D2D1 bmpHeroR !");

	for (int i = 0; i < 16; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing main D2D1 bmpIntro !");

	for (int i = 0; i < 20; ++i)if (!FreeMem(&bmpMeteor1[i]))LogErr(L"Error releasing main D2D1 bmpMeteor1 !");
	for (int i = 0; i < 20; ++i)if (!FreeMem(&bmpMeteor2[i]))LogErr(L"Error releasing main D2D1 bmpMeteor2 !");
	for (int i = 0; i < 27; ++i)if (!FreeMem(&bmpSupply[i]))LogErr(L"Error releasing main D2D1 bmpSupply !");
}
void ErrExit(int what)
{
	MessageBeep(MB_ICONERROR);
	MessageBox(NULL, ErrHandle(what), L"Критична гпешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	ClearResources();
	std::remove(tmp_file);
	exit(1);
}

void GameOver()
{
	KillTimer(bHwnd, bTimer);
	PlaySound(NULL, NULL, NULL);





	bMsg.message = WM_QUIT;
	bMsg.wParam = 0;
}
void InitGame()
{
	bTimer = 0;
	level = 1;
	score = 0;


	civs_saved = 0;

	distance = 600;

	wcscpy_s(current_player, L"TARLYO");
	name_set = false;

	need_field_left = false;
	need_field_right = false;

	need_ground_left = false;
	need_ground_right = false;

	if (Hero)
		if (!FreeMem(&Hero))LogErr(L"Error freeing memory for Hero !");

	if (!vFields.empty())for (int i = 0; i < vFields.size(); ++i)if (!FreeMem(&vFields[i]))
		LogErr(L"Error freeing memory for vFields element !");
	vFields.clear();

	if (!vGrounds.empty())for (int i = 0; i < vGrounds.size(); ++i)if (!FreeMem(&vGrounds[i]))
		LogErr(L"Error freeing memory for vGrounds element !");
	vGrounds.clear();

	if (!vCivilians.empty())for (int i = 0; i < vCivilians.size(); ++i)if (!FreeMem(&vCivilians[i]))
		LogErr(L"Error freeing memory for vCivilians element !");
	vCivilians.clear();

	if (!vEvils.empty())for (int i = 0; i < vEvils.size(); ++i)if (!FreeMem(&vEvils[i]))
		LogErr(L"Error freeing memory for vEvils element !");
	vEvils.clear();

	if (!vGuns.empty())for (int i = 0; i < vGuns.size(); ++i)if (!FreeMem(&vGuns[i]))
		LogErr(L"Error freeing memory for vGuns element !");
	vGuns.clear();

	if (!vBombs.empty())for (int i = 0; i < vBombs.size(); ++i)if (!FreeMem(&vBombs[i]))
		LogErr(L"Error freeing memory for vBombs element !");
	vBombs.clear();

	if (!vPowerups.empty())for (int i = 0; i < vPowerups.size(); ++i)if (!FreeMem(&vPowerups[i]))
		LogErr(L"Error freeing memory for vPowerups element !");
	vPowerups.clear();

	if (!vEvilShots.empty())for (int i = 0; i < vEvilShots.size(); ++i)if (!FreeMem(&vEvilShots[i]))
		LogErr(L"Error freeing memory for vEvilShots element !");
	vEvilShots.clear();

	vBonuses.clear();
	vExplosions.clear();

	for (float i = -scr_width; i < 2.0f * scr_width; i += scr_width)vFields.push_back(dll::FIELDS::create(assets::field, i, 50.0f));
	for (float i = -scr_width; i < 2.0f * scr_width; i += scr_width)
		vGrounds.push_back(dll::FIELDS::create(assets::ground, i, scr_height - 100.0f));

	Hero = dll::HERO::create(100.0f, scr_height / 2.0f);



}


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
		return true;

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDOK:
			if (GetDlgItemTextW(hwnd, IDC_NAME, current_player, 16) < 1)
			{
				wcscpy_s(current_player, L"TARLYO");

				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
				EndDialog(hwnd, IDCANCEL);
				break;
			}
			EndDialog(hwnd, IDOK);
			break;
		}
		break;
	}

	return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_CREATE:
		if (bIns)
		{
			SetTimer(hwnd, bTimer, 500, NULL);

			bBar = CreateMenu();
			bMain = CreateMenu();
			bStore = CreateMenu();

			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Меню за данни");

			AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
			AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
			AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bMain, MF_STRING, mExit, L"Изход");

			AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
			AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
			AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

			SetMenu(hwnd, bBar);

			InitGame();
		}
		break;

	case WM_CLOSE:
		pause = true;
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		if (MessageBox(hwnd, L"Ако излезеш, губиш тази игра !\n\nНаистина ли излизаш ?", L"Изход ?",
			MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
		{
			pause = false;
			break;
		}
		GameOver();
		break;

	case WM_PAINT:
		PaintDC = BeginPaint(hwnd, &bPaint);
		FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(10, 10, 10)));
		EndPaint(hwnd, &bPaint);
		break;

	case WM_TIMER:
		if (pause)break;
		--distance;
		if (!vBonuses.empty())for (int i = 0; i < vBonuses.size(); ++i)vBonuses[i].set_opacity();
		break;

	case WM_SETCURSOR:
		GetCursorPos(&cur_pos);
		ScreenToClient(hwnd, &cur_pos);
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (!in_client)
			{
				in_client = true;
				pause = false;
			}

			if (cur_pos.y * scale_y <= 50)
			{
				if (cur_pos.x * scale_x >= b1Rect.left && cur_pos.x * scale_x <= b1Rect.right)
				{
					if (!b1hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1hglt = true;
						b2hglt = false;
						b3hglt = false;
					}
				}
				else if (cur_pos.x * scale_x >= b2Rect.left && cur_pos.x * scale_x <= b2Rect.right)
				{
					if (!b2hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1hglt = false;
						b2hglt = true;
						b3hglt = false;
					}
				}
				else if (cur_pos.x * scale_x >= b3Rect.left && cur_pos.x * scale_x <= b3Rect.right)
				{
					if (!b3hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1hglt = false;
						b2hglt = false;
						b3hglt = true;
					}
				}
				else if (b1hglt || b2hglt || b3hglt)
				{
					if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
					b1hglt = false;
					b2hglt = false;
					b3hglt = false;
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1hglt || b2hglt || b3hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1hglt = false;
				b2hglt = false;
				b3hglt = false;
			}

			SetCursor(mainCursor);
			return true;
		}
		else
		{
			if (in_client)
			{
				in_client = false;
				pause = true;
			}

			if (b1hglt || b2hglt || b3hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1hglt = false;
				b2hglt = false;
				b3hglt = false;
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case mNew:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако рестартираш, губиш тази игра !\n\nНаистина ли рестартираш ?", L"Рестарт ?",
				MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			InitGame();
			break;

		case mLvl:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако прескочиш нивото, губиш бонусите му !\n\nНаистина ли прескачаш ?", L"Прескочи ниво ?",
				MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			//LevelUp();
			break;

		case mExit:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;

		}
		break;

	case WM_KEYDOWN:
		if (pause || !Hero)break;
		switch (LOWORD(wParam))
		{
		case VK_LEFT:
			if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
			Hero->dir = dirs::left;
			Hero->move((float)(level));
			break;

		case VK_RIGHT:
			if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
			Hero->dir = dirs::right;
			Hero->move((float)(level));
			break;

		case VK_UP:
			if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
			Hero->dir = dirs::up;
			Hero->move((float)(level));
			break;

		case VK_DOWN:
			if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
			Hero->dir = dirs::down;
			Hero->move((float)(level));
			break;

		case VK_SPACE:
			switch (Hero->dir)
			{
			case dirs::up:
				vBombs.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, Hero->center.x, scr_height, true));
				break;

			case dirs::down:
				vBombs.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, Hero->center.x, scr_height, true));
				break;

			case dirs::left:
				vBombs.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, Hero->center.x - 150.0f, scr_height, true));
				break;

			case dirs::right:
				vBombs.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, Hero->center.x + 150.0f, scr_height, true));
				break;
			}
			if (sound)mciSendString(L"play .\\res\\snd\\bomb.wav", NULL, NULL, NULL);
			vBombs.back()->damage = 50;
			break;

		case VK_SHIFT:
			switch (Hero->dir)
			{
			case dirs::up:
				vMyShots.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, scr_width, Hero->center.y, false));
				break;

			case dirs::down:
				vMyShots.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, scr_width, Hero->center.y, false));
				break;

			case dirs::left:
				vMyShots.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, 0, Hero->center.y, false));
				break;

			case dirs::right:
				vMyShots.push_back(dll::SHOTS::create(Hero->center.x, Hero->center.y, scr_width, Hero->center.y, false));
				break;
			}
			if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
			vMyShots.back()->damage = Hero->damage;
			break;
		}
		break;



	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return(LRESULT)(FALSE);
}

void CreateResources()
{
	int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f));
	int win_y = 100;

	int result = 0;

	if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) || GetSystemMetrics(SM_CYSCREEN) < win_y + (int)(scr_height))
		ErrExit(eScreen);
	CheckFile(Ltmp_file, &result);
	if (result == FILE_EXIST)ErrExit(eStarted);
	else
	{
		std::wofstream start{ Ltmp_file };
		start << L"Game started at: " << std::chrono::system_clock::now();
		start.clear();
	}

	mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE));
	if (!mainIcon)ErrExit(eIcon);
	mainCursor = LoadCursorFromFileW(L".\\res\\main.ani");
	outCursor = LoadCursorFromFileW(L".\\res\\out.ani");
	if (!mainCursor || !outCursor)ErrExit(eCursor);

	bWinClass.lpszClassName = bWinClassName;
	bWinClass.hInstance = bIns;
	bWinClass.lpfnWndProc = &WinProc;
	bWinClass.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	bWinClass.hIcon = mainIcon;
	bWinClass.hCursor = mainCursor;
	bWinClass.style = CS_DROPSHADOW;

	if (!RegisterClass(&bWinClass))ErrExit(eClass);

	bHwnd = CreateWindow(bWinClassName, L"Rescue in space mission !", WS_CAPTION | WS_SYSMENU, win_x, win_y,
		(int)(scr_width), (int)(scr_height), NULL, NULL, bIns, NULL);

	if (!bHwnd)ErrExit(eWindow);
	else
	{
		ShowWindow(bHwnd, SW_SHOWDEFAULT);

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
		if (hr != S_OK)
		{
			LogErr(L"Error creating D2D1Factory !");
			ErrExit(eD2D);
		}

		if (iFactory)
		{
			hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
				D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1HwndRenderTarget !");
				ErrExit(eD2D);
			}
		}
		if (Draw)
		{
			RECT clR{};
			GetClientRect(bHwnd, &clR);

			D2D1_SIZE_F hwndR{ Draw->GetSize() };

			scale_x = hwndR.width / (clR.right - clR.left);
			scale_y = hwndR.height / (clR.bottom - clR.top);

			D2D1_GRADIENT_STOP gSt[2]{};
			ID2D1GradientStopCollection* gColl{ nullptr };

			gSt[0].position = 0;
			gSt[0].color = D2D1::ColorF(D2D1::ColorF::MediumSlateBlue);
			gSt[1].position = 1.0f;
			gSt[1].color = D2D1::ColorF(D2D1::ColorF::MidnightBlue);

			hr = Draw->CreateGradientStopCollection(gSt, 2, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1GradientStopCollection !");
				ErrExit(eD2D);
			}

			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left +
					(b1Rect.right - b1Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2.0f,
					25.0f), gColl, &b1BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left +
					(b2Rect.right - b2Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2.0f,
					25.0f), gColl, &b2BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left +
					(b3Rect.right - b3Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2.0f,
					25.0f), gColl, &b3BckgBrush);
				if (hr != S_OK)
				{
					LogErr(L"Error creating D2D1Button Background brushes !");
					ErrExit(eD2D);
				}

				FreeMem(&gColl);
			}

			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkCyan), &statBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &txtBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &hgltBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::IndianRed), &inactBrush);

			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1TextBrushes !");
				ErrExit(eD2D);
			}

			bmpLogo = Load(L".\\res\\img\\logo.png", Draw);
			if (!bmpLogo)
			{
				LogErr(L"Error loading bmpLogo !");
				ErrExit(eD2D);
			}
			bmpLoose = Load(L".\\res\\img\\loose.png", Draw);
			if (!bmpLoose)
			{
				LogErr(L"Error loading bmpLoose !");
				ErrExit(eD2D);
			}
			bmpWin = Load(L".\\res\\img\\win.png", Draw);
			if (!bmpWin)
			{
				LogErr(L"Error loading bmpWin !");
				ErrExit(eD2D);
			}
			bmpRecord = Load(L".\\res\\img\\record.png", Draw);
			if (!bmpRecord)
			{
				LogErr(L"Error loading bmpRecord !");
				ErrExit(eD2D);
			}
			bmpLevel = Load(L".\\res\\img\\level.png", Draw);
			if (!bmpLevel)
			{
				LogErr(L"Error loading bmpLevel !");
				ErrExit(eD2D);
			}
			bmpBullet = Load(L".\\res\\img\\bullet.png", Draw);
			if (!bmpBullet)
			{
				LogErr(L"Error loading bmpBullet !");
				ErrExit(eD2D);
			}
			bmpField = Load(L".\\res\\img\\field.png", Draw);
			if (!bmpField)
			{
				LogErr(L"Error loading bmpField !");
				ErrExit(eD2D);
			}
			bmpGround = Load(L".\\res\\img\\ground.png", Draw);
			if (!bmpGround)
			{
				LogErr(L"Error loading bmpGround !");
				ErrExit(eD2D);
			}
			bmpPoints = Load(L".\\res\\img\\points.png", Draw);
			if (!bmpPoints)
			{
				LogErr(L"Error loading bmpPoints !");
				ErrExit(eD2D);
			}
			bmpRepair = Load(L".\\res\\img\\repair.png", Draw);
			if (!bmpRepair)
			{
				LogErr(L"Error loading bmpRepair !");
				ErrExit(eD2D);
			}
			bmpShield = Load(L".\\res\\img\\shield.png", Draw);
			if (!bmpShield)
			{
				LogErr(L"Error loading bmpShield !");
				ErrExit(eD2D);
			}
			bmpGun = Load(L".\\res\\img\\gun.png", Draw);
			if (!bmpGun)
			{
				LogErr(L"Error loading bmpGun !");
				ErrExit(eD2D);
			}
			bmpBomb = Load(L".\\res\\img\\bomb.png", Draw);
			if (!bmpBomb)
			{
				LogErr(L"Error loading bmpBomb !");
				ErrExit(eD2D);
			}

			for (int i = 0; i < 17; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\civil\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");
			
				bmpCivil[i] = Load(name, Draw);
				if (!bmpCivil[i])
				{
					LogErr(L"Error loading bmpCivil !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evil\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpEvilL[i] = Load(name, Draw);
				if (!bmpEvilL[i])
				{
					LogErr(L"Error loading bmpEvilL !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evil\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpEvilR[i] = Load(name, Draw);
				if (!bmpEvilR[i])
				{
					LogErr(L"Error loading bmpEvilR !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 24; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\Explosion\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpExplosion[i] = Load(name, Draw);
				if (!bmpExplosion[i])
				{
					LogErr(L"Error loading bmpExplosion !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\hero\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroL[i] = Load(name, Draw);
				if (!bmpHeroL[i])
				{
					LogErr(L"Error loading bmpHeroL !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\Hero\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroR[i] = Load(name, Draw);
				if (!bmpHeroR[i])
				{
					LogErr(L"Error loading bmpHeroR !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\Cannon\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpCannon[i] = Load(name, Draw);
				if (!bmpCannon[i])
				{
					LogErr(L"Error loading bmpCannon !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 16; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\intro\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpIntro[i] = Load(name, Draw);
				if (!bmpIntro[i])
				{
					LogErr(L"Error loading bmpIntro !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 20; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\meteor1\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpMeteor1[i] = Load(name, Draw);
				if (!bmpMeteor1[i])
				{
					LogErr(L"Error loading bmpMeteor1 !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 20; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\meteor2\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpMeteor2[i] = Load(name, Draw);
				if (!bmpMeteor1[2])
				{
					LogErr(L"Error loading bmpMeteor2 !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 27; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\supply\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpSupply[i] = Load(name, Draw);
				if (!bmpSupply[i])
				{
					LogErr(L"Error loading bmpSupply !");
					ErrExit(eD2D);
				}
			}
		}

		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
		if (hr != S_OK)
		{
			LogErr(L"Error creating D2D1WriteFactory !");
			ErrExit(eD2D);
		}
		if (iWriteFactory)
		{
			hr = iWriteFactory->CreateTextFormat(L"CASCADIA CODE", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &nrmFormat);
			hr = iWriteFactory->CreateTextFormat(L"CASCADIA CODE", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 28.0f, L"", &midFormat);
			hr = iWriteFactory->CreateTextFormat(L"CASCADIA CODE", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 64.0f, L"", &bigFormat);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1WriteTextFormats !");
				ErrExit(eD2D);
			}
		}
	}

	PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_ASYNC);

	for (int i = 0; i < 220; ++i)
	{
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpIntro[dll::IntroFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->DrawBitmap(bmpLogo, D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->EndDraw();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_wsetlocale(LC_ALL, L"");

	bIns = hInstance;
	if (!bIns)ErrExit(eClass);



	CreateResources();

	while (bMsg.message != WM_QUIT)
	{
		if ((bRet = PeekMessage(&bMsg, nullptr, NULL, NULL, PM_REMOVE)) != 0)
		{
			if (bRet == -1)ErrExit(eMsg);
			TranslateMessage(&bMsg);
			DispatchMessage(&bMsg);
		}

		if (pause)
		{
			if (show_help)continue;

			if (txtBrush && bigFormat)
			{
				Draw->BeginDraw();
				Draw->DrawBitmap(bmpIntro[dll::IntroFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
				Draw->DrawTextW(L"ПАУЗА", 6, bigFormat, D2D1::RectF(scr_width / 2.0f - 100.0f, scr_height / 2.0f - 50.0f,
					scr_width, scr_height), txtBrush);
				Draw->EndDraw();
				continue;
			}
		}

		//////////////////////////////////////////////////////////////////////

		if (Hero)
		{
			if (Hero->dir == dirs::right)nature_dir = dirs::left;
			else if (Hero->dir == dirs::left)nature_dir = dirs::right;
		}

		if (!vGrounds.empty())
		{
			for (std::vector<dll::FIELDS*>::iterator field = vGrounds.begin(); field < vGrounds.end(); ++field)
			{
				if (!(*field)->move(nature_dir, (float)(level)))
				{
					if (nature_dir == dirs::left)need_ground_right = true;
					else need_ground_left = true;
					(*field)->Release();
					vGrounds.erase(field);
					break;
				}
			}
		}
		if (need_ground_left)
		{
			float sx = (*vGrounds.begin())->start.x - scr_width;

			vGrounds.insert(vGrounds.begin(), dll::FIELDS::create(assets::ground,sx, scr_height - 100.0f));
			need_ground_left = false;
		}
		if (need_ground_right)
		{
			float sx = vGrounds.back()->end.x;
			vGrounds.push_back(dll::FIELDS::create(assets::ground, sx, scr_height - 100.0f));
			need_ground_right = false;
		}

		if (!vFields.empty())
		{
			for (std::vector<dll::FIELDS*>::iterator field = vFields.begin(); field < vFields.end(); ++field)
			{
				if (!(*field)->move(nature_dir, (float)(level)))
				{
					if (nature_dir == dirs::left)need_field_right = true;
					else need_field_left = true;
					(*field)->Release();
					vFields.erase(field);
					break;
				}
			}
		}
		if (need_field_left)
		{
			float sx = (*vFields.begin())->start.x - scr_width;
			vFields.insert(vFields.begin(), dll::FIELDS::create(assets::field, sx, 50.0f));
			need_field_left = false;
		}
		if (need_field_right)
		{
			float sx = vFields.back()->end.x;

			vFields.push_back(dll::FIELDS::create(assets::field, sx, 50.0f));
			need_field_right = false;
		}

		if (vCivilians.size() <= 3 && RandIt(0, 300) == 6)
			vCivilians.push_back(dll::ASSETS::create(assets::civilian, scr_width + RandIt(50.0f, 200.0f), scr_height - 140.0f));

		if (!vCivilians.empty())
		{
			for (std::vector<dll::ASSETS*>::iterator civil = vCivilians.begin(); civil < vCivilians.end(); ++civil)
			{
				if (!(*civil)->move(nature_dir, (float)(level)))
				{
					(*civil)->Release();
					vCivilians.erase(civil);
					break;
				}
			}
		}

		if (vGuns.size() <= 3 + level && RandIt(0, 500) == 66)
		{
			dll::GUN* a_gun = dll::GUN::create(scr_width + RandIt(10.0f, 100.0f), scr_height - 120.0f);

			bool is_ok = true;

			if (!vCivilians.empty())
			{
				for (int i = 0; i < vCivilians.size(); ++i)
				{
					FRECT GunR{ a_gun->start.x,a_gun->start.y,a_gun->end.x,a_gun->end.y };
					FRECT CivR{ vCivilians[i]->start.x,vCivilians[i]->start.y,vCivilians[i]->end.x,vCivilians[i]->end.y };

					if (dll::Intersect(GunR, CivR))
					{
						is_ok = false;
						break;
					}
				}
			}

			if (is_ok && !vGuns.empty())
			{
				for (int i = 0; i < vGuns.size(); ++i)
				{
					FRECT NewR{ a_gun->start.x,a_gun->start.y,a_gun->end.x,a_gun->end.y };
					FRECT GunR{ vGuns[i]->start.x,vGuns[i]->start.y,vGuns[i]->end.x,vGuns[i]->end.y };

					if (dll::Intersect(NewR, GunR))
					{
						is_ok = false;
						break;
					}
				}
			}

			if (is_ok)vGuns.push_back(a_gun);
		}

		if (vEvils.size() < 3 + level && RandIt(0, 400) == 33)
		{
			float tx{ RandIt(scr_width, scr_width + 300.0f) };
			float ty{ RandIt(sky, ground - 100.0f) };
		
			FRECT dummy{ tx,ty,tx + 120.0f,ty + 42.0f };

			bool ok = true;

			if (!vEvils.empty())
			{
				for (int i = 0; i < vEvils.size(); ++i)
				{
					FRECT evil{ vEvils[i]->start.x,vEvils[i]->start.y,vEvils[i]->end.x,vEvils[i]->end.y };
					if (dll::Intersect(dummy, evil))
					{
						ok = false;
						break;
					}
				}
			}

			if (ok)vEvils.push_back(dll::EVIL::create(tx, ty));
		}

		if (!vEvils.empty() && Hero)
		{
			for (std::vector<dll::EVIL*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				dll::BAG<FPOINT>bCivs(vCivilians.size());
				dll::BAG<FPOINT>bShots(vMyShots.size());
				dll::BAG<FPOINT>bPowerups(vPowerups.size());

				if (!vCivilians.empty())for (int i = 0; i < vCivilians.size(); ++i)bCivs.push_back(vCivilians[i]->center);
				if (!vMyShots.empty())for (int i = 0; i < vMyShots.size(); ++i)bShots.push_back(vMyShots[i]->center);
				if (!vPowerups.empty())for (int i = 0; i < vPowerups.size(); ++i)bPowerups.push_back(vPowerups[i]->center);

				if (!bCivs.empty())dll::Sort(bCivs, (*evil)->center);
				if (!bShots.empty())dll::Sort(bShots, (*evil)->center);
				if (!bPowerups.empty())dll::Sort(bPowerups, (*evil)->center);

				todo next_move = dll::AINextMove((*(*evil)), bCivs, bShots, bPowerups, Hero->center);

				if (next_move == todo::shoot)
				{
					int damage = (*evil)->attack();
					if (damage > 0)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
						vEvilShots.push_back(dll::SHOTS::create((*evil)->center.x, (*evil)->center.y,
							Hero->center.x, Hero->center.y, false));
						vEvilShots.back()->damage = damage;
					}
				}
				else
				{
					float ex = (*evil)->get_target_x();
					float ey = (*evil)->get_target_y();

					if (!(*evil)->move(ex, ey, (float)(level)))
					{
						(*evil)->Release();
						vEvils.erase(evil);
						break;
					}
				}
			}
		}

		if (!vGuns.empty())
		{
			for (std::vector<dll::GUN*>::iterator gun = vGuns.begin(); gun < vGuns.end(); ++gun)
			{
				if (!(*gun)->move(nature_dir, (float)(level)))
				{
					(*gun)->Release();
					vGuns.erase(gun);
					break;
				}
			}
		}

		if (!vGuns.empty() && Hero)
		{
			for (int i = 0; i < vGuns.size(); ++i)
			{
				if (dll::Distance(vGuns[i]->center, Hero->center) <= 300.0f)
				{
					int damage = vGuns[i]->attack();
					
					if (damage > 0)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
						vEvilShots.push_back(dll::SHOTS::create(vGuns[i]->center.x, vGuns[i]->center.y,
							Hero->center.x, Hero->center.y, false));
						vEvilShots.back()->damage = damage;
					}
				}
			}
		}

		if (!vEvilShots.empty())
		{
			for (std::vector<dll::SHOTS*>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); ++shot)
			{
				if (!(*shot)->move((float)(level)))
				{
					(*shot)->Release();
					vEvilShots.erase(shot);
					break;
				}
			}
		}

		if (!vMyShots.empty())
		{
			for (std::vector<dll::SHOTS*>::iterator shot = vMyShots.begin(); shot < vMyShots.end(); ++shot)
			{
				if (!(*shot)->move((float)(level)))
				{
					(*shot)->Release();
					vMyShots.erase(shot);
					break;
				}
			}
		}

		if (!vBombs.empty())
		{
			for (std::vector<dll::SHOTS*>::iterator shot = vBombs.begin(); shot < vBombs.end(); ++shot)
			{
				if (!(*shot)->move((float)(level)))
				{
					(*shot)->Release();
					vBombs.erase(shot);
					break;
				}
			}
		}

		if (Hero && !vCivilians.empty())
		{
			for (std::vector<dll::ASSETS*>::iterator civ = vCivilians.begin(); civ < vCivilians.end(); ++civ)
			{
				FRECT HeroR{ Hero->start.x,Hero->start.y,Hero->end.x,Hero->end.y };
				FRECT civR{ (*civ)->start.x,(*civ)->start.y,(*civ)->end.x,(*civ)->end.y };

				if (dll::Intersect(HeroR, civR))
				{
					if (sound)mciSendString(L"play .\\res\\snd\\warp.wav", NULL, NULL, NULL);
					score += 30 + level;
					(*civ)->Release();
					vCivilians.erase(civ);
					civs_saved++;
					break;
				}
			}
		}

		if (Hero && !vPowerups.empty())
		{
			for (std::vector<dll::ASSETS*>::iterator sup = vPowerups.begin(); sup < vPowerups.end(); ++sup)
			{
				FRECT HeroR{ Hero->start.x,Hero->start.y,Hero->end.x,Hero->end.y };
				FRECT SupR{ (*sup)->start.x,(*sup)->start.y,(*sup)->end.x,(*sup)->end.y };

				if (dll::Intersect(HeroR, SupR))
				{
					switch (RandIt(0, 3))
					{
					case 0:
						vBonuses.push_back(dll::BONUS{ bonus::armor,(*sup)->center.x,(*sup)->center.y });
						vBonuses.back().set_edges();
						Hero->armor++;
						if (sound)mciSendString(L"play .\\res\\snd\\upgrade.wav", NULL, NULL, NULL);
						break;

					case 1:
						vBonuses.push_back(dll::BONUS{ bonus::repair,(*sup)->center.x,(*sup)->center.y });
						vBonuses.back().set_edges();
						if (Hero->lifes + 50 <= 150)Hero->lifes += 50;
						else Hero->lifes = 150;
						if (sound)mciSendString(L"play .\\res\\snd\\lifes.wav", NULL, NULL, NULL);
						break;

					case 2:
						vBonuses.push_back(dll::BONUS{ bonus::gun,(*sup)->center.x,(*sup)->center.y });
						vBonuses.back().set_edges();
						Hero->damage += 5;
						if (sound)mciSendString(L"play .\\res\\snd\\upgrade.wav", NULL, NULL, NULL);
						break;

					case 3:
						vBonuses.push_back(dll::BONUS{ bonus::points,(*sup)->center.x,(*sup)->center.y });
						vBonuses.back().set_edges();
						score += 10 * level;
						if (sound)mciSendString(L"play .\\res\\snd\\upgrade.wav", NULL, NULL, NULL);
						break;

					}
					(*sup)->Release();
					vPowerups.erase(sup);
					break;
				}
			}
		}

		// BATTLE *************************************

		if (!vBombs.empty() && !vGuns.empty())
		{
			for (std::vector<dll::GUN*>::iterator gun = vGuns.begin(); gun < vGuns.end(); ++gun)
			{
				bool killed = false;

				for (std::vector<dll::SHOTS*>::iterator shot = vBombs.begin(); shot < vBombs.end(); ++shot)
				{
					if (dll::Intersect((*gun)->center, (*shot)->center, (*gun)->x_rad, (*shot)->x_rad,
						(*gun)->y_rad, (*shot)->y_rad))
					{
						(*gun)->lifes -= (*shot)->damage;
						(*shot)->Release();
						vBombs.erase(shot);

						if ((*gun)->lifes <= 0)
						{
							if (sound)mciSendString(L"play .\\res\\snd\\boom.wav", NULL, NULL, NULL);
							killed = true;
							score += 20 + level;
							vExplosions.push_back(EXPLOSION((*gun)->center.x, (*gun)->start.y));
							(*gun)->Release();
							vGuns.erase(gun);
						}

						break;
					}
				}

				if (killed)break;
			}
		}

		if (Hero && !vEvilShots.empty())
		{
			for (std::vector<dll::SHOTS*>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); ++shot)
			{
				if (dll::Intersect(Hero->center, (*shot)->center, Hero->x_rad, (*shot)->x_rad,
					Hero->y_rad, (*shot)->y_rad))
				{
					Hero->lifes -= ((*shot)->damage - Hero->armor);
					if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);
					(*shot)->Release();
					vEvilShots.erase(shot);
					if (Hero->lifes <= 0)
					{
						hero_killed = true;
						vExplosions.push_back(EXPLOSION(Hero->start.x, Hero->start.y));
						FreeMem(&Hero);
					}
					break;
				}
			}
		}

		if (!vEvils.empty() && !vMyShots.empty())
		{
			for (std::vector<dll::EVIL*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				bool killed = false;

				for (std::vector<dll::SHOTS*>::iterator shot = vMyShots.begin(); shot < vMyShots.end(); ++shot)
				{
					if (dll::Intersect((*evil)->center, (*shot)->center, (*evil)->x_rad, (*shot)->x_rad,
						(*evil)->y_rad, (*shot)->y_rad))
					{
						(*evil)->lifes -= ((*shot)->damage - (*evil)->armor);
						if (sound)mciSendString(L"play .\\res\\snd\\evildamage.wav", NULL, NULL, NULL);
						(*shot)->Release();
						vMyShots.erase(shot);

						if ((*evil)->lifes <= 0)
						{
							vExplosions.push_back(EXPLOSION((*evil)->center.x, (*evil)->center.y));
							if (RandIt(0, 3) == 1)vPowerups.push_back(dll::ASSETS::create(assets::supply, (*evil)->center.x,
								(*evil)->center.y));
							(*evil)->Release();
							vEvils.erase(evil);
							score += 10 + level;
							killed = true;
						}
						break;
					}
				}

				if (killed)break;
			}
		}

		if (!vEvils.empty() && !vBombs.empty())
		{
			for (std::vector<dll::EVIL*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				bool killed = false;

				for (std::vector<dll::SHOTS*>::iterator shot = vBombs.begin(); shot < vBombs.end(); ++shot)
				{
					if (dll::Intersect((*evil)->center, (*shot)->center, (*evil)->x_rad, (*shot)->x_rad,
						(*evil)->y_rad, (*shot)->y_rad))
					{
						(*evil)->lifes -= ((*shot)->damage - (*evil)->armor);
						if (sound)mciSendString(L"play .\\res\\snd\\evildamage.wav", NULL, NULL, NULL);
						(*shot)->Release();
						vBombs.erase(shot);

						if ((*evil)->lifes <= 0)
						{
							vExplosions.push_back(EXPLOSION((*evil)->center.x, (*evil)->center.y));
							if (RandIt(0, 3) == 1)vPowerups.push_back(dll::ASSETS::create(assets::supply, (*evil)->center.x,
								(*evil)->center.y));
							(*evil)->Release();
							vEvils.erase(evil);
							score += 10 + level;
							killed = true;
						}
						break;
					}
				}

				if (killed)break;
			}
		}

		// DRAW THINGS ***************************************************

		Draw->BeginDraw();

		if (!vFields.empty())
			for (int i = 0; i < vFields.size(); ++i)Draw->DrawBitmap(bmpField, D2D1::RectF(vFields[i]->start.x,
				vFields[i]->start.y, vFields[i]->end.x, vFields[i]->end.y));

		if (!vGrounds.empty())
			for (int i = 0; i < vGrounds.size(); ++i)Draw->DrawBitmap(bmpGround, D2D1::RectF(vGrounds[i]->start.x,
				vGrounds[i]->start.y, vGrounds[i]->end.x, vGrounds[i]->end.y));

		if (Hero)
		{
			dirs hero_show_dir{ dirs::stop };
			if (Hero->dir == dirs::right || Hero->dir == dirs::stop)hero_show_dir = dirs::right;
			else if (Hero->dir == dirs::left)hero_show_dir = dirs::left;

			if (hero_show_dir == dirs::left)
			{
				int frame = Hero->get_frame();
				Draw->DrawBitmap(bmpHeroL[frame], Resizer(bmpHeroL[frame], Hero->start.x, Hero->start.y));
			}
			else
			{
				int frame = Hero->get_frame();
				Draw->DrawBitmap(bmpHeroR[frame], Resizer(bmpHeroL[frame], Hero->start.x, Hero->start.y));
			}

			Draw->DrawLine(D2D1::Point2F(Hero->start.x - 2.0f, Hero->end.y + 2.0f),
				D2D1::Point2F(Hero->start.x - 2.0f + (float)(Hero->lifes / 1.5), Hero->end.y + 2.0f), txtBrush, 4.0f);
		}

		if (!vCivilians.empty())
		{
			for (int i = 0; i < vCivilians.size(); ++i)
			{
				int frame = vCivilians[i]->get_frame();
				Draw->DrawBitmap(bmpCivil[frame], Resizer(bmpCivil[frame], vCivilians[i]->start.x, vCivilians[i]->start.y));
			}
		}

		if (!vGuns.empty())
		{
			for (int i = 0; i < vGuns.size(); ++i)
			{
				int frame = vGuns[i]->get_frame();
				Draw->DrawBitmap(bmpCannon[frame], Resizer(bmpCannon[frame], vGuns[i]->start.x, vGuns[i]->start.y));
			}
		}

		if (!vEvilShots.empty())
		{
			for (int i = 0; i < vEvilShots.size(); ++i)
				Draw->DrawBitmap(bmpBullet, D2D1::RectF(vEvilShots[i]->start.x, vEvilShots[i]->start.y,
					vEvilShots[i]->end.x, vEvilShots[i]->end.y));
		}

		if (!vMyShots.empty())
		{
			for (int i = 0; i < vMyShots.size(); ++i)
				Draw->DrawBitmap(bmpBullet, D2D1::RectF(vMyShots[i]->start.x, vMyShots[i]->start.y,
					vMyShots[i]->end.x, vMyShots[i]->end.y));
		}

		if (!vBombs.empty())
		{
			for (int i = 0; i < vBombs.size(); ++i)
				Draw->DrawBitmap(bmpBomb, D2D1::RectF(vBombs[i]->start.x, vBombs[i]->start.y,
					vBombs[i]->end.x, vBombs[i]->end.y));
		}

		if (!vExplosions.empty())
		{
			for (std::vector<EXPLOSION>::iterator expl = vExplosions.begin(); expl < vExplosions.end(); ++expl)
			{
				--expl->frame_delay;
				if (expl->frame_delay <= 0)
				{
					expl->frame_delay = expl->max_frame_delay;
					++expl->frame;
					if (expl->frame > expl->max_frames)
					{
						vExplosions.erase(expl);
						break;
					}
				}
				Draw->DrawBitmap(bmpExplosion[expl->frame], Resizer(bmpExplosion[expl->frame], expl->x, expl->y));
			}
		}

		if (!vEvils.empty())
		{
			for (int i = 0; i < vEvils.size(); ++i)
			{
				int frame = vEvils[i]->get_frame();

				switch (vEvils[i]->dir)
				{
				case dirs::left:
					Draw->DrawBitmap(bmpEvilL[frame], Resizer(bmpEvilL[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case dirs::stop:
					Draw->DrawBitmap(bmpEvilL[frame], Resizer(bmpEvilL[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case dirs::right:
					Draw->DrawBitmap(bmpEvilR[frame], Resizer(bmpEvilR[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;
				}

				Draw->DrawLine(D2D1::Point2F(vEvils[i]->start.x, vEvils[i]->end.y + 2.0f),
					D2D1::Point2F(vEvils[i]->start.x + (float)(vEvils[i]->lifes / 1.5), vEvils[i]->end.y + 2.0f), txtBrush, 4.0f);
			}
		}

		if (!vPowerups.empty())
		{
			for (int i = 0; i < vPowerups.size(); ++i)
			{
				int frame = vPowerups[i]->get_frame();
				Draw->DrawBitmap(bmpSupply[frame], Resizer(bmpSupply[frame], vPowerups[i]->start.x, vPowerups[i]->start.y));
			}
		}

		if (!vBonuses.empty())
		{
			for (int i = 0; i < vBonuses.size(); ++i)
			{
				vBonuses[i].set_opacity();
				float opacity = vBonuses[i].opacity;
				switch (vBonuses[i].what)
				{
				case bonus::armor:
					Draw->DrawBitmap(bmpShield, D2D1::RectF(vBonuses[i].sx, vBonuses[i].sy, vBonuses[i].ex, vBonuses[i].ey),
						opacity);
					break;

				case bonus::gun:
					Draw->DrawBitmap(bmpGun, D2D1::RectF(vBonuses[i].sx, vBonuses[i].sy, vBonuses[i].ex, vBonuses[i].ey),
						opacity);
					break;

				case bonus::repair:
					Draw->DrawBitmap(bmpRepair, D2D1::RectF(vBonuses[i].sx, vBonuses[i].sy, vBonuses[i].ex, vBonuses[i].ey),
						opacity);
					break;

				case bonus::points:
					Draw->DrawBitmap(bmpPoints, D2D1::RectF(vBonuses[i].sx, vBonuses[i].sy, vBonuses[i].ex, vBonuses[i].ey),
						opacity);
					break;
				}

				if (opacity <= 0)
				{
					vBonuses.erase(vBonuses.begin() + i);
					break;
				}
			}
		}

		/////////////////////////

		if (nrmFormat && txtBrush && hgltBrush && inactBrush && statBrush && b1BckgBrush && b2BckgBrush && b3BckgBrush)
		{
			Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), statBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, 20.0f, 25.0f), b1BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, 20.0f, 25.0f), b2BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, 20.0f, 25.0f), b3BckgBrush);

			if (name_set)Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmFormat, b1txtRect, inactBrush);
			else
			{
				if (!b1hglt)Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmFormat, b1txtRect, txtBrush);
				else Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmFormat, b1txtRect, hgltBrush);
			}
			if (!b2hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2txtRect, txtBrush);
			else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2txtRect, hgltBrush);
			if (!b3hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3txtRect, txtBrush);
			else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3txtRect, hgltBrush);
		}

		Draw->EndDraw();
	
		if (vExplosions.empty() && hero_killed)GameOver();
	}

	ClearResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}