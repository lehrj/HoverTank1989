//
// Game.cpp
//
// 

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

namespace
{
    constexpr X3DAUDIO_CONE c_listenerCone = {
        X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f
    };
    constexpr X3DAUDIO_CONE c_emitterCone = {
        0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f
    };

    constexpr X3DAUDIO_DISTANCE_CURVE_POINT c_emitter_LFE_CurvePoints[3] = {
        { 0.0f, 1.0f }, { 0.25f, 0.0f}, { 1.0f, 0.0f }
    };
    constexpr X3DAUDIO_DISTANCE_CURVE c_emitter_LFE_Curve = {
        const_cast<X3DAUDIO_DISTANCE_CURVE_POINT*>(&c_emitter_LFE_CurvePoints[0]), 3
    };

    constexpr X3DAUDIO_DISTANCE_CURVE_POINT c_emitter_Reverb_CurvePoints[3] = {
        { 0.0f, 0.5f}, { 0.75f, 1.0f }, { 1.0f, 0.0f }
    };
    constexpr X3DAUDIO_DISTANCE_CURVE c_emitter_Reverb_Curve = {
        const_cast<X3DAUDIO_DISTANCE_CURVE_POINT*>(&c_emitter_Reverb_CurvePoints[0]), 3
    };
}

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

    srand(time(nullptr));

    m_environment = new Environment();
    m_debugData = std::make_shared<DebugData>();

    int outputWidth = 800;
    int outputHeight = 600;
    GetDefaultSize(outputWidth, outputHeight);
    m_camera = new Camera(outputWidth, outputHeight);
    m_camera->InintializePreSwingCamera(DirectX::SimpleMath::Vector3::Zero, 0.0);
    m_lighting = new Lighting();
    m_vehicle = std::make_shared<Vehicle>();
    m_vehicle->SetEnvironment(m_environment);
    m_camera->SetVehicleFocus(m_vehicle);
    m_camera->SetCameraEnvironment(m_environment);
    m_camera->SetDebugData(m_debugData);

    m_npcController = std::make_shared<NPCController>();
    m_npcController->SetNPCEnvironment(m_environment);
    m_npcController->SetDebugData(m_debugData);
    m_npcController->SetPlayer(m_vehicle);
    m_camera->SetNpcController(m_npcController);
    m_modelController = std::make_shared<ModelController>();
    m_modelController->SetDebugData(m_debugData);
    m_modelController->SetEnvironment(m_environment);
    m_vehicle->SetModelController(m_modelController);

    m_testVehicleHover = std::make_shared<VehicleHover>();

    m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
    //m_currentGameState = GameState::GAMESTATE_INTROSCREEN;
    m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_TEST01);
    m_currentUiState = UiState::UISTATE_SWING;
}

void Game::AudioFxDelete(std::shared_ptr<Utility::SoundFx> aFx)
{
    aFx->fx.reset();
    aFx->emitter.reset();
}

void Game::AudioFxReset(std::shared_ptr<Utility::SoundFx> aFx)
{
    if (aFx->fx)
    {
        aFx->fx->Play(true);
    }
}

void Game::AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK aSFX)
{
    m_audioMusicStream = m_audioBank->CreateStreamInstance(aSFX);
    //m_audioMusicStream = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_BRAVESPACEEXPLORERS);
    if (m_audioMusicStream)
    {
        m_audioMusicStream->SetVolume(m_musicVolume);
        m_audioMusicStream->Play(true);
    }
}

void Game::AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK aSFX)
{
    m_audioEffectStream = m_audioBank->CreateStreamInstance(aSFX);
    if (m_audioEffectStream)
    {
        m_audioEffectStream->SetVolume(m_sfxVolume);
        m_audioEffectStream->Play();
    }
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
    m_keyboard = std::make_unique<DirectX::Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    // Audio

    /*
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef DEBUG  //#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    */

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_EnvironmentalReverb | AudioEngine_ReverbUseFilters;
#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);

    //m_audioEngine->SetReverb(Reverb_Hangar);
    m_audioEngine->SetReverb(Reverb_Mountains);

    //m_audioEngine = std::make_unique<AudioEngine>(eflags);
    m_retryAudio = false;
    m_audioBank = std::make_unique<WaveBank>(m_audioEngine.get(), L"Art/Audio/audioBank.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audioEngine.get(), L"Art/Audio/RocketBoostEngineLoop.wav");
    //m_soundSource = m_soundEffect->CreateInstance();
    //m_soundSource = m_soundEffect->CreateInstance(SoundEffectInstance_Use3D);
    //m_soundSource->Play(true);

    m_soundSource = m_soundEffect->CreateInstance(SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
     
    //m_soundSource = m_audioBank->CreateInstance(XACT_WAVEBANK_AUDIOBANK_BRAVESPACEEXPLORERS);
    //m_soundSource = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_BRAVESPACEEXPLORERS);
    //m_soundSource = m_audioBank->CreateStreamInstance(2);

    //m_soundSource->Play(true);

    m_listener.pCone = const_cast<X3DAUDIO_CONE*>(&c_listenerCone);

    m_emitter.pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
    m_emitter.pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
    m_emitter.CurveDistanceScaler = 14.f;
    m_emitter.pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
    
    // sound stream instance testing
    m_ssiTest = m_audioBank->CreateStreamInstance(3, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
    //m_ssiTest->Play(true);

    m_soundFxVecTest.clear();

    m_fxEmitter.pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
    m_fxEmitter.pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
    m_fxEmitter.CurveDistanceScaler = 14.f;
    m_fxEmitter.pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
    //

    // Game Pad
    m_gamePad = std::make_unique<GamePad>();

    // height map intit
    bool result;
    bool isInitSuccessTrue = true;

    result = InitializeTerrainArray();
    if (!result)
    {
        isInitSuccessTrue = false;
    }

    m_terrainStartScreen.environType = EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN;
    result = InitializeTerrainArrayStartScreen(m_terrainStartScreen);
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    m_terrainGamePlay.environType = EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY;
    result = InitializeTerrainArrayNew(m_terrainGamePlay);
    if (!result)
    {
        isInitSuccessTrue = false;
    }
    if (!isInitSuccessTrue)
    {
        // add initialization failure testing  here;
        int errorBreak = 0;
        errorBreak++;
    }

    auto context = m_deviceResources->GetD3DDeviceContext();
    //m_vehicle->InitializeVehicle(context, m_npcController);
    m_vehicle->InitializeVehicle(context, m_npcController, m_vehicle);
    m_vehicle->SetDebugData(m_debugData);
    m_vehicle->PassFireControl(m_fireControl);

    m_npcController->LoadNPCs(context, m_npcController);
    //m_npcController->LoadNPCsTestFireRange(context, m_npcController);


    m_npcController->InitializeTextureMaps(NpcTextureMapType::TEXTUREMAPTYPE_BLANK, m_texture, m_normalMap, m_specular);
    m_npcController->InitializeTextureMaps(NpcTextureMapType::TEXTUREMAPTYPE_TEST1, m_textureMetalTest1, m_normalMapMetalTest1, m_specularMetalTest1);
    m_npcController->InitializeTextureMaps(NpcTextureMapType::TEXTUREMAPTYPE_TEST2, m_textureMetalTest2, m_normalMapMetalTest2, m_specularMetalTest2);
    m_npcController->InitializeTextureMaps(NpcTextureMapType::TEXTUREMAPTYPE_FLAME, m_textureFlameTest, m_normalMapFlameTest, m_specularFlameTest);

    m_fireControl->InitializeTextureMapsExplosion(m_textureFlameTest, m_normalMapFlameTest, m_specularFlameTest);
    m_camera->SetFireControl(m_fireControl);

    m_terrainVector.clear();
}

// Testing Terrain Vertex
bool Game::InitializeTerrainArray()
{
    std::vector<DirectX::VertexPositionColor> vertexPC = m_environment->GetTerrainColorVertex();

    m_terrainVertexCount = static_cast<int>(vertexPC.size());
    m_terrainVertexArray = new DirectX::VertexPositionColor[m_terrainVertexCount];
    m_terrainVertexArrayBase = new DirectX::VertexPositionColor[m_terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.0, 0.0, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor2(0.0, 0.0, 0.0, 1.0);

    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);

    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < m_terrainVertexCount; ++i)
    {
        m_terrainVertexArray[i].position = vertexPC[i].position;
        m_terrainVertexArray[i].position.y += 0.0005; // Move up the lines so they don't get hidden by terrain
        m_terrainVertexArray[i].color = lineColor;
        m_terrainVertexArrayBase[i].position = vertexPC[i].position;

        if (i % 2 == 0)
        {
            m_terrainVertexArrayBase[i].color = baseColor;
        }
        else
        {
            m_terrainVertexArrayBase[i].color = baseColor2;
        }
    }

    return true;
}

bool Game::InitializeTerrainArrayNew(Terrain& aTerrain)
{
    std::vector<DirectX::VertexPositionNormalColor> vertexPC = m_environment->GetTerrainPositionNormalColorVertex(aTerrain.environType);
    m_terrainVector2.clear();
    m_terrainVector2 = vertexPC;
    aTerrain.terrainVertexCount = static_cast<int>(vertexPC.size());
    aTerrain.terrainVertexArray = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];
    aTerrain.terrainVertexArrayBase = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.01, 0.01, 0.01, 1.0);

    DirectX::XMFLOAT4 baseColor2(1.0, 1.0, 1.0, 1.0);
    m_testColor = baseColor;
    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);
    DirectX::XMFLOAT4 testRed = DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.0, 1.0);
    DirectX::XMFLOAT4 testBlue = DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.0, 1.0);
    DirectX::XMFLOAT4 testGray = DirectX::XMFLOAT4(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    testWhite = baseColor;

    float maxY = 0.0f;

    if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        baseColor = DirectX::XMFLOAT4(0.01, 0.01, 0.01, 1.0);
        testWhite = baseColor;

    }
    if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        baseColor = DirectX::XMFLOAT4(0.000000000f, 0.292156899f, 0.000000000f, 1.000000000f);
        testWhite = baseColor;
    }
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector3 flipNormal = vertexPC[i].normal;
        aTerrain.terrainVertexArray[i].position = vertexPC[i].position;
        // Flip normals around for lighting;
        aTerrain.terrainVertexArray[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArray[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArray[i].normal.z = flipNormal.z;

        int testRandom = rand() % 1000;
        float testFloat = testRandom * 0.000001f;
        baseColor = DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 0.0f);
        float elevationPercentage = aTerrain.terrainVertexArray[i].position.y / m_gameTerrainMaxY;
        float colorVal = elevationPercentage;

        colorVal += testFloat;
        baseColor.x = colorVal;
        const float colorMax = 0.85f;
        if (baseColor.x > colorMax)
        {
            baseColor.x = colorMax;
        }
        baseColor.y = colorVal + 0.292156899f;
        if (baseColor.y > colorMax)
        {
            baseColor.y = colorMax;
        }
        baseColor.z = colorVal;
        if (baseColor.z > colorMax)
        {
            baseColor.z = colorMax;
        }

        lineColor = baseColor;

        if (elevationPercentage > 0.4f)
        {
            lineColor.z += 0.15f;
            lineColor.y -= 0.15f;
        }
        else
        {
            lineColor.y += 0.15f;
        }

        aTerrain.terrainVertexArray[i].color = lineColor;
        aTerrain.terrainVertexArrayBase[i].position = vertexPC[i].position;

        // flip normals around for lighting
        aTerrain.terrainVertexArrayBase[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArrayBase[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArrayBase[i].normal.z = flipNormal.z;

        if (i % 2 == 0)
        {
            aTerrain.terrainVertexArrayBase[i].color = baseColor;
        }
        else
        {
            aTerrain.terrainVertexArrayBase[i].color = baseColor;
        }

        if (aTerrain.terrainVertexArray[i].position.y >= maxY)
        {
            maxY = aTerrain.terrainVertexArray[i].position.y;
        }
    }

    std::vector<DirectX::SimpleMath::Vector3> testNorms;
    testNorms.resize(aTerrain.terrainVertexCount);
    std::vector<DirectX::SimpleMath::Vector3> testNorms2;
    testNorms2.resize(aTerrain.terrainVertexCount);
    float gridLineOffSetY = 0.0f;
    if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        gridLineOffSetY = 0.003f;
    }
    else if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        gridLineOffSetY = 0.3f;
    }
    gridLineOffSetY = 0.22f;
    const float gridLineOffSetY2 = 1.1f;
    //const float gridLineOffSetY2 = 0.1f;
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        if (aTerrain.terrainVertexArray[i].position.y > 5.0f)
        {
            aTerrain.terrainVertexArray[i].position.y += gridLineOffSetY2;
        }
        else
        {
            aTerrain.terrainVertexArray[i].position.y += gridLineOffSetY;
        }
        testNorms[i] = aTerrain.terrainVertexArray[i].normal;
        testNorms2[i] = aTerrain.terrainVertexArrayBase[i].normal;
    }

    return true;
}

