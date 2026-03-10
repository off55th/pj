#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");


HBRUSH hBrushRed;
HWND hDiceText;
HWND hTurnText;
bool gameOver = false; //สถานะเกม
int playerPos[4] = {0,0,0,0}; //ตำแหน่งบนกระดาน
double playerHP[4] = {100,100,100,100};
double playerATK[4] = {10,10,10,10};
HWND hPlayerText[4];
bool playerAlive[4] = {true,true,true,true}; //สถานะมีชีวิต
int currentPlayer = 0;
int stepsToMove = 0;
bool isMoving = false;
int diceValue = 0;

HWND hWinnerText;
int playerLapCount[4] = {0,0,0,0}; // นับจำนวนรอบของแต่ละผู้เล่น
int winLapGoal = 5; // จำนวนรอบที่ต้องครบเพื่อชนะ
int aliveCheckCount;
int lastAliveIndex;
HWND hLapText[4];
int playerMoney[4] = {500,500,500,500};
HWND hMoneyText[4];
int playerCount = 4; // จำนวนผู้เล่น (2-4)
HWND hBtn2P, hBtn3P, hBtn4P;
bool playerSelected = false;
int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(RGB(60,120,90));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("fighting board game"),       /* Title Text */
           WS_MAXIMIZEBOX | WS_SYSMENU, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           1900,                 /* The programs width */
           820,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
void GetBoardPosition(int pos, int *x, int *y)
{
    int cell = 40;
    int rows = 10;
    int cols = 10;

    int screenW = 1900;
    int screenH = 820;

    int gridW = cols * cell;
    int gridH = rows * cell;

    int startX = (screenW - gridW) / 2;
    int startY = (screenH - gridH) / 2;

    if(pos < 10)//บน
    {
        *x = startX + pos * cell;
        *y = startY;
    }
    else if(pos < 19)//ขวา
    {
        *x = startX + 9 * cell;
        *y = startY + (pos-9) * cell;
    }
    else if(pos < 28)//ล่าง
    {
        *x = startX + (27-pos) * cell;
        *y = startY + 9 * cell;
    }
    else//ซ้าย
    {
        *x = startX;
        *y = startY + (36-pos) * cell;
    }
}

