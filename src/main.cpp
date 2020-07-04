#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/beatsaber-hook/include/modloader.hpp"
#include "../include/nix_tcp.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"
#include "../extern/BeatSaberQuestCustomUI/shared/customui.hpp"
#include "../extern/tcp_server_client-0.1/include/tcp_server.h"
#include "../extern/tcp_server_client-0.1/include/tcp_client.h"
#include "../extern/cista/cista.h"
#include "../extern/sha1/sha1.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <thread>
using namespace il2cpp_utils;

namespace cistadata = cista::raw;

struct ReplayLine { //ReplayLine And SongInfo Have To be the same size
    Vector3 rightPosition;
    Vector3 rightRotation;
    Vector3 leftPosition;
    Vector3 leftRotation;
    Vector3 headPosition;
    int score;
    int combo;
    float time;
    float energy;
    float rank;
};

struct SongInfo { //ReplayLine And SongInfo Have To be the same size
    bool batteryEnergy;
    bool disappearingArrows;
    bool noObstacles;
    bool noBombs;
    bool noArrows;
    bool slowerSong;
    bool noFail;
    bool instafail;
    bool ghostNotes;
    bool fasterSong;
    bool leftHanded;
    int difficulty;
    int mode;
    cistadata::string SongHash; 
};

struct Packet { // The packet size should be exactly 104!
    int type;
    cistadata::string data;
};


class CustomButton {
    public:
        Il2CppObject* gameObject = nullptr;
        Il2CppObject* TMP = nullptr;
        Il2CppObject* button = nullptr;
        Il2CppObject* buttonTransform = nullptr;
        Il2CppObject* parent = nullptr;
        Il2CppObject* parentTransform = nullptr;
        Il2CppObject* TMPLocalizer = nullptr;
        Il2CppObject* rectTransform = nullptr;
        Vector3 sizeDelta = {0, 0, 0};
        Vector3 scale = {1.0f, 1.0f, 1.0f};
        Vector3 rotation = {0, 0, 0};
        float fontSize = 10.0f;
        bool toggle = true;//Use this when creating toggles
        std::string text = "Custom Button UI";
        function_ptr_t<void> onPress;
 
        bool isCreated = false;
 
        void setParentAndTransform(Il2CppObject* Obj, int parentedAmount) {
            parent = Obj;
 
            if(parentedAmount < 1) {
                log("Parented amount has to be greater than 0, setting to 1...");
                parentedAmount = 1;
            }
            Il2CppObject* transform = *RunMethod(Obj, "get_transform");
            std::vector<Il2CppObject*> parents;
            Il2CppObject* firstParent = *RunMethod(transform, "GetParent");
            parents.push_back(firstParent);
            for(int i = 1; i < parentedAmount; i++) {
                Il2CppObject* otherParent = *RunMethod(parents[i-1], "GetParent");
                parents.push_back(otherParent);
            }
            parentTransform = parents[parents.size()-1];
        }
 
        void setParentTransform(Il2CppObject* Obj, int parentedAmount) {
            if(parentedAmount < 1) {
                log("Parented amount has to be greater than 0, setting to 1...");
                parentedAmount = 1;
            }
            Il2CppObject* transform = *RunMethod(Obj, "get_transform");
            std::vector<Il2CppObject*> parents;
            Il2CppObject* firstParent = *RunMethod(transform, "GetParent");
            parents.push_back(firstParent);
            for(int i = 1; i < parentedAmount; i++) {
                Il2CppObject* otherParent = *RunMethod(parents[i-1], "GetParent");
                parents.push_back(otherParent);
            }
            parentTransform = parents[parents.size()-1];
        }
 
        void create() {
            if(!isCreated && parent != nullptr && parentTransform != nullptr) {
                log("Custom Button: Creating an instance of the parent");
                Il2CppObject* button = CRASH_UNLESS(*il2cpp_utils::RunMethod("UnityEngine", "Object", "Instantiate", parent));
                log("Custom Button: Getting the transform");
                Il2CppObject* buttonTransform = CRASH_UNLESS(*il2cpp_utils::RunMethod(button, "get_transform"));
                log("Custom Button: Setting the parent transform");
                CRASH_UNLESS(il2cpp_utils::RunMethod(buttonTransform, "SetParent", parentTransform));
                log("Custom Button: Setting the local scale");
                CRASH_UNLESS(il2cpp_utils::RunMethod(buttonTransform, "set_localScale", scale));
                log("Custom Button: Setting the local position");
                CRASH_UNLESS(il2cpp_utils::RunMethod(buttonTransform, "set_localPosition", sizeDelta));
                log("Custom Button: Setting the euler angles");
                CRASH_UNLESS(il2cpp_utils::RunMethod(buttonTransform, "set_eulerAngles", rotation));
                log("Custom Button: Getting the game object");
                gameObject = CRASH_UNLESS(*il2cpp_utils::RunMethod(button, "get_gameObject"));
                log("Custom Button: Getting the TMProUGUI");
                TMP = CRASH_UNLESS(*il2cpp_utils::RunMethod(gameObject, "GetComponentInChildren", il2cpp_utils::GetSystemType("TMPro", "TextMeshProUGUI")));
                log("Custom Button: Getting the TMP Localizer");
                TMPLocalizer = *RunMethod<Il2CppObject*>(gameObject, "GetComponentInChildren", GetSystemType("Polyglot", "LocalizedTextMeshProUGUI"));
                log("Custom Button: Getting the rect transform");
                rectTransform = CRASH_UNLESS(*RunMethod(TMP, "get_rectTransform"));
                log("Custom Button: Setting the text");
                CRASH_UNLESS(il2cpp_utils::RunMethod(TMP, "set_text", il2cpp_utils::createcsstr(text)));
                log("Custom Button: Setting the font size");
                CRASH_UNLESS(il2cpp_utils::RunMethod(TMP, "set_fontSize", fontSize));
                log("Custom Button: Getting the onClick property");
                Il2CppObject* onClick = CRASH_UNLESS(*il2cpp_utils::GetPropertyValue(button, "onClick"));
                log("Custom Button: Creating the action");
                auto actionToRun = il2cpp_utils::MakeAction(il2cpp_functions::class_get_type(il2cpp_utils::GetClassFromName("UnityEngine.Events", "UnityAction")), (Il2CppObject*)nullptr, onPress);
                log("Custom Button: Setting the action to onClick");
                CRASH_UNLESS(il2cpp_utils::RunMethod(onClick, "AddListener", actionToRun));
                isCreated = true;
            } else {
                log("Button was already created or parent/parentTransform was null");
            }
        }
 
