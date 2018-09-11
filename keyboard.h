#ifndef KEYBOARD_H
#define KEYBOARD_H
class Keyboard
{
 public:
 static void
 keyDown(unsigned char key, int x, int y)
 {
 _keyboard[key]=true;
 }

 static void
 keyUp(unsigned char key, int x, int y)
 {
 _keyboard[key]=false;

 }

 static bool
 isPressed(char c) { return _keyboard[c]; }

 private:
 static bool _keyboard[256];
};
bool Keyboard::_keyboard[256];
#endif