bool Game::InitializeTerrainArrayStartScreen(Terrain& aTerrain)
{
    std::vector<DirectX::VertexPositionNormalColor> vertexPC = m_environment->GetTerrainPositionNormalColorVertex(aTerrain.environType);
    m_terrainVector2.clear();
    m_terrainVector2 = vertexPC;
    aTerrain.terrainVertexCount = static_cast<int>(vertexPC.size());
    aTerrain.terrainVertexArray = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];
    aTerrain.terrainVertexArrayBase = new DirectX::VertexPositionNormalColor[aTerrain.terrainVertexCount];

    DirectX::XMFLOAT4 lineColor(.486274540f, .988235354f, 0.0, 1.0);
    DirectX::XMFLOAT4 baseColor(0.01, 0.01, 0.01, 1.0);
    DirectX::XMFLOAT4 baseColor2(1.0, 1.0, 1.0, 1.0);
    m_testColor = baseColor;
    DirectX::XMFLOAT4 sandColor1(0.956862807f, 0.643137276f, 0.376470625f, 1.0);
    DirectX::XMFLOAT4 sandColor2(0.960784376f, 0.960784376f, 0.862745166f, 1.0);
    DirectX::XMFLOAT4 greenColor1 = DirectX::XMFLOAT4(0.0, 0.501960814f, 0.0, 1.0);
    DirectX::XMFLOAT4 greenColor2 = DirectX::XMFLOAT4(0.486274540f, 0.988235354f, 0.0, 1.0);

    DirectX::XMFLOAT4 grassColor1 = DirectX::XMFLOAT4(0.133333340f, 0.545098066f, 0.133333340f, 1.0);
    DirectX::XMFLOAT4 grassColor2 = DirectX::XMFLOAT4(0.000000000f, 0.392156899f, 0.0, 1.0);
    DirectX::XMFLOAT4 testRed = DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.0, 1.0);
    DirectX::XMFLOAT4 testBlue = DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.0, 1.0);
    DirectX::XMFLOAT4 testGray = DirectX::XMFLOAT4(0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f);
    DirectX::XMFLOAT4 testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    testWhite = baseColor;

    testWhite = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
    baseColor = m_defaultStartTerrainColor;
    float maxY = 0.0f;
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        DirectX::SimpleMath::Vector3 flipNormal = vertexPC[i].normal;
        aTerrain.terrainVertexArray[i].position = vertexPC[i].position;
        // Flip normals around for lighting;
        aTerrain.terrainVertexArray[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArray[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArray[i].normal.z = flipNormal.z;

        int testRandom = rand() % 1000;
        float testFloat = testRandom * 0.000001f;

        DirectX::XMFLOAT4 testColor(0.0f, 0.0f, 0.0f, 0.0f);
        float colorVal = aTerrain.terrainVertexArray[i].position.y / (m_startTerrainMaxY + 0.15f);
        colorVal += testFloat;
        colorVal -= 0.05;
        if (colorVal < 0.0f)
        {
            colorVal = 0.0f;
        }
        testColor.x = colorVal;
        if (testColor.x > 1.0f)
        {
            testColor.x = 1.0f;
        }
        testColor.y = colorVal;
        if (testColor.y > 1.0f)
        {
            testColor.y = 1.0f;
        }
        testColor.z = colorVal;
        if (testColor.z > 1.0f)
        {
            testColor.z = 1.0f;
        }
        DirectX::XMFLOAT4 testColor2 = testColor;
        testColor2.y -= 0.15f;

        aTerrain.terrainVertexArray[i].color = lineColor;
        aTerrain.terrainVertexArrayBase[i].position = vertexPC[i].position;

        //m_terrainVertexArrayBase2[i].normal = vertexPC[i].normal;
        // flip normals around for lighting
        aTerrain.terrainVertexArrayBase[i].normal.x = flipNormal.x;
        aTerrain.terrainVertexArrayBase[i].normal.y = flipNormal.y;
        aTerrain.terrainVertexArrayBase[i].normal.z = flipNormal.z;

        if (i % 2 == 0)
        {
            aTerrain.terrainVertexArrayBase[i].color = testColor;
        }
        else
        {
            aTerrain.terrainVertexArrayBase[i].color = testColor;
        }

        if (aTerrain.terrainVertexArrayBase[i].position.y > (m_startTerrainMaxY - 0.1f))
        {
            aTerrain.terrainVertexArrayBase[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (aTerrain.terrainVertexArray[i].position.y >= maxY)
        {
            maxY = aTerrain.terrainVertexArray[i].position.y;
        }
    }

    std::vector<DirectX::SimpleMath::Vector3> testNorms;
    testNorms.resize(aTerrain.terrainVertexCount);
    std::vector<DirectX::SimpleMath::Vector3> testNorms2;
    testNorms2.resize(aTerrain.terrainVertexCount);
    float gridLineOffSetY = 0.0f;
    if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        gridLineOffSetY = 0.003f;
    }
    else if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        gridLineOffSetY = 4.7f;
    }
    for (int i = 0; i < aTerrain.terrainVertexCount; ++i)
    {
        aTerrain.terrainVertexArray[i].position.y += gridLineOffSetY;
        testNorms[i] = aTerrain.terrainVertexArray[i].normal;
        testNorms2[i] = aTerrain.terrainVertexArrayBase[i].normal;
    }

    return true;
}