void UpdateMoneyUI(int player)
{
    char moneyText[50];
    sprintf(moneyText,"Money: %d",playerMoney[player]);
    SetWindowText(hMoneyText[player], moneyText);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
case WM_CREATE:
{

srand(time(NULL));

hBrushRed = CreateSolidBrush(RGB(60,120,90));
//-----------------------start-------------//
//ปุ่ม หาจำนวนผู้เล่น
hBtn2P = CreateWindow(
    "BUTTON", "2 Players",
    WS_VISIBLE | WS_CHILD | WS_BORDER,
    800, 300, 120, 40,
    hwnd, (HMENU)2001, NULL, NULL
);

hBtn3P = CreateWindow(
    "BUTTON", "3 Players",
    WS_VISIBLE | WS_CHILD | WS_BORDER,
    800, 350, 120, 40,
    hwnd, (HMENU)2002, NULL, NULL
);

hBtn4P = CreateWindow(
    "BUTTON", "4 Players",
    WS_VISIBLE | WS_CHILD | WS_BORDER,
    800, 400, 120, 40,
    hwnd, (HMENU)2003, NULL, NULL
);


for(int i = playerCount; i < 4; i++) // ไม่มีผู้เล่นให้ตาย
{
    playerAlive[i] = false;
    playerHP[i] = 0;
    playerATK[i] = 0;
    playerMoney[i] = 0;

    // อัปเดต UI
    SetWindowText(hPlayerText[i], "DEAD");
    SetWindowText(hMoneyText[i], "Money: 0");
    SetWindowText(hLapText[i], "Lap: -");
}
//------------------------------------------------//

// ข้อความLab
hLapText[0] = CreateWindow("STATIC",
                           "Lap: 0/5",
                           WS_VISIBLE | WS_CHILD,
                           270, 10, 100, 20,
                           hwnd, NULL, NULL, NULL);

hLapText[1] = CreateWindow("STATIC",
                           "Lap: 0/5",
                           WS_VISIBLE | WS_CHILD,
                           270, 30, 100, 20,
                           hwnd, NULL, NULL, NULL);

hLapText[2] = CreateWindow("STATIC",
                           "Lap: 0/5",
                           WS_VISIBLE | WS_CHILD,
                           270, 50, 100, 20,
                           hwnd, NULL, NULL, NULL);

hLapText[3] = CreateWindow("STATIC",
                           "Lap: 0/5",
                           WS_VISIBLE | WS_CHILD,
                           270, 70, 100, 20,
                           hwnd, NULL, NULL, NULL);

//ข้อความผู้ชนะ
hWinnerText = CreateWindow("STATIC",
                           "",
                           WS_VISIBLE | WS_CHILD,
                           850, 150, 300, 40,
                           hwnd, NULL, NULL, NULL);

//ข้่อความผู้เล่น
hPlayerText[0] = CreateWindow("STATIC",
                              "Player1: HP 100 ATK 10",
                              WS_VISIBLE | WS_CHILD,
                              10, 10, 250, 20,
                              hwnd, (HMENU)101, NULL, NULL);

hPlayerText[1] = CreateWindow("STATIC",
                              "Player2: HP 100 ATK 10",
                              WS_VISIBLE | WS_CHILD,
                              10, 30, 250, 20,
                              hwnd, (HMENU)102, NULL, NULL);

hPlayerText[2] = CreateWindow("STATIC",
                              "Player3: HP 100 ATK 10",
                              WS_VISIBLE | WS_CHILD,
                              10, 50, 250, 20,
                              hwnd, (HMENU)103, NULL, NULL);

hPlayerText[3] = CreateWindow("STATIC",
                              "Player4: HP 100 ATK 10",
                              WS_VISIBLE | WS_CHILD,
                              10, 70, 250, 20,
                              hwnd, (HMENU)104, NULL, NULL);
// เงินผู้เล่น
hMoneyText[0] = CreateWindow("STATIC",
                             "Money: 500",
                             WS_VISIBLE | WS_CHILD,
                             380, 10, 150, 20,
                             hwnd, NULL, NULL, NULL);

hMoneyText[1] = CreateWindow("STATIC",
                             "Money: 500",
                             WS_VISIBLE | WS_CHILD,
                             380, 30, 150, 20,
                             hwnd, NULL, NULL, NULL);

hMoneyText[2] = CreateWindow("STATIC",
                             "Money: 500",
                             WS_VISIBLE | WS_CHILD,
                             380, 50, 150, 20,
                             hwnd, NULL, NULL, NULL);

hMoneyText[3] = CreateWindow("STATIC",
                             "Money: 500",
                             WS_VISIBLE | WS_CHILD,
                             380, 70, 150, 20,
                             hwnd, NULL, NULL, NULL);


//ข้อความเทิร์น
hTurnText = CreateWindow("STATIC",
                         "Turn: Player 1",
                         WS_VISIBLE | WS_CHILD,
                         900, 110, 200, 30,
                         hwnd, NULL, NULL, NULL);
//ปุ่มสุ่ม
CreateWindow("BUTTON",
             "Roll Dice",
             WS_VISIBLE | WS_CHILD | WS_BORDER,
             900, 20, 100, 40,
             hwnd, (HMENU)1, NULL, NULL);
//textแสดงdiceที่ได้
hDiceText = CreateWindow("STATIC",
                         "Dice: -",
                         WS_VISIBLE | WS_CHILD,
                         900, 70, 150, 30,
                         hwnd, NULL, NULL, NULL);

}




        break ;

case WM_TIMER:


if(stepsToMove > 0)
{
    playerPos[currentPlayer]++;

    if(playerPos[currentPlayer] > 35) //เช็คเดินครบรอบ
{
    playerPos[currentPlayer] = 0;
    playerLapCount[currentPlayer]++; // เพิ่มรอบ
    char lapText[50];
    sprintf(lapText,"Lap: %d/5",playerLapCount[currentPlayer]);
    SetWindowText(hLapText[currentPlayer],lapText);
}
    stepsToMove--;

    InvalidateRect(hwnd,NULL,TRUE);
}
else
{
    KillTimer(hwnd,1);
    isMoving = false;

    int pos = playerPos[currentPlayer];

// BLUE EVENT (STEAL ATK)
if(pos == 2 || pos == 13 || pos == 27 || pos == 32)
{
    int target;

    // เลือกเป้าหมายแบบสุ่มที่ไม่ใช่ตัวเอง
    do{
        target = rand() % playerCount;
    }while(target == currentPlayer || !playerAlive[target]);

    int steal = rand()%15 + 1;

    if(playerATK[target] < steal)
        steal = playerATK[target];

    playerATK[target] -= steal;
    playerATK[currentPlayer] += steal;

    char txt[150];
    sprintf(txt,
    "Player %d stole %d ATK from Player %d!",
    currentPlayer+1,steal,target+1);

    MessageBox(hwnd,txt,"Blue Event",MB_OK);

    // update UI
    char hpText[50];

    sprintf(hpText,"Player%d: HP %.1f ATK %.1f",
            target+1,playerHP[target],playerATK[target]);
    SetWindowText(hPlayerText[target],hpText);

    sprintf(hpText,"Player%d: HP %.1f ATK %.1f",
            currentPlayer+1,
            playerHP[currentPlayer],
            playerATK[currentPlayer]);
    SetWindowText(hPlayerText[currentPlayer],hpText);
}
    // EVENT ช่องพิเศษ
if(pos == 5 || pos == 10 || pos == 15 ||
   pos == 20 || pos == 25 || pos == 30 || pos == 35)
{
    int event = rand() % 4;

    if(event == 0)
    {
        int hpGain = rand()%20 + 1;

        playerHP[currentPlayer] += hpGain;

        char txt[100];
        sprintf(txt,"Event: +%d HP",hpGain);
        MessageBox(hwnd,txt,"Event",MB_OK);
    }
    else if(event == 1)
    {
        playerHP[currentPlayer] -= 20;

        if(playerHP[currentPlayer] <= 0)
        {
            playerHP[currentPlayer] = 0;
            playerAlive[currentPlayer] = false;
            MessageBox(hwnd,"Event: -20 HP (You died!)","Game",MB_OK);
        }
        else
        {
            MessageBox(hwnd,"Event: -20 HP","Event",MB_OK);
        }
    }
    else if(event == 2)
    {
        int atkGain = rand()%8 + 1;

        playerATK[currentPlayer] += atkGain;

        char txt[100];
        sprintf(txt,"Event: +%d ATK",atkGain);
        MessageBox(hwnd,txt,"Event",MB_OK);
    }
    else if(event == 3)
    {
        int atkLoss = rand()%8 + 1;

        playerATK[currentPlayer] -= atkLoss;

        if(playerATK[currentPlayer] < 0)
            playerATK[currentPlayer] = 0;

        char txt[100];
        sprintf(txt,"Event: -%d ATK",atkLoss);
        MessageBox(hwnd,txt,"Event",MB_OK);
    }

    char hpText[50];
    sprintf(hpText,"Player%d: HP %.1f ATK %.1f",
            currentPlayer+1,
            playerHP[currentPlayer],
            playerATK[currentPlayer]);

    SetWindowText(hPlayerText[currentPlayer],hpText);
}

// ===== PINK EVENT (MONEY SYSTEM) =====
if(pos == 1 || pos == 9 || pos == 18 || pos == 34)
{
    int event = rand() % 4;

    //  ขโมยเงินจากผู้เล่นอื่น
    if(event == 0)
    {
        int target;
        do{
            target = rand() % playerCount;
        }while(target == currentPlayer || !playerAlive[target]);

        int steal = rand()%50 + 1;
        if(playerMoney[target] < steal)
            steal = playerMoney[target];

        playerMoney[target] -= steal;
        playerMoney[currentPlayer] += steal;

        char txt[150];
        sprintf(txt,"Pink Event: Player %d stole %d money from Player %d",
                currentPlayer+1,steal,target+1);

        MessageBox(hwnd,txt,"Pink Event",MB_OK);

        UpdateMoneyUI(target);
        UpdateMoneyUI(currentPlayer);
    }

    // เสียเงินตัวเอง
    else if(event == 1)
    {
        int loss = rand()%25 + 1;
        if(playerMoney[currentPlayer] < loss)
            loss = playerMoney[currentPlayer];

        playerMoney[currentPlayer] -= loss;

        char txt[100];
        sprintf(txt,"Pink Event: You lost %d money",loss);

        MessageBox(hwnd,txt,"Pink Event",MB_OK);
        UpdateMoneyUI(currentPlayer);
    }

    // ได้เงิน
    else if(event == 2)
    {
        int gain = rand()%50 + 1;
        playerMoney[currentPlayer] += gain;

        char txt[100];
        sprintf(txt,"Pink Event: You gained %d money",gain);

        MessageBox(hwnd,txt,"Pink Event",MB_OK);
        UpdateMoneyUI(currentPlayer);
    }

    // ให้เงินผู้เล่นอื่น
    else if(event == 3)
    {
        int target;
        do{
            target = rand() % playerCount;
        }while(target == currentPlayer || !playerAlive[target]);

        int give = rand()%25 + 1;
        if(playerMoney[currentPlayer] < give)
            give = playerMoney[currentPlayer];

        playerMoney[currentPlayer] -= give;
        playerMoney[target] += give;

        char txt[150];
        sprintf(txt,"Pink Event: Player %d give %d money to Player %d",
                currentPlayer+1,give,target+1);

        MessageBox(hwnd,txt,"Pink Event",MB_OK);

        UpdateMoneyUI(currentPlayer);
        UpdateMoneyUI(target);
    }
}

// ===== PURPLE CARD TILE (YES / NO SHOP) =====
if(pos == 23 || pos == 26 || pos == 29)
{
    int card = rand() % 5;
    int choice;

    // +10 ATK | 100
    if(card == 0)
    {
        choice = MessageBox(
            hwnd,
            "+10 ATK Card\nPrice: 100\nBuy this card?",
            "Purple Card",
            MB_YESNO | MB_ICONQUESTION
        );

        if(choice == IDYES)
        {
            if(playerMoney[currentPlayer] >= 100)
            {
                playerMoney[currentPlayer] -= 100;
                playerATK[currentPlayer] += 10;
                UpdateMoneyUI(currentPlayer);
            }
            else
                MessageBox(hwnd,"Not enough money!","Purple Card",MB_OK);
        }
    }

    // +50 HP | 100
    else if(card == 1)
    {
        choice = MessageBox(
            hwnd,
            "+50 HP Card\nPrice: 100\nBuy this card?",
            "Purple Card",
            MB_YESNO | MB_ICONQUESTION
        );

        if(choice == IDYES)
        {
            if(playerMoney[currentPlayer] >= 100)
            {
                playerMoney[currentPlayer] -= 100;
                playerHP[currentPlayer] += 50;
                UpdateMoneyUI(currentPlayer);
            }
            else
                MessageBox(hwnd,"Not enough money!","Purple Card",MB_OK);
        }
    }

    // Steal 10 ATK | 150
    else if(card == 2)
    {
        choice = MessageBox(
            hwnd,
            "Steal 10 ATK from another player\nPrice: 150\nBuy this card?",
            "Purple Card",
            MB_YESNO | MB_ICONQUESTION
        );

        if(choice == IDYES)
        {
            if(playerMoney[currentPlayer] >= 150)
            {
                int target;
                do{
                    target = rand() % playerCount;
                }while(target == currentPlayer || !playerAlive[target]);

                playerMoney[currentPlayer] -= 150;

                int steal = 10;
                if(playerATK[target] < steal)
                    steal = playerATK[target];

                playerATK[target] -= steal;
                playerATK[currentPlayer] += steal;

                UpdateMoneyUI(currentPlayer);
            }
            else
                MessageBox(hwnd,"Not enough money!","Purple Card",MB_OK);
        }
    }

    // +20 HP ให้ผู้เล่น 1 คน | 50
    else if(card == 3)
    {
        choice = MessageBox(
            hwnd,
            "+20 HP to a random player\nPrice: 50\nBuy this card?",
            "Purple Card",
            MB_YESNO | MB_ICONQUESTION
        );

        if(choice == IDYES)
        {
            if(playerMoney[currentPlayer] >= 50)
            {
                int target;
                do{
                    target = rand() % playerCount;
                }while(!playerAlive[target]);

                playerMoney[currentPlayer] -= 50;
                playerHP[target] += 20;

                UpdateMoneyUI(currentPlayer);
            }
            else
                MessageBox(hwnd,"Not enough money!","Purple Card",MB_OK);
        }
    }

    // +5 ATK ให้ผู้เล่น 1 คน | 50
    else if(card == 4)
    {
        choice = MessageBox(
            hwnd,
            "+5 ATK to a random player\nPrice: 50\nBuy this card?",
            "Purple Card",
            MB_YESNO | MB_ICONQUESTION
        );

        if(choice == IDYES)
        {
            if(playerMoney[currentPlayer] >= 50)
            {
                int target;
                do{
                    target = rand() % playerCount;
                }while(!playerAlive[target]);

                playerMoney[currentPlayer] -= 50;
                playerATK[target] += 5;

                UpdateMoneyUI(currentPlayer);
            }
            else
                MessageBox(hwnd,"Not enough money!","Purple Card",MB_OK);
        }
    }

    // อัปเดต UI HP / ATK
    for(int i=0;i<playerCount;i++)
    {
        char txt[60];
        sprintf(txt,"Player%d: HP %.1f ATK %.1f",
                i+1,playerHP[i],playerATK[i]);
        SetWindowText(hPlayerText[i],txt);
    }
}

        // BLACK EVENT
if(pos == 3 || pos == 7 || pos == 11 ||
   pos == 17 || pos == 19 || pos == 24 || pos == 31)
{
    int event = rand()%2;

    if(event == 0)
    {
        int back = rand()%3 + 1;

        for(int i=0;i<back;i++)
        {
            playerPos[currentPlayer]--;

            if(playerPos[currentPlayer] < 0)
                playerPos[currentPlayer] = 35;
        }

        char txt[100];
        sprintf(txt,"Black Tile: Move back %d spaces!",back);
        MessageBox(hwnd,txt,"Black Event",MB_OK);

        InvalidateRect(hwnd,NULL,TRUE);
    }
    else
    {
        int extra = rand()%3 + 1;
        stepsToMove = extra;

        char txt[100];
        sprintf(txt,"Black Tile: Move forward %d more!",extra);
        MessageBox(hwnd,txt,"Black Event",MB_OK);

        isMoving = true;
        SetTimer(hwnd,1,300,NULL);
        return 0;
    }
}
// ===== BATTLE SYSTEM =====
for(int i=0;i<playerCount;i++)
{
    if(i != currentPlayer &&
       playerPos[i] == playerPos[currentPlayer] &&
       playerAlive[i])
    {

        // ----- MISS SYSTEM (10%) -----
        int miss = rand()%100;

        if(miss < 10)
        {
            char missText[100];
            sprintf(missText,
            "Player %d attack missed Player %d!",
            currentPlayer+1,i+1);

            MessageBox(hwnd,missText,"Miss!",MB_OK);
        }
        else
        {
            // ----- DODGE SYSTEM (10%) -----
            int dodge = rand()%100;

            if(dodge < 10)
            {
                char dodgeText[100];
                sprintf(dodgeText,
                "Player %d dodged Player %d attack!",
                i+1,currentPlayer+1);

                MessageBox(hwnd,dodgeText,"Dodge!",MB_OK);
            }
            else
            {
                // ----- CRITICAL SYSTEM (30%) -----
                double damage = playerATK[currentPlayer];
                int crit = rand()%100;

                if(crit < 30)
                {
                    damage *= 2;

                    char critText[100];
                    sprintf(critText,
                    "CRITICAL HIT! Player %d dealt %.1f damage!",
                    currentPlayer+1,damage);

                    MessageBox(hwnd,critText,"Critical!",MB_OK);
                }

                // ----- ATTACK -----
                playerHP[i] -= damage;
                char battleText[100];
                sprintf(battleText,
                "Player %d attacked Player %d (-%.1f HP)",
                currentPlayer+1,i+1,damage);
                MessageBox(hwnd,battleText,"Battle",MB_OK);

                // ----- CHECK DEAD -----
                if(playerHP[i] <= 0)
                {
                    playerHP[i] = 0;
                    playerAlive[i] = false;

                    //โอนเงินทั้งหมด
                    int loot = playerMoney[i];
                    playerMoney[i] = 0;
                    playerMoney[currentPlayer] += loot;
                    char deadText[150];
                    sprintf(deadText,
                            "Player %d killed Player %d and got %d money!",
                            currentPlayer+1,i+1,loot);
                    MessageBox(hwnd,deadText,"Kill!",MB_OK);
                    UpdateMoneyUI(i);
                    UpdateMoneyUI(currentPlayer);
                }
                else
                {
                    // ----- COUNTER ATTACK (30%) -----
                    int counter = rand()%100;
                    if(counter < 30)
                    {
                    double counterDamage = playerATK[i] / 2;
                    playerHP[currentPlayer] -= counterDamage;
                    char counterText[100];
                    sprintf(counterText,
                            "Player %d counter attacked Player %d (-%.1f HP)",
                            i+1,currentPlayer+1,counterDamage);

                    MessageBox(hwnd,counterText,"Counter Attack",MB_OK);

                        if(playerHP[currentPlayer] <= 0)
                        {
                        playerHP[currentPlayer] = 0;
                        playerAlive[currentPlayer] = false;

                        // โอนเงินให้ผู้ที่ counter
                        int loot = playerMoney[currentPlayer];
                        playerMoney[currentPlayer] = 0;
                        playerMoney[i] += loot;

                        char deadText[150];
                        sprintf(deadText,
                                "Player %d counter-killed Player %d and got %d money!",
                                i+1,currentPlayer+1,loot);

                        MessageBox(hwnd,deadText,"Counter Kill!",MB_OK);

                        UpdateMoneyUI(currentPlayer);
                        UpdateMoneyUI(i);
}}}}}

// ----- UPDATE HP UI -----
char hpText[50];

sprintf(hpText,"Player%d: HP %.1f ATK %.1f",
        i+1,playerHP[i],playerATK[i]);
        SetWindowText(hPlayerText[i],hpText);

sprintf(hpText,"Player%d: HP %.1f ATK %.1f",
        currentPlayer+1,
        playerHP[currentPlayer],
        playerATK[currentPlayer]);
SetWindowText(hPlayerText[currentPlayer],hpText);
    }
}

// ===== CHECK LAST PLAYER ALIVE =====
aliveCheckCount = 0;
lastAliveIndex = -1;
for(int i=0;i<playerCount;i++)
{
    if(playerAlive[i])
    {
        aliveCheckCount++;
        lastAliveIndex = i;
    }
}
if(aliveCheckCount == 0)
{
    gameOver = true;
    MessageBox(hwnd,
        "All players died!\nNo winner.",
        "Game Over",
        MB_OK);
    return 0;
}

if(aliveCheckCount == 1)
{
    gameOver = true;

    char winText[100];
    sprintf(winText,"Player %d is the last survivor and wins!",lastAliveIndex+1);

    SetWindowText(hWinnerText,winText);
    MessageBox(hwnd,winText,"Game Over",MB_OK);

    return 0;
}
    if(playerLapCount[currentPlayer] >= winLapGoal)
{
    gameOver = true;

    char winText[100];
    sprintf(winText,"Player %d Wins! (Completed 5 laps)",currentPlayer+1);

    SetWindowText(hWinnerText,winText);
    MessageBox(hwnd,winText,"Game Over",MB_OK);

    return 0;
}

    // เปลี่ยนเทิร์น
    do{
        currentPlayer++;
        if(currentPlayer >= playerCount)
            currentPlayer = 0;
    }while(!playerAlive[currentPlayer]);

    char turnText[50];
    sprintf(turnText,"Turn: Player %d",currentPlayer+1);
    SetWindowText(hTurnText,turnText);
}

break;
case WM_PAINT:
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    int cell = 40;
    int x,y;

    // Brushes
    HBRUSH eventBrush = CreateSolidBrush(RGB(255,255,120));
    HBRUSH normalBrush = CreateSolidBrush(RGB(255,255,255));
    HBRUSH brownBrush = CreateSolidBrush(RGB(150,75,0));
    HBRUSH blueBrush = CreateSolidBrush(RGB(80,150,255));
    HBRUSH pinkBrush = CreateSolidBrush(RGB(255,105,180));
    HBRUSH purpleBrush = CreateSolidBrush(RGB(160, 80, 200));
    // วาดกระดาน 36 ช่อง
    for(int pos=0; pos<36; pos++)
    {
        GetBoardPosition(pos,&x,&y);

if(pos == 23 || pos == 26 || pos == 29)
{
    SelectObject(hdc, purpleBrush);
}
else if(pos == 1 || pos == 9 || pos == 18 || pos == 34)
{
    SelectObject(hdc, pinkBrush);
}
else if(pos == 2 || pos == 13 || pos == 27 || pos == 32)
{
    SelectObject(hdc, blueBrush);
}
else if(pos == 3 || pos == 7 || pos == 11 ||
        pos == 17 || pos == 19 || pos == 24 || pos == 31)
{
    SelectObject(hdc, brownBrush);
}
else if(pos == 5 || pos == 10 || pos == 15 ||
        pos == 20 || pos == 25 || pos == 30 || pos == 35)
{
    SelectObject(hdc, eventBrush);
}
else
{
    SelectObject(hdc, normalBrush);
}

        Rectangle(hdc,x,y,x+cell,y+cell);
    }

    // วาดผู้เล่น
    int r = 10;
    HBRUSH playerBrush[4];
    playerBrush[0] = CreateSolidBrush(RGB(255,0,0));     // Player 1
    playerBrush[1] = CreateSolidBrush(RGB(0,0,255));     // Player 2
    playerBrush[2] = CreateSolidBrush(RGB(0,255,0));     // Player 3
    playerBrush[3] = CreateSolidBrush(RGB(255,255,0));   // Player 4

    for(int i = 0; i < playerCount; i++)
    {
        if(!playerAlive[i]) continue;

        GetBoardPosition(playerPos[i], &x, &y);
        SelectObject(hdc, playerBrush[i]);

        // หมากซ้อน
        int offsetX = (i % 2) * 12 + 5;
        int offsetY = (i / 2) * 12 + 5;

        Ellipse(
            hdc,
            x + offsetX,
            y + offsetY,
            x + offsetX + r,
            y + offsetY + r
        );
    }


    EndPaint(hwnd,&ps);
}
break;

