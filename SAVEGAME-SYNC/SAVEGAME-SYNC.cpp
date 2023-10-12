#include "header.h"
using namespace std;

// Currently only win32 with win32 binarys

// CLI v0.3 ROADMAP:
// (100%) -> Profile Create in CLI 
// (100%) -> Profile Edit in CLI
// (100%) -> Profile Delete in CLI
// (100%) -> Improve Menu UI ->
// (100%) -> Create Auth.json and Profiles.json when not there
// (0%) -> Logs.log
// (0%) -> Better editing of existing profiles (when no input is given the same value is used as already is)
//
// CLI v0.4 ROADMAP:
// (0%) - More Cloud Provider (multiple auth profiles)
//

int main()
{
    Initializing();

    Main_Menu();

    return 0;
}
