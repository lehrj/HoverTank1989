//
// Game.cpp
//
// 

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

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
    m_vehicle = new Vehicle();
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
    m_vehicle->SetModelController(m_modelController);

    m_testVehicleHover = std::make_shared<VehicleHover>();

    m_currentGameState = GameState::GAMESTATE_GAMEPLAY;
    m_lighting->SetLighting(Lighting::LightingState::LIGHTINGSTATE_TEST01);
    m_currentUiState = UiState::UISTATE_SWING;
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
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef DEBUG  //#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);
    m_retryAudio = false;
    m_audioBank = std::make_unique<WaveBank>(m_audioEngine.get(), L"Art/Audio/audioBank.xwb");

    // height map intit
    bool result;
    bool isInitSuccessTrue = true;

    result = InitializeTerrainArray();
    if (!result)
    {
        isInitSuccessTrue = false;
    }

    m_terrainStartScreen.environType = EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN;
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
    m_vehicle->InitializeVehicle(context, m_npcController);
    m_vehicle->SetDebugData(m_debugData);
    m_npcController->LoadNPCs(context, m_npcController);
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

    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
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
    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
    {
        gridLineOffSetY = 0.003f;
    }
    else if (aTerrain.environType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        gridLineOffSetY = 0.3f;
    }
    gridLineOffSetY = 0.22f;
    const float gridLineOffSetY2 = 1.1f;
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
    if (aTerrain.environType == EnvironmentType::ENIVRONMENTTYPE_STARTSCREEN)
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

    delete m_camera;
    delete m_environment;
    delete m_lighting;
    delete m_vehicle;

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
    
    UpdateInput(aTimer);
    
    if (m_isPauseOn == false)
    {
        m_debugData->DebugClearUI();
        m_testTimer1 += static_cast<float>(aTimer.GetElapsedSeconds());
        m_vehicle->UpdateVehicle(aTimer.GetElapsedSeconds());
        m_modelController->UpdatePlayerModel(m_vehicle->GetAlignment(), m_vehicle->GetPos(), m_vehicle->GetWeaponPitch(), m_vehicle->GetTurretYaw());
        m_npcController->UpdateNPCController(m_vehicle->GetPos(), m_vehicle->GetVelocity(), m_vehicle->GetAlignment(),aTimer.GetElapsedSeconds());
    }
    m_camera->UpdateCamera(aTimer);
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
}
#pragma endregion

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
    if (m_kbStateTracker.pressed.Up)
    {
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            --m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Down)
    {
        if (m_currentGameState == GameState::GAMESTATE_MAINMENU)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            ++m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Left)
    {
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            --m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            --m_menuSelect;
        }
    }
    if (m_kbStateTracker.pressed.Right)
    {
        if (m_currentGameState == GameState::GAMESTATE_CHARACTERSELECT)
        {
            ++m_menuSelect;
        }
        if (m_currentGameState == GameState::GAMESTATE_ENVIRONTMENTSELECT)
        {
            ++m_menuSelect;
        }
    }
    if (kb.Up)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Down)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Left)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.Right)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

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
    if (kb.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY && m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE || m_currentGameState == GameState::GAMESTATE_GAMEPLAYSTART)
        {
            m_vehicle->InputThrottle(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePitchYaw(0.0f, 0.0f + static_cast<float>(aTimer.GetElapsedSeconds()));
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
    if (kb.C)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
        {
            m_camera->UpdatePos(0.0f, 0.0f - static_cast<float>(aTimer.GetElapsedSeconds()), 0.0f);
        }
    }
    if (m_kbStateTracker.pressed.D1)
    {
        if (m_moveLightWithCameraFocus1 == true)
        {
            m_moveLightWithCameraFocus1 = false;
        }
        else
        {
            m_moveLightWithCameraFocus1 = true;
        }
    }
    if (m_kbStateTracker.pressed.D2)
    {
        if (m_moveLightWithCameraFocus2 == true)
        {
            m_moveLightWithCameraFocus2 = false;
        }
        else
        {
            m_moveLightWithCameraFocus2 = true;
        }
    }
    if (m_kbStateTracker.pressed.D3)
    {
        if (m_moveLightWithCameraFocus3 == true)
        {
            m_moveLightWithCameraFocus3 = false;
        }
        else
        {
            m_moveLightWithCameraFocus3 = true;
        }
    }
    if (m_kbStateTracker.pressed.T)
    {
        m_testTimer1 = 0.0f;
        m_npcController->DebugToggleAI();
    }
    if (m_kbStateTracker.pressed.Y)
    {
        if (m_camera->GetCameraState() == CameraState::CAMERASTATE_SPRINGCAMERA)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FOLLOWVEHICLE);
        }
        else if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FOLLOWVEHICLE)
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_FIRSTPERSON);
        }
        else
        {
            m_camera->SetCameraState(CameraState::CAMERASTATE_SPRINGCAMERA);
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
            m_lightPos0.y -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCollective(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad2)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.x -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicPitch(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad3)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos0.y += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCollective(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad4)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.z -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicRoll(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad5)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (kb.NumPad6)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.z += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicRoll(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad7)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.y -= static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputYawPedal(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad8)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.x += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputCyclicPitch(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad9)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_lightPos1.y += static_cast<float>(aTimer.GetElapsedSeconds()) * m_lightMovementSpeed;
            m_vehicle->InputYawPedal(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.NumPad0)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputThrottle(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Decimal)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputThrottle(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (m_kbStateTracker.pressed.R)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->ResetVehicle();
        }
    }
    if (m_kbStateTracker.pressed.U)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            DirectX::SimpleMath::Vector3 endPos(11.0, 2.0f, 0.0f);
            DirectX::SimpleMath::Vector3 targetEndPos = m_vehicle->GetPos();
            DirectX::SimpleMath::Vector3 centerPointPos = m_vehicle->GetPos();
            float rotation = Utility::ToRadians(90.0);
            m_camera->SetCameraStartPos(m_camera->GetPos());
            m_camera->SetCameraEndPos(endPos);
            m_camera->SetTargetStartPos(m_camera->GetTargetPos());
            m_camera->SetTargetEndPos(targetEndPos);
            m_camera->TurnEndPosAroundPoint(rotation, centerPointPos);
            m_camera->SetCameraState(CameraState::CAMERASTATE_TRANSITION);
        }
    }
    if (kb.I)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->SpinClockwise(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.O)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputWeaponPitch(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.L)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

            m_vehicle->InputWeaponPitch(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.OemPeriod)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputTurretYaw(static_cast<float>(-aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.OemComma)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputTurretYaw(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.Z)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->InputJet(static_cast<float>(aTimer.GetElapsedSeconds()));
        }
    }
    if (kb.X)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugInputVelocityZero();
        }
    }
    if (m_kbStateTracker.pressed.N)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->PrepareTrailerStart();
        }
    }
    if (m_kbStateTracker.pressed.M)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->StartTrailerCamera();
        }
    }
    if (m_kbStateTracker.pressed.P)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            TogglePause();
        }
    }
    if (m_kbStateTracker.pressed.L)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {

        }
    }
    if (m_kbStateTracker.pressed.Space)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->Jump();
        }
    }
    if (m_kbStateTracker.released.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_vehicle->DebugToggle();
        }
    }
    if (m_kbStateTracker.pressed.H)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_testTimer1 = 0.0f;
            m_npcController->TestPositionChange();
        }
    }
    if (m_kbStateTracker.pressed.G)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_isDisplayCountdownTrue = true;
        }
    }
    if (m_kbStateTracker.pressed.K)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_camera->StartTrailerCamera4();
            m_vehicle->TestFire();
        }
    }
    if (m_kbStateTracker.pressed.J)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_camera->StartTrailerCamera3();
            m_vehicle->TestFire2();
        }
    }
    if (m_kbStateTracker.pressed.OemOpenBrackets)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->CycleNpcFocus(false);
        }
    }
    if (m_kbStateTracker.pressed.OemCloseBrackets)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_camera->CycleNpcFocus(true);
        }
    }
    if (m_kbStateTracker.pressed.B)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            //m_camera->SetCameraState(CameraState::CAMERASTATE_FOLLOWNPC);
            //m_isSlowMoOn = true;
            m_camera->TransitionToNpcSpringCamera();
            //m_camera->SetCameraState(CameraState::CAMERASTATE_SPRINGCAMERANPC);
        }
    }
    if (m_kbStateTracker.pressed.V)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_isSlowMoOn = false;
            //m_camera->ReturnToOverwatchPosition();
        }
    }
    if (m_kbStateTracker.pressed.Q)
    {
        if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
        {
            m_endScreenTimer = 0.0f;
            m_isDisplayEndScreenTrue = true;
        }
    }
    auto mouse = m_mouse->GetState();

    if (m_camera->GetCameraState() == CameraState::CAMERASTATE_FIRSTPERSON)
    {
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
}

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

    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        m_modelController->DrawModel(context, *m_states, DirectX::SimpleMath::Matrix::Identity, m_camera->GetViewMatrix(), m_proj);
    }
    
    m_batch->Begin();
    if (m_currentGameState == GameState::GAMESTATE_GAMEPLAY)
    {
        m_vehicle->DrawVehicleProjectiles(m_camera->GetViewMatrix(), m_proj);
        m_npcController->DrawNPCs(m_camera->GetViewMatrix(), m_proj);
        DrawSky();
        //DrawTestTrack();
    }
    m_batch->End();

    // m_batch2 start
    /*
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullNone());
    */
    //context->RSSetState(m_raster.Get());

    m_effect2->SetWorld(m_world);
    m_effect2->Apply(context);
    context->IASetInputLayout(m_inputLayout.Get());
    m_batch2->Begin();
    DrawTerrainNew(m_terrainGamePlay);
    m_batch2->End();

    m_effect3->SetWorld(m_world);
    m_effect3->Apply(context);
    m_batch3->Begin();
    DrawDebugLinesVector();
    m_batch3->End();

    m_spriteBatch->Begin();
    if (m_isDisplayCountdownTrue == true)
    {
        DrawUnlockUI();
    }
    DrawDebugDataUI();
    if (m_isDisplayEndScreenTrue == true)
    {
        DrawEndUI();
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
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
    m_audioEngine->Suspend();
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
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
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/Textures/skyTexture.jpg", nullptr, m_textureSky.ReleaseAndGetAddressOf()));
    // Jackson Industry textures
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

    m_effect2 = std::make_unique<BasicEffect>(device);
    m_effect2->SetVertexColorEnabled(true);
    //m_effect2->EnableDefaultLighting();
    //m_effect2->SetLightDiffuseColor(0, Colors::Gray);

    m_effect3 = std::make_unique<BasicEffect>(device);
    m_effect3->SetVertexColorEnabled(true);

    //void const* shaderByteCode2;
    //size_t byteCodeLength2;
    //m_effect2->GetVertexShaderBytecode(&shaderByteCode2, &byteCodeLength2);
    //DX::ThrowIfFailed(device->CreateInputLayout(VertexType2::InputElements, VertexType2::InputElementCount, shaderByteCode2, byteCodeLength2, m_inputLayout.ReleaseAndGetAddressOf()));
    //m_batch2 = std::make_unique<PrimitiveBatch<VertexType2>>(device);
    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType2>(device, m_effect2.get(), m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch2 = std::make_unique<PrimitiveBatch<VertexType2>>(context);

    //m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch = std::make_unique<PrimitiveBatch<VertexType>>(context);

    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexType3>(device, m_effect3.get(), m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch3 = std::make_unique<PrimitiveBatch<VertexType3>>(context);

    m_skyShape = GeometricPrimitive::CreateSphere(context, 200000.0f, 32, false);
    m_skyShape->CreateInputLayout(m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf());

    m_font = std::make_unique<SpriteFont>(device, L"Art/Fonts/myfile.spritefont");
    m_titleFont = std::make_unique<SpriteFont>(device, L"Art/Fonts/titleFont.spritefont");
    m_bitwiseFont = std::make_unique<SpriteFont>(device, L"Art/Fonts/bitwise24.spritefont");
    m_spriteBatch = std::make_unique<SpriteBatch>(context);
   
    m_effect4 = std::make_unique<BasicEffect>(device);
    m_effect4->SetVertexColorEnabled(true);

    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexPositionNormalColor>(device, m_effect4.get(), m_inputLayout.ReleaseAndGetAddressOf()));
    m_batch4 = std::make_unique<PrimitiveBatch<VertexPositionNormalColor>>(context);
    
    //DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"../HoverTank1989/Art/SpecularMaps/blankSpecular.jpg", nullptr, m_specular.ReleaseAndGetAddressOf()));
    //m_model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
    //m_modelTank01 = Model::CreateFromCMO(device, L"HoverTankTest.cmo", *m_fxFactory);
    m_modelTank01 = Model::CreateFromCMO(device, L"HoverTankTest.cmo", *m_fxFactory);
    m_modelTankBody01 = Model::CreateFromCMO(device, L"HoverTankBody02.cmo", *m_fxFactory);
    m_modelTankTurret01 = Model::CreateFromCMO(device, L"HoverTankTurret02.cmo", *m_fxFactory);
    m_modelTankBarrel01 = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);

    m_modelTest = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);

    m_modelTestBarrel = Model::CreateFromCMO(device, L"HoverTankBarrel02.cmo", *m_fxFactory);
    m_modelTestBody = Model::CreateFromCMO(device, L"HoverTankBody02.cmo", *m_fxFactory);
    m_modelTestTurret = Model::CreateFromCMO(device, L"HoverTankTurret02.cmo", *m_fxFactory);

    m_modelController->InitializePlayerModel(m_modelTestBarrel, m_modelTestBody, m_modelTestTurret);

    m_testShape = DirectX::GeometricPrimitive::CreateCylinder(context, 1.0f, 80.0f);
    m_testShape2 = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(250.0f, 1.0f, 6.0f));
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

    m_effect4->SetView(m_view);
    m_effect4->SetProjection(m_proj);
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

    // Draw Timer
    //std::string textLine = "Timer  " + std::to_string(m_timer.GetTotalSeconds());
    std::string textLine = "Timer  " + std::to_string(m_testTimer1);
    DirectX::SimpleMath::Vector2 textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    textLine = "FPS   " + std::to_string(m_timer.GetFramesPerSecond());
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;

    /*
    textLine = "m_unlockTimer1   " + std::to_string(m_unlockTimer1);
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    textLine = "m_unlockTimer2   " + std::to_string(m_unlockTimer2);
    textLineOrigin = m_bitwiseFont->MeasureString(textLine.c_str()) / 2.f;
    textLinePos.x = textLineOrigin.x + 20;
    m_bitwiseFont->DrawString(m_spriteBatch.get(), textLine.c_str(), textLinePos, Colors::White, 0.f, textLineOrigin);
    textLinePos.y += 30;
    */
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
}

