// there are other files required in order to run the program you may visit my github to see them for yourself
// https://github.com/daniaru6340/GameDB/tree/main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <ncurses.h>
#include <menu.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <time.h>

#define KEY_ESC 27
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
// #define CTRLD 4
#define BUFF_SIZE 4096

int userindex = 0;

char *saltgen()
{
    size_t length = 2 + (rand() % (20 - 2 + 1));

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_len = strlen(charset);
    char *salt = malloc(length + 1);

    for (int i = 0; i < length; i++)
    {
        salt[i] = charset[rand() % charset_len];
    }
    salt[length] = '\0';
    return salt;
}

char *hash(char *passwd, char *salt)
{

    size_t i;
    char buff[BUFF_SIZE];
    unsigned int md_len;
    EVP_MD_CTX *mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];

    char *input = malloc(strlen(salt) + strlen(passwd) + 1);
    sprintf(input, "%s%s", salt, passwd);
    char *output = malloc(129);

    mdctx = EVP_MD_CTX_new();
    const EVP_MD *EVP_md5();

    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
    EVP_DigestUpdate(mdctx, input, strlen(input));
    EVP_DigestFinal(mdctx, md_value, &md_len);

    for (i = 0; i < md_len; i++)
    {

        sprintf(output, "%s%02x", output, md_value[i]);
    }

    // printw(" %s\n", input);

    EVP_MD_CTX_free(mdctx);
    free(input);
    return output;
}

int login(char *username, char *password)
{

    FILE *logininfofile = fopen("data/logindata.json", "r");

    if (logininfofile == NULL)
    {
        return 0;
    }

    fseek(logininfofile, 0, SEEK_END);
    long fileSize = ftell(logininfofile);
    fseek(logininfofile, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize + 1);
    fread(buffer, 1, fileSize, logininfofile);
    buffer[fileSize] = '\0';

    fclose(logininfofile);

    cJSON *logindata = cJSON_Parse(buffer);

    const cJSON *accounts = NULL;
    const cJSON *account = NULL;
    int status = 0;

    if (logindata == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
    }

    accounts = cJSON_GetObjectItemCaseSensitive(logindata, "accounts");
    cJSON_ArrayForEach(account, accounts)
    {
        cJSON *fuser = cJSON_GetObjectItemCaseSensitive(account, "username");
        cJSON *fpass = cJSON_GetObjectItemCaseSensitive(account, "password");
        cJSON *fsalt = cJSON_GetObjectItemCaseSensitive(account, "salt");
        cJSON *flvl = cJSON_GetObjectItemCaseSensitive(account, "level");

        char *passwd = hash(password, fsalt->valuestring);

        if ((cJSON_IsString(fuser)) && (fuser->valuestring != NULL))
        {
            if ((strcmp(username, fuser->valuestring) == 0) && (strcmp(passwd, fpass->valuestring)) == 0)
            {

                status = flvl->valueint;
            }
            else
            {
            }
        }
    }

    return status;
}

// Function prototype

void usermenu(int userindex, char *user);

// lowercase string

void toLower(char *str)
{

    for (int i = 0; i < strlen(str); i++)
    {

        if (isalpha(str[i]))
        {
            str[i] = tolower(str[i]);
        }
    }
}

// digits counter

int noDigits(int num)
{
    int i = 0;

    if (num == 0)
    {
        return 1;
    }

    while (num != 0)
    {
        num /= 10;
        i++;
    }
    return i;
}

// action to take when user choses an option from menu

