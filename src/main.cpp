#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/beatsaber-hook/include/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <list>
#include <stdlib.h>
#include <stdio.h>
using namespace il2cpp_utils;

void log(std::string str) {
    Logger::get().info(str);
}

class CustomButton {
    public:
        Il2CppObject* gameObject = nullptr;
        Il2CppObject* TMP = nullptr;
        Il2CppObject* button = nullptr;
        Il2CppObject* buttonTransform = nullptr;
        Il2CppObject* parent = nullptr;
        Il2CppObject* parentTransform = nullptr;
        Il2CppObject* TMPLocalizer = nullptr;
        Vector3 sizeDelta = {0, 0, 0};
        Vector3 scale = {1.0f, 1.0f, 1.0f};
        Vector3 rotation = {0, 0, 0};
        float fontSize = 10.0f;
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
            if(!isCreated) {
                Il2CppObject* button = *il2cpp_utils::RunMethod("UnityEngine", "Object", "Instantiate", parent);
                Il2CppObject* buttonTransform = *il2cpp_utils::RunMethod(button, "get_transform");
                il2cpp_utils::RunMethod(buttonTransform, "SetParent", parentTransform);
                il2cpp_utils::RunMethod(buttonTransform, "set_localScale", scale);
                il2cpp_utils::RunMethod(buttonTransform, "set_localPosition", sizeDelta);
                il2cpp_utils::RunMethod(buttonTransform, "set_eulerAngles", rotation);
                gameObject = *il2cpp_utils::RunMethod(button, "get_gameObject");
                TMP = *il2cpp_utils::RunMethod(gameObject, "GetComponentInChildren", il2cpp_utils::GetSystemType("TMPro", "TextMeshProUGUI"));
                TMPLocalizer = *RunMethod<Il2CppObject*>(gameObject, "GetComponentInChildren", GetSystemType("Polyglot", "LocalizedTextMeshProUGUI"));
                auto* rectTransform = *RunMethod(TMP, "get_rectTransform");
                il2cpp_utils::RunMethod(TMP, "set_text", il2cpp_utils::createcsstr(text));
                il2cpp_utils::RunMethod(TMP, "set_fontSize", fontSize);
                Il2CppObject* onClick = *il2cpp_utils::GetPropertyValue(button, "onClick");
                auto actionToRun = il2cpp_utils::MakeAction(il2cpp_functions::class_get_type(il2cpp_utils::GetClassFromName("UnityEngine.Events", "UnityAction")), (Il2CppObject*)nullptr, onPress);
                il2cpp_utils::RunMethod(onClick, "AddListener", actionToRun);
                isCreated = true;
            } else {
                log("Button was already created");
            }
        }
 
        void setText(std::string newText) {
            RunMethod(TMP, "SetText", createcsstr(newText));
        }
 
        void setActive(bool isActive) {
            RunMethod(gameObject, "SetActive", isActive);
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

static CustomButton replayButton;

static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration configuration(modInfo);
    return configuration;
}

Il2CppObject* playButton = nullptr;

bool recording = true;

std::string ssEnabled = "0";
bool firstTime = true;

void replayButtonOnClick() {
    setenv("disable_ss_upload", "1", true);
    if(playButton != nullptr) {
        recording = false;
        RunMethod(playButton, "Press");
    }
}

float songTime = 0.0f;
bool inSong = false;

int score;
int highScore;
std::string rank;
float rankFloat;
float scoreMultiplier = 1.0f;

int combo = 0;

bool inPracticeMode;

float energy;

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

std::string stringToSave;

Il2CppObject* scoreUI;

int indexNum = 0;

std::string songHash;

std::string replayDirectory = "sdcard/Android/data/com.beatgames.beatsaber/files/replays/";

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

Il2CppObject* ScoreControllerInstance = nullptr;

int amountPerLine = 20;

int offset = getConfig().config["Offset"].GetInt();

