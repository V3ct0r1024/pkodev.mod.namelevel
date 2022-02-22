#include <Windows.h>
#include <detours.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "loader.h"

namespace pkodev
{
    namespace address
    {
        // Game.exe 0 (1222073761)
        namespace GAME_13X_0
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x00470690;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x0067052C;
        }

        // Game.exe 1 (1243412597)
        namespace GAME_13X_1
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x004706D0;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x0067061C;
        }

        // Game.exe 2 (1252912474)
        namespace GAME_13X_2
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x00470770;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x0067061C;
        }

        // Game.exe 3 (1244511158)
        namespace GAME_13X_3
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x00470760;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x0067061C;
        }

        // Game.exe 4 (1585009030)
        namespace GAME_13X_4
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x004707D0;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x00670634;
        }

        // Game.exe 5 (1207214236)
        namespace GAME_13X_5
        {
            // void CHeadSay::Render(D3DXVECTOR3& pos)
            const unsigned int CHeadSay__Render = 0x00470360;

            // CCharacter* CGameScene::_pMainCha 
            const unsigned int CGameScene___pMainCha = 0x00670534;
        }
    }

    namespace pointer
    {
        // void CHeadSay::Render(D3DXVECTOR3& pos)
        typedef void(__thiscall* CHeadSay__Render__Ptr)(void*, void*);
        CHeadSay__Render__Ptr CHeadSay__Render = (CHeadSay__Render__Ptr)(void*)(address::MOD_EXE_VERSION::CHeadSay__Render);
    }

    namespace hook
    {
        // void CHeadSay::Render(D3DXVECTOR3& pos)
        void __fastcall CHeadSay__Render(void* This, void* NotUsed, void* Pos);
    }

    // Name format
    std::string format{""};

    // Format order
    // true: level name
    // false: name level
    bool order = true;
}

// Trim a string
std::string trim(const std::string& str, const std::string& whitespace = " \t");

// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get mod information
void GetModInformation(mod_info& info)
{
    strcpy_s(info.name, TOSTRING(MOD_NAME));
    strcpy_s(info.version, TOSTRING(MOD_VERSION));
    strcpy_s(info.author, TOSTRING(MOD_AUTHOR));
    info.exe_version = MOD_EXE_VERSION;
}

// Start the mod
void Start(const char* path)
{
    // Build path to config file
    char buf[MAX_PATH]{ 0x00 };
    sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));

    // Open file
    std::ifstream file(buf, std::ifstream::in);

    // Check that file is open
    if ( file.is_open() == true )
    {
        // Read name format
        std::getline(file, pkodev::format);

        // Close file
        file.close();

        // Trim string
        pkodev::format = trim(pkodev::format);
    }

    // Format string
    auto format = [](std::string& str, bool& order) -> bool
    {
        // Placeholders
        const std::string ph_level{ "{:level:}" };
        const std::string ph_name{ "{:name:}" };

        // Make a copy of original string
        std::string copy{ str };

        // Check that string is not empty
        if ( copy.empty() == true )
        {
            return false;
        }

        // Search {:level:} placeholder
        const std::size_t pos_level = copy.find(ph_level);

        // Check that the placeholder is found
        if ( pos_level == std::string::npos )
        {
            // Placeholder is not found
            return false;
        }

        // Replace the placeholder with %d
        copy.replace(pos_level, ph_level.length(), "%d");

        // Search {:name:} placeholder
        const std::size_t pos_name = copy.find(ph_name);

        // Check that the placeholder is found
        if ( pos_name == std::string::npos )
        {
            return false;
        }

        // Replace the placeholder with %s
        copy.replace(pos_name, ph_name.length(), "%s");

        // Check the length of resulting string
        if ( copy.length() > 16 )
        {
            // String too long
            return false;
        }

        // String is successfully formatted
        str = copy;

        // Set order
        order = ( pos_level < pos_name );

        return true;
    };

    // Formating string
    if ( format(pkodev::format, pkodev::order) == false )
    {
        // Set default format
        pkodev::format = std::string("Lv%d %s");
        pkodev::order = true;
    }

    // Enable hooks
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pkodev::pointer::CHeadSay__Render, pkodev::hook::CHeadSay__Render);
    DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
    // Disable hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pkodev::pointer::CHeadSay__Render, pkodev::hook::CHeadSay__Render);
    DetourTransactionCommit();
}

// void CHeadSay::Render(D3DXVECTOR3& pos)
void __fastcall pkodev::hook::CHeadSay__Render(void* This, void* NotUsed, void* Pos)
{
    // Get pointer to the current character
    void* cha = reinterpret_cast<void*>(
        *reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned int>(This) + 0x28
        )
    );

    // Get the character's type
    unsigned int type = *reinterpret_cast<unsigned int*>(
        reinterpret_cast<unsigned int>(cha) + 0x0CD8
    );

    // Check character type
    if ( type == 1 )
    {
        // Get pointer to the player's character
        void* main_cha = reinterpret_cast<void*>(
            *reinterpret_cast<unsigned int*>(pkodev::address::MOD_EXE_VERSION::CGameScene___pMainCha)
        );

        // Check that character is not the player's character
        if ( cha != main_cha)
        {
            // Temporary buffer for original character's name
            static char tmp[33]{ 0x00 };

            // Get character level
            unsigned int level = *reinterpret_cast<unsigned int*>(
                reinterpret_cast<unsigned int>(cha) + 0x00D0
            );

            // Get character name
            char* name = reinterpret_cast<char*>(
                reinterpret_cast<unsigned int>(cha) + 0x041C
            );

            // Save old character name
            strcpy_s(tmp, sizeof(tmp), name);

            // Build new character name
            {
                if ( order == true ) 
                {
                    sprintf_s(name, sizeof(tmp), pkodev::format.c_str(), level, tmp);
                }
                else 
                {
                    sprintf_s(name, sizeof(tmp), pkodev::format.c_str(), tmp, level);
                }
            }
            
            // Render name
            pkodev::pointer::CHeadSay__Render(This, Pos);

            // Rollback name
            strcpy_s(name, sizeof(tmp), tmp);

            // Exit the hook
            return;
        }
    }

    // Call original CHeadSay::Render() function
    pkodev::pointer::CHeadSay__Render(This, Pos);
}

// Trim a string
std::string trim(const std::string& str, const std::string& whitespace)
{
    // Left trim
    auto ltrim = [](const std::string& str, const std::string& whitespace) -> std::string
    {
        // Looking for whitespaces at the beginning of a string
        const std::size_t pos = str.find_first_not_of(whitespace);

        // Remove spaces at the beginning of a string
        if (std::string::npos != pos)
        {
            return str.substr(pos);
        }

        return "";
    };

    // Right trim
    auto rtrim = [](const std::string& str, const std::string& whitespace) -> std::string
    {
        // Looking for whitespaces at the end of a string
        const std::size_t pos = str.find_last_not_of(whitespace);

        // Remove spaces at the end of a string
        if (std::string::npos != pos)
        {
            return str.substr(0, pos + 1);
        }

        return "";
    };

    return ltrim(rtrim(str, whitespace), whitespace);
}