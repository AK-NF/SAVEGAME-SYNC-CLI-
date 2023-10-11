#pragma once
#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>
#include "single_include/nlohmann/json.hpp"





///////// Namespaces /////////
using namespace std;
using json = nlohmann::json;



///////// Global Vars /////////
SHELLEXECUTEINFO lpExecInfo{};


json json_auth_data;
json json_profiles_data;


string str_rclone_path(".\\bin\\rclone\\rclone.exe"); // relative path to rclone
string str_zip_path(".\\bin\\7zip\\7za.exe");

string str_mega_user;
string str_mega_pass;



///////// Functions Init /////////
int Main_Menu();
void Sync_Menu();
void EditProfiles_Menu();
void Edit_Menu(int i);
void Game_Menu(int i);
void SetMegaAuth();


///////// Functions /////////


std::string exec_cmd(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}// Thanks to Stackoverflow


void DeleteZipFolder(string filename) {
    remove((filename + ".zip").c_str()); 
}

void CreateZipArchive(string path_to_savefolder, string filename) {
    system((str_zip_path + " a -y " + filename + ".zip " + '"' + path_to_savefolder + '"' + "\\*").c_str());

    //  Example:
    // .\bin\7zr.exe a -y test.zip .\test\test\*   -> creates test.zip from contents of .\test\test\
    // 7zr.exe a (path+name or just name) (path to dir * as a wildcard for all files)
}

void ExtractZipArchive(string path_to_savefolder, string filename) {
    system((str_zip_path + " x -y " + filename + ".zip " + "-o" + '"' + path_to_savefolder + '"' + "\\").c_str());

    //  Example:
    // .\bin\7zr.exe x -y test.zip -o.\test2   -> extracts zip contents to test2 folder
    // 7zr.exe x -y (path+name or just name) (path to dir * as a wildcard for all files)
}

string ParseJSONDataToString(string s) {
    replace(s.begin(), s.end(), '"', '\0'); // replace all 'x' to 'y'
    return s;
} 

void ParseFilesToJSON() {

    ifstream auth("auth.json");
    ifstream profiles("profiles.json");

    if (auth.fail()) {
        ofstream auth_out("auth.json");
        auth_out << json({{"email",""},{"password",""}});
        json_auth_data = json({ {"email",""},{"password",""} });
    }
    else {
        json_auth_data = json::parse(auth);
    }

    if (profiles.fail()) {
        ofstream profiles_out("profiles.json");
        profiles_out << json({ {"profile",json(0,"")} });
        json_profiles_data = json({ {"profile",json(0,"")} });
    }
    else {
        json_profiles_data = json::parse(profiles);
    }

    str_mega_user = json_auth_data["email"];
    str_mega_pass = json_auth_data["password"];
}


void MegaLogin() {
    // rclone config create mega2 mega pass urpassword123  user yourmail@example.com
    system((str_rclone_path + " " + "config create MegaNz mega " + "pass " + str_mega_pass + " user " + str_mega_user).c_str());
}

void MegaDownload(string filename){
    // rclone copy MegaNz:destpath path
    system((str_rclone_path + " " + "copy" + " MegaNz:" + filename + ".zip" + " .\ -P --no-check-dest").c_str());
    Sleep(1000);
}

void MegaUpload(string filename) {
    // rclone copy filepath dest:destpath
    system((str_rclone_path + " " + "copy" + " " + filename + ".zip" + " MegaNz:" + ". -P --no-check-dest").c_str());
    Sleep(1000);
}

void Initializing() {
    try {
        ParseFilesToJSON();
        MegaLogin();
        if (str_mega_user.length() <= string("").length() || str_mega_pass.length() <= string("").length()) {
            SetMegaAuth();
        }
    }
    catch (exception& e) { cerr << "\nError Init: " << e.what() << "\n\n"; system("pause"); }
}

void SetMegaAuth() {
    string Username;
    string Password;

    system("cls");
    cout << "######## Savegame Sync - Set your login details (MegaNz) ########\n\n";
    cout << "Username (E-Mail): "; getline(cin, Username); cout << "\n";
    cout << "Password: "; getline(cin, Password); cout << "\n";

    json_auth_data["email"] = Username;
    json_auth_data["password"] = Password;

    try { ofstream fileoutputjson("auth.json"); fileoutputjson << json_auth_data; }
    catch (exception& e) { cerr << "\n Error Setting Auth details: " << e.what() << "\n\n"; system("pause"); }

    MegaLogin();

    Main_Menu();
}

