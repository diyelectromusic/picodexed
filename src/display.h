#ifndef _display_h
#define _display_h
#include <string>

class CDisplay
{
public:
    CDisplay (void);
    ~CDisplay (void);

    bool Init (void);
    void Logo (void);
    void Print (std::string sString);
    void Update (void);

private:
};

#endif