Game::~Game()
{
    if (m_audioEngine)
    {
        m_audioEngine->Suspend();
    }
    m_audioMusicStream.reset();
    m_audioEffectStream.reset();
    m_soundSource.reset();
    m_ssiTest.reset();

    for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
    {
        AudioFxDelete(m_soundFxVecTest[i]);
    }
    m_soundFxVecTest.clear();

    delete m_camera;
    delete m_environment;
    delete m_lighting;
    //delete m_vehicle;

    delete[] m_terrainGamePlay.terrainVertexArray;
    m_terrainGamePlay.terrainVertexArray = 0;
    delete[] m_terrainGamePlay.terrainVertexArrayBase;
    m_terrainGamePlay.terrainVertexArrayBase = 0;

    delete[] m_terrainStartScreen.terrainVertexArray;
    m_terrainStartScreen.terrainVertexArray = 0;
    delete[] m_terrainStartScreen.terrainVertexArrayBase;
    m_terrainStartScreen.terrainVertexArrayBase = 0;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

void Game::ToggleMusicFadeOut()
{
    m_isMusicFadeOutTrue = true;
}

void Game::TogglePause()
{
    if (m_isPauseOn == true)
    {
        m_isPauseOn = false;
    }
    else
    {
        m_isPauseOn = true;
    }
}

// Updates the world.
void Game::Update(DX::StepTimer const& aTimer)
{
    float elapsedTime = float(aTimer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    //UpdateInput(aTimer);

    if (m_npcController->GetIsDebugPauseToggleTrue() == true)
    {
        m_isPauseOn = true;
    }
    else
    {
        m_isPauseOn = false;
    }

    if (m_isPauseOn == false)
    {
        m_debugData->DebugClearUI();
        m_testTimer1 += static_cast<float>(aTimer.GetElapsedSeconds());

        m_vehicle->UpdateVehicle(aTimer.GetElapsedSeconds());
        m_modelController->UpdatePlayerModel(m_vehicle->GetAlignment(), m_vehicle->GetAltitude(), m_vehicle->GetPos(), m_vehicle->GetWeaponPitch(), m_vehicle->GetTurretYaw(), m_vehicle->GetGroundPlane());
        m_vehicle->UpdateVehicleFireControl(aTimer.GetElapsedSeconds());
        m_npcController->UpdateNPCController(m_camera->GetCameraFrustum() , aTimer.GetElapsedSeconds());

        auto context = m_deviceResources->GetD3DDeviceContext();
        m_npcController->UpdateLoadQueue(context, m_npcController, aTimer.GetElapsedSeconds());
    }
    UpdateInput(aTimer);
    m_camera->UpdateCamera(aTimer);
    m_environment->SetCameraPos(m_camera->GetPos());
    //m_lighting->UpdateLighting(m_effect, aTimer.GetTotalSeconds());

    m_proj = m_camera->GetProjectionMatrix();
    m_effect->SetProjection(m_proj);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetProjection(m_proj);

    DirectX::SimpleMath::Matrix viewMatrix = m_camera->GetViewMatrix();
    m_effect->SetView(viewMatrix);
    m_effect2->SetView(viewMatrix);
    m_effect3->SetView(viewMatrix);

    if (m_isSlowMoOn == true)
    {
        m_isPauseOn = true;
    }
    else if (m_isSlowMoOn == false)
    {
        m_isPauseOn = false;
    }

    // audio
    if (m_isMusicFadeOutTrue == true)
    {
        FadeOutMusic();
    }

    if (m_retryAudio)
    {
        m_retryAudio = false;
        if (m_audioEngine->Reset())
        {
            // ToDo: restart any looped sounds here
            if (m_audioMusicStream)
            {
                //m_audioMusicStream->Play(true);
            }
            if (m_audioEffectStream)
            {
                //m_audioEffectStream->Play(); // WLJ this could lead to problems and might not be needed, maybe cause unwanted effect to play after reset?
            }

            if (m_soundSource)
            {
                //m_soundSource->Play(true);
            }

            if (m_ssiTest)
            {
                //m_ssiTest->Play(true);
            }

            for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
            {
                AudioFxReset(m_soundFxVecTest[i]);
            }
        }
    }
    else if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            m_retryAudio = true;
        }
    }

    m_emitter.Update(m_vehicle->GetPos(), m_vehicle->GetVehicleUp(), static_cast<float>(aTimer.GetElapsedSeconds()));
    m_fxEmitter.Update(m_vehicle->GetPos(), m_vehicle->GetVehicleUp(), static_cast<float>(aTimer.GetElapsedSeconds()));
    m_listener.SetOrientation(m_camera->GetForwardAudio(), m_camera->GetUpAudio());
    m_listener.Update(m_camera->GetPos(), m_camera->GetUpAudio(), aTimer.GetElapsedSeconds());
   
    if (m_soundSource)
    {
        m_soundSource->Apply3D(m_listener, m_emitter);
    }

    if (m_ssiTest)
    {
        m_ssiTest->Apply3D(m_listener, m_emitter);
    }
    
    UpdateAudioFx(aTimer);

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullNone());

    m_effect->Apply(context);
    auto sampler = m_states->LinearClamp();
    context->PSSetSamplers(0, 1, &sampler);
    context->IASetInputLayout(m_inputLayout.Get());

    m_effect->SetProjection(m_proj);
    m_effect->SetView(m_camera->GetViewMatrix());
    m_effect->SetWorld(m_world);

    auto ilights = dynamic_cast<DirectX::IEffectLights*>(m_effect.get());
    if (ilights)
    {
        ilights->EnableDefaultLighting();
        ilights->SetLightEnabled(0, true);
        ilights->SetLightEnabled(1, true);
        ilights->SetLightEnabled(2, true);
    }

    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawSky2(m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);

        m_modelController->DrawModel(context, *m_states, m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);
 
        m_npcController->DrawNPCs(m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);
        m_vehicle->DrawVehicleProjectiles2(m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);
        //DrawSky();
        //DrawSky2(m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);
        DrawTestTrack();
        //DrawTestRangeMissile();
        DrawSpawner();
        //DrawSky2Base(m_camera->GetViewMatrix(), m_proj, m_effect, m_inputLayout);
    }

    m_effect->EnableDefaultLighting();
    m_effect->SetWorld(m_world);

    m_effect2->SetWorld(m_world);
    m_effect2->Apply(context);
    context->IASetInputLayout(m_inputLayout2.Get());
    m_batch2->Begin();
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawTerrainNew(m_terrainGamePlay);
    }
    m_batch2->End();

    m_effect3->SetWorld(m_world);
    m_effect3->Apply(context);
    context->IASetInputLayout(m_inputLayout3.Get());
    m_batch3->Begin();

    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawDebugLinesVector();
    }
    m_batch3->End();

    m_spriteBatch->Begin();
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        DrawDebugDataUI();

        if (m_isDisplayCountdownTrue == true)
        {
            DrawUnlockUI();
        }
        if (m_isDisplayEndScreenTrue == true)
        {
            DrawEndUI();
        }
        if (m_isUiDisplayTrue == true)
        {
            DrawUIDisplay();
        }
    }
    m_spriteBatch->End();

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
    m_gamePad->Resume();
    m_buttons.Reset();
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
    m_gamePad->Suspend();
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
    m_audioEngine->Suspend();
    m_gamePad->Suspend();
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
    m_gamePad->Resume();
    m_buttons.Reset();
    m_kbStateTracker.Reset();
    m_audioEngine->Resume();
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
    m_camera->OnResize(std::max(width, 1), std::max(height, 1));
    m_proj = m_camera->GetProjectionMatrix();
    m_effect->SetProjection(m_proj);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetProjection(m_proj);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1600;
    height = 900;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

    // sky texture
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/skyTexture.jpg", nullptr, m_textureSky.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/blankTexture.jpg", nullptr, m_textureSky.ReleaseAndGetAddressOf()));

    // metal tests
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/blankTexture.jpg", nullptr, m_textureMetalTest1.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/blankNormal.jpg", nullptr, m_normalMapMetalTest1.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specularMetalTest1.ReleaseAndGetAddressOf())); 
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/metalScarredNormal.jpg", nullptr, m_normalMapMetalTest1.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/metalScarredSpecular.jpg", nullptr, m_specularMetalTest1.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/metalRustNormal.jpg", nullptr, m_normalMapMetalTest1.ReleaseAndGetAddressOf()));
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/metalRustSpecular.jpg", nullptr, m_specularMetalTest1.ReleaseAndGetAddressOf()));

    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/blankTexture.jpg", nullptr, m_textureMetalTest2.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/blankNormal.jpg", nullptr, m_normalMapMetalTest2.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specularMetalTest2.ReleaseAndGetAddressOf()));

    // flame test
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/blankTexture.jpg", nullptr, m_textureFlameTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/blankNormal.jpg", nullptr, m_normalMapFlameTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specularFlameTest.ReleaseAndGetAddressOf()));
    
    // Blank textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/blankTexture.jpg", nullptr, m_texture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/blankNormal.jpg", nullptr, m_normalMap.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specular.ReleaseAndGetAddressOf()));

    // test textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/TestOP.png", nullptr, m_textureTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/TestOP.png", nullptr, m_normalMapTest.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/SpecularTeaser1.png", nullptr, m_specularTest.ReleaseAndGetAddressOf()));

    // Jackson Industry textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/logoJI2.png", nullptr, m_textureJI.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/normJI.png", nullptr, m_normalMapJI.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/specularJI2.png", nullptr, m_specularJI.ReleaseAndGetAddressOf()));

    // BMW textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/logoBMW.png", nullptr, m_textureBMW.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/normBMW.png", nullptr, m_normalMapBMW.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/normBMW.png", nullptr, m_normalMapBMW2.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/specularBMW.png", nullptr, m_specularBMW.ReleaseAndGetAddressOf()));
    // Start screen textures
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/textureUV.png", nullptr, m_textureAutoGame.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/normalUV.png", nullptr, m_normalMapAutoGame.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/specularUV.png", nullptr, m_specularAutoGame.ReleaseAndGetAddressOf()));

    // Textures for teaser trailer    
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/LogoTeaser.png", nullptr, m_textureTeaser.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/NormalMaps/NormalMapTeaser.png", nullptr, m_normalMapTeaser.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/SpecularTeaser.png", nullptr, m_specularTeaser.ReleaseAndGetAddressOf()));

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);

    m_world = DirectX::SimpleMath::Matrix::Identity;
    m_effect = std::make_unique<NormalMapEffect>(device);
   
    // Make sure you called CreateDDSTextureFromFile and CreateWICTextureFromFile before this point!
    m_effect->SetTexture(m_texture.Get());
    m_effect->SetNormalTexture(m_normalMap.Get());
    m_effect->SetLightDiffuseColor(0, Colors::White);
    m_effect->SetAlpha(1.0);
    m_effect->SetFogEnabled(false);
    m_effect->SetFogColor(DirectX::Colors::Black);
    m_effect->SetFogStart(1.0);
    m_effect->SetFogEnd(4.0);
    /////////////////////////////

    m_effect->SetLightEnabled(0, true);
    m_effect->SetLightDiffuseColor(0, Colors::Blue);
    m_effect->SetLightDirection(0, -DirectX::SimpleMath::Vector3::UnitZ);

    //m_effect->EnableDefaultLighting();
    m_effect->SetLightEnabled(0, true);
    m_effect->SetLightEnabled(1, true);
    m_effect->SetLightEnabled(2, true);
    m_effect->SetLightDirection(0, -DirectX::SimpleMath::Vector3::UnitY);
    m_effect->SetLightDirection(1, -DirectX::SimpleMath::Vector3::UnitY);
    m_effect->SetLightDirection(2, -DirectX::SimpleMath::Vector3::UnitY);
    m_effect->SetAmbientLightColor(Colors::Blue);

    m_effect2 = std::make_unique<BasicEffect>(device);
    m_effect2->SetVertexColorEnabled(true);

    m_effect3 = std::make_unique<BasicEffect>(device);
    m_effect3->SetVertexColorEnabled(true);

    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType2>(device, m_effect2.get(), m_inputLayout2.ReleaseAndGetAddressOf()));
    const int maxVertices = 8192;
    const int maxIndices = maxVertices * 3;
    m_batch2 = std::make_unique<PrimitiveBatch<VertexType2>>(context, maxIndices, maxVertices);

    //m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch = std::make_unique<PrimitiveBatch<VertexType>>(context);

    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType3>(device, m_effect3.get(), m_inputLayout3.ReleaseAndGetAddressOf()));
    m_batch3 = std::make_unique<PrimitiveBatch<VertexType3>>(context);

    //m_skyShape = GeometricPrimitive::CreateSphere(context, m_skyBoxSize, 32, false);
    m_skyShape = GeometricPrimitive::CreateSphere(context, -m_skyBoxSize, 32);
    m_skyShape->CreateInputLayout(m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf());

    m_font = std::make_unique<SpriteFont>(device, L"Art/Fonts/myfile.spritefont");
    m_titleFont = std::make_unique<SpriteFont>(device, L"Art/Fonts/titleFont.spritefont");
    m_bitwiseFont = std::make_unique<SpriteFont>(device, L"Art/Fonts/bitwise24.spritefont");
    m_spriteBatch = std::make_unique<SpriteBatch>(context);


    m_modelTank01 = Model::CreateFromCMO(device, L"HoverTankTest.cmo", *m_fxFactory);
    m_modelTankBody01 = Model::CreateFromCMO(device, L"HoverTankBody02.cmo", *m_fxFactory);
    m_modelTankTurret01 = Model::CreateFromCMO(device, L"HoverTankTurret02.cmo", *m_fxFactory);
    m_modelTankBarrel01 = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);

    m_modelTest = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);

    m_modelTestBarrel = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);
    m_modelTestBody = Model::CreateFromCMO(device, L"HoverTankBody02.cmo", *m_fxFactory);
    m_modelTestTurret = Model::CreateFromCMO(device, L"HoverTankTurret02.cmo", *m_fxFactory);

    m_modelTestBody->UpdateEffects([](IEffect* effect)
        {
            auto lights = dynamic_cast<IEffectLights*>(effect);
            if (lights)
            {

                lights->SetLightingEnabled(true);
                lights->SetPerPixelLighting(true);
                lights->SetLightEnabled(0, true);
                lights->SetLightDiffuseColor(0, Colors::Gold);
                lights->SetLightEnabled(1, true);
                lights->SetLightEnabled(2, true);
                lights->SetLightDirection(0, -DirectX::SimpleMath::Vector3::UnitY);
                lights->SetLightDirection(1, -DirectX::SimpleMath::Vector3::UnitY);
                lights->SetLightDirection(2, -DirectX::SimpleMath::Vector3::UnitY);
                lights->EnableDefaultLighting();

            }
            /*
            auto fog = dynamic_cast<IEffectFog*>(effect);
            if (fog)
            {
                fog->SetFogEnabled(true);
                fog->SetFogColor(Colors::CornflowerBlue);
                fog->SetFogStart(3.f);
                fog->SetFogEnd(400.f);
            }
            */
        });

    //m_modelController->InitializePlayerModel(m_modelTestBarrel, m_modelTestBody, m_modelTestTurret);
    m_modelController->InitializePlayerModel(m_modelTestBarrel, m_modelTestBody, m_modelTestTurret, context);

    m_testShape = DirectX::GeometricPrimitive::CreateCylinder(context, 1.0f, 80.0f);
    m_testShape2 = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(250.0f, 1.0f, 6.0f));
    //m_testShape2 = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(m_missileRangeDistance, 1.0f, 6.0f));
  
    //m_testShape3 = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    //m_testShape3 = DirectX::GeometricPrimitive::CreateCylinder(context, -m_baseTerrainHeight, -m_baseTerrainDiameter);
    m_testShape3 = DirectX::GeometricPrimitive::CreateTorus(context, m_ringDiameter, m_ringHeight);

    const float spawnerHeight = 50.0f;
    const float spawnerDiameter = 50.0f;
    const float spawnerThickness = 10.0f;
    const float spawnerScale = 15.0f;
    auto pos = DirectX::SimpleMath::Vector3(200.0f, 0.0f, 0.0f);
    m_spawnerOuterShape = DirectX::GeometricPrimitive::CreateTorus(context, spawnerDiameter, spawnerThickness);
    m_spawnerOuterMat = DirectX::SimpleMath::Matrix::Identity;
    m_spawnerOuterMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    m_spawnerOuterMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, spawnerScale));
    m_spawnerOuterMat *= DirectX::SimpleMath::Matrix::CreateWorld(m_spawnerPos, DirectX::SimpleMath::Vector3::UnitZ, DirectX::SimpleMath::Vector3::UnitY);

    const float spawnerInnerSize = spawnerDiameter;
    m_spawnerInnerShape = DirectX::GeometricPrimitive::CreateCylinder(context, 2.0f, spawnerInnerSize);
    m_spawnerInnerMat = DirectX::SimpleMath::Matrix::Identity;
    m_spawnerInnerMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    //m_spawnerInnerMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    //m_spawnerInnerMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 10.0f));
    m_spawnerInnerMat *= DirectX::SimpleMath::Matrix::CreateWorld(m_spawnerPos, DirectX::SimpleMath::Vector3::UnitZ, DirectX::SimpleMath::Vector3::UnitY);
}