        bool setText(std::string newText) {
            if(gameObject != nullptr) {
                RET_0_UNLESS(RunMethod(TMP, "SetText", createcsstr(newText)));
                return true;
            }
            log("Game object is null, not setting text");
            return false;
        }
 
        bool setActive(bool isActive) {
            if(gameObject != nullptr) {
                RET_0_UNLESS(RunMethod(gameObject, "SetActive", isActive));
                return true;
            }
            log("Game object is null, not setting active");
            return false;
        }
 
        void setPos(Vector3 pos) {//Doesnt work yet, work on it more later
            il2cpp_utils::RunMethod(buttonTransform, "set_localPosition", pos);
        }
 
        void destroy() {
            if(gameObject != nullptr) {
                RunMethod("UnityEngine", "Object", "Destroy", gameObject);
                gameObject = nullptr;
                TMP = nullptr;
                parentTransform = nullptr;
                parent = nullptr;
                TMPLocalizer = nullptr;
                isCreated = false;
            } else {
                log("Button was already destroyed");
            }
        }
};

static CustomButton spectateButton;

CustomUI::TextObject spectateText;

static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration configuration(modInfo);
    return configuration;
}

Il2CppObject* playButton = nullptr;

SongInfo currentSongInfo {0};

size_t playingLevel = 0;

bool shouldSkip = false;

float skipTime = 0.0;

bool recording = true;

bool spectating = false;

static bool threadStarted = false;

TcpServer server;

TcpSocket sck(104); // Size = 104

bool waiting = false;
int offset = 0;
int spectatorPort = 0;
int serverPort = 0;
std::string spectatorIp;

void SocketThread() {
    pipe_ret_t startRet = server.start(serverPort);
    if (startRet.success) {
        while(1) {
            Client client = server.acceptClient(0);
            if (client.isConnected()) {
                log("A new client connected!");
                // TODO: REWORK AFTER NEW PACKET FORMAT
                // if (currentSongInfo.SongHash0 > 0) {
                //     std::vector<uint8_t> buf = cista::serialize(currentSongInfo);
                //     std::string songInfoStr(buf.begin(), buf.end());
                //     Packet packet{1, songInfoStr};
                //     std::vector<uint8_t> bufPacket = cista::serialize(packet);
                //     std::string packetStr(bufPacket.begin(), bufPacket.end());
                //     server.sendToClient(client, packetStr.c_str(), packetStr.length());
                // }
            }
        }
    }
}

std::string ssEnabled = "0";
bool firstTime = true;

void onDisconnection(const pipe_ret_t & ret) {
	log("Disconnected");
}

float songTime = 0.0f;
bool inSong = false;
bool inSongOrResults = false;

int score = 0;
int highScore;
std::string rank ="100.0%";
float rankFloat = 1;
float scoreMultiplier = 1.0f;

int combo = 0;

bool inPracticeMode;

float energy = 0.5f;

std::vector<Vector3> rightPositions;
std::vector<Vector3> rightRotations;
std::vector<Vector3> leftPositions;
std::vector<Vector3> leftRotations;
std::vector<Vector3> headPositions;
std::vector<int> scores;
std::vector<int> combos;
std::vector<float> times;
std::vector<float> energies;
std::vector<std::string> ranks;
std::vector<ReplayLine> replayLines; //TODO: Remove all other vectors and only use this one!
Il2CppObject* roomCenter = nullptr;

std::string stringToSave;

Il2CppObject* scoreUI;

int indexNum = 0;

std::string songHash;
int songDifficulty;
int mode;

