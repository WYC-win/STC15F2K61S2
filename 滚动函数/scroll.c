void scroll_display()
{
    unsigned char i;
    code unsigned char msg[12] = {
        0xFF,0xFF,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0xFF,0xFF
    };
    static unsigned char index = 0;

    if(scroll_ms >= 30)
    {
        scroll_ms = 0;
        index++;
        if(index > 4)
            index = 0;
    }

    for(i=0;i<8;i++)
    {
        digitaltube(i+1, msg[index+i]);
    }
}