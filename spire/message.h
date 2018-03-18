#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "graphics.h"

#define NUMMESSAGELINES 5
#define NUM_QUOTES_PER_NPC 4

static const STRING NPC_chat_quotes[NUM_TOTAL_BEINGS - 2][NUM_QUOTES_PER_NPC] =
{
    {
        "#straightens his suit.",
        "#checks his watch.",
        "#glares at you.",
        "#looks around impatiently."
    },
    {
        "Psst. You. Come here.",
        "Don't tell anyone I'm here.",
        "Want a big gun?",
        "Traveler, I've got what your looking for."
    },
    {
        "#grunts angrily.",
        "Get him!",
        "Stop!",
        "Halt!"
    },
    {
        "#stares into the distance.",
        "#looks ready to kill someone or something.",
        "#seems to be controlled remotely.",
        "#seems to be controlled remotely."
    },
    {
        "#twitches its antennae.",
        "#makes a clicking sound.",
        "#hops around.",
        "Are you food?"
    },
    {
        "#is giving off a deranged look.",
        "#seems to be going insane.",
        "MORE AMMO!!! hahahaha",
        "HAHAHAHAHAHAHA!"
    },
    {
        "You'll never get me alive!",
        "You'll never get me alive!",
        "You'll never get me alive!",
        "You'll never get me alive!"
    },
    {
        "#flickers.",
        "#glows.",
        "#fizzles.",
        "#whispers inaudibly."
    },
    {
        "Come closer, wuss.",
        "Launcher is for real men!",
        "God you suck at using that weapon.",
        "Using the shotgun is easy, see?"
    },
    {
        "#bats her eyelashes.",
        "#waves her multi-hued hair.",
        "#sings.",
        "#sings."
    },
    {
        "Sector 7-1-H-2-G not clear *static*",
        "Sector Y-K-0-4-5 not clear *static*",
        "Contaminated lifeform detected! *static*",
        "#gives off a muffled, static noise."
    },
    {
        "DIE!",
        "DIE!",
        "It will be over with soon...",
        "It will be over with soon..."
    },
    {
        "Time to clean this place up!",
        "Time to clean this place up!",
        "Destroy everything, no survivors *static*",
        "Destroy everything, no survivors *static*"
    },
    {
        "#roars!",
        "#Wanders around aimlessly.",
        "*THUMP!* - *THUMP!*",
        "ROOOOOOOAAAAR!"
    },
    {
        "Die, ephemeral creature!",
        "Die, ephemeral creature!",
        "Die, ephemeral creature!",
        "Die, ephemeral creature!"
    },
    {
        "Die, insignificant mortal!",
        "Die, insignificant mortal!",
        "Die, insignificant mortal!",
        "Die, insignificant mortal!"
    },
    {
        "Die, irritating pest!",
        "Die, irritating pest!",
        "Die, irritating pest!",
        "Die, irritating pest!"
    },
    {
        "#changes color.",
        "#laughs uncontrollably.",
        "#says something incomprehensible.",
        "See this weapon? Yeah, magic."
    },
    {
        "#brightens its red, metallic eye.",
        "#makes a low-pitched beeping sound.",
        "#makes a low-pitched humming sound.",
        "D-E-S-T-R-O-Y"
    },
    {
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication."
    },
    {
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication."
    },
    {   "#gives you a threatening stare.",
        "#gives you a threatening smirk.",
        "#twirls an ancient flail.",
        "#drags an ancient flail."
    },
    {
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication.",
        "#is not responsive to your form of communication."
    },
    {
        "#seems to communicating long-distance with someone or something.",
        "You don't want to go up there, trust me.",
        "Can't afford to let anyone else up there, sorry...",
        "The express elevator is closed!"
    },
    {
        "#completely ignores your attempt to communicate.",
        "#completely ignores your attempt to communicate.",
        "#completely ignores your attempt to communicate.",
        "#completely ignores your attempt to communicate."
    },
    {
        "#adjusts his remote communications headset.",
        "#says \"Yeah, I got him.\" into a remote headset.",
        "Well done. You won't get any further, unfortunately.",
        "Time to clean this place up!"
    }
};

class msgbuffer
{
public:
    msgbuffer();
    bool addMessage(STRING m, color_pair c);
    void printMessages();
    void clearMessages();
    void deleteAllMessages();
    void clearAndDeleteAllMessages();
    int getMessageSize();
private:
    STRING temp_message_string;
    STRING message_string;
    STRING last_message;
    int message_cursor;
    int message_repeat_counter;
    color_pair message_color_data[NUMMESSAGELINES*GRIDWID];
};

struct npcdamageindicator
{
    STRING dam;
    item_t w;
    chtype asym;
    chtype tsym;
    bool fatal;
    bool explode;
    bool see_attacker;
    bool was_melee;
    int crit;
};

class npcdamagebuffer
{
public:
    npcdamagebuffer();
    // m -> NPC_t damages NPC_t with item_t
    bool addMessage(STRING,item_t,chtype,chtype,bool,bool,bool,bool,int);
    void printMessages();
    void deleteTopMessage();
    void clearAndDeleteAllMessages();
private:
    VECTOR <npcdamageindicator> buffer;

};

#endif
