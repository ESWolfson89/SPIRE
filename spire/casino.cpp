#include "casino.h"

machine::machine()
{
}

uint machine::getMachineCredits()
{
    return 1;
}

point machine::at()
{
    return loc;
}

uint64 machine::getCredits()
{
    return credits;
}

void machine::setCredits(uint64 c)
{
    credits = c;
}

void machine::delayFunc(Uint32 t)
{
    SDL_Delay(t);
}

void machine::updateFunc()
{
    updateScreen();
}

void machine::initMachineColorAttributes()
{
    machine_color.fg = getPresetColor(ROLL(11));

    arrow_color.fg = getPresetColor(ROLL(11));

    title_color.fg = getPresetColor(ROLL(11));

    number_color.fg = getPresetColor(ROLL(11));

    text_color.fg = getPresetColor(ROLL(11));

    machine_color.bg = title_color.bg = text_color.bg = arrow_color.bg = number_color.bg = color_black;
}

void machine::gotoNextState()
{
    if (machine_type == SLOT_MACHINE)
    {
        if ((uint)current_state == MSTATE_SLOTHACK)
            current_state = MSTATE_SLOTSPINMAX;
        else
            current_state = (machine_state)(((uint)(current_state) + 1));
    }

    if (machine_type == DIAMONDS_MACHINE)
    {
        if ((uint)current_state == MSTATE_DIAMONDHACK)
            current_state = MSTATE_DIAMONDENDGAME;
        else
            current_state = (machine_state)(((uint)(current_state) + 1));
    }
}

void machine::initState()
{
    if (machine_type == SLOT_MACHINE)
        current_state = (machine_state)ROLL(7);
    if (machine_type == DIAMONDS_MACHINE)
        current_state = (machine_state)(ROLL(5) + 8);
}

void machine::setState(machine_state ms)
{
    current_state = ms;
}

terrain_t machine::getMachineType()
{
    return machine_type;
}

machine_state machine::getState()
{
    return current_state;
}

slot::slot()
{

}

slot::slot(point p, int dl, uint c)
{
    credits = c;
    danger_level = dl;
    loc = p;
    machine_type = SLOT_MACHINE;
}

void slot::drawMachine()
{
    drawSlotFrame();
    drawSlotTileKey();
    drawSlotReels();
    drawSlotText();
    drawAllArrows(true);
}

void slot::drawSlotStateIndicator(STRING txt, machine_state s)
{
    color_pair select_color = {text_color.fg,color_verydarkgray};

    point loc = point(2+(((int)((int)s/4))*11)+SHOWWID,16+(int)(s % 4));

    if (s != current_state)
        addGFXString(text_color,txt,loc.x(),loc.y());
    else
        addGFXString(select_color,txt,loc.x(),loc.y());
}

