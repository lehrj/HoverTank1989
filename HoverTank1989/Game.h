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
//#include "FireControl.h"
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

    void Update(DX::StepTimer const& aTimer);
    void UpdateInput(DX::StepTimer const& aTimer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();


    void DrawDebugLines(const DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor);
    void DrawDebugNormalLines(const DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor);
    void DrawDebugLinesVector();

    void DrawDebugDataUI();
    void DrawUnlockUI();
    void DrawGridForStartScreen();
    void DrawGamePlayStart();
    void DrawIntroScene();
    void DrawLightBar();

    void DrawLoadScreen();
    void DrawLogoScreen();
    void DrawMenuEnvironmentSelect();
    void DrawMenuMain();
    void DrawSky();
    void DrawStartScreen();
    void DrawTeaserScreen();
    void DrawTerrain();
    void DrawTerrain2();
    void DrawTerrainNew(Terrain& aTerrain);
    void DrawTerrainNormals();
    void DrawTimer();
    void DrawUIIntroScreen();
    void DrawWorld();
    void DrawWorldCube();
    void DrawWorldCubeTextured();

    bool InitializeTerrainArray();
    bool InitializeTerrainArray2();

    bool InitializeTerrainArrayNew(Terrain& aTerrain);
    bool InitializeTerrainArrayStartScreen(Terrain& aTerrain);

    void TogglePause();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    
    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_proj;

    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
    std::unique_ptr<DirectX::Model> m_model;
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

    Camera* m_camera;   
    Environment* m_environment;
    Lighting* m_lighting;

    //NPCController* m_npcController;
    std::shared_ptr<NPCController> m_npcController;

    Vehicle* m_vehicle;
    std::shared_ptr<ModelController> m_modelController;
    std::shared_ptr<DebugData> m_debugData;


    std::shared_ptr<VehicleHover> m_testVehicleHover;

    using VertexType = DirectX::VertexPositionNormalColorTexture;
    using VertexType2 = DirectX::VertexPositionNormalColor;
    using VertexType3 = DirectX::VertexPositionColor;

    std::unique_ptr<DirectX::BasicEffect> m_effect4;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalColor>> m_batch4;


    //std::unique_ptr<DirectX::CommonStates>          m_states;
    std::shared_ptr<DirectX::NormalMapEffect>       m_effect;
    std::shared_ptr<DirectX::BasicEffect>           m_effect2;
    std::shared_ptr<DirectX::BasicEffect>           m_effect3;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType2>> m_batch2;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType3>> m_batch3;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureSky;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specular;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTest;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI;

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

    std::shared_ptr<DirectX::IEffect>               m_lightEffect;

    std::unique_ptr<DirectX::GeometricPrimitive>    m_skyShape;
    /*
    DirectX::SimpleMath::Matrix                     m_world;
    DirectX::SimpleMath::Matrix                     m_view;
    DirectX::SimpleMath::Matrix                     m_proj;
    */
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_raster; // WLJ anti-aliasing

    // WLJ added for drawing projectile and swing  
    int                                             m_swingPathStep = 0;
    int                                             m_projectilePathStep = 0;

    // WLJ add for mouse and keybord interface
    std::unique_ptr<DirectX::Keyboard>              m_keyboard;
    std::unique_ptr<DirectX::Mouse>                 m_mouse;
    DirectX::Keyboard::KeyboardStateTracker         m_kbStateTracker;

    // WLJ added for displaying text
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

    // WLJ Intro screen logos
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bmwLogoTexture;
    DirectX::SimpleMath::Vector2                    m_bmwLogoPos;
    DirectX::SimpleMath::Vector2                    m_bmwLogoOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jiLogoTexture;
    DirectX::SimpleMath::Vector2                    m_jiLogoPos;
    DirectX::SimpleMath::Vector2                    m_jiLogoOrigin;

    // WLJ variable to hold selected menu item
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
        GAMESTATE_TEASERSCREEN
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
    DirectX::VertexPositionColor* m_terrainVertexArray;
    DirectX::VertexPositionColor* m_terrainVertexArrayBase;
    int                                         m_terrainVertexCount;
    std::vector<DirectX::VertexPositionNormalColor>   m_terrainVector2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArray2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArrayBase2;
    int                                  m_terrainVertexCount2;
    Terrain m_terrainGamePlay;
    Terrain m_terrainStartScreen;
    DirectX::SimpleMath::Vector4        m_startScreenGridDimmerColor = DirectX::SimpleMath::Vector4(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);
    float                               m_debugValue1 = 0.0;
    float                               m_debugValue2 = 0.0;
    float                               m_debugValue3 = 0.0;
    float                               m_debugValue4 = 0.0;
    float                               m_debugValue5 = 0.0;

    double                              m_testTimer = 0.0;
    double                              m_testTimerOffset = 0.0;

    // start scene control variables
    const float                         m_fadeDuration = 4.0f;
    const float                         m_logoDisplayDuration = 11.5f;
    const float                         m_logoDisplayGap = 0.8f;
    const float                         m_startDelay = 5.2f;

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

    const float m_startTerrainMaxY = 0.347000033f;
    //const float m_gameTerrainMaxY = 2471.24097f;
    const float m_gameTerrainMaxY = 600.24097f;

    float m_testTimer1 = 0.0f;
    float m_testTimer2 = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive> m_testShape;
    std::unique_ptr<DirectX::GeometricPrimitive> m_testShape2;

    bool m_isPauseOn = false;
};