bool batteryEnergy = false;
bool disappearingArrows = false;
bool noObstacles = false;
bool noBombs = false;
bool noArrows = false;
bool slowerSong = false;
bool noFail = false;
bool instafail = false;
bool ghostNotes = false;
bool fasterSong = false;
bool leftHanded = false;

int triggerNode;
float rTriggerVal;
float lTriggerVal;

bool playing = false;

int amountPerLine = 20;

int lineCount = 0;

std::string songName = "";

void handleReplayLine(ReplayLine *replayLineToHandle) {
    // replayLines.push_back(replayLineToHandle); //TODO: Remove all other vectors and only use this one!
    rightPositions.push_back(replayLineToHandle->rightPosition);
    rightRotations.push_back(replayLineToHandle->rightRotation);
    leftPositions.push_back(replayLineToHandle->leftPosition);
    leftRotations.push_back(replayLineToHandle->leftRotation);
    headPositions.push_back(replayLineToHandle->headPosition);
    if (times.size() < 1) {
        shouldSkip = true;
        skipTime = replayLineToHandle->time;
    }
    times.push_back(replayLineToHandle->time);
    scores.push_back(replayLineToHandle->score);
    combos.push_back(replayLineToHandle->combo);
    energies.push_back(replayLineToHandle->energy);
    //ranks.push_back(replayLineToHandle.rank);
}

void handleSongInfo(SongInfo *songInfoToHandle) {

    batteryEnergy = songInfoToHandle->batteryEnergy;
    disappearingArrows = songInfoToHandle->disappearingArrows;
    noObstacles = songInfoToHandle->noObstacles;
    noBombs = songInfoToHandle->noBombs;
    noArrows = songInfoToHandle->noArrows;
    slowerSong = songInfoToHandle->slowerSong;
    noFail = songInfoToHandle->noFail;
    instafail = songInfoToHandle->instafail;
    ghostNotes = songInfoToHandle->ghostNotes;
    fasterSong = songInfoToHandle->fasterSong;
    leftHanded = songInfoToHandle->leftHanded;
}

void onIncomingMsg(std::string msg) {
    auto packet = cista::deserialize<Packet>(msg);
    if (packet->type == 1) {
        auto songInfo = cista::deserialize<SongInfo>(packet->data);
        // TODO REWORK AFTER NEW PACKET FORMAT
        //playingLevel = songInfo->SongHash0;
        if (playingLevel > 0) {
            handleSongInfo(songInfo);
        }
    } else if (spectating) {
        auto replayLine = cista::deserialize<ReplayLine>(packet->data);
        handleReplayLine(replayLine);
        if(playButton != nullptr) {
            if (!playing) {
                playing = true;
            }
        }
        if (playing) {
            lineCount++;
        }
    }
}

void ClientThread() {
    sck.bind("4321");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (sck.connect(spectatorIp, std::to_string(spectatorPort))) {
        log("Connected!");
        while(sck.is_connected()) {
            auto data = sck.recv();
            if (data.size() > 0) {
                std::string currentPacket(data.begin(), data.end());
                onIncomingMsg(currentPacket);
            }
        }
    } else {
        log("Couldn't connect to player");
        return;
    }   
    
}

void spectateButtonOnClick() {
    rightPositions.clear();
    rightRotations.clear();
    leftPositions.clear();
    leftRotations.clear();
    headPositions.clear();
    scores.clear();
    times.clear();
    energies.clear();
    combos.clear();
    ranks.clear();
    lineCount = 0;

    batteryEnergy = false;
    disappearingArrows = false;
    noObstacles = false;
    noBombs = false;
    noArrows = false;
    slowerSong = false;
    noFail = false;
    instafail = false;
    ghostNotes = false;
    fasterSong = false;
    leftHanded = false;

    score = 0;
    combo = 0;
    energy = 0.5f;

    //TODO: Rework after new PACKET Format
    // if (playingLevel > 0) {
    //     std::hash<std::string> hash_fn;
    //     size_t hashToCheck = hash_fn(songHash);
    //     if (playingLevel == hashToCheck) {
    //         if(fileexists("sdcard/Android/data/com.beatgames.beatsaber/files/mods/libScoreSaber.so")) {
    //             setenv("disable_ss_upload", "1", true);
    //         }
    //         if(playButton != nullptr) {
    //             spectating = true;
    //             recording = false;
    //             log("pressing play");
    //             RunMethod(playButton, "Press");
    //         }
    //     }
    // }
}

float lerp(float a, float b, float t) {
    float newFloat = a + (t * (b - a));
    if(a > b) {
        if(newFloat > a) {
            newFloat = a;
        } else if(newFloat < b) {
            newFloat = b;
        }
    } else {
        if(newFloat < a) {
            newFloat = a;
        } else if(newFloat > b) {
            newFloat = b;
        }
    }
    return newFloat;
}

Vector3 lerpVectors(Vector3 a, Vector3 b, float t) {
    Vector3 newVector = {0, 0, 0};
    newVector.x = lerp(a.x, b.x, t);
    newVector.y = lerp(a.y, b.y, t);
    newVector.z = lerp(a.z, b.z, t);
    return a;
}

