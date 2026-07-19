#include "../core/SystemManager.h"
#include "gui.h"

int main() {
    SystemManager* sys = createSystemManager();
    
    initGUI();
    runGUI(sys);
    closeGUI();
    
    return 0;
}