void Game::DrawSpawner()
{
    m_spawnerOuterShape->Draw(m_spawnerOuterMat, m_camera->GetViewMatrix(), m_proj);
    m_spawnerInnerShape->Draw(m_spawnerInnerMat, m_camera->GetViewMatrix(), m_proj, DirectX::Colors::Black);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    auto size = m_deviceResources->GetOutputSize();
    m_view = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3(2.f, 2.f, 2.f), DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);
    m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, float(size.right) / float(size.bottom), m_camera->GetViewPlaneNear(), m_camera->GetViewPlaneFar());

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect2->SetView(m_view);
    m_effect2->SetProjection(m_proj);
    m_effect3->SetView(m_view);
    m_effect3->SetProjection(m_proj);
}

void Game::DrawDebugDataUI()
{
    std::vector<std::string> uiVector = m_debugData->DebugGetUIVector();
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 200;
    textLinePos.y += 30;
    for (int i = 0; i < uiVector.size(); ++i)
    {
        std::string textLine = uiVector[i];
        DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
        textLinePos.x = textLineOrigin.x + 20;
        m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
        textLinePos.y += 30;
    }

    /*
    // Draw Timer
    //std::string textLine = "Timer  " + std::to_string(m_timer.GetTotalSeconds());
    std::string textLine = "Timer  " + std::to_string(m_testTimer1);
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */

    /*
    if (m_vehicle->GetIsDebugToggled() == true)
    {
        textLine = "DebugToggle = true";
    }
    else
    {
        textLine = "DebugToggle = false";
    }
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    if (m_vehicle->GetIsDebugToggled2() == true)
    {
        textLine = "DebugToggle2 = true";
    }
    else
    {
        textLine = "DebugToggle2 = false";
    }
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    if (m_vehicle->GetIsDebugToggled3() == true)
    {
        textLine = "DebugToggle3 = true";
    }
    else
    {
        textLine = "DebugToggle3 = false";
    }
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */

    std::string textLine = "FPS   " + std::to_string(m_timer.GetFramesPerSecond());
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    /*
    textLine = "m_soundFxVecTest   " + std::to_string(m_soundFxVecTest.size());
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */

    
    textLine = "m_currentFxShotBang   " + std::to_string(m_currentFxShotBang);
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;


    textLine = "m_currentFxExplosion   " + std::to_string(m_currentFxExplosion);
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

}

