#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <string>

// Display resolution
constexpr int SCREEN_WIDTH = 320;
constexpr int SCREEN_HEIGHT = 240;
constexpr int SCALE = 3;

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;

// Texture & World constants
constexpr int TEX_WIDTH = 64;
constexpr int TEX_HEIGHT = 64;
constexpr int MAP_COLS = 31;
constexpr int MAP_ROWS = 31;

// Render modes
enum RenderMode {
    RENDER_TEXTURED = 0,
    RENDER_SOLID_SHADED,
    RENDER_DEPTH_HEATMAP,
    RENDER_MODE_COUNT
};

enum GameState {
    STATE_GAMEPLAY,
    STATE_MENU
};

// 8x8 Minimal Bitmap Font for software UI (ASCII 32-127 basic set)
const uint8_t FONT_8X8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // Space
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // !
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, // "
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // #
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, // $
    {0x00,0x66,0xAC,0xD8,0x36,0x6A,0x00,0x00}, // %
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // &
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // '
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // (
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // )
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // *
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // +
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // ,
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // -
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // .
    {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00}, // /
    {0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00}, // 0
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 1
    {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, // 2
    {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00}, // 3
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, // 4
    {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00}, // 5
    {0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, // 6
    {0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00}, // 7
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, // 8
    {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00}, // 9
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // :
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // ;
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // <
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // =
    {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, // >
    {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00}, // ?
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, // @
    {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // A
    {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, // B
    {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00}, // C
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, // D
    {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00}, // E
    {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, // F
    {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00}, // G
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // H
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // I
    {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, // J
    {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00}, // K
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, // L
    {0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00}, // M
    {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, // N
    {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // O
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, // P
    {0x7C,0xC6,0xC6,0xC6,0xC6,0xCE,0x7C,0x06}, // Q
    {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, // R
    {0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00}, // S
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // T
    {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // U
    {0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, // V
    {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00}, // W
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, // X
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, // Y
    {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}  // Z
};

class Interactive3DEngine {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* screenTexture = nullptr;
    std::vector<uint32_t> pixelBuffer;
    bool isRunning = false;

    GameState currentState = STATE_GAMEPLAY;
    RenderMode currentRenderMode = RENDER_TEXTURED;
    int selectedMenuItem = 0;

    // Procedural Textures (64x64)
    std::vector<std::vector<uint32_t>> textures;

    int levelMap[MAP_ROWS][MAP_COLS];
    struct Point { int x, y; } startPos, endPos;

    struct Player {
        float posX = 1.5f;
        float posY = 1.5f;
        float dirX = -1.0f;
        float dirY = 0.0f;
        float planeX = 0.0f;
        float planeY = 0.66f; // Standard ~66 FOV
        float moveSpeed = 3.2f;
        float rotSpeed = 2.4f;

        int forward = 0;
        int strafe = 0;
        int rotate = 0;
    } player;

    void generateTextures() {
        textures.resize(4, std::vector<uint32_t>(TEX_WIDTH * TEX_HEIGHT));

        for (int x = 0; x < TEX_WIDTH; ++x) {
            for (int y = 0; y < TEX_HEIGHT; ++y) {
                // Texture 0: Stone Blocks with Mortar
                bool isMortar = (x == 0 || x == TEX_WIDTH - 1 || y == 0 || y == TEX_HEIGHT - 1 || (y == 32 && (x % 32 != 0)));
                textures[0][y * TEX_WIDTH + x] = isMortar ? 0xFF2A2D34 : 0xFF5A6275;

                // Texture 1: Red Dungeon Brick
                bool isBrickBorder = (y % 16 == 0) || ((y / 16) % 2 == 0 && x % 32 == 0) || ((y / 16) % 2 != 0 && (x + 16) % 32 == 0);
                textures[1][y * TEX_WIDTH + x] = isBrickBorder ? 0xFF2B1111 : 0xFF8A3324;

                // Texture 2: Golden Goal Portal
                int cx = x - 32, cy = y - 32;
                int distSq = cx * cx + cy * cy;
                bool ring = (distSq > 100 && distSq < 150) || (distSq > 300 && distSq < 400);
                textures[2][y * TEX_WIDTH + x] = ring ? 0xFFFFFBEB : 0xFFD97706;

                // Texture 3: Tech Grid
                bool grid = (x % 8 == 0 || y % 8 == 0);
                textures[3][y * TEX_WIDTH + x] = grid ? 0xFF38BDF8 : 0xFF0F172A;
            }
        }
    }

    void generateMaze() {
        for (int r = 0; r < MAP_ROWS; ++r) {
            for (int c = 0; c < MAP_COLS; ++c) {
                levelMap[r][c] = 1;
            }
        }

        std::stack<Point> stack;
        startPos = { 1, 1 };
        levelMap[startPos.y][startPos.x] = 0;
        stack.push(startPos);

        const int dx[4] = { 0, 0, 2, -2 };
        const int dy[4] = { -2, 2, 0, 0 };

        while (!stack.empty()) {
            Point curr = stack.top();
            std::vector<int> dirs;

            for (int i = 0; i < 4; ++i) {
                int nx = curr.x + dx[i];
                int ny = curr.y + dy[i];
                if (nx > 0 && nx < MAP_COLS - 1 && ny > 0 && ny < MAP_ROWS - 1) {
                    if (levelMap[ny][nx] == 1) dirs.push_back(i);
                }
            }

            if (!dirs.empty()) {
                int d = dirs[rand() % dirs.size()];
                levelMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2] = 0;
                levelMap[curr.y + dy[d]][curr.x + dx[d]] = 0;
                stack.push({ curr.x + dx[d], curr.y + dy[d] });
            } else {
                stack.pop();
            }
        }

        endPos = { MAP_COLS - 2, MAP_ROWS - 2 };
        levelMap[endPos.y][endPos.x] = 2; // Goal Tile

        player.posX = startPos.x + 0.5f;
        player.posY = startPos.y + 0.5f;
    }

    inline uint32_t applyShade(uint32_t color, float factor) {
        factor = std::clamp(factor, 0.10f, 1.0f);
        uint8_t a = (color >> 24) & 0xFF;
        uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * factor);
        uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * factor);
        uint8_t b = static_cast<uint8_t>((color & 0xFF) * factor);
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    void drawChar(int px, int py, char c, uint32_t color) {
        if (c < 32 || c > 127) return;
        const uint8_t* glyph = FONT_8X8[c - 32];

        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                if ((glyph[y] >> (7 - x)) & 1) {
                    int screenX = px + x;
                    int screenY = py + y;
                    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
                        pixelBuffer[screenY * SCREEN_WIDTH + screenX] = color;
                    }
                }
            }
        }
    }

    void drawString(int px, int py, const std::string& text, uint32_t color) {
        int cursor = px;
        for (char c : text) {
            char upperC = std::toupper(static_cast<unsigned char>(c));
            drawChar(cursor, py, upperC, color);
            cursor += 8;
        }
    }

    void drawRect(int rx, int ry, int rw, int rh, uint32_t color) {
        for (int y = ry; y < ry + rh; ++y) {
            if (y < 0 || y >= SCREEN_HEIGHT) continue;
            for (int x = rx; x < rx + rw; ++x) {
                if (x < 0 || x >= SCREEN_WIDTH) continue;
                pixelBuffer[y * SCREEN_WIDTH + x] = color;
            }
        }
    }

public:
    bool init() {
        srand(static_cast<unsigned int>(time(nullptr)));

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return false;

        window = SDL_CreateWindow(
            "Retro 3D Renderer Testbed & Menu",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE,
            SDL_WINDOW_SHOWN
        );

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT
        );

        pixelBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000);
        
        generateTextures();
        generateMaze();
        isRunning = true;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = (currentState == STATE_GAMEPLAY) ? STATE_MENU : STATE_GAMEPLAY;
                }

                if (currentState == STATE_MENU) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
                        selectedMenuItem = (selectedMenuItem - 1 + 4) % 4;
                    }
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                        selectedMenuItem = (selectedMenuItem + 1) % 4;
                    }
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        executeMenuOption();
                    }
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        adjustOptionValue(-1);
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT) {
                        adjustOptionValue(1);
                    }
                } else {
                    if (event.key.keysym.sym == SDLK_r) generateMaze();
                    if (event.key.keysym.sym == SDLK_m) {
                        currentRenderMode = static_cast<RenderMode>((currentRenderMode + 1) % RENDER_MODE_COUNT);
                    }
                }
            }
        }

        if (currentState == STATE_GAMEPLAY) {
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            player.forward = 0;
            player.strafe = 0;
            player.rotate = 0;

            if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])    player.forward += 1;
            if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])  player.forward -= 1;
            if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])  player.rotate += 1;
            if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) player.rotate -= 1;
        }
    }

    void adjustOptionValue(int delta) {
        if (selectedMenuItem == 0) {
            // Cycle Render Mode
            currentRenderMode = static_cast<RenderMode>((currentRenderMode + delta + RENDER_MODE_COUNT) % RENDER_MODE_COUNT);
        } else if (selectedMenuItem == 1) {
            // Adjust Speed
            player.moveSpeed = std::clamp(player.moveSpeed + delta * 0.5f, 1.0f, 8.0f);
        } else if (selectedMenuItem == 2) {
            // Adjust FOV ratio
            player.planeY = std::clamp(player.planeY + delta * 0.05f, 0.40f, 1.20f);
        }
    }

    void executeMenuOption() {
        if (selectedMenuItem == 0) {
            adjustOptionValue(1);
        } else if (selectedMenuItem == 3) {
            currentState = STATE_GAMEPLAY;
        }
    }

    void update(double dtSeconds) {
        if (currentState != STATE_GAMEPLAY) return;

        float moveStep = player.forward * player.moveSpeed * static_cast<float>(dtSeconds);
        float rotStep  = player.rotate  * player.rotSpeed  * static_cast<float>(dtSeconds);

        if (player.rotate != 0) {
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rotStep) - player.dirY * sin(rotStep);
            player.dirY = oldDirX * sin(rotStep) + player.dirY * cos(rotStep);

            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rotStep) - player.planeY * sin(rotStep);
            player.planeY = oldPlaneX * sin(rotStep) + player.planeY * cos(rotStep);
        }

        if (player.forward != 0) {
            float buffer = (moveStep > 0) ? 0.25f : -0.25f;
            float newX = player.posX + player.dirX * moveStep;
            float newY = player.posY + player.dirY * moveStep;

            if (levelMap[int(player.posY)][int(newX + player.dirX * buffer)] != 1) player.posX = newX;
            if (levelMap[int(newY + player.dirY * buffer)][int(player.posX)] != 1) player.posY = newY;
        }

        if (int(player.posX) == endPos.x && int(player.posY) == endPos.y) {
            generateMaze();
        }
    }

    void render3DView() {
        // 1. Clear Ceiling & Floor
        for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                pixelBuffer[y * SCREEN_WIDTH + x] = 0xFF111827; // Deep Slate Ceiling
                pixelBuffer[(SCREEN_HEIGHT - 1 - y) * SCREEN_WIDTH + x] = 0xFF030712; // Void Floor
            }
        }

        // 2. Cast Rays across columns
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            float cameraX = 2.0f * x / float(SCREEN_WIDTH) - 1.0f;
            float rayDirX = player.dirX + player.planeX * cameraX;
            float rayDirY = player.dirY + player.planeY * cameraX;

            int mapX = int(player.posX);
            int mapY = int(player.posY);

            float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);
            float sideDistX, sideDistY;
            float perpWallDist;

            int stepX, stepY;
            int hit = 0;
            int side = 0;

            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (player.posX - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0f - player.posX) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (player.posY - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0f - player.posY) * deltaDistY;
            }

            while (hit == 0) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if (mapX >= 0 && mapX < MAP_COLS && mapY >= 0 && mapY < MAP_ROWS) {
                    if (levelMap[mapY][mapX] > 0) hit = levelMap[mapY][mapX];
                }
            }

            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else           perpWallDist = (sideDistY - deltaDistY);
            if (perpWallDist < 0.05f) perpWallDist = 0.05f;

            int lineHeight = int(SCREEN_HEIGHT / perpWallDist);
            int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;

            int clampedStart = std::max(0, drawStart);
            int clampedEnd = std::min(SCREEN_HEIGHT - 1, drawEnd);

            // Shading & Attenuation calculations
            float distanceFalloff = 1.0f / (1.0f + perpWallDist * 0.15f);
            float sideShade = (side == 1) ? 0.75f : 1.0f;

            if (currentRenderMode == RENDER_TEXTURED) {
                // Calculate exact wall X coordinate (where ray hit wall)
                float wallX;
                if (side == 0) wallX = player.posY + perpWallDist * rayDirY;
                else           wallX = player.posX + perpWallDist * rayDirX;
                wallX -= std::floor(wallX);

                // Texture X coordinate
                int texX = int(wallX * float(TEX_WIDTH));
                if (side == 0 && rayDirX > 0) texX = TEX_WIDTH - texX - 1;
                if (side == 1 && rayDirY < 0) texX = TEX_WIDTH - texX - 1;

                int texIndex = (hit == 2) ? 2 : 0; // Goal Portal vs Stone Wall

                // Draw vertical textured line
                float step = 1.0f * TEX_HEIGHT / lineHeight;
                float texPos = (clampedStart - SCREEN_HEIGHT / 2.0f + lineHeight / 2.0f) * step;

                for (int y = clampedStart; y <= clampedEnd; ++y) {
                    int texY = (int)texPos & (TEX_HEIGHT - 1);
                    texPos += step;

                    uint32_t color = textures[texIndex][TEX_HEIGHT * texY + texX];
                    pixelBuffer[y * SCREEN_WIDTH + x] = applyShade(color, distanceFalloff * sideShade);
                }
            } 
            else if (currentRenderMode == RENDER_SOLID_SHADED) {
                uint32_t solidColor = (hit == 2) ? 0xFFFACC15 : 0xFF64748B;
                uint32_t shaded = applyShade(solidColor, distanceFalloff * sideShade);
                for (int y = clampedStart; y <= clampedEnd; ++y) {
                    pixelBuffer[y * SCREEN_WIDTH + x] = shaded;
                }
            } 
            else if (currentRenderMode == RENDER_DEPTH_HEATMAP) {
                // Depth buffer visualization (White = Near, Dark Purple = Far)
                uint8_t depth = static_cast<uint8_t>(std::clamp(255.0f / (1.0f + perpWallDist * 0.3f), 0.0f, 255.0f));
                uint32_t heatColor = (0xFF << 24) | (depth << 16) | ((depth / 2) << 8) | depth;
                for (int y = clampedStart; y <= clampedEnd; ++y) {
                    pixelBuffer[y * SCREEN_WIDTH + x] = heatColor;
                }
            }
        }
    }

    void renderUIOverlay() {
        // HUD at the top
        drawString(8, 8, "ESC: MENU | M: SHADER | R: MAZE", 0xFF94A3B8);

        std::string modeText = "MODE: ";
        if (currentRenderMode == RENDER_TEXTURED) modeText += "TEXTURED";
        else if (currentRenderMode == RENDER_SOLID_SHADED) modeText += "FLAT SHADED";
        else modeText += "DEPTH BUFFER";
        drawString(8, 20, modeText, 0xFF38BDF8);

        // Pause Menu Dialog
        if (currentState == STATE_MENU) {
            // Semi-transparent dark overlay box
            drawRect(40, 40, 240, 160, 0xFF0F172A);
            drawRect(38, 38, 244, 2, 0xFF38BDF8); // Top border
            drawRect(38, 198, 244, 2, 0xFF38BDF8); // Bottom border

            drawString(110, 52, "- SETTINGS -", 0xFFFACC15);

            // Menu Items
            std::string menuItems[4] = {
                "RENDERER: " + std::string(currentRenderMode == RENDER_TEXTURED ? "TEXTURED" : (currentRenderMode == RENDER_SOLID_SHADED ? "FLAT" : "HEATMAP")),
                "SPEED:    " + std::to_string(int(player.moveSpeed * 10)),
                "FOV:      " + std::to_string(int(player.planeY * 100)),
                "RESUME GAME"
            };

            for (int i = 0; i < 4; ++i) {
                uint32_t col = (i == selectedMenuItem) ? 0xFF22C55E : 0xFFE2E8F0;
                std::string prefix = (i == selectedMenuItem) ? "> " : "  ";
                drawString(60, 80 + i * 22, prefix + menuItems[i], col);
            }

            drawString(55, 175, "LEFT/RIGHT: ADJUST | ENTER: SELECT", 0xFF64748B);
        }
    }

    void render() {
        render3DView();
        renderUIOverlay();

        SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void run() {
        uint32_t previousTime = SDL_GetTicks();
        double lag = 0.0;

        while (isRunning) {
            uint32_t currentTime = SDL_GetTicks();
            double elapsedTime = static_cast<double>(currentTime - previousTime);
            previousTime = currentTime;
            lag += elapsedTime;

            handleEvents();

            while (lag >= FIXED_TIMESTEP) {
                update(FIXED_TIMESTEP / 1000.0);
                lag -= FIXED_TIMESTEP;
            }

            render();
            SDL_Delay(1);
        }
    }

    void cleanup() {
        if (screenTexture) SDL_DestroyTexture(screenTexture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    Interactive3DEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();
    return 0;
}