void slot::drawSlotText()
{
    addGFXString(title_color,"SLOTS",10+SHOWWID,1);

    addGFXString(text_color,"T      ",16+SHOWWID,10);
    addGFXString(text_color,"L      ",16+SHOWWID,11);
    addGFXString(text_color,"B      ",16+SHOWWID,12);

    drawSlotStateIndicator(" MAX SPIN ",MSTATE_SLOTSPINMAX);
    drawSlotStateIndicator("   SPIN   ",MSTATE_SLOTSPIN);
    drawSlotStateIndicator("  INSERT  ",MSTATE_SLOTINSERT);
    drawSlotStateIndicator(" CASH OUT ",MSTATE_SLOTCASHOUT);
    drawSlotStateIndicator(" SPIN ONE ",MSTATE_SLOTSPINONE);
    drawSlotStateIndicator(" NEXT BET ",MSTATE_SLOTBETAMOUNT);
    drawSlotStateIndicator("NEXT LINES",MSTATE_SLOTBETLINES);
    drawSlotStateIndicator("  UNUSED  ",MSTATE_SLOTHACK);

    addGFXString(text_color,"CREDIT=",2+SHOWWID,14);
    addGFXString(number_color,"              ",9+SHOWWID,14);
    addGFXString(number_color,int2String((uint64)credits),9+SHOWWID,14);

    for (uint i = 0; i < 3; ++i)
        addGFXObject(text_color,26,17+SHOWWID,10+i);

    addGFXString(number_color,int2String((uint64)(current_bet)),18+SHOWWID,12);
    addGFXString(number_color,int2String((uint64)(current_lines)),18+SHOWWID,11);
    addGFXString(number_color,int2String((uint64)(current_total_bet)),18+SHOWWID,10);

    color_pair dollar_multiplier_indicator_color = {color_black,machine_color.fg};

    addGFXString(dollar_multiplier_indicator_color,"x",11+SHOWWID,9);
    addGFXString(dollar_multiplier_indicator_color,int2String((uint64)dollar_match_multiplier),13+SHOWWID,9);

    STRING last_won_indicator1 = int2String((uint64)last_winnings);
    STRING last_won_indicator2 = "";

    if (last_won_indicator1.size() > 7)
    {
        last_won_indicator2 = STRING(last_won_indicator1.begin()+7,last_won_indicator1.end());
        last_won_indicator1 = STRING(last_won_indicator1.begin(),last_won_indicator1.begin()+7);
    }

    if (last_winnings == 0)
    {
        addGFXString(text_color,"SPIN   ",2+SHOWWID,10);
        addGFXString(text_color,"   2   ",2+SHOWWID,11);
        addGFXString(text_color,"   WIN!",2+SHOWWID,12);
    }
    else
    {
        if (last_line_win_tile <= 10)
            addGFXString(text_color,"YOU WIN",2+SHOWWID,10);
        if (last_line_win_tile == 11)
            addGFXString(text_color,"JACKPOT",2+SHOWWID,10);

        addGFXString(number_color,"       ",2+SHOWWID,11);
        addGFXString(number_color,"       ",2+SHOWWID,12);

        addGFXString(number_color,last_won_indicator1,2+SHOWWID,11);
        addGFXString(number_color,last_won_indicator2,2+SHOWWID,12);
    }
}

void slot::drawSlotReels()
{
    slot_tile t;

    point p;

    for (uint i = 0; i < NUM_SLOT_REELS; ++i)
        for (uint j = 0; j < NUM_VISIBLE_LINES; ++j)
        {
            t = reel[reel_state[i][j]];
            p = point((i+1)*2 - 1,j);
            drawSlotTile(point(p.x()+SHOWWID+9,p.y()+10),t);
        }

    // draw the dollar sign additionally
    addGFXObject(dollar_colors[current_dollar_color],'$',12+SHOWWID,8);
}

void slot::drawArrow(uint n, bool lit)
{
    color_pair colp = (lit == true ? arrow_color : colorpair_verydarkgray_black);
    addGFXObject(colp,31,10+SHOWWID+(2*n),9);
    addGFXObject(colp,30,10+SHOWWID+(2*n),13);
}

void slot::flickerDollarSignColor()
{
    current_dollar_color = (current_dollar_color + 1) % 10;
}

void slot::drawSlotTile(point p, slot_tile t)
{
    chtype ct = slot_tile_symbol[(int)t];
    addGFXObject(ct.color,ct.ascii,p.x(),p.y());
}

void slot::drawSlotFrame()
{
    int current_ascii;

    // print machine border one block at a time
    for (int x = 0; x < RIGHTSIDEWID; ++x)
        for (int y = 0; y < SHOWHGT; ++y)
        {
            current_ascii = slot_backdrop_array[y][x];

            addGFXObject(machine_color,current_ascii,x+SHOWWID,y);
        }
}

void slot::drawAllArrows(bool lit)
{
    for (uint i = 0; i < 3; ++i)
        drawArrow(i,lit);
}