void CreateNewProfile(string Entry_Name, string Entry_KeyID, string Entry_Path) {
    json_profiles_data["profile"].push_back({ {"Name", Entry_Name},{"KeyID", Entry_KeyID},{"Path", Entry_Path} });

    try { ofstream fileoutputjson("profiles.json"); fileoutputjson << json_profiles_data; }
    catch (exception& e) { cerr << "\n Error Creating Profile: " << e.what() << "\n\n"; system("pause"); }
}

void ProfileCreation_Menu() {
    string Entry_Name;
    string Entry_KeyID;
    string Entry_Path;

    system("cls");


    cout << "######## Savegame Sync - Creating new profile entry ########\n";
    cout << "\nEnter Name for Profile: "; getline(cin, Entry_Name); cout << "\n";
    cout << "\nEnter Identifier for Profile: "; getline(cin, Entry_KeyID); cout << "\n";
    cout << "\nEnter Path for Profile: "; getline(cin, Entry_Path); cout << "\n";

    CreateNewProfile(Entry_Name, Entry_KeyID, Entry_Path);

    Main_Menu();
}

void ProfileEdit(int i) {
    string Entry_Name;
    string Entry_KeyID;
    string Entry_Path;

    system("cls");
    cout << "######## Savegame Sync - " << string(json_profiles_data["profile"][i]["Name"]) << " - Edit ########\n";
    for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " - Edit ########").length(); j++) {
        cout << "-";
    }

    cout << "\nCurrent Settings:\n";
    cout << "Name: " << string(json_profiles_data["profile"][i]["Name"]) << "\n";
    cout << "KeyID: " << string(json_profiles_data["profile"][i]["KeyID"]) << "\n";
    cout << "Path: " << string(json_profiles_data["profile"][i]["Path"]) << "\n";
   

        for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " - Edit ########").length(); j++) {
            cout << "-"; 
        }

    cout << "\nEnter Name for Profile: "; getline(cin, Entry_Name); cout << "\n";
    cout << "Enter Identifier for Profile: "; getline(cin, Entry_KeyID); cout << "\n";
    cout << "Enter Path for Profile: "; getline(cin, Entry_Path); cout << "\n\n";


    json_profiles_data["profile"][i]["Name"] = Entry_Name;
    json_profiles_data["profile"][i]["KeyID"] = Entry_KeyID;
    json_profiles_data["profile"][i]["Path"] = Entry_Path;


    try { ofstream fileoutputjson("profiles.json"); fileoutputjson << json_profiles_data; }
    catch (exception& e) { cerr << "\n Error Editing Profile: " << e.what() << "\n\n"; system("pause"); }

    Edit_Menu(i);
}

void ProfileDelete(int i){
    json_profiles_data["profile"].erase(json_profiles_data["profile"].begin() + i);

    try { ofstream fileoutputjson("profiles.json"); fileoutputjson << json_profiles_data; }
    catch (exception& e) { cerr << "\n Error Deleting Profile: " << e.what() << "\n\n"; system("pause"); }

}

void Edit_Menu(int i) {
    string selection;

    system("cls");
    cout << "######## Savegame Sync - " << string(json_profiles_data["profile"][i]["Name"]) << " - Edit/Delete ########\n";
    cout << "1. Edit\n";
    cout << "2. Delete\n";
    for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " - Edit/Delete ########").length(); j++) {
        cout << "-";
    }
    cout << "\n0. go back \n";

    for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " - Edit/Delete ########").length(); j++) {
        cout << "#";
    }

    cout << "\n\nEnter: "; getline(cin, selection);
    
    switch (stoi(selection))
    {
    case 1:

        try {
            ProfileEdit(i);
        }
        catch (exception& e) { cerr << "\nError Edit Profile: " << e.what() << "\n\n"; system("pause"); }

        cout << "\n";
        Game_Menu(i);

    case 2:

        try {
            ProfileDelete(i);
        }
        catch (exception& e) { cerr << "\nError Delete Profile: " << e.what() << "\n\n"; system("pause"); }

        cout << "\n";
        Main_Menu();

    case 0:
        EditProfiles_Menu();

    default:
        Edit_Menu(i);
    }
}