bool hasFakeMiss() {
    int amountCheckingEachSide = 2;
    int biggestCombo = 0;
    for(int i = -amountCheckingEachSide; i < (amountCheckingEachSide*2)+1; i++) {
        if(combos[indexNum+i] <= 1) {
            return false;
        }
    }
    return true;
}

SongInfo getSongInfo() {
    unsigned char hash[20];
    std::transform(songHash.begin(), songHash.end(),songHash.begin(), ::toupper); //Making sure the song LEVELID is in uppercase on both PC and Quest for easier searching
    sha1::calc(songHash.c_str(),songHash.length(),hash);
    std::string songSha1( hash, hash + sizeof hash / sizeof hash[0] ); //Using SHA1 allows me to make sure the Hash is always the same size for packet padding.
    char hexstring[41];
    sha1::toHexString(hash, hexstring);

    SongInfo songInfo{batteryEnergy, disappearingArrows, noObstacles, noBombs, noArrows, slowerSong, noFail, instafail, ghostNotes, fasterSong, leftHanded, songDifficulty, mode, songSha1 + "00000000000000000000"}; // the 0's are for Padding the SongInfo to 104B, TODO implement it in the struct directly
    return songInfo;
}

MAKE_HOOK_OFFSETLESS(SceneManager_SetActiveScene, bool, Scene scene) {
    if (!threadStarted) {
        threadStarted = true;
        std::thread socketThread(SocketThread); //Starting Server
        socketThread.detach();
        std::thread clientThread(ClientThread); // Starting Client
        clientThread.detach();
    }
    return SceneManager_SetActiveScene(scene);
}

MAKE_HOOK_OFFSETLESS(PlayerController_Update, void, Il2CppObject* self) {
    // When Playing
    if(recording) {
        Il2CppObject* leftSaber = *il2cpp_utils::GetFieldValue(self, "_leftSaber");
        Il2CppObject* rightSaber = *il2cpp_utils::GetFieldValue(self, "_rightSaber");

        if(leftSaber != nullptr && rightSaber != nullptr) {
            Il2CppObject* leftSaberTransform = nullptr;
            Il2CppObject* rightSaberTransform = nullptr;

            Il2CppClass* componentsClass = il2cpp_utils::GetClassFromName("", "Saber");
            leftSaberTransform = *il2cpp_utils::RunMethod(leftSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));
            rightSaberTransform = *il2cpp_utils::RunMethod(rightSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));

            if(leftSaberTransform != nullptr && rightSaberTransform != nullptr) {
                Vector3 rightPos = *RunMethod<Vector3>(rightSaberTransform, "get_position");
                Vector3 rightRot = *RunMethod<Vector3>(rightSaberTransform, "get_eulerAngles");
                Vector3 leftPos = *RunMethod<Vector3>(leftSaberTransform, "get_position");
                Vector3 leftRot = *RunMethod<Vector3>(leftSaberTransform, "get_eulerAngles");
                Vector3 headPos = *GetFieldValue<Vector3>(self, "_headPos");
                ReplayLine replayLine{rightPos, rightRot, leftPos, leftRot,headPos, score, combo, songTime, energy, rankFloat}; //Initiating a replay line with current values
                std::vector<uint8_t> buf = cista::serialize(replayLine);
                std::string replayLineStr(buf.begin(), buf.end());
                Packet packet{0, replayLineStr}; //Making a ReplayLine Packet
                std::vector<uint8_t> bufPacket = cista::serialize(packet);
                std::string packetStr(bufPacket.begin(), bufPacket.end());
                server.sendToAllClients(packetStr.c_str(), packetStr.length()); //Sending the ReplayLine Packet to spectators
            }
        }
    }

    PlayerController_Update(self);
    

    // When Spectating
    if(!recording) {
        bool foundCorrectIndex = false;
        while(!foundCorrectIndex) {
            if(indexNum < times.size()-1) {
                if(times[indexNum+offset] > songTime) {
                    foundCorrectIndex = true;
                } else if(indexNum < times.size()) {
                    indexNum++;
                }
            } else {
                foundCorrectIndex = true;
            }
        }
        
        float lerpAmount = 1 - (((times[indexNum+1] - songTime) / (times[indexNum+1] - times[indexNum])) - 1);
        if(lerpAmount > 1) {
            lerpAmount = 1;
        } else if(lerpAmount < 0) {
            lerpAmount = 0;
        }

        Il2CppObject* leftSaber = *il2cpp_utils::GetFieldValue(self, "_leftSaber");
        Il2CppObject* rightSaber = *il2cpp_utils::GetFieldValue(self, "_rightSaber");

        if(leftSaber != nullptr && rightSaber != nullptr) {
            Il2CppObject* leftSaberTransform = nullptr;
            Il2CppObject* rightSaberTransform = nullptr;

            Il2CppClass* componentsClass = il2cpp_utils::GetClassFromName("", "Saber");
            leftSaberTransform = *il2cpp_utils::RunMethod(leftSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));
            rightSaberTransform = *il2cpp_utils::RunMethod(rightSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));
            if(leftSaberTransform != nullptr && rightSaberTransform != nullptr) {
                CRASH_UNLESS(RunMethod(rightSaberTransform, "set_position", lerpVectors(rightPositions[indexNum], rightPositions[indexNum], lerpAmount)));
                CRASH_UNLESS(RunMethod(rightSaberTransform, "set_eulerAngles", lerpVectors(rightRotations[indexNum], rightRotations[indexNum], lerpAmount)));
                CRASH_UNLESS(RunMethod(leftSaberTransform, "set_position", lerpVectors(leftPositions[indexNum], leftPositions[indexNum+1], lerpAmount)));
                CRASH_UNLESS(RunMethod(leftSaberTransform, "set_eulerAngles", lerpVectors(leftRotations[indexNum], leftRotations[indexNum+1], lerpAmount)));
                CRASH_UNLESS(SetFieldValue(self, "_headPos", lerpVectors(headPositions[indexNum], headPositions[indexNum+1], lerpAmount)));
            }
        }
    }
}