void action(const char *choice, char *user)
{

    if (strcmp("Read", choice) == 0)
    {

        FILE *dataFile = fopen("info.json", "r");
        if (dataFile == NULL)
        {
            printw("File: 404 or 403");
        }
        fseek(dataFile, 0, SEEK_END);
        long fileSize = ftell(dataFile);
        fseek(dataFile, 0, SEEK_SET);

        char *data = malloc(fileSize + 1);
        if (data == NULL)
        {
            printw("Memory Allocation Err");
            fclose(dataFile);
            return;
        }

        size_t readSize = fread(data, 1, fileSize, dataFile);
        fclose(dataFile);
        data[readSize] = '\0';

        cJSON *json = cJSON_Parse(data);
        free(data);

        int noGames = cJSON_GetArraySize(json);
        cJSON *gameno = json->child;
        printw("%-3s%-40s%-20s%-6s\n", "ID", "Game", "Release", "Rating");
        curs_set(0);
        for (int i = 0; i < noGames; i++)
        {
            cJSON *gname, *rdate;
            double rating;

            gname = cJSON_GetObjectItemCaseSensitive(gameno, "Name");
            rdate = cJSON_GetObjectItemCaseSensitive(gameno, "Release");
            cJSON *ratingDataFromFile = cJSON_GetObjectItemCaseSensitive(gameno, "Rating");
            if (cJSON_IsNumber(ratingDataFromFile))
            {
                rating = ratingDataFromFile->valuedouble;
            }

            printw("%-3d%-40s%-20s%-6.2lf\n", i + 1, gname->valuestring, rdate->valuestring, rating);

            gameno = gameno->next;
        }
        getch();
        curs_set(1);

        clear();

        cJSON_Delete(json);
        usermenu(userindex, user);
    }
    else if (strcmp("Write", choice) == 0)
    {
        choice = NULL;
        char gname[400], rdate[100];
        double rating;
        int noGames;

        // See if file exist or not or has the neccessary contents

        FILE *filecr = fopen("info.json", "r");
        if (filecr == NULL)
        {
            printw("filecr File:403");
            refresh();
            free(filecr);
            return;
        }

        fseek(filecr, 0, SEEK_END);
        long filecrSize = ftell(filecr);
        fseek(filecr, 0, SEEK_SET);

        if (filecrSize == 0)
        {
            FILE *filecw = fopen("info.json", "w");
            fprintf(filecw, "{}");
            fclose(filecw);
        }

        fclose(filecr);

        // open file
        FILE *filer = fopen("info.json", "r");
        if (filer == NULL)
        {
            printw("filer File:403");
            fclose(filer);
            return;
        }

        // Check file size
        fseek(filer, 0, SEEK_END);
        long fileSize = ftell(filer);
        fseek(filer, 0, SEEK_SET);

        if (filer == 0)
        {
            FILE *filec = fopen("info.json", "w");
            fprintf(filec, "{}");
            fclose(filec);
        }

        // allocate enough memory to hold file
        char *data = malloc(fileSize + 1);
        if (data == NULL)
        {
            printw("Memory Allocation Err");
            fclose(filer);
            return;
        }

        cJSON *json;

        if (fileSize != 0)
        {
            // buffer the file
            size_t readSize = fread(data, 1, fileSize, filer);
            fclose(filer);
            data[readSize] = '\0';

            // parse the file
            json = cJSON_Parse(data);
            if (json == NULL)
            {
                printw("Err: Failed to parse JSON\n");
                free(data);
                return;
            }
            noGames = cJSON_GetArraySize(json);
        }

        // open file as write
        FILE *filew = fopen("info.json", "w");

        echo();
        // User data
        printw("GAME DB\n");
        refresh();
        printw("Name: ");
        refresh();
        getstr(gname);
        gname[strcspn(gname, "\n")] = 0;
        gname[sizeof(gname) - 1] = '\0';
        printw("Release Date: ");
        refresh();
        getstr(rdate);
        rdate[strcspn(rdate, "\n")] = 0;
        printw("Rating: ");
        refresh();
        scanw("%lf", &rating);

        cJSON *game = cJSON_CreateObject();
        if (game == NULL)
        {
            printw("Err: Failed to create object");
            cJSON_Delete(json);
            return;
        }

        cJSON_AddStringToObject(game, "Name", gname);
        cJSON_AddStringToObject(game, "Release", rdate);
        cJSON_AddNumberToObject(game, "Rating", rating);

        char *curNoGame = malloc((noDigits(noGames) + 1) * sizeof(char));
        // noGames = noGames + 1;

        sprintf(curNoGame, "%d", noGames + 1);

        if (json == NULL)
        {
            json = cJSON_CreateObject();
        }

        cJSON_AddItemToObject(json, curNoGame, game);

        // add items

        // save the file

        char *json_string = cJSON_Print(json);

        printw("%s", json_string);

        fprintf(filew, "%s", json_string);

        // free mem
        cJSON_Delete(json);
        fclose(filew);
        free(data);
        free(json_string);

        usermenu(userindex, user);
    }
    else if (strcmp("Edit", choice) == 0)
    {

        choice = NULL;
        char gname[400], rdate[100], id[20];
        double rating;
        int noGames;

        // See if file exist or not or has the neccessary contents

        FILE *filecr = fopen("info.json", "r");
        if (filecr == NULL)
        {
            printw("filecr File:403");
            refresh();
            free(filecr);
            return;
        }

        fseek(filecr, 0, SEEK_END);
        long filecrSize = ftell(filecr);
        fseek(filecr, 0, SEEK_SET);

        if (filecrSize == 0)
        {
            FILE *filecw = fopen("info.json", "w");
            fprintf(filecw, "{}");
            fclose(filecw);
        }

        fclose(filecr);

        // open file
        FILE *filer = fopen("info.json", "r");
        if (filer == NULL)
        {
            printw("filer File:403");
            fclose(filer);
            return;
        }

        // Check file size
        fseek(filer, 0, SEEK_END);
        long fileSize = ftell(filer);
        fseek(filer, 0, SEEK_SET);

        if (filer == 0)
        {
            FILE *filec = fopen("info.json", "w");
            fprintf(filec, "{}");
            fclose(filec);
        }

        // allocate enough memory to hold file
        char *data = malloc(fileSize + 1);
        if (data == NULL)
        {
            printw("Memory Allocation Err");
            fclose(filer);
            return;
        }

        cJSON *json;

        if (fileSize != 0)
        {
            // buffer the file
            size_t readSize = fread(data, 1, fileSize, filer);
            fclose(filer);
            data[readSize] = '\0';

            // parse the file
            json = cJSON_Parse(data);
            if (json == NULL)
            {
                printw("Err: Failed to parse JSON\n");
                free(data);
                return;
            }
            noGames = cJSON_GetArraySize(json);
        }

        // open file as write
        FILE *filew = fopen("info.json", "w");

        echo();
        // User data
        printw("GAME DB\n");
        refresh();
        printw("ID: ");
        getstr(id);
        printw("New Name: ");
        refresh();
        getstr(gname);
        gname[strcspn(gname, "\n")] = 0;
        gname[sizeof(gname) - 1] = '\0';
        printw("New Release Date: ");
        refresh();
        getstr(rdate);
        rdate[strcspn(rdate, "\n")] = 0;
        printw("New Rating: ");
        refresh();
        scanw("%lf", &rating);

        cJSON *game = cJSON_GetObjectItem(json, id);

        if (game)
        {
            cJSON *Name = cJSON_GetObjectItem(game, "Name");
            if (Name)
            {
                cJSON_SetValuestring(Name, gname);
            }

            cJSON *Release = cJSON_GetObjectItem(game, "Release");
            if (Release)
            {
                cJSON_SetValuestring(Release, rdate);
            }

            cJSON *Rating = cJSON_GetObjectItem(game, "Rating");
            if (Rating)
            {
                cJSON_SetNumberValue(Rating, rating);
            }
        }

        // save the file

        char *json_string = cJSON_Print(json);

        printw("%s", json_string);

        fprintf(filew, "%s", json_string);

        // free mem
        cJSON_Delete(json);
        fclose(filew);
        free(data);
        free(json_string);

        // printw("EOF");
        usermenu(userindex, user);
    }
    else if (strcmp("Search", choice) == 0)
    {

        char searchterm[400];

        FILE *datajson = fopen("info.json", "r");
        if (datajson == NULL)
        {
            printw("File: 404 or 403");
        }
        fseek(datajson, 0, SEEK_END);
        long fileSize = ftell(datajson);
        fseek(datajson, 0, SEEK_SET);

        char *data = malloc(fileSize + 1);
        if (data == NULL)
        {
            printw("Memory Allocation Err");
            fclose(datajson);
            return;
        }

        size_t readSize = fread(data, 1, fileSize, datajson);
        fclose(datajson);
        data[readSize] = '\0';

        cJSON *json = cJSON_Parse(data);
        free(data);

        echo();
        printw("Search: ");
        refresh();
        getstr(searchterm);
        noecho();

        int noGames = cJSON_GetArraySize(json);
        cJSON *gameno = json->child;
        printw("%-3s%-40s%-20s%-6s\n", "ID", "Game", "Release", "Rating");
        curs_set(0);
        int result = 0;
        for (int i = 0; i < noGames; i++)
        {
            cJSON *gname, *rdate;
            double rating;
            char ratingstring[3];

            gname = cJSON_GetObjectItemCaseSensitive(gameno, "Name");
            rdate = cJSON_GetObjectItemCaseSensitive(gameno, "Release");
            cJSON *ratingDataFromFile = cJSON_GetObjectItemCaseSensitive(gameno, "Rating");
            if (cJSON_IsNumber(ratingDataFromFile))
            {
                rating = ratingDataFromFile->valuedouble;
            }
            sprintf(ratingstring, "%d", rating);

            char *gameName = gname->valuestring, *releaseDate = rdate->valuestring;

            // search logic

            // lowecase all the search strings
            toLower(searchterm);
            toLower(gameName);
            toLower(releaseDate);

            char tempi[400];

            sprintf(tempi, "%d", i + 1);

            if ((strcmp(searchterm, gameName) == 0 || strcmp(searchterm, releaseDate) == 0 || strcmp(searchterm, ratingstring) == 0 || strcmp(searchterm, tempi) == 0) || (strstr(searchterm, gameName) != NULL || strstr(searchterm, releaseDate) != NULL || strstr(searchterm, ratingstring) != NULL))
            {
                result++;
                printw("%-3d%-40s%-20s%-6.2lf\n", i + 1, gname->valuestring, rdate->valuestring, rating);
            }

            gameno = gameno->next;
        }
        if (result == 0)
        {
            printw("NO RESULTS FOUND");
        }
        getch();
        curs_set(1);

        clear();

        cJSON_Delete(json);
        usermenu(userindex, user);
    }
    else if (strcmp("Add user", choice) == 0)
    {

        // User add logic

        FILE *logininfofile = fopen("data/logindata.json", "r");

        if (logininfofile == NULL)
        {
            printf("404/403 file\n");
            return;
        }

        fseek(logininfofile, 0, SEEK_END);
        long fileSize = ftell(logininfofile);
        fseek(logininfofile, 0, SEEK_SET);

        char *buffer = (char *)malloc(fileSize + 1);
        fread(buffer, 1, fileSize, logininfofile);
        buffer[fileSize] = '\0';

        fclose(logininfofile);

        cJSON *logindata = cJSON_Parse(buffer);

        const cJSON *naccounts = NULL;
        int status = 0;

        if (logindata == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            status = 0;
        }

        naccounts = cJSON_GetObjectItemCaseSensitive(logindata, "accounts");
        char nuser[1000];
        char npass[1000];
        int nlvl;

        echo();
        printw("Add New User\n");
        printw("Username: ");
        refresh();
        getstr(nuser);
        noecho();
        printw("Password: ");
        refresh();
        getstr(npass);
        echo();
        printw("Access level: ");
        refresh();
        scanf("%d", &nlvl);
        noecho();
        refresh();

        if (nuser == NULL || npass == NULL || (nlvl >= 3 && nlvl <= 1))
        {
            printw("invalid or insufficient data provided\n");
            refresh();
            return;
        }
        else
        {

            int usrindex = cJSON_GetArraySize(naccounts) + 1;
            char *salt = saltgen();
            char *hashedpass = hash(npass, salt);

            cJSON *newuser = cJSON_CreateObject();
            cJSON_AddStringToObject(newuser, "username", nuser);
            cJSON_AddStringToObject(newuser, "password", hashedpass);
            cJSON_AddStringToObject(newuser, "salt", salt);
            cJSON_AddNumberToObject(newuser, "level", nlvl);
            cJSON_AddNumberToObject(newuser, "index", usrindex);

            if (cJSON_AddItemToArray(naccounts, newuser) != 0)
            {

                printw("\nUser \"%s\", successfully added\n", nuser);
                refresh();
            }

            FILE *logindatafile = fopen("./data/logindata.json", "w");
            char *jsonString = cJSON_Print(logindata);

            refresh();

            fprintf(logindatafile, "%s", jsonString);
            fclose(logindatafile);
            printw("press any key to continue");
            refresh();
            getch();
        }

        usermenu(userindex, user);
        return;
    }
    else if (strcmp("Remove user", choice) == 0)
    {

        // retreive and parse the logininfofile

        FILE *logininfofile = fopen("data/logindata.json", "r");

        if (logininfofile == NULL)
        {
            printf("404/403 file\n");
            return;
        }

        fseek(logininfofile, 0, SEEK_END);
        long fileSize = ftell(logininfofile);
        fseek(logininfofile, 0, SEEK_SET);

        char *buffer = (char *)malloc(fileSize + 1);
        fread(buffer, 1, fileSize, logininfofile);
        buffer[fileSize] = '\0';

        fclose(logininfofile);

        cJSON *logindata = cJSON_Parse(buffer);

        const cJSON *naccounts = NULL;
        int status = 0;

        if (logindata == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            status = 0;
        }

        naccounts = cJSON_GetObjectItemCaseSensitive(logindata, "accounts");

        // get username for deletion

        char delUser[400];
        int userChildNo;
        int confirmination;
        char uInput;
        printw("User Deletion\n");
        printw("Username: ");
        echo();
        refresh();
        getstr(delUser);

        if (strcmp(delUser, user) == 0)
        {
            printw("Silly you cannot delete yourself\n");
        }
        else
        {

            // Deletion

            printw("Are you sure you want to delete \"%s\"[y/N]: ", delUser);
            uInput = getch();
            printw("\n");

            switch (uInput)
            {
            case 'Y':
            case 'y':
                printw("y\n");
                cJSON *acc;
                cJSON *currentAccount;
                cJSON *uIndex;

                cJSON_ArrayForEach(acc, naccounts)
                {

                    if (acc == NULL)
                    {
                        continue;
                    }

                    currentAccount = cJSON_GetObjectItem(acc, "username");
                    uIndex = cJSON_GetObjectItem(acc, "index");

                    if (currentAccount != NULL && strcmp(currentAccount->valuestring, delUser) == 0)
                    {
                        int delindex = uIndex->valueint;
                        cJSON_DetachItemFromArray(naccounts, delindex);

                        printw("user deleted");
                    }
                }

                // savefile
                FILE *logindatafile = fopen("./data/logindata.json", "w");
                char *jsonString = cJSON_Print(logindata);
                refresh();

                fprintf(logindatafile, "%s", jsonString);
                fclose(logindatafile);
                break;

            default:
                printw("\"%s\" not deleted\n", delUser);
                break;
            }

            refresh();
        }

        usermenu(userindex, user);
    }
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);

    wrefresh(local_win);

    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}

