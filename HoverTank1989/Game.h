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

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void DrawDebugLinesVector();
    void DrawDebugDataUI();
    void DrawEndUI();
    void DrawIntroScene();
    void DrawLogoScreen();
    void DrawSky();
    void DrawSky2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawSky2MultisampleTest(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::BasicEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    
    void DrawSky2Base(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawSpawner();
    void DrawStartScreen();
    void DrawTerrainNew(Terrain& aTerrain);
    void DrawTestRangeMissile();
    void DrawTestTrack();
    void DrawUIDisplay();
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
    void SetUiDisplay(std::string aString);
    
    void TriggerFireWithAudio();

    void ToggleMusicFadeOut();
    void TogglePause();
    void Update(DX::StepTimer const& aTimer);
    void UpdateAudioFx(DX::StepTimer const& aTimer);
    void UpdateInput(DX::StepTimer const& aTimer);
    void UpdateAutoFire();

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
    //const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(50.0f, 50.0f, 50.0f);
    //const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(0.01f, 0.5f, 0.8888f);
    //const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(1.0f, 0.5f, 0.8888f);
    //const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(1.0f, 0.75f, 0.8888f);
    const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
    //const DirectX::SimpleMath::Vector3              m_billboardSize = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
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

    std::unique_ptr<DirectX::SoundEffect>       m_soundEffect;
    std::shared_ptr<DirectX::SoundEffectInstance> m_soundSource;

    std::shared_ptr<DirectX::SoundStreamInstance> m_ssiTest;
    //std::vector<std::shared_ptr<DirectX::SoundEffectInstance>> m_soundSourceVec;
    std::vector<std::shared_ptr<Utility::SoundFx>> m_soundFxVecTest;

    DirectX::AudioEmitter  m_fxEmitter;
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
    const float m_audioDistanceExplosion = 140.0f;
    const float m_audioDistancePoof = 14.0f;
    //const float m_audioDistanceRocketLoopable = 14.0f;
    const float m_audioDistanceRocketLoopable = 75.0f;
    const float m_audioLockToneFadeRate = 5.0f;

    float                                       m_musicFadeOutTimer = 0.0f;
    const float                                 m_musicFadeOutDuration = 5.0f;
    const float                                 m_musicFadeOutRate = 0.2f;
    bool                                        m_isMusicFadeOutTrue = false;

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
    //const float                         m_logoDisplayDuration = 6.4f; // 7.4,5.1
    //const float                         m_logoDisplayDuration = 12.4f; // 7.4,5.1
    const float                         m_logoDisplayDuration = 9.4f; // 7.4,5.1
    const float                         m_logoDisplayGap = 0.35f;
    //const float                         m_startDelay = 5.5f;
    const float                         m_startDelay = 1.5f;

    float m_audioVolumeGamePlay = 0.0f;
    const float m_audioVolumeGamePlayMax = 1.0f;
    const float m_audioVolumeTransitionTime = 1.0f;
    float m_audioVolumeTimer = 0.0f;

    const float m_audioPlayerVehicleMod = 1.0f;
    const float m_audioPlayerNPCMod = 1.0f;
    const float m_audioAmbientMod = 1.0f;

    const int m_audioFxIdNPCVehicle = 27;
    const int m_audioFxIdPlayerVehicle = 27;
    const int m_audioFxIdAmbient = 31;
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


    const int m_audioFxIdRavens= 24;

    bool m_isStartTriggerTrue1 = false;
    bool m_isStartTriggerTrue2 = false;

    bool m_isLogoAudioTriggerTrue1 = false;
    bool m_isLogoAudioTriggerTrue2 = false;
    bool m_isLogoAudioTriggerTrue3 = false;
    bool m_isLogoAudioTriggerTrue4 = false;

    bool m_isBMWLogoAudioTriggerTrue1 = false;

    bool m_isJILogoAudioTriggerTrue1 = false;
    bool m_isJILogoAudioTriggerTrue2 = false;
    bool m_isJILogoAudioTriggerTrue3 = false;
    bool m_isJILogoAudioTriggerTrue4 = false;
    
    DirectX::AUDIO_ENGINE_REVERB m_jIAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Plate;
    DirectX::AUDIO_ENGINE_REVERB m_bMWAudioReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Alley;
    DirectX::AUDIO_ENGINE_REVERB m_jIGameReverb = DirectX::AUDIO_ENGINE_REVERB::Reverb_Mountains;

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

    const float m_startTrigger1 = 7.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    //const float m_startTrigger2 = 13.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);
    const float m_startTrigger2 = 13.0f + m_startDelay + (m_logoDisplayDuration * 2.0f);

    const float m_jiNormalTrigger = 0.0f + m_startDelay + (m_logoDisplayDuration * 0.5f);

    const float                         m_fogGap1 = 0.0;
    const float                         m_fogGap2 = 10.0;

    DirectX::SimpleMath::Vector3        m_introCamPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3        m_introCamTarg = DirectX::SimpleMath::Vector3(1.1, 0.0, 0.0);

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
    const DirectX::SimpleMath::Vector4 m_terrainBaseColor = DirectX::XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

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

    std::unique_ptr<DirectX::GeometricPrimitive> m_testShape;
    std::unique_ptr<DirectX::GeometricPrimitive> m_testShape2;
    std::unique_ptr<DirectX::GeometricPrimitive> m_testShape3;


    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerInnerShape;
    DirectX::SimpleMath::Matrix                  m_spawnerInnerMat;
    DirectX::SimpleMath::Matrix                  m_spawnerInnerMat2;
    std::unique_ptr<DirectX::GeometricPrimitive> m_spawnerOuterShape;
    DirectX::SimpleMath::Matrix                  m_spawnerOuterMat;
    DirectX::SimpleMath::Matrix                  m_spawnerOuterMat2;

    DirectX::SimpleMath::Vector3                 m_spawnerPos = DirectX::SimpleMath::Vector3(600.0f, 0.0f, 900.0f);
    DirectX::SimpleMath::Vector3                 m_spawnerPos2 = DirectX::SimpleMath::Vector3(600.0f, 0.0f, -900.0f);

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
    //const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(800.0f, 120.0f, 1100.0f);
    const DirectX::SimpleMath::Vector3 m_introPos0 = DirectX::SimpleMath::Vector3(850.0f, 90.0f, 1000.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg0 = DirectX::SimpleMath::Vector3(340.0f, 10.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_introPos1 = DirectX::SimpleMath::Vector3(500.0f, 170.0f, -900.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg1 = DirectX::SimpleMath::Vector3(540.0f, 10.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(0.0f, 130.0f, 00.0f);
    //const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(200.0f, 350.0f, 800.0f);
    const DirectX::SimpleMath::Vector3 m_introPos2 = DirectX::SimpleMath::Vector3(-200.0f, 350.0f, 400.0f);
    //const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3(640.0f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg2 = DirectX::SimpleMath::Vector3(475.0f, 10.0f, 0.0f);

    const DirectX::SimpleMath::Vector3 m_introPos3 = DirectX::SimpleMath::Vector3(1200.0f, 170.0f, -500.0f);
    const DirectX::SimpleMath::Vector3 m_introTarg3 = DirectX::SimpleMath::Vector3(540.0f, 10.0f, 0.0f);

    const DirectX::SimpleMath::Vector3 m_introPos1 = m_introPos2;
    const DirectX::SimpleMath::Vector3 m_introTarg1 = m_introTarg2;

    const float m_introCamStep0 = 0.05f;
    const float m_introTargStep0 = 0.09f;
    const float m_introSlerp0 = 0.9f;

    const float m_introCamStep1 = 0.05f;
    const float m_introTargStep1 = 0.09f;
    const float m_introSlerp1 = 0.9f;

    //const DirectX::SimpleMath::Vector3 m_gamePlayCamPos = DirectX::SimpleMath::Vector3(-23.0f, 3.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_gamePlayCamPos = DirectX::SimpleMath::Vector3(-23.0f, 3.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_gamePlayTarg = DirectX::SimpleMath::Vector3(0.0f, 3.0f, 0.0f);
    const float m_gamePlayCamStep = 0.16f;
    const float m_gamePlayTargStep = 0.16f;
    const float m_gamePlaySlerp = 0.16f;

    // multisampling
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_offscreenRenderTarget;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_offscreenRenderTargetSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilSRV;

};