void SaveConfig() {
    if(!getConfig().config.HasMember("Offset")) {
        log("Creating config");
        getConfig().config.RemoveAllMembers();
        getConfig().config.SetObject();
        rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();
        getConfig().config.AddMember("Offset", 0, allocator);
        getConfig().Write();
        if(fileexists("sdcard/Android/data/com.beatgames.beatsaber/files/mod_cfgs/Replay.json")) {
            log("Config was successfully created");
        }
    } else {
        getConfig().Load();
        log("Not creating config");
    }
}

void getReplayValues(std::string str) {
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
    
    int timesThrough = 0;
 
    std::stringstream ss;
 
    /* Storing the whole string into string stream */
    ss << str;
 
    /* Running loop till the end of the stream */
    std::string temp;
    float floatFound;
    int intFound;
    std::string stringFound;
    Vector3 tempVector;
    while (!ss.eof()) {
 
        /* extracting word by word from stream */
        ss >> temp;
 
        /* Checking the given word is integer or not */
        if (std::stringstream(temp) >> floatFound) {
            if(timesThrough%amountPerLine == 0 || timesThrough%amountPerLine == 3 || timesThrough%amountPerLine == 6 || timesThrough%amountPerLine == 9 || timesThrough%amountPerLine == 15) {
                tempVector.x = floatFound;
            } else if(timesThrough%amountPerLine == 1 || timesThrough%amountPerLine == 4 || timesThrough%amountPerLine == 7 || timesThrough%amountPerLine == 10 || timesThrough%amountPerLine == 16) {
                tempVector.y = floatFound;
            } else if(timesThrough%amountPerLine == 2 || timesThrough%amountPerLine == 5 || timesThrough%amountPerLine == 8 || timesThrough%amountPerLine == 11 || timesThrough%amountPerLine == 17) {
                tempVector.z = floatFound;
                if(timesThrough%amountPerLine == 2) {
                    rightPositions.push_back(tempVector);
                } else if(timesThrough%amountPerLine == 5) {
                    rightRotations.push_back(tempVector);
                } else if(timesThrough%amountPerLine == 8) {
                    leftPositions.push_back(tempVector);
                } else if(timesThrough%amountPerLine == 11) {
                    leftRotations.push_back(tempVector);
                } else if(timesThrough%amountPerLine == 17) {
                    headPositions.push_back(tempVector);
                }
            } else if(timesThrough%amountPerLine == 13) {
                times.push_back(floatFound);
            } else if(timesThrough%amountPerLine == 14) {
                energies.push_back(floatFound);
            }
        }
        if(timesThrough%amountPerLine == 19) {
            log(temp);
            ranks.push_back(temp);
            // timesThrough++;
        }
        if(std::stringstream(temp) >> intFound) {
            if(timesThrough%amountPerLine == 12) {
                scores.push_back(intFound);
            } else if(timesThrough%amountPerLine == 18) {
                combos.push_back(intFound);
            }
            timesThrough++;
        }
        if(temp == "batteryEnergy") batteryEnergy = true;
        if(temp == "disappearingArrows") disappearingArrows = true;
        if(temp == "noObstacles") noObstacles = true;
        if(temp == "noBombs") noBombs = true;
        if(temp == "noArrows") noArrows = true;
        if(temp == "slowerSong") slowerSong = true;
        if(temp == "noFail") noFail = true;
        if(temp == "instafail") instafail = true;
        if(temp == "ghostNotes") ghostNotes = true;
        if(temp == "fasterSong") fasterSong = true;
        if(temp == "slowerSong") slowerSong = true;
        if(temp == "leftHanded") leftHanded = true;
 
        /* To save from space at the end of string */
        temp = "";
    }
}

void extractFloat(std::string str) {

    std::stringstream ss;
 
    /* Storing the whole string into string stream */
    ss << str;
 
    /* Running loop till the end of the stream */
    std::string temp;
    float found;
    while (!ss.eof()) {
 
        /* extracting word by word from stream */
        ss >> temp;
 
        /* Checking the given word is integer or not */
        if(std::stringstream(temp) >> found) {
            rankFloat = found;
        }
 
        /* To save from space at the end of string */
        temp = "";
    }
}