void Game::DrawEndUI()
{
    m_endScreenTimer += static_cast<float>(m_timer.GetElapsedSeconds());
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 960;
    textLinePos.y = 540;
    const float maxScale = 3.0f;

    float fontScale = m_endScreenTimer * 0.5f;
    fontScale *= 3.0f;
    if (fontScale > maxScale)
    {
        fontScale = maxScale;
    }
    std::string textLine = "Thanks for watching";
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Black, 0.f, textLineOrigin, fontScale);
    const float shiftMod = -2.0f;
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::DimGray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin, fontScale);
}

void Game::DrawUIDisplay()
{
    m_uiDisplayTimer += static_cast<float>(m_timer.GetElapsedSeconds());
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 960;
    textLinePos.y = 540;

    textLinePos.x = static_cast<float>(m_camera->GetWindowWidth() * 0.02f);
    textLinePos.y = static_cast<float>(m_camera->GetWindowHeight() * 0.9f);
    float fontScale = 3.0f;

    std::string textLine = m_uiAmmoDisplayString;

    if (m_uiDisplayTimer < m_uiDisplayTypeDuration)
    {
        int textSize = static_cast<int>(textLine.size());
        float ratio = m_uiDisplayTimer / m_uiDisplayTypeDuration;
        int displaySize = static_cast<int>(ratio * static_cast<float>(textSize));
        textLine.resize(displaySize);
    }

    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.0f;
    textLineOrigin.x = 0.0f;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Black, 0.f, textLineOrigin, fontScale);
    const float shiftMod = -2.0f;
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::DimGray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin, fontScale);

    if (m_uiDisplayTimer > m_uiDisplayDuration)
    {
        m_isUiDisplayTrue = false;
    }
}

void Game::DrawUnlockUI()
{
    m_unlockTimer1 += static_cast<float>(m_timer.GetElapsedSeconds());
    DirectX::SimpleMath::Vector2 textLinePos = m_fontPos2;
    textLinePos.x = 870;
    textLinePos.y = 890;
    std::string textLine = "Unlocking Jump Boosters in :   ";
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Black, 0.f, textLineOrigin, 3.0f);
    const float shiftMod = -2.0f;
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::DimGray, 0.f, textLineOrigin, 3.0f);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, 3.0f);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, 3.0f);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin, 3.0f);

    textLinePos.x = 1580;
    const float maxScale = 3.0f;
    float fontScale = (cos(m_unlockTimer1) + 1.0f) * 2.5f;
    fontScale = m_unlockTimer2;
    fontScale *= 3.0f;
    if (fontScale > maxScale)
    {
        fontScale = maxScale;
    }

    textLine = std::to_string(m_unlockCountdown);
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.y -= fontScale;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Black, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::DimGray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::Gray, 0.f, textLineOrigin, fontScale);
    textLinePos.x += shiftMod;
    textLinePos.y += shiftMod;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin, fontScale);

    m_unlockTimer2 += static_cast<float>(m_timer.GetElapsedSeconds());
    if (m_unlockTimer2 > 1.0f)
    {
        m_unlockCountdown--;
        m_unlockTimer2 = 0.0f;
    }

    if (m_unlockCountdown <= 0)
    {
        m_unlockCountdown = m_unlockCountDownTicks;
        m_unlockTimer1 = 0.0f;
        m_npcController->UnlockJumpAbility();
        m_isDisplayCountdownTrue = false;
    }
}

void Game::DrawDebugLinesVector()
{
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> lineTup = m_debugData->DebugGetTestLines();
    for (unsigned int i = 0; i < lineTup.size(); ++i)
    {
        DirectX::SimpleMath::Vector4 lineColor = std::get<2>(lineTup[i]);
        VertexPositionColor lineStart(std::get<0>(lineTup[i]), lineColor);
        VertexPositionColor lineEnd(std::get<1>(lineTup[i]), lineColor);
        m_batch3->DrawLine(lineStart, lineEnd);
    }

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32>> lineTupVec = m_debugData->GetDebugLinesVec();
    for (unsigned int i = 0; i < lineTupVec.size(); ++i)
    {
        DirectX::XMVECTORF32 lineColor = std::get<2>(lineTupVec[i]);
        VertexPositionColor lineStart(std::get<0>(lineTupVec[i]), lineColor);
        VertexPositionColor lineEnd(std::get<1>(lineTupVec[i]), lineColor);
        m_batch3->DrawLine(lineStart, lineEnd);
    }
}

void Game::DrawSky()
{
    m_skyRotation += static_cast<float>(m_timer.GetElapsedSeconds()) * 0.19f;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-m_skyRotation));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(30.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());
}

void Game::DrawSky2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    m_skyRotation += static_cast<float>(m_timer.GetElapsedSeconds()) * 0.19f;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-m_skyRotation));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(30.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    //m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());
    //m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());

    aEffect->EnableDefaultLighting();

    DirectX::SimpleMath::Vector4 skyColor = DirectX::SimpleMath::Vector4(0.f, 0.749019623f, 1.f, 1.f);
    aEffect->SetColorAndAlpha(skyColor);
    DirectX::SimpleMath::Vector3 skyBoxTransVec = DirectX::SimpleMath::Vector3::Zero;
    skyBoxTransVec.y -= m_skyBoxSize * 0.17f;

    DirectX::SimpleMath::Matrix worldMat = DirectX::SimpleMath::Matrix::Identity;
    worldMat = DirectX::SimpleMath::Matrix::Identity;
    worldMat *= DirectX::SimpleMath::Matrix::CreateTranslation(skyBoxTransVec);
    aEffect->SetWorld(worldMat);
    m_skyShape->Draw(aEffect.get(), aInputLayout.Get(), false, true);
}

void Game::DrawSky2Base(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    aEffect->EnableDefaultLighting();

    aEffect->SetColorAndAlpha(DirectX::SimpleMath::Vector4(0.f, 0.f, 0.0f, 0.f));

    DirectX::SimpleMath::Vector3 baseTransVec = DirectX::SimpleMath::Vector3::Zero;
    baseTransVec.y = m_ringOffset;

    DirectX::SimpleMath::Matrix worldMat = DirectX::SimpleMath::Matrix::Identity;
    worldMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, m_ringScale, 1.0f));
    worldMat *= DirectX::SimpleMath::Matrix::CreateTranslation(baseTransVec);
    aEffect->SetWorld(worldMat);
    m_testShape3->Draw(aEffect.get(), aInputLayout.Get());
}

void Game::DrawTestRangeMissile()
{
    const DirectX::SimpleMath::Vector3 wPos1 = m_missileRangePos1;
    const DirectX::SimpleMath::Vector3 wPos2 = m_missileRangePos2;
    const float wpDistance = wPos1.z - wPos2.z;
    const float rodOffset = 0.05f;
    // waypoint 1 & cross bar
    DirectX::SimpleMath::Vector3 pos1 = wPos1;
    DirectX::SimpleMath::Matrix posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Vector3 pos2 = pos1;
    pos2.z -= wpDistance * 0.5f;
    pos2.y -= rodOffset;
    DirectX::SimpleMath::Matrix posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // waypoint 2
    pos1 = wPos2;
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
}