case WM_CTLCOLORSTATIC:
{
    HDC hdc = (HDC)wParam;
    int id = GetDlgCtrlID((HWND)lParam);

    SetBkMode(hdc, TRANSPARENT); //พื้นหลังโปร่งใส

    switch(id)//เปลี่ยนสีข้อความตามID(ชื่อผู้เล่น)
    {
        case 101: SetTextColor(hdc, RGB(255,0,0)); break;
        case 102: SetTextColor(hdc, RGB(0,0,255)); break;
        case 103: SetTextColor(hdc, RGB(0,255,0)); break;
        case 104: SetTextColor(hdc, RGB(255,255,0)); break;
    }

    return (LRESULT)hBrushRed;
}


case WM_COMMAND:
{
    int id = LOWORD(wParam);

    switch(id)
    {
        case 1: // Roll Dice
            if(gameOver || isMoving)
                break;

            diceValue = rand() % 6 + 1;
            stepsToMove = diceValue;
            isMoving = true;

            {
                char text[50];
                sprintf(text,"Dice: %d",diceValue);
                SetWindowText(hDiceText,text);
            }

            SetTimer(hwnd,1,300,NULL);
            break;
/////////////////จำนวน player///////////////////////////
        case 2001: // 2 Players
            playerCount = 2;
            playerSelected = true;
            break;

        case 2002: // 3 Players
            playerCount = 3;
            playerSelected = true;
            break;

        case 2003: // 4 Players
            playerCount = 4;
            playerSelected = true;
            break;
    }

    if(playerSelected)
    {
        ShowWindow(hBtn2P, SW_HIDE);
        ShowWindow(hBtn3P, SW_HIDE);
        ShowWindow(hBtn4P, SW_HIDE);

        for(int i = playerCount; i < 4; i++)
        {
            playerAlive[i] = false;
            playerHP[i] = 0;
            playerATK[i] = 0;
            playerMoney[i] = 0;

            SetWindowText(hPlayerText[i], "DEAD");
            SetWindowText(hMoneyText[i], "Money: 0");
            SetWindowText(hLapText[i], "Lap: -");
        }
    }
/////////////////////////////////////////////////////////////
}
break;
        case WM_DESTROY:

            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