MAKE_HOOK_OFFSETLESS(PlayerController_Update, void, Il2CppObject* self) {
    // log("PlayerControllerUpdate");
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
                stringToSave = stringToSave+std::to_string(rightPos.x)+" "+std::to_string(rightPos.y)+" "+std::to_string(rightPos.z)+" "+std::to_string(rightRot.x)+" "+std::to_string(rightRot.y)+" "+std::to_string(rightRot.z)+" "+std::to_string(leftPos.x)+" "+std::to_string(leftPos.y)+" "+std::to_string(leftPos.z)+" "+std::to_string(leftRot.x)+" "+std::to_string(leftRot.y)+" "+std::to_string(leftRot.z)+" "+std::to_string(score)+" "+std::to_string(songTime)+" "+std::to_string(energy)+" "+std::to_string(headPos.x)+" "+std::to_string(headPos.y)+" "+std::to_string(headPos.z)+" "+std::to_string(combo)+" "+rank+" ";
            }
        }
    }

    PlayerController_Update(self);

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

        Il2CppObject* leftSaber = *il2cpp_utils::GetFieldValue(self, "_leftSaber");
        Il2CppObject* rightSaber = *il2cpp_utils::GetFieldValue(self, "_rightSaber");

        if(leftSaber != nullptr && rightSaber != nullptr) {
            Il2CppObject* leftSaberTransform = nullptr;
            Il2CppObject* rightSaberTransform = nullptr;

            Il2CppClass* componentsClass = il2cpp_utils::GetClassFromName("", "Saber");
            leftSaberTransform = *il2cpp_utils::RunMethod(leftSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));
            rightSaberTransform = *il2cpp_utils::RunMethod(rightSaber, il2cpp_functions::class_get_method_from_name(componentsClass, "get_transform", 0));

            if(leftSaberTransform != nullptr && rightSaberTransform != nullptr) {
                CRASH_UNLESS(RunMethod(rightSaberTransform, "set_position", rightPositions[indexNum]));
                CRASH_UNLESS(RunMethod(rightSaberTransform, "set_eulerAngles", rightRotations[indexNum]));
                CRASH_UNLESS(RunMethod(leftSaberTransform, "set_position", leftPositions[indexNum]));
                CRASH_UNLESS(RunMethod(leftSaberTransform, "set_eulerAngles", leftRotations[indexNum]));
                CRASH_UNLESS(SetFieldValue(self, "_headPos", headPositions[indexNum]));
            }
        }
    }
}

MAKE_HOOK_OFFSETLESS(SongUpdate, void, Il2CppObject* self) {
    
    // log("SongUpdate");

    SongUpdate(self);

    songTime = *il2cpp_utils::GetFieldValue<float>(self, "_songTime");
}