void Game::DrawTestTrack()
{
    const float yOffset = 0.5f;
    const float rodOffset = 0.05f;
    const float xOffset = 300.0f;
    // inside top left
    DirectX::SimpleMath::Vector3 pos1(300.0f + xOffset, yOffset, -100.0f);
    DirectX::SimpleMath::Matrix posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Vector3 pos2 = pos1;
    pos2.z -= 100.0f;
    pos2.y -= rodOffset;
    DirectX::SimpleMath::Matrix posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside top left
    pos1 = DirectX::SimpleMath::Vector3(300.0f + xOffset, yOffset, -300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);

    // inside top right
    pos1 = DirectX::SimpleMath::Vector3(300.0f + xOffset, yOffset, 100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside top right
    pos1 = DirectX::SimpleMath::Vector3(300.0f + xOffset, yOffset, 300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);

    // inside bottom right
    pos1 = DirectX::SimpleMath::Vector3(75.0f + xOffset, yOffset, 100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside bottom right
    pos1 = DirectX::SimpleMath::Vector3(75.0f + xOffset, yOffset, 300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.x += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // inside bottom left
    pos1 = DirectX::SimpleMath::Vector3(75.0f + xOffset, yOffset, -100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z -= 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside bottom left
    pos1 = DirectX::SimpleMath::Vector3(75.0f + xOffset, yOffset, -300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.x += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // cross 1
    pos2 = DirectX::SimpleMath::Vector3(187.0f + xOffset, yOffset - rodOffset, 0.0f);
    const float angle = 50.0f;
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(angle));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);
    // cross 2
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-angle));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);
}

void Game::DrawTerrainNew(Terrain& aTerrain)
{
    //m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, aTerrain.terrainVertexArrayBase, aTerrain.terrainVertexCount);
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, aTerrain.terrainVertexArray, aTerrain.terrainVertexCount);

    // Create base to black out skydome under terrain
    DirectX::SimpleMath::Vector3 vertNorm = -DirectX::SimpleMath::Vector3::UnitY;
    const float baseSize = m_skyBoxSize;
    const float heightOffSet = -m_groundBlackHeight;

    DirectX::SimpleMath::Vector3 v1 = DirectX::SimpleMath::Vector3(baseSize, heightOffSet, -baseSize);
    DirectX::SimpleMath::Vector3 v2 = DirectX::SimpleMath::Vector3(baseSize, heightOffSet, baseSize);
    DirectX::SimpleMath::Vector3 v3 = DirectX::SimpleMath::Vector3(-baseSize, heightOffSet, baseSize);
    DirectX::SimpleMath::Vector3 v4 = DirectX::SimpleMath::Vector3(-baseSize, heightOffSet, -baseSize);

    DirectX::SimpleMath::Vector4 vertColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::VertexPositionNormalColor vpn1(v1, vertNorm, vertColor);
    DirectX::VertexPositionNormalColor vpn2(v2, vertNorm, vertColor);
    DirectX::VertexPositionNormalColor vpn3(v3, vertNorm, vertColor);
    DirectX::VertexPositionNormalColor vpn4(v4, vertNorm, vertColor);

    m_batch2->DrawQuad(vpn1, vpn2, vpn3, vpn4);
}

