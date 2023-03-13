#include <string>

int main()
{

    typedef struct
    {
        char buf[128];
        struct Logbuf *next;
    } Logbuf;
    static Logbuf iringbuf[30];
    static int logbuf_index = 0;

    if (logbuf_index < 30)
    {
        strcpy(iringbuf[logbuf_index].buf, _this->logbuf);
        iringbuf[logbuf_index].next = &iringbuf[logbuf_index + 1];
        logbuf_index++;
    }
    else
    {
        strcpy(iringbuf[0].buf, _this->logbuf);
        iringbuf[29].next = &iringbuf[0];
        iringbuf[0].next = NULL; 
    }
    if ()
}