void slot::drawSlotTileKey()
{
    point p;

    uint ith_payout;

    STRING payout_string;

    STRING payout_string_modifier;

    for (uint i = 11; i >= 2; --i)
    {
        for (uint j = 1; j < 4; ++j)
        {
            p.set(j+1+SHOWWID,14-i);

            if (i <= 5)
                p = addPoints(p,point(11,-6));

            drawSlotTile(p,(slot_tile)i);
        }

        p.setx(p.x() + 1);

        payout_string = "x";

        payout_string_modifier = "";

        ith_payout = line_payouts[i-2];

        if (ith_payout % 1000 == 0)
        {
            payout_string_modifier = "K";
            ith_payout /= 1000;
        }

        payout_string += int2String((uint64)ith_payout);
        payout_string += payout_string_modifier;

        addGFXString(text_color,payout_string,p.x(),p.y());
    }

    p.set(14+SHOWWID,7);

    for (uint i = 0; i < 2; ++i)
    {
        drawSlotTile(point(p.x(),p.y() + i),(slot_tile)(11-i));

        payout_string = "x" + int2String((uint64)tile_multipliers[1-i]) + " each";

        addGFXString(text_color,payout_string,p.x() + 1,p.y() + i);
    }
}

void slot::initSlotDisplayAttributes()
{
    chtype ct;

    VECTOR<uint> pcolor_list;

    uint index = 0;

    for (uint i = 0; i <= 11; ++i)
    {
        pcolor_list.push_back(i);
    }

    for (uint i = 0; i < NUM_TOTAL_SLOT_TILES; ++i)
    {
        index = ROLL(pcolor_list.size()-1);
        ct.ascii = getSlotAscii(i);
        ct.color.fg = getPresetColor(pcolor_list[index]);
        ct.color.bg = color_black;
        slot_tile_symbol[i] = ct;
        pcolor_list.erase(pcolor_list.begin() + index);
    }

    initMachineColorAttributes();

    for (uint i = 0; i < 10; ++i)
        dollar_colors[i] = slot_tile_symbol[i+2].color;
}

void slot::initSlots()
{
    uint tval = 1;

    const uint num_slot_reel_tiles = BASE_NUM_SLOT_REEL_TILES +
                                     (NUM_SLOT_REEL_TILES_OFFSET *
                                      (danger_level - 1));

    initSlotDisplayAttributes();

    current_dollar_color = ROLL(9);

    for (uint i = 1; i <= num_slot_reel_tiles; ++i)
    {
        reel.push_back((slot_tile)(12 - tval));
        if (i == (uint)(((double)tval * ((double)tval + 1.0)/2.0)) && i <= BASE_NUM_SLOT_REEL_TILES)
            tval++;
    }

    for (uint i = 0; i < 3; ++i)
    {
        reel_state[i][0] = ROLL(num_slot_reel_tiles - 1);
        reel_state[i][1] = (reel_state[i][0] + 1) % num_slot_reel_tiles;
        reel_state[i][2] = (reel_state[i][1] + 1) % num_slot_reel_tiles;
    }

    RANDOM_SHUFFLE(reel.begin(),reel.end());
    RANDOM_SHUFFLE(reel.begin(),reel.end());

    uint multiplier = 1;

    if (danger_level > 1)
        multiplier = ROLL(slot_base_danger_multiplier[danger_level - 1] -
                          slot_base_danger_multiplier[danger_level - 2] - 1) +
                     slot_base_danger_multiplier[danger_level - 2] + 1;

    for (uint i = 0; i < 10; ++i)
        line_payouts[i] = slot_base_payout[i] * multiplier;

    multiplier = (uint)(multiplier / 10) + 1;

    for (uint i = 0; i < 5; ++i)
        bet_values[i] = slot_base_bet[i] * multiplier;

    setBet(ROLL(4),ROLL(4));

    initState();

    dollar_match_multiplier = ROLL(base_dollar_match_multiplier[danger_level - 1]) + 2;

    multiplier = ROLL((int)(danger_level/6));

    tile_multipliers[0] = 2 + multiplier;
    tile_multipliers[1] = 3 + multiplier;

    last_winnings = 0;

    last_line_win_tile = -1;
}

