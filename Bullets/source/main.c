#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENEMIES 10 // Define the maximum number of enemies

void clearScreen() {
    printf("\x1b[1;1H\x1b[2J");
}

int main(int argc, char** argv)
{
    gfxInitDefault();

    // Initialize console on bottom screen
    PrintConsole bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&bottomScreen); // Select the bottom screen

    printf("Start to exit\n");
    printf("Made by Sethamphetamine\n");

    // Wait a bit for user to read
    svcSleepThread(1000000000); // 1 second

    // Initialize console on top screen
    consoleInit(GFX_TOP, NULL);

    // Define variables for player character position
    int playerX = 20;
    int playerY = 15;

    // Define variables for enemies
    int enemyCount = 0;
    int enemyX[MAX_ENEMIES];
    int enemyY[MAX_ENEMIES];
    int enemySpeed = 1;

    // Seed the random number generator
    srand(time(NULL));

    // Timing variables for enemy spawn
    u64 lastEnemySpawnTime = svcGetSystemTick(); // Initial time
    u64 enemySpawnInterval = 500000000; // 0.5 seconds in nanoseconds

    // Variable to track game over state
    bool gameOver = false;
    u64 gameOverTime = 0;
    u64 gameOverDelay = 2000000000; // 2 seconds in nanoseconds

    // Variable to indicate when to exit the game loop
    bool exitGameLoop = false;

    // Main loop
    while (aptMainLoop() && !exitGameLoop)
    {
        // Clear the entire top screen
        consoleSelect(&bottomScreen);
        clearScreen();

        // Draw player character at its current position
        printf("\x1b[%d;%dH()", playerY, playerX);

        // Check if it's time to spawn a new enemy
        u64 currentTime = svcGetSystemTick();
        if (!gameOver && currentTime - lastEnemySpawnTime >= enemySpawnInterval && enemyCount < MAX_ENEMIES) {
            // Spawn a new enemy
            enemyX[enemyCount] = 50;
            enemyY[enemyCount] = rand() % 30 + 1;
            enemyCount++;
            lastEnemySpawnTime = currentTime;
        }

        // Move existing enemies towards the left and check for collisions with the player
        for (int i = 0; i < enemyCount; i++) {
            if (!gameOver) {
                enemyX[i] -= enemySpeed;
                // If an enemy reaches the left edge of the screen, loop it to a random Y position
                if (enemyX[i] < 1) {
                    enemyX[i] = 50; // Reset X position to the right side of the screen
                    enemyY[i] = rand() % 30 + 1; // Set Y position to a random value
                }
                // Check for collision with player
                if (enemyX[i] == playerX && enemyY[i] == playerY) {
                    gameOver = true;
                    gameOverTime = currentTime;
                }
            }
            // Draw enemy at its current position
            printf("\x1b[%d;%dH.", enemyY[i], enemyX[i]);
        }

        // Check if game over and display message
        if (gameOver && (currentTime - gameOverTime >= gameOverDelay)) {
            consoleSelect(&bottomScreen);
            printf("\x1b[32;16HGame Over!");
            exitGameLoop = true; // Set the flag to exit the outer loop
        }

        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START)
            break;

        // Check D-pad input and update player character position accordingly
        if (!gameOver) {
            if (kDown & KEY_UP && playerY > 1)
                playerY--;
            if (kDown & KEY_DOWN && playerY < 30)
                playerY++;
            if (kDown & KEY_LEFT && playerX > 1)
                playerX--;
            if (kDown & KEY_RIGHT && playerX < 50)
                playerX++;
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();

        // Wait for VBlank
        gspWaitForVBlank();
    }

    // After the loop, we'll wait for 5 seconds before exiting
    u64 gameEndTime = svcGetSystemTick() + gameOverDelay;
    while (svcGetSystemTick() < gameEndTime) {
        // Clear the screen and display the game over message during the delay
        consoleSelect(&bottomScreen);
        clearScreen();
        printf("\x1b[32;16HGame Over!");

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();

        // Wait for VBlank
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