// MENU

void usermenu(int userindex, char *user)
{
    refresh();

    ITEM **app_items;
    int c;
    MENU *app_menu;
    int no_choices, i;
    ITEM *current_item_no;
    char *choices[6];

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    if (userindex >= 1)
    {

        printw("\n\tLogged in as %s\n", user);
        refresh();
        clear();
        refresh();
    }

    // initialize window
    WINDOW *menu_win;

    int ch;
    int row, col;
    int startx, starty, width, height;

    height = 20;
    width = 60;

    starty = ((LINES / 2) - (height / 2));
    startx = ((COLS / 2) - (width / 2));

    menu_win = create_newwin(height, width, starty, startx);

    switch (userindex)
    {
    case 1:
        choices[0] = "Read";
        choices[1] = "Write";
        choices[2] = "Edit";
        choices[3] = "Search";
        no_choices = 4;
        break;
    case 2:
        choices[0] = "Read";
        choices[1] = "Write";
        choices[2] = "Edit";
        choices[3] = "Search";
        choices[4] = "Add user";
        choices[5] = "Remove user";
        no_choices = 6;
        break;
    case 3:
        choices[0] = "Read";
        choices[1] = "Search";
        no_choices = 2;

        break;

    default:
        break;
    }

    app_items = (ITEM **)calloc(no_choices + 1, sizeof(ITEM *));
    int max_length = 0;

    for (i = 0; i < no_choices; i++)
    {

        app_items[i] = new_item(choices[i], NULL);
        if (max_length < strlen(choices[i]))
        {

            max_length = strlen(choices[i]);
        }
    }

    app_items[no_choices] = (ITEM *)NULL;

    app_menu = new_menu((ITEM **)app_items);

    int menu_width = (max_length) + 2;
    int menu_height = ((no_choices) + 2);

    int menu_window_height, menu_window_width;

    getmaxyx(menu_win, menu_window_height, menu_window_width);

    int liststarty = ((menu_window_height / 2) - (menu_height / 2));
    int liststartx = ((menu_window_width / 2) - (menu_width / 2));

    set_menu_sub(app_menu, derwin(menu_win, menu_height, menu_width, (liststarty), (liststartx)));

    set_menu_format(app_menu, no_choices, 1);

    mvwprintw(menu_win, 18, 2, "ESC to Exit");
    post_menu(app_menu);
    wrefresh(menu_win);

    while ((c = getch()) != KEY_ESC)
    {

        switch (c)
        {
        case KEY_DOWN:
            menu_driver(app_menu, REQ_DOWN_ITEM);
            wrefresh(menu_win);
            break;
        case KEY_UP:
            menu_driver(app_menu, REQ_UP_ITEM);
            refresh();
            wrefresh(menu_win);
            break;

        case 10:

            // get selected choice of user
            const char *selected_item = item_name(current_item(app_menu));
            clear();

            action(selected_item, user);
            refresh();

            break;

        default:
            break;
        }
    }

    for (int o = 0; o < no_choices; o++)
    {
        free_item(app_items[o]);
    }
    free_menu(app_menu);

    endwin();
}