void slot::setBet(uint b, uint l)
{
    bet_iter = b <= 4 ? b : 0;
    lines_iter = l <= 4 ? l : 0;

    current_bet = bet_values[bet_iter];
    current_lines = slot_base_lines[lines_iter];

    current_total_bet = current_bet * current_lines;
}

uint64 slot::useMachine(uint64 inp_credits)
{
    uint64 out_cred = 0;

    switch(current_state)
    {
    case(MSTATE_SLOTSPIN):
        spinReel();
        break;
    case(MSTATE_SLOTSPINMAX):
        setBet(4,4);
        drawSlotText();
        spinReel();
        break;
    case(MSTATE_SLOTSPINONE):
        setBet(0,0);
        drawSlotText();
        spinReel();
        break;
    case(MSTATE_SLOTBETAMOUNT):
        setBet(bet_iter + 1,lines_iter);
        drawSlotText();
        break;
    case(MSTATE_SLOTBETLINES):
        setBet(bet_iter,lines_iter + 1);
        drawSlotText();
        break;
    case(MSTATE_SLOTCASHOUT):
        out_cred = credits;
        credits = 0;
        break;
    case(MSTATE_SLOTINSERT):
        credits += inp_credits;
        break;
    default:
        break;
    }

    return out_cred;
}

void slot::spinReel()
{
    if (current_total_bet > credits)
        return;

    credits -= current_total_bet;

    drawAllArrows(false);
    drawSlotText();
    // reel_spin_factor determines how many units will
    // each reel will turn in succession
    // (1st randInt is first reel, 2nd randInt
    // is second reel, 3rd randInt is third reel)
    int reel_spin_factor[3] = {(int)ROLL(55)+15,
                               (int)ROLL(55)+15,
                               (int)ROLL(55)+15
                              };

    // n indicates which reel is being spun
    uint n = 0;
    // indicates speed of reel (turn/millisec)
    // for each reel spinning...

    int new_state;

    while(n <= 2)
    {
        // do for reel_speed milliseconds...
        // ONLY IF player is using
        delayFunc(5);

        // ...flicker dollar sign,
        flickerDollarSignColor();

        // (do col*row times)
        for (uint i = n; i < NUM_SLOT_REELS; ++i)
            for (uint j = 0; j < NUM_VISIBLE_LINES; ++j)
            {
                new_state = reel_state[i][j];
                if (new_state == 0)
                    new_state = reel.size() - 1;
                else
                    new_state--;
                reel_state[i][j] = new_state;
            }
        // decrement counter (until 0)
        reel_spin_factor[n]--;
        // redisplay reels only
        drawSlotReels();
        // gfx update via sdl function call (from graphics.cpp/h)
        updateFunc();
        // when counter == 0 (one of three counters)
        if (reel_spin_factor[n] == 0)
        {
            drawArrow(n,true);
            n++; // increment reel
        }
    }

    calculateSpinWinnings();
}

void slot::setLineWinnings(slot_tile st1, slot_tile st2, slot_tile st3)
{
    if (st1 == BLANK || st2 == BLANK || st3 == BLANK)
        return;

    if (!(st1 == st2 && st2 == st3))
        return;

    uint temp = line_payouts[(int)st1 - 2];

    if (temp > last_winnings)
    {
        last_line_win_tile = (int)st1;
        last_winnings = temp;
    }
}

uint slot::getTop2MatchMultiplier()
{
    uint ret_val = 1;

    if (last_line_win_tile > -1)
        if ((slot_tile)last_line_win_tile == SS_9 ||
                (slot_tile)last_line_win_tile == SS_10)
            return 0;

    for (uint i = 0; i < 3; ++i)
        for (uint j = 0; j < 3; ++j)
        {
            if (reel[reel_state[i][j]] == SS_9)
                ret_val *= tile_multipliers[0];
            if (reel[reel_state[i][j]] == SS_10)
                ret_val *= tile_multipliers[1];
        }

    return ret_val - 1;
}

