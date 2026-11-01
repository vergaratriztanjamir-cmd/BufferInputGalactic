
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>

#include "ModeSettings.hpp"
#include "Physics.hpp"
#include "InputBuffer.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

namespace Galactic {

    ModeSettings g_settings;
    InputBuffer g_inputBuffer;

    bool g_initialized = false;
    bool g_firstInputLogged = false;

    int g_lastMode[2] = {-1, -1};

    uint64_t getTimestampMs() {
        auto now = std::chrono::steady_clock::now();

        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count()
        );
    }

    GameMode detectMode(PlayerObject* player) {
        if (!player) return GameMode::Cube;

        if (player->m_isShip) return GameMode::Ship;
        if (player->m_isBall) return GameMode::Ball;
        if (player->m_isBird) return GameMode::UFO;
        if (player->m_isDart) return GameMode::Wave;
        if (player->m_isRobot) return GameMode::Robot;
        if (player->m_isSpider) return GameMode::Spider;
        if (player->m_isSwing) return GameMode::Swing;

        return GameMode::Cube;
    }

    const char* modeName(GameMode mode) {
        switch (mode) {
            case GameMode::Cube: return "Cube";
            case GameMode::Ship: return "Ship";
            case GameMode::Ball: return "Ball";
            case GameMode::UFO: return "UFO";
            case GameMode::Wave: return "Wave";
            case GameMode::Robot: return "Robot";
            case GameMode::Spider: return "Spider";
            case GameMode::Swing: return "Swing";
            default: return "Unknown";
        }
    }

    void initializeGalactic() {
        if (g_initialized) return;

        g_settings.loadFromGeode();
        g_initialized = true;

        log::info("Galactic initialized!");
        log::info("8 game modes loaded.");
        log::info("Dual-player profiles loaded.");
        log::info("Input recording initialized.");
        log::info("Game-mode detection initialized.");
    }
}

$on_mod(Loaded) {
    Galactic::initializeGalactic();
}

class $modify(GalacticGameLayer, GJBaseGameLayer) {

    static void onModify(auto& self) {
        auto hook = self.getHook("GJBaseGameLayer::handleButton");

        if (!hook) {
            log::error("GALACTIC: Hook registration FAILED!");
            return;
        }

        log::info("GALACTIC: Input hook registered!");
    }

    void handleButton(bool down, int button, bool isPlayer1) {

        if (down) {
            int playerNumber = isPlayer1 ? 1 : 2;

            PlayerObject* playerObject =
                isPlayer1 ? m_player1 : m_player2;

            if (playerObject) {
                auto mode = Galactic::detectMode(playerObject);
                int modeIndex = static_cast<int>(mode);

                Galactic::g_inputBuffer.push(
                    Galactic::getTimestampMs(),
                    playerNumber,
                    modeIndex,
                    button
                );

                int slot = playerNumber - 1;

                if (Galactic::g_lastMode[slot] != modeIndex) {
                    Galactic::g_lastMode[slot] = modeIndex;

                    log::info(
                        "GALACTIC: Player {} mode detected: {}",
                        playerNumber,
                        Galactic::modeName(mode)
                    );
                }

                if (!Galactic::g_firstInputLogged) {
                    Galactic::g_firstInputLogged = true;
                    log::info("GALACTIC: Input recorded!");
                }
            }
        }

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};