void EditProfiles_Menu() {
    string selection;

    system("cls");
    cout << "######## Savegame Sync - Edit or Delete profiles ########\n";
    for (int i = 0; i < json_profiles_data["profile"].size(); i++) {
        cout << i + 1 << ". " << ParseJSONDataToString(string(json_profiles_data["profile"][i]["Name"])) << endl;
    }
    for (int i = 0; i < string("######## Savegame Sync - Edit or Delete profiles ########").length(); i++) {
        cout << "-";
    }
    cout << "\n0. go back \n";

    for (int i = 0; i < string("######## Savegame Sync - Edit or Delete profiles ########").length(); i++) {
        cout << "#";
    }

    cout << "\n\nEnter: "; getline(cin, selection);

    if (selection == "0") {
        Main_Menu();
    }
    else {
        Edit_Menu(stoi(selection) - 1);
    }
}


void Game_Menu(int i) {
    string selection;

    system("cls");
    cout << "######## Savegame Sync - " << string(json_profiles_data["profile"][i]["Name"]) << " ########\n";
    cout << "1. Upload\n";
    cout << "2. Download\n";
    for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " ########").length(); j++) {
        cout << "-";
    }
    cout << "\n0. go back \n";

    for (int j = 0; j < string("######## Savegame Sync - " + string(json_profiles_data["profile"][i]["Name"]) + " ########").length(); j++) {
        cout << "#";
    }

    cout << "\n\nEnter: "; getline(cin, selection);


    switch (stoi(selection))
    {
    case 1:

        try {
            CreateZipArchive(string(json_profiles_data["profile"][i]["Path"]),
                string(json_profiles_data["profile"][i]["KeyID"]));

            MegaUpload(string(json_profiles_data["profile"][i]["KeyID"]));

            DeleteZipFolder(string(json_profiles_data["profile"][i]["KeyID"]));
        }
        catch (exception& e) { cerr << "\nError Upload: " << e.what() << "\n\n"; system("pause"); }

        cout << "\n";
        Game_Menu(i);

    case 2:
        
        try {
            MegaDownload(string(json_profiles_data["profile"][i]["KeyID"]));

            ExtractZipArchive(string(json_profiles_data["profile"][i]["Path"]),
                string(json_profiles_data["profile"][i]["KeyID"]));

            DeleteZipFolder(string(json_profiles_data["profile"][i]["KeyID"]));
        }
        catch (exception& e) { cerr << "\nError Download: " << e.what() << "\n\n"; system("pause");}

       
        cout << "\n";
        Game_Menu(i);

    case 0:
        Sync_Menu();

    default:
        Game_Menu(i);
    }

 

}

void Sync_Menu() {
    string selection;

    system("cls");
    cout << "######## Savegame Sync ########\n";
    for (int i = 0; i < json_profiles_data["profile"].size(); i++) {
        cout << i + 1 << ". " << ParseJSONDataToString(string(json_profiles_data["profile"][i]["Name"])) << endl;
    }
    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "-";
    }
    cout << "\n0. go back \n";

    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "#";
    }

    cout << "\n\nEnter: "; getline(cin, selection);


    if (selection == "0") {
        Main_Menu();
    }
    else {
        Game_Menu(stoi(selection) - 1);
    }
}

int Main_Menu() {
    string selection;

    system("cls");
    cout << "######## Savegame Sync ########\n";
    cout << "1. Sync Games\n";
    cout << "2. Create New Profile\n";
    cout << "3. Edit/Delete Profile\n";
    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "-";
    }
    cout << "\n4. Set Auth Data (MegaNz)\n";

    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "-";
    }
    cout << "\n5. Reload Profiles\n";
    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "-";
    }
    cout << "\n0. Exit \n";

    for (int i = 0; i < string("######## Savegame Sync ########").length(); i++) {
        cout << "#";
    }
 
    cout << "\n\nEnter: "; getline(cin, selection);

    if (selection == "1") Sync_Menu();
    else if(selection == "2") ProfileCreation_Menu();
    else if (selection == "3") EditProfiles_Menu();
    else if (selection == "4") SetMegaAuth();
    else if (selection == "5"){
        try { system("cls"); cout << "Reloading Profiles.json" << endl; ParseFilesToJSON();  Sleep(2000); }
        catch (exception& e) { system("cls"); cerr << "\nError Reload Profiles: " << e.what() << endl; system("pause"); }
        Main_Menu();
    }
    else if (selection == "0") return 0;
    else Main_Menu();

}