uint slot::getDollarMatchMultiplier(slot_tile st)
{
    if (isColor(dollar_colors[current_dollar_color].fg,dollar_colors[(int)st - 2].fg))
        return dollar_match_multiplier;

    return 1;
}

void slot::calculateSpinWinnings()
{
    last_line_win_tile = -1;

    last_winnings = 0;

    uint top2winnings = 0;

    slot_tile t[3][3];

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            t[i][j] = reel[reel_state[i][j]];

    if (current_lines >= 9)
    {
        setLineWinnings(t[0][0],t[1][2],t[2][0]);
        setLineWinnings(t[0][2],t[1][0],t[2][2]);
    }
    if (current_lines >= 7)
    {
        setLineWinnings(t[0][0],t[1][1],t[2][0]);
        setLineWinnings(t[0][2],t[1][1],t[2][2]);
    }
    if (current_lines >= 5)
    {
        setLineWinnings(t[0][2],t[1][1],t[2][0]);
        setLineWinnings(t[0][0],t[1][1],t[2][2]);
    }
    if (current_lines >= 3)
    {
        setLineWinnings(t[0][2],t[1][2],t[2][2]);
        setLineWinnings(t[0][0],t[1][0],t[2][0]);
    }
    if (current_lines >= 1)
    {
        setLineWinnings(t[0][1],t[1][1],t[2][1]);
    }

    if (last_line_win_tile > -1)
        last_winnings *= getDollarMatchMultiplier((slot_tile)last_line_win_tile);

    top2winnings = getTop2MatchMultiplier();

    if (last_winnings == 0)
    {
        if (top2winnings > 0)
            last_winnings = 1 + top2winnings;
    }
    else
        last_winnings *= (1 + top2winnings);

    last_winnings *= current_bet;

    credits += last_winnings;

    drawSlotText();
}

uint slot::getBetVal(uint i)
{
    return bet_values[i];
}

uint slot::getLineVal(uint i)
{
    return slot_base_lines[i];
}

int getSlotAscii(uint i)
{
    return slot_tile_char[i];
}

// diamonds machine class implementation

diamond::diamond()
{

}

diamond::diamond(point p, int dl, uint c)
{
    credits = c;
    danger_level = dl;
    loc = p;
    diamond_columns = diamond_column_base_quantity[dl-1];
    diamond_rows = diamond_columns + 1;
    machine_type = DIAMONDS_MACHINE;
}

void diamond::initDiamonds()
{
    diamond_lights.resize(diamond_columns, VECTOR<bool>( diamond_rows, false));
    num_diamond_lights.resize(diamond_columns, 0);
    // reel.resize((diamond_columns * 5)+5, 0);
    diamond_colors.resize(diamond_columns, colorpair_verydarkgray_black);
    payouts.resize(diamond_rows, 1);

    // reel[reel.size()-1] = DIAMOND_REEL_BONUS_TILE_VALUE;

    uint secondary_multiplier = ROLL(danger_level) + 1;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < (int)diamond_columns; ++j)
        {
            reel.push_back((uint)j);
        }
        reel.push_back((uint)DIAMOND_REEL_BONUS_TILE_VALUE);
    }

    for (uint i = 0; i < payouts.size(); ++i)
    {
        payouts[i] = diamond_base_payout_mod10[i] * diamond_base_danger_multiplier[danger_level-1];
        if (i >= 2 && i <= 5)
            payouts[i] *= secondary_multiplier;
    }
    for (int i = 0; i < 3; ++i)
        bet_values[i] = diamond_base_bet[i] * (uint)((danger_level+1)/2);

    current_reel_position = 0;

    is_playing = false;
    bonus_round = false;

    current_winnings = 0;
    spin_rounds_left = max_spin_rounds = diamond_rows;

    bet_iter = ROLL(2);

    setBet(bet_iter);

    initState();

    initDiamondDisplayAttributes();
}

