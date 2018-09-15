#ifndef MOUSE_H
#define MOUSE_H
class Mouse
{
public:
    static bool left_isPressed()
    {
        return _leftButton;
    }
    static bool right_isPressed()
    {
        return _rightButton;
    }
    static void
    updatePosition(int x, int y)
    {
        _currentX = x;
        _currentY = y;
        _mouseLogic();
    }
    static void
    updateButton(int button, int state, int x, int y)
    {
        switch (button)
        {
        case SDL_BUTTON_LEFT:
            _leftButton = (state== SDL_MOUSEBUTTONDOWN);
            break;
        case SDL_BUTTON_RIGHT:
            _rightButton = (state== SDL_MOUSEBUTTONDOWN);
            break;
        }
        updatePosition(x, y);
    }
    static int getLeftDiffX()
    {
        return (_currentX-_lastX_left);
    }
    static int getLeftDiffY()
    {
        return -(_currentY - _lastY_left);
    }
    static int getMiddleDiffX()
    {
        return (_currentX-_lastX_middle);
    }
    static int getMiddleDiffY()
    {
        return -(_currentY-_lastY_middle);
    }
    static int getRightDiffX()
    {
        return (_currentX-_lastX_right);
    }
    static int getRightDiffY()
    {
        return -(_currentY-_lastY_right);
    }
    static int getX()
    {
        return _currentX;
    }
    static int getY()
    {
        return _currentY;
    }

private:
    static int _currentX, _currentY, _lastX_left, _lastY_left,
           _lastX_middle, _lastY_middle, _lastX_right, _lastY_right;
    static bool _leftButton, _middleButton, _rightButton;

    static void
    _mouseLogic()
    {
        if (!_leftButton)
        {
            _lastX_left = _currentX;
            _lastY_left = _currentY;
        }
        if (!_middleButton)
        {
            _lastX_middle = _currentX;
            _lastY_middle = _currentY;
        }
        if (!_rightButton)
        {
            _lastX_right = _currentX;
            _lastY_right = _currentY;
        }
    }
};
int Mouse::_currentX = 0;
int Mouse::_currentY = 0;
int Mouse::_lastX_left = 0;
int Mouse::_lastY_left = 0;
int Mouse::_lastX_middle = 0;
int Mouse::_lastY_middle = 0;
int Mouse::_lastX_right = 0;
int Mouse::_lastY_right = 0;
bool Mouse::_leftButton = false;
bool Mouse::_middleButton = false;
bool Mouse::_rightButton = false;
#endif
