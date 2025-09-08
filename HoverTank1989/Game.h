#pragma once

#include <sstream>
#include "AnimatedTexture.h"
#include "DeviceResources.h"
#include "Keyboard.h"
#include "sounds.h"
#include "SpriteSheet.h"
#include "StepTimer.h"
#include "WICTextureLoader.h"

#include "Camera.h"
#include "DebugData.h"
#include "Environment.h"
#include "Lighting.h"
#include "NPCController.h"
#include "ModelController.h"
#include "Vehicle.h"
#include "VehiclePlayer.h"
#include "VehicleNPC.h"
#include "VehicleHover.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    //~Game() = default;
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void AudioFxDelete(std::shared_ptr<Utility::SoundFx> aFx);
    void AudioFxReset(std::shared_ptr<Utility::SoundFx> aFx);
    void AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK aSFX);
    void AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK aSFX);

    void AudioCreateSFX3D(const DirectX::SimpleMath::Vector3 aPos, Utility::SoundFxType aSfxType);

    void CalculateSpawnerData();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void DrawDebugLinesVector();
    void DrawDebugDataUI();
    void DrawEndUI();
    void DrawIntroScene();
    void DrawLaunchSite();
    void DrawLogoScreen();
    void DrawLogoScreenOld();

    void DrawSky();
    void DrawSky2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawSky2MultisampleTest(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::BasicEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    
    void DrawSky2Base(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawSpawner();
    void DrawSpawner1();
    void DrawSpawner1to2();
    void DrawSpawner2();
    void DrawStartScreen();
    void DrawTerrainNew(Terrain& aTerrain);
    void DrawTestRangeMissile();
    void DrawTestTrack();
    void DrawUIDisplay();
    void DrawUIDisplayLaser();
    void DrawUnlockUI();

    void FadeOutMusic();
  
    unsigned int GetRandomNonRepeatingFxIndex(unsigned int aCurrentIndex, Utility::SoundFxType aFxType);

    bool InitializeTerrainArray();
    bool InitializeTerrainArrayNew(Terrain& aTerrain);
    bool InitializeTerrainArrayNewBackUp(Terrain& aTerrain);
    bool InitializeTerrainArrayStartScreen(Terrain& aTerrain);

    void Render();

    void SetFogVals(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals2(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals3(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);

    void SetUiAmmoDisplay(AmmoType aAmmoType);
    void SetUiTextDisplay(std::string aString);

    void SetUiDisplay(std::string aString);
    
    void TriggerFireWithAudio();

    void ToggleMusicFadeOut();
    void TogglePause();
    void Update(DX::StepTimer const& aTimer);
    void UpdateAudio(DX::StepTimer const& aTimer);
    void UpdateAudioEmitters(DX::StepTimer const& aTimer);
    void UpdateAudioListener(DX::StepTimer const& aTimer);
    void UpdateAutoFire();
    void UpdateInput(DX::StepTimer const& aTimer);
    void UpdateGameplayOnramp(DX::StepTimer const& aTimer);
    void UpdateSpawners();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    
    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_proj;

    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
    std::unique_ptr<DirectX::Model> m_modelTank01;
    std::unique_ptr<DirectX::Model> m_modelTankBody01;
    std::unique_ptr<DirectX::Model> m_modelTankTurret01;
    std::unique_ptr<DirectX::Model> m_modelTankBarrel01;

    std::shared_ptr<DirectX::Model> m_modelTest;
    std::shared_ptr<DirectX::Model> m_modelTestBarrel;
    std::shared_ptr<DirectX::Model> m_modelTestBody;
    std::shared_ptr<DirectX::Model> m_modelTestTurret;

    DirectX::ModelBone::TransformArray m_drawBones;
    DirectX::ModelBone::TransformArray m_animBones;

    Camera*                         m_camera;   
    std::shared_ptr<DebugData>      m_debugData;
    Environment*                    m_environment;
    std::shared_ptr<FireControl>    m_fireControl;
    Lighting*                       m_lighting;
    std::shared_ptr<ModelController> m_modelController;
    std::shared_ptr<NPCController>  m_npcController;
    std::shared_ptr<Vehicle>        m_vehicle;
    
    std::shared_ptr<VehicleHover> m_testVehicleHover;

    using VertexType = DirectX::VertexPositionNormalColorTexture;
    using VertexType2 = DirectX::VertexPositionNormalColor;
    using VertexType3 = DirectX::VertexPositionColor;
    
    std::shared_ptr<DirectX::NormalMapEffect>       m_effect;
    std::shared_ptr<DirectX::BasicEffect>           m_effect2;
    std::shared_ptr<DirectX::BasicEffect>           m_effect3;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType2>> m_batch2;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType3>> m_batch3;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureSky;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapSky;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularSky;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specular;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTest;


    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapMetalTest1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureMetalTest1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularMetalTest1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapMetalTest2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureMetalTest2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularMetalTest2;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapFlameTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureFlameTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularFlameTest;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI0;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI0;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI2;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI3;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI4;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapBMW;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapBMW2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularBMW;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureBMW;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalStartScreen;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularStartScreen;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureStartScreen;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapBluePrint;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularBluePrint;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureBluePrint;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTopAttack;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTopAttack;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack0;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack6;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTopAttack7;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapAutoGame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularAutoGame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureAutoGame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTeaser;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTeaser;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTeaser;


    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout2;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout3;

    std::shared_ptr<DirectX::IEffect>               m_lightEffect;

    std::unique_ptr<DirectX::GeometricPrimitive>    m_skyShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    m_billboardShape;
    const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
    const DirectX::SimpleMath::Vector3              m_billboardPosition = DirectX::SimpleMath::Vector3(0.0f, 50.0f, 0.0f);

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_raster;

    std::unique_ptr<DirectX::Keyboard>              m_keyboard;
    std::unique_ptr<DirectX::Mouse>                 m_mouse;
    DirectX::Keyboard::KeyboardStateTracker         m_kbStateTracker;
    std::unique_ptr<DirectX::GamePad>               m_gamePad;
    DirectX::GamePad::ButtonStateTracker            m_buttons;

    std::unique_ptr<DirectX::SpriteFont>            m_font;
    std::unique_ptr<DirectX::SpriteFont>            m_titleFont;
    std::unique_ptr<DirectX::SpriteFont>            m_bitwiseFont;
    DirectX::SimpleMath::Vector2                    m_fontPos;
    DirectX::SimpleMath::Vector2                    m_fontPos2;
    DirectX::SimpleMath::Vector2                    m_fontPosDebug;
    DirectX::SimpleMath::Vector2                    m_fontMenuPos;
    DirectX::SimpleMath::Vector2                    m_bitwiseFontPos;
    std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch;

    // Character Textures
    std::unique_ptr<AnimatedTexture>                m_character;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_characterTexture;
    DirectX::SimpleMath::Vector2                    m_characterPos;

    std::unique_ptr<AnimatedTexture>                m_character0;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character0Texture;
    DirectX::SimpleMath::Vector2                    m_character0Pos;

    std::unique_ptr<AnimatedTexture>                m_character1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character1Texture;
    DirectX::SimpleMath::Vector2                    m_character1Pos;

    std::unique_ptr<AnimatedTexture>                m_character2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character2Texture;
    DirectX::SimpleMath::Vector2                    m_character2Pos;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_characterBackgroundTexture;
    DirectX::SimpleMath::Vector2                    m_characterBackgroundPos;
    DirectX::SimpleMath::Vector2                    m_characterBackgroundOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectCalmTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectCalmPos;
    DirectX::SimpleMath::Vector2                    m_environSelectCalmOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectBreezyTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectBreezyPos;
    DirectX::SimpleMath::Vector2                    m_environSelectBreezyOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectAlienTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectAlienPos;
    DirectX::SimpleMath::Vector2                    m_environSelectAlienOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bmwLogoTexture;
    DirectX::SimpleMath::Vector2                    m_bmwLogoPos;
    DirectX::SimpleMath::Vector2                    m_bmwLogoOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jiLogoTexture;
    DirectX::SimpleMath::Vector2                    m_jiLogoPos;
    DirectX::SimpleMath::Vector2                    m_jiLogoOrigin;

    int                                             m_menuSelect = 0;

    enum class GameState
    {
        GAMESTATE_INTROSCREEN,
        GAMESTATE_STARTSCREEN,
        GAMESTATE_MAINMENU,
        GAMESTATE_CHARACTERSELECT,
        GAMESTATE_COURSESELECT,
        GAMESTATE_ENVIRONTMENTSELECT,
        GAMESTATE_GAMEPLAY,
        GAMESTATE_GAMEPLAYSTART,
        GAMESTATE_TEASERSCREEN,
        GAMESTATE_LOADSCREEN
    };
    GameState                                   m_currentGameState;

    enum class UiState
    {
        UISTATE_SWING,
        UISTATE_SHOT,
        UISTATE_SCORE
    };
    UiState                                     m_currentUiState;

    // audio 
    std::unique_ptr<DirectX::AudioEngine>       m_audioEngine;
    bool                                        m_retryAudio;
    float                                       m_musicVolume = 0.9f;
    float                                       m_sfxVolume = 0.5f;

    std::unique_ptr<DirectX::WaveBank>          m_audioBank;
    std::unique_ptr<DirectX::SoundStreamInstance> m_audioMusicStream;
    std::unique_ptr<DirectX::SoundStreamInstance> m_audioEffectStream;

 //   std::unique_ptr<DirectX::SoundEffect>       m_soundEffect;
  //  std::shared_ptr<DirectX::SoundEffectInstance> m_soundSource;

    std::shared_ptr<DirectX::SoundStreamInstance> m_ssiTest;
    //std::vector<std::shared_ptr<DirectX::SoundEffectInstance>> m_soundSourceVec;
    std::vector<std::shared_ptr<Utility::SoundFx>> m_soundFxVecTest;

    

  //  DirectX::AudioEmitter  m_fxEmitter;
    DirectX::AudioListener m_listener;
    DirectX::AudioEmitter  m_emitter;

    unsigned int m_currentFxExplosion = 4;
    unsigned int m_currentFxShotBang = 10;
    
    const unsigned int m_fxExplosionRangeMax = 9;
    const unsigned int m_fxExplosionRangeMin = 4;
    const unsigned int m_fxShotBangRangeMax = 16;
    const unsigned int m_fxShotBangRangeMin = 10;

    const float m_audioDistanceBang = 14.0f;
    const float m_audioDistanceBeacon = 14.0f;
    const float m_audioDistanceExplosion = 14.0f;
    const float m_audioDistancePoof = 14.0f;
    //const float m_audioDistanceRocketLoopable = 14.0f;
    const float m_audioDistanceRocketLoopable = 75.0f;
    const float m_audioLockToneFadeRate = 5.0f;
    const float m_audioLockToneVolMod = 1.0f;

    float                                       m_musicFadeOutTimer = 0.0f;
    const float                                 m_musicFadeOutDuration = 5.0f;
    const float                                 m_musicFadeOutRate = 0.2f;
    bool                                        m_isMusicFadeOutTrue = false;

    float m_audioVolumeGamePlay = 0.0f;
    const float m_audioVolumeGamePlayMax = 1.0f;
    const float m_audioVolumeTransitionTime = 1.0f;
    float m_audioVolumeTimer = 0.0f;

    const float m_audioAmbientMod = 1.0f;
    const float m_audioPlayerHoverMod = 0.5f;
    const float m_audioPlayerDriveMod = 0.3f;
    const float m_audioNPCMod = 0.7f;
    const float m_audioSpawnerMod = 0.0f;
    const float m_audioLogoMod = 0.0f;

    const float m_audioPlayerDriveVolMod = 0.6f;
    const float m_audioPlayerHoverVolMod = 0.4f;
    const float m_audioPlayerHoverVolMin = 0.1f;

    bool m_audioTestBoolTrue = false;
   
    float m_audioDebugTestVal = 0.0f;

    float m_audioDebugRavenTimer = 0.0f;
    const float m_audioRavenDistance = 10.0f;
    const DirectX::SimpleMath::Vector3 m_audioRavenOffset = DirectX::SimpleMath::Vector3(3.0f, -1.0f, 0.0f);

    float m_audioHoverVal = 0.0f;
    float m_audioHoverVal1 = 0.0f;
    float m_audioHoverVal2 = 0.0f;
    float m_audioHoverVal3 = 0.0f;
    float m_audioHoverVal4 = 0.0f;
    float m_audioHoverVal5 = 0.0f;


    const int m_audioFxIdDebug = 44;
    //const int m_audioFxIdNPCVehicle = 25;
    const int m_audioFxIdNPCVehicle = 3;
    const int m_audioFxIdPlayerVehicle = 27;
    const int m_audioFxIdPlayerVehicleHover = 29;
    const int m_audioFxIdAmbient = 44;
    /*
    const int m_audioFxIdJI1 = 12;
    const int m_audioFxIdJI2 = 12;
    const int m_audioFxIdJI3 = 15;
    const int m_audioFxIdJI4 = 22;
    */
    const int m_audioFxIdJI1 = 11;
    const int m_audioFxIdJI2 = 13;
    const int m_audioFxIdJI3 = 14;
    const int m_audioFxIdJI4 = 16;
    

    /*
    const int m_audioFxIdJI1 = 22;
    const int m_audioFxIdJI2 = 22;
    const int m_audioFxIdJI3 = 22;
    const int m_audioFxIdJI4 = 22;
    */

    const int m_audioSpawner1 = 41;
    const int m_audioSpawner2 = 41;

    const int m_audioSpawnerLightsOn = 14;
    const int m_audioSpawnerLightsOff = 14;
    const float m_audioSpawnerLightsPitchOn = 1.0f;
    const float m_audioSpawnerLightsPitchOff = 0.35f;

    const int m_audioFxIdRaven = 24;
    const int m_audioFxIdRavenAlt = 44;

    const unsigned int m_audioFxIdRocketMotor = 3;

    //const float m_audioCurveDistanceScalarNPC = 19.0f;
    const float m_audioCurveDistanceScalarNPC = 9.0f;
    const float m_audioCurveDistanceScalarPlayer = 8.0f;
    //const float m_audioCurveDistanceScalarSpawner = 54.0f;
    const float m_audioCurveDistanceScalarSpawner = 54.0f;
    const float m_audioCurveDistanceScalarLogo = 14.0f;
    const float m_audioCurveDistScalarMissileRocket = 14.0f;
    const float m_audioCurveDistScalarMissileFin = 14.0f;


    const bool                                  m_isInDebugMode = true;

    DirectX::SimpleMath::Vector3                m_cameraFocusPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3                m_lightPos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos2 = DirectX::SimpleMath::Vector3::UnitX;

    // light pos for m_effect2
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos2 = DirectX::SimpleMath::Vector3::UnitX;

    std::vector<std::pair<DirectX::VertexPositionNormalColor, DirectX::VertexPositionNormalColor>>   m_worldGrid;
    std::vector<std::pair<DirectX::VertexPositionColor, DirectX::VertexPositionColor>>   m_debugWorldGrid;

    bool                                        m_moveLightWithCameraFocus1 = false;
    bool                                        m_moveLightWithCameraFocus2 = false;
    bool                                        m_moveLightWithCameraFocus3 = false;

    const float                                 m_lightMovementSpeed = 0.4;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundTex;

    int                                         m_testDisplayCount = 0;
    const int                                   m_testDisplayCountMax = 3;

    // for height map
    std::vector<DirectX::VertexPositionColor>   m_terrainVector;
    DirectX::VertexPositionColor*       m_terrainVertexArray;
    DirectX::VertexPositionColor*       m_terrainVertexArrayBase;
    int                                 m_terrainVertexCount;
    std::vector<DirectX::VertexPositionNormalColor>   m_terrainVector2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArray2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArrayBase2;
    int                                 m_terrainVertexCount2;
    Terrain                             m_terrainGamePlay;
    Terrain                             m_terrainGamePlayLightsOn;
    Terrain                             m_terrainStartScreen;
    DirectX::SimpleMath::Vector4        m_startScreenGridDimmerColor = DirectX::SimpleMath::Vector4(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);
    float                               m_debugValue1 = 0.0;
    float                               m_debugValue2 = 0.0;
    float                               m_debugValue3 = 0.0;
    float                               m_debugValue4 = 0.0;
    float                               m_debugValue5 = 0.0;

    // start scene control variables
    /*
    const float                         m_fadeDuration = 4.0f;
    const float                         m_logoDisplayDuration = 11.5f;
    const float                         m_logoDisplayGap = 0.8f;
    const float                         m_startDelay = 5.2f;
    */

    const float                         m_logoSize = 1.0f;
    //const float                         m_fadeDuration = 1.5f;
    const float                         m_fadeDuration = 0.85f;
    const float                         m_fadeDurationRavenIn = 3.0f;
    //const float                         m_logoDisplayDuration = 6.4f; // 7.4,5.1
    //const float                         m_logoDisplayDuration = 12.4f; // 7.4,5.1
    const float                         m_logoDisplayDuration = 7.4f; // 7.4,5.1
    //const float                         m_logoDisplayDuration = 3.4f; // 7.4,5.1
    //const float                         m_logoDisplayDuration = 0.8f; // 7.4,5.1
  //  const float                         m_logoDisplayDuration = 0.4f;
    //const float                         m_logoDisplayGap = 0.35f;
    const float                         m_logoDisplayGap = 0.55f;
    //const float                         m_startDelay = 5.5f;
    const float                         m_startDelay = 1.5f;

    float m_logoZoomMod = 1.17f;
    const float m_logoZoomModBMW = 1.6f;
    const float m_logoZoomModJI = 1.17f;
    const float m_logoZoomModBluePrint = 0.97f;

    //float m_logoPosOffsetX = 0.0f;
    float m_logoPosOffsetY = 0.0f;
    const float m_logoPosOffsetYMax = 0.29f;
    //const float m_logoPosOffsetXMax = 0.3f;

    bool m_isLogoTriggerBMWTrue = false;

    bool m_isDemoTriggerTrue0 = false;

    bool m_isStartTriggerTrue0 = false;
    bool m_isStartTriggerTrue00 = false;
    bool m_isStartTriggerTrue1 = false;
    bool m_isStartTriggerTrue2 = false;
    bool m_isStartTriggerTrue3 = false;

    bool m_isLogoAudioTriggerTrueDice = false;
    bool m_isLogoAudioTriggerTrue1 = false;
    bool m_isLogoAudioTriggerTrue2 = false;
    bool m_isLogoAudioTriggerTrue3 = false;
    bool m_isLogoAudioTriggerTrue4 = false;

    bool m_isBMWLogoAudioTriggerTrue1 = false;

    bool m_isJILogoAudioTriggerTrue1 = false;
    bool m_isJILogoAudioTriggerTrue2 = false;
    bool m_isJILogoAudioTriggerTrue3 = false;
    bool m_isJILogoAudioTriggerTrue4 = false;
    
    //DirectX::AUDIO_ENGINE_REVERB m_jIAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Plate;
    DirectX::AUDIO_ENGINE_REVERB m_jIAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Off;
   // DirectX::AUDIO_ENGINE_REVERB m_bMWAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Alley;
    DirectX::AUDIO_ENGINE_REVERB m_bMWAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Off;
    //DirectX::AUDIO_ENGINE_REVERB m_jIGameReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Mountains;
    DirectX::AUDIO_ENGINE_REVERB m_jIGameReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Off;

    const float m_jiTriggerTimeDice = -2.2f;
    const float m_jiTriggerTime1 = 0.0f + m_startDelay + m_fadeDuration + (m_logoDisplayDuration * 0.35f);
    const float m_jiTriggerTime2 = 0.0f + m_startDelay + m_fadeDuration + (m_logoDisplayDuration * 0.45f);
    const float m_jiTriggerTime3 = 0.0f + m_startDelay + m_fadeDuration + (m_logoDisplayDuration * 0.55f);
    const float m_jiTriggerTime4 = 0.0f + m_startDelay + m_fadeDuration + (m_logoDisplayDuration * 0.65f);

    /*
    const float m_jiTriggerTime1 = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.3f);
    const float m_jiTriggerTime2 = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.4f);
    const float m_jiTriggerTime3 = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.5f);
    const float m_jiTriggerTime4 = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.6f);
    */

    int m_jiTriggerCount = 0;

    const float m_bmwTriggerTime = 0.0f + m_startDelay + m_logoDisplayDuration + (m_logoDisplayDuration * 0.1f);
    const float m_jiNormalTrigger = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.5f);

    //const float m_startTrigger1 = 7.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger2 = 13.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger1 = 1.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger2 = 2.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger1 = 16.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger2 = 19.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger3 = 21.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);

    
    //const float m_startTrigger0 = 4.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    ////const float m_startTrigger00 = 6.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger00 = 7.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger1 = 6.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger2 = 9.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger3 = 11.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);

    
    /* last good sept 1
    const float m_startTrigger0 = 1.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger00 = 6.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger00 = 1.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger1 = 1.2f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger2 = 2.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger3 = 3.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    */

    /*
    const float m_startTrigger0 = 6.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger00 = 13.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger1 = 16.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger2 = 19.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger3 = 21.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    */

    /*
    const float m_startTrigger0 = 7.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger00 = 14.5f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger1 = 16.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger2 = 22.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger3 = 24.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    */

    const float m_startOffset = 70.0f;
    const float m_startTrigger0 = 7.0f + m_startDelay + (m_logoDisplayDuration * 3.0f) + m_startOffset;
    const float m_startTrigger00 = 14.5f + m_startDelay + (m_logoDisplayDuration * 3.0f) + m_startOffset;
    const float m_startTrigger1 = 16.0f + m_startDelay + (m_logoDisplayDuration * 3.0f) + m_startOffset;
    const float m_startTrigger2 = 22.0f + m_startDelay + (m_logoDisplayDuration * 3.0f) + m_startOffset;
    const float m_startTrigger3 = 24.0f + m_startDelay + (m_logoDisplayDuration * 3.0f) + m_startOffset;

    const float                         m_fogGap1 = 0.0;
    const float                         m_fogGap2 = 10.0;

    DirectX::SimpleMath::Vector3        m_introCamPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3        m_introCamTarg = DirectX::SimpleMath::Vector3(1.1f, 0.0f, 0.0f);

    const float m_logoBluePrintOffsetY = 0.0f;
    DirectX::SimpleMath::Vector3        m_introCamPosBluePrint = DirectX::SimpleMath::Vector3(0.0f, m_logoBluePrintOffsetY, 0.0f);
    DirectX::SimpleMath::Vector3        m_introCamTargBluePrint = DirectX::SimpleMath::Vector3(1.1f, m_logoBluePrintOffsetY, 0.0f);

    DirectX::SimpleMath::Vector3        m_introCamTarg2 = DirectX::SimpleMath::Vector3(.9, -0.03, 0.0);
    DirectX::SimpleMath::Vector3        m_introCamPos2 = DirectX::SimpleMath::Vector3(0.0, -0.03, 0.0);

    DirectX::SimpleMath::Vector3        m_startScreenCamPos = DirectX::SimpleMath::Vector3(-0.8, 0.2, 0.0);
    DirectX::SimpleMath::Vector3        m_startScreenCamTarg = DirectX::SimpleMath::Vector3(1.1, 0.1, 0.0);

    DirectX::SimpleMath::Vector3        m_startScreenCamZoomPos = DirectX::SimpleMath::Vector3(4.2, 0.1, 0.0);
    DirectX::SimpleMath::Vector3        m_startScreenZCamZoomTarg = DirectX::SimpleMath::Vector3(7.3, 0.0, 0.0);

    const float                         m_teaserScreenDistance = 90.0;

    DirectX::SimpleMath::Vector3        m_teaserCamPos = DirectX::SimpleMath::Vector3(m_teaserScreenDistance - 1.0f, 0.0, 0.0);
    DirectX::SimpleMath::Vector3        m_teaserCamTarg = DirectX::SimpleMath::Vector3(m_teaserScreenDistance, 0.0, 0.0);

    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos1 = DirectX::SimpleMath::Vector3(8.0f, 1.0, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg1 = DirectX::SimpleMath::Vector3(0.0, -3.8, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos2 = DirectX::SimpleMath::Vector3(18.0f, 3.5, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg2 = DirectX::SimpleMath::Vector3(0.0, 5.0f, 0.0);
    float                               m_gamePlayStartOffSetTimer = 0.0f;

    DirectX::SimpleMath::Vector3        m_testNorm = DirectX::SimpleMath::Vector3::UnitZ;

    float                               m_terrainColorDimmer = 0.0;
    const double                         m_debugStartTime = 0.0;
    const float                         m_startScreenTimerMod = 0.0f;
    const float                         m_startScreenTimerMod2 = 0.0f;

    float                               m_loadScreenTimerStart = 0.0f;

    DirectX::XMFLOAT4 m_testColor;
    const DirectX::XMFLOAT4 m_defaultStartTerrainColor = DirectX::XMFLOAT4(0.1f, 0.01f, 0.01f, 1.0f);
    const DirectX::XMFLOAT4 m_defaultGameTerrainColor = DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 m_terrainBaseColor = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    const DirectX::SimpleMath::Vector4 m_terrainBaseColorCap = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

    const float m_terrainTreeLineStart = 0.7f;
    const float m_startTerrainMaxY = 0.347000033f;
    const float m_gameTerrainMaxY = 396.0f;
    float m_skyRotation = 0.0f;

    double m_testTimer = 0.0;
    double m_testTimerOffset = 0.0;
    float m_testTimer1 = 0.0f;
    float m_testTimer2 = 0.0f; 
    float m_unlockTimer1 = 0.0f;
    float m_unlockTimer2 = 0.0f;
    const int m_unlockCountDownTicks = 5;
    int m_unlockCountdown = m_unlockCountDownTicks;
    bool m_isDisplayCountdownTrue = false;

    float m_endScreenTimer = 0.0f;
    bool m_isDisplayEndScreenTrue = false;

    const float m_uiDisplayDuration = 4.0f;
    const float m_uiDisplayTypeDuration = 1.2f;
    float m_uiDisplayTimer = 0.0f;
    bool m_isUiDisplayTrue = false;
    std::string m_uiAmmoDisplayString = "";

    // 3D shapes
    //const float                                  m_spawnerZOffSet = 0.3f;
    //const float                                  m_spawnerZOffSet = 0.5f;
    const float                                  m_spawnerZOffSet = 0.7f;

    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeNormCube;
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeNormCylinder;
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeNormCylinderGear;
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeNormCylinderAxel;
    const float                                  m_shapeAxelDiameterScale = 0.6f;
    const float                                  m_shapeAxelHeightScale = 1.3f;
    const int                                    m_shapeGearCount = 6;

    std::unique_ptr<DirectX::GeometricPrimitive> m_shapePlatform;
    DirectX::SimpleMath::Matrix                  m_shapePlatformMat;
    const DirectX::SimpleMath::Vector3           m_shapeDimensionsPlatform = DirectX::SimpleMath::Vector3(30.0f, 10.0f, 30.0f);
    const DirectX::SimpleMath::Vector3           m_shapeTranslatePlatform = DirectX::SimpleMath::Vector3(-500.0f, 5.0f, 0.0f);

    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeWayPoint;
    const DirectX::SimpleMath::Vector3           m_shapeDimensionsWayPoint = DirectX::SimpleMath::Vector3(80.0f, 1.0f, 80.0f);
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeWayPath;
    const DirectX::SimpleMath::Vector3           m_shapeDimensionsWayPath = DirectX::SimpleMath::Vector3(250.0f, 1.0f, 6.0f);
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeSkyboxBase;

    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeLaunchPad;
    DirectX::SimpleMath::Matrix                  m_shapeLaunchPadMat;
    DirectX::SimpleMath::Vector3                 m_shapeLaunchPadPos = DirectX::SimpleMath::Vector3(-500.0f, 0.5f, 0.0f);
    DirectX::SimpleMath::Vector3                 m_shapeLaunchPadDimensions = DirectX::SimpleMath::Vector3(100.0f, 1.0f, 100.0f);

    // spawner shape
    const DirectX::SimpleMath::Vector3           m_spawnerDimensions = DirectX::SimpleMath::Vector3(50.0f, 50.0f, 10.0f);
    const float                                  m_spawnerShellThickness = 10.0f;
    //const float                                  m_spawnerScale = 16.15f;
    const float                                  m_spawnerScale = 18.6f;
    const int                                    m_spawnerSides = 6;

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerInnerBackShadowShape;
    DirectX::SimpleMath::Matrix                  m_spawnerInnerBackShadowMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerInnerBackShadowMat2;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerOuterShape;
    DirectX::SimpleMath::Matrix                  m_spawnerOuterMat;
    DirectX::SimpleMath::Matrix                  m_spawnerOuterMat2;

    DirectX::SimpleMath::Matrix                  m_spawnerShellInteriorMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerShellInteriorMat2;

    //DirectX::SimpleMath::Vector3                 m_spawnerPos = DirectX::SimpleMath::Vector3(600.0f, 0.0f, 900.0f);
    //DirectX::SimpleMath::Vector3                 m_spawnerPos2 = DirectX::SimpleMath::Vector3(600.0f, 0.0f, -900.0f);
    DirectX::SimpleMath::Vector3                 m_spawnerPos = DirectX::SimpleMath::Vector3(600.0f, 75.0f, 900.0f);
    DirectX::SimpleMath::Vector3                 m_spawnerPos2 = DirectX::SimpleMath::Vector3(600.0f, 75.0f, -900.0f);
    // axels 
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelShape;
    DirectX::SimpleMath::Matrix                  m_spawnerAxelMat;
    DirectX::SimpleMath::Matrix                  m_spawnerMainAxelMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerMainAxelMat2;
    DirectX::SimpleMath::Quaternion              m_spawnerMainAxelQuat1 = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion              m_spawnerMainAxelQuat2 = DirectX::SimpleMath::Quaternion::Identity;
    const DirectX::SimpleMath::Vector3           m_spawnerAxelDimensions = DirectX::SimpleMath::Vector3(70.0f, 20.0f, 20.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerAxelOffset = DirectX::SimpleMath::Vector3(0.0f, 15.0f, 25.0f);
    //const DirectX::SimpleMath::Vector3           m_spawnerShadowOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, m_spawnerAxelOffset.z + (m_spawnerAxelDimensions.z * 0.5f));
    const DirectX::SimpleMath::Vector3           m_spawnerShadowOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -15.0f);

    const DirectX::SimpleMath::Vector3           m_spawnerMainAxelPos1 = DirectX::SimpleMath::Vector3(m_spawnerPos.x, m_spawnerPos.y + 15.0f, m_spawnerPos.z - 25.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerMainAxelPos2 = DirectX::SimpleMath::Vector3(m_spawnerPos.x, m_spawnerPos.y + 15.0f, m_spawnerPos2.z + 25.0f);

    // gearbox
    const DirectX::SimpleMath::Vector3           m_spawneGearboxDimensions = DirectX::SimpleMath::Vector3(40.0f, 18.5f, 54.0f);
    const DirectX::SimpleMath::Vector3           m_spawneGearboxDimensionsDual = DirectX::SimpleMath::Vector3(40.0f, 16.0f, 4.0f);
    std::unique_ptr<DirectX::GeometricPrimitive> m_shapeGearbox;
    const float m_spawnerGearboxOffsetX = 5.0f;
    const float m_spawnerGearboxOffsetY = - m_spawneGearboxDimensionsDual.y * 0.54f;
    //const float m_spawnerGearboxOffsetZ = (m_spawnerAxelDimensions.x * 0.5f) - (m_spawnerArmDimensions.x * 0.5f) - (m_spawneGearboxDimensionsDual.x * 1.0f);
    const float m_spawnerGearboxOffsetZ = 25.0f;

    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxMat2;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxPortMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxPortMat2;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxStarMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxStarMat2;


    const DirectX::SimpleMath::Vector3           m_spawnerGearboxOffset = DirectX::SimpleMath::Vector3(0.0f, -10.0f, 5.0f);
    //const DirectX::SimpleMath::Vector3           m_spawnerGearboxOffset = DirectX::SimpleMath::Vector3(0.0f, -10.0f, -8.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerGearboxPos1 = DirectX::SimpleMath::Vector3(m_spawnerMainAxelPos1.x + m_spawnerGearboxOffset.x,
        m_spawnerMainAxelPos1.y + m_spawnerGearboxOffset.y, m_spawnerMainAxelPos1.z - m_spawnerGearboxOffset.z);
    const DirectX::SimpleMath::Vector3           m_spawnerGearboxPos2 = DirectX::SimpleMath::Vector3(m_spawnerMainAxelPos2.x + m_spawnerGearboxOffset.x,
        m_spawnerMainAxelPos2.y + m_spawnerGearboxOffset.y, m_spawnerMainAxelPos2.z + m_spawnerGearboxOffset.z);

    // dual axels
    const DirectX::SimpleMath::Vector3           m_spawnerMainGearDimensions = DirectX::SimpleMath::Vector3(15.0f, 21.0f, 20.0f);
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerMainAxelShape;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerMainAxelCoreShape;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelDualShape2;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelCoreDualShape2;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelDualShape3;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelCoreDualShape3;

    const float m_spawnerAxelHeight2 = 10.0f;
    const float m_spawnerAxelDiameter2 = 10.0f;

    const float m_spawnerAxelHeight3 = 10.0f;
    const float m_spawnerAxelDiameter3 = 5.0f;

    const DirectX::SimpleMath::Vector3           m_spawnerSecondAxelPos2 = DirectX::SimpleMath::Vector3(m_spawnerMainAxelPos2.x, m_spawnerMainAxelPos2.y -6.0f, m_spawnerMainAxelPos2.z + 14.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerThirdAxelPos2 = DirectX::SimpleMath::Vector3(m_spawnerMainAxelPos2.x, m_spawnerMainAxelPos2.y - 4.0f, m_spawnerMainAxelPos2.z + 22.0f);

    const float m_spawnerMainGearOffsetX = 5.0f;
    const float m_spawnerMainGearOffsetY = 15.0;
    const float m_spawnerMainGearOffsetZ = 27.0f;
   
    DirectX::SimpleMath::Matrix                  m_spawnerMainGearPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerMainGearStarMat1 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerMainGearPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerMainGearStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerSecondGearPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerSecondGearStarMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerSecondGearPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerSecondGearStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerThirdGearPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerThirdGearStarMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerThirdGearPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerThirdGearStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelShape2;
    DirectX::SimpleMath::Matrix                  m_spawnerAxelMat2;
    DirectX::SimpleMath::Vector3                 m_spawnerAxel2PosSpawner1 = DirectX::SimpleMath::Vector3(600.0f, 9.0f, 861.0f);
    DirectX::SimpleMath::Vector3                 m_spawnerAxel2PosSpawner2 = DirectX::SimpleMath::Vector3(600.0f, 9.0f, -861.0f);
    //DirectX::SimpleMath::Vector3                 m_spawnerAxel2PosSpawner1 = DirectX::SimpleMath::Vector3(600.0f, 8.5f, 888.5f);
    //DirectX::SimpleMath::Vector3                 m_spawnerAxel2PosSpawner2 = DirectX::SimpleMath::Vector3(600.0f, 8.5f, -888.5f);
    DirectX::SimpleMath::Matrix                  m_spawnerAxel2Spawner1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerAxel2Spawner2 = DirectX::SimpleMath::Matrix::Identity;
    const DirectX::SimpleMath::Vector3           m_spawnerAxelDimensions2 = DirectX::SimpleMath::Vector3(65.0f, 20.0f, 20.0f);

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelShape3;
    DirectX::SimpleMath::Matrix                  m_spawnerAxel3Spawner1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerAxel3Spawner2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3                 m_spawnerAxel3PosSpawner1 = DirectX::SimpleMath::Vector3(600.0f, 11.0f, 853.0f);
    DirectX::SimpleMath::Vector3                 m_spawnerAxel3PosSpawner2 = DirectX::SimpleMath::Vector3(600.0f, 11.0f, -853.0f);
    //DirectX::SimpleMath::Vector3                 m_spawnerAxel3PosSpawner1 = DirectX::SimpleMath::Vector3(600.0f, 11.0f, 895.5f);
    //DirectX::SimpleMath::Vector3                 m_spawnerAxel3PosSpawner2 = DirectX::SimpleMath::Vector3(600.0f, 11.0f, -895.5f);
    const DirectX::SimpleMath::Vector3           m_spawnerAxelDimensions3 = DirectX::SimpleMath::Vector3(65.0f, 20.0f, 20.0f);

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerAxelShaftShape;
    const DirectX::SimpleMath::Vector3           m_spawnerAxelShaftDimensions = DirectX::SimpleMath::Vector3(65.5f, 12.0f, 1.0f);
    const int                                    m_spawnerAxelShaftSideCount = 3;

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerDoorShape;
    DirectX::SimpleMath::Matrix                  m_spawnerDoorMat;
    const DirectX::SimpleMath::Vector3           m_spawnerDoorDimensions = DirectX::SimpleMath::Vector3(72.0f, 35.0f, 4.0f);

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerArmShape;
    DirectX::SimpleMath::Matrix                  m_spawnerArmMat;
    DirectX::SimpleMath::Vector3           m_spawnerArmBaseTransPreVec = DirectX::SimpleMath::Vector3((m_spawnerAxelDimensions.x * 0.5f) - (m_spawnerArmDimensions.x * 0.5f), 5.0f + (m_spawnerArmDimensions.y * 0.5f), (m_spawnerArmDimensions.z * 0.5f) - (m_spawnerAxelDimensions.z * sqrt(3.0f) * 0.25f));
    const DirectX::SimpleMath::Vector3           m_spawnerArmBaseOffsetVec = DirectX::SimpleMath::Vector3(0.0f, m_spawnerAxelOffset.y, m_spawnerAxelOffset.z);

    DirectX::SimpleMath::Matrix                  m_spawnerArmBasePortMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerArmBaseStarMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerArmBaseStarMat1Alt;
    DirectX::SimpleMath::Matrix                  m_spawnerArmBasePortMat2;
    DirectX::SimpleMath::Matrix                  m_spawnerArmBaseStarMat2;

    const float                                  m_spawnerArmExtendedCombinedBase = 57.62f;
    const DirectX::SimpleMath::Vector3           m_spawnerArmDimensions = DirectX::SimpleMath::Vector3(4.0f, 5.0f, m_spawnerArmExtendedCombinedBase);

    const DirectX::SimpleMath::Vector3           m_spawnerExtendorDimensions = DirectX::SimpleMath::Vector3(5.0f, 3.0f, 33.5f);
    //const DirectX::SimpleMath::Vector3           m_spawnerExtenderBasePos;
    const float                                  m_spawnerExtenderBaseVerticleOffset = m_spawnerExtendorDimensions.y * 0.5f;
    const float                                  m_spawnerExtenderBaseHorizontalOffset = m_spawnerDoorDimensions.x * 0.5f;
    float                                        m_spawnerExtenderMod1 = 0.0f;
    float                                        m_spawnerExtenderMod2 = 0.0f;
    //const float                                  m_spawnerExtenderDistanceMax = 11.0f;
    const float                                  m_spawnerExtenderDistanceMax = 13.0f;
    const float                                  m_spawnerExtenderDeployTime = 1.0f;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderUpperPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderLowerPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderUpperStarMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderLowerStarMat1 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderUpperPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderLowerPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderUpperStarMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderLowerStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderDoorMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderDoorMat2 = DirectX::SimpleMath::Matrix::Identity;

    // shadows
    const int                                    m_spawnerTerrainFadeStartRow1Spawn1 = 2247;
    const int                                    m_spawnerTerrainFadeEndRow1Spawn1 = 2258;
    const int                                    m_spawnerTerrainFadeStartRow2Spawn1 = 2444;
    const int                                    m_spawnerTerrainFadeEndRow2Spawn1 = 2460;

    const float                                  m_spawnerShadowDoorOffset = 10.0f;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseArmPortShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseArmStarShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseArmPortShadowMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseArmStarShadowMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderArmPortShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderArmStarShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderArmPortShadowMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderArmStarShadowMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderDoorShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderDoorShadowMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerShellShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerShellShadowMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerGearBoxShadowMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerMainAxelShadowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerMainAxelShadowMat2 = DirectX::SimpleMath::Matrix::Identity;

    // extender gear housing
    const DirectX::SimpleMath::Vector3           m_spawnerExtendHousingDimensions = DirectX::SimpleMath::Vector3(m_spawnerExtendorDimensions.x, 22.0f, 5.0f);
    const float m_spawnerExtendHousingOffsetX = 40.0f;
    const float m_spawnerExtendHousingOffsetY = - 12.0f;
    const float m_spawnerExtendHousingOffsetZ = (m_spawnerAxelDimensions.x * 0.5f) - (m_spawnerArmDimensions.x * 0.5f) - (m_spawnerExtendHousingDimensions.x * 1.0f);

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderHousePortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderHouseStarMat1 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderHousePortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderHouseStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    // extender gear
    const float           m_spawnerExtendGearDiameter = 9.0f;
    const float           m_spawnerExtendGearHeight = m_spawnerExtendorDimensions.x;
    const DirectX::SimpleMath::Vector3 m_spawnerExtendGearScale = DirectX::SimpleMath::Vector3(m_spawnerExtendGearDiameter, m_spawnerExtendGearHeight, m_spawnerExtendGearDiameter);

    const float m_spawnerExtendGearOffsetX = m_spawnerExtendHousingOffsetX;
    const float m_spawnerExtendGearOffsetY = m_spawnerArmDimensions.y - (m_spawnerExtendorDimensions.y * 1.0f) - (m_spawnerExtendGearDiameter * 0.5f);
    const float m_spawnerExtendGearOffsetZ = m_spawnerExtendHousingOffsetZ + (m_spawnerExtendHousingDimensions.z * 1.0f);

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderGearPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderGearStarMat1 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix                  m_spawnerExtenderGearPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerExtenderGearStarMat2 = DirectX::SimpleMath::Matrix::Identity;


    //base
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerBaseShape;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseMat2;
    const DirectX::SimpleMath::Vector3           m_spawnerBaseDimensions = DirectX::SimpleMath::Vector3(80.0f, 50.0f, 20.0f);
    //const DirectX::SimpleMath::Vector3           m_spawnerBaseTrans = DirectX::SimpleMath::Vector3(0.0f, 8.0f, 110.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerBaseTrans = DirectX::SimpleMath::Vector3(0.0f, 8.0f, 110.0f);

    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerBaseRoofShape;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseRoofMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseRoofMat2;
    const DirectX::SimpleMath::Vector3           m_spawnerBaseRoofDimensions = DirectX::SimpleMath::Vector3(115.0f, 5.0f, 80.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerBaseRoofTrans = DirectX::SimpleMath::Vector3(0.0f, 27.0f, 110.0f);


    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerLightRingShape;
    const float                                  m_spawnerLightAngle = Utility::ToRadians(100.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightHousingDimensions = DirectX::SimpleMath::Vector3(10.0f, 6.0f, 10.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightInnerDimensions = DirectX::SimpleMath::Vector3(
        m_spawnerLightHousingDimensions.x * 0.9f, m_spawnerLightHousingDimensions.y * 1.0f, m_spawnerLightHousingDimensions.z * 0.9f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightInnerOffset = DirectX::SimpleMath::Vector3(-0.5f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightingCenterTrans = DirectX::SimpleMath::Vector3(
        -m_spawnerBaseRoofDimensions.x * 0.5f + (m_spawnerLightHousingDimensions.x * 0.0f), 31.5f, 0.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightingPosOffset = DirectX::SimpleMath::Vector3(m_spawnerBaseRoofDimensions.x * 1.0f, 0.0f, 0.0f);
    const float                                  m_spawnerLightingSpacing = m_spawnerBaseRoofDimensions.x * 0.2f;
    const float                                  m_spawnerLightingRingThickness = 0.5f;
    const float                                  m_spawnerLightingRingScale = 8.0f;

    float                                        m_spawnerInteriorLightAngle0Spawner1 = 0.0f;
    float                                        m_spawnerInteriorLightAngle1Spawner1 = 0.0f;
    float                                        m_spawnerInteriorLightAngle2Spawner1 = 0.0f;
    const float                                  m_spawnerInteriorLightAngleFrequencyMod = 39.04;

    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat1Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat1Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat1Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat1Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat1Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat1Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat1Pos4;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat1Pos4;

    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat2Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat2Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat2Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat2Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat2Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat2Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightHousingMat2Pos4;
    DirectX::SimpleMath::Matrix                  m_spawnerLightInnerMat2Pos4;

    //const DirectX::SimpleMath::Vector3           m_spawnerLightRingTrans = DirectX::SimpleMath::Vector3(m_spawnerLightHousingDimensions.x, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3           m_spawnerLightRingTrans = DirectX::SimpleMath::Vector3(0.0f, m_spawnerLightHousingDimensions.y * 0.5f, 0.0f);
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat1Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat1Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat1Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat1Pos4;

    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat2Pos1;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat2Pos2;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat2Pos3;
    DirectX::SimpleMath::Matrix                  m_spawnerLightRingMat2Pos4;

    DirectX::SimpleMath::Vector3                 m_spawnerLightDirection1;
    DirectX::SimpleMath::Vector3                 m_spawnerLightDirection2;

    DirectX::SimpleMath::Vector3                 m_spawnerLightPos1;
    DirectX::SimpleMath::Vector3                 m_spawnerLightPos2;

    const DirectX::SimpleMath::Vector3 m_spawnerWindowDimensions = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Matrix                  m_spawnerWindowMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerWindowMat2;

    DirectX::SimpleMath::Matrix                  m_spawnerBaseLowerMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseLowerMat2;
    const DirectX::SimpleMath::Vector3           m_spawnerBaseLowerDimensions = DirectX::SimpleMath::Vector3(70.0f, 30.0f, 100.0f);

    const DirectX::SimpleMath::Vector3 m_spawnerBasePos1 = DirectX::SimpleMath::Vector3(m_spawnerPos.x + m_spawnerBaseTrans.x, 
        m_spawnerPos.y + m_spawnerAxelOffset.y + m_spawnerBaseTrans.y, m_spawnerPos.z - m_spawnerAxelOffset.z + m_spawnerBaseTrans.z);
    const DirectX::SimpleMath::Vector3 m_spawnerBasePos2 = DirectX::SimpleMath::Vector3(m_spawnerBasePos1.x, m_spawnerBasePos1.y, -m_spawnerBasePos1.z);
    const DirectX::SimpleMath::Vector3 m_spawnerRoofPos1 = DirectX::SimpleMath::Vector3(m_spawnerBasePos1.x, m_spawnerBasePos1.y + m_spawnerBaseRoofTrans.y, m_spawnerBasePos1.z);
    const DirectX::SimpleMath::Vector3 m_spawnerRoofPos2 = DirectX::SimpleMath::Vector3(m_spawnerRoofPos1.x, m_spawnerRoofPos1.y, -m_spawnerRoofPos1.z);
    const DirectX::SimpleMath::Vector3 m_spawnerLowerPos1 = DirectX::SimpleMath::Vector3(m_spawnerRoofPos1.x, m_spawnerRoofPos1.y - 30.0f, m_spawnerRoofPos1.z);
    const DirectX::SimpleMath::Vector3 m_spawnerLowerPos2 = DirectX::SimpleMath::Vector3(m_spawnerLowerPos1.x, m_spawnerLowerPos1.y, -m_spawnerLowerPos1.z);

    DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastMat2;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastScaledMat1;
    DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastScaledMat2;

    const DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastMatPost1 = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3(m_spawnerBasePos1.x, m_spawnerBasePos1.y - 2.0f, m_spawnerBasePos1.z), DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    const DirectX::SimpleMath::Matrix                  m_spawnerBaseBlastMatPost2 = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3(m_spawnerBasePos2.x, m_spawnerBasePos2.y - 2.0f, m_spawnerBasePos2.z), DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);

    DirectX::SimpleMath::Vector3                  m_spawnerBlastWindowDimensions = DirectX::SimpleMath::Vector3(13.0f, 15.0f, 5.0f);
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowMat2 = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3                 m_spawnerBlastWindowBarDimensions = DirectX::SimpleMath::Vector3(3.0f, 15.0f, 5.0f);
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowBarPortMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowBarStarMat1 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowBarPortMat2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix                  m_spawnerBlastWindowBarStarMat2 = DirectX::SimpleMath::Matrix::Identity;

    bool m_isSpanwerBlastWindowOn1 = false;
    bool m_isSpanwerBlastWindowOn2 = false;

    const float m_spawnerBlastWindowOffsetX = 15.0f;
    const float m_spawnerBlastWindowOffsetY = 19.8f;
    //const float m_spawnerBlastWindowOffsetZ = 49.5f;
    const float m_spawnerBlastWindowOffsetZ = 51.5f;
    const float m_spawnerBlastWindowAngle = Utility::ToRadians(29.0f);

    const DirectX::SimpleMath::Vector3 m_spawnerRearScale = DirectX::SimpleMath::Vector3(m_spawnerBaseLowerDimensions.x * 0.9f, m_spawnerBaseLowerDimensions.y, m_spawnerBaseLowerDimensions.z * 0.6f);
    const DirectX::SimpleMath::Vector3 m_spawnerRearPos2 = DirectX::SimpleMath::Vector3(m_spawnerBasePos2.x, m_spawnerBasePos2.y + 11.0f, m_spawnerBasePos2.z - (m_spawnerRearScale.z * 0.2f));
    DirectX::SimpleMath::Matrix        m_spawnerBaseRearMat2;
    DirectX::SimpleMath::Matrix        m_spawnerBaseRearMat1;

    const DirectX::XMVECTORF32 m_spawnerColorArms = DirectX::Colors::LightGray;
    const DirectX::XMVECTORF32 m_spawnerColorAxelCore1 = m_spawnerColorArms;
    const DirectX::XMVECTORF32 m_spawnerColorAxel1 = DirectX::Colors::Gray;
    DirectX::XMVECTORF32 m_spawnerColorAxel2 = { { { 0.55f, 0.55f, 0.65f, 1.f } } };
    const DirectX::XMVECTORF32 m_spawnerColorAxel3 = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorAxelShaft = DirectX::Colors::White;
    const DirectX::XMVECTORF32 m_spawnerColorBlastWindowBars = DirectX::Colors::LightGray;
    const DirectX::XMVECTORF32 m_spawnerColorBlastWindowDeploy = DirectX::Colors::LightSteelBlue;
    const DirectX::XMVECTORF32 m_spawnerColorBlastWindowInterior = DirectX::Colors::Black;
    const DirectX::XMVECTORF32 m_spawnerColorDoor = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorExtenderDoor = DirectX::Colors::DarkGray;
    const DirectX::XMVECTORF32 m_spawnerColorExtenderGear = m_spawnerColorAxel1;
    const DirectX::XMVECTORF32 m_spawnerColorExterior = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorInterior = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorGearBox = DirectX::Colors::DarkGray;
    const DirectX::XMVECTORF32 m_spawnerColorGray1 = DirectX::Colors::LightGray;
    const DirectX::XMVECTORF32 m_spawnerColorGray2 = DirectX::Colors::Gray;
    //const DirectX::XMVECTORF32 m_spawnerColorShell = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorShell = { { { 0.4f, 0.4f, 0.4f, 1.f } } };

    const DirectX::XMVECTORF32 m_spawnerColorLightHousing = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorLightOn = DirectX::Colors::LightYellow;
    const DirectX::XMVECTORF32 m_spawnerColorLightOff = DirectX::Colors::Black;
    const DirectX::XMVECTORF32 m_spawnerColorBaseArm= DirectX::Colors::LightGray;
    const DirectX::XMVECTORF32 m_spawnerColorExtenderArm = DirectX::Colors::DarkGray;

    const DirectX::XMVECTORF32 m_spawnerColorRoof = DirectX::Colors::SlateBlue;
    const DirectX::XMVECTORF32 m_spawnerColorShadow = DirectX::Colors::Black;
    const DirectX::XMVECTORF32 m_spawnerColorStackFirstFloor = DirectX::Colors::Gray;
    const DirectX::XMVECTORF32 m_spawnerColorStackSecondFloor = DirectX::Colors::Gray;
    const DirectX::XMFLOAT4 m_spawnerColorTerrainPatch = DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

    float m_altSpawnerAng2 = 0.0f;
    const float                                  m_spawnerDoorAngleMax = Utility::ToRadians(40.0f);
    const DirectX::SimpleMath::Quaternion        m_spawnerDoorClosedQuat = DirectX::SimpleMath::Quaternion::Identity;
    const DirectX::SimpleMath::Quaternion        m_spawnerDoorOpenQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, m_spawnerDoorAngleMax);
    const DirectX::SimpleMath::Quaternion        m_spawnerDoorOpenQuat2 = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, -m_spawnerDoorAngleMax);
    float m_spawnerDoorPrevAngle1 = 0.0f;
    float m_spawnerDoorPrevAngle2 = 0.0f;

    float m_spawnerDatatMainAxelAngle1 = Utility::ToRadians(0.0f);
    float m_spawnerDatatMainAxelAngle2 = Utility::ToRadians(0.0f);
    float m_spawnerDatatSecondAxelAngle1 = Utility::ToRadians(0.0f);
    float m_spawnerDatatSecondAxelAngle2 = Utility::ToRadians(0.0f);
    float m_spawnerDatatThirdAxelAngle1 = Utility::ToRadians(0.0f);
    float m_spawnerDatatThirdAxelAngle2 = Utility::ToRadians(0.0f);
    float m_spawnerMainAxelRatio1 = 0.0f;
    float m_spawnerMainAxelRatio2 = 0.0f;
    float m_spawnerBlastShieldRatio1 = 0.0f;
    float m_spawnerBlastShieldRatio2 = 0.0f;
    float m_spawnerGlowRatio1 = 0.0f;
    float m_spawnerGlowRatio2 = 0.0f;

    const float m_spawneBlastShieldExtendMod = 1.1f;

    const float m_spawnerRatioLightOn = 0.3f;
    //const float m_spawnerRatioLightOff = 0.6f;
    //const float m_spawnerDoorSwingTimeMax = 4.5f;
    const float m_spawnerDoorSwingTimeMax = 5.5f;
    const float m_spawnerIntiorLightCutOffMod = 0.8f;

    float m_spawnerDoorTimer1 = 0.0f;
    float m_spawnerDoorTimer2 = 0.0f;
    bool m_isSpawnerDoorActive1 = false;
    bool m_isSpawnerDoorActive2 = false;

    bool m_isSpawnerLightOn1 = false;
    bool m_isSpawnerLightOn2 = false;

    bool m_isSpawnerLightOnAudioTrigger1 = false;
    bool m_isSpawnerLightOnAudioTrigger2 = false;

    int m_doorSwingCount1 = 0;
    int m_doorSwingCount2 = 0;

    bool m_isPauseOn = false;
    bool m_isSlowMoOn = false;

    bool  m_isQuckCameraToggleTrue = false;

    const int m_sideCount = 64;
    float m_shape3Diameter = 5.89f;
    float m_shape3Height = 1.1f;
    float m_shape3Scale = 3.0f;

    float m_lightRotation = 0.0f;
    float m_shapeRotation = 0.0f;

    //const float m_gamePadInputRateTurretHorizontal = 0.03f;
    const float m_gamePadInputRateTurretHorizontal = 0.09f;
    const float m_gamePadInputRateTurretVerticle = 0.015f;
    const float m_gamePadInputRateBodyTurn = 1.0f;
    const float m_gamePadInputRateBodyAccel = 1.0f;
    const float m_gamePadInputRateBodySideStrafe = 1.0f;
    const float m_gamePadInputDeadZone = 0.005f;

    const float m_gamePadInputCamStrafe = 1.5f;
    const float m_gamePadInputCamRotate = 0.35f;
    const float m_gamePadInputCamHeight = 0.4f;

    const float m_gamePadZoomSpeedMod = 0.6f;

    // variables for place holder skybox and ground box, less visually abtrusive to aid in tunning missiles in flight
    const DirectX::SimpleMath::Vector3 m_skyBoxTerrainBaseOffset = DirectX::SimpleMath::Vector3(0.0f, 1000.0f, 0.0f);
    const float m_skyBoxSize = 200000.0f;
    //const float m_skyBoxSize = 2000.0f;
    const float m_baseTerrainDiameter = 100000.0f;
    const float m_baseTerrainHeight = 3000.0f;
    const float m_groundBlackHeight = 5000.0f;
    const float m_ringDiameter = 100000.0f;
    const float m_ringHeight = 230.0f;
    const float m_ringScale = 100.0f;
    const float m_ringOffset = -3000.0f;

    const float m_missileRangeDepth = 300.0f;
    const float m_missileRangeDistance = 1400.0f;
    const float m_missileRangeHeight = 0.5f;
    const DirectX::SimpleMath::Vector3 m_missileRangePos1 = DirectX::SimpleMath::Vector3(m_missileRangeDepth, m_missileRangeHeight, (m_missileRangeDistance * 0.5f));
    const DirectX::SimpleMath::Vector3 m_missileRangePos2 = DirectX::SimpleMath::Vector3(m_missileRangeDepth, m_missileRangeHeight, -(m_missileRangeDistance * 0.5f));


    //const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(1000.0f, 170.0f, 900.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg0 = DirectX::SimpleMath::Vector3(340.0f, 10.0f, 0.0f);


    const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(1000.0f, 170.0f, 900.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(800.0f, 120.0f, 1100.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(850.0f, 90.0f, 1000.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg0 = DirectX::SimpleMath::Vector3(340.0f, 10.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_introPos1 = DirectX::SimpleMath::Vector3(500.0f, 170.0f, -900.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg1 = DirectX::SimpleMath::Vector3(540.0f, 10.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(0.0f, 130.0f, 00.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(200.0f, 350.0f, 800.0f);
   // const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(-200.0f, 350.0f, 400.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3(640.0f, 0.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3(475.0f, 10.0f, 0.0f);

  //  const DirectX::SimpleMath::Vector3 m_introPos3 = DirectX::SimpleMath::Vector3(1200.0f, 170.0f, 500.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg3 = DirectX::SimpleMath::Vector3(540.0f, 10.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_introPos1 = DirectX::SimpleMath::Vector3(-200.0f, 350.0f, 400.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg1 = DirectX::SimpleMath::Vector3(475.0f, 10.0f, 0.0f);

    const DirectX::SimpleMath::Vector3 m_introPos1 = DirectX::SimpleMath::Vector3(-200.0f, 350.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg1 = DirectX::SimpleMath::Vector3(475.0f, 10.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(-500.0f, 15.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(-200.0f, 350.0f, 400.0f);
    const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(-523.0f, 215.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3(475.0f, 10.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(-23.0f, 0.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3::Zero;



    const float m_introCamStep0 = 0.05f;
    const float m_introTargStep0 = 0.09f;
    const float m_introSlerp0 = 0.9f;

    const float m_introCamStep1 = 0.05f;
    const float m_introTargStep1 = 0.09f;
    const float m_introSlerp1 = 0.9f;

    const float m_introCamStep2 = 0.09f;
    const float m_introTargStep2 = 0.15f;
    const float m_introSlerp2 = 0.9f;

    //const DirectX::SimpleMath::Vector3 m_gamePlayCamPos = DirectX::SimpleMath::Vector3(-23.0f, 3.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_gamePlayCamPos = DirectX::SimpleMath::Vector3(-23.0f, 3.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_gamePlayTarg = DirectX::SimpleMath::Vector3(0.0f, 3.0f, 0.0f);

    //const float m_gamePlayCamStep = 0.16f;
    //const float m_gamePlayTargStep = 0.16f;
    //const float m_gamePlaySlerp = 0.16f;

    const float m_gamePlayCamStep = 0.99f;
    const float m_gamePlayTargStep = 0.0099f;
    const float m_gamePlayTargStepAlt = 0.99f;
    const float m_gamePlaySlerp = 0.99f;

    // multisampling
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_offscreenRenderTarget;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_offscreenRenderTargetSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilSRV;

};