void diamond::drawDiamondLights()
{
    for (uint i = 0; i < diamond_columns; ++i)
    {
        for (uint j = 0; j < diamond_rows; ++j)
        {
            if (j == 0 && bonus_round)
                addGFXObject(getInvertedColorpair(diamond_colors[i]),(int)'B',SHOWWID+15+i, 11-j);
            else
            {
                if (!diamond_lights[i][j])
                    addGFXObject(diamond_colors[i],4,SHOWWID+15+i, 11-j);
                else
                    addGFXObject(getInvertedColorpair(diamond_colors[i]),4,SHOWWID+15+i, 11-j);
            }
        }
    }
}

void diamond::initDiamondDisplayAttributes()
{
    initMachineColorAttributes();

    VECTOR<uint> pcolor_list;

    uint index = 0;

    for (uint i = 0; i <= 11; ++i)
    {
        pcolor_list.push_back(i);
    }

    for (uint i = 0; i < diamond_colors.size(); ++i)
    {
        index = ROLL(pcolor_list.size() - 1);
        diamond_colors[i].bg = color_black;
        diamond_colors[i].fg = getPresetColor(pcolor_list[index]);
        pcolor_list.erase(pcolor_list.begin() + index);
    }
}

void diamond::drawDiamondFrame()
{
    int current_ascii;

    // print machine border one block at a time
    for (int x = 0; x < RIGHTSIDEWID; ++x)
        for (int y = 0; y < SHOWHGT; ++y)
        {
            current_ascii = diamonds_backdrop_array[y][x];

            addGFXObject(machine_color,current_ascii,x+SHOWWID,y);
        }

    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 9; ++y)
            addGFXObject(getInvertedColorpair(colorpair_verydarkgray_black),4,SHOWWID+15+x, 11-y);

    switch(diamond_rows)
    {
    case(9):
        addGFXString(text_color,"       ",SHOWWID+7,3);
        addGFXObject(machine_color,179,SHOWWID+6,3);
        addGFXObject(machine_color,179,SHOWWID+14,3);
    case(8):
        addGFXString(text_color,"       ",SHOWWID+7,4);
        addGFXObject(machine_color,179,SHOWWID+6,4);
        addGFXObject(machine_color,179,SHOWWID+14,4);
    case(7):
        addGFXString(text_color,"       ",SHOWWID+7,5);
        addGFXObject(machine_color,179,SHOWWID+6,5);
        addGFXObject(machine_color,179,SHOWWID+14,5);
    case(6):
        addGFXString(text_color,"       ",SHOWWID+7,6);
        addGFXObject(machine_color,179,SHOWWID+6,6);
        addGFXObject(machine_color,179,SHOWWID+14,6);
    default:
        break;
    }
}

void diamond::drawMachine()
{
    drawDiamondFrame();
    drawDiamondLights();
    drawDiamondText();
    drawDiamondPayouts();
    drawReelLights();
    drawArrows(true);
}

void diamond::drawReelLights()
{
    uint displacement_iter = 0;
    uint x_coord;
    for (uint i = current_reel_position; i < current_reel_position + NUM_VISIBLE_DIAMOND_SLOTS; ++i)
    {
        x_coord = SHOWWID+7+displacement_iter;

        if (!is_playing)
            addGFXObject(arrow_color,247,x_coord,13);
        else
        {
            if (reel[i % reel.size()] == DIAMOND_REEL_BONUS_TILE_VALUE)
                addGFXObject(colorpair_white_black,diamond_bonus_spin_tile[diamond_bonus_spin_amount[danger_level - 1] - 1],x_coord,13);
            else
                addGFXObject(diamond_colors[reel[i % reel.size()]],4,x_coord,13);
        }
        displacement_iter++;
    }
}