MAKE_HOOK_OFFSETLESS(SongStart, void, Il2CppObject* self, Il2CppObject* difficultyBeatmap, Il2CppObject* overrideEnvironmentSettings, Il2CppObject* overrideColorScheme, Il2CppObject* gameplayModifiers, Il2CppObject* playerSpecificSettings, Il2CppObject* practiceSettings, Il2CppString* backButtonText, bool useTestNoteCutSoundEffects) {

    log("Song Start");

    energy = 0.5f;
    inSong = true;
    indexNum = 0;
    
    if(recording) {
        stringToSave = "";

        batteryEnergy = *RunMethod<bool>(gameplayModifiers, "get_batteryEnergy");
        if(batteryEnergy) stringToSave = stringToSave+"batteryEnergy ";

        disappearingArrows = *RunMethod<bool>(gameplayModifiers, "get_disappearingArrows");
        if(disappearingArrows) stringToSave = stringToSave+"disappearingArrows ";

        ghostNotes = *RunMethod<bool>(gameplayModifiers, "get_ghostNotes");
        if(ghostNotes) stringToSave = stringToSave+"ghostNotes ";

        instafail = *RunMethod<bool>(gameplayModifiers, "get_instaFail");
        if(instafail) stringToSave = stringToSave+"instafail ";

        noArrows = *RunMethod<bool>(gameplayModifiers, "get_noArrows");
        if(noArrows) stringToSave = stringToSave+"noArrows ";

        noBombs = *RunMethod<bool>(gameplayModifiers, "get_noBombs");
        if(noBombs) stringToSave = stringToSave+"noBombs ";

        noFail = *RunMethod<bool>(gameplayModifiers, "get_noFail");
        if(noFail) stringToSave = stringToSave+"noFail ";

        noObstacles = *RunMethod<bool>(gameplayModifiers, "get_noObstacles");
        if(noObstacles) stringToSave = stringToSave+"noObstacles ";

        int songSpeedLevel = *RunMethod<int>(gameplayModifiers, "get_songSpeed");
        slowerSong = false;
        fasterSong = false;
        if(songSpeedLevel == 1) {
            fasterSong = true;
            stringToSave = stringToSave+"fasterSong ";
        } else if(songSpeedLevel == 2) {
            slowerSong = true;
            stringToSave = stringToSave+"slowerSong ";
        }

        leftHanded = *RunMethod<bool>(playerSpecificSettings, "get_leftHanded");
        if(leftHanded) stringToSave = stringToSave+"leftHanded ";
    } else {
        stringToSave = readfile(replayDirectory+songHash+".txt");
        getReplayValues(stringToSave);

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

    if(!recording) {
        setenv("disable_ss_upload", "1", true);
    }

    inSong = false;

    ScoreControllerInstance = nullptr;
    int levelEndState = *GetFieldValue<int>(levelCompleteionResults, "levelEndStateType");

    if(recording && levelEndState == 1 && !inPracticeMode) {
        if(score > highScore || !fileexists(replayDirectory+songHash+".txt")) {
            writefile(replayDirectory+songHash+".txt", stringToSave);
        } else {
            stringToSave = readfile(replayDirectory+songHash+".txt");
            getReplayValues(stringToSave);
            if(score > scores[scores.size()-1]) {
                writefile(replayDirectory+songHash+".txt", stringToSave);
            }
        }
    }
    
    SongEnd(self, levelCompleteionResults);
}

MAKE_HOOK_OFFSETLESS(ScoreChanged, void, Il2CppObject* self, int rawScore, int modifiedScore) {

    // log("Score Changed");
    
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

    recording = true;

    // if(firstTime) {
    //     log("Getting ss");
    //     char* temp = getenv("disable_ss_upload");
    //     std::string tempString(temp);
    //     // ssEnabled = std::string(getenv("disable_ss_upload"));
    //     log(std::string(tempString));
    //     firstTime = false;
    // } else if(ssEnabled == "0") {
        setenv("disable_ss_upload", "0", true);
    // } else {
    //     setenv("disable_ss_upload", "1", true);
    // }

    Il2CppObject* Level = CRASH_UNLESS(*GetFieldValue(self, "_level"));
    Il2CppString* LevelID = CRASH_UNLESS(*GetPropertyValue<Il2CppString*>(Level, "levelID"));

    Il2CppObject* SelectedBeatmapDifficulty = CRASH_UNLESS(*GetFieldValue(self, "_selectedDifficultyBeatmap"));
    int Difficulty = *GetPropertyValue<int>(SelectedBeatmapDifficulty, "difficulty");
    Il2CppObject* beatMapData = CRASH_UNLESS(*GetPropertyValue(SelectedBeatmapDifficulty, "beatmapData"));
    Il2CppObject* parentDifficultyBeatmapSet = CRASH_UNLESS(*GetPropertyValue(SelectedBeatmapDifficulty, "parentDifficultyBeatmapSet"));
    Il2CppObject* beatmapCharacteristic = CRASH_UNLESS(*GetPropertyValue(parentDifficultyBeatmapSet, "beatmapCharacteristic"));

    Il2CppObject* PlayerData = CRASH_UNLESS(*GetFieldValue(self, "_playerData"));
    Il2CppObject* playerLevelStatsData = CRASH_UNLESS(*RunMethod(PlayerData, "GetPlayerLevelStatsData", LevelID, Difficulty, beatmapCharacteristic));
    highScore = *GetPropertyValue<int>(playerLevelStatsData, "highScore");
    log("Highscore is "+std::to_string(highScore));

    songHash = to_utf8(csstrtostr(LevelID))+std::to_string(Difficulty);

    replayButton.destroy();

    if(fileexists(replayDirectory+songHash+".txt")) {
        log("Making Replay button");
        replayButton.setParentAndTransform(playButton, 1);
        replayButton.onPress = replayButtonOnClick;
        replayButton.scale = {1, 1, 1};
        replayButton.fontSize = 5;
        replayButton.create();
        if(replayButton.TMPLocalizer != nullptr) {
            RunMethod("UnityEngine", "Object", "Destroy", replayButton.TMPLocalizer);
        }
        replayButton.setText("Replay");
    } else {
        log("Not making Replay button");
    }
}

MAKE_HOOK_OFFSETLESS(LevelSelectionFlowCoordinator_StartLevel, void, Il2CppObject* self, Il2CppObject* difficultyBeatmap, Il2CppObject* beforeSceneSwitchCallback, bool practice) {
    
    // log("StartLevel");

    inPracticeMode = practice;
    LevelSelectionFlowCoordinator_StartLevel(self, difficultyBeatmap, beforeSceneSwitchCallback, practice);
}

MAKE_HOOK_OFFSETLESS(EnergyBarUpdate, void, Il2CppObject* self, int value) {
    
    // log("EnergyBarUpdate");

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

    // log("ScoreControllerLateUpdate");

    ScoreControllerInstance = self;

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

    log("RefreshRank");

    RefreshRank(self);

    Il2CppObject* percentText = *GetFieldValue(self, "_relativeScoreText");

    if(!recording) {
        RunMethod(percentText, "SetText", createcsstr(ranks[indexNum+1]));
    } else {
        rank = to_utf8(csstrtostr(*RunMethod<Il2CppString*>(percentText, "get_text")));
    }
}

extern "C" void setup(ModInfo& info) {
    info.id = "Replay";
    info.version = "0.1.2";
    modInfo = info;
    // Create logger
    static std::unique_ptr<const Logger> ptr(new Logger(info));
    Logger::get().info("Completed setup!");
    // We can even check information specific to the modloader!
    Logger::get().info("Modloader name: %s", Modloader::getInfo().name.c_str());
}

extern "C" void load() {
    Logger::get().info("Installing hooks...");
    INSTALL_HOOK_OFFSETLESS(SongUpdate, il2cpp_utils::FindMethodUnsafe("", "AudioTimeSyncController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(SongStart, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 8));
    INSTALL_HOOK_OFFSETLESS(SongEnd, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Finish", 1));
    INSTALL_HOOK_OFFSETLESS(PlayerController_Update, il2cpp_utils::FindMethodUnsafe("", "PlayerController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(ScoreChanged, il2cpp_utils::FindMethodUnsafe("", "ScoreUIController", "UpdateScore", 2));
    INSTALL_HOOK_OFFSETLESS(RefreshContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "RefreshContent", 0));
    INSTALL_HOOK_OFFSETLESS(LevelSelectionFlowCoordinator_StartLevel, il2cpp_utils::FindMethodUnsafe("", "LevelSelectionFlowCoordinator", "StartLevel", 3));
    INSTALL_HOOK_OFFSETLESS(EnergyBarUpdate, il2cpp_utils::FindMethodUnsafe("", "GameEnergyCounter", "AddEnergy", 1));
    INSTALL_HOOK_OFFSETLESS(ScoreControllerLateUpdate, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "LateUpdate", 0));
    INSTALL_HOOK_OFFSETLESS(RefreshRank, il2cpp_utils::FindMethodUnsafe("", "ImmediateRankUIPanel", "RefreshUI", 0));
    Logger::get().info("Installed all hooks!");
    il2cpp_functions::Init();

    // SaveConfig();

    std::string path = replayDirectory+"test.txt";
    mkpath(const_cast<char*>(path.c_str()), 0700);
}