void Game::FadeOutMusic()
{
    if (m_isMusicFadeOutTrue == true)
    {
        float timeStep = static_cast<float>(m_timer.GetElapsedSeconds());
        float fadeVal = timeStep / m_musicFadeOutDuration;
        m_musicVolume -= fadeVal;
        if (m_musicVolume <= 0.0f)
        {
            m_musicVolume = 0.0f;
            m_isMusicFadeOutTrue = false;
        }
        m_audioMusicStream->SetVolume(m_musicVolume);
    }
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    m_states.reset();
    m_fxFactory.reset();

    m_modelTank01.reset();
    m_modelTankBody01.reset();
    m_modelTankTurret01.reset();
    m_modelTankBarrel01.reset();

    m_modelTest.reset();
    m_modelTestBarrel.reset();
    m_modelTestBody.reset();
    m_modelTestTurret.reset();

    m_raster.Reset(); // anti-aliased lines
    m_states.reset();
    m_effect.reset();
    m_effect2.reset();
    m_effect3.reset();
    m_batch.reset();
    m_batch2.reset();
    m_batch3.reset();
    m_skyShape.reset();
    m_normalMap.Reset();
    m_texture.Reset();
    m_specular.Reset();
    m_textureSky.Reset();
    m_normalMapTest.Reset();
    m_textureTest.Reset();
    m_specularTest.Reset();

    m_normalMapMetalTest1.Reset();
    m_textureMetalTest1.Reset();
    m_specularMetalTest1.Reset();

    m_normalMapMetalTest2.Reset();
    m_textureMetalTest2.Reset();
    m_specularMetalTest2.Reset();

    m_normalMapFlameTest.Reset();
    m_textureFlameTest.Reset();
    m_specularFlameTest.Reset();

    m_normalMapJI.Reset();
    m_specularJI.Reset();
    m_textureJI.Reset();
    m_normalMapBMW.Reset();
    m_normalMapBMW2.Reset();
    m_specularBMW.Reset();
    m_textureBMW.Reset();
    m_normalMapAutoGame.Reset();
    m_specularAutoGame.Reset();
    m_textureAutoGame.Reset();
    m_normalMapTeaser.Reset();
    m_specularTeaser.Reset();
    m_textureTeaser.Reset();

    m_inputLayout.Reset();
    m_inputLayout2.Reset();
    m_inputLayout3.Reset();

    m_font.reset();
    m_titleFont.reset();
    m_bitwiseFont.reset();
    m_spriteBatch.reset();
    m_kbStateTracker.Reset();

    // Charcters
    m_character.reset();
    m_characterTexture.Reset();
    m_character0.reset();
    m_character0Texture.Reset();
    m_character1.reset();
    m_character1Texture.Reset();
    m_character2.reset();
    m_character2Texture.Reset();
    m_characterBackgroundTexture.Reset();

    // Environment Select
    m_environSelectCalmTexture.Reset();
    m_environSelectBreezyTexture.Reset();
    m_environSelectAlienTexture.Reset();

    // Intro Screen
    m_bmwLogoTexture.Reset();
    m_jiLogoTexture.Reset();

    m_backgroundTex.Reset();

    m_testShape.reset();
    m_testShape2.reset();
    m_testShape3.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

void Game::SetUiDisplay(std::string aString)
{
    m_uiAmmoDisplayString = aString;
    m_uiDisplayTimer = 0.0f;
    m_isUiDisplayTrue = true;
}

void Game::SetUiAmmoDisplay(AmmoType aAmmoType)
{
    if (aAmmoType == AmmoType::AMMOTYPE_CANNON)
    {
        m_uiAmmoDisplayString = "Kinetic Cannon Ammo Loaded";
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        m_uiAmmoDisplayString = "Explosive Cannon Ammo Loaded";
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        m_uiAmmoDisplayString = "Machine Gum Ammo Loaded";
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        m_uiAmmoDisplayString = "Explosive MIRV Ammo Loaded";
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        m_uiAmmoDisplayString = "Shotgun Ammo Loaded";
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_GUIDEDMISSILE)
    {
        m_uiAmmoDisplayString = "Guided Missle Loaded";
    }
    else
    {
        m_uiAmmoDisplayString = "Error : Unknown Ammo Type";
    }

    m_uiDisplayTimer = 0.0f;
    m_isUiDisplayTrue = true;
}

void Game::UpdateInput(DX::StepTimer const& aTimer)
{
    // WLJ add for mouse and keybord interface   
    auto kb = m_keyboard->GetState();
    m_kbStateTracker.Update(kb);

    if (kb.Escape)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_PRESWINGVIEW);
        }
        m_currentGameState = GameState::GAMESTATE_MAINMENU;
    }
    if (m_kbStateTracker.pressed.Enter)
    {
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            if (m_menuSelect == 0)
            {

            }
            if (m_menuSelect == 1)
            {

            }
            if (m_menuSelect == 2)
            {

            }
            m_menuSelect = 0;
            m_currentGameState = GameState::GAMESTATE_STARTSCREEN;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            if (m_menuSelect == 0)
            {

            }
            if (m_menuSelect == 1)
            {

            }
            if (m_menuSelect == 2)
            {

            }
            m_menuSelect = 0;
            //m_currentGameState = GameState::GAMESTATE_MAINMENU; // Return to Main Menu after selecting character, ToDo: using value of 1 doesn't return to main menu
            m_currentGameState = GameState::GAMESTATE_STARTSCREEN;// Return to Main Menu after selecting character, ToDo: using value of 1 doesn't return to main menu
        }
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            if (m_menuSelect == 0) // GoTo Game State
            {
                m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
                m_menuSelect = 0;
            }
            if (m_menuSelect == 1) // GoTo Character Select State
            {
                m_currentGameState = GameState::GAMESTATE_CHARACTERSELECT;
                m_menuSelect = 0;
            }
            if (m_menuSelect == 2) // GoTo Environment Select State
            {
                m_currentGameState = GameState::GAMESTATE_ENVIRONTMENTSELECT;
                m_menuSelect = 0;
            }
            /*
            if (m_menuSelect == 3) // GoTo Demo Select State
            {
                m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
            }
            */
            if (m_menuSelect == 3) // Quit Game
            {
                ExitGame();
            }
            //m_menuSelect = 0;
        }
        if (m_currentGameState == GameState::GAMESTATE_STARTSCREEN)
        {

        }
    }
    if (kb.Up)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ManualControlInputPitch(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Down)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ManualControlInputPitch(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Left)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ManualControlInputYaw(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Right)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ManualControlInputYaw(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (m_kbStateTracker.pressed.OemQuestion)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ManualInputReset(FinType::CANARD_PITCH, true);
        }
    }
    if (m_kbStateTracker.pressed.OemQuotes)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->CycleControlInputType();
        }
    }

    if (kb.D)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputYawPedal(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f + static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f, 0.0f);
        }
    }
    if (kb.S)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputCollective(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.A)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputYawPedal(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f - static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f, 0.0f);
        }
    }
    if (kb.W)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputCollective(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.E)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputThrottle(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePitchYaw(0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.F)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f);
        }
    }
    if (m_kbStateTracker.pressed.P)
    {
        m_npcController->DebugToggleAI();
    }
    if (m_kbStateTracker.pressed.Y)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_SNAPCAM)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FIRSTPERSON);
        }
        else if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_SNAPCAM);
        }
        else
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FIRSTPERSON);
        }
    }
    if (m_kbStateTracker.pressed.F1)
    {
        m_camera->SetCameraState(CameraState::CAMERASTATE_SWINGVIEW);
    }
    if (kb.NumPad1)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputTurretYaw(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad2)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputWeaponPitch(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad3)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputTurretYaw(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad4)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputCyclicRollNew(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad5)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputCyclicPitchNew(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad6)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputCyclicRollNew(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad7)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputYawPedal(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad8)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputCyclicPitchNew(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad9)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputYawPedal(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad0)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputWeaponPitch(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }

    if (m_kbStateTracker.pressed.R)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->DetonateAllMissiles();
        }
    }
    if (m_kbStateTracker.pressed.U)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->ActivateMissleTrackCamera();
        }
    }
    if (m_kbStateTracker.pressed.I)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->CamMissileSelectNext();
        }
    }
    if (m_kbStateTracker.pressed.O)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->CycleMissileTrackState();
        }
    }

    if (kb.X)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugInputVelocityZero();
        }
    }
    if (kb.C)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f);
        }
    }
    if (m_kbStateTracker.pressed.N)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->TriggerMirvDeploy();
        }
    }
    if (m_kbStateTracker.pressed.L)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->CycleFireControlAmmo();
            SetUiAmmoDisplay(m_fireControl->GetCurrentAmmoType());
        }
    }
    //if (kb.Space)
    if (m_kbStateTracker.pressed.Space)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            if (m_fireControl->GetIsMissileFireAvailable() == true)
            {
                if (m_fireControl->GetIsMissileDualFireTrue() == true)
                {
                    TriggerFireWithAudio();             
                    TriggerFireWithAudio();
                }
                else
                {
                    TriggerFireWithAudio();
                }  
            }
        }
    }
    if (m_kbStateTracker.pressed.J)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->Jump();
        }
    }
    if (m_kbStateTracker.pressed.B)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_npcController->ToggleDebugBool();
        }
    }
    if (m_kbStateTracker.pressed.V)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_npcController->ResetNpcDebugPauseToggle();
        }
    }
    if (m_kbStateTracker.pressed.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle();
        }
    }
    if (m_kbStateTracker.pressed.Z)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle2();
        }
    }
    if (m_kbStateTracker.pressed.P)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //TogglePause();
        }
    }
    if (m_kbStateTracker.pressed.D1)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ToggleDebug1();
        }
    }
    if (m_kbStateTracker.pressed.D2)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ToggleDebug2();
        }
    }
    if (m_kbStateTracker.pressed.D3)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ToggleDebug3();
        }
    }
    if (m_kbStateTracker.pressed.D4)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle4();
        }
    }
    if (m_kbStateTracker.pressed.D5)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle5();
        }
    }
    if (m_kbStateTracker.pressed.D6)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle6();
        }
    }
    if (m_kbStateTracker.pressed.D7)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle7();
        }
    }
    if (m_kbStateTracker.pressed.D8)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle8();
        }
    }
    if (m_kbStateTracker.pressed.D9)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle9();
        }
    }
    if (m_kbStateTracker.pressed.D0)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->ZeroMissileVelocities();
        }
    }
    if (m_kbStateTracker.pressed.K)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->ToggleFireControlLaser();
        }
    }
    if (kb.OemOpenBrackets)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_fireControl->DebugIntputValUpdate(static_cast<float>(aTimer.GetElapsedSeconds()));
            //m_fireControl->DebugIntputValUpdateStatic(true);
            m_camera->FovUp(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.OemCloseBrackets)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_fireControl->DebugIntputValUpdate(static_cast<float>(-aTimer.GetElapsedSeconds()));
            //m_fireControl->DebugIntputValUpdateStatic(false);
            m_camera->FovDown(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.OemQuotes)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_fireControl->DebugInputZero();
        }
    }

    auto mouse = m_mouse->GetState();

    if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
    {
        if (mouse.scrollWheelValue > 0)
        {
            m_debugData->ToggleDebugOnOverRide();
            m_camera->FreeLookSpeedUp();
            m_debugData->DebugPushUILineDecimalNumber("GetFreeLookSpeed = ", m_camera->GetFreeLookSpeed(), "");
            m_mouse->ResetScrollWheelValue();
            m_debugData->ToggleDebugOff();

        }
        else if (mouse.scrollWheelValue < 0)
        {
            m_debugData->ToggleDebugOnOverRide();
            m_camera->FreeLookSpeedDown();
            m_debugData->DebugPushUILineDecimalNumber("GetFreeLookSpeed = ", m_camera->GetFreeLookSpeed(), "");
            m_mouse->ResetScrollWheelValue();
            m_debugData->ToggleDebugOff();
        }

        if (mouse.positionMode == Mouse::MODE_RELATIVE)
        {
            const float ROTATION_GAIN = 0.004f;
            DirectX::SimpleMath::Vector3 delta = DirectX::SimpleMath::Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

            float pitch = -delta.y;
            float yaw = -delta.x;

            m_camera->UpdatePitchYaw(pitch, yaw);
        }

        m_mouse->SetMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
    }

    auto pad = m_gamePad->GetState(0);

    if (pad.IsConnected())
    {
        m_buttons.Update(pad);

        if (pad.thumbSticks.leftY > m_gamePadInputDeadZone || pad.thumbSticks.leftY < -m_gamePadInputDeadZone)
        {
            const float inputMod = m_gamePadInputRateBodyAccel;
            m_vehicle->InputGamePadForward(pad.thumbSticks.leftY * inputMod);
        }
        if (pad.thumbSticks.leftX > m_gamePadInputDeadZone || pad.thumbSticks.leftX < -m_gamePadInputDeadZone)
        {
            //m_vehicle->InputCyclicRoll(static_cast<float>(aTimer.GetElapsedSeconds()));
            const float inputMod = m_gamePadInputRateBodySideStrafe;
            //m_vehicle->InputGamePadStrafe(-pad.thumbSticks.leftX * inputMod);
            //m_vehicle->InputGamePadStrafe(-pad.thumbSticks.leftX * static_cast<float>(aTimer.GetElapsedSeconds()));
            m_vehicle->InputGamePadStrafe(-pad.thumbSticks.leftX);
            //m_vehicle->InputGamePadTurn(pad.thumbSticks.leftX);
        }
        if (pad.triggers.left > m_gamePadInputDeadZone || pad.triggers.right > m_gamePadInputDeadZone)
        {
            const float turnMod = m_gamePadInputRateBodyTurn;
            m_vehicle->InputGamePadTurn((-pad.triggers.left + pad.triggers.right) * turnMod);
            //m_vehicle->InputGamePadStrafe((pad.triggers.left - pad.triggers.right)* turnMod);
        }
        if (pad.thumbSticks.rightX > m_gamePadInputDeadZone || pad.thumbSticks.rightX < -m_gamePadInputDeadZone)
        {
            m_vehicle->InputTurretYaw(-pad.thumbSticks.rightX * m_gamePadInputRateTurretHorizontal);
        }
        if (pad.thumbSticks.rightY > m_gamePadInputDeadZone || pad.thumbSticks.rightY < -m_gamePadInputDeadZone)
        {
            const float pitchMod = m_gamePadInputRateTurretVerticle;
            m_vehicle->InputWeaponPitch(-pad.thumbSticks.rightY * pitchMod);
        }

        if (pad.IsRightShoulderPressed() == true)
        {
            m_vehicle->FireWeapon();
        }
        if (pad.IsLeftStickPressed() == true)
        {
            m_vehicle->DebugInputVelocityZero();
        }
        if (m_buttons.y == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                m_vehicle->CycleFireControlAmmo();
                SetUiAmmoDisplay(m_fireControl->GetCurrentAmmoType());
            }
        }
        if (m_buttons.leftShoulder == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                m_fireControl->TriggerMirvDeploy();
            }
        }
        if (m_buttons.b == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                const unsigned int columnCount = 8;
                const unsigned int rowCount = 2;
                //const float columSpaceing = 15.0f;
                //const float rowSpacing = 20.0f;
                const float columSpaceing = 20.0f;
                const float rowSpacing = 25.0f;

                const DirectX::SimpleMath::Vector2 spacing = DirectX::SimpleMath::Vector2(columSpaceing, rowSpacing);
                DirectX::SimpleMath::Vector3 dropDirection = m_vehicle->GetForward();
                const float dropDistance = 250.0f;
                dropDirection *= dropDistance;
                DirectX::SimpleMath::Vector3 dropPosition = m_vehicle->GetPos();
                dropPosition.y += 145.0f;
                dropPosition += dropDirection;

                const DirectX::SimpleMath::Vector3 orientation = -m_vehicle->GetForward();
                const DirectX::SimpleMath::Quaternion alignQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicle->GetAlignment());

                m_npcController->LoadToQueue(dropPosition, orientation, columnCount, rowCount, spacing, alignQuat);
            }
        }
        if (m_buttons.x == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                m_npcController->SetAllNpcsToDead();
            }
        }
        if (m_buttons.dpadDown == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                ToggleMusicFadeOut();
            }
        }
        if (m_buttons.dpadUp == GamePad::ButtonStateTracker::PRESSED)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_AUDIOBANK_ROCKETBOOSTENGINELOOP);
                //AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_SOUNDS_KNIGHTRIDERMUSIC);
                //AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_AUDIOBANK_BRAVESPACEEXPLORERS);
                //AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK::XACT_WAVEBANK_AUDIOBANK_COINSFX);
            }
        }
        if (m_buttons.rightStick == GamePad::ButtonStateTracker::HELD)
        {
            if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
            {
                m_vehicle->InputWeaponPitchToZero(static_cast<float>(aTimer.GetElapsedSeconds()));
            }
        }
        
    }
    else
    {
        m_buttons.Reset();
    }
}

