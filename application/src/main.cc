#include <core/application.hh>
#include <core/events.hh>
int main() {
    Application app = Application("Test Window", 800, 600);
    app.run();
}