void diamond::drawDiamondText()
{
    addGFXString(title_color,"DIAMONDS!",SHOWWID+8,1);
    addGFXString(text_color,"SPIN",SHOWWID+2,12);
    addGFXString(number_color,int2String((uint64)spin_rounds_left) + "/" + int2String((uint64)max_spin_rounds),SHOWWID+2,13);
    addGFXString(text_color,"BET",SHOWWID+2,9);
    addGFXString(number_color,int2String((uint64)total_bet),SHOWWID+2,10);
    addGFXString(text_color,"WON",SHOWWID+2,3);

    STRING winnings_indicator1 = int2String(current_winnings);
    STRING winnings_indicator2 = "";
    STRING winnings_indicator3 = "";

    if (winnings_indicator1.size() > 4)
    {
        winnings_indicator2 = STRING(winnings_indicator1.begin()+4,winnings_indicator1.end());
        winnings_indicator1 = STRING(winnings_indicator1.begin(),winnings_indicator1.begin()+4);
    }

    if (winnings_indicator2.size() > 4)
    {
        winnings_indicator3 = STRING(winnings_indicator2.begin()+4,winnings_indicator2.end());
        winnings_indicator2 = STRING(winnings_indicator2.begin(),winnings_indicator2.begin()+4);
    }

    addGFXString(number_color,winnings_indicator1,SHOWWID+2,4);
    addGFXString(number_color,winnings_indicator2,SHOWWID+2,5);
    addGFXString(number_color,winnings_indicator3,SHOWWID+2,6);

    addGFXString(text_color,"CREDIT=",2+SHOWWID,15);
    addGFXString(number_color,"              ",9+SHOWWID,15);
    addGFXString(number_color,int2String((uint64)credits),9+SHOWWID,15);

    drawDiamondStateIndicator(" END GAME ",MSTATE_DIAMONDENDGAME);
    drawDiamondStateIndicator("   PLAY   ",MSTATE_DIAMONDPLAY);
    drawDiamondStateIndicator("  INSERT  ",MSTATE_DIAMONDINSERT);
    drawDiamondStateIndicator(" CASH OUT ",MSTATE_DIAMONDCASHOUT);
    drawDiamondStateIndicator(" NEXT BET ",MSTATE_DIAMONDBET);
    drawDiamondStateIndicator("  UNUSED  ",MSTATE_DIAMONDHACK);
}

void diamond::drawDiamondStateIndicator(STRING txt, machine_state s)
{
    color_pair select_color = {text_color.fg,color_verydarkgray};

    point loc = point(2+(((int)((int)(s-8)/3))*11)+SHOWWID,17+(int)((s-8)%3));

    if (s != current_state)
        addGFXString(text_color,txt,loc.x(),loc.y());
    else
        addGFXString(select_color,txt,loc.x(),loc.y());
}

uint64 diamond::useMachine(uint64 inp_credits)
{
    uint64 out_cred = 0;

    switch(current_state)
    {
    case(MSTATE_DIAMONDPLAY):
        playRoundOrBet();
        break;
    case(MSTATE_DIAMONDENDGAME):
        if (is_playing)
            resetDiamondMachine();
        break;
    case(MSTATE_DIAMONDBET):
        if (!is_playing)
        {
            setBet(bet_iter + 1);
            drawDiamondText();
        }
        break;
    case(MSTATE_DIAMONDCASHOUT):
        if (!is_playing)
        {
            out_cred = credits;
            credits = 0;
        }
        break;
    case(MSTATE_DIAMONDINSERT):
        credits += inp_credits;
        break;
    default:
        break;
    }

    return out_cred;
}

void diamond::playRoundOrBet()
{
    if (!is_playing)
    {
        if (credits >= total_bet)
        {
            is_playing = true;
            credits -= total_bet;
        }
    }
    else if (spin_rounds_left > 0)
    {
        playRound();
    }
}

void diamond::playRound()
{
    spin_rounds_left--;
    spinReel();
    updateLights();
    checkBonus();
    checkWinnings();
    drawDiamondText();
}

void diamond::resetDiamondMachine()
{
    for (uint i = 0; i < diamond_columns; ++i)
    {
        num_diamond_lights[i] = 0;
        for (uint j = 0; j < diamond_rows; ++j)
            diamond_lights[i][j] = false;
    }
    credits += current_winnings;
    current_winnings = 0;
    bonus_round = false;
    is_playing = false;
    spin_rounds_left = max_spin_rounds = diamond_rows;
}