void Game::DrawSky()
{
    m_skyRotation += static_cast<float>(m_timer.GetElapsedSeconds()) * 0.19f;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-m_skyRotation));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(30.0f));
    rotMat *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(30.0f));
    m_skyShape->Draw(rotMat, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix(), DirectX::SimpleMath::Vector4(1.0, 1.0, 1.0, 2.0f), m_textureSky.Get());
}

void Game::DrawTestTrack()
{
    const float yOffset = 0.5f;
    const float rodOffset = 0.05f;
    // inside top left
    DirectX::SimpleMath::Vector3 pos1(300.0f, yOffset, -100.0f);
    DirectX::SimpleMath::Matrix posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Vector3 pos2 = pos1;
    pos2.z -= 100.0f;
    pos2.y -= rodOffset;
    DirectX::SimpleMath::Matrix posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside top left
    pos1 = DirectX::SimpleMath::Vector3(300.0f, yOffset, -300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);

    // inside top right
    pos1 = DirectX::SimpleMath::Vector3(300.0f, yOffset, 100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside top right
    pos1 = DirectX::SimpleMath::Vector3(300.0f, yOffset, 300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);

    // inside bottom right
    pos1 = DirectX::SimpleMath::Vector3(75.0f, yOffset, 100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside bottom right
    pos1 = DirectX::SimpleMath::Vector3(75.0f, yOffset, 300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.x += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // inside bottom left
    pos1 = DirectX::SimpleMath::Vector3(75.0f, yOffset, -100.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.z -= 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // outside bottom left
    pos1 = DirectX::SimpleMath::Vector3(75.0f, yOffset, -300.0f);
    posMat = DirectX::SimpleMath::Matrix::CreateWorld(pos1, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    pos2 = pos1;
    pos2.x += 100.0f;
    pos2.y -= rodOffset;
    posMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    posMat2 *= DirectX::SimpleMath::Matrix::CreateWorld(pos2, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    m_testShape->Draw(posMat, m_camera->GetViewMatrix(), m_proj);
    m_testShape2->Draw(posMat2, m_camera->GetViewMatrix(), m_proj);

    // cross 1
    pos2 = DirectX::SimpleMath::Vector3(187.0f, yOffset - rodOffset, 0.0f);
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
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, aTerrain.terrainVertexArrayBase, aTerrain.terrainVertexCount);
    m_batch2->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, aTerrain.terrainVertexArray, aTerrain.terrainVertexCount);
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

    m_effect4.reset();
    m_batch4.reset();

    m_testShape.reset();
    m_testShape2.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