MAKE_HOOK_OFFSETLESS(SongUpdate, void, Il2CppObject* self) {
    // When Spectating
    if (!recording) {
        float lastTime = times[times.size() - 1];
        float delay = lastTime - songTime;
        float speed = 1.00;
        float nullSpeed = 0.00;
        float minDelay = 0.50;
        float safeDelay = 3.00;
        int pauseCheck = times.size() - indexNum;
        Il2CppObject* audioSource = *GetFieldValue(self, "_audioSource");
        if (playingLevel > 0 && delay < minDelay) {
            SetFieldValue(self, "_timeScale", nullSpeed);
            RunMethod(audioSource, "set_pitch", nullSpeed);
            waiting = true;
        } 
        if (waiting && delay > safeDelay) {
            SetFieldValue(self, "_timeScale", speed);
            RunMethod(audioSource, "set_pitch", speed);
            waiting = false;
        } else if (playingLevel == 0) {
            SetFieldValue(self, "_timeScale", speed);
            RunMethod(audioSource, "set_pitch", speed);
            waiting = false;
        }
    }
    
    SongUpdate(self);
    songTime = *il2cpp_utils::GetFieldValue<float>(self, "_songTime");
}

MAKE_HOOK_OFFSETLESS(SongAudioStart, void, Il2CppObject* self) {
    // To skip song to the first ReplayLine data received, for close to no delay.
    if (shouldSkip) {
        shouldSkip = false;
        Il2CppObject* initData = CRASH_UNLESS(*GetFieldValue(self, "_initData"));
        SetFieldValue(initData, "startSongTime", skipTime);
    }
    SongAudioStart(self);
}