void Game::UpdateAudioFx(DX::StepTimer const& aTimer)
{
    for (unsigned int i = 0; i < m_fireControl->GetCreateAudioCount(); ++i)
    {
        std::shared_ptr <Utility::SoundFx> createdFx(new Utility::SoundFx());
        createdFx = m_fireControl->GetFxToCreate(i);

        if (createdFx->fxType == Utility::SoundFxType::SOUNDFXTYPE_EXPLOSION)
        {
            m_currentFxExplosion = GetRandomNonRepeatingFxIndex(m_currentFxExplosion, createdFx->fxType);
            createdFx->fx = m_audioBank->CreateStreamInstance(m_currentFxExplosion, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

            createdFx->emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
            createdFx->emitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
            createdFx->emitter->CurveDistanceScaler = m_audioDistanceExplosion;
            createdFx->emitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
            createdFx->isTriggeredTrue = true;
            createdFx->fx->Play(false);
        }
        else if (createdFx->fxType == Utility::SoundFxType::SOUNDFXTYPE_BEACON)
        {
            createdFx->fx = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_BEACON_4, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

            createdFx->emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
            createdFx->emitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
            createdFx->emitter->CurveDistanceScaler = m_audioDistanceBeacon;
            createdFx->emitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
            createdFx->isTriggeredTrue = true;
            createdFx->fx->Play(false);
        }
        else if (createdFx->fxType == Utility::SoundFxType::SOUNDFXTYPE_BEACON_ALT)
        {
            createdFx->fx = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_BEACON_5, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

            createdFx->emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
            createdFx->emitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
            createdFx->emitter->CurveDistanceScaler = m_audioDistanceBeacon;
            createdFx->emitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
            createdFx->isTriggeredTrue = true;
            createdFx->fx->Play(false);
        }
        else if (createdFx->fxType == Utility::SoundFxType::SOUNDFXTYPE_POOF)
        {
            createdFx->fx = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_POOF_2, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

            createdFx->emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
            createdFx->emitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
            createdFx->emitter->CurveDistanceScaler = m_audioDistancePoof;
            createdFx->emitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
            createdFx->isTriggeredTrue = true;
            createdFx->fx->Play(false);
        }
        else if (createdFx->fxType == Utility::SoundFxType::SOUNDFXTYPE_LASER_LOCK_TONE)
        {
            createdFx->fx = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_UFO_11, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
            //createdFx->fx = m_audioBank->CreateStreamInstance(XACT_WAVEBANK_AUDIOBANK_BEACON_4, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
            createdFx->emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
            createdFx->emitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
            createdFx->emitter->CurveDistanceScaler = m_audioDistancePoof;
            createdFx->emitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
            createdFx->isTriggeredTrue = true;
            //createdFx->fx->IsLooped();
            createdFx->fx->Play(true);
        }
        m_soundFxVecTest.push_back(createdFx);
    }
    m_fireControl->ClearCreateAudioVec();

    for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
    {
        m_soundFxVecTest[i]->emitter->Update(m_soundFxVecTest[i]->pos, m_soundFxVecTest[i]->up, aTimer.GetElapsedSeconds());
        m_soundFxVecTest[i]->fx->Apply3D(m_listener, *m_soundFxVecTest[i]->emitter);
        m_fxEmitter.Update(m_soundFxVecTest[i]->pos, m_soundFxVecTest[i]->up, aTimer.GetElapsedSeconds());
    }

    // laser tone
    for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
    {
        if (m_soundFxVecTest[i]->fxType == Utility::SoundFxType::SOUNDFXTYPE_LASER_LOCK_TONE)
        {
            if (m_fireControl->GetIsTargetingLaserHitTrue() == false)
            {
                const float soundDropRate = 0.1f;
                m_soundFxVecTest[i]->volume -= soundDropRate;
                
                if (m_soundFxVecTest[i]->volume < 0.0f)
                {
                    m_soundFxVecTest[i]->fx->SetVolume(0.0f);

                    m_soundFxVecTest[i]->fx->Stop();
                    m_soundFxVecTest[i]->isDestroyTrue = true;
                }
                else
                {
                    m_soundFxVecTest[i]->fx->SetVolume(m_soundFxVecTest[i]->volume);
                }
            }
        }
    }

    for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
    {
        if (m_soundFxVecTest[i]->fx->GetState() == SoundState::STOPPED)
        {
            m_soundFxVecTest[i]->isDestroyTrue = true;
        }
    }

    for (unsigned int i = 0; i < m_soundFxVecTest.size(); ++i)
    {
        if (m_soundFxVecTest[i]->isDestroyTrue == true)
        {
            std::vector<std::shared_ptr<Utility::SoundFx>>::iterator it;
            it = m_soundFxVecTest.begin() + i;
            AudioFxDelete(m_soundFxVecTest[i]);
            m_soundFxVecTest.erase(it);
        }
    }
}

void Game::TriggerFireWithAudio()
{
    // rocket fx
    std::shared_ptr <Utility::SoundFx> rocketFx(new Utility::SoundFx());
    rocketFx->fx = m_audioBank->CreateStreamInstance(3, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);;
    std::shared_ptr<DirectX::AudioEmitter> rocketEmitter = std::make_shared<DirectX::AudioEmitter>();
    rocketEmitter->SetOmnidirectional();
    rocketEmitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
    rocketEmitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
    rocketEmitter->CurveDistanceScaler = 14.f;
    rocketEmitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);

    rocketFx->SetEmitter(rocketEmitter);

    // trigger fire fx
    std::shared_ptr <Utility::SoundFx> fireFx(new Utility::SoundFx());
    fireFx->fxType = Utility::SoundFxType::SOUNDFXTYPE_SHOTBANG;
    m_currentFxShotBang = GetRandomNonRepeatingFxIndex(m_currentFxShotBang, Utility::SoundFxType::SOUNDFXTYPE_SHOTBANG);
    fireFx->fx = m_audioBank->CreateStreamInstance(m_currentFxShotBang, SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
    std::shared_ptr<DirectX::AudioEmitter> fireEmitter = std::make_shared<DirectX::AudioEmitter>();
    fireEmitter->SetOmnidirectional();
    fireEmitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
    fireEmitter->pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
    fireEmitter->CurveDistanceScaler = 14.f;
    fireEmitter->pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
    fireFx->SetEmitter(fireEmitter);

    m_vehicle->FireWeapon(fireFx, rocketFx);

    m_soundFxVecTest.push_back(fireFx);
    m_soundFxVecTest.push_back(rocketFx);
}

unsigned int Game::GetRandomNonRepeatingFxIndex(unsigned int aCurrentIndex, Utility::SoundFxType aFxType)
{
    unsigned int min;
    unsigned int max;
    if (aFxType == Utility::SoundFxType::SOUNDFXTYPE_EXPLOSION)
    {
        min = m_fxExplosionRangeMin;
        max = m_fxExplosionRangeMax;
    }
    else if (aFxType == Utility::SoundFxType::SOUNDFXTYPE_SHOTBANG)
    {
        min = m_fxShotBangRangeMin;
        max = m_fxShotBangRangeMax;
    }
    else if (aFxType == Utility::SoundFxType::SOUNDFXTYPE_DEBUG)
    {
        return 1;
    }

    unsigned int randomInt = min + static_cast <unsigned int> (rand()) / (static_cast <unsigned int> (RAND_MAX / (max - min)));

    if (randomInt == aCurrentIndex)
    {
        ++randomInt;
    }
    if (randomInt > max)
    {
        randomInt = min;
    }
    return randomInt; 
}
#pragma endregion