void diamond::drawDiamondPayouts()
{
    uint64 ith_payout = 1;

    STRING payout_string;

    STRING payout_string_modifier;

    STRING payout_index1_string_modifier = "x0.";

    payout_index1_string_modifier += int2String(payouts[1]);

    addGFXString(text_color,"WON 0",SHOWWID+7,11);
    addGFXString(text_color,payout_index1_string_modifier,SHOWWID+7,10);

    for (uint i = 5; i < diamond_rows; ++i)
    {
        addGFXString(text_color,"       ",SHOWWID+7,11-i);
    }

    for (uint i = 2; i < payouts.size(); ++i)
    {
        payout_string = "x";
        payout_string_modifier = "";

        ith_payout = (uint64)(payouts[i] / 10);

        if (ith_payout % 1000000 == 0)
        {
            payout_string_modifier = "M";
            ith_payout /= 1000000;
        }

        else if (ith_payout % 1000 == 0)
        {
            payout_string_modifier = "K";
            ith_payout /= 1000;
        }

        payout_string += int2String(ith_payout);
        payout_string += payout_string_modifier;

        addGFXString(text_color,payout_string,SHOWWID+7,11-i);
    }
}

void diamond::shiftReelNTimes(uint n)
{
    current_reel_position += n;

    if (current_reel_position >= reel.size())
        current_reel_position %= reel.size();
}

void diamond::spinReel()
{
    uint n = ROLL(100) + 100;

    drawArrows(false);
    const uint max_n = ROLL(10) + 25;

    Uint32 speed = 1;

    while(1)
    {
        delayFunc(speed);
        shiftReelNTimes(1);
        drawReelLights();
        updateFunc();
        n--;
        if (n < max_n)
            speed+=5;
        if (n == 0)
            break;
    }
}

void diamond::checkBonus()
{
    bool enter_bonus_round = true;

    for (uint i = 0; i < diamond_columns; ++i)
    {
        if (num_diamond_lights[i] != 1)
        {
            enter_bonus_round = false;
            break;
        }
    }

    if (enter_bonus_round)
    {
        bonus_round = true;
        spin_rounds_left = max_spin_rounds;
    }
}

void diamond::updateLights()
{
    uint n = 0;

    uint light_selected = reel[current_reel_position];

    if (light_selected == DIAMOND_REEL_BONUS_TILE_VALUE)
        spin_rounds_left += (diamond_bonus_spin_amount[danger_level - 1] + 1);
    else
    {
        while(1)
        {
            // should never happen
            if (n == diamond_rows)
            {
                std::cout << "DEBUG: fault -> casino machine";
                break;
            }
            if (!diamond_lights[light_selected][n])
            {
                diamond_lights[light_selected][n] = true;
                num_diamond_lights[light_selected]++;
                break;
            }
            n++;
        }
    }

    drawDiamondLights();
}

void diamond::checkWinnings()
{
    uint64 winnings = 0ULL;

    for (uint i = 0; i < diamond_columns; ++i)
    {
        if (num_diamond_lights[i] == diamond_rows)
            spin_rounds_left = 0;
        if (!bonus_round && num_diamond_lights[i] == 1)
        {
            winnings = 0ULL;
            break;
        }
        else if (num_diamond_lights[i] == 2)
            winnings += (uint64)(((double)payouts[1]/10.0) * (double)total_bet);
        else if (num_diamond_lights[i] > 2)
            winnings += (uint64)((payouts[num_diamond_lights[i]-1]/10) * total_bet);
    }

    current_winnings = winnings;
}

void diamond::setBet(uint b)
{
    bet_iter = b <= 2 ? b : 0;
    total_bet = bet_values[bet_iter];
}

void diamond::drawArrows(bool lit)
{
    color_pair colp = (lit == true ? arrow_color : colorpair_verydarkgray_black);
    addGFXObject(colp,31,7+SHOWWID,12);
    addGFXObject(colp,30,7+SHOWWID,14);
}