MAKE_HOOK_OFFSETLESS(SongStart, void, Il2CppObject* self, Il2CppObject* difficultyBeatmap, Il2CppObject* overrideEnvironmentSettings, Il2CppObject* overrideColorScheme, Il2CppObject* gameplayModifiers, Il2CppObject* playerSpecificSettings, Il2CppObject* practiceSettings, Il2CppString* backButtonText, bool useTestNoteCutSoundEffects) {

    energy = 0.5f;
    inSong = true;
    inSongOrResults = true;
    indexNum = 0;
    
    // When Playing
    if(recording) {
        stringToSave = "";

        batteryEnergy = *RunMethod<bool>(gameplayModifiers, "get_batteryEnergy");

        disappearingArrows = *RunMethod<bool>(gameplayModifiers, "get_disappearingArrows");

        ghostNotes = *RunMethod<bool>(gameplayModifiers, "get_ghostNotes");

        instafail = *RunMethod<bool>(gameplayModifiers, "get_instaFail");

        noArrows = *RunMethod<bool>(gameplayModifiers, "get_noArrows");

        noBombs = *RunMethod<bool>(gameplayModifiers, "get_noBombs");

        noFail = *RunMethod<bool>(gameplayModifiers, "get_noFail");

        noObstacles = *RunMethod<bool>(gameplayModifiers, "get_noObstacles");

        int songSpeedLevel = *RunMethod<int>(gameplayModifiers, "get_songSpeed");

        slowerSong = false;
        fasterSong = false;
        if(songSpeedLevel == 1) {
            fasterSong = true;
        } else if(songSpeedLevel == 2) {
            slowerSong = true;
        }

        leftHanded = *RunMethod<bool>(playerSpecificSettings, "get_leftHanded");
        SongInfo songInfo = getSongInfo();
        currentSongInfo = songInfo;

        std::vector<uint8_t> buf = cista::serialize(songInfo);
        std::string songInfoStr(buf.begin(), buf.end());
        Packet packet{1, songInfoStr}; // Making a SongInfo Packet
        std::vector<uint8_t> bufPacket = cista::serialize(packet);
        std::string packetStr(bufPacket.begin(), bufPacket.end());
        stringToSave = stringToSave + packetStr;
        server.sendToAllClients(packetStr.c_str(), packetStr.length()); //Sending the SongInfo Packet to spectators
    } else { // When Spectating
        RunMethod(gameplayModifiers, "set_batteryEnergy", batteryEnergy);
        RunMethod(gameplayModifiers, "set_disappearingArrows", disappearingArrows);
        RunMethod(gameplayModifiers, "set_ghostNotes", ghostNotes);
        RunMethod(gameplayModifiers, "set_instaFail", instafail);
        RunMethod(gameplayModifiers, "set_noArrows", noArrows);
        RunMethod(gameplayModifiers, "set_noBombs", noBombs);
        RunMethod(gameplayModifiers, "set_noFail", noFail);
        RunMethod(gameplayModifiers, "set_noObstacles", noObstacles);
        RunMethod(playerSpecificSettings, "set_leftHanded", leftHanded);
    }

    SongStart(self, difficultyBeatmap, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
}

MAKE_HOOK_OFFSETLESS(SongEnd, void, Il2CppObject* self, Il2CppObject* levelCompleteionResults) {
    
    log("SongEnd");
    shouldSkip = false;
    if (recording) {
        SongInfo songInfo{};
        currentSongInfo = songInfo;
        std::vector<uint8_t> buf = cista::serialize(songInfo);
        std::string songInfoStr(buf.begin(), buf.end());
        Packet packet{1, songInfoStr};
        std::vector<uint8_t> bufPacket = cista::serialize(packet);
        std::string packetStr(bufPacket.begin(), bufPacket.end());
        //server.sendToAllClients(packetStr.c_str(), packetStr.length());  //This line is ment for Quest to Quest spectating, commented because of conflict with PC for now
    }
    if(!recording && fileexists("sdcard/Android/data/com.beatgames.beatsaber/files/mods/libScoreSaber.so")) {
        setenv("disable_ss_upload", "1", true);
    }

    spectating = false;
    playing = false;

    inSong = false;

    if(spectateText.gameObj != nullptr) {
        log("Destroying spectate text");
        spectateText.destroy();
    }

    int levelEndState = *GetFieldValue<int>(levelCompleteionResults, "levelEndStateType");
    
    SongEnd(self, levelCompleteionResults);

    log("Song has successfully ended");
}

MAKE_HOOK_OFFSETLESS(ScoreChanged, void, Il2CppObject* self, int rawScore, int modifiedScore) {
    // When Playing
    if(!recording) {
        rawScore = scores[indexNum];
        modifiedScore = rawScore * scoreMultiplier;
    }
 
    ScoreChanged(self, rawScore, modifiedScore);
}

MAKE_HOOK_OFFSETLESS(RefreshContent, void, Il2CppObject* self) {
    
    log("Refreshing Content");

    RefreshContent(self);

    playButton = *GetFieldValue(self, "_playButton");

    if(!inSongOrResults) {
        recording = true;

        if(fileexists("sdcard/Android/data/com.beatgames.beatsaber/files/mods/libScoreSaber.so")) {
            log("Score saber is loaded");
            setenv("disable_ss_upload", "0", true);
        }
    }

    Il2CppObject* Level = CRASH_UNLESS(*GetFieldValue(self, "_level"));
    Il2CppString* LevelID = CRASH_UNLESS(*GetPropertyValue<Il2CppString*>(Level, "levelID"));

    Il2CppObject* SelectedBeatmapDifficulty = CRASH_UNLESS(*GetFieldValue(self, "_selectedDifficultyBeatmap"));
    int Difficulty = *GetPropertyValue<int>(SelectedBeatmapDifficulty, "difficulty");
    Il2CppObject* beatMapData = CRASH_UNLESS(*GetPropertyValue(SelectedBeatmapDifficulty, "beatmapData"));
    Il2CppObject* parentDifficultyBeatmapSet = CRASH_UNLESS(*GetPropertyValue(SelectedBeatmapDifficulty, "parentDifficultyBeatmapSet"));
    Il2CppObject* beatmapCharacteristic = CRASH_UNLESS(*GetPropertyValue(parentDifficultyBeatmapSet, "beatmapCharacteristic"));
    std::string modeName = to_utf8(csstrtostr(*GetFieldValue<Il2CppString*>(beatmapCharacteristic, "_compoundIdPartName")));
    log("Mode name is "+modeName);

    Il2CppObject* PlayerData = CRASH_UNLESS(*GetFieldValue(self, "_playerData"));
    Il2CppObject* playerLevelStatsData = CRASH_UNLESS(*RunMethod(PlayerData, "GetPlayerLevelStatsData", LevelID, Difficulty, beatmapCharacteristic));
    highScore = *GetPropertyValue<int>(playerLevelStatsData, "highScore");
    log("Highscore is "+std::to_string(highScore));

    mode = 0;
    if (modeName == "OneSaber") {
        mode = 1;
    } else if (modeName == "NoArrows") {
        mode = 2;
    } else if (modeName == "360Degree") {
        mode = 3;
    } else if (modeName == "90Degree") {
        mode = 4;
    }
    songHash = to_utf8(csstrtostr(LevelID));
    songDifficulty = Difficulty;

    spectateButton.destroy();

    Il2CppObject* songNameText = *GetFieldValue(self, "_songNameText");
    songName = to_utf8(csstrtostr(*RunMethod<Il2CppString*>(songNameText, "get_text")));

    if (playingLevel > 0) {
        std::hash<std::string> hash_fn;
        size_t hashToCheck = hash_fn(songHash);
        if (playingLevel == hashToCheck) {
            log("Making Spectate button");
            spectateButton.setParentAndTransform(playButton, 1);
            spectateButton.onPress = spectateButtonOnClick;
            spectateButton.scale = {1, 1, 1};
            spectateButton.fontSize = 5;
            spectateButton.create();
            if(spectateButton.TMPLocalizer != nullptr) {
                RunMethod("UnityEngine", "Object", "Destroy", spectateButton.TMPLocalizer);
            }
            spectateButton.setText("Spectate");
        }
    } else {
       log("Not making Spectate button");
    }

    log("Song name is "+songName);
}

MAKE_HOOK_OFFSETLESS(LevelSelectionFlowCoordinator_StartLevel, void, Il2CppObject* self, Il2CppObject* difficultyBeatmap, Il2CppObject* beforeSceneSwitchCallback, bool practice) {
    
    log("StartLevel");

    inPracticeMode = practice;
    LevelSelectionFlowCoordinator_StartLevel(self, difficultyBeatmap, beforeSceneSwitchCallback, practice);
}

MAKE_HOOK_OFFSETLESS(EnergyBarUpdate, void, Il2CppObject* self, int value) {

    if(!recording) {
        value = 0;
    }

    EnergyBarUpdate(self, value);

    if(recording) {
        energy = *RunMethod<float>(self, "get_energy");
    } else {
        if(energies[indexNum] < 0.1f) {
            CRASH_UNLESS(RunMethod(self, "set_energy", 0.1f));
        } else {
            CRASH_UNLESS(RunMethod(self, "set_energy", energies[indexNum]));
        }
    }
}

MAKE_HOOK_OFFSETLESS(ScoreControllerLateUpdate, void, Il2CppObject* self) {

    ScoreControllerLateUpdate(self);

    scoreMultiplier = *GetFieldValue<float>(self, "_gameplayModifiersScoreMultiplier");

    if(indexNum > 2 && !recording) {
        SetFieldValue(self, "_baseRawScore", scores[indexNum]);
        SetFieldValue(self, "_prevFrameRawScore", scores[indexNum-1]);
        SetFieldValue(self, "_combo", combos[indexNum]);
    }
    if(recording) {
        score = *GetFieldValue<int>(self, "_baseRawScore");
        combo = *GetFieldValue<int>(self, "_combo");
    }
}

MAKE_HOOK_OFFSETLESS(RefreshRank, void, Il2CppObject* self) {

    RefreshRank(self);

    if(recording) {
        rankFloat = *GetFieldValue<float>(self, "_prevRelativeScore");
    }
}

MAKE_HOOK_OFFSETLESS(Triggers, void, Il2CppObject* self, int node) {
    triggerNode = node;

    Triggers(self, node);
}

MAKE_HOOK_OFFSETLESS(ControllerUpdate, void, Il2CppObject* self) {

    float trigger = *RunMethod<float>(self, "get_triggerValue");

    if (triggerNode == 4) {
        lTriggerVal = trigger;
    }
    if (triggerNode == 5) {
        rTriggerVal = trigger;
    }
    ControllerUpdate(self);
}

MAKE_HOOK_OFFSETLESS(ProgressUpdate, void, Il2CppObject* self) {
    // When spectating
    if(!recording) {
        std::string textToSetTo = "Spectating "+songName;

        if(spectateText.gameObj == nullptr && inSong) {
            log("Making spectateText");
            Il2CppObject* slider = *il2cpp_utils::GetFieldValue(self, "_slider");
            Il2CppObject* sliderTransform = *il2cpp_utils::RunMethod(slider, "get_transform");
            Il2CppObject* sliderParent = *il2cpp_utils::RunMethod(sliderTransform, "GetParent");
    
            spectateText.text = textToSetTo;
            spectateText.fontSize = 12.0f;
            spectateText.parentTransform = sliderParent;
            spectateText.sizeDelta = {-400, 100};
            spectateText.anchoredPosition = {-400, 100};
            spectateText.create();
        } else {
            if(inSong) {
                spectateText.set(textToSetTo);
            }
        }
    }
    ProgressUpdate(self);
}

MAKE_HOOK_OFFSETLESS(PauseMenuManager_MenuButtonPressed, void, Il2CppObject* self) {
    PauseMenuManager_MenuButtonPressed(self);

    inSongOrResults = false;
}

MAKE_HOOK_OFFSETLESS(ResultsScreenEnd, void, Il2CppObject* self, int deactivationType) {

    inSongOrResults = false;

    log("Results screen has Ended");

    ResultsScreenEnd(self, deactivationType);
}

MAKE_HOOK_OFFSETLESS(NoteWasMissed, void, Il2CppObject* self) {
    if(!recording && hasFakeMiss()) {
        return;
    }

    NoteWasMissed(self);
}

MAKE_HOOK_OFFSETLESS(NoteWasCut, void, Il2CppObject* self, Il2CppObject* noteCutInfo) {
    bool allIsOk = *RunMethod<bool>(noteCutInfo, "get_allIsOK");

    if(!recording && !allIsOk && hasFakeMiss()) {
        return;
    }

    NoteWasCut(self, noteCutInfo);
}

MAKE_HOOK_OFFSETLESS(VRCenterAdjust_Enable, void, Il2CppObject* self) {
    Vector3 newCenter = {0, 0, -3.0f};
    if(!inSong || !spectating) {
        newCenter.z = 0;
    }
    Il2CppObject* currentRoomCenter = *GetFieldValue(self, "_roomCenter");
    il2cpp_utils::RunMethod(currentRoomCenter, "set_value", newCenter);
    VRCenterAdjust_Enable(self);
}

void SaveConfig() {
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("spectatorIp", "111.111.111.111", allocator);
    getConfig().config.AddMember("spectatorPort", 65123, allocator);
    getConfig().config.AddMember("serverPort", 65122, allocator);
    getConfig().Write();
}   

bool LoadConfig() {
    getConfig().Load();
    //return false;
    bool foundEverything = true;
    if (getConfig().config.HasMember("spectatorIp") && getConfig().config["spectatorIp"].IsString()) {
        spectatorIp = getConfig().config["spectatorIp"].GetString();
    }
    else {
        foundEverything = false;
    }
    if (getConfig().config.HasMember("spectatorPort") && getConfig().config["spectatorPort"].IsInt()) {
        spectatorPort = getConfig().config["spectatorPort"].GetInt();
    }
    else {
        foundEverything = false;
    }
    if (getConfig().config.HasMember("serverPort") && getConfig().config["serverPort"].IsInt()) {
        serverPort = getConfig().config["serverPort"].GetInt();
    }
    else {
        foundEverything = false;
    }
    if (foundEverything) {
        return true;
    }
    return false;
}

extern "C" void setup(ModInfo& info) {
    info.id = "Spectator";
    info.version = "0.1.2";
    modInfo = info;
    static std::unique_ptr<const Logger> ptr(new Logger(info));
    Logger::get().info("Completed setup!");
    Logger::get().info("Modloader name: %s", Modloader::getInfo().name.c_str());
    getConfig();
    getConfig().Load();
    getConfig().Write();
}

extern "C" void load() {
    if(!LoadConfig())
    {
        SaveConfig();
    }
    Logger::get().info("Installing hooks...");
    INSTALL_HOOK_OFFSETLESS(SongUpdate, il2cpp_utils::FindMethodUnsafe("", "AudioTimeSyncController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(SongAudioStart, il2cpp_utils::FindMethodUnsafe("", "AudioTimeSyncController", "StartSong", 0));
    INSTALL_HOOK_OFFSETLESS(SongStart, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 8));
    INSTALL_HOOK_OFFSETLESS(SongEnd, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Finish", 1));
    INSTALL_HOOK_OFFSETLESS(PlayerController_Update, il2cpp_utils::FindMethodUnsafe("", "PlayerController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(ScoreChanged, il2cpp_utils::FindMethodUnsafe("", "ScoreUIController", "UpdateScore", 2));
    INSTALL_HOOK_OFFSETLESS(RefreshContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "RefreshContent", 0));
    INSTALL_HOOK_OFFSETLESS(LevelSelectionFlowCoordinator_StartLevel, il2cpp_utils::FindMethodUnsafe("", "LevelSelectionFlowCoordinator", "StartLevel", 3));
    INSTALL_HOOK_OFFSETLESS(EnergyBarUpdate, il2cpp_utils::FindMethodUnsafe("", "GameEnergyCounter", "AddEnergy", 1));
    INSTALL_HOOK_OFFSETLESS(ScoreControllerLateUpdate, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "LateUpdate", 0));
    INSTALL_HOOK_OFFSETLESS(RefreshRank, il2cpp_utils::FindMethodUnsafe("", "ImmediateRankUIPanel", "RefreshUI", 0));
    INSTALL_HOOK_OFFSETLESS(Triggers, il2cpp_utils::FindMethodUnsafe("", "VRControllersInputManager", "TriggerValue", 1));
    INSTALL_HOOK_OFFSETLESS(ControllerUpdate, il2cpp_utils::FindMethodUnsafe("", "VRController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(ProgressUpdate, il2cpp_utils::FindMethodUnsafe("", "SongProgressUIController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(PauseMenuManager_MenuButtonPressed, il2cpp_utils::FindMethodUnsafe("", "PauseMenuManager", "MenuButtonPressed", 0));
    INSTALL_HOOK_OFFSETLESS(ResultsScreenEnd, il2cpp_utils::FindMethodUnsafe("", "ResultsViewController", "DidDeactivate", 1));
    INSTALL_HOOK_OFFSETLESS(NoteWasMissed, il2cpp_utils::FindMethodUnsafe("", "NoteController", "SendNoteWasMissedEvent", 0));
    INSTALL_HOOK_OFFSETLESS(NoteWasCut, il2cpp_utils::FindMethodUnsafe("", "NoteController", "SendNoteWasCutEvent",1));
    INSTALL_HOOK_OFFSETLESS(SceneManager_SetActiveScene, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "SetActiveScene", 1));
    INSTALL_HOOK_OFFSETLESS(VRCenterAdjust_Enable, il2cpp_utils::FindMethodUnsafe("", "VRCenterAdjust", "Start", 0));
    Logger::get().info("Installed all hooks!");
    il2cpp_functions::Init();
}
