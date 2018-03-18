#include "message.h"

msgbuffer::msgbuffer()
{
  // set defaults
  message_string = "";
  last_message = "";
  temp_message_string = "";
  message_repeat_counter = 1;
  message_cursor = 0;
  for (int i = 0; i < NUMMESSAGELINES * GRIDWID; ++i)
  {
      message_color_data[i] = colorpair_gray_black;
  }
}

// assumes m is not larger than 2*GRIDWID
// I wrote this a year ago and need to find
// out how it does what it does! (ack)
bool msgbuffer::addMessage(STRING m, color_pair col)
{
  message_string.append(m);
  message_string.append(" ");
  int space_iter = 0;
  for (uint i = 0; i < m.size(); ++i)
  {
    if (i + message_cursor >= NUMMESSAGELINES * GRIDWID)
      break;
    if ((i + message_cursor) % GRIDWID == 0 &&
                    i + message_cursor >= GRIDWID)
    {
      space_iter = 0;
      if (message_string[i+message_cursor-1] != ' ')
      {
        do
        {
            space_iter++;
        }while(message_string[i+message_cursor-1-space_iter] != ' ');
        for (int n = 0; n < space_iter; ++n)
            message_string.insert(message_string.begin() + i +
                                                                    message_cursor - 1 - space_iter,' ');
      }
    }
    message_color_data[i+message_cursor] = col;
  }
  message_cursor += m.size() + 1;

  for (int i = 0; i < space_iter; ++i)
  {
    message_color_data[i+message_cursor-1] = col;
  }

  message_cursor += space_iter;

  temp_message_string = message_string;
  if (message_cursor > GRIDWID * (NUMMESSAGELINES - 2))
  {
      message_string.append(" [SPACE] -> more...");
      message_cursor += 19;
      return true;
  }
  return false;
}

void msgbuffer::clearMessages()
{
    for (int i = 0; i < NUMMESSAGELINES; ++i)
                addGFXString(colorpair_gray_black,"                                                                                ",0,GRIDHGT-NUMMESSAGELINES+i);
}

void msgbuffer::deleteAllMessages()
{
  message_cursor = 0;
    message_string = "";
  for (int i = 0; i < NUMMESSAGELINES * GRIDWID; ++i)
  {
      message_color_data[i] = colorpair_gray_black;
  }
}

void msgbuffer::clearAndDeleteAllMessages()
{
    clearMessages();
    deleteAllMessages();
}

int msgbuffer::getMessageSize()
{
  return message_string.size();
}

void msgbuffer::printMessages()
{
  for (uint i = 0; i < message_string.size(); ++i)
  {
        addGFXObject(message_color_data[i],message_string[i],i % GRIDWID, (GRIDHGT - NUMMESSAGELINES) + (int)(i / GRIDWID));
  }
}

npcdamagebuffer::npcdamagebuffer()
{
    buffer.clear();
}

bool npcdamagebuffer::addMessage(STRING dam, item_t w, chtype asym, chtype tsym, bool fatal, bool explode, bool see_attacker, bool was_melee, int crit)
{
    npcdamageindicator ndi = {dam,w,asym,tsym,fatal,explode,see_attacker,was_melee,crit};
    buffer.push_back(ndi);
    if (buffer.size() > SHOWHGT)
        deleteTopMessage();
    return true;
}

// H u -> W (5555) FATAL

void npcdamagebuffer::printMessages()
{
    for (int i = 0; i < SHOWHGT; ++i)
         addGFXString(colorpair_gray_black,"                       ",SHOWWID+1,i);
    chtype asym, tsym;
    item_t w;
    for (uint i = 0; i < buffer.size(); ++i)
    {
        asym = buffer[i].asym;
        tsym = buffer[i].tsym;
        w = buffer[i].w;

        if (buffer[i].see_attacker)
            addGFXObject(asym.color,asym.ascii,SHOWWID+1,i);
        else
            addGFXObject(colorpair_gray_black,(int)'?',SHOWWID+1,i);

        if (!buffer[i].explode)
        {
            if (w == NIL_i || (buffer[i].was_melee && (((int)w >= PISTOL_10MM && (int)w <= PULSE_CANNON) || w == FUSION_CANNON)))
                addGFXObject(colorpair_gray_black,(int)'m',SHOWWID+3,i);
            else
                addGFXObject(symbol_item[(int)w].color,symbol_item[(int)w].ascii,SHOWWID+3,i);
        }
        else
            addGFXObject(colorpair_red_black, (int)'*',SHOWWID+3,i);

        addGFXObject(colorpair_white_black,26,SHOWWID+5,i);

        addGFXObject(tsym.color,tsym.ascii,SHOWWID+7,i);

        addGFXString(colorpair_gray_black,"("+buffer[i].dam+")",SHOWWID+9,i);

        if (buffer[i].crit > 1)
            addGFXString(colorpair_purple_black,"C" + sint2String(buffer[i].crit),SHOWWID+16,i);

        if (buffer[i].fatal)
            addGFXString(colorpair_red_black,"FATAL",SHOWWID+19,i);

    }
}

void npcdamagebuffer::deleteTopMessage()
{
    buffer.erase(buffer.begin());

}

void npcdamagebuffer::clearAndDeleteAllMessages()
{
    buffer.clear();
}