int main(int argc, char *argv[])
{
    // seeding the salt
    srand((unsigned int)time(NULL));

    // login details
    char *user = malloc(100 * sizeof(char));
    char *pass = malloc(100 * sizeof(char));
    size_t len = 0;

    // ncurses aka tui

    ITEM **item;
    int c;
    MENU *menu;
    int n_choice, i;
    ITEM *current_item_no;

    // window
    WINDOW *app_win;

    int ch;
    int row, col;
    int startx, starty, width, height;

    initscr();
    cbreak();
    raw();
    keypad(stdscr, TRUE);

    height = 20;
    width = 60;

    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    printw("press esc to exit");
    refresh();

    app_win = create_newwin(height, width, starty, startx);

    while (userindex == 0)
    {
        getmaxyx(stdscr, row, col);

        refresh();
        mvwprintw(app_win, 1, ((width - strlen("LOGIN")) / 2), "LOGIN");
        refresh();

        mvwprintw(app_win, 4, 8, "\n\tUsername: ");
        wrefresh(app_win);

        wgetnstr(app_win, user, 99);

        wrefresh(app_win);

        noecho();
        wprintw(app_win, "\n\tPassword: ");
        wrefresh(app_win);
        wgetnstr(app_win, pass, 99);

        wrefresh(app_win);
        echo();

        user[strcspn(user, "\n")] = '\0';
        pass[strcspn(pass, "\n")] = '\0';
        userindex = login(user, pass);

        if (userindex <= 0)
        {

            wclear(app_win);
            box(app_win, 0, 0);
            mvwprintw(app_win, 2, 1, "\n\tLogin incorrect");
            wrefresh(app_win);
        }
        else if (userindex >= 1)
        {

            wclear(app_win);
            wrefresh(app_win);
            usermenu(userindex, user);
        }
    }

    endwin();

    return 0;
}
