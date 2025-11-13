/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Config.h"
#include "World.h"
#include "Player.h"
#include "WorldSession.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <curl/curl.h>
#include <chrono>

class DisconnectProtectionWorldScript : public WorldScript
{
public:
    DisconnectProtectionWorldScript() : WorldScript("DisconnectProtectionWorldScript") 
    {
        _lastCheckTime = std::chrono::steady_clock::now();
        _unreachableStartTime = std::chrono::steady_clock::time_point();
        _bothUnreachable = false;
        _protectionTriggered = false;
    }

    void OnUpdate(uint32 diff) override
    {
        if (!sConfigMgr->GetOption<bool>("DisconnectProtection.Enable", true))
            return;

        auto now = std::chrono::steady_clock::now();
        uint32 checkInterval = sConfigMgr->GetOption<uint32>("DisconnectProtection.CheckInterval", 1000);
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastCheckTime).count();
        
        if (elapsed < checkInterval)
            return;
            
        _lastCheckTime = now;
        
        // Check if both URLs are reachable
        bool redReachable = CheckURL("http://000.red");
        bool exampleReachable = CheckURL("http://example.com");
        
        if (!redReachable && !exampleReachable)
        {
            // Both are unreachable
            if (!_bothUnreachable)
            {
                // Just became unreachable
                _bothUnreachable = true;
                _unreachableStartTime = now;
                LOG_INFO("module", "DisconnectProtection: Both URLs are unreachable. Starting countdown...");
            }
            else
            {
                // Check if threshold has been reached
                auto unreachableDuration = std::chrono::duration_cast<std::chrono::seconds>(now - _unreachableStartTime).count();
                uint32 threshold = sConfigMgr->GetOption<uint32>("DisconnectProtection.UnreachableThreshold", 3);
                
                if (unreachableDuration >= threshold && !_protectionTriggered)
                {
                    LOG_WARN("module", "DisconnectProtection: Threshold reached! Casting Divine Intervention on all online players.");
                    CastDivineInterventionOnAllPlayers();
                    _protectionTriggered = true;
                }
            }
        }
        else
        {
            // At least one URL is reachable - reset
            if (_bothUnreachable)
            {
                LOG_INFO("module", "DisconnectProtection: Connection restored.");
            }
            _bothUnreachable = false;
            _protectionTriggered = false;
        }
    }

private:
    std::chrono::steady_clock::time_point _lastCheckTime;
    std::chrono::steady_clock::time_point _unreachableStartTime;
    bool _bothUnreachable;
    bool _protectionTriggered;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        return size * nmemb;
    }

    bool CheckURL(const std::string& url)
    {
        CURL* curl = curl_easy_init();
        if (!curl)
            return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        return (res == CURLE_OK);
    }

    void CastDivineInterventionOnAllPlayers()
    {
        const uint32 DIVINE_INTERVENTION_SPELL_ID = 19753;
        
        SessionMap const& sessions = sWorld->GetAllSessions();
        for (SessionMap::const_iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
        {
            if (Player* player = itr->second->GetPlayer())
            {
                if (player->IsInWorld() && player->IsAlive())
                {
                    // Cast Divine Intervention on the player
                    player->CastSpell(player, DIVINE_INTERVENTION_SPELL_ID, true);
                    LOG_DEBUG("module", "DisconnectProtection: Cast Divine Intervention on player {}", player->GetName());
                }
            }
        }
        
        LOG_INFO("module", "DisconnectProtection: Divine Intervention cast on all online players.");
    }
};

void AddDisconnectProtectionScripts()
{
    new DisconnectProtectionWorldScript();